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
    explicit calibrate(QWidget *parent = 0, stepper_motor *step, counting *count);
    int firstpeak(double wavelength); //if there is no calibration file the first peak has to added with this function
    int addpeak(double wavelength);
    void docalib();

private:
    int predictposition(double wavelength); //estimates peak position for given wavelength with default calibration

    int searcharea = 1000; //area around predicted peak position in which peak will be searched for
    std::string calibpath; //where calibration file will looked for/go to
    std::vector<double> wavelengths;
    std::vector<int> positions;
    int motnum = 0;
    stepper_motor * step_mot_point;
    counting * counting_point;
    unsigned long integtime; //needs to be initialized with proper value
    int maxtimesl; //needs to be initialized with proper value
    double blazingangle = ; // in radian
    int gratingconstant = ; //lines per meter


signals:

public slots:
};

#endif // CALIBRATE_H
