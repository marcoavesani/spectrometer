#include "spectrometer.h"
#include <cmath>
#include <QDebug>
using namespace std;
spectrometer::spectrometer(stepper_motor * step, counting * count, QWidget *parent) : QWidget(parent)
{

    step_mot_point=step;
    counting_point=count;
    stepsize = int(0.00171892/360*(step->getnumbofsteps()));
    searcharea=300*(0.00171892/360*(step->getnumbofsteps()));
    motnum=0;
    step_mot_point->go(0,0);
    step_mot_point->go(1,0);
    step_mot_point->go(2,0);


}

spectrometer::~spectrometer(){
    step_mot_point->go(0,0);
    step_mot_point->go(1,0);
    step_mot_point->go(2,0);
}
int spectrometer::firstpeak(double atwavelength) {
    //step_mot_point->set(motnum,0);
    step_mot_point->go(motnum,predictposition(atwavelength));
    qDebug()<< predictposition(atwavelength);

    /*int events=0;
    counting_point->getcount(events, integtime);
    qDebug()<<events;*/

    std::ofstream fileout;
    fileout.open("./fileout",std::ofstream::out | std::ofstream::app);
    fileout <<atwavelength<< " "<<(predictposition(atwavelength)/step_mot_point->getnumbofsteps()*360)<<endl;



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

int spectrometer::predictposition(double iswavelength){
    //returns in steps. if you make a new spectroscope linear model should be sufficient

return (-2593400+3390.29*iswavelength+-1.28168*pow(iswavelength,2));
}

int spectrometer::addpeak(double atwavelength) {
    qDebug()<<atwavelength;
    std::ofstream fileout;
    fileout.open("./fileout",std::ofstream::out | std::ofstream::app);
    qDebug() << "filout:" << fileout.is_open();
    fileout << "peak at " << atwavelength << "nm" << std::endl;

    std::ofstream forplot;
    forplot.open("./forplot",std::ofstream::out);
    qDebug() << "forplot:" << forplot.is_open();

    int position = predictposition(atwavelength);
    double maxcount = 0;
    int maxcountposition = 0;
    //step_mot_point->set(motnum, position-(searcharea/2));

    for(int i = 0; i*stepsize<searcharea; i++) {

        int events = 0;
        step_mot_point->go(motnum, position+i*stepsize-searcharea/2);
        sleep(0.1);
        counting_point->getcount(events, integtime);
        qDebug()<<events;
        maxcountposition = (maxcount>events?(position+i*stepsize-searcharea/2):maxcountposition);
        maxcount = (maxcount>events?maxcount:events);
        forplot<<(position+i*stepsize-searcharea/2)<<" "<<events<<std::endl;

    }
     qDebug()<<maxcount;
    //positions.push_back(360*(double)maxcountposition/(double)step_mot_point->getnumbofsteps());
    //wavelengths.push_back(atwavelength);

    fileout<<atwavelength<<'/t'<<360*(double)maxcountposition/(double)step_mot_point->getnumbofsteps()<<std::endl;
    fileout.close();
    forplot.close();
}

int spectrometer::scan(double shortestwavelength, double longestwavelength, double precision, vector <double> &lambdas, vector <int> &count){

    lambdas, count = {}; //make sure vectors are empty


    double temp = shortestwavelength;

    if(shortestwavelength > longestwavelength) {shortestwavelength = longestwavelength; longestwavelength = temp;}

    for(double i = shortestwavelength; i <= longestwavelength; i += precision ) { //scan and fill the vectors

        step_mot_point->go(motnum, predictposition(i));
        lambdas.push_back(i);

        int events;
        counting_point->getcount(events, integtime);
        count.push_back(events);
    }
}





void spectrometer::changestepsize(double angle) {

    stepsize = (int)angle/360*step_mot_point->getnumbofsteps();
}

/*double spectrometer::getangle(double l){


    return acos((4*gratingconstant*l*cos(blazingangle)*sin(blazingangle) +
                 4*sqrt(2*pow(gratingconstant,4)*pow(sin(blazingangle),4) - pow(gratingconstant,2)*pow(l,2)*pow(sin(blazingangle),4) -
                    2*pow(gratingconstant,4)*cos(2*blazingangle)*pow(sin(blazingangle),4)))/
               (2.*pow(gratingconstant,2)*(1 - 2*pow(cos(blazingangle),2) + pow(cos(blazingangle),4) + 2*pow(sin(blazingangle),2) +
                   2*pow(cos(blazingangle),2)*pow(sin(blazingangle),2) + pow(sin(blazingangle),4))));
}*/




double spectrometer::stepstowavelength(int steps){

    double iswavelength=0;

    for ( int i=0; i<coeff_poly.size(); i++) {

        iswavelength += coeff_poly[i]*pow(steps,i);
    }

    return iswavelength;
}

int spectrometer::compensatecounts(int steps, int counts){

    double compensation=0;

    for ( int i=0; i<power_poly.size(); i++) {

        compensation += power_poly[i]*pow(steps,i);
    }

    return (counts/compensation);
}


int spectrometer::scanandplot(double shortestwavelength, double longestwavelength, double precision) {

    wavelength = {};
    scan(shortestwavelength,longestwavelength,precision,wavelength,count);


    for(int i = 0; i < wavelength.size(); i++){

        count[i] = compensatecounts(predictposition(wavelength[i]),count[i]);
    }
}

int spectrometer::savedataas( std::string nameoffile ) {

    ofstream fileout;
    fileout.open(nameoffile,std::ofstream::out);

    if(!fileout.is_open()) {

        return 1;
        qDebug() << "unable to open file";
    }

    for(int i = 0; i < wavelength.size(); i++) {

        fileout << wavelength[i] << " " << count[i] << endl;
    }
}

vector<double> spectrometer::fitgaus(string filename){

  //gStyle->SetY(0);
  ifstream filein_gaus;
  filein_gaus.open(filename.c_str());
  double steps_temp,count_temp;
  vector <double> step_v;
  vector <double > count_v;
  vector<double> result;
  if(filein_gaus.is_open()){
   while(!(filein_gaus.eof())){
     if (!filein_gaus.good()){
       break;
     }
     filein_gaus>>steps_temp;
     filein_gaus>>count_temp;
     step_v.push_back(steps_temp);
     count_v.push_back(count_temp);
     std::cout<<steps_temp<<endl;
   }
    auto max_i= max_element(std::begin(step_v), std::end(step_v));
    auto min_i = min_element(std::begin(step_v), std::end(step_v)); // c++11
    long max=*max_i;
    long min=*min_i;
    std::string res="res_"+filename;
    TH1D * histo= new TH1D(filename.c_str(),filename.c_str(),step_v.size()-2,min,max);
    TH1D * residuals= new TH1D( res.c_str(),res.c_str(),step_v.size()-2,min,max);
    //histo->SetDefaultSumw2(kFALSE);
    //residuals->SetDefaultSumw2(kFALSE);

    TF1 gaus("gaus","gaus(0)",min,max);
    TF1 gaus_back("gaus_back","gaus(0)+pol0(3)",min,max);
    for(int i=0; i<step_v.size();i++){
      histo->SetBinContent(histo->FindBin(step_v[i]),count_v[i]);
    }


   histo->Fit(&gaus);
   gaus_back.SetParameters(gaus.GetParameter(0),gaus.GetParameter(1),gaus.GetParameter(2),1);
   histo->Fit(&gaus_back,"MIE");
   TFile file("hsimple.root","update");
   for(int i=0; i<histo->GetNbinsX();i++){
      residuals->SetBinContent(i,(histo->GetBinContent(i)-gaus_back(histo->GetBinCenter(i))));
    }
   histo->Write();
   residuals->Write();
   for(int i=0;i<4;i++){
    double temp=0;
    temp=gaus_back.GetParameter(i);
    result.push_back(temp);
   }
    file.Close();
  }

   return result;

}
