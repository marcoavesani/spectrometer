#include "calibrate.h"

calibrate::calibrate(QWidget *parent,stepper_motor * step, counting * count) : QWidget(parent)
{

    step_mot_point=step;
    counting_point=count;
}

int calibrate::firstpeak(double wavelength) {

    step_mot_point->setzero(motnum);
    step_mot_point->go(-(step_mot_point->getnumbofsteps()/4));
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

        cout << "Couldn't open file to save claibration parameters. \n";
        return 1;
    }


}

int calibrate::predictposition(double wavelength){

}

int calibrate::addpeak(double wavelength) {

    int position = predictposition(wavelength);
    double maxcount = 0;
    int maxcountposition = 0;
    step_mot_point->set(motnum, position-(searcharea/2));

    for(int i = 0; i<searcharea; i++) {

        int events = 0;
        step_mot_point->go(motnum, 1);
        counting_point->getcount(events, integtime, maxtimesl);
        maxcountposition = (maxcount>events?(position-searcharea+i):maxcountposition);
        maxcount = (maxcount>events?maxcount:events);
    }

    positions.push_back(360*(double)maxcountposition/(double)step_mot_point->getnumbofsteps());
    wavelengths.push_back(wavelength);
}

