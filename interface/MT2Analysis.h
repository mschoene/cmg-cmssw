#ifndef MT2Analysis_h
#define MT2Analysis_h


#include <string>
#include <set>
#include <iostream>
#include "MT2Region.h"
#include "TFile.h"






  
template<class T> 
class MT2Analysis {

 public:

  MT2Analysis( const std::string& aname, const std::string& regionsSet="13TeV", int id=-1 );
  MT2Analysis( const std::string& aname, std::set<MT2HTRegion> htRegions, std::set<MT2SignalRegion> signalRegions, int id=-1 );
  MT2Analysis( const MT2Analysis& rhs );
  ~MT2Analysis() {};

  std::set<MT2HTRegion> getHTRegions() const { return htRegions_; };
  std::set<MT2SignalRegion> getSignalRegions() const { return signalRegions_; };

  MT2Region* getRegion( float ht, int njets, int nbjets, float met );
  T* get( MT2Region* r ) const;
  T* get( float ht, int njets, int nbjets, float met ) {
    return this->get( this->getRegion(ht, njets, nbjets, met ) );
  }


  const MT2Analysis& operator=( const MT2Analysis& rhs);
  MT2Analysis operator+( const MT2Analysis& rhs);
  void add( const MT2Analysis& rhs );
  MT2Analysis operator/( const MT2Analysis& rhs);
  void divide( const MT2Analysis& rhs );
  MT2Analysis operator*( const MT2Analysis& rhs);


  static MT2Analysis* readFromFile( const std::string& fileName, const std::string& matchName="" );
  static std::set<MT2Analysis*> readAllFromFile( const std::string& fileName );
  void writeToFile( const std::string& fileName );

  void finalize();

  std::string name;
  int id;


  std::set<T*> data;
  //std::map< MT2Region, T*> data;


 private:

  void createAnalysisStructure() {

    for( std::set<MT2HTRegion>::iterator iHT=htRegions_.begin(); iHT!=htRegions_.end(); ++iHT ) {
      for( std::set<MT2SignalRegion>::iterator iSR=signalRegions_.begin(); iSR!=signalRegions_.end(); ++iSR ) {
        MT2Region thisRegion(&(*iHT), &(*iSR));
        T* t = new T(name + "_" + thisRegion.getName(), thisRegion);
        data.insert(t);
      }
    }
    
  }

  std::set<MT2HTRegion> htRegions_;
  std::set<MT2SignalRegion> signalRegions_;

};









template<class T> 
MT2Analysis<T>::MT2Analysis( const std::string& aname, const std::string& regionsSet, int aid ) {


  name = aname;
  id = aid;


  if( regionsSet=="13TeV" ) {

    htRegions_.insert(MT2HTRegion( 450.,   575., 0.));
    htRegions_.insert(MT2HTRegion( 575.,  1000., 0.));
    htRegions_.insert(MT2HTRegion(1000.,    -1., 0.));

    signalRegions_.insert(MT2SignalRegion(2,  3, 0,  0)); 
    signalRegions_.insert(MT2SignalRegion(4, -1, 0,  0)); 
    signalRegions_.insert(MT2SignalRegion(2,  3, 1,  2)); 
    signalRegions_.insert(MT2SignalRegion(4, -1, 1,  2)); 
    signalRegions_.insert(MT2SignalRegion(2, -1, 3, -1)); 


  } else if( regionsSet=="8TeV" ) {

    htRegions_.insert(MT2HTRegion( 450.,  750., 200., "" ));
    htRegions_.insert(MT2HTRegion( 750., 1200.,  30., "" ));
    htRegions_.insert(MT2HTRegion(1200.,   -1.,  30., "" ));

    signalRegions_.insert(MT2SignalRegion(2, 2, 0, 0));  // 2j0b
    signalRegions_.insert(MT2SignalRegion(2, 2, 1, 2));  // 2j1to2b
    signalRegions_.insert(MT2SignalRegion(3, 5, 0, 0));  // 3to5j0b
    signalRegions_.insert(MT2SignalRegion(3, 5, 1, 1));  // 3to5j1b
    signalRegions_.insert(MT2SignalRegion(3, 5, 2, 2));  // 3to5j2b
    signalRegions_.insert(MT2SignalRegion(6, -1, 0, 0));  // 6j0b
    signalRegions_.insert(MT2SignalRegion(6, -1, 1, 1));  // 6j1b
    signalRegions_.insert(MT2SignalRegion(6, -1, 2, 2));  // 6j2b
    signalRegions_.insert(MT2SignalRegion(-1, -1, 3, -1));  // 3b

  } else {

    std::cout << "[MT2Analysis::MT2Analysis] Analysis region set '" << regionsSet << "' not implemented yet. Exiting." << std::endl;
    exit(917);

  }

  this->createAnalysisStructure();

}




template<class T> 
MT2Analysis<T>::MT2Analysis( const std::string& aname, std::set<MT2HTRegion> htRegions, std::set<MT2SignalRegion> signalRegions, int aid ) {

  name = aname;
  id = aid;

  for( std::set<MT2HTRegion>::iterator iHT=htRegions.begin(); iHT!=htRegions.end(); ++iHT ) 
    htRegions_.insert(*iHT);

  for( std::set<MT2SignalRegion>::iterator iSR=signalRegions.begin(); iSR!=signalRegions.end(); ++iSR ) 
    signalRegions_.insert(*iSR);

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

    if( ht<htMin ) continue;
    if( htMax>0. && ht>htMax ) continue;
    if( met<metMin ) continue;

    int njetsmin  = (*it)->region->sigRegion()->nJetsMin;
    int njetsmax  = (*it)->region->sigRegion()->nJetsMax;
    int nbjetsmin = (*it)->region->sigRegion()->nBJetsMin;
    int nbjetsmax = (*it)->region->sigRegion()->nBJetsMax;

    if( njetsmin >=0 && njets <njetsmin  ) continue;
    if( njetsmax >=0 && njets >njetsmax  ) continue;
    if( nbjetsmin>=0 && nbjets<nbjetsmin ) continue;
    if( nbjetsmax>=0 && nbjets>nbjetsmax ) continue;

    region = (*it)->region;
    break;

  }  // for

  return region;

}



template<class T>
T* MT2Analysis<T>::get( MT2Region* r ) const {

  if( r==0 ) return 0;

  T* t = 0;

  for( typename std::set<T*>::iterator it=data.begin(); it!=data.end(); ++it ) {
    if( *((*it)->region) == *r ) {
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

  for( std::set<MT2HTRegion>::iterator iHT=htRegions_.begin(); iHT!=htRegions_.end(); ++iHT ) {
    for( std::set<MT2SignalRegion>::iterator iSR=signalRegions_.begin(); iSR!=signalRegions_.end(); ++iSR ) {

      MT2Region thisRegion(&(*iHT), &(*iSR));

      T* t1 = this->get(&thisRegion); 
      T* t2 = rhs.get(&thisRegion); 
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
MT2Analysis<T> MT2Analysis<T>::operator/( const MT2Analysis& rhs ) {

  htRegions_ = rhs.getHTRegions();
  signalRegions_ = rhs.getSignalRegions();

  for( std::set<MT2HTRegion>::iterator iHT=htRegions_.begin(); iHT!=htRegions_.end(); ++iHT ) {
    for( std::set<MT2SignalRegion>::iterator iSR=signalRegions_.begin(); iSR!=signalRegions_.end(); ++iSR ) {

      MT2Region thisRegion(&(*iHT), &(*iSR));

      T* t1 = this->get(&thisRegion); 
      T* t2 = rhs.get(&thisRegion); 
      if( t2==0 ) {
        std::cout << "[MT2Analysis::operator/] ERROR! Can't divide MT2Analysis with different regional structures!" << std::endl;
        exit(111);
      }
      *t1 = *t1 / *t2;

    }

  }


  return *this;

}



template<class T>
void MT2Analysis<T>::divide( const MT2Analysis& rhs ) {

  (*this) = (*this) / rhs;

}




template<class T> 
MT2Analysis<T> MT2Analysis<T>::operator*( const MT2Analysis& rhs ) {

  htRegions_ = rhs.getHTRegions();
  signalRegions_ = rhs.getSignalRegions();

  for( std::set<MT2HTRegion>::iterator iHT=htRegions_.begin(); iHT!=htRegions_.end(); ++iHT ) {
    for( std::set<MT2SignalRegion>::iterator iSR=signalRegions_.begin(); iSR!=signalRegions_.end(); ++iSR ) {

      MT2Region thisRegion(&(*iHT), &(*iSR));

      T* t1 = this->get(&thisRegion); 
      T* t2 = rhs.get(&thisRegion); 
      if( t2==0 ) {
        std::cout << "[MT2Analysis::operator*] ERROR! Can't add MT2Analysis with different regional structures!" << std::endl;
        exit(111);
      }
      *t1 = (*t1) * (*t2);

    }

  }


  return *this;

}



template<class T> 
void MT2Analysis<T>::writeToFile( const std::string& fileName ) {

  TFile* file = TFile::Open(fileName.c_str(), "recreate");
  file->cd();

  file->mkdir(this->name.c_str());
  file->cd(this->name.c_str());

  std::set<MT2HTRegion> htRegions = this->getHTRegions();
  for( std::set<MT2HTRegion>::iterator it=htRegions.begin(); it!=htRegions.end(); ++it ) {
    file->cd(this->name.c_str());
    file->mkdir(Form("%s/%s", this->name.c_str(), it->getName().c_str()) );
  }

  
  for( typename std::set<T*>::iterator it=data.begin(); it!=data.end(); ++it ) {
    file->cd();
    file->cd(Form("%s/%s", this->name.c_str(), (*it)->region->htRegion()->getName().c_str()) );
    (*it)->write();
  }

  file->Close();

  std::cout << "-> Written data to file: " << fileName << std::endl;

}



template<class T> 
std::set<MT2Analysis<T>*> MT2Analysis<T>::readAllFromFile( const std::string& fileName ) {

  TFile* file = TFile::Open( fileName.c_str(), "READ" );

  if( file==0 ) {
   std::cout << "[MT2Analysis::readAllFromFile] Didn't find file: " << fileName << "!! Exiting" << std::endl;
   exit(733);
  }


  std::set<MT2Analysis<T>*> analyses;


  TIter next(file->GetListOfKeys());

  // these are the uppermost dirs in the file, 
  // so one dir per analysis name (tyipically only one anyways)
  while(TObject *obj = next()) { 

    std::string analysisName(obj->GetName());
    file->cd(analysisName.c_str()); 

    std::set<MT2HTRegion> htRegions;
    std::set<MT2SignalRegion> sigRegions;

    bool firstIteration = true;

    TIter next2(gDirectory->GetListOfKeys());

    // these are the directiories inside the analysis dir
    // there will be one directory per HT region
    // and the dir name is the HT region identifying name
    while(TObject *obj2 = next2()) { 

      std::string htRegionName(obj2->GetName());
      MT2HTRegion htRegion(htRegionName);
      htRegions.insert(htRegion);

      
      if( firstIteration ) {

        std::string path = analysisName + "/" + htRegionName;
        file->cd(path.c_str());

        TList* hists = gDirectory->GetListOfKeys();
        TIter next3(hists);

        // these are the histos inside the HT region folder
        // the name format is: [prefix]_[name]_[HTregion]_[signalRegion]
        // so we now have to find the signal regions
        // (to do it once is enough as for now the format is that
        // each HT region has the same jet/bjet regions)
        while(TObject *obj3 = next3()) { 

          std::string name_str(obj3->GetName());
          std::size_t foundPos = name_str.find(htRegionName);
          std::string signalRegionName(name_str.begin()+foundPos+htRegionName.size()+1, name_str.end());
          MT2SignalRegion sigRegion(signalRegionName);
          sigRegions.insert(sigRegion);

        } 

        firstIteration=false;

      } // if firstIteration

      file->cd(analysisName.c_str());

    } // while ht regions

    // now that we know name and region structure we can istantiate an MT2Analysis:
    MT2Analysis<T>* analysis = new MT2Analysis<T>( analysisName, htRegions, sigRegions );    

    // second loop to set everything
    file->cd(analysisName.c_str()); 
    TIter nextAgain(gDirectory->GetListOfKeys());

    while(TObject *obj2 = nextAgain()) { // loop on ht regions

      std::string htRegionName(obj2->GetName());
      MT2HTRegion htRegion(htRegionName);
      
      std::string path = analysisName + "/" + htRegionName;
      file->cd(path.c_str());


      std::set<MT2SignalRegion> sigRegions = analysis->getSignalRegions();

      for( std::set<MT2SignalRegion>::iterator iSR=sigRegions.begin(); iSR!=sigRegions.end(); ++iSR ) {

        MT2Region thisRegion( &htRegion, &(*iSR) );

        T* thisT = analysis->get(&thisRegion);
        thisT->getShit( file, path );

      } // for signal regions        

    } // while ht regions

    analyses.insert( analysis );

  } // while analysis names


  return analyses;
  
}



template<class T> 
MT2Analysis<T>* MT2Analysis<T>::readFromFile( const std::string& fileName, const std::string& matchName ) {

  std::set<MT2Analysis<T>*> analyses = readAllFromFile(fileName);

  if( analyses.size()==0 ) {
    std::cout << "[MT2Analysis::readFromFile] WARNING!!! Didn't find any MT2Analysis in file " << fileName << std::endl;
    return 0;
  }


  MT2Analysis<T>* analysis = 0;
  if( matchName=="" ) {
    analysis = *(analyses.begin());
  } else {
    for( typename std::set<MT2Analysis<T>*>::iterator iAn=analyses.begin(); iAn!=analyses.end(); ++iAn ) {
      if( (*iAn)->name == matchName ) {
        analysis = *iAn;
        break;
      }
    }
    if( analysis==0 ) {
      std::cout << "[MT2Analysis::readFromFile] WARNING!!! Didn't find any MT2Analysis named '" << matchName << "' in file " << fileName << std::endl;
      return 0;
    }
  }


  if( analyses.size()>1 ) {
    std::cout << "[MT2Analysis::readFromFile] WARNING!!! Multiple analyses found, but reading only one ('" << analysis->name << "')" << std::endl;
    std::cout << "[MT2Analysis::readFromFile] (if you want to read all of them you should use readAllFromFile)" << std::endl;
  } else {
    std::cout << "[MT2Analysis::readFromFile] Grabbed MT2Analysis '" << analysis->name << "' from file " << fileName << std::endl;
  }

  return analysis;

}




template<class T> 
void MT2Analysis<T>::finalize() {

  for( typename std::set<T*>::iterator it=data.begin(); it!=data.end(); ++it ) 
    (*it)->addOverflow();

}


#endif
