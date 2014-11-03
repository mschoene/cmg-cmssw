#ifndef MT2Analysis_h
#define MT2Analysis_h


#include <vector>
#include <string>
#include <set>
#include <iostream>
#include "MT2Region.h"
#include "TFile.h"





  
template<class T> 
class MT2Analysis {

 public:

  MT2Analysis( const std::string& aname, const std::string& regionsSet, int id=-1 );
  MT2Analysis( const std::string& aname, std::vector<MT2HTRegion> htRegions, std::vector<MT2SignalRegion> signalRegions, int id=-1 );
  MT2Analysis( const MT2Analysis& rhs );
  ~MT2Analysis() {};

  std::vector<MT2HTRegion> getHTRegions() const { return htRegions_; };
  std::vector<MT2SignalRegion> getSignalRegions() const { return signalRegions_; };

  MT2Region* getRegion( float ht, int njets, int nbjets, float met );
  T* get( MT2Region* r ) const;
  T* get( float ht, int njets, int nbjets, float met ) {
    return this->get( this->getRegion(ht, njets, nbjets, met ) );
  }


  const MT2Analysis& operator=( const MT2Analysis& rhs);
  MT2Analysis operator+( const MT2Analysis& rhs);
  void add( const MT2Analysis& rhs );


  virtual void writeToFile( const std::string& fileName );
  virtual void addOverflow();

  std::string name;
  int id;


  std::set<T*> data;
  //std::map< MT2Region, T*> data;


 private:

  void createAnalysisStructure() {

    for( unsigned iHT=0; iHT<htRegions_.size(); ++iHT ) {
      for( unsigned iSR=0; iSR<signalRegions_.size(); ++iSR ) {
        MT2Region thisRegion(&htRegions_[iHT], &signalRegions_[iSR]);
        T* t = new T(name + "_" + thisRegion.getName(), thisRegion);
        data.insert(t);
      }
    }
    
  }

  std::vector<MT2HTRegion> htRegions_;
  std::vector<MT2SignalRegion> signalRegions_;

};










template<class T> 
MT2Analysis<T>::MT2Analysis( const std::string& aname, const std::string& regionsSet, int aid ) {


  name = aname;
  id = aid;


  if( regionsSet=="13TeV" ) {

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

    std::cout << "[MT2Analysis::MT2Analysis] Analysis region set '" << regionsSet << "' not implemented yet. Exiting." << std::endl;
    exit(917);

  }

  this->createAnalysisStructure();

}




template<class T> 
MT2Analysis<T>::MT2Analysis( const std::string& aname, std::vector<MT2HTRegion> htRegions, std::vector<MT2SignalRegion> signalRegions, int aid ) {

  name = aname;
  id = aid;

  htRegions_ = htRegions;
  signalRegions_ = signalRegions;

  this->createAnalysisStructure();

}



template<class T> 
MT2Analysis<T>::MT2Analysis( const MT2Analysis& rhs ) {

  htRegions_ = rhs.getHTRegions();
  signalRegions_ = rhs.getSignalRegions();

  name = rhs.name;
  id = rhs.id;

  this->createAnalysisStructure();

}




template<class T>
MT2Region* MT2Analysis<T>::getRegion( float ht, int njets, int nbjets, float met ) {

  MT2Region* region = 0;
  
  for( typename std::set<T*>::iterator it=data.begin(); it!=data.end(); ++it ) {

    float htMin  = (*it)->region->htRegion()->htMin;
    float htMax  = (*it)->region->htRegion()->htMax;
    float metMin = (*it)->region->htRegion()->metMin;

    if( ht<htMin || ht>htMax ) continue;
    if( met<metMin ) continue;

    int njetsmin  = (*it)->region->sigRegion()->nJetsMin;
    int njetsmax  = (*it)->region->sigRegion()->nJetsMax;
    int nbjetsmin = (*it)->region->sigRegion()->nBJetsMin;
    int nbjetsmax = (*it)->region->sigRegion()->nBJetsMax;

    if( njetsmin >0 && njets <njetsmin  ) continue;
    if( njetsmax >0 && njets >njetsmax  ) continue;
    if( nbjetsmin>0 && nbjets<nbjetsmin ) continue;
    if( nbjetsmax>0 && nbjets>nbjetsmax ) continue;

    region = (*it)->region;

  }  // for

  return region;

}



template<class T>
T* MT2Analysis<T>::get( MT2Region* r ) const {

  T* t = 0;

  for( typename std::set<T*>::iterator it=data.begin(); it!=data.end(); ++it ) {
    if( (*it)->region == r ) {
      t = *it;
      break;
    }
  }

  return t;

}


template<class T> 
const MT2Analysis<T>& MT2Analysis<T>::operator=( const MT2Analysis& rhs ) {

  htRegions_ = rhs.getHTRegions();
  signalRegions_ = rhs.getSignalRegions();

  return *this;

}



template<class T> 
MT2Analysis<T> MT2Analysis<T>::operator+( const MT2Analysis& rhs ) {

  htRegions_ = rhs.getHTRegions();
  signalRegions_ = rhs.getSignalRegions();

  for( unsigned iHT=0; iHT<htRegions_.size(); ++iHT ) {

    for( unsigned iSR=0; iSR<signalRegions_.size(); ++iSR ) {

      MT2Region* thisRegion = new MT2Region( &(htRegions_[iHT]), &(signalRegions_[iSR]) );
      T* t1 = this->get(thisRegion); 
      T* t2 = rhs.get(thisRegion); 
      if( t2==0 ) {
        std::cout << "[MT2Analysis::operator+] ERROR! Can't add MT2Analysis with different regional structures!" << std::endl;
        exit(111);
      }
      *t1 = *t1 + *t2;

    }

  }


  return *this;

}



template<class T>
void MT2Analysis<T>::add( const MT2Analysis& rhs ) {

  (*this) = (*this) + rhs;

}




template<class T> 
void MT2Analysis<T>::writeToFile( const std::string& fileName ) {

  TFile* file = TFile::Open(fileName.c_str(), "recreate");
  file->cd();

  std::vector<MT2HTRegion> htRegions = this->getHTRegions();
  for( std::vector<MT2HTRegion>::iterator it=htRegions.begin(); it!=htRegions.end(); ++it ) {
    file->cd();
    file->mkdir(it->name.c_str());
  }

  
  for( typename std::set<T*>::iterator it=data.begin(); it!=data.end(); ++it ) {
    file->cd();
    file->cd((*it)->region->htRegion()->name.c_str());
    (*it)->write();
  }

  file->Close();

  std::cout << "-> Written data to file: " << fileName << std::endl;

}



template<class T> 
void MT2Analysis<T>::addOverflow() {

  for( typename std::set<T*>::iterator it=data.begin(); it!=data.end(); ++it ) 
    (*it)->addOverflow();

}


#endif
