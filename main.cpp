#include "mainwindow.h"
#include <QApplication>
#include <iostream>
#include <string>
#include "stepper_motor.h"
#include "counting.h"
#include "spectrometer.h"
#include "TApplication.h"
#include "TCanvas.h"
#include "TSystem.h"
#include "TStyle.h"
#include "TH1D.h"
#include "gpib.h"



int main(int argc, char *argv[])

{   TApplication app ("A",0,0);
    app.SetReturnFromRun(kTRUE);
    gStyle-> SetCanvasPreferGL(kTRUE);
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    stepper_motor motor;
    if(motor.isthedeviceopen()==false){
        qDebug()<<"ERROR MOTOR NOT FOUND LAUNCHING DESTRUCTOR";
        return 1;
    }
    counting count;
    gpib communication;
    communication.init();
    spectrometer calibration(&motor,&count,&communication);
    QObject::connect(&w, &MainWindow::valuewl,&calibration , &spectrometer::addpeak );

    calibration.gatherlotsofdata(1530, 1570, 1,"",0,0);
    /*
    sleep(10);
    communication.GPIBWrite("WA1558");
    qDebug()<<communication.GPIBRead();
    //calibration.scanandplot(1605,1605.5,0.05);
    //calibration.savedataas("./temp");

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
    //TCanvas * c1= new TCanvas("c1");
    //TH1D * histo=new TH1D ("th1","th1",1000,0,1000);
    //histo->Draw();

    return a.exec();
}
