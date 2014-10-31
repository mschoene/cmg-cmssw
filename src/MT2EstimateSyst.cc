#include "../interface/MT2EstimateSyst.h"

#include <iostream>
#include <cmath>




MT2EstimateSyst::MT2EstimateSyst( const std::string& aname, const MT2Region& aregion ) : MT2Estimate( aname, aregion ) {

  int nBins;
  double* bins;
  region->getBins(nBins, bins);

  yield_btagUp = new TH1D(Form("yield_btagUp_%s", name.c_str()), "", nBins, bins);
  yield_btagUp->Sumw2();
  yield_btagDown = new TH1D(Form("yield_btagDown_%s", name.c_str()), "", nBins, bins);
  yield_btagDown->Sumw2();

}



MT2EstimateSyst::~MT2EstimateSyst() {

  delete yield_btagUp;
  delete yield_btagDown;

}



void MT2EstimateSyst::addOverflow() {

  MT2Estimate::addOverflowSingleHisto( yield );
  MT2Estimate::addOverflowSingleHisto( yield_btagUp );
  MT2Estimate::addOverflowSingleHisto( yield_btagDown );

}




MT2EstimateSyst MT2EstimateSyst::operator+( const MT2EstimateSyst& rhs ) const {


  if( this->region != rhs.region ) {
    std::cout << "[MT2EstimateSyst::operator+] ERROR! Can't add MT2EstimateSyst with different MT2Regions!" << std::endl;
    exit(113);
  }

  std::string newname = this->name + "_" + rhs.name;

  MT2EstimateSyst result(newname, *(this->region) );

  result.yield->Add(this->yield);
  result.yield->Add(rhs.yield);

  result.yield_btagUp->Add(this->yield_btagUp);
  result.yield_btagUp->Add(rhs.yield_btagUp);

  result.yield_btagDown->Add(this->yield_btagDown);
  result.yield_btagDown->Add(rhs.yield_btagDown);

  return result;

}



