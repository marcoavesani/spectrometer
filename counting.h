#ifndef COUNTING_H
#define COUNTING_H

#include <QWidget>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <time.h>
#include "dt304.h"
#include <iostream>
#include <vector>
#include <string>

/* using definitions of minor device 1 */
#define BOARDNAME "/dev/ioboards/dt30x_0"
#define DEFAULT_INTEGTIME  1000
#define CR_OPTION_DEFAULT 1  /* send newline after values */

/* some check ranges */
#define MAX_INTEGTIME  60000 /* one minute integration time */
#define INITIAL_DELAY 10      /* initial time delay before active gate */
#define DEFAULT_MAXTIMESLOT 60000    /* time to be integrated per run */



class counting : public QWidget
{
    Q_OBJECT
public:
    explicit counting(QWidget *parent = 0);
    int emsg(int code);
    int counting_function(int argc, char *argv[]);
    int getcount(int &finalcount, unsigned long integtime);

private:
    char *optarg;
    int   optind, opterr, optopt;
    struct timespec ten_millisec = {0,10000000};
    struct timespec time_left;
    //char *errormessage[];
    std::vector <std::string> errormessage;
signals:

public slots:
};

#endif // COUNTING_H
