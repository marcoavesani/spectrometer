#include "stepper_motor.h"
#include <QDebug>
#include <iostream>

stepper_motor::stepper_motor(QWidget *parent) : QWidget(parent)
{
    float volts[3]={DEFAULT_VOLT,DEFAULT_VOLT,DEFAULT_VOLT};
    int speed[3] = {DEFAULT_SPEED,DEFAULT_SPEED,DEFAULT_SPEED};
    int movemode[3] ={DEFAULT_MOVEMODE,DEFAULT_MOVEMODE,DEFAULT_MOVEMODE};
    //std::vector<std::string> errormessage;
    errormessage.push_back("No error.");
    errormessage.push_back("device not found.");
    errormessage.push_back("Not enough parameters.");
    errormessage.push_back("Wrong motor number. Must be 0, 1 or 2.");
    errormessage.push_back("Motor write timeout.");
    errormessage.push_back("Voltage out of range (0 to 12V).");
    errormessage.push_back("Speed out of range (0 to 1000).");



    int MOT[3] = {MOT_0,MOT_1,MOT_2,}; //Lib specific??
    commands.push_back("");
      commands.push_back("");  // 0: do nothing
      commands.push_back("."); // terminate
      commands.push_back("go");// go to a certain value
      commands.push_back("init");
      commands.push_back("on");
      commands.push_back("off"); // 5
      commands.push_back("set");
      commands.push_back("exit");
      commands.push_back("setvolt");
      commands.push_back("setspeed");
      commands.push_back("reset"); // 10
      commands.push_back("break");
      commands.push_back("vmode");
      commands.push_back("pmode");
      commands.push_back("getpos");

}

int stepper_motor::emsg(int a){
    qDebug()<<(errormessage[a]).c_str();
    return a;
  }

int stepper_motor::is_motor_ready(int num){
    int tc=timeout_motor_ready/20;
    while (!ioctl(handle,SM32IsReady| MOT[num]) && tc>0) {
    // wait if device is busy //
    nanosleep(&twenty_millisec,&time_left);
    tc--;
  };
    return tc; // zero if timeout //
}

int stepper_motor::get_mot_status(int num){
    int tc=timeout_motor_ready/20;
    ioctl(handle,SM32Request | mcState | MOT[num]);
    while (!ioctl(handle,SM32Replied| MOT[num]) && tc>0) {
    // wait if device is busy //
    nanosleep(&twenty_millisec,&time_left);
    tc--;
    };
    if (tc) return ioctl(handle, SM32Fetch | MOT[num]);
    return 0; // timeout //
}
int stepper_motor::is_motor_there(int motnum){
  int tc=timeout_motor_travel/20;
  while ((get_mot_status(motnum) == mstPos) && tc>0) {
    // wait if device is busy //
    nanosleep(&twenty_millisec,&time_left);
    tc--;
  };
  return tc; // zero if timeout //
}
int stepper_motor::get_mot_pos(int num){
    int tc=timeout_motor_ready/20;
    ioctl(handle,SM32Request | mcPosition | MOT[num]);
    while (!ioctl(handle,SM32Replied| MOT[num]) && tc>0) {
    // wait if device is busy //
    nanosleep(&twenty_millisec,&time_left);
    tc--;
    };
    if (tc) return ioctl(handle, SM32Fetch | MOT[num]);
    return 0; // timeout //
}

int stepper_motor::parse_command(char * cmd) { // returns  0 on success, or error code //

  std::cout<<"Debug"<<std::endl;
  char cmdi2[MAXINLEN+1];
  int idx, retval, converted,arg2;
  int motnum, steps, inspeed;
  float involts;
  if (1!=sscanf(cmd,"%s",cmdi2)){
      idx=0;   // no command //
  } else {
      // get command number //
      for(idx=NUMCOMMANDS;(idx>0)&&(strcmp(cmdi2,(commands[idx]).c_str()));idx--);
  };
  retval=0;
#ifdef DEBUG
  printf("command:>%s<, cmd index:%d\n",cmd,idx);
#endif
  switch (idx) {
      case 1:case 7: // terminate //
      retval=1;break;
      case 2: // go to a certain position //
      switch(sscanf(cmd,"%s%d%d",cmdi2,&motnum,&steps)) {
          case 2: steps=0;
          case 3:
          if (motnum<0 || motnum>2) {retval=-emsg(3);break;};
          // do it //
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
      case 3: // init command //
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
          // set voltage //
          ioctl(handle,SM32Post | mcU | MOT[motnum],
            (int)(volts[motnum]*10.));
          // set pos / move mode //
          if (!is_motor_ready(motnum)) {retval=-emsg(4);break;};
          ioctl(handle,SM32Post | mcPosMode | MOT[motnum],
            (movemode[motnum]?mmMove:mmPos));
          // set speed //
          if (!is_motor_ready(motnum)) {retval=-emsg(4);break;};
          ioctl(handle,SM32Post | mcF | MOT[motnum],
            (int)(movemode[motnum]?0:speed[motnum]));
          // switch on //
          if (!is_motor_ready(motnum)) {retval=-emsg(4);break;};
          ioctl(handle,SM32Post | mcPower | MOT[motnum], 1);
          break;
          default:
          retval=-emsg(2);
      };
      break;
      case 4:case 5: // on/off command //
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
      case 6: // set pos command //
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
      case 8: // set actual and default voltage //
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
          // set voltage //
          ioctl(handle,SM32Post | mcU | MOT[motnum],
            (int)(10.0*volts[motnum]));
      };
      break;
      case 9: // set actual and default speed //
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
          // set speed //
          ioctl(handle,SM32Post | mcF | MOT[motnum], (int)speed[motnum]);
      };
      break;
      case 10: // reset command //
      if (!is_motor_ready(0)) {retval=-emsg(4);break;};
      ioctl(handle, SM32Post | mcReset, 0);
      break;
      case 11: // break command //
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
      case 12:case 13: // vmode/pmode command //
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

 // no command //
  };
  return retval;
}

int stepper_motor::main_func(char * command) {
  int idx, ir, cmo;
  int inh=0; /* stdin */

  if ((fp = fopen(FIFO_FILE, "wb")) ==NULL){
  printf("Error opening file\n");
  exit(1);
  }

  /* open stepper motor device */
  handle=open(DEVICE,O_RDWR);
  if (handle==-1) return -emsg(1);

  do {
    /* read in one line, waiting for newline */
    idx=0;
    while ((ir=read(inh,&cmd[idx],1))==1 && idx<MAXINLEN) {
      if (cmd[idx]=='\n' || cmd[idx]==';') break;
      idx++;
    };
    cmd[idx]='\0';
    if (idx==MAXINLEN)
      while ((ir=read(inh,&cmo,1))==1) if (cmo=='n' || cmo == ',') break;
    /* command parser */
    cmo=parse_command(cmd);
#ifdef DEBUG
    printf("parser return: %d\n",cmo);
#endif
  } while (!cmo);

  fclose(fp);
  ::close(handle);
  return 0;
}

<<<<<<< HEAD

=======
/*=======================*/
>>>>>>> eb4d5a816e00ed9be1e6ae0adb00720039cd409d
int stepper_motor::go(int motnum, int steps) {

        if (motnum<0 || motnum>2) {return(emsg(3));};
        // do it //
        if (!is_motor_ready(motnum)) {return(emsg(4));};
        ioctl(handle,SM32Post | mcGo | MOT[motnum], steps);
        if (waitmode)
            if (!is_motor_there(motnum)) {return(emsg(5));};
        nanosleep(&twenty_millisec,&time_left);
    return(emsg(2));
}

int stepper_motor::init(int motnum, double involts, int inspeed) { // init command //

    if (motnum<0 || motnum>2) {return(emsg(3));};
        if (inspeed<0.0 || inspeed >MAXSPEED) {return(emsg(6));};
        speed[motnum]=inspeed;
        if (involts<0.0 || involts >MAXVOLT) {
            return(emsg(5));};
        volts[motnum]=involts;
        if (!is_motor_ready(motnum)) {return(emsg(4));};
        // set voltage //
        ioctl(handle,SM32Post | mcU | MOT[motnum],
          (int)(volts[motnum]*10.));
        // set pos / move mode //
        if (!is_motor_ready(motnum)) {return(emsg(4));};
        ioctl(handle,SM32Post | mcPosMode | MOT[motnum],
          (movemode[motnum]?mmMove:mmPos));
        // set speed //
        if (!is_motor_ready(motnum)) {return(emsg(4));};
        ioctl(handle,SM32Post | mcF | MOT[motnum],
          (int)(movemode[motnum]?0:speed[motnum]));
        // switch on //
        if (!is_motor_ready(motnum)) {return(emsg(4));};
        ioctl(handle,SM32Post | mcPower | MOT[motnum], 1);
        return(emsg(2));

}

int stepper_motor::set(int motnum, int steps) {  //not sure if that's working as intended

    if (!is_motor_ready(motnum)) {return(emsg(4));};
    ioctl(handle, SM32Post| mcPosition | MOT[motnum], (steps-zeroposition[motnum]));
}

int stepper_motor::setvolt(int motnum, double involts) {

    if (involts<0.0 || involts >MAXVOLT) {
        return (emsg(5));};
    if (motnum<0 || motnum>2) {return(emsg(3));};
        volts[motnum]=involts;
        if (!is_motor_ready(motnum)) {return(emsg(4));};
        // set voltage //
        ioctl(handle,SM32Post | mcU | MOT[motnum],(int)(10.0*volts[motnum]));
}



int stepper_motor::setspeed(int motnum, int inspeed) {

    if (!movemode)
        if (inspeed<0.0 || inspeed >MAXSPEED) {
        return(emsg(6));};
    if (motnum<0 || motnum>2) {return(emsg(3));};
    speed[motnum]=inspeed;
    if (!is_motor_ready(motnum)) {return(emsg(4));};
    // set speed //
    ioctl(handle,SM32Post | mcF | MOT[motnum], (int)speed[motnum]);

}

int stepper_motor::reset(int motnum) {

    if (!is_motor_ready(0)) {return(emsg(4));};
    ioctl(handle, SM32Post | mcReset, 0);

}

int stepper_motor::getpos(int motnum, int &position) {// get position

    if (motnum<0 || motnum>2) {return(emsg(3));};
    position=get_mot_pos(motnum);

}

int stepper_motor::motbreak(int motnum) {

    if (motnum<0 || motnum>2) {return(emsg(3));};
    if (!is_motor_ready(motnum)) {return(emsg(4));};
    ioctl(handle,SM32Post | mcBreak | MOT[motnum],0);
}

int stepper_motor::on(int motnum) { // on command

    if (motnum<0 || motnum>2) {return(emsg(3));};
    if (!is_motor_ready(motnum)) {return(emsg(4));};
    ioctl(handle,SM32Post | mcPower |MOT[motnum],1);

}

int stepper_motor::off(int motnum) { //off command

    if (motnum<0 || motnum>2) {return(emsg(3));};
    if (!is_motor_ready(motnum)) {return(emsg(4));};
    ioctl(handle,SM32Post | mcPower |MOT[motnum],0);

}

int stepper_motor::vmode(int motnum) {

    if (motnum<0 || motnum>2) {return(emsg(3));};
    if (!is_motor_ready(motnum)) {return(emsg(4));};
    movemode[motnum]=1;
    ioctl(handle,SM32Post | mcPosMode | MOT[motnum],(movemode[motnum]?mmMove:mmPos));
}

int stepper_motor::pmode(int motnum) {

    if (motnum<0 || motnum>2) {return(emsg(3));};
    if (!is_motor_ready(motnum)) {return(emsg(4));};
    movemode[motnum]=0;
    ioctl(handle,SM32Post | mcPosMode | MOT[motnum],(movemode[motnum]?mmMove:mmPos));
}

int stepper_motor::setzero(int motnum) {

    if (motnum<0 || motnum>2) {return(emsg(3));};
    if (!is_motor_ready(motnum)) {return(emsg(4));};
    int position;
    getpos(motnum, position);
    zeroposition[motnum] = position;
}

int getnumbofsteps() {

    return NUMBOFSTEPS;
}

