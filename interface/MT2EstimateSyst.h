#ifndef MT2EstimateSyst_h
#define MT2EstimateSyst_h


#include "MT2Estimate.h"






class MT2EstimateSyst : public MT2Estimate {

 public:

  MT2EstimateSyst( const MT2EstimateSyst& rhs ) : MT2Estimate( rhs ) {
     yield_btagUp   = new TH1D(*(rhs.yield_btagUp  ));
     yield_btagDown = new TH1D(*(rhs.yield_btagDown));
     yield_btagUp  ->Sumw2();
     yield_btagDown->Sumw2();
   }

  MT2EstimateSyst( const std::string& aname, const MT2Region& aregion );
  ~MT2EstimateSyst();
 
  TH1D* yield_btagUp;
  TH1D* yield_btagDown;

  MT2EstimateSyst operator+( const MT2EstimateSyst& rhs ) const;

  virtual void addOverflow();

  //virtual void writeTable( const std::string& fileName ) const;

 private:

};





#endif
