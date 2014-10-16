#include <iostream>
#include <cmath>

#include "TFile.h"
#include "TCanvas.h"
#include "TTree.h"
#include "TChain.h"
#include "TH2D.h"
#include "TH1D.h"
#include "TLegend.h"

#include "CommonTools/DrawBase.h"
#include "QGDev/qgMiniTuple/localQGLikelihoodCalculator/localQGLikelihoodCalculator2.h"


struct FlavorHistos {

  TH1F* h1_njets;
  TH1F* h1_nquarks_partonFlavour;
  TH1F* h1_nquarks_partonId;
  TH1F* h1_nquarks_qg05;
  TH1F* h1_nquarks_qgLikeli;

};


FlavorHistos getFlavorHistos( QGLikelihoodCalculator2* qglc, TTree* tree, const std::string& name );
void drawFlavorHistos( const std::string& outputdir, FlavorHistos fh, const std::string& savename, const std::string& axisName );
float getMaxNorm( TH1F* h1 );



int main() {

  DrawBase* db = new DrawBase("qgStudies");

  std::string dir = "/scratch/pandolf/CSA14_babytrees_QG/skim_mt2_200/";

  TChain* tree_wjets = new TChain("mt2");
  tree_wjets->Add(Form("%s/WJetsToLNu_HT*_PU_S14_POSTLS170_babytree_QG_post_skim.root/mt2", dir.c_str()));

  std::string signal1 = "T1qqqq_2J_mGl1000_mLSP800";
  std::string signal2 = "T1qqqq_2J_mGl1400_mLSP100";
  std::string signal1_forPlots = "T1qqqq (1000, 800)";
  std::string signal2_forPlots = "T1qqqq (1400, 100)";
  std::string bg = "wjets";
  std::string bg_forPlots = "W+Jets";

  TChain* tree_signal1 = new TChain("mt2");
  tree_signal1->Add(Form("%s/SMS_%s_PU_S14_POSTLS170_babytree_QG_post_skim.root/mt2", dir.c_str(), signal1.c_str()));
  
  TChain* tree_signal2 = new TChain("mt2");
  tree_signal2->Add(Form("%s/SMS_%s_PU_S14_POSTLS170_babytree_QG_post_skim.root/mt2", dir.c_str(), signal2.c_str()));

  std::string qgfileName = "pdfsOnlyPt.root";
  QGLikelihoodCalculator2* qglc = new QGLikelihoodCalculator2(qgfileName); 

  std::string outputdir = "QGStudiesPlots";
  system( Form("mkdir -p %s", outputdir.c_str()) );

  FlavorHistos fh_signal1 = getFlavorHistos(qglc, tree_signal1, signal1); 
  drawFlavorHistos( outputdir, fh_signal1, signal1, signal1_forPlots );

  FlavorHistos fh_signal2 = getFlavorHistos(qglc, tree_signal2, signal2); 
  drawFlavorHistos( outputdir, fh_signal2, signal2, signal2_forPlots );

  FlavorHistos fh_wjets   = getFlavorHistos(qglc, tree_wjets, bg );
  drawFlavorHistos( outputdir, fh_wjets, bg, bg_forPlots );


  //compareHistos( outputdir, fh_signal1, signal1, signal1_forPlots, fh_wjets, bg, bg_forPlots );

  return 0;

}


FlavorHistos getFlavorHistos( QGLikelihoodCalculator2* qglc, TTree* tree, const std::string& name ) {


  float evt_scale1fb;
  tree->SetBranchAddress( "evt_scale1fb", &evt_scale1fb );
  float mt2;
  tree->SetBranchAddress( "mt2", &mt2 );
  float ht;
  tree->SetBranchAddress( "ht", &ht );
  int nJet40;
  tree->SetBranchAddress( "nJet40", &nJet40 );
  int nBJet40;
  tree->SetBranchAddress( "nBJet40", &nBJet40 );
  int njet;
  tree->SetBranchAddress( "njet", &njet );
  float jet_pt[100];
  tree->SetBranchAddress( "jet_pt", jet_pt );
  float jet_eta[100];
  tree->SetBranchAddress( "jet_eta", jet_eta );
  int jet_mcFlavour[100];
  tree->SetBranchAddress( "jet_mcFlavour", jet_mcFlavour );
  int jet_partonId[100];
  tree->SetBranchAddress( "jet_partonId", jet_partonId );
  float jet_ptd[100];
  tree->SetBranchAddress( "jet_ptd", jet_ptd );
  int jet_mult[100];
  tree->SetBranchAddress( "jet_mult", jet_mult );
  float jet_axis2[100];
  tree->SetBranchAddress( "jet_axis2", jet_axis2 );
  int jet_id[100];
  tree->SetBranchAddress( "jet_id", jet_id );
  int jet_puId[100];
  tree->SetBranchAddress( "jet_puId", jet_puId );


  int nbins = 15;
  TH1F* h1_njets                 = new TH1F(Form("njets_%s",                 name.c_str()), "", nbins, -0.5, (float)nbins-0.5);
  TH1F* h1_nquarks_partonFlavour = new TH1F(Form("nquarks_partonFlavour_%s", name.c_str()), "", nbins, -0.5, (float)nbins-0.5);
  TH1F* h1_nquarks_partonId      = new TH1F(Form("nquarks_partonId_%s",      name.c_str()), "", nbins, -0.5, (float)nbins-0.5);
  TH1F* h1_nquarks_qg05          = new TH1F(Form("nquarks_qg05_%s",          name.c_str()), "", nbins, -0.5, (float)nbins-0.5);
  TH1F* h1_nquarks_qgLikeli      = new TH1F(Form("nquarks_qgLikeli_%s",      name.c_str()), "", nbins, -0.5, (float)nbins-0.5);


  int nentries = tree->GetEntries();
 
  std::cout << "-> Starting loop on: " << name << std::endl;

  for( unsigned iEntry=0; iEntry<nentries; ++iEntry ) {

    tree->GetEntry(iEntry);

    if( iEntry % 10000 == 0 ) std::cout << "  Entry: " << iEntry << " / " << nentries << std::endl;

    if( ht < 450. ) continue;
    if( mt2 < 200. ) continue;
    if( nJet40 < 2 ) continue;
    if( nBJet40 != 0 ) continue;

    int njets = 0;
    int nquarks_partonFlavour = 0;
    int nquarks_partonId = 0;
    int nquarks_qg05 = 0;
    int nquarks_qgLikeli = 0;

    for( unsigned i=0; i<njet && i<100; ++i ) {

      if( jet_pt[i]<40. ) continue; 
      if( fabs(jet_eta[i])>2.5 ) continue; 
      if( jet_id[i]==0 ) continue; 
      if( jet_puId[i]==0 ) continue; 

      std::vector<float> vars;
      vars.push_back( jet_mult[i] );
      vars.push_back( jet_ptd[i] );
      vars.push_back( jet_axis2[i] );
      float qgl = qglc->computeQGLikelihood( jet_pt[i], jet_eta[i], vars );

      njets++;
      if( abs(jet_mcFlavour[i])<4 && jet_mcFlavour[i]!=0 ) nquarks_partonFlavour++;
      if( abs(jet_partonId[i])<4 && jet_partonId[i]!=0 ) nquarks_partonId++;
      if( qgl>0.5 ) nquarks_qg05++;

    } // for jets

    h1_njets->Fill( njets, evt_scale1fb );
    h1_nquarks_partonFlavour->Fill( nquarks_partonFlavour, evt_scale1fb );
    h1_nquarks_partonId->Fill( nquarks_partonId, evt_scale1fb );
    h1_nquarks_qg05->Fill( nquarks_qg05, evt_scale1fb );

  } // for entries

  FlavorHistos fh;
  fh.h1_njets = h1_njets;
  fh.h1_nquarks_partonFlavour = h1_nquarks_partonFlavour;
  fh.h1_nquarks_partonId = h1_nquarks_partonId;
  fh.h1_nquarks_qg05 = h1_nquarks_qg05;
  fh.h1_nquarks_qgLikeli = 0;

  return fh;
  
}
  


void drawFlavorHistos( const std::string& outputdir, FlavorHistos fh, const std::string& savename, const std::string& axisName ) {

  TCanvas* c1 = new TCanvas("c1", "", 600, 600 );
  c1->cd();

  float xMax = fh.h1_njets->GetXaxis()->GetXmax();

  float yMax_njets = getMaxNorm(fh.h1_njets);
  float yMax_nquarks_partonFlavour = getMaxNorm(fh.h1_nquarks_partonFlavour);
  float yMax_nquarks_partonId = getMaxNorm(fh.h1_nquarks_partonId);
  float yMax_nquarks_qg05 = getMaxNorm(fh.h1_nquarks_qg05);

  float yMax = yMax_njets;
  if( yMax_nquarks_partonFlavour>yMax ) yMax = yMax_nquarks_partonFlavour;
  if( yMax_nquarks_partonId>yMax ) yMax = yMax_nquarks_partonId;
  if( yMax_nquarks_qg05>yMax ) yMax = yMax_nquarks_qg05;
  yMax *= 1.2;

  TH2D* h2_axes = new TH2D("axes", "", 10, 0., xMax, 10, 0., yMax );
  h2_axes->SetXTitle("Jet Multiplicity");
  h2_axes->SetYTitle("Normalized to Unity");
  h2_axes->Draw();

  fh.h1_njets->SetLineWidth(2);
  fh.h1_njets->SetLineColor(kBlack);

  fh.h1_nquarks_partonFlavour->SetLineWidth(2);
  fh.h1_nquarks_partonFlavour->SetLineColor(46);

  fh.h1_nquarks_partonId->SetLineWidth(2);
  fh.h1_nquarks_partonId->SetLineColor(kRed+1);

  fh.h1_nquarks_qg05->SetLineWidth(2);
  fh.h1_nquarks_qg05->SetLineColor(38);


  TLegend* legend = new TLegend( 0.55, 0.65, 0.9, 0.9, axisName.c_str() );
  legend->SetTextSize(0.035);
  legend->SetFillColor(0);
  legend->AddEntry( fh.h1_njets, "Jets", "L" );
  //legend->AddEntry( fh.h1_nquarks_partonFlavour, "Quarks (mcFlavour)", "L" );
  legend->AddEntry( fh.h1_nquarks_partonId, "Quarks (MC Truth)", "L" );
  legend->AddEntry( fh.h1_nquarks_qg05, "Quarks (QGL>0.5)", "L" );
  legend->Draw("same");

  fh.h1_njets->DrawNormalized("histo same");
  //fh.h1_nquarks_partonFlavour->DrawNormalized("histo same");
  fh.h1_nquarks_partonId->DrawNormalized("histo same");
  fh.h1_nquarks_qg05->DrawNormalized("histo same");

  gPad->RedrawAxis();

  c1->SaveAs(Form("%s/mult_%s.eps", outputdir.c_str(), savename.c_str()));
  c1->SaveAs(Form("%s/mult_%s.png", outputdir.c_str(), savename.c_str()));

  delete c1;
  delete h2_axes;

}


float getMaxNorm( TH1F* h1 ) {
 
  return h1->GetMaximum()/h1->Integral();

}


//void  compareHistos( outputdir, fh_signal1, signal1, signal1_forPlots, fh_wjets, bg, bg_forPlots );
