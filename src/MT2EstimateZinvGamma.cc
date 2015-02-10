#include "../interface/MT2EstimateZinvGamma.h"

#include <iostream>
#include <iomanip>
#include <fstream>
#include <cmath>





MT2EstimateZinvGamma::MT2EstimateZinvGamma( const std::string& aname, const MT2Region& aregion ) : MT2Estimate( aname, aregion ) {

  int nbins = 25;
  float xmax = 1.;

  // this histo will be used to create histogram templates:
  iso = new TH1D( this->getHistoName("iso").c_str(), "", nbins, 0., xmax);
  iso->Sumw2();

  x_ = new RooRealVar( "x", "", 0., xmax );

  int nbins_mt2;
  double *bins_mt2;
  aregion.getBins( nbins_mt2, bins_mt2 );


  for( unsigned i=0; i<nbins_mt2; ++i ) {

    RooDataSet* isoDataset = new RooDataSet( this->getHistoName(Form("iso_bin%d", i)).c_str(), "", *x_ );
    iso_bins.push_back(isoDataset);

    TH1D* this_iso_hist = new TH1D( this->getHistoName(Form("iso_bin%d_hist", i)).c_str() , "", nbins, 0., xmax);
    this_iso_hist->Sumw2();
    iso_bins_hist.push_back(this_iso_hist);

  }

}




MT2EstimateZinvGamma::MT2EstimateZinvGamma( const MT2EstimateZinvGamma& rhs ) : MT2Estimate(rhs) {

  this->iso = new TH1D(*(rhs.iso));

  for( unsigned i=0; i<rhs.iso_bins.size(); ++i ) {
    RooDataSet* newDataSet = new RooDataSet( *(rhs.iso_bins[i]) );
    this->iso_bins.push_back(newDataSet);
    TH1D* this_iso_hist = new TH1D( *(rhs.iso_bins_hist[i]) );
    iso_bins_hist.push_back(this_iso_hist);
  }

}



// destructor

MT2EstimateZinvGamma::~MT2EstimateZinvGamma() {

  delete iso;

  for( unsigned i=0; i<iso_bins.size(); ++i ) { 
    delete iso_bins[i];
    delete iso_bins_hist[i];
  }


}



void MT2EstimateZinvGamma::setName( const std::string& newName ) {

  MT2Estimate::setName(newName);

  iso->SetName( this->getHistoName("iso").c_str() );

  for( unsigned i=0; i<iso_bins.size(); ++i ) {
    iso_bins[i]->SetName( this->getHistoName(Form("iso_bin%d", i)).c_str() );
    iso_bins_hist[i]->SetName( this->getHistoName(Form("iso_bin%d_hist", i)).c_str() );
  }

}



void MT2EstimateZinvGamma::fillIso( float iso, float weight, float mt2 ) {

  this->iso->Fill( iso, weight );


  if( mt2>0. ) {
    int foundBin = this->yield->FindBin(mt2);
    if( foundBin > this->yield->GetNbinsX() ) foundBin=this->yield->GetNbinsX(); // overflow will go in last bin
    foundBin-=1; // want first bin to be 0 (fuck you root)
    if( foundBin>=0 ) {
      x_->setVal(iso);
      iso_bins[foundBin]->add( *x_, weight );
      iso_bins_hist[foundBin]->Fill( iso, weight );
    }
  }
      
}
 
 


void MT2EstimateZinvGamma::finalize() {

  MT2Estimate::addOverflow();

  MT2Estimate::addOverflowSingleHisto( iso );

  for( unsigned i=0; i<iso_bins.size(); ++i ) {
    MT2Estimate::addOverflowSingleHisto(iso_bins_hist[i]);
  }

}



void MT2EstimateZinvGamma::getShit( TFile* file, const std::string& path ) {

  MT2Estimate::getShit(file, path);
  iso = (TH1D*)file->Get(Form("%s/%s", path.c_str(), iso->GetName()));

  for( unsigned i=0; i<iso_bins.size(); ++i ) {
    iso_bins[i] = (RooDataSet*)file->Get(Form("%s/%s", path.c_str(), iso_bins[i]->GetName()));
    iso_bins_hist[i] = (TH1D*)file->Get(Form("%s/%s", path.c_str(), iso_bins_hist[i]->GetName()));
  }


}


void MT2EstimateZinvGamma::print(const std::string& ofs){

  MT2Estimate::print( ofs );

}



void MT2EstimateZinvGamma::write() const {

  MT2Estimate::write();

  iso->Write();

  for( unsigned i=0; i<iso_bins.size(); ++i ) {
    iso_bins[i]->Write();
    iso_bins_hist[i]->Write();
  }

}



const MT2EstimateZinvGamma& MT2EstimateZinvGamma::operator=( const MT2EstimateZinvGamma& rhs ) {

  if( this->iso == 0 ) { // first time

    this->setName(rhs.getName());

    this->region = new MT2Region(*(rhs.region));

    this->yield = new TH1D(*(rhs.yield));

    this->iso = new TH1D(*(rhs.iso));

    for( unsigned i=0; i<iso_bins.size(); ++i ) {
      this->iso_bins[i] = new RooDataSet( *(rhs.iso_bins[i]) );
      this->iso_bins_hist[i] = new TH1D( *(rhs.iso_bins_hist[i]) );
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


    for( unsigned i=0; i<iso_bins.size(); ++i ) {

      std::string oldName_bin = this->iso_bins[i]->GetName();
      this->iso_bins[i] = new RooDataSet( *(rhs.iso_bins[i]) );
      this->iso_bins[i]->SetName( oldName_bin.c_str() );

      std::string oldName_bin_hist = this->iso_bins_hist[i]->GetName();
      this->iso_bins_hist[i] = new TH1D( *(rhs.iso_bins_hist[i]) );
      this->iso_bins_hist[i]->SetName( oldName_bin_hist.c_str() );

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

  for( unsigned i=0; i<iso_bins.size(); ++i ) {

    result.iso_bins[i]->append( *(rhs.iso_bins[i]) );
    result.iso_bins_hist[i]->Add( rhs.iso_bins_hist[i] );

  }
  
  return result;

}




/*
MT2EstimateZinvGamma MT2EstimateZinvGamma::operator/( const MT2EstimateZinvGamma& rhs ) const{


  if( *(this->region) != *(rhs.region) ) {
    std::cout << "[MT2EstimateZinvGamma::operator/] ERROR! Can't divide MT2EstimateZinvGamma with different MT2Regions!" << std::endl;
    exit(113);
  }

  MT2EstimateZinvGamma result(*this);
  result.yield->Divide(rhs.yield);

  result.iso->Divide(rhs.iso);

  for( unsigned i=0; i<iso_bins.size(); ++i ) {
    result.iso_bins[i]->Divide(rhs.iso_bins[i]);
  }
  
  return result;

}
*/


const MT2EstimateZinvGamma& MT2EstimateZinvGamma::operator+=( const MT2EstimateZinvGamma& rhs ) {

  this->yield->Add(rhs.yield);

  this->iso->Add(rhs.iso);

  for( unsigned i=0; i<iso_bins.size(); ++i ) {

    this->iso_bins[i]->append( *(rhs.iso_bins[i]) );
    this->iso_bins_hist[i]->Add( rhs.iso_bins_hist[i] );

  }

  return (*this);

}



/*
MT2EstimateZinvGamma MT2EstimateZinvGamma::operator/=( const MT2EstimateZinvGamma& rhs ) const {

  this->yield->Divide(rhs.yield);

  this->iso->Divide(rhs.iso);

  for( unsigned i=0; i<iso_bins.size(); ++i ) {
    this->iso_bins[i]->Divide(rhs.iso_bins[i]);
  }

  return (*this);

}
*/

