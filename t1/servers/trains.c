#include <bwio.h>
#include <ts7200.h>
#include <debug.h>
#include "servers.h"
#include "track.h"
#include "../user/usyscall.h"
#include "../ktests/tests.h"  // PANIC
#include "../kernel/switch.h" // FOREVER, NULL
#include <ANSI.h>

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
       if (x    >= 1 && x    <= 16) { ans.group = 'A'; ans.id = x;    }  
  else if (x-16 >= 1 && x-16 <= 16) { ans.group = 'B'; ans.id = x-16; }
  else if (x-32 >= 1 && x-32 <= 16) { ans.group = 'C'; ans.id = x-32; }
  else if (x-48 >= 1 && x-48 <= 16) { ans.group = 'D'; ans.id = x-48; }
  else if (x-64 >= 1 && x-64 <= 16) { ans.group = 'E'; ans.id = x-64; }
//  if (ans.group == 'C' && ans.id == 3) { Putc (COM1, 0x0f); Putc(COM1, 0x34); }
  return ans;
}

void zeromsg (struct msg *f) {
  f->id=f->c1=f->c2=f->c3=0;
  f->d1=f->d2=0;
}

void sensor_secretary () {
  int sensor[80];
  struct msg in;
  struct msg out;
  int r, i, tid, t;
  RegisterAs ("sens");
  for (i = 0; i < 80; i++) sensor[i] = 0;

  while (1) {
    zeromsg (&in);
    zeromsg (&out);
    r = Receive (&tid, (char*)(&in), sizeof(struct msg));
    switch (in.id) {
      case 'D':
        sensor[in.d1] = in.d2;
        Reply (tid, NULL, 0);
        break;
      case 'R': // train is requesting status of sensor
        if (sensor[in.d1]) {
          out.d1 = sensor[in.d1];
          sensor[in.d1] = 0;
          Reply (tid, (char*)(&out), sizeof (struct msg));
        }
        else { 
          Reply (tid, NULL, 0);
        }
        break;
     case 'L': // i am lost!
       t = Time();
       for (i = 0; i < 80; i++) {
         if (sensor[i] && t-sensor[i] > 60 ) {
           sensor[i] = 0;
         }
         else if (sensor[i] && t-sensor[i] > 20) {
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

#define LOST_TIMEOUT 10000000

int nextsensor (int cur, int trktid) {
  struct trip t;
  struct msg out;

  zeromsg(&out);
  out.id = 'n';
  out.d1 = cur - 1;

  // poll for our expectednext
  // if nothing, nothing. otherwise update shit
  // update velocity shit?
  if (Send(trktid, (char *)&out, sizeof(struct msg), (char *)&t,
           sizeof(struct trip)) != sizeof(struct trip)) PANIC;
  return t.destination + 1;
}

void train_agent () {
  struct msg out,in;
  char msg = 'U';
  int trid = MyParentTid ();
  int senid = WhoIs ("sens");
  int trktid = WhoIs ("trak");
  int lastsensor = -1;
  int newspeed; int speed;
  int timelastsensor  = 0;
  int expectednext;
  int lost = 1; // start off lost
  int dx;
  int r,t;

  unsigned int updatemsg[3];
  updatemsg[0] = (unsigned int)'U';

  while(1) {
    t = Time();
    // measure 508khz clock, update timesincelastsensor
    updatemsg[1] = lastsensor;
    updatemsg[2] = dx;

    // ask train server for updates on speed, update it with our positions.
    Send (trid, (char*)(updatemsg), 3*sizeof(int), (char*)(&newspeed), 4);
    if (newspeed != speed) {
      speed = newspeed;
      // time since speed change for blending?
    }

    if (t-timelastsensor > LOST_TIMEOUT) {
      lost = 1;
      lastsensor = -1;
    }
    if (lost) { // we are lost  (be a bit more smart about this?)
      zeromsg(&out);
      zeromsg(&in);
      out.id = 'L';
      r = Send (senid, (char*)(&out), sizeof(struct msg), (char*)(&in), sizeof(struct msg));
      if (r) {
        lost = 0;
        timelastsensor = in.d2;
        lastsensor = in.d1;
        expectednext = nextsensor(lastsensor, trktid);
        bwprintf(COM2, "I am at %d now, after that: %d\r\n", lastsensor - 1,
                  expectednext);
      }
    }
    else {
      zeromsg(&out);
      zeromsg(&in);
      out.id = 'R';
      out.d1 = lastsensor;
      r = Send (senid, (char*)&out, sizeof (struct msg), (char*)&in, sizeof (struct msg));
      if (r) { // calibrate velocity more????
        timelastsensor = in.d1;
        lastsensor = expectednext;
        expectednext = nextsensor(lastsensor, trktid);
      }
    }
 
    dx += 0; // calculate distance past current sensor
  }
}


void trains () {
  int tid;
  char cmd[32];
  char lastread[10];

  int speeds[100];
  int tr2tid[100];
  int locations[100];
  int tid2tr[100];

  char sw[32];
  //int head = 0;
  int i;

  struct msg out;
  unsigned char train_dict[256];
  struct sensorevent latest;
  latest.group = 0;
  for (i = 0; i < 100; i++) { speeds[i] = tr2tid[i] = 0; tid2tr[i] = 0; locations[i] = -2; }
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
        Reply (tid, cmd, 1);
        goto start;
        break;
      case 'U': // update from a train
        locations[tid2tr[tid]] = ((int*)cmd)[1];
        Reply (tid, (char*)(&speeds[tid2tr[tid]]), 4);
        goto start;
        break;
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
             char wut = (cmd[i+1]^lastread[i]) & cmd[i+1];
             lastread[i] = cmd[i+1];
             int j = 1;
             while (wut) {
               if (wut &128) { 
                 latest = fucksensor (i*8+j,Time()); 
                 out.id = 'D';
                 out.d1 = i*8+j;
                 out.d2 = Time(); // change me, 508khz reading?
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
