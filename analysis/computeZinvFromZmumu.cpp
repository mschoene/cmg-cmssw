#include <iostream>
#include <sstream>
#include <fstream>
#include <iomanip>


#include <TTreeFormula.h>
#include <TTree.h>

#include "interface/MT2Region.h"
#include "interface/MT2Analysis.h"
#include "interface/MT2Estimate.h"
#include "interface/MT2EstimateSyst.h"

#define mt2_cxx
#include "../interface/mt2.h"



int main() {

  /*
  if( argc!=2 ) {
    std::cout << "USAGE: ./computeZinvFromGamma [samplesFileName]" << std::endl;
    std::cout << "Exiting." << std::endl;
    exit(11);
  }

  std::string sampleName(argv[1]);

  std::string samplesFileName = "samples/samples_" + sampleName + ".dat";
  std::cout << std::endl << std::endl;
  std::cout << "-> Loading samples from file: " << samplesFileName << std::endl;

  std::vector<MT2Sample> fSamples = MT2Common::loadSamples(samplesFileName);
  if( fSamples.size()==0 ) {
    std::cout << "There must be an error: samples is empty!" << std::endl;
    exit(1209);
  }
  */



  //  TFile* file = TFile::Open(sample.file.c_str());
  TFile* file = TFile::Open("/scratch/dalfonso/POSTPROCESSING/midSEPT/DYJets/DYJetsM50_HT600toInf_PU_S14_POSTLS170_babytree_post.root");
  TTree* tree = (TTree*) file->Get("mt2");

  MT2Tree  myTree;
  myTree.Init(tree);

  ///////////////
  //// MAKE SKIM
  /////

  MT2Tree  myFullTree;
  myFullTree.Init(tree);

  /*
  std::ostringstream preselectionStream;
  //  preselectionStream << "ngenLep>=2" << " && "
  //                     << "abs(jet_pdgId[0])==13"  << " && "
  //                     << "abs(jet_pdgId[1])==13";

  TString preselection = preselectionStream.str().c_str();
  TString cuts = preselection;

  TFile* tmpFile = TFile::Open("tmp.root", "recreate");
  tmpFile->cd();
  MT2Tree myTree = tree->CopyTree(cuts);
  */

  ///////////////
  //// 
  /////


  std::string regionsSet = "8TeV";

  // create a signal region structure with the 8TeV definitions of signal regions:
  // "pippo" is a name that will figure in the histogram names
  // "8TeV" is the name of a given signal region set (for now i've implemented the one used at 8 tev and the recent proposal by mario - called "13TeV")
  // 702 is an optional argument: it's the id of the sample (so 702 in this case is DYJetsToLL_M-50_HT-100to200)
  MT2Analysis<MT2EstimateSyst> *analysis = new MT2Analysis<MT2EstimateSyst>("pippo", regionsSet, 702);
  // ^ the above command creates the whole HT x SR structure, and instantiates one MT2EstimateSyst per region


  //  unsigned int nentries = myTree.GetEntries;
  unsigned int nentries = 10000;
  
  for( unsigned iEntry=0; iEntry< nentries ; ++iEntry ) {
    
    if( iEntry % 500 == 0 ) std::cout << "    Entry: " << iEntry << " / " << nentries << std::endl;
    
    myTree.GetEntry(iEntry);

    float ht       = myTree.ht;
    float met      = myTree.met_pt;
    float mt2      = myTree.mt2;
    int njets      = myTree.nJet40;
    int nbjets     = myTree.nBJet40;

    Double_t weight = myTree.evt_scale1fb;

    float weight_btagUp = weight;
    float weight_btagDown = weight;

    MT2EstimateSyst* est = analysis->get(ht,njets,nbjets,met);
    // ^ the above command looks for the region identified by ht,njets,nbjets,met, and gets the corresponding pointer to the MT2EstimateSyst
    // it will return a null pointer if the event doesnt fall in any region (eg if it has too low HT)
    if( est!=0 ) {
      // if you found a valid region, you can use the pointer, for example filling histograms
      std::cout << "filling region: " << est->region->getName() << std::endl;
      est->yield->Fill( mt2, weight );
      est->yield_btagUp->Fill( mt2, weight_btagUp );
      est->yield_btagDown->Fill( mt2, weight_btagDown );
    }
    
  } // for entries 

  // the command below calls the method addOverflow() for all elements in the MT2Analysis
  // so make sure that if you implement a new class, it has that method implemented
  analysis->finalize();
  
  // write the histograms to a file:
  analysis->writeToFile("ZinvFromZmumu.root");
  // ^ the above will call the method write() for all objects, so again make sure it's implemented
  
  system( "rm tmp.root" );

  //  delete tree;
  //  delete myTree;

  //  tmpFile->Close();
  //  delete tmpFile;

  return 0;

}

