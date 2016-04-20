#include "counting.h"
#include <QDebug>

counting::counting(QWidget *parent) : QWidget(parent)
{
    struct timespec ten_millisec = {0,10000000};
   /*
    char *errormes = {
      "No error.",
      "Integration time out of range.",
      "IO device busy.",
      "Can't open devcice.",
      "Option error. Usage: dt304_counter [-t time ] [-n] [-m maxtime]\n",
      "??",
      "??",
      "Maximum timeslot size out of range.",
    };*/
    errormessage.push_back("No error.");
    errormessage.push_back("Integration time out of range.");
    errormessage.push_back( "IO device busy.");
    errormessage.push_back( "Can't open devcice.");
    errormessage.push_back("Option error. Usage: dt304_counter [-t time ] [-n] [-m maxtime]\n");
    errormessage.push_back("??");
    errormessage.push_back("??");
    errormessage.push_back( "Maximum timeslot size out of range.");

}

//int counting::emsg(int code) {
//  fprintf(stderr,"%s\n",errormessage[code]);
//  return code;
//}

int counting::emsg(int code) {
  //fprintf(stderr,"%s\n",errormessage[code]);
  qDebug()<<(errormessage[code]).c_str();
  return code;
}

int counting::counting_function(int argc, char *argv[]) {
  unsigned long integtime = DEFAULT_INTEGTIME;
  unsigned long integtime1;
  int opt;
  int val0,val1; /* store intermediate results */
  int fh;     /* iocard handle */
  int maxtimesl = DEFAULT_MAXTIMESLOT;
  int base_mode_3; /* to store mode of one shot counter */
  int uwert;
  int finalcount=0;
  int cr_option = CR_OPTION_DEFAULT;  /* if true, append newline to output */


  /* try to interpret options */
  opterr=0; /* be quiet when there are no options */
  while ((opt=getopt(argc, argv, "nm:t:")) != EOF) {
    switch (opt) {
    case 't':
      sscanf(optarg,"%ld",&integtime);
      /* integration time ok? */
      if (integtime > MAX_INTEGTIME || integtime < 0) return emsg(1);
      break;
    case 'm':
      sscanf(optarg,"%d",&maxtimesl);
      if (maxtimesl <1 || maxtimesl > DEFAULT_MAXTIMESLOT) return emsg(7);
      break;
    case 'n':
      /* clear newline flag */
      cr_option=0;
      break;
    default:
    return emsg(4);
      break;
    };
  };

  /* open counter card driver */
  if ((fh=open(BOARDNAME,O_RDWR)) == -1) {
    if (errno == EBUSY) { return -emsg(2);}
    else { return -emsg(3);};
  };

  /* calculate real integration time in elementary steps */
  while (integtime >0) {
    integtime1=(integtime>maxtimesl?maxtimesl:integtime);
    integtime-=integtime1;


    /* reset/unreset counter chip */
    ioctl(fh,TIMER_RESET);

    /* prepare counter 2 as 1 kHz heartbeat for the gate output */
    ioctl(fh,SET_USER_PERIODE_2, 0xffffff+2-200);
    //ioctl(fh,SET_USER_PERIODE_2, 1);
    ioctl(fh,SET_USER_PULSE_2, 0xffffff-100+1);
    ioctl(fh,SET_USER_CONTROL_2, SELECT_INTERNAL_CLOCK | USER_GATE_HIGH |
      CONTINUOUS_INCR_MODE | OUTPUT_HIGH_ACTIVE|ONESHOT_TRIGGER_ENABLE);

    /* prepare counter 3 as gating one shot, out high active */
    ioctl(fh,SET_USER_PULSE_3, 0xffff-integtime1+1);
    ioctl(fh,SET_USER_PERIODE_3, 0xffff-integtime1-INITIAL_DELAY+1);
    base_mode_3 = SELECT_CASCADE_CLOCK |  USER_GATE_LOW |
    NONRETRIG_ONESHOT_MODE | OUTPUT_HIGH_ACTIVE;
    ioctl(fh,SET_USER_CONTROL_3,base_mode_3);

    /* prepare counter 0 for event counting */
    ioctl(fh,SET_USER_PERIODE_0, 1);
    ioctl(fh,SET_USER_PULSE_0, 0x8000);
    ioctl(fh,SET_USER_CONTROL_0,SELECT_EXTERNAL_CLOCK | USER_GATE_EXTERN |
      CONTINUOUS_INCR_MODE | OUTPUT_LOW_ACTIVE|ONESHOT_TRIGGER_ENABLE );

    /* prepare counter 1 for overflow detect counting */
    ioctl(fh,SET_USER_CONTROL_1,SELECT_CASCADE_CLOCK | USER_GATE_HIGH |
      CONTINUOUS_INCR_MODE | OUTPUT_LOW_ACTIVE );
    ioctl(fh,SET_USER_PERIODE_1, 1);

   /* trigger one shot */
    ioctl(fh,SET_USER_CONTROL_3, base_mode_3 | ONESHOT_TRIGGER_ENABLE);
    ioctl(fh,SET_USER_CONTROL_3, base_mode_3 |  USER_GATE_HIGH );

   /* check if counting is done */
    while (ioctl(fh,GET_USER_STATUS) & CNT_3_IS_TRIG_ENABLED) {
     nanosleep(&ten_millisec, &time_left);
    }

    /* read in values in counter0/1 */
    val0=ioctl(fh,GET_USER_PERIODE_0); val1=ioctl(fh,GET_USER_PERIODE_1);

    /* add value to total counts */
    uwert = val0-1+((val1-1)<<16);
    finalcount +=uwert;
  }

  /* reset timing chip */
  ioctl(fh,ASSERT_TIMER_RESET);
  ::close(fh);  //Call the global instead of QWidget

  /* send final value to stdout */
  fprintf(stdout," %d",finalcount);
  if (cr_option) fprintf(stdout,"\n");
  return 0;
}


int counting::getcount(int &finalcount, unsigned long integtime) {
  int maxtimesl= DEFAULT_MAXTIMESLOT;
    finalcount=0;
  unsigned long integtime1;
  int val0,val1; /* store intermediate results */
  int fh;     /* iocard handle */
  int base_mode_3; /* to store mode of one shot counter */
  int uwert;
  if (integtime > MAX_INTEGTIME || integtime < 0) return emsg(1);
  if (maxtimesl <1 || maxtimesl > DEFAULT_MAXTIMESLOT) return emsg(7);
 // std::cout<<"Debug"<<std::endl;
  /* open counter card driver */
  if ((fh=open(BOARDNAME,O_RDWR)) == -1) {
    if (errno == EBUSY) { return -emsg(2);}
    else { return -emsg(3);};
  };
   //std::cout<<"Debug"<<std::endl;
  /* calculate real integration time in elementary steps */
  while (integtime >0) {
    integtime1=(integtime>maxtimesl?maxtimesl:integtime);
    integtime-=integtime1;
//std::cout<<"Debug"<<std::endl;

    /* reset/unreset counter chip */
    ioctl(fh,TIMER_RESET);
//std::cout<<"Debug"<<std::endl;
    /* prepare counter 2 as 1 kHz heartbeat for the gate output */
    ioctl(fh,SET_USER_PERIODE_2, 0xffff-20000+2);
    ioctl(fh,SET_USER_PULSE_2, 0xffff-10000+1);
    ioctl(fh,SET_USER_CONTROL_2, SELECT_INTERNAL_CLOCK | USER_GATE_HIGH |
      CONTINUOUS_INCR_MODE | OUTPUT_HIGH_ACTIVE|ONESHOT_TRIGGER_ENABLE);

    /* prepare counter 3 as gating one shot, out high active */
    ioctl(fh,SET_USER_PULSE_3, 0xffff-integtime1+1);
    ioctl(fh,SET_USER_PERIODE_3, 0xffff-integtime1-INITIAL_DELAY+1);
    base_mode_3 = SELECT_CASCADE_CLOCK |  USER_GATE_LOW |
    NONRETRIG_ONESHOT_MODE | OUTPUT_HIGH_ACTIVE;
    ioctl(fh,SET_USER_CONTROL_3,base_mode_3);

    /* prepare counter 0 for event counting */
    ioctl(fh,SET_USER_PERIODE_0, 1);
    ioctl(fh,SET_USER_PULSE_0, 0x8000);
    ioctl(fh,SET_USER_CONTROL_0,SELECT_EXTERNAL_CLOCK | USER_GATE_EXTERN |
      CONTINUOUS_INCR_MODE | OUTPUT_LOW_ACTIVE|ONESHOT_TRIGGER_ENABLE );

    /* prepare counter 1 for overflow detect counting */
    ioctl(fh,SET_USER_CONTROL_1,SELECT_CASCADE_CLOCK | USER_GATE_HIGH |
      CONTINUOUS_INCR_MODE | OUTPUT_LOW_ACTIVE );
    ioctl(fh,SET_USER_PERIODE_1, 1);

   /* trigger one shot */
    ioctl(fh,SET_USER_CONTROL_3, base_mode_3 | ONESHOT_TRIGGER_ENABLE);
    ioctl(fh,SET_USER_CONTROL_3, base_mode_3 |  USER_GATE_HIGH );

   /* check if counting is done */
    while (ioctl(fh,GET_USER_STATUS) & CNT_3_IS_TRIG_ENABLED) {
     nanosleep(&ten_millisec, &time_left);
    }

    /* read in values in counter0/1 */
    val0=ioctl(fh,GET_USER_PERIODE_0); val1=ioctl(fh,GET_USER_PERIODE_1);

    /* add value to total counts */
    uwert = val0-1+((val1-1)<<16);
    finalcount +=uwert;
  }

  /* reset timing chip */
  ioctl(fh,ASSERT_TIMER_RESET);
  ::close(fh);  //Call the global instead of QWidget
  return 0;
}







