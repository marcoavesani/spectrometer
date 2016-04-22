#ifndef GPIB_H
#define GPIB_H

#include <QObject>
#include <QWidget>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <string.h>

#include "gpib/ib.h"
#define ARRAYYSIZE 100 // Size of read buffer
#define ERRMSGSIZE 1024 // Maximum size of SCPI command string
class gpib : public QWidget
{
    Q_OBJECT
public:
    explicit gpib(QWidget *parent = 0);
    ~gpib();
    static int  Dev;                        // Device handle
    static char ReadBuffer[ARRAYYSIZE + 1];  // Read data buffer
    static char ErrorMnemonic[29][5];

    static void GPIBCleanup(int Dev, const char * ErrorMsg);
    int init ();
    int close();
    int send_data(std::string datastring );
    int receive_data();

    void GPIBWrite(const char* SCPIcmd);
    char *GPIBRead(void);
    //void GPIBCleanup(int Dev, char* ErrorMsg);
signals:

public slots:
};

#endif // GPIB_H
