#include "stepper_motor.h"

stepper_motor::stepper_motor(QWidget *parent) : QWidget(parent)
{

}

int stepper_motor::emsg(int a){
    fprintf(stderr, "%s\n",errormessage[a]);
    return a;
  }

int stepper_motor::is_motor_ready(int num){
    int tc=timeout_motor_ready/20;
    while (!ioctl(handle,SM32IsReady| MOT[num]) && tc>0) {
    /* wait if device is busy */
    nanosleep(&twenty_millisec,&time_left);
    tc--;
  };
    return tc; /* zero if timeout */
}

int stepper_motor::get_mot_status(int num){
    int tc=timeout_motor_ready/20;
    ioctl(handle,SM32Request | mcState | MOT[num]);
    while (!ioctl(handle,SM32Replied| MOT[num]) && tc>0) {
    /* wait if device is busy */
    nanosleep(&twenty_millisec,&time_left);
    tc--;
    };
    if (tc) return ioctl(handle, SM32Fetch | MOT[num]);
    return 0; /* timeout */
}
int stepper_motor::is_motor_there(motnum){
  int tc=timeout_motor_travel/20;
  while ((get_mot_status(motnum) == mstPos) && tc>0) {
    /* wait if device is busy */
    nanosleep(&twenty_millisec,&time_left);
    tc--;
  };
  return tc; /* zero if timeout */
}
int stepper_motor::get_mot_pos(int num){
    int tc=timeout_motor_ready/20;
    ioctl(handle,SM32Request | mcPosition | MOT[num]);
    while (!ioctl(handle,SM32Replied| MOT[num]) && tc>0) {
    /* wait if device is busy */
    nanosleep(&twenty_millisec,&time_left);
    tc--;
    };
    if (tc) return ioctl(handle, SM32Fetch | MOT[num]);
    return 0; /* timeout */
}

int stepper_motor::parse_command(char * cmd) { /* returns  0 on success, or error code */
  char cmdi2[MAXINLEN+1];
  int idx, retval, converted,arg2;
  int motnum, steps, inspeed;
  float involts;
  if (1!=sscanf(cmd,"%s",cmdi2)){
      idx=0;   /* no command */
  } else {
      /* get command number */
      for(idx=NUMCOMMANDS;(idx>0)&&(strcmp(cmdi2,commands[idx]));idx--);
  };
  retval=0;
#ifdef DEBUG
  printf("command:>%s<, cmd index:%d\n",cmd,idx);
#endif
  switch (idx) {
      case 1:case 7: /* terminate */
      retval=1;break;
      case 2: /* go to a certain position */
      switch(sscanf(cmd,"%s%d%d",cmdi2,&motnum,&steps)) {
          case 2: steps=0;
          case 3:
          if (motnum<0 || motnum>2) {retval=-emsg(3);break;};
          /* do it */
          if (!is_motor_ready(motnum)) {retval=-emsg(4);break;};
          ioctl(handle,SM32Post | mcGo | MOT[motnum], steps);
          if (waitmode)
              if (!is_motor_there(motnum)) {retval=-emsg(5);break;};
          nanosleep(&twenty_millisec,&time_left);
          break;
          default:
      retval=-emsg(2);
      };
      break;
      case 3: /* init command */
      arg2=sscanf(cmd,"%s%d%f%d",cmdi2,&motnum,&involts,&inspeed);
      if (motnum<0 || motnum>2) {retval=-emsg(3);break;};
      switch (arg2) {
          case 4:
          if (inspeed<0.0 || inspeed >MAXSPEED) {
              retval = -emsg(6);break;};
          speed[motnum]=inspeed;
          case 3:
          if (involts<0.0 || involts >MAXVOLT) {
              retval = -emsg(5);break;};
          volts[motnum]=involts;
          case 2:
          if (!is_motor_ready(motnum)) {retval=-emsg(4);break;};
          /* set voltage */
          ioctl(handle,SM32Post | mcU | MOT[motnum],
            (int)(volts[motnum]*10.));
          /* set pos / move mode */
          if (!is_motor_ready(motnum)) {retval=-emsg(4);break;};
          ioctl(handle,SM32Post | mcPosMode | MOT[motnum],
            (movemode[motnum]?mmMove:mmPos));
          /* set speed */
          if (!is_motor_ready(motnum)) {retval=-emsg(4);break;};
          ioctl(handle,SM32Post | mcF | MOT[motnum],
            (int)(movemode[motnum]?0:speed[motnum]));
          /* switch on */
          if (!is_motor_ready(motnum)) {retval=-emsg(4);break;};
          ioctl(handle,SM32Post | mcPower | MOT[motnum], 1);
          break;
          default:
          retval=-emsg(2);
      };
      break;
      case 4:case 5: /* on/off command */
      arg2=sscanf(cmd,"%s%d",cmdi2,&motnum);
      if (motnum<0 || motnum>2) {retval=-emsg(3);break;};
      switch (arg2) {
          case 2:
          if (!is_motor_ready(motnum)) {retval=-emsg(4);break;};
          ioctl(handle,SM32Post | mcPower |MOT[motnum],
            (idx==4?1:0));
          break;
          default:
          retval=-emsg(2);
      };
      break;
      case 6: /* set pos command */
      steps=0;
      switch (sscanf(cmd,"%s%d%d",cmdi2,&motnum,&steps)) {
          case 3:case 2:
          if (!is_motor_ready(motnum)) {retval=-emsg(4);break;};
          ioctl(handle, SM32Post| mcPosition | MOT[motnum], steps);
          break;
          default:
          retval=-emsg(2);
      };
      break;
      case 8: /* set actual and default voltage */
      switch (sscanf(cmd,"%s%d%f",cmdi2,&motnum,&involts)) {
          case 2:
          involts=DEFAULT_VOLT;
          break;
          case 3:
          if (involts<0.0 || involts >MAXVOLT) {
              retval = -emsg(5);break; };
          if (motnum<0 || motnum>2) {retval=-emsg(3);break;};
          break;
          default:
      retval = -emsg(2);
      };
      if (retval==0) {
          volts[motnum]=involts;
          if (!is_motor_ready(motnum)) {retval=-emsg(4);break;};
          /* set voltage */
          ioctl(handle,SM32Post | mcU | MOT[motnum],
            (int)(10.0*volts[motnum]));
      };
      break;
      case 9: /* set actual and default speed */
      switch (sscanf(cmd,"%s%d%d",cmdi2,&motnum,&inspeed)) {
          case 2:
          inspeed=DEFAULT_SPEED;
          break;
          case 3:
          if (!movemode)
              if (inspeed<0.0 || inspeed >MAXSPEED) {
              retval = -emsg(6);break; };
          if (motnum<0 || motnum>2) {retval=-emsg(3);break;};
          break;
          default:
          retval = -emsg(2);
      };
      if (retval==0) {
          speed[motnum]=inspeed;
          if (!is_motor_ready(motnum)) {retval=-emsg(4);break;};
          /* set speed */
          ioctl(handle,SM32Post | mcF | MOT[motnum], (int)speed[motnum]);
      };
      break;
      case 10: /* reset command */
      if (!is_motor_ready(0)) {retval=-emsg(4);break;};
      ioctl(handle, SM32Post | mcReset, 0);
      break;
      case 11: /* break command */
      switch (sscanf(cmd,"%s%d",cmdi2,&motnum)) {
          case 2:
          if (motnum<0 || motnum>2) {retval=-emsg(3);break;};
          if (!is_motor_ready(motnum)) {retval=-emsg(4);break;};
          ioctl(handle,SM32Post | mcBreak | MOT[motnum],0);
          break;
          default:
          retval = -emsg(2);
      };
      break;
      case 12:case 13: /* vmode/pmode command */
      switch (sscanf(cmd,"%s%d",cmdi2,&motnum)) {
          case 2:
          if (motnum<0 || motnum>2) {retval=-emsg(3);break;};
          if (!is_motor_ready(motnum)) {retval=-emsg(4);break;};
          movemode[motnum]=(idx=12?1:0);
          ioctl(handle,SM32Post | mcPosMode | MOT[motnum],
            (movemode[motnum]?mmMove:mmPos));
          break;
          default:
          retval = -emsg(2);
      };
      case 14: // get position
      sscanf(cmd,"%s%d",cmdi2,&motnum);
      if (motnum<0 || motnum>2) {retval=-emsg(3);break;};
      //printf("%ld\n", get_mot_pos(motnum), "\n");
      fprintf(fp, "%ld\n", get_mot_pos(motnum), "\n");
      fflush(fp);
      break;

      case 0:default: ;

 /* no command */
  };
  return retval;
}
