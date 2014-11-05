#include "TFile.h"
#include "TH1F.h"
#include "TH2D.h"
#include "TTree.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "TLorentzVector.h"




int main() {

  TFile* file = TFile::Open("TTJets_PU20bx25_10000events.root");
  TTree* tree = (TTree*)file->Get("treeProducerSusyFullHad");

  int njet;
  tree->SetBranchAddress("njet", &njet );
  float jet_pt[100];
  tree->SetBranchAddress("jet_pt", jet_pt );
  float jet_eta[100];
  tree->SetBranchAddress("jet_eta", jet_eta );
  float jet_phi[100];
  tree->SetBranchAddress("jet_phi", jet_phi );
  float jet_mass[100];
  tree->SetBranchAddress("jet_mass", jet_mass );
  float jet_qgl[100];
  tree->SetBranchAddress("jet_qgl", jet_qgl );
  float jet_btag[100];
  tree->SetBranchAddress("jet_btagCSV", jet_btag );
  float jet_partonId[100];
  tree->SetBranchAddress("jet_partonId", jet_partonId );


  int nentries = tree->GetEntries();
  nentries = 10000;

  std::vector<float> deltaRbins;
  deltaRbins.push_back( 0.4 );
  deltaRbins.push_back( 0.8 );
  deltaRbins.push_back( 1.5 );
  deltaRbins.push_back( 5. );

  std::vector<TH1D*> h1_qgl;
  for( unsigned i=0; i<deltaRbins.size()-1; ++i ) {
    TH1D* h1 = new TH1D(Form("qgl_%d", i), "", 25, 0., 1.001);
    h1_qgl.push_back(h1);
  }


  TFile* outfile = new TFile("deltaR.root", "recreate" );
  outfile->cd();

  for( unsigned iEntry=0; iEntry<nentries; ++iEntry ) {

    tree->GetEntry( iEntry );

    if( njet==0 ) continue;

    TLorentzVector jet;
    jet.SetPtEtaPhiM( jet_pt[0], jet_eta[0], jet_phi[0], jet_mass[0] );

    if( jet.Pt() < 20. ) continue;
    if( fabs( jet.Eta() )>1.3 ) continue;
    if( jet_btag[0]>0.25 ) continue;
    if( abs(jet_partonId[0])>3 ) continue;
    if( jet_partonId[0]==0 ) continue;

    float deltaRmin = 999.;
    int foundJet=-1;

    for( unsigned i=1; i<njet && i<100; ++i ) {
    
      if( jet_pt[i]<20. ) continue;

      TLorentzVector otherJet;
      otherJet.SetPtEtaPhiM( jet_pt[i], jet_eta[i], jet_phi[i], jet_mass[i] );

      float deltaR = otherJet.DeltaR(jet);

      if( deltaR<deltaRmin ) {
        deltaRmin=deltaR;
        foundJet = i;
      }

    }  //for jets


    TLorentzVector otherJet;
    otherJet.SetPtEtaPhiM( jet_pt[foundJet], jet_eta[foundJet], jet_phi[foundJet], jet_mass[foundJet] );

    for( unsigned i=0; i<deltaRbins.size()-1; ++i ) {
      if( deltaRmin>deltaRbins[i] && deltaRmin<deltaRbins[i+1] ) {
        h1_qgl[i]->Fill( jet_qgl[foundJet] );
      }
    }

  } // for entries



  TCanvas* c1 = new TCanvas( "c1", "", 600, 600 );
  c1->cd();

  TH2D* axes = new TH2D("axes", "", 10, 0., 1.001, 10, 0., 0.12 );
  axes->SetXTitle("QGL");
  axes->SetYTitle( "Normalized to Unity");
  axes->Draw("same");


  TLegend* legend = new TLegend( 0.3, 0.6, 0.7, 0.9 );
  legend->SetFillColor(0);
  legend->SetTextSize(0.035);

  for( unsigned i=0; i<h1_qgl.size(); ++i ) {

    h1_qgl[i]->SetLineColor(i+1);
    h1_qgl[i]->SetLineWidth(2);

    h1_qgl[i]->DrawNormalized("same");

    legend->AddEntry( h1_qgl[i], Form("%.1f < #Delta R < %.1f", deltaRbins[i], deltaRbins[i+1]), "L" );

  }

  legend->Draw("same");

  gPad->RedrawAxis();

  c1->SaveAs("qgl_deltaR.eps");
  c1->SaveAs("qgl_deltaR.png");


  outfile->cd();
  for( unsigned i=0; i<h1_qgl.size(); ++i ) {
     h1_qgl[i]->Write();
  }


  outfile->Close();



  return 0;

}

      
