#ifndef MT2EstimateZinvGamma_h
#define MT2EstimateZinvGamma_h

#include "MT2Estimate.h"

#include <iostream>
#include <vector>


class MT2EstimateZinvGamma : public MT2Estimate {

 public:

  MT2EstimateZinvGamma( const MT2EstimateZinvGamma& rhs );
  MT2EstimateZinvGamma( const std::string& aname, const MT2Region& aregion );
  ~MT2EstimateZinvGamma();

  virtual void setName( const std::string& newName );
 
  // integrated over mt2:
  TH1D* iso;
  TH1D* iso_prompt;
  TH1D* iso_fake;

  // for each bin of mt2:
  std::vector<TH1D*> iso_bins;
  std::vector<TH1D*> iso_prompt_bins;
  std::vector<TH1D*> iso_fake_bins; 


  const MT2EstimateZinvGamma& operator=( const MT2EstimateZinvGamma& rhs );
  MT2EstimateZinvGamma operator+( const MT2EstimateZinvGamma& rhs ) const;
  MT2EstimateZinvGamma operator/( const MT2EstimateZinvGamma& rhs ) const;
  MT2EstimateZinvGamma operator+=( const MT2EstimateZinvGamma& rhs ) const;
  MT2EstimateZinvGamma operator/=( const MT2EstimateZinvGamma& rhs ) const;

  //MT2EstimateZinvGamma operator/ ( float k ) const;
  //MT2EstimateZinvGamma operator* ( float k ) const;
  //MT2EstimateZinvGamma operator/=( float k ) const;
  //MT2EstimateZinvGamma operator*=( float k ) const;


  void fillIso( float iso, float weight=1., int mcMatchId=-1, float mt2=-1 );

  virtual void finalize();

  virtual void getShit( TFile* file, const std::string& path );

  virtual void write() const;

  virtual void print(const std::string& ofs);

 private:

};





#endif
