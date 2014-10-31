#include "../interface/MT2Estimate.h"

#include <iostream>
#include <cmath>




MT2Estimate::MT2Estimate( const MT2Estimate& rhs ) {

  MT2Estimate( rhs.name, *(rhs.region) );

}



MT2Estimate::MT2Estimate( const std::string& aname, const MT2Region& aregion ) {

  name = aname;
  region = new MT2Region(aregion);

  int nBins;
  double* bins;
  region->getBins(nBins, bins);

  yield = new TH1D(Form("yield_%s", name.c_str()), "", nBins, bins);
  yield->Sumw2();

}



MT2Estimate::~MT2Estimate() {

  delete region;
  delete yield;

}



void MT2Estimate::addOverflow() {

  MT2Estimate::addOverflowSingleHisto( yield );

}


void MT2Estimate::addOverflowSingleHisto( TH1D* yield ) {

  yield->SetBinContent(yield->GetNbinsX(),
      yield->GetBinContent(yield->GetNbinsX()  )+
      yield->GetBinContent(yield->GetNbinsX()+1)  );
  yield->SetBinError(  yield->GetNbinsX(),
      sqrt(yield->GetBinError(yield->GetNbinsX()  )*
           yield->GetBinError(yield->GetNbinsX()  )+
           yield->GetBinError(yield->GetNbinsX()+1)*
           yield->GetBinError(yield->GetNbinsX()+1)  ));

}




MT2Estimate MT2Estimate::operator+( const MT2Estimate& rhs ) const {


  if( this->region != rhs.region ) {
    std::cout << "[MT2Estimate::operator+] ERROR! Can't add MT2SingleLLEstimate with different MT2Regions!" << std::endl;
    exit(113);
  }

  std::string newname = this->name + "_" + rhs.name;

  MT2Estimate result(newname, *(this->region) );

  result.yield->Add(this->yield);
  result.yield->Add(rhs.yield);

  return result;

}



