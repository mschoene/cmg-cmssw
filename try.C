#define treeProducerSusyFullHad_cxx
#include "treeProducerSusyFullHad.h"
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>

void main(){


  TFile* file = TFile::Open("/shome/mmasciov/CMSSW_5_3_7_patch5/src/treeProducerSusyFullHad_tree_small.root");
  TTree* tree = (TTree*)file->Get("treeProducerSusyFullHad");

  //gROOT->ProcessLine(".L treeProducerSusyFullHad.C");

  treeProducerSusyFullHad myTree;
  myTree.Init(tree);

  long int nEntries= tree->GetEntries();
  cout<< "nEntries "<<nEntries<<endl;
  
  for(long int j=0; j<nEntries; ++j){
    myTree.GetEntry(j);
    //cout<<myTree.evt<<endl;
  }
  
  std::ostringstream preselectionStream;
  preselectionStream << " "
		     << "(nTaus20==0 && nMuons10==0 && nElectrons10==0)"                   << " && "
		     << "(nVert > 0)"                      << " && "
		     << "(nJet40 > 1)"                     << " && "
		     << "(jet_pt[1] > 100)"                << " && "
		     << "(deltaPhiMin > 0.3)"              << " && "
		     << "(diffMetMht < 70)";

  TString preselection = preselectionStream.str().c_str();
  TString cuts = preselection;

  TFile* tmpFile = TFile::Open("tmp.root", "recreate");
  tmpFile->cd();
 
  TTree* tree_reduced = tree->CopyTree(cuts);
  tmpFile->Write();

}
