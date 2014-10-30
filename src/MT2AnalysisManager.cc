#include "../interface/MT2AnalysisManager.h"

#include <iostream>




MT2AnalysisManager::MT2AnalysisManager( const std::string& regionsSet ) {


  if( regionsSet=="default" ) {

    htRegions_.push_back(MT2HTRegion("lowHT",    450.,    575., 0.));
    htRegions_.push_back(MT2HTRegion("mediumHT", 575.,   1000., 0.));
    htRegions_.push_back(MT2HTRegion("highHT",  1000., 100000., 0.));

    signalRegions_.push_back(MT2SignalRegion(2,  3, 0,  0)); 
    signalRegions_.push_back(MT2SignalRegion(4, -1, 0,  0)); 
    signalRegions_.push_back(MT2SignalRegion(2,  3, 1,  2)); 
    signalRegions_.push_back(MT2SignalRegion(4, -1, 1,  2)); 
    signalRegions_.push_back(MT2SignalRegion(2, -1, 3, -1)); 


  } else if( regionsSet=="8TeV" ) {

    htRegions_.push_back(MT2HTRegion("lowHT",    450.,    750., 200., "" ));
    htRegions_.push_back(MT2HTRegion("mediumHT", 750.,   1200.,  30., "" ));
    htRegions_.push_back(MT2HTRegion("highHT",  1200., 100000.,  30., "" ));

    signalRegions_.push_back(MT2SignalRegion(2, 2, 0, 0));  // 2j0b
    signalRegions_.push_back(MT2SignalRegion(2, 2, 1, 2));  // 2j1to2b
    signalRegions_.push_back(MT2SignalRegion(3, 5, 0, 0));  // 3to5j0b
    signalRegions_.push_back(MT2SignalRegion(3, 5, 1, 1));  // 3to5j1b
    signalRegions_.push_back(MT2SignalRegion(3, 5, 2, 2));  // 3to5j2b
    signalRegions_.push_back(MT2SignalRegion(6, -1, 0, 0));  // 6j0b
    signalRegions_.push_back(MT2SignalRegion(6, -1, 1, 1));  // 6j1b
    signalRegions_.push_back(MT2SignalRegion(6, -1, 2, 2));  // 6j2b
    signalRegions_.push_back(MT2SignalRegion(-1, -1, 3, -1));  // 3b

  } else {

    std::cout << "[MT2AnalysisManager::MT2AnalysisManager] Analysis region set '" << regionsSet << "' not implemented yet. Exiting." << std::endl;
    exit(917);

  }


}




MT2AnalysisManager::MT2AnalysisManager( std::vector<MT2HTRegion> htRegions, std::vector<MT2SignalRegion> signalRegions ) {

  htRegions_ = htRegions;
  signalRegions_ = signalRegions;

}



MT2AnalysisManager::MT2AnalysisManager( const MT2AnalysisManager& rhs ) {

  htRegions_ = rhs.getHTRegions();
  signalRegions_ = rhs.getSignalRegions();

}


MT2AnalysisManager::~MT2AnalysisManager() {};



MT2Region* MT2AnalysisManager::getRegion( float ht, int njets, int nbjets, float met ) {

  int iHT=-1;
  int iSR=-1;

  for( unsigned i=0; i<htRegions_.size(); ++i ) {

    if( ht >= htRegions_[i].htMin && ht < htRegions_[i].htMax && met > htRegions_[i].metMin ) {

      iHT = i;

      for( unsigned j=0; j<signalRegions_.size(); ++j ) {

        int nJetsMin = signalRegions_[j].nJetsMin;
        int nJetsMax = signalRegions_[j].nJetsMax;
        int nBJetsMin = signalRegions_[j].nBJetsMin;
        int nBJetsMax = signalRegions_[j].nBJetsMax;

        if( njets >= nJetsMin && njets <= nJetsMax && nbjets >= nBJetsMin && nbjets <= nBJetsMax ) {

          iSR = j;
          break;

        }

      } // for signal regions

    } // if HT region

  } // for HT regions


  if( iHT<0 || iSR<0 ) {

    return 0;

  }



  MT2Region* region = new MT2Region( &htRegions_[iHT], &signalRegions_[iSR] ); 

  return region;

}



MT2AnalysisManager MT2AnalysisManager::operator=( MT2AnalysisManager rhs ) {

  htRegions_ = rhs.getHTRegions();
  signalRegions_ = rhs.getSignalRegions();

  return *this;

}


