#ifndef SPECTROMETER_H
#define SPECTROMETER_H
#include <string>
#include <vector>
#include "counting.h"
#include "stepper_motor.h"
#include <QWidget>
#include <fstream>
#include "TFile.h"
#include <algorithm>
#include "TH1D.h"
#include "TF1.h"
#include "TApplication.h"
#include "TSystem.h"
#include "TCanvas.h"
#include "Math/Minimizer.h"



class spectrometer : public QWidget
{
    Q_OBJECT
public:
    explicit spectrometer( stepper_motor *step, counting *count,QWidget *parent = 0);
    ~spectrometer();
    int firstpeak(double wavelength); //moves to the position where you expect maximal emission according to the grating equation
    int addpeak(double wavelength);
    int scan(double shortestwavelength, double longestwavelength, double precision, std::vector<double> &lambdas, std::vector<int> &count);
    void docalib();
    void changestepsize(double angle); //Change stepsize in degree
    std::vector< double> fitgaus(std::string filename);
    double stepstowavelength(int steps);
    int compensatecounts(int steps, int counts); //uses empirical fit to compensate the wavelength dependent efficiency of the components
    int scanandplot(double shortestwavelength, double longestwavelength, double precision);
    int savedataas(std::string nameoffile);
    void automatic_calibration(double initial_w, double final_w, double res);


private:

    int predictposition(double wavelength);
    //double getangle(double l);
    int searcharea; //area around predicted peak position in which peak will be searched for
    std::string calibpath; //where calibration file will looked for/go to
    //std::vector<double> wavelengths;
    //std::vector<int> positions;
    int motnum = 0;
    stepper_motor * step_mot_point;
    counting * counting_point;
    unsigned long integtime=50000;
    //int maxtimesl; //needs to be initialized with proper value

    double blazingangle = M_PI*((28.68)/180); // in Rad
    double gratingconstant =( 1.66666*pow(10,3)); //inverse of( distance between two lines in nm)
    int stepsize;

    std::vector < double> coeff_poly = {-269.685, -0.0069944, -0.00000000631545};
    std::vector < double> power_poly = {1};
    std::vector <double> wavelength;
    std::vector <int> count;


signals:

public slots:
};

#endif // SPECTROMETER_H
