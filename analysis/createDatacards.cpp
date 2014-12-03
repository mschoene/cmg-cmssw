#include "fstream"

#include "TFile.h"
#include "TH1D.h"
#include "TList.h"
#include "TObject.h"

#include "interface/MT2Analysis.h"
#include "interface/MT2Estimate.h"







int main( int argc, char* argv[] ) {


  if( argc!=2 ) {
    std::cout << "USAGE: ./createDatacards [dir]" << std::endl;
    exit(113);
  } 


  std::string dir( argv[1] );

  std::vector< MT2Analysis<MT2Estimate>* > analyses = MT2Analysis<MT2Estimate>::readAllFromFile( dir + "/analyses.root" );

  if( analyses.size()==0 ) {
    std::cout << "ERROR! No analyses found!" << std::endl;
    exit(87);
  }

  std::set<MT2HTRegion> htRegions = analyses[0]->getHTRegions();
  std::set<MT2SignalRegion> sigRegions = analyses[0]->getSignalRegions();
  
  for( std::set<MT2HTRegion>::iterator iHT=htRegions.begin(); iHT!=htRegions.end(); ++iHT ) {
    for( std::set<MT2SignalRegion>::iterator iSR=sigRegions.begin(); iSR!=sigRegions.end(); ++iSR ) {

     MT2Region thisRegion(*iHT, *iSR);

     std::string path = dir + "/" + iHT->getName() + "/" + iSR->getName();
     std::string histoFileName = path + "/histograms_" + thisRegion.getName() + ".root";
     TFile* histoFile = TFile::Open( histoFileName.c_str() );
     if( histoFile==0 ) {
       std::cout << "ERROR! Didn't find the histogram file for region: " << thisRegion.getName() << std::endl;
       exit(11);
     }
     TH1D* data = 0;
     std::vector<TH1D*> processes;
     TIter next(histoFile->GetListOfKeys());
     while( TObject *obj = next() ) {
       std::string thisName(obj->GetName());
       if( thisName == ("yield_data_"+thisRegion.getName()) )
         data = (TH1D*)histoFile->Get(thisName.c_str());
       else {
         TH1D* thisHisto = (TH1D*)histoFile->Get(thisName.c_str());
         processes.push_back(thisHisto);
       }
     }


     ofstream datacard( Form("%s/datacard_%s.txt", path.c_str(), thisRegion.getName().c_str()) );

     datacard << "imax 1" << std::endl;
     datacard << "jmax " << analyses.size() << std::endl;
     datacard << "kmax *" << std::endl;
     datacard << "-------------" << std::endl;
     datacard << std::endl << std::endl;
     
     // all the shapes stuff goes here

     datacard << std::endl << std::endl;
     datacard << "bin  " << thisRegion.getName() << std::endl;
     datacard << "observation  " << data->GetEntries() << std::endl;

     datacard.close();

    }
  }

  return 0;

} 
