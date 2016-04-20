#include "spectrom.h"

spectrom::spectrom(QWidget *parent) : QWidget(parent)
{

}

double spectrom::stepstowavelength(int steps){

    double wavelength=0;

    for ( int i=0; i<coeff_poly.size(); i++) {

        wavelength += coeff_poly[i]*pow(steps,i);
    }

    return wavelength;
}

//
