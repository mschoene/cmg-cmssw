#include "../interface/MT2EstimateZinvGamma.h"

#include <iostream>
#include <iomanip>
#include <fstream>
#include <cmath>




MT2EstimateZinvGamma::MT2EstimateZinvGamma( const std::string& aname, const MT2Region& aregion ) : MT2Estimate( aname, aregion ) {

  int nbins = 30;
  float xmax = 1.2;

  iso = new TH1D( this->getHistoName("iso").c_str(), "", nbins, 0., xmax);
  iso->Sumw2();
  iso_prompt = new TH1D( this->getHistoName("iso_prompt").c_str(), "", nbins, 0., xmax);
  iso_prompt->Sumw2();
  iso_fake = new TH1D( this->getHistoName("iso_fake").c_str(), "", nbins, 0., xmax);
  iso_fake->Sumw2();

  int nbins_mt2;
  double *bins_mt2;
  aregion.getBins( nbins_mt2, bins_mt2 );

  for( unsigned i=0; i<nbins_mt2; ++i ) {
    TH1D* this_iso = new TH1D( this->getHistoName(Form("iso_bin%d", i)).c_str() , "", nbins, 0., xmax);
    this_iso->Sumw2();
    iso_bins.push_back(this_iso);
    TH1D* this_prompt = new TH1D( this->getHistoName(Form("iso_prompt_bin%d", i)).c_str() , "", nbins, 0., xmax);
    this_prompt->Sumw2();
    iso_prompt_bins.push_back(this_prompt);
    TH1D* this_fake = new TH1D( this->getHistoName(Form("iso_fake_bin%d", i)).c_str() , "", nbins, 0., xmax);
    this_fake->Sumw2();
    iso_fake_bins.push_back(this_fake);
  }

}




MT2EstimateZinvGamma::MT2EstimateZinvGamma( const MT2EstimateZinvGamma& rhs ) : MT2Estimate(rhs) {

  this->iso = new TH1D(*(rhs.iso));
  this->iso_prompt = new TH1D(*(rhs.iso_prompt));
  this->iso_fake = new TH1D(*(rhs.iso_fake));

  for( unsigned i=0; i<rhs.iso_bins.size(); ++i ) {
    this->iso_bins.push_back(new TH1D(*(rhs.iso_bins[i])));
    this->iso_prompt_bins.push_back(new TH1D(*(rhs.iso_prompt_bins[i])));
    this->iso_fake_bins.push_back(new TH1D(*(rhs.iso_fake_bins[i])));
  }

}


MT2EstimateZinvGamma::~MT2EstimateZinvGamma() {

  delete iso;
  delete iso_prompt;
  delete iso_fake;

  for( unsigned i=0; i<iso_bins.size(); ++i ) delete iso_bins[i];
  for( unsigned i=0; i<iso_prompt_bins.size(); ++i ) delete iso_prompt_bins[i];
  for( unsigned i=0; i<iso_fake_bins.size(); ++i ) delete iso_fake_bins[i];


}



void MT2EstimateZinvGamma::setName( const std::string& newName ) {

  MT2Estimate::setName(newName);

  iso->SetName( this->getHistoName("iso").c_str() );
  iso_prompt->SetName( this->getHistoName("iso_prompt").c_str() );
  iso_fake->SetName( this->getHistoName("iso_fake").c_str() );

  for( unsigned i=0; i<iso_bins.size(); ++i ) {
    iso_bins[i]->SetName( this->getHistoName(Form("iso_bin%d", i)).c_str() );
    iso_prompt_bins[i]->SetName( this->getHistoName(Form("iso_prompt_bin%d", i)).c_str() );
    iso_fake_bins[i]->SetName( this->getHistoName(Form("iso_fake_bin%d", i)).c_str() );
  }

}



void MT2EstimateZinvGamma::fillIso( float iso, float weight, int mcMatchId, float mt2 ) {

  this->iso->Fill( iso, weight );

  if( mcMatchId==22 ) this->iso_prompt->Fill( iso, weight );
  else if( mcMatchId==0 ) this->iso_fake->Fill( iso, weight );

  if( mt2>0. ) {
    int foundBin = this->yield->FindBin(mt2);
    if( foundBin > this->yield->GetNbinsX() ) foundBin=this->yield->GetNbinsX(); // overflow will go in last bin
    foundBin-=1; // want first bin to be 0 (fuck you root)
    if( foundBin>=0 ) {
      iso_bins[foundBin]->Fill( iso, weight );
      if( mcMatchId==22 ) this->iso_prompt_bins[foundBin]->Fill( iso, weight );
      else if( mcMatchId==0 ) this->iso_fake_bins[foundBin]->Fill( iso, weight );
    }
  }
      
}
 
 


void MT2EstimateZinvGamma::finalize() {

  MT2Estimate::addOverflow();

  MT2Estimate::addOverflowSingleHisto( iso );
  MT2Estimate::addOverflowSingleHisto( iso_prompt );
  MT2Estimate::addOverflowSingleHisto( iso_fake );

  for( unsigned i=0; i<iso_bins.size(); ++i ) {
    MT2Estimate::addOverflowSingleHisto(iso_bins[i]);
    MT2Estimate::addOverflowSingleHisto(iso_prompt_bins[i]);
    MT2Estimate::addOverflowSingleHisto(iso_fake_bins[i]);
  }

  //float int_prompt = iso_prompt->Integral("width");
  //float int_fake = iso_fake->Integral("width");

  //if( int_prompt>0. ) iso_prompt->Scale( 1./int_prompt );
  //if( int_fake>0. ) iso_fake->Scale( 1./int_fake );

}



void MT2EstimateZinvGamma::getShit( TFile* file, const std::string& path ) {

  MT2Estimate::getShit(file, path);
  iso = (TH1D*)file->Get(Form("%s/%s", path.c_str(), iso->GetName()));
  iso_prompt = (TH1D*)file->Get(Form("%s/%s", path.c_str(), iso_prompt->GetName()));
  iso_fake = (TH1D*)file->Get(Form("%s/%s", path.c_str(), iso_fake->GetName()));

  for( unsigned i=0; i<iso_bins.size(); ++i ) {
    iso_bins[i] = (TH1D*)file->Get(Form("%s/%s", path.c_str(), iso_bins[i]->GetName()));
    iso_prompt_bins[i] = (TH1D*)file->Get(Form("%s/%s", path.c_str(), iso_prompt_bins[i]->GetName()));
    iso_fake_bins[i] = (TH1D*)file->Get(Form("%s/%s", path.c_str(), iso_fake_bins[i]->GetName()));
  }


}


void MT2EstimateZinvGamma::print(const std::string& ofs){

  MT2Estimate::print( ofs );

}



void MT2EstimateZinvGamma::write() const {

  MT2Estimate::write();

  iso->Write();
  iso_prompt->Write();
  iso_fake->Write();

  for( unsigned i=0; i<iso_bins.size(); ++i ) {
    iso_bins[i]->Write();
    iso_prompt_bins[i]->Write();
    iso_fake_bins[i]->Write();
  }

}



const MT2EstimateZinvGamma& MT2EstimateZinvGamma::operator=( const MT2EstimateZinvGamma& rhs ) {

  if( this->iso == 0 ) { // first time

    this->setName(rhs.getName());

    this->region = new MT2Region(*(rhs.region));

    this->yield = new TH1D(*(rhs.yield));

    this->iso = new TH1D(*(rhs.iso));
    this->iso_prompt = new TH1D(*(rhs.iso_prompt));
    this->iso_fake = new TH1D(*(rhs.iso_fake));

    for( unsigned i=0; i<iso_bins.size(); ++i ) {
      this->iso_bins[i] = new TH1D( *(rhs.iso_bins[i]) );
      this->iso_prompt_bins[i] = new TH1D( *(rhs.iso_prompt_bins[i]) );
      this->iso_fake_bins[i] = new TH1D( *(rhs.iso_fake_bins[i]) );
    }


  } else { // keep name and histo name, just make histogram identical

    if( this->region!=0 ) delete this->region;
    this->region = new MT2Region(*(rhs.region));

    std::string oldName = this->yield->GetName();
    delete this->yield;
    this->yield = new TH1D(*(rhs.yield));
    this->yield->SetName(oldName.c_str());

    std::string oldName_iso = this->iso->GetName();
    delete this->iso;
    this->iso = new TH1D(*(rhs.iso));
    this->iso->SetName(oldName_iso.c_str());

    std::string oldName_prompt = this->iso_prompt->GetName();
    delete this->iso_prompt;
    this->iso_prompt = new TH1D(*(rhs.iso_prompt));
    this->iso_prompt->SetName(oldName_prompt.c_str());

    std::string oldName_fake = this->iso_fake->GetName();
    delete this->iso_fake;
    this->iso_fake = new TH1D(*(rhs.iso_fake));
    this->iso_fake->SetName(oldName_fake.c_str());

    for( unsigned i=0; i<iso_bins.size(); ++i ) {

      std::string oldName_bin = this->iso_bins[i]->GetName();
      this->iso_bins[i] = new TH1D( *(rhs.iso_bins[i]) );
      this->iso_bins[i]->SetName(oldName_bin.c_str());

      std::string oldName_prompt_bin = this->iso_prompt_bins[i]->GetName();
      this->iso_prompt_bins[i] = new TH1D( *(rhs.iso_prompt_bins[i]) );
      this->iso_prompt_bins[i]->SetName(oldName_prompt_bin.c_str());

      std::string oldName_fake_bin = this->iso_fake_bins[i]->GetName();
      this->iso_fake_bins[i] = new TH1D( *(rhs.iso_fake_bins[i]) );
      this->iso_fake_bins[i]->SetName(oldName_fake_bin.c_str());

    }


  }

  return *this;

}




MT2EstimateZinvGamma MT2EstimateZinvGamma::operator+( const MT2EstimateZinvGamma& rhs ) const{


  if( *(this->region) != *(rhs.region) ) {
    std::cout << "[MT2EstimateZinvGamma::operator+] ERROR! Can't add MT2EstimateZinvGamma with different MT2Regions!" << std::endl;
    exit(113);
  }

  MT2EstimateZinvGamma result(*this);
  result.yield->Add(rhs.yield);

  result.iso->Add(rhs.iso);
  result.iso_prompt->Add(rhs.iso_prompt);
  result.iso_fake->Add(rhs.iso_fake);

  for( unsigned i=0; i<iso_bins.size(); ++i ) {
    result.iso_bins[i]->Add(rhs.iso_bins[i]);
    result.iso_prompt_bins[i]->Add(rhs.iso_prompt_bins[i]);
    result.iso_fake_bins[i]->Add(rhs.iso_fake_bins[i]);
  }
  
  return result;

}




MT2EstimateZinvGamma MT2EstimateZinvGamma::operator/( const MT2EstimateZinvGamma& rhs ) const{


  if( *(this->region) != *(rhs.region) ) {
    std::cout << "[MT2EstimateZinvGamma::operator/] ERROR! Can't divide MT2EstimateZinvGamma with different MT2Regions!" << std::endl;
    exit(113);
  }

  MT2EstimateZinvGamma result(*this);
  result.yield->Divide(rhs.yield);

  result.iso->Divide(rhs.iso);
  result.iso_prompt->Divide(rhs.iso_prompt);
  result.iso_fake->Divide(rhs.iso_fake);

  for( unsigned i=0; i<iso_bins.size(); ++i ) {
    result.iso_bins[i]->Divide(rhs.iso_bins[i]);
    result.iso_prompt_bins[i]->Divide(rhs.iso_prompt_bins[i]);
    result.iso_fake_bins[i]->Divide(rhs.iso_fake_bins[i]);
  }
  
  return result;

}



MT2EstimateZinvGamma MT2EstimateZinvGamma::operator+=( const MT2EstimateZinvGamma& rhs ) const {

  this->yield->Add(rhs.yield);

  this->iso->Add(rhs.iso);
  this->iso_prompt->Add(rhs.iso_prompt);
  this->iso_fake->Add(rhs.iso_fake);

  for( unsigned i=0; i<iso_bins.size(); ++i ) {
    this->iso_bins[i]->Add(rhs.iso_bins[i]);
    this->iso_prompt_bins[i]->Add(rhs.iso_prompt_bins[i]);
    this->iso_fake_bins[i]->Add(rhs.iso_fake_bins[i]);
  }

  return (*this);

}

MT2EstimateZinvGamma MT2EstimateZinvGamma::operator/=( const MT2EstimateZinvGamma& rhs ) const {

  this->yield->Divide(rhs.yield);

  this->iso->Divide(rhs.iso);
  this->iso_prompt->Divide(rhs.iso_prompt);
  this->iso_fake->Divide(rhs.iso_fake);

  for( unsigned i=0; i<iso_bins.size(); ++i ) {
    this->iso_bins[i]->Divide(rhs.iso_bins[i]);
    this->iso_prompt_bins[i]->Divide(rhs.iso_prompt_bins[i]);
    this->iso_fake_bins[i]->Divide(rhs.iso_fake_bins[i]);
  }

  return (*this);

}


