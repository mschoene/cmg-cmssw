#ifndef MT2EstimateSyst_h
#define MT2EstimateSyst_h

#include "MT2Estimate.h"

#include <iostream>


class MT2EstimateSyst : public MT2Estimate {

 public:

  MT2EstimateSyst( const MT2EstimateSyst& rhs ) : MT2Estimate(rhs) {
    this->yield_btagUp = new TH1D(*(rhs.yield_btagUp));
    this->yield_btagDown = new TH1D(*(rhs.yield_btagDown));
  }
  MT2EstimateSyst( const std::string& aname, const MT2Region& aregion );
  ~MT2EstimateSyst();

  virtual void setName( const std::string& newName );
 
  TH1D* yield_btagUp;
  TH1D* yield_btagDown;

  const MT2EstimateSyst& operator=( const MT2EstimateSyst& rhs );
  MT2EstimateSyst operator+( const MT2EstimateSyst& rhs ) const;
  MT2EstimateSyst operator/( const MT2EstimateSyst& rhs ) const;
  MT2EstimateSyst operator*( const MT2EstimateSyst& rhs ) const;
  MT2EstimateSyst operator+=( const MT2EstimateSyst& rhs ) const;
  MT2EstimateSyst operator/=( const MT2EstimateSyst& rhs ) const;
  MT2EstimateSyst operator*=( const MT2EstimateSyst& rhs ) const;

  MT2EstimateSyst operator/ ( float k ) const;
  MT2EstimateSyst operator* ( float k ) const;
  MT2EstimateSyst operator/=( float k ) const;
  MT2EstimateSyst operator*=( float k ) const;

  virtual void addOverflow();

  virtual void getShit( TFile* file, const std::string& path );

  virtual void write() const {
    MT2Estimate::write();
    yield_btagUp->Write();
    yield_btagDown->Write();
  }

  virtual void print(const std::string& ofs);

 private:

};





#endif
