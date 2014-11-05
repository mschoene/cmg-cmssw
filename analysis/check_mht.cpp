#include <iostream>
#include "TFile.h"
#include "TTree.h"
#include "TH1D.h"
#include "TLorentzVector.h"
#include "TCanvas.h"




int main() {


  TFile* file = TFile::Open("/scratch/mmasciov/CSA14_babytrees_QG/QCD_Pt300to470_PU_S14_POSTLS170_babytree_QG.root");
  TTree* tree = (TTree*)file->Get("treeProducerSusyFullHad");
  
  int njet;
  tree->SetBranchAddress("njet", &njet);
  float jet_pt[200];
  tree->SetBranchAddress("jet_pt", jet_pt);
  float jet_eta[200];
  tree->SetBranchAddress("jet_eta", jet_eta);
  float jet_phi[200];
  tree->SetBranchAddress("jet_phi", jet_phi);
  float jet_mass[200];
  tree->SetBranchAddress("jet_mass", jet_mass);
  
  int nentries = tree->GetEntries();
  
  TH1D* h1_mht_tot = new TH1D("h1_mht_tot", "", 100, 0., 600.);
  TH1D* h1_mht_central = new TH1D("h1_mht_central", "", 100, 0., 600.);
  TH1D* h1_mht_central_hard = new TH1D("h1_mht_central_hard", "", 100, 0., 600.);
  
  for( unsigned iEntry=0; iEntry<nentries; ++iEntry ) {

    if( iEntry % 10000 == 0 ) std::cout << "Entry: " << iEntry << " / " << nentries << std::endl;
  
    tree->GetEntry(iEntry);
  
    TLorentzVector mht_tot(0.,0.,0.,0.);
    TLorentzVector mht_central(0.,0.,0.,0.);
    TLorentzVector mht_central_hard(0.,0.,0.,0.);
    
    for( unsigned i=0; i<njet && njet<200; ++i ) {
    
      TLorentzVector jet;
      jet.SetPtEtaPhiM( jet_pt[i], jet_eta[i], jet_phi[i], jet_mass[i] );
     
      mht_tot += jet;
      if( jet.Pt()>40. && fabs(jet.Eta())<2.5 ) mht_central += jet;
      if( jet.Pt()>100. && fabs(jet.Eta())<2.5 ) mht_central_hard += jet;
    
    }
    
    h1_mht_tot->Fill( mht_tot.Pt()); 
    h1_mht_central->Fill( mht_central.Pt()); 
    h1_mht_central_hard->Fill( mht_central_hard.Pt()); 
    
  }
  
  TFile* outfile = TFile::Open("mht.root", "recreate");
  outfile->cd();
  h1_mht_central->SetLineColor(kBlue);
  h1_mht_central_hard->SetLineColor(kRed);
  h1_mht_tot->Write();
  h1_mht_central->Write();
  h1_mht_central_hard->Write();
  outfile->Close();

  TCanvas* c1 = new TCanvas( "c1", "", 600, 600 );
  c1->cd();

  h1_mht_tot->Draw();
  h1_mht_central->Draw("same");
  h1_mht_central_hard->Draw("same");

  c1->SaveAs("mht.eps");
  
  return 0;

}
