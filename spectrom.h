#ifndef SPECTROM_H
#define SPECTROM_H

#include <vector>
#include <QWidget>
//This class will be used for the calculation of the diffraction and for the calibration

//The calibration will be done in this way.. Once the wavelength is provided the program scans all the angles for the maximum intensity..
//The angle is recorded and associated with that wavel
//Then a fit is performed with a poly and the coefficients are extracted.. The results and the resiudual are plotted for check

//The reconstruction is simply done using the fitted function

using namespace std;
class spectrom : public QWidget
{
    Q_OBJECT
public:
    explicit spectrom(QWidget *parent = 0);
    double spect_function();


private:
    vector < vector< double> > calib;
    vector < double> coeff_poly;



signals:

public slots:
};

#endif // SPECTROM_H
