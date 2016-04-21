


double calibrate::stepstowavelength(int steps){

    double wavelength=0;

    for ( int i=0; i<coeff_poly.size(); i++) {

        wavelength += coeff_poly[i]*pow(steps,i);
    }

    return wavelength;
}

int calibrate::compensatecounts(int steps, int counts){

    double compensation=0;

    for ( int i=0; i<power_poly.size(); i++) {

        compensation += power_poly[i]*pow(steps,i);
    }

    return (counts/compensation);
}


int calibrate::scanandplot(double shortestwavelength, double longestwavelength, double precision) {

    vector <int> position;
    scan(shortestwavelength,longestwavelength,precision,position,count);
    wavelength = {};

    for(int i = 0; i < position.size(); i++){

        wavelength.push_back(stepstowavelength(position[i]));
        count[i] = compensatecounts(position[i],count[i]);
    }
}

int calibrate::savedataas( string nameoffile ) {

    ofstream fileout;
    fileout.open(nameoffile,std::ofstream::out);

    if(fileout.is_open()) {

        return 1;
        qDebug() << "unable to open file";
    }

    for(int i = 0; i < wavelength.size(); i++) {

        fileout << wavelength[i] << " " << count[i] << endl;
    }
}

//
