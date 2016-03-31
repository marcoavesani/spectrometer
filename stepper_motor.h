#ifndef STEPPER_MOTOR_H
#define STEPPER_MOTOR_H
#include <QWidget>

#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include "sm32_2.h"   //Probably card specific lib
#include <time.h>

/* #define DEBUG */
/* default values and global variables */
#define DEFAULT_VOLT 3.1        //vel for PWM
#define DEFAULT_SPEED 1000      //Speed of stepper
#define DEFAULT_MOVEMODE 0  /* 0: pos, 1: vel */

#define DEVICE  "/dev/ioboards/stepper3"    //Not really nice... Can change
#define FIFO_FILE "/home/kikor/programs/spectro/fifo_spectro"   //IO
#define MAXINLEN 200

/* some timeouts (all in ms) and limits*/
#define timeout_motor_ready 2000
#define waitmode 0   /* o: don't wait after write command */
#define timeout_motor_travel 15000
#define MAXVOLT 12.0
#define MAXSPEED 1000
#define NUMCOMMANDS 14

class stepper_motor : public QWidget
{
    Q_OBJECT
public:
    explicit stepper_motor(QWidget *parent = 0);
    int emsg(int a);
    int is_motor_ready(int num);
    int get_mot_status(int num);
    int is_motor_there(motnum);
    int get_mot_pos(int num);
    int parse_command;





private:
    int handle;
    int motnum=3;
    float volts[3]={DEFAULT_VOLT,DEFAULT_VOLT,DEFAULT_VOLT};        //3 Values... Number of motors...
    int speed[3] = {DEFAULT_SPEED,DEFAULT_SPEED,DEFAULT_SPEED};
    int movemode[3] ={DEFAULT_MOVEMODE,DEFAULT_MOVEMODE,DEFAULT_MOVEMODE};
    FILE *fp;
    char * errormessage[] ={
      "No error.",
      "device not found.",
      "Not enough parameters.",
      "Wrong motor number. Must be 0, 1 or 2.",
      "Motor write timeout.",
      "Voltage out of range (0 to 12V).", /* 5 */
      "Speed out of range (0 to 1000).",
    };
    int MOT[] = {MOT_0,MOT_1,MOT_2,}; //Lib specific??

    /* some basic motor commands */
    struct timespec twenty_millisec = {0,20000000};
    struct timespec time_left;
    char cmd[MAXINLEN+1];
    char * commands[]={
      "",  /* 0: do nothing */
      ".", /* terminate */
      "go",/* go to a certain value */
      "init",
      "on",
      "off", /* 5 */
      "set",
      "exit",
      "setvolt",
      "setspeed",
      "reset", /* 10 */
      "break",
      "vmode",
      "pmode",
      "getpos",
    };



signals:

public slots:
};

#endif // STEPPER_MOTOR_H
///
