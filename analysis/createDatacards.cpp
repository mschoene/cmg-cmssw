#include "fstream"

#include "TFile.h"
#include "TH1D.h"
#include "TList.h"
#include "TObject.h"

#include "interface/MT2Analysis.h"
#include "interface/MT2Estimate.h"




MT2Analysis<MT2Estimate>* get( const std::string& name, std::vector< MT2Analysis<MT2Estimate>* > analyses, const std::string& name1, const std::string& name2="", const std::string& name3="", const std::string& name4="" );



int main( int argc, char* argv[] ) {


  if( argc!=2 ) {
    std::cout << "USAGE: ./createDatacards [dir]" << std::endl;
    exit(113);
  } 


  std::string dir( argv[1] );
  std::string fileName = dir + "/analyses.root";

  //std::vector< MT2Analysis<MT2Estimate>* > analyses = MT2Analysis<MT2Estimate>::readAllFromFile( dir + "/analyses.root" );

  //if( analyses.size()==0 ) {
  //  std::cout << "ERROR! No analyses found!" << std::endl;
  //  exit(87);
  //}

  MT2Analysis<MT2Estimate>* data  = MT2Analysis<MT2Estimate>::readFromFile( fileName, "data" );
  MT2Analysis<MT2Estimate>* qcd   = MT2Analysis<MT2Estimate>::readFromFile( fileName, "QCD"  );
  MT2Analysis<MT2Estimate>* zinv  = MT2Analysis<MT2Estimate>::readFromFile( fileName, "ZJets");
  MT2Analysis<MT2Estimate>* wjets = MT2Analysis<MT2Estimate>::readFromFile( fileName, "WJets");
  MT2Analysis<MT2Estimate>* top   = MT2Analysis<MT2Estimate>::readFromFile( fileName, "Top"  );
  MT2Analysis<MT2Estimate>* lostLepton = new MT2Analysis<MT2Estimate>( *top + *wjets );
  lostLepton->setName( "lostLepton" );
  //*lostLepton += *wjets;
  
  //MT2Analysis<MT2Estimate>* lostLepton = new MT2Analysis<MT2Estimate>( *wjets );
  //*lostLepton += *top;


  //MT2Analysis<MT2Estimate>* data       = get( "data"    , analyses, "data" );
  //MT2Analysis<MT2Estimate>* lostLepton = get( "LostLept", analyses, "WJets", "Top" );
  //MT2Analysis<MT2Estimate>* qcd        = get( "QCD"     , analyses, "QCD" );
  //MT2Analysis<MT2Estimate>* zinv       = get( "Zinv"    , analyses, "ZJets" );



  std::set<MT2HTRegion> htRegions = data->getHTRegions();
  std::set<MT2SignalRegion> sigRegions = data->getSignalRegions();
  
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
     //TH1D* data = 0;
     //std::vector<TH1D*> processes;
     //TIter next(histoFile->GetListOfKeys());
     //while( TObject *obj = next() ) {
     //  std::string thisName(obj->GetName());
     //  if( thisName == ("yield_data_"+thisRegion.getName()) )
     //    data = (TH1D*)histoFile->Get(thisName.c_str());
     //  else {
     //    TH1D* thisHisto = (TH1D*)histoFile->Get(thisName.c_str());
     //    processes.push_back(thisHisto);
     //  }
     //}

     TH1D* this_data       = data      ->get(thisRegion)->yield;
     TH1D* this_qcd        = qcd       ->get(thisRegion)->yield;
     TH1D* this_Zinv       = zinv      ->get(thisRegion)->yield;
     TH1D* this_lostLepton = lostLepton->get(thisRegion)->yield;

     std::string datacardName(Form("%s/datacard_%s.txt", path.c_str(), thisRegion.getName().c_str()) );
     ofstream datacard( datacardName.c_str() );

     datacard << "imax 1" << std::endl;
     datacard << "jmax 3" << std::endl;
     datacard << "kmax *" << std::endl;
     datacard << "-------------" << std::endl;
     datacard << std::endl << std::endl;
     
     // all the shapes stuff goes here

     datacard << std::endl << std::endl;
     datacard << "bin  " << thisRegion.getName() << std::endl;
     datacard << "observation  " << this_data->Integral(1,this_data->GetXaxis()->GetNbins()) << std::endl;
     datacard << "-------------" << std::endl;
     datacard << std::endl << std::endl;

     // sig QCD Zinv LostLepton
     datacard << "bin \t" << thisRegion.getName() << "\t" << thisRegion.getName() << "\t" << thisRegion.getName() << "\t" << thisRegion.getName() << std::endl;
     datacard << "process \t sig \t qcd \t zinv \t llep" << std::endl;
     datacard << "process \t 0 \t 1 \t 2 \t 3" << std::endl;
     datacard << "rate \t XXX \t " << this_qcd->Integral() << " \t " << this_Zinv->Integral() << " \t " << this_lostLepton->Integral() << std::endl;
     datacard << "-------------" << std::endl;

     datacard << "syst_sig     lnN \t 1.1 - - -" << std::endl;
     datacard << "syst_qcd     lnN \t - 2.0 - -" << std::endl;
     datacard << "syst_ll_corr lnN \t - - - 1.15" << std::endl;

     datacard.close();

     std::cout << "-> Created datacard: " << datacardName << std::endl;

    }
  }

  return 0;

} 



MT2Analysis<MT2Estimate>* get( const std::string& name, std::vector< MT2Analysis<MT2Estimate>* > analyses, const std::string& name1, const std::string& name2, const std::string& name3, const std::string& name4 ) {


  std::cout << "Looking for: " << name << std::endl;
  MT2Analysis<MT2Estimate>* returnAnalysis = new MT2Analysis<MT2Estimate>( name, analyses[0]->getHTRegions(), analyses[0]->getSignalRegions() );

  for( unsigned i=0; i<analyses.size(); ++i ) {

    if( analyses[i]->name == name1 || analyses[i]->name == name2 || analyses[i]->name == name3 || analyses[i]->name == name4 ) {
      std::cout << "  added: " << analyses[i]->name << std::endl;
      (*returnAnalysis) += (*analyses[i]);
    }

  }

  return returnAnalysis;

}
