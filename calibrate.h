#ifndef CALIBRATE_H
#define CALIBRATE_H
#include <string>
#include <vector>
#include "counting.h"
#include "stepper_motor.h"
#include <QWidget>
#include <fstream>
#include <iostream>

class calibrate : public QWidget
{
    Q_OBJECT
public:
    explicit calibrate( stepper_motor *step, counting *count,QWidget *parent = 0);
    ~calibrate();
    int firstpeak(double wavelength); //moves to the position where you expect maximal emission according to the grating equation
    int addpeak(double wavelength);
    void docalib();
    void changestepsize(double angle); //Change stepsize in degree

private:

    int predictposition(double wavelength);
    //double getangle(double l);
    int searcharea; //area around predicted peak position in which peak will be searched for
    std::string calibpath; //where calibration file will looked for/go to
    std::vector<double> wavelengths;
    std::vector<int> positions;
    int motnum = 0;
    stepper_motor * step_mot_point;
    counting * counting_point;
    unsigned long integtime=10000;
    //int maxtimesl; //needs to be initialized with proper value

    double blazingangle = M_PI*((28.68)/180); // in Rad
    double gratingconstant =( 1.66666*pow(10,3)); //inverse of( distance between two lines in nm)
    int stepsize;

signals:

public slots:
};

#endif // CALIBRATE_H
