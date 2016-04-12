#include "calibrate.h"
#include <cmath>
#include <QDebug>
using namespace std;
calibrate::calibrate(stepper_motor * step, counting * count, QWidget *parent) : QWidget(parent)
{

    step_mot_point=step;
    counting_point=count;
    stepsize = int(0.0171892/360*(step->getnumbofsteps()));
    searcharea=500*(0.0171892/360*(step->getnumbofsteps()));
    motnum=0;
}

int calibrate::firstpeak(double wavelength) {
    //step_mot_point->set(motnum,0);
    step_mot_point->go(motnum,int (((predictposition(wavelength))/360)*step_mot_point->getnumbofsteps()));
    qDebug()<< predictposition(wavelength);
    qDebug()<< int (((predictposition(wavelength))/360)*step_mot_point->getnumbofsteps());



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

double calibrate::predictposition(double wavelength){
    //Wavelength is in nm output is in radians
    /*theta= -1*(std::acos((4*gratingconstant*wavelength*cos(blazingangle)*sin(blazingangle) -
                      4*sqrt(2*pow(gratingconstant,4)*pow(sin(blazingangle),4) -
                         pow(gratingconstant,2)*pow(wavelength,2)*pow(sin(blazingangle),4) -
                         2*pow(gratingconstant,4)*cos(2*blazingangle)*pow(sin(blazingangle),4)))/
                    (2.*pow(gratingconstant,2)*(1 - 2*pow(cos(gratingconstant),2) + pow(cos(gratingconstant),4) +
                        2*pow(sin(blazingangle),2) + 2*pow(cos(blazingangle),2)*pow(sin(blazingangle),2) +
                        pow(sin(blazingangle),4)))));

    return (((theta)*180)/M_PI); */


    if(wavelength>1403.56) {
        return (getangle(wavelength));
    }
    else {
        return (-1*getangle(wavelength));
    }

}

int calibrate::addpeak(double wavelength) {
    qDebug()<<wavelength;
    std::ofstream fileout;
    fileout.open("./fileout",std::ofstream::out | std::ofstream::app);

    int position = predictposition(wavelength);
    double maxcount = 0;
    int maxcountposition = 0;
    step_mot_point->set(motnum, position-(searcharea/2));

    for(int i = 0; i<searcharea; i++) {

        int events = 0;
        step_mot_point->go(motnum, stepsize);
        counting_point->getcount(events, integtime);
        maxcountposition = (maxcount>events?(position-searcharea+i):maxcountposition);
        maxcount = (maxcount>events?maxcount:events);
    }

    positions.push_back(360*(double)maxcountposition/(double)step_mot_point->getnumbofsteps());
    wavelengths.push_back(wavelength);

    fileout<<wavelength<<'/t'<<360*(double)maxcountposition/(double)step_mot_point->getnumbofsteps()<<std::endl;
    fileout.close();
}

void calibrate::changestepsize(double angle) {

    stepsize = (int)angle/360*step_mot_point->getnumbofsteps();
}

double calibrate::getangle(double l){

    return (std::acos((4*gratingconstant*l*cos(blazingangle)*sin(blazingangle) +
                       4*sqrt(2*pow(gratingconstant,4)*pow(sin(blazingangle),4) -
                          pow(gratingconstant,2)*pow(l,2)*pow(sin(blazingangle),4) -
                          2*pow(gratingconstant,4)*cos(2*blazingangle)*pow(sin(blazingangle),4)))/
                     (2.*pow(gratingconstant,2)*(1 - 2*pow(cos(gratingconstant),2) + pow(cos(gratingconstant),4) +
                         2*pow(sin(blazingangle),2) + 2*pow(cos(blazingangle),2)*pow(sin(blazingangle),2) +
                         pow(sin(blazingangle),4)))));
}
