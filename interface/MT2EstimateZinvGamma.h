#ifndef MT2EstimateZinvGamma_h
#define MT2EstimateZinvGamma_h

#include "MT2Estimate.h"

#include <iostream>


class MT2EstimateZinvGamma : public MT2Estimate {

 public:

  MT2EstimateZinvGamma( const MT2EstimateZinvGamma& rhs ) : MT2Estimate(rhs) {
    this->template_prompt = new TH1D(*(rhs.template_prompt));
    this->template_fake = new TH1D(*(rhs.template_fake));
    this->template_unmatched = new TH1D(*(rhs.template_unmatched));
  }
  MT2EstimateZinvGamma( const std::string& aname, const MT2Region& aregion );
  ~MT2EstimateZinvGamma();

  virtual void setName( const std::string& newName );
 
  TH1D* template_prompt;
  TH1D* template_fake;
  TH1D* template_unmatched;

  const MT2EstimateZinvGamma& operator=( const MT2EstimateZinvGamma& rhs );
  MT2EstimateZinvGamma operator+( const MT2EstimateZinvGamma& rhs ) const;
  MT2EstimateZinvGamma operator/( const MT2EstimateZinvGamma& rhs ) const;
  MT2EstimateZinvGamma operator+=( const MT2EstimateZinvGamma& rhs ) const;
  MT2EstimateZinvGamma operator/=( const MT2EstimateZinvGamma& rhs ) const;

  //MT2EstimateZinvGamma operator/ ( float k ) const;
  //MT2EstimateZinvGamma operator* ( float k ) const;
  //MT2EstimateZinvGamma operator/=( float k ) const;
  //MT2EstimateZinvGamma operator*=( float k ) const;


  virtual void finalize();

  virtual void getShit( TFile* file, const std::string& path );

  virtual void write() const;

  virtual void print(const std::string& ofs);

 private:

};





#endif
