#include "mainwindow.h"
#include <QApplication>
#include <iostream>
#include "stepper_motor.h"
#include "counting.h"
#include "calibrate.h"
#include "spectrom.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    stepper_motor motor;
    if(motor.isthedeviceopen()==false){
        qDebug()<<"ERROR MOTOR NOT FOUND LAUNCHING DESTRUCTOR";
        return 1;
    }
    counting count;
    calibrate calibration(&motor,&count);
    spectrom spectrometer(&calibration);
    QObject::connect(&w, &MainWindow::valuewl,&calibration , &calibrate::addpeak );

    spectrometer.scanandplot(1540,1550,0.01);
    spectrometer.savedataas("./spectrum");

  // motor.go(0,0);
   // motor.go(1,1000000000);
   // motor.go(2,1000000000);
   // std::string acommands="set 0 100000";
   //motor.parse_command("init 0 3.1 1000");
   //motor.parse_command("on 0");
   //motor.parse_command("go 0 100000000");


    //int final_count=0;
    //count.getcount(final_count,1000);
    // count.emsg(2);
    //std::cout<<final_count<<std::endl;
    //motor.parse_command("go 1 10");
    //std::cout<<count.optind<<std::endl;*/
    return a.exec();
}
