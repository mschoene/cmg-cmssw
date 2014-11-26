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

  MT2Estimate::addOverflow();

  MT2Estimate::addOverflowSingleHisto( yield_btagUp );
  MT2Estimate::addOverflowSingleHisto( yield_btagDown );

}



void MT2EstimateSyst::getShit( TFile* file, const std::string& path ) {

  MT2Estimate::getShit(file, path);
  yield_btagUp = (TH1D*)file->Get(Form("%s/%s", path.c_str(), yield_btagUp->GetName()));
  yield_btagDown = (TH1D*)file->Get(Form("%s/%s", path.c_str(), yield_btagDown->GetName()));


}



const MT2EstimateSyst& MT2EstimateSyst::operator=( const MT2EstimateSyst& rhs ) {

  if( this->yield == 0 ) { // first time

    this->name = rhs.name;

    this->region = new MT2Region(*(rhs.region));

    this->yield = new TH1D(*(rhs.yield));
    this->yield_btagUp = new TH1D(*(rhs.yield_btagUp));
    this->yield_btagDown = new TH1D(*(rhs.yield_btagDown));

  } else { // keep name and histo name, just make histogram identical

    if( this->region!=0 ) delete this->region;
    this->region = new MT2Region(*(rhs.region));

    std::string oldName = this->yield->GetName();
    delete this->yield;
    this->yield = new TH1D(*(rhs.yield));
    this->yield->SetName(oldName.c_str());

    std::string oldName_btagUp = this->yield_btagUp->GetName();
    delete this->yield_btagUp;
    this->yield_btagUp = new TH1D(*(rhs.yield_btagUp));
    this->yield_btagUp->SetName(oldName_btagUp.c_str());

    std::string oldName_btagDown = this->yield_btagDown->GetName();
    delete this->yield_btagDown;
    this->yield_btagDown = new TH1D(*(rhs.yield_btagDown));
    this->yield_btagDown->SetName(oldName_btagDown.c_str());

  }

  return *this;

}




MT2EstimateSyst MT2EstimateSyst::operator+( const MT2EstimateSyst& rhs ) const{


  if( *(this->region) != *(rhs.region) ) {
    std::cout << "[MT2EstimateSyst::operator+] ERROR! Can't add MT2EstimateSyst with different MT2Regions!" << std::endl;
    exit(113);
  }

  //MT2EstimateSyst result(*this);
  //result.yield->Add(rhs.yield);
  //result.yield_btagUp->Add(rhs.yield_btagUp);
  //result.yield_btagDown->Add(rhs.yield_btagDown);
  
  this->yield->Add(rhs.yield);
  this->yield_btagUp->Add(rhs.yield_btagUp);
  this->yield_btagDown->Add(rhs.yield_btagDown);
  
  return *this;
  //return result;

}


MT2EstimateSyst MT2EstimateSyst::operator/( const MT2EstimateSyst& rhs ) const{


  if( *(this->region) != *(rhs.region) ) {
    std::cout << "[MT2EstimateSyst::operator/] ERROR! Can't divide MT2EstimateSyst with different MT2Regions!" << std::endl;
    exit(113);
  }

  //MT2EstimateSyst result(*this);
  //result.yield->Add(rhs.yield);
  //result.yield_btagUp->Add(rhs.yield_btagUp);
  //result.yield_btagDown->Add(rhs.yield_btagDown);
  
  this->yield->Divide(rhs.yield);
  this->yield_btagUp->Divide(rhs.yield_btagUp);
  this->yield_btagDown->Divide(rhs.yield_btagDown);
  
  return *this;
  //return result;

}


MT2EstimateSyst MT2EstimateSyst::operator*( const MT2EstimateSyst& rhs ) const{


  if( *(this->region) != *(rhs.region) ) {
    std::cout << "[MT2EstimateSyst::operator*] ERROR! Can't multiply MT2EstimateSyst with different MT2Regions!" << std::endl;
    exit(113);
  }

  //MT2EstimateSyst result(*this);
  //result.yield->Add(rhs.yield);
  //result.yield_btagUp->Add(rhs.yield_btagUp);
  //result.yield_btagDown->Add(rhs.yield_btagDown);
  
  this->yield->Multiply(rhs.yield);
  this->yield_btagUp->Multiply(rhs.yield_btagUp);
  this->yield_btagDown->Multiply(rhs.yield_btagDown);
  
  return *this;
  //return result;

}


MT2EstimateSyst MT2EstimateSyst::operator+=( const MT2EstimateSyst& rhs ) const {

  return (*this) + rhs ;

}

MT2EstimateSyst MT2EstimateSyst::operator/=( const MT2EstimateSyst& rhs ) const {

  return (*this) / rhs ;

}

MT2EstimateSyst MT2EstimateSyst::operator*=( const MT2EstimateSyst& rhs ) const {

  return (*this) * rhs ;

}

