#include <bwio.h>
#include <ts7200.h>
#include <debug.h>
#include <lock.h>
#include "servers.h"
#include "track.h"
#include "../user/usyscall.h"
#include "../user/lib.h"
#include "../ktests/tests.h"  // PANIC
#include "../kernel/switch.h" // FOREVER, NULL
#include "../kernel/boot.h"   // TIMER3*          HARDWARE HAX
#include <ANSI.h>
#include <lock.h>

int abs(int a) { if (a>0) return a; else return -a; }
typedef unsigned int uint;
struct sensorevent {
  char group;
  int id;
  int time;
};

#define MAX_TRAINS 6
#define HEIGHT 8
#define WIDTH 24

struct pos {
  int x, y;
};

struct pos sensorlocation (int sen) {
  struct pos p;
  switch (sen) {
    case 0: case 1:   p.x = 4; p.y = 1; return p; // A1
    case 2: case 3:   p.x = 7; p.y = 4; return p;// A3
    case 4: case 5:   p.x = 5; p.y = 8; return p;// A5
    case 6: case 7:   p.x = 4; p.y = 7; return p;// A7
    case 8: case 9:   p.x = 3; p.y = 6; return p;// A9
    case 10: case 11: p.x = 4; p.y = 5; return p; // A11
    case 12: case 13: p.x = 3; p.y = 2; return p; // A13
    case 14: case 15: p.x = 4; p.y = 3; return p; // A15
    case 16: case 17: p.x = 14; p.y = 6; return p;// B1
    case 18: case 19: p.x = 13; p.y = 5; return p;// B3
    case 20: case 21: p.x = 14; p.y = 2; return p;// B5
    case 22: case 23: p.x = 1; p.y = 6; return p;// B7
    case 24: case 25: p.x = 1; p.y = 8; return p;// B9
    case 26: case 27: p.x = 1; p.y = 7; return p; // B11
    case 28: case 29: p.x = 15; p.y = 4; return p;// B13
    case 30: case 31: p.x = 7; p.y = 4; return p;// B15
    case 32: case 33: p.x = 15; p.y = 4; return p;// C1
    case 34: case 35: p.x = 19; p.y = 8; return p;// C3
    case 36: case 37: p.x = 9; p.y = 7; return p;// C5
    case 38: case 39: p.x = 11; p.y = 8; return p;// C7
    case 40: case 41: p.x = 9; p.y = 6; return p;// C9
    case 42: case 43: p.x = 9; p.y = 2; return p; // C11
    case 44: case 45: p.x = 8; p.y = 1; return p; // C13
    case 46: case 47: p.x = 14; p.y = 7; return p;// C15
    case 48: case 49: p.x = 15; p.y = 4; return p;// E1
    case 50: case 51: p.x = 17; p.y = 3; return p;// E3
    case 52: case 53: p.x = 18; p.y = 2; return p;// E5
    case 54: case 55: p.x = 21; p.y = 1; return p; // E7
    case 56: case 57: p.x = 22; p.y = 6; return p;// E9
    case 58: case 59: p.x = 21; p.y = 7; return p;// E11
    case 60: case 61: p.x = 18; p.y = 6; return p;// E13
    case 62: case 63: p.x = 13; p.y = 3; return p;// E15
    case 64: case 65: p.x = 15; p.y = 4; return p;// D1
    case 66: case 67: p.x = 16; p.y = 2; return p;// D3
    case 68: case 69: p.x = 22; p.y = 2; return p;// D5
    case 70: case 71: p.x = 23; p.y = 2; return p; // D7
    case 72: case 73: p.x = 23; p.y = 6; return p;// D9
    case 74: case 75: p.x = 16; p.y = 7; return p;// D11
    case 76: case 77: p.x = 16; p.y = 6; return p;// D13
    case 78: case 79: p.x = 17; p.y = 5; return p;// D15
  }
  return p;
}


struct pos switchlocation (int sw) {
  struct pos p;
  switch (sw) {
    case 0: p.x = 4; p.y = 6; return p; // SW1
    case 1: p.x = 5; p.y = 7; return p;// SW2
    case 2: p.x = 6; p.y = 8; return p;// SW3
    case 3: p.x = 4; p.y = 2; return p; // SW4
    case 4: p.x = 17; p.y = 8; return p;// SW5
    case 5: p.x = 12; p.y = 7; return p;// SW6
    case 6: p.x = 18; p.y = 7; return p;// SW7
    case 7: p.x = 23; p.y = 5; return p;// SW8
    case 8: p.x = 23; p.y = 3; return p;// SW9
    case 9: p.x = 17; p.y = 2; return p;// SW10
    case 10:p.x = 7; p.y = 1; return p; // SW11
    case 11:p.x = 5; p.y = 1; return p; // SW12
    case 12:p.x = 13; p.y = 2; return p;// SW13
    case 13:p.x = 7; p.y = 3; return p; // SW14
    case 14:p.x = 7; p.y = 5; return p;// SW15
    case 15:p.x = 13; p.y = 6; return p;// SW16
    case 16:p.x = 17; p.y = 6; return p;// SW17
    case 17:p.x = 13; p.y = 8; return p;// SW18
    case 18:p.x = 14; p.y = 5; return p;// SW99
    case 19:p.x = 16; p.y = 5; return p;// SW9A
    case 20:p.x = 16; p.y = 3; return p;// SW9B
    case 21:p.x = 14; p.y = 3; return p;// SW9C
  }
  return p;
}

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

void drawtrack (char *trk) { 
  LockAcquire(COM2_W_LOCK);
  CURSORPUSH();
  SETCOLOUR(BG+BRIGHT+BLACK);
  SETCOLOUR(FG+BRIGHT+WHITE);
  CURSORMOVE(2,1);
  int x,y; 
  for (y = 0; y < HEIGHT; y++) { 
    bwprintf (COM2," [K[%d;1H  ", y+3); 
    for (x = 0; x < WIDTH; x++) { 
      bwprintf (COM2,"%c", trk[x+y*WIDTH]); 
    } 
  }
  bwprintf (COM2, "[K\n");
  CLEARTOEND();
  SETCOLOUR(BG+BLACK);
  SETCOLOUR(FG+WHITE);
  CURSORPOP();
  LockRelease (COM2_W_LOCK);
}

char charatlocation(struct pos p, char *trk) {
  return trk[(p.x-1)+(p.y-1)*WIDTH];
}

void putpixel (struct pos p, int colour, char *trk) {
  CURSORPUSH();
  bwprintf (COM2, "[%d;%dH[%dm%c[m", p.y+2, p.x+2, colour, charatlocation(p,trk));
  CURSORPOP();
}

void drawswitches (char *trk, int c) {
  LockAcquire (COM2_W_LOCK);
  int i;
  for (i=0; i< 22; i++) {
    putpixel (switchlocation(i),c,trk);
  }
  LockRelease (COM2_W_LOCK);
}
void drawlegend (int *locations, uint *dx, int *legend, char *trk,uint *velocity) {
  LockAcquire (COM2_W_LOCK);
  CURSORPUSH();
  CURSORMOVE(3,30);
  SETCOLOUR(BG+BRIGHT+BLACK);
  bwprintf (COM2, "Train ID\t\tPosition");
  int i;
  struct sensorevent s;
//  struct msg in;
  for (i = 0; legend[i] && i < MAX_TRAINS; i++) {
    //get train pos
/* TODO port    packet[1] = (char)legend[i];
    Send (trid, packet,2, (char*)(&in), sizeof (struct msg));*/
    CURSORMOVE(4+i,30); 
    SETCOLOUR(41+i);
    bwprintf (COM2, "  ");
    SETCOLOUR(BG+BRIGHT+BLACK);
    if (locations[legend[i]] != -1) {
      s = fucksensor (locations[legend[i]],0);
      bwprintf (COM2,      " Train %d     %c%d+%d.%dcm @ %dmm/s        ", legend[i], s.group, s.id, dx[legend[i]]/10000000,(dx[legend[i]]/1000000)%10, velocity[legend[i]]);
   } else bwprintf (COM2, " Train %d     lost!                ",legend[i]);
/*   
  //  struct sensorevent s;// = fucksensor((int)c,0);
   switch (locations[legend[i]]) {
      case -1:
      case -2:
        if (locations[legend[i]] != -1) {
          CURSORPOP();
          putpixel (sensorlocation(locations[legend[i]]),BG+BRIGHT+BLACK, trk);
          CURSORPUSH();
 //         CURSORMOVE(4+i, 32);
          locations[legend[i]] = -1;
        }
 //       bwprintf (COM2, "  Train %d\t(lost!)\t", legend[i]); 
        break; 
       // bwprintf (COM2, "  I AM ERROR."); 
        break;
      default:
        if (locations[legend[i]] != -1) {
          CURSORPOP();
          putpixel (sensorlocation(locations[legend[i]]),BG+BRIGHT+BLACK, trk);
          CURSORPUSH();
  //        CURSORMOVE(4+i, 32);
          locations[i] = -1;
        }
  //      s = fucksensor(locations[legend[i]],0);
        CURSORPOP();
        putpixel (sensorlocation(locations[legend[i]]), 41+i,trk);
        CURSORPUSH();
  //      CURSORMOVE(4+i,32); 
  //      SETCOLOUR(BG+BRIGHT+BLACK);
  //      bwprintf (COM2, "  Train %d\t(%c%d+%dmm)", legend[i],s.group, s.id, dx[legend[i]]);
        break;
    }*/
  }
  SETCOLOUR(BG+BLACK);
  CURSORPOP();
  LockRelease (COM2_W_LOCK);
}

void sensorserver () {
  int last = Time ();
  int cur;
  char msg[11]; msg[0] = 'p';
  int i = 1;
  RegisterAs ("sen");
  int trid = WhoIs ("tr"); 
  int c;
  Send (trid, msg, 1, NULL, 0);
  LockAcquire (COM1_R_LOCK);
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
     //   LockAcquire (COM2_W_LOCK);
     //    DPRINTOK ("bing c=%x i=%d\n",c,i);
     //   LockRelease (COM2_W_LOCK);
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
//  char name[10];
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

int nextsensor (int cur, int trktid, unsigned int *d) {
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
  *d = (unsigned int)t.distance*1000000;
  return t.destination;
}

struct msg lostfunc (int sens, int time, int senid) {
 /* first attempt:
      poll for _any_ sensor, return time properly */

  struct msg out,in;
  out.id = 'L';
  int r = Send (senid, (char*)(&out), sizeof(struct msg), (char*)(&in), sizeof(struct msg));
  if (r) { in.id = 1; return in; }
  else   { in.id = 0; return in; }
  return out;
}
 
#define MARGIN_OF_ERROR 2
void train_agent_notsuck () {
  int trid = MyParentTid();
  int senid = WhoIs("sens");
  int trktid = WhoIs ("trak");
  int calitid = WhoIs ("cali");

  int nextsens; 
  int lastsensor = -1; // TODO: fucked or unfucked version?
  
  // micrometers
  uint dx = 0;
  uint dxuntilnextsensor = 0xFFFFFFFF;   

  // in "train speed" units (0-14)
  uint newtrainspeed;
  uint currenttrainspeed = 0; 
  
  // in micrometers/microsecond
  uint velocity = 0;
  uint newvelocity;

  // in micrometers/microseconds^2
  int acc;

  // boolean variables
  int lost = 1;
  int accelerating = 0;
  
  // in units of ticks (~1.9microseconds)
  uint t;
  uint lastticks = READ_TIMER3;
  uint timesincelost = lastticks;  
  uint timeofaccel;

  // in milliseconds
  uint dt;
 // uint sensorlag = 0;
  uint acceltime;

 
  int trainnum; // what train am i?

  struct msg in, out;
  /*
    out (train server update):
     d1 - last sensor we reached, -1 if lost
     d2 - the current speed of the train
     d3 - the displacement from last sensor

    in:
     d1 - speed the user wants us to adjust to
     c1 - 0 if no command, 1 if "go" command
     d2 - if c1 = 1, this is the sensor of our destination.
  */
  char tmp = '?';
  Send (trid, &tmp, 1, (char*)(&trainnum), sizeof(int));

  while (1) {
    t = READ_TIMER3;

    zeromsg(&out);
    zeromsg(&in);
    out.id = 'U';
    out.d1 = lastsensor;
    out.d2 = currenttrainspeed;
    out.d3 = dx;
    out.d4 = velocity;
 
    Send (trid, (char*)(&out), sizeof(struct msg), (char*)(&in), sizeof(struct msg));
    if (!accelerating) {
      if (in.d1 != currenttrainspeed) {
        accelerating = 1;
        newtrainspeed = in.d1;
 
        // get atime
        out.id = (newtrainspeed == 0) ? 'D' : 'A';
        out.d1 = trainnum; // DEFINE THIS
        out.d2 = (newtrainspeed ==0) ? currenttrainspeed : newtrainspeed;
        //Send (calitid, (char*)(&out), sizeof (struct msg), (char*)(&acceltime), sizeof(int));
        acceltime = 1000000; // 1 second
        // get new velocity
        out.id = 'S';
        out.d1 = trainnum;
        out.d2 = 14;
        int tmp;
        Send (calitid, (char*)(&out), sizeof (struct msg), (char*)(&tmp), sizeof (int));
        newvelocity = (tmp*newtrainspeed)/14;// TRUTH

        acc = (newvelocity - velocity)/acceltime;
        timeofaccel = READ_TIMER3;
        LockAcquire (COM2_W_LOCK);
 //       bwprintf (COM2, "we were told to accelerate from %d to %d at time %x ticks, it will take %d microsec so it will be done at %x ticktime.\n", currenttrainspeed, newtrainspeed,timeofaccel,acceltime, timeofaccel+(acceltime*100)/197);
   //     bwprintf (COM2, "velocity now is %d, will end with %d. atime is %d and that gives us an acc of %d\n", velocity, newvelocity,acceltime, acc);
        LockRelease (COM2_W_LOCK);
      }
    }

    dt = ((lastticks - t)*197)/100;
    lastticks = t;

    if (lost) {
      zeromsg(&in);
      in = lostfunc (lastsensor, timesincelost, senid);
      /*
        in:
        id = 0 if still lost, 1 if found
        d1 = what sensor we got assigned
        d2 = time sensor was tripped, from timer3
      */
      if (in.id) { // found
        lost = 0;
        dt = 0;
        lastsensor = in.d1;
        dx =0;// velocity*(sensorlag+(((in.d2-t)*197)/100))/1000;
        nextsens = nextsensor(lastsensor, trktid, &dxuntilnextsensor);//TODO:MAKE IT RETURN MICROSEC
      //  LockAcquire (COM2_W_LOCK);
      //  bwprintf (COM2, "found!! %d. next: %d\n", in.d1, nextsens);
     //   LockRelease (COM2_W_LOCK);
      }
    }
    
    if (accelerating) {
      if (READ_TIMER3 <= timeofaccel+(acceltime*100)/197) {
     //   LockAcquire (COM2_W_LOCK);
    //    bwprintf (COM2, "\ndone accelerating at %x (dt=%d).\n", READ_TIMER3,dt);
      //  LockRelease (COM2_W_LOCK);
        accelerating = 0;
        currenttrainspeed = newtrainspeed;
        velocity = newvelocity;
      }
      else {
//        LockAcquire (COM2_W_LOCK);
  //      bwprintf (COM2, "%d ", acceltime - ((timeofaccel-t)*100)/197);
    //    LockRelease (COM2_W_LOCK);
        velocity += acc*dt;
      }
    }
    else if (!lost) {
      // poll for sensor TODO

      zeromsg(&out);
      zeromsg(&in);
      out.id = 'R';
      out.d1 = nextsens;
      int r = Send (senid, (char*)(&out), sizeof(struct msg), (char*)(&in), sizeof(struct msg));

      if (r) {
     //   LockAcquire (COM2_W_LOCK);
      //  bwprintf (COM2, "dt = %d, dx (mm)= %d. ", dt,dx/1000000);
     //   LockRelease (COM2_W_LOCK);
     //   int off =abs ((int)dxuntilnextsensor-(int)dx)/1000000;
        dx = 0; //velocity * (((in.d1 - t)*197)/100+sensorlag);
        dt=0;
        lastsensor = nextsens;
        nextsens = nextsensor (lastsensor, trktid, &dxuntilnextsensor);
     //   LockAcquire (COM2_W_LOCK);
     //   bwprintf (COM2,"got to %d! next is %d, a distance of %d mm. velocity is %d. I was off by %d mm.\n",lastsensor, nextsens,dxuntilnextsensor/1000,velocity* 1000, off);
     //   LockRelease (COM2_W_LOCK);

        ReleaseAll();
        r = ReserveChunks(nextsens, 550); // 350mm is the stopdist at speed 8
        if (r == 0) {
          // cool
          char sname[4];
          sens_id_to_name(nextsens, sname);
          //LockAcquire (COM2_W_LOCK);
          //bwprintf(COM2, "Train %d reserved %s+\r\n", trainnum, sname);
          //LockRelease (COM2_W_LOCK);
        }
        else {
          char sname[4];
          sens_id_to_name(nextsens, sname);
          LockAcquire (COM2_W_LOCK);
          bwprintf (COM2,"XXXXXXX------->Woah, tr %d couldn't reserve %s+550mm\r\n", trainnum, sname);
          LockRelease (COM2_W_LOCK);
          char buf[3];
          buf[0] = 't'; buf[1] = 0; buf[2] = trainnum;
          Send(trid, buf, 3, NULL, 0); // stop the train if we can't reserve
        }
      }
    }
        

    dx += velocity*dt;
    if (dxuntilnextsensor*MARGIN_OF_ERROR < dx) {
  //    LockAcquire (COM2_W_LOCK);
   //   bwprintf (COM2, "%d is lost!\n", trainnum);
    //  LockRelease (COM2_W_LOCK);
      lost = 1;
      
    }
  }
}

void train_agent () {
  struct msg out,in;
  int trid = MyParentTid ();
  int senid = WhoIs ("sens");
 // int trktid = WhoIs ("trak");
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
    // TODO: throttle?

    updatemsg[1] = lastsensor;
    updatemsg[2] = dx;

    // ask train server for updates on speed, update it with our positions.
    Send (trid, (char*)(updatemsg), 3*sizeof(int), (char*)(&newspeed), 4);
    if (newspeed != virtspeed) {
      virtspeed = newspeed;
      LockAcquire(COM2_W_LOCK);
      bwprintf(COM2, "Got new speed from train server: %d\r\n", newspeed);
      LockRelease(COM2_W_LOCK);
      // time since speed change for blending?
    }
    t = READ_TIMER3;
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
       // expectednext = nextsensor(lastsensor, trktid, &sensdistance);
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
        t = READ_TIMER3;
        int delta_t = timelastsensor - t; // in 1.97us
        // Scale the integers, to get mm/sec
        sensdistance *= 100000 ; //TODO: wrong?
      // this was wrong?  delta_t ; // delta_t is approx in microsec

        realspeed = (sensdistance / delta_t) / 197; // the ratio of ticks to time
        avg_val = (avg_val * avg_cnt + realspeed) / (avg_cnt + 1);
        ++avg_cnt;

        sens_id_to_name(lastsensor, nam2);
        sens_id_to_name(expectednext, msg2);
        //LockAcquire(COM2_W_LOCK);
        //bwprintf (COM2, "Got from %s to %s\tdistance %dmm, dt"
        //          " %d,"
        //          " v %dmm/s\tAverage %dmm/s DIST so far %d\r\n",
        //          nam2, msg2, sensdistance / 100000, delta_t, realspeed, avg_val, dx);
        //LockRelease(COM2_W_LOCK);

        if (dx < (sensdistance / 100000)) {
          LockAcquire(COM2_W_LOCK);
          bwprintf(COM2, "<<<< UNDERSHOT THE SENSOR(%s) by %dmm\r\n",
                   msg2, (sensdistance / 100000) - dx);
          LockRelease(COM2_W_LOCK);
        }
        dx = 0;
        timelastsensor = in.d1;
        lastsensor = expectednext;
       // expectednext = nextsensor(lastsensor, trktid, &sensdistance);
        sens_id_to_name(expectednext, nam2);
        //bwprintf (COM2, "should now get to %s.\n", nam2);
      }
    }
 
    // calculate the distance past the current sensor in mm
    // But we don't want to use seconds to calculate this, not enough
    // significant digits. Hopefully this caculation is more accurate.
    dx = ((avg_val * 10) * (((timelastsensor - t) * 197) / 10000000)) / 100;
    if (dx > sensdistance)
    {
      LockAcquire(COM2_W_LOCK);
      bwprintf(COM2, ">>>> OVERSHOT  THE SENSOR by %dmm\r\n", dx - sensdistance);
      LockRelease(COM2_W_LOCK);
    }
    if (dx >= 0) {
      LockAcquire (COM2_W_LOCK);
      bwprintf (COM2, "moved %d\n",dx);
      LockRelease (COM2_W_LOCK);
    }
    else {
      LockAcquire (COM2_W_LOCK);
      bwprintf (COM2, "uhoh: no movement.\n");
      LockRelease (COM2_W_LOCK);
    }
  }
}

#define TR2IDX(n) \
  n == 12 ? 0 : \
  n == 22 ? 1 : \
  n == 23 ? 2 : \
  n == 32 ? 3 : \
  n == 52 ? 4 : 0xFFFFFFFF // This'll make us crash!

void trains () {
  RegisterAs ("tr");
  int tid;
  char cmd[64];
  char lastread[10];

  int speeds[80];
  int tr2tid[80];
  int locations[80];
  unsigned int dx[80];
  unsigned int velocity[80];
  int tid2tr[80];

  /*
   * Train# * Calibrated Speed
   * 15 speeds, so that calibrations[N][0] can be 0, and all other numbers map
   * straight into their speeds. Use TR2IDX().
   */
  int calibrations[5][15];

  char sw[32];
  //int head = 0;
  int i ,j;

  struct msg out;
  struct msg *in;
  struct sensorevent latest;
  struct sensorevent trap;
  trap.group   = '\0';
  trap.id      = 0;
  latest.group = 0;
  int legend[MAX_TRAINS];
  for (i=0; i<MAX_TRAINS; i++) {
    legend[i] = 0;
  }
  char *trkB = "---------------------,  "
                "---/  |,-------------,\\ "
                "   |  /      \\|/      \\|"
                "   |  |       |        |"
                "   |  \\      /|\\      /|"
                "---\\  |`-------------\'/ "
                "----\\  `-------------\'  "
                "------------\\---/-------";
  char *trk = trkB;
  // Synchro with ui
  Receive (&tid, NULL, 0);
  Reply (tid, NULL, 0);

  //drawtrack (trk);
  START_TIMER3();

  for (i = 0; i < 80; i++) { dx[i] = speeds[i] = tr2tid[i] = 0; tid2tr[i] = 0; locations[i] = -2; }
  for (i = 0; i < 10; i++) { lastread[i] = 0; }
  for (i = 0; i < 5 ; i++) { for (j = 0; j < 15; j++) { calibrations[i][j] = 0; } }

//TODO REMOVE
  #include "calibration.h"

  int r;
  Create (USER_HIGH, sensorserver);
  int sens = Create (USER_HIGH, sensor_secretary);

  FOREVER {
start:
    r = Receive (&tid, cmd, 64);
    zeromsg (&out);
    // "special" ones
    switch (cmd[0]) {
      case 'v': // st
        Reply (tid, &sw[unfuckswitch(cmd[1])], 1);
        goto start;
        break;
 /*     case 'd': // latest sensor.. .TODO DEPRECATE
        Reply (tid, (char*)(&latest), sizeof (struct sensorevent));
        goto start;
        break;*/
/*      case 'P': // give back location of train TODO DEPRECATE
        cmd[0] = (char)locations[(int)cmd[1]];
        out.d1 = locations[(int)cmd[1]];
        out.d2 = dx[(int)cmd[1]];
        Reply (tid, (char*)(&out), sizeof (struct msg));
        goto start;
        break;*/
      case 'U': // update from a train TODO ADD DRAW CODE
        in = (struct msg*)cmd;
        locations[tid2tr[tid]] = in->d1;
        dx[tid2tr[tid]] = in->d3;
        velocity[tid2tr[tid]] = in->d4;
        zeromsg(&out);
        out.d1 = speeds[tid2tr[tid]];
        if (READ_TIMER3%2 ==0) drawlegend (locations, dx, legend, trk, velocity); // ????TODO
        //bwprintf(COM2, "Giving train %d(%d) new speed: %d --> \r\n",
        //tid2tr[tid],
        //TR2IDX(tid2tr[tid]),
        //speeds[tid2tr[tid]]);
        ////                    calibrations[TR2IDX(tid2tr[tid])][speeds[tid2tr[tid]]]);
        //LockRelease(COM2_W_LOCK);
        //Reply (tid, (char*)(&calibrations[TR2IDX(tid2tr[tid])][speeds[tid2tr[tid]]]), 4);
        Reply (tid, (char*)(&out), sizeof(struct msg));
        goto start;
        break;
      case 'T': //set a trap TODO MAKE BETTER?
        trap.group = cmd[5];
        int id;
        if (cmd[7] == ' ') {
          id = cmd[6] - '0';
        }
        else {
          id = 10 + cmd[7] - '0';
        }
        trap.id = id;
        LockAcquire (COM2_W_LOCK);
        bwprintf(COM2, "Trapping at %c%d\r\n", trap.group, trap.id);
        LockRelease (COM2_W_LOCK);
        break;
      case '?':
        Reply (tid, (char*)(&tid2tr[tid]), sizeof(int));
        break;
      default:
        break;
    }

    Reply (tid, NULL, 0);
    switch (cmd[0]) {
      case 'W': // wh
        LockAcquire (COM2_W_LOCK);
        bwprintf (COM2, "%c%d at %x\n", latest.group, latest.id, latest.time);
        LockRelease (COM2_W_LOCK);
        break;
      case 'A': // add train TODO DRAW CODE
        tr2tid[(int)cmd[1]] =  Create (USER_HIGH, train_agent_notsuck);
        tid2tr[tr2tid[(int)cmd[1]]] = (int)cmd[1];
        i = 0;
        while (legend[i] != 0 && i < MAX_TRAINS) i++;
        LockAcquire (COM2_W_LOCK);
        if (i == MAX_TRAINS) bwprintf (COM2, "Too many trains!\n");  
        else legend[i] = (int)cmd[1];
        LockRelease (COM2_W_LOCK);
        drawlegend (locations, dx, legend, trk, velocity); // ??? TODO
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
                 //if ((trap.id && trap.id == latest.id && trap.group ==
                 //    latest.group) || (latest.id==14 && latest.group=='C')) {
                 //  Putc(COM1, 0x61);
           //    //    Putc(COM1, 0x0C); //stop train 32
                 //}
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
      case 'g': // go TODO DRAW STUFF
        Putc (COM1, 0x60);
        break;
      case 's': // stop TODO DRAW STUFF
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
        LockAcquire (COM2_W_LOCK);
        if (cmd[2] == 's' || cmd[2] == 'S') putpixel(switchlocation(unfuckswitch(cmd[1])),47,trk);
        else putpixel (switchlocation(unfuckswitch(cmd[1])),100,trk);
        LockRelease (COM2_W_LOCK); 
        break;
      case 'a': // switch all
        drawswitches(trk, (cmd[1]=='s' || cmd[1]=='S') ? 47 : 100);
        for (i = 0; i < 22; i++) {
          trains_switch(fuckswitch(i), cmd[1]);
          sw[i] = cmd[1];
        }
        break;
    }
  }
}
