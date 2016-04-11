#include "mainwindow.h"
#include <QApplication>
#include <iostream>
#include "stepper_motor.h"
#include "counting.h"
#include "calibrate.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    stepper_motor motor;
    counting count;
    calibrate calibration(&motor,&count);
    QObject::connect(&w, &MainWindow::valuewl,&calibration , &calibrate::addpeak );

/*

    int final_count=0;
    //count.getcount(final_count,1000,100);
    // count.emsg(2);
    std::cout<<final_count<<std::endl;
    //motor.parse_command("go 1 10");
    //std::cout<<count.optind<<std::endl;*/
    return a.exec();
}
