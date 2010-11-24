#include <bwio.h>
#include <ts7200.h>
#include <debug.h>
#include "servers.h"
#include "track.h"
#include "../user/usyscall.h"
#include "../user/lib.h"
#include "../ktests/tests.h"  // PANIC
#include "../kernel/switch.h" // FOREVER, NULL
#include "../kernel/boot.h"   // TIMER3*          HARDWARE HAX
#include <ANSI.h>

typedef unsigned int uint;

void sensorserver () {
  int last = Time ();
  int cur;
  char msg[11]; msg[0] = 'p';
  int i = 1;
  RegisterAs ("sen");
  int trid = WhoIs ("tr"); 
  int c;
  Send (trid, msg, 1, NULL, 0);

  FOREVER {
    cur = Time ();
    if (cur - last > 50) {
       //DPRINTERR ("timeout cur = %d, last = %d??\n",cur,last);
       Delay (1);
       Send (trid, msg, 1, NULL, 0);
       i = 1;
       last = cur;
    }
    else {
      c = Getc_r(COM1);
      if (c != -1) {
        last = cur;
        // DPRINTOK ("bing c=%x i=%d\n",c,i);
        msg[i] = (char)c;
        i++;
        if (i > 10) {
          i = 1;
          Send (trid, msg, 11, NULL, 0);
          Delay (1);
          Send (trid, msg, 1,  NULL, 0);
        }
      }
    }
  }
}

int trains_switch (char id, char dir) {
  if (dir=='S' || dir == 's') {
    Putc(COM1,33);
    Putc(COM1,id);
  }
  else if (dir=='C' || dir == 'c') {
    Putc(COM1, 34);
    Putc(COM1, id);
  }
  else { return 1; }
  Putc (COM1, 32);
  return 0;
}

int fuckswitch (int x) {
  if (x < 18) return x+1;
  else return x+153-18;
}

int unfuckswitch (int x) {
  if (x <19) return x-1;
  else return x-153+18;
}

struct sensorevent {
  char group;
  int id;
  int time;
};

struct sensorevent fucksensor (int x, int time) {
  struct sensorevent ans;
  ans.time = time;
       if (x    >= 0 && x    <= 15) { ans.group = 'A'; ans.id = x+1;    }  
  else if (x-16 >= 0 && x-16 <= 15) { ans.group = 'B'; ans.id = x-15; }
  else if (x-32 >= 0 && x-32 <= 15) { ans.group = 'C'; ans.id = x-31; }
  else if (x-48 >= 0 && x-48 <= 15) { ans.group = 'D'; ans.id = x-47; }
  else if (x-64 >= 0 && x-64 <= 15) { ans.group = 'E'; ans.id = x-63; }
//  if (ans.group == 'C' && ans.id == 3) { Putc (COM1, 0x0f); Putc(COM1, 0x34); }
  return ans;
}

void zeromsg (struct msg *f) {
  f->id=f->c1=f->c2=f->c3=0;
  f->d1=f->d2=0;
}

/*
 * PROTOCOL
 *  D - receive new time of sensor data
 *  L - we are now lost
 *  R - train requesting status of sensor
 */
void sensor_secretary () {
  int sensor[80]; // Timings, counting down from 0xFFFFFFFF at 508KHz on TIMER3
  char name[10];
  struct msg in;
  struct msg out;
  int r, i, tid, t;
  RegisterAs ("sens");
  for (i = 0; i < 80; i++) sensor[i] = 0;

  while (1) {
    zeromsg (&in);
    zeromsg (&out);
    t = READ_TIMER3;
    r = Receive (&tid, (char*)(&in), sizeof(struct msg));
    switch (in.id) {
      case 'D':
        //sens_id_to_name(in.d1, name);
        //bwprintf (COM2, "was told that sensor %s was triggered at %d.\n", name,
        //in.d2);
        sensor[in.d1] = in.d2;
        Reply (tid, NULL, 0);
        break;
      case 'R': // train is requesting status of sensor
        if (sensor[in.d1] != 0 && sensor[in.d1] - t > 1200000) {
          //sens_id_to_name(in.d1, name);
          //bwprintf (COM2, "Expiring sensor %s, which was triggered at %d\n",
          //          name, sensor[in.d1]);
          sensor[in.d1] = 0;
          Reply(tid,NULL, 0);
        }
        else if (sensor[in.d1]) {
          //bwprintf (COM2, "success???\n");
          out.d1 = sensor[in.d1];
          sensor[in.d1] = 0;
          Reply (tid, (char*)(&out), sizeof (struct msg));
        }
        else { 
          Reply (tid, NULL, 0);
        }
        break;
     case 'L': // i am lost!
       for (i = 0; i < 80; i++) {
         if (sensor[i] && sensor[i] - t > 1200000 ) {
           sensor[i] = 0;
         }
         else if (sensor[i] && sensor[i] - t > 400000) {
           out.d1 = i;
           out.d2 = sensor[i];
           sensor[i] = 0;
           Reply (tid, (char*)(&out), sizeof (struct msg));
           goto fart;
         }
       }
       Reply(tid, NULL, 0);
fart: 
       break;
    }
  }
}

#define LOST_TIMEOUT 0xFFFFFFFF

int nextsensor (int cur, int trktid, int *d) {
  struct trip t;
  struct msg out;

  zeromsg(&out);
  out.id = 'n';
  out.d1 = cur;

  // poll for our expectednext
  // if nothing, nothing. otherwise update shit
  // update velocity shit?
  if (Send(trktid, (char *)&out, sizeof(struct msg), (char *)&t,
           sizeof(struct trip)) != sizeof(struct trip)) PANIC;
  *d = t.distance;
  return t.destination;
}

void train_agent () {
  struct msg out,in;
  int trid = MyParentTid ();
  int senid = WhoIs ("sens");
  int trktid = WhoIs ("trak");
  int lastsensor = -1;
  int newspeed;
  int virtspeed; // as calibrated/guesstimated
  int realspeed; // Calculated in this function in mm/sec
  int timelastsensor = 0; // in ticks, descending from 0xFFFFFFFF
  int expectednext = -1;
  int lost = 1; // start off lost
  int dx = 0; // in mm
  int sensdistance = 0; // in mm
  int r = 0;
  int t = 0; // in 508KHz ticks aka ~1.97us
  int avg_val = 0, avg_cnt = 0;

  unsigned int updatemsg[3];
  updatemsg[0] = (unsigned int)'U';

  while(1) {
    t = READ_TIMER3;
    updatemsg[1] = lastsensor;
    updatemsg[2] = dx;

    // ask train server for updates on speed, update it with our positions.
    Send (trid, (char*)(updatemsg), 3*sizeof(int), (char*)(&newspeed), 4);
    if (newspeed != virtspeed) {
      virtspeed = newspeed;
      // time since speed change for blending?
    }

    if (timelastsensor - t > LOST_TIMEOUT) {
      //bwprintf (COM2, "Now i am lost...\n");
      lost = 1;
      lastsensor = -1;
    }
    if (lost) { // we are lost  (be a bit more smart about this?)
      zeromsg(&out);
      zeromsg(&in);
      out.id = 'L';
      r = Send (senid, (char*)(&out), sizeof(struct msg), (char*)(&in), sizeof(struct msg));
      if (r) {
        char name[4];
        lost = 0;
        timelastsensor = in.d2;
        lastsensor = in.d1;
        expectednext = nextsensor(lastsensor, trktid, &sensdistance);
        sens_id_to_name(lastsensor, name);
        //bwprintf(COM2, "Found! I am at %s now, after that: %d\r\n", name,
        //          expectednext);
      }
    }
    else {
      char msg2[4];
      char nam2[4];
      zeromsg(&out);
      zeromsg(&in);
      out.id = 'R';
      out.d1 = expectednext;
      r = Send (senid, (char*)&out, sizeof (struct msg), (char*)&in, sizeof (struct msg));
      if (r) { // We've hit a fresh sensor, calibrate velocity more????
        int delta_t = timelastsensor - t; // in 1.97us
        // Scale the integers, to get mm/sec
        sensdistance *= 100000;
        delta_t      /=   1000;
        realspeed = (sensdistance / delta_t) / 197; // the ratio of ticks to time
        avg_val = (avg_val * avg_cnt + realspeed) / (avg_cnt + 1);
        ++avg_cnt;

        sens_id_to_name(lastsensor, nam2);
        sens_id_to_name(expectednext, msg2);
        bwprintf (COM2, "Got from %s to %s\tdistance %dmm, dt"
                  " %d,"
                  " v %dmm/s\tAverage %dmm/s DIST so far %d\r\n",
                  nam2, msg2, sensdistance, delta_t, realspeed, avg_val, dx);

        dx = 0;
        timelastsensor = in.d1;
        lastsensor = expectednext;
        expectednext = nextsensor(lastsensor, trktid, &sensdistance);
        sens_id_to_name(expectednext, nam2);
        //bwprintf (COM2, "should now get to %s.\n", nam2);
      }
    }
 
    dx = realspeed * timelastsensor; // calculate distance past current sensor (in mm) FIXME
  }
}

void trains () {
  int tid;
  char cmd[32];
  char lastread[10];

  int speeds[80];
  int tr2tid[80];
  int locations[80];
  int dx[80];
  int tid2tr[80];

  char sw[32];
  //int head = 0;
  int i;

  struct msg out;
  struct sensorevent latest;
  struct sensorevent trap;
  trap.group   = '\0';
  trap.id      = 0;
  latest.group = 0;

  START_TIMER3();

  for (i = 0; i < 80; i++) { dx[i] = speeds[i] = tr2tid[i] = 0; tid2tr[i] = 0; locations[i] = -2; }
  for (i = 0; i < 10; i++)  { lastread[i] = 0; }

  int r;
  RegisterAs ("tr");
  Create (USER_HIGH, sensorserver);
  int sens = Create (USER_HIGH, sensor_secretary);

  FOREVER {
start:
    r = Receive (&tid, cmd, 32);
    zeromsg (&out);
    // "special" ones
    switch (cmd[0]) {
      case 'v': // st
        Reply (tid, &sw[unfuckswitch(cmd[1])], 1);
        goto start;
        break;
      case 'd':
        Reply (tid, (char*)(&latest), sizeof (struct sensorevent));
        goto start;
        break;
      case 'P': // give back location of train
        cmd[0] = (char)locations[(int)cmd[1]];
        out.d1 = locations[(int)cmd[1]];
        out.d2 = dx[(int)cmd[1]];
        Reply (tid, (char*)(&out), sizeof (struct msg));
        goto start;
        break;
      case 'U': // update from a train
        locations[tid2tr[tid]] = ((int*)cmd)[1];
        dx[tid2tr[tid]] = ((int*)cmd)[1];
        Reply (tid, (char*)(&speeds[tid2tr[tid]]), 4);
        goto start;
        break;
      case 'T': //set a trap
        trap.group = cmd[5];
        int id;
        if (cmd[7] == ' ') {
          id = cmd[6] - '0';
        }
        else {
          id = 10 + cmd[7] - '0';
        }
        trap.id = id;
        bwprintf(COM2, "Trapping at %c%d\r\n", trap.group, trap.id);
      default:
        break;
    }

    Reply (tid, NULL, 0);
    switch (cmd[0]) {
      case 'A': // add train
        tr2tid[(int)cmd[1]] =  Create (USER_HIGH, train_agent);
        tid2tr[tr2tid[(int)cmd[1]]] = (int)cmd[1];
        break;
      case 'p': // poll sensors
        switch (r) {
          case 1:   /*bwprintf (COM2, "pinging\n");*/Putc (COM1, 133); break;
          case 11: 
           for (i = 0; i < 10; i++) {
             int tt = READ_TIMER3;
             char wut = (cmd[i+1]^lastread[i]) & cmd[i+1];
             lastread[i] = cmd[i+1];
             int j = 0;
             while (wut) {
               if (wut &128) { 
                 latest = fucksensor (i*8+j,tt); 
                 if (trap.id && trap.id == latest.id && trap.group ==
                     latest.group) {
                   Putc(COM1, 0x00);
                   Putc(COM1, 0x20); //stop train 32
                 }
                 out.id = 'D';
                 out.d1 = i*8+j;
                 out.d2 = latest.time;
                 Send (sens, (char*)(&out), sizeof (struct msg), NULL, 0);
               }
               wut <<= 1;
               j++;
             }
           }
           break;
          default: PANIC;
        }
        break;
      case 'g': // go
        Putc (COM1, 0x60);
        break;
      case 's': // stop
        Putc (COM1, 0x61);
        break;
      case 't': // tr 
        Putc (COM1, cmd[1]);
        Putc (COM1, cmd[2]);
        speeds[(int)cmd[2]] = cmd[1];
        break;
      case 'r': // reverse
        Putc (COM1, 0);
        Putc (COM1, cmd[1]);
        Delay (100);
        Putc (COM1, 15);
        Putc (COM1, cmd[1]);
        Putc (COM1, speeds[(int)cmd[1]]);
        Putc (COM1, cmd[1]);
        break;
      case 'w': // sw
        trains_switch(cmd[1],cmd[2]); 
        sw[unfuckswitch(cmd[1])] = cmd[2];  
        break;
      case 'a': // switch all
        for (i = 0; i < 22; i++) {
          trains_switch(fuckswitch(i), cmd[1]);
          sw[i] = cmd[1];
        }
        break;
    }
  }
}
