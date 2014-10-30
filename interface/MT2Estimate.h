#ifndef MT2Estimate_h
#define MT2Estimate_h


#include "MT2Region.h"
#include "TH1D.h"




// this is the basic Estimate class: 
// it refers to a region, and it has a yield histogram
// other more complex classes (like LostLepton estimates)
// should inherit from this one, 
// and add further specialized data members



class MT2Estimate {

 public:

  MT2Estimate( const MT2Estimate& rhs );  
  MT2Estimate( const std::string& aname, const MT2Region& aregion );
  ~MT2Estimate();
 
  // this is just a name to differentiate different
  // instances of the same class
  std::string name;

  // the region it refers to
  MT2Region* region;

  // the main data member: the yield histogram
  // classes that inherit from this one will add other data members
  TH1D* yield;


  void getBins( int& nBins, double* bins ) const {
    return region->getBins(nBins, bins);
  }

  MT2HTRegion* htRegion() const {
    return region->htRegion();
  }
  MT2SignalRegion* sigRegion() const {
    return region->sigRegion();
  }

  // this is a univocal identifier of the region
  // regions with the same definition (jet numbers and HT cuts)
  // have the same name
  std::string regionName() const {
    return region->getName();
  }

  MT2Estimate operator+( const MT2Estimate& rhs ) const;

  virtual void addOverflow();
  void addOverflowSingleHisto( TH1D* yield );

  //virtual void writeTable( const std::string& fileName ) const;

 private:

};





#endif
