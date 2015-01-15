#include "../interface/MT2EstimateZinvGamma.h"

#include <iostream>
#include <iomanip>
#include <fstream>
#include <cmath>




MT2EstimateZinvGamma::MT2EstimateZinvGamma( const std::string& aname, const MT2Region& aregion ) : MT2Estimate( aname, aregion ) {

  int nbins = 200;
  float xmax = 50.;

  template_prompt = new TH1D( this->getHistoName("template_prompt").c_str(), "", nbins, 0., xmax);
  template_prompt->Sumw2();
  template_fake = new TH1D( this->getHistoName("template_fake").c_str(), "", nbins, 0., xmax);
  template_fake->Sumw2();
  template_unmatched = new TH1D( this->getHistoName("template_unmatched").c_str(), "", nbins, 0., xmax);
  template_unmatched->Sumw2();

}



MT2EstimateZinvGamma::~MT2EstimateZinvGamma() {

  delete template_prompt;
  delete template_fake;
  delete template_unmatched;

}



void MT2EstimateZinvGamma::setName( const std::string& newName ) {

  MT2Estimate::setName(newName);

  template_prompt->SetName( this->getHistoName("template_prompt").c_str() );
  template_fake->SetName( this->getHistoName("template_fake").c_str() );
  template_unmatched->SetName( this->getHistoName("template_unmatched").c_str() );

}



void MT2EstimateZinvGamma::finalize() {

  MT2Estimate::addOverflow();

  //MT2Estimate::addOverflowSingleHisto( template_prompt );
  //MT2Estimate::addOverflowSingleHisto( template_fake );
  //MT2Estimate::addOverflowSingleHisto( template_unmatched );

  float int_prompt = template_prompt->Integral("width");
  float int_fake = template_fake->Integral("width");
  float int_unmatched = template_unmatched->Integral("width");

  if( int_prompt>0. ) template_prompt->Scale( 1./int_prompt );
  if( int_fake>0. ) template_fake->Scale( 1./int_fake );
  if( int_unmatched>0. ) template_unmatched->Scale( 1./int_unmatched );

}



void MT2EstimateZinvGamma::getShit( TFile* file, const std::string& path ) {

  MT2Estimate::getShit(file, path);
  template_prompt = (TH1D*)file->Get(Form("%s/%s", path.c_str(), template_prompt->GetName()));
  template_fake = (TH1D*)file->Get(Form("%s/%s", path.c_str(), template_fake->GetName()));
  template_unmatched = (TH1D*)file->Get(Form("%s/%s", path.c_str(), template_unmatched->GetName()));


}


void MT2EstimateZinvGamma::print(const std::string& ofs){

  MT2Estimate::print( ofs );

}



void MT2EstimateZinvGamma::write() const {

    MT2Estimate::write();

    template_prompt->Write();
    template_fake->Write();
    template_unmatched->Write();

}



const MT2EstimateZinvGamma& MT2EstimateZinvGamma::operator=( const MT2EstimateZinvGamma& rhs ) {

  if( this->template_prompt == 0 ) { // first time

    this->setName(rhs.getName());

    this->region = new MT2Region(*(rhs.region));

    this->yield = new TH1D(*(rhs.yield));
    this->template_prompt = new TH1D(*(rhs.template_prompt));
    this->template_fake = new TH1D(*(rhs.template_fake));
    this->template_unmatched = new TH1D(*(rhs.template_unmatched));

  } else { // keep name and histo name, just make histogram identical

    if( this->region!=0 ) delete this->region;
    this->region = new MT2Region(*(rhs.region));

    std::string oldName = this->yield->GetName();
    delete this->yield;
    this->yield = new TH1D(*(rhs.yield));
    this->yield->SetName(oldName.c_str());

    std::string oldName_prompt = this->template_prompt->GetName();
    delete this->template_prompt;
    this->template_prompt = new TH1D(*(rhs.template_prompt));
    this->template_prompt->SetName(oldName_prompt.c_str());

    std::string oldName_fake = this->template_fake->GetName();
    delete this->template_fake;
    this->template_fake = new TH1D(*(rhs.template_fake));
    this->template_fake->SetName(oldName_fake.c_str());

    std::string oldName_unmatched = this->template_unmatched->GetName();
    delete this->template_unmatched;
    this->template_unmatched = new TH1D(*(rhs.template_unmatched));
    this->template_unmatched->SetName(oldName_unmatched.c_str());


  }

  return *this;

}




MT2EstimateZinvGamma MT2EstimateZinvGamma::operator+( const MT2EstimateZinvGamma& rhs ) const{


  if( *(this->region) != *(rhs.region) ) {
    std::cout << "[MT2EstimateZinvGamma::operator+] ERROR! Can't add MT2EstimateZinvGamma with different MT2Regions!" << std::endl;
    exit(113);
  }

  //MT2EstimateZinvGamma result(*this);
  //result.yield->Add(rhs.yield);
  //result.yield_btagUp->Add(rhs.yield_btagUp);
  //result.yield_btagDown->Add(rhs.yield_btagDown);
  
  this->yield->Add(rhs.yield);
  this->template_prompt->Add(rhs.template_prompt);
  this->template_fake->Add(rhs.template_fake);
  this->template_unmatched->Add(rhs.template_unmatched);
  
  return *this;
  //return result;

}


MT2EstimateZinvGamma MT2EstimateZinvGamma::operator/( const MT2EstimateZinvGamma& rhs ) const{


  if( *(this->region) != *(rhs.region) ) {
    std::cout << "[MT2EstimateZinvGamma::operator/] ERROR! Can't divide MT2EstimateZinvGamma with different MT2Regions!" << std::endl;
    exit(113);
  }

  //MT2EstimateZinvGamma result(*this);
  //result.yield->Add(rhs.yield);
  //result.yield_btagUp->Add(rhs.yield_btagUp);
  //result.yield_btagDown->Add(rhs.yield_btagDown);
  
  this->yield->Divide(rhs.yield);
  this->template_prompt->Divide(rhs.template_prompt);
  this->template_fake->Divide(rhs.template_fake);
  this->template_unmatched->Divide(rhs.template_unmatched);
  
  return *this;
  //return result;

}



MT2EstimateZinvGamma MT2EstimateZinvGamma::operator+=( const MT2EstimateZinvGamma& rhs ) const {

  return (*this) + rhs ;

}

MT2EstimateZinvGamma MT2EstimateZinvGamma::operator/=( const MT2EstimateZinvGamma& rhs ) const {

  return (*this) / rhs ;

}


