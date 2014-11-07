#include <iostream>
#include <sstream>
#include <fstream>
#include <iomanip>


#include <TTreeFormula.h>

#include "interface/MT2Sample.h"
#include "interface/MT2Region.h"
#include "interface/MT2Analysis.h"
#include "interface/MT2Estimate.h"

#define mt2_cxx
#include "../interface/mt2.h"









MT2Analysis<MT2Estimate>* computeYield( const std::string& outputdir, const MT2Sample& sample, const std::string& regionsSet );



int main( int argc, char* argv[] ) {


  //if( argc!=2 ) {
  //  std::cout << "USAGE: ./computeZinvFromGamma [samplesFileName]" << std::endl;
  //  std::cout << "Exiting." << std::endl;
  //  exit(11);
  //}


  std::string samplesFile_gammaJet = "samples/samples_gammaJet.dat";

  std::cout << std::endl << std::endl;
  std::cout << "-> Loading gamma+jet samples from file: " << samplesFile_gammaJet << std::endl;

  std::vector<MT2Sample> samples_gammaJet = MT2Sample::loadSamples(samplesFile_gammaJet);
  if( samples_gammaJet.size()==0 ) {
    std::cout << "There must be an error: samples_gammaJet is empty!" << std::endl;
    exit(1209);
  }


  std::string samplesFile_Zinv = "samples/samples_Zinv.dat";

  std::cout << std::endl << std::endl;
  std::cout << "-> Loading gamma+jet samples from file: " << samplesFile_Zinv << std::endl;

  std::vector<MT2Sample> samples_Zinv = MT2Sample::loadSamples(samplesFile_Zinv);
  if( samples_Zinv.size()==0 ) {
    std::cout << "There must be an error: samples_Zinv is empty!" << std::endl;
    exit(1207);
  }


  std::string regionsSet = "13TeV";

  TH1::AddDirectory(kFALSE); // stupid ROOT memory allocation needs this


  std::string outputdir = "ZinvEstimateFromGamma_" + regionsSet;
  system(Form("mkdir -p %s", outputdir.c_str()));

  
  MT2Analysis<MT2Estimate>* gammaJet = new MT2Analysis<MT2Estimate>( "gammaJet", regionsSet );
  for( unsigned i=0; i<samples_gammaJet.size(); ++i )
    (*gammaJet) += *(computeYield( outputdir, samples_gammaJet[i], regionsSet ));
  

  MT2Analysis<MT2Estimate>* Zinv = new MT2Analysis<MT2Estimate>( "Zinv", regionsSet );
  for( unsigned i=0; i<samples_Zinv.size(); ++i )
    (*Zinv) += *(computeYield( outputdir, samples_Zinv[i], regionsSet ));
  

  system( "rm tmp.root" );
  

  MT2Analysis<MT2Estimate>* ZgammaRatio = new MT2Analysis<MT2Estimate>( (*Zinv)/(*gammaJet) );

  MT2Analysis<MT2Estimate>* ZinvEstimate = new MT2Analysis<MT2Estimate>( (*ZgammaRatio)*(*gammaJet) );

  std::string outputdirPlots = outputdir + "/plots";
  system(Form("mkdir -p %s", outputdirPlots.c_str()));

  //drawCompare( outputdirPlots, ZinvEstimate, Zinv );


  return 0;

}




MT2Analysis<MT2Estimate>* computeYield( const std::string& outputdir, const MT2Sample& sample, const std::string& regionsSet ) {


  std::cout << std::endl << std::endl;
  std::cout << "-> Starting computation for sample: " << sample.name << std::endl;

  TFile* file = TFile::Open(sample.file.c_str());
  TTree* tree = (TTree*)file->Get("mt2");
  
  MT2Tree myTree;
  myTree.Init(tree);


  std::ostringstream preselectionStream;
  preselectionStream << " " 
                     << "(nTaus20==0 && nMuons10==0 && nElectrons10==0)" << " && "
                     << "(nVert > 0)"                      << " && "
                     << "(nJet40 > 1)"                     << " && "
                     << "(jet_pt[1] > 100)"                << " && "
                     << "(ht > 450)"                       << " && "
                     << "((ht < 750 && met_pt > 200) || (ht > 750 && met_pt > 30))" << " && "
                     << "(deltaPhiMin > 0.3)"              << " && " 
                     << "(diffMetMht < 70)"                << "&&"
                     << "(mt2 > 50)" ;

  

  TString preselection = preselectionStream.str().c_str();
  TString cuts = preselection;

  TFile* tmpFile = TFile::Open("tmp.root", "recreate");
  tmpFile->cd();
  TTree* tree_reduced = tree->CopyTree(cuts);

    

  // global sample weight:
  ////Double_t weight = sample.xsection * sample.kfact * sample.lumi / (sample.nevents*sample.PU_avg_weight);
  //Double_t weight = sample.xsection * sample.kfact * sample.lumi / (sample.nevents);
  //cout << endl << "Weight " << weight <<endl; 


  MT2Analysis<MT2Estimate>* analysis = new MT2Analysis<MT2Estimate>( sample.sname, regionsSet, sample.id );

  

  int nentries = tree->GetEntries();



  for( unsigned iEntry=0; iEntry<nentries; ++iEntry ) {

    if( iEntry % 50000 == 0 ) std::cout << "    Entry: " << iEntry << " / " << nentries << std::endl;

    myTree.GetEntry(iEntry);


    float ht   = myTree.ht;
    float met  = myTree.met_pt;
    float mt2  = myTree.mt2;
    int njets  = myTree.nJet40;
    int nbjets = myTree.nBJet40;

    Double_t weight = myTree.evt_scale1fb; 


    MT2Estimate* thisEstimate = analysis->get( ht, njets, nbjets, met );
    if( thisEstimate==0 ) continue;

    thisEstimate->yield->Fill(mt2, weight );

    
  } // for entries


  analysis->finalize();
  

  delete tree;
  delete tree_reduced;

  tmpFile->Close();
  delete tmpFile;

  file->Close();
  delete file;
  
  return analysis;

}








