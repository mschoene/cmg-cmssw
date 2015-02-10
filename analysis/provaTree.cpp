#include <iostream>

#include "../interface/MT2Analysis.h"
#include "../interface/MT2EstimateTree.h"
#include "../interface/MT2Region.h"
#include "../interface/MT2Sample.h"

#define mt2_cxx
#include "../interface/mt2_float.h"


float lumi = 5.;


MT2Analysis<MT2EstimateTree> computeYield( const MT2Sample& sample, const std::string& regionsSet );


int main() {

  std::string samplesFileName = "../samples/samples_CSA14_skimprune.dat";
  std::cout << std::endl << std::endl;
  std::cout << "-> Loading samples from file: " << samplesFileName << std::endl;

  std::vector<MT2Sample> fSamples = MT2Sample::loadSamples(samplesFileName, 1000, 1010); // only signal
  if( fSamples.size()==0 ) {
    std::cout << "There must be an error: samples is empty!" << std::endl;
    exit(1209);
  }


  std::string regionsSet = "13TeV_CSA14";

  MT2Analysis<MT2EstimateTree>*  analysis = new MT2Analysis<MT2EstimateTree>( "provaTree", regionsSet );
  for( unsigned i=0; i<fSamples.size(); ++i ) {
    MT2Analysis<MT2EstimateTree> newana = ( computeYield( fSamples[i], regionsSet ) );
    (*analysis) += newana;
  }

  analysis->writeToFile("provaTree.root");

  return 0;

}



MT2Analysis<MT2EstimateTree> computeYield( const MT2Sample& sample, const std::string& regionsSet ) {


  std::cout << std::endl << std::endl;
  std::cout << "-> Starting computation for sample: " << sample.name << std::endl;

  TFile* file = TFile::Open(sample.file.c_str());
  std::cout << "-> Getting mt2 tree from file: " << sample.file << std::endl;

  TTree* tree = (TTree*)file->Get("mt2");
  

  MT2Tree myTree;
  myTree.loadGenStuff = false;
  myTree.Init(tree);



  std::cout << "-> Setting up MT2Analysis with name: " << sample.sname << std::endl;
  MT2Analysis<MT2EstimateTree> analysis( sample.sname, regionsSet, sample.id );
  //MT2Analysis<MT2EstimateTree>* analysis = new MT2Analysis<MT2EstimateTree>( sample.sname, regionsSet, sample.id );
  MT2EstimateTree::addVar( &analysis, "puppa" );


  bool isData = sample.id<100 && sample.id>0;



  int nentries = tree->GetEntries();

  for( unsigned iEntry=0; iEntry<nentries; ++iEntry ) {

    if( iEntry % 50000 == 0 ) std::cout << "    Entry: " << iEntry << " / " << nentries << std::endl;


    myTree.GetEntry(iEntry);

    if( myTree.nMuons10 > 0) continue;
    if( myTree.nElectrons10 > 0 ) continue;
    if( myTree.nPFLep5LowMT > 0) continue;
    if( myTree.nPFHad10LowMT > 0) continue;
  
    if( myTree.deltaPhiMin<0.3 ) continue;
    if( myTree.diffMetMht>0.5*myTree.met_pt ) continue;

    if( myTree.nVert==0 ) continue;
    if( myTree.nJet40<2 ) continue;
    if( myTree.njet<2 ) continue;
    if( myTree.jet_pt[1]<100. ) continue;


    MT2EstimateTree* thisEstimate = analysis.get( myTree.ht, myTree.nJet40, myTree.nBJet40, myTree.met_pt, myTree.minMTBMet, myTree.mt2 );

    if( thisEstimate==0 ) continue;

    thisEstimate->assignTree( myTree, lumi*myTree.evt_scale1fb );
    thisEstimate->assignVar( "puppa", -13. );
    thisEstimate->tree->Fill();
    //thisEstimate->fillTree( myTree, lumi*myTree.evt_scale1fb );

    
  } // for entries

  //ofs.close();

  analysis.finalize();
  
  delete tree;

  file->Close();
  delete file;
  
  return analysis;

}


