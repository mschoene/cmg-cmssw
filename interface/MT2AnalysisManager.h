#ifndef MT2AnalysisManager_h
#define MT2AnalysisManager_h


#include <vector>
#include <map>
#include "MT2Region.h"






class MT2AnalysisManager {

 public:

  MT2AnalysisManager( const std::string& regionsSet );
  MT2AnalysisManager( std::vector<MT2HTRegion> htRegions, std::vector<MT2SignalRegion> signalRegions );
  MT2AnalysisManager( const MT2AnalysisManager& rhs );
  ~MT2AnalysisManager();

  std::vector<MT2HTRegion> getHTRegions() const { return htRegions_; };
  std::vector<MT2SignalRegion> getSignalRegions() const { return signalRegions_; };

  MT2Region* getRegion( float ht, int njets, int nbjets, float met );
  
  template<class T> 
  std::map< MT2Region, T*> createAnalysisEstimates( const std::string& name ) {

    std::map< MT2Region, T*> theMap;

    for( unsigned iHT=0; iHT<htRegions_.size(); ++iHT ) {
      for( unsigned iSR=0; iSR<signalRegions_.size(); ++iSR ) {
        MT2Region thisRegion(&htRegions_[iHT], &signalRegions_[iSR]);
        T* t = new T(name + "_" + thisRegion.getName(), thisRegion);
        theMap[thisRegion] = t;
      }
    }
    
    return theMap;

  }


  MT2AnalysisManager operator=( const MT2AnalysisManager );


 private:

  std::vector<MT2HTRegion> htRegions_;
  std::vector<MT2SignalRegion> signalRegions_;

};


#endif
