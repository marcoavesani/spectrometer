#include "calibrate.h"
#include <cmath>
#include <QDebug>
using namespace std;
calibrate::calibrate(stepper_motor * step, counting * count, QWidget *parent) : QWidget(parent)
{

    step_mot_point=step;
    counting_point=count;
    stepsize = int(0.00171892/360*(step->getnumbofsteps()));
    searcharea=100*(0.00171892/360*(step->getnumbofsteps()));
    motnum=0;
    step_mot_point->go(0,0);
    step_mot_point->go(1,0);
    step_mot_point->go(2,0);


}

calibrate::~calibrate(){
    step_mot_point->go(0,0);
    step_mot_point->go(1,0);
    step_mot_point->go(2,0);
}
int calibrate::firstpeak(double wavelength) {
    //step_mot_point->set(motnum,0);
    step_mot_point->go(motnum,predictposition(wavelength));
    qDebug()<< predictposition(wavelength);

    /*int events=0;
    counting_point->getcount(events, integtime);
    qDebug()<<events;*/

    std::ofstream fileout;
    fileout.open("./fileout",std::ofstream::out | std::ofstream::app);
    fileout <<wavelength<< " "<<(predictposition(wavelength)/step_mot_point->getnumbofsteps()*360)<<endl;



    /*
    step_mot_point->setzero(motnum);
    step_mot_point->go(motnum,-(step_mot_point->getnumbofsteps()/4));
    double maxcount = 0;
    int maxcountposition = 0;

    for(int i = 0; i<step_mot_point->getnumbofsteps()/2; i++) {

        int events = 0;
        step_mot_point->go(motnum, 1);
        counting_point->getcount(events, integtime, maxtimesl);
        maxcountposition = (maxcount>events?(position-searcharea+i):maxcountposition);
        maxcount = (maxcount>events?maxcount:events);
    }
    positions.push_back(360*(double)maxcountposition/(double)step_mot_point->getnumbofsteps());
    wavelengths.push_back(wavelength);

    std::ofstream firstcalib;
    firstcalib.open("firstcalib.var");

    if(firstcalib.is_open()) {

        firstcalib << 360*(double)maxcountposition/(double)step_mot_point->getnumbofsteps() << "\n" << wavelength;
        firstcalib.close();
    }
    else {

        std::cout << "Couldn't open file to save claibration parameters. \n";
        return 1;
    }
    */


}

int calibrate::predictposition(double wavelength){
    //returns in steps. if you make a new spectroscope linear model should be sufficient

return (-2593400+3390.29*wavelength+-1.28168*pow(wavelength,2));
}

int calibrate::addpeak(double wavelength) {
    qDebug()<<wavelength;
    std::ofstream fileout;
    fileout.open("./fileout",std::ofstream::out | std::ofstream::app);
    qDebug() << "filout:" << fileout.is_open();
    fileout << "peak at " << wavelength << "nm" << std::endl;

    std::ofstream forplot;
    forplot.open("./forplot",std::ofstream::out);
    qDebug() << "forplot:" << forplot.is_open();

    int position = predictposition(wavelength);
    double maxcount = 0;
    int maxcountposition = 0;
    //step_mot_point->set(motnum, position-(searcharea/2));

    for(int i = 0; i*stepsize<searcharea; i++) {

        int events = 0;
        step_mot_point->go(motnum, position+i*stepsize-searcharea/2);
        sleep(1);
        counting_point->getcount(events, integtime);
        qDebug()<<events;
        maxcountposition = (maxcount>events?(position+i*stepsize-searcharea/2):maxcountposition);
        maxcount = (maxcount>events?maxcount:events);
        forplot<<(position+i*stepsize-searcharea/2)<<" "<<events<<std::endl;

    }
     qDebug()<<maxcount;
    positions.push_back(360*(double)maxcountposition/(double)step_mot_point->getnumbofsteps());
    wavelengths.push_back(wavelength);

    fileout<<wavelength<<'/t'<<360*(double)maxcountposition/(double)step_mot_point->getnumbofsteps()<<std::endl;
    fileout.close();
    forplot.close();
}

void calibrate::changestepsize(double angle) {

    stepsize = (int)angle/360*step_mot_point->getnumbofsteps();
}

/*double calibrate::getangle(double l){


    return acos((4*gratingconstant*l*cos(blazingangle)*sin(blazingangle) +
                 4*sqrt(2*pow(gratingconstant,4)*pow(sin(blazingangle),4) - pow(gratingconstant,2)*pow(l,2)*pow(sin(blazingangle),4) -
                    2*pow(gratingconstant,4)*cos(2*blazingangle)*pow(sin(blazingangle),4)))/
               (2.*pow(gratingconstant,2)*(1 - 2*pow(cos(blazingangle),2) + pow(cos(blazingangle),4) + 2*pow(sin(blazingangle),2) +
                   2*pow(cos(blazingangle),2)*pow(sin(blazingangle),2) + pow(sin(blazingangle),4))));
}*/
