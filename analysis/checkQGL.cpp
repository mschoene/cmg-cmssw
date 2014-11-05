#include <iostream>
#include <cmath>

#include "TFile.h"
#include "TTree.h"
#include "TChain.h"
#include "TH1F.h"
#include "TF1.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "TH2D.h"
#include "TKey.h"
#include "TDirectory.h"
#include "TGraph.h"
#include "TLine.h"
#include "TVectorT.h"
#include "TLorentzVector.h"

#include "../QGDev/qgMiniTuple/localQGLikelihoodCalculator/localQGLikelihoodCalculator2.h"
#include "../QGDev/qgMiniTuple/localQGLikelihoodCalculator/localQGLikelihoodCalculator.h"
#include "../QGDev/qgMiniTuple/macros/binFunctions.h"
#include "CommonTools/DrawBase.h"



void drawSingleRoC( const std::string& outputdir, const std::string& saveName, float ptMin, float ptMax, const std::string& name1, TH1F* h1_qgl_quark, TH1F* h1_qgl_gluon, const std::string& name2, TH1F* h1_qgl2_quark, TH1F* h1_qgl2_gluon );
TGraph* getSingleRoC( const std::string& name, TH1F* h1_quark, TH1F* h1_gluon );
void drawQuarkVsGluon( const std::string& outputdir, const std::string& savename, const std::string& axisName, float ptMin, float ptMax, TH1F* h1_quark, TH1F* h1_gluon );



std::string selection = "zjet";
//std::string selection = "dijet";


int main() {

  DrawBase* db = new DrawBase("qg");

  std::string qgfileName = "pdfsOnlyPt.root";
  QGLikelihoodCalculator2* qglc = new QGLikelihoodCalculator2(qgfileName); 


  TChain* tree = new TChain("mt2");
  if( selection=="dijet" )
    tree->Add("/scratch/mangano/babies/postProcessed/versionToBeDetermined/QCD_Pt*_PU_S14_POSTLS170_babytree.root/treeProducerSusyFullHad");
  else {
    tree->Add("/scratch/pandolf/CSA14_babytrees_QG/ZJetsToNuNu_HT100to200_PU_S14_POSTLS170_babytree_QG_post.root/mt2");
    tree->Add("/scratch/pandolf/CSA14_babytrees_QG/ZJetsToNuNu_HT200to400_PU_S14_POSTLS170_babytree_QG_post.root/mt2");
    tree->Add("/scratch/pandolf/CSA14_babytrees_QG/ZJetsToNuNu_HT400to600_PU_S14_POSTLS170_babytree_QG_post.root/mt2");
    tree->Add("/scratch/pandolf/CSA14_babytrees_QG/ZJetsToNuNu_HT600toInf_PU_S14_POSTLS170_babytree_QG_post.root/mt2");
  }

  std::cout << "-> Added trees. Chain has " << tree->GetEntries() << " entries." << std::endl;



  float weight;
  tree->SetBranchAddress("evt_scale1fb", &weight);
  int njet;
  tree->SetBranchAddress("njet", &njet);
  float pt[100];
  tree->SetBranchAddress("jet_pt", pt);
  float eta[100];
  tree->SetBranchAddress("jet_eta", eta);
  float phi[100];
  tree->SetBranchAddress("jet_phi", phi);
  float mass[100];
  tree->SetBranchAddress("jet_mass", mass);
  float ptD[100];
  tree->SetBranchAddress("jet_ptd", ptD);
  int mult[100];
  tree->SetBranchAddress("jet_mult", mult);
  float axis2[100];
  tree->SetBranchAddress("jet_axis2", axis2);
  int partonId[100];
  tree->SetBranchAddress("jet_partonId", partonId);
  int id[100];
  tree->SetBranchAddress("jet_id", id);
  int puId[100];
  tree->SetBranchAddress("jet_puId", puId);
  float btag[100];
  tree->SetBranchAddress("jet_btagCSV", btag);
  float mcPt[100];
  tree->SetBranchAddress("jet_mcPt", mcPt);
  float qgl_old[100];
  tree->SetBranchAddress("jet_qgl", qgl_old);

  int ngenPart;
  tree->SetBranchAddress("ngenPart", &ngenPart);
  float genPart_pt[200];
  tree->SetBranchAddress("genPart_pt", genPart_pt);
  float genPart_eta[200];
  tree->SetBranchAddress("genPart_eta", genPart_eta);
  float genPart_phi[200];
  tree->SetBranchAddress("genPart_phi", genPart_phi);
  float genPart_mass[200];
  tree->SetBranchAddress("genPart_mass", genPart_mass);
  int genPart_pdgId[200];
  tree->SetBranchAddress("genPart_pdgId", genPart_pdgId);



  std::vector<float> etaBins = {0,2.5};
  std::vector<float> ptBinsC; getBins(ptBinsC, 20, 20, 2000, true); ptBinsC.push_back(4000);


  int nbins = 50;

  std::vector< TH1F* > vh1_mult_quark;
  std::vector< TH1F* > vh1_mult_gluon;

  std::vector< TH1F* > vh1_ptD_quark;
  std::vector< TH1F* > vh1_ptD_gluon;

  std::vector< TH1F* > vh1_axis2_quark;
  std::vector< TH1F* > vh1_axis2_gluon;

  std::vector< TH1F* > vh1_qgl_quark;
  std::vector< TH1F* > vh1_qgl_gluon;

  std::vector< TH1F* > vh1_qgl_old_quark;
  std::vector< TH1F* > vh1_qgl_old_gluon;



  for( unsigned i=0; i<ptBinsC.size()-1; ++i ) { // integrated in rho

    TH1F* h1_mult_quark = new TH1F(Form("mult_quark_pt%d", i), "", 100, 0., 100);
    TH1F* h1_mult_gluon = new TH1F(Form("mult_gluon_pt%d", i), "", 100, 0., 100);
    h1_mult_quark->Sumw2();
    h1_mult_gluon->Sumw2();

    vh1_mult_quark.push_back(h1_mult_quark);
    vh1_mult_gluon.push_back(h1_mult_gluon);


    TH1F* h1_ptD_quark = new TH1F(Form("ptD_quark_pt%d", i), "", nbins, 0., 1.0001);
    TH1F* h1_ptD_gluon = new TH1F(Form("ptD_gluon_pt%d", i), "", nbins, 0., 1.0001);
    h1_ptD_quark->Sumw2();
    h1_ptD_gluon->Sumw2();

    vh1_ptD_quark.push_back(h1_ptD_quark);
    vh1_ptD_gluon.push_back(h1_ptD_gluon);


    TH1F* h1_axis2_quark = new TH1F(Form("axis2_quark_pt%d", i), "", nbins, 0., 10);
    TH1F* h1_axis2_gluon = new TH1F(Form("axis2_gluon_pt%d", i), "", nbins, 0., 10);
    h1_axis2_quark->Sumw2();
    h1_axis2_gluon->Sumw2();

    vh1_axis2_quark.push_back(h1_axis2_quark);
    vh1_axis2_gluon.push_back(h1_axis2_gluon);


    
    TH1F* h1_qgl_quark = new TH1F(Form("qgl_quark_pt%d", i), "", nbins, 0., 1.0001);
    TH1F* h1_qgl_gluon = new TH1F(Form("qgl_gluon_pt%d", i), "", nbins, 0., 1.0001);
    h1_qgl_quark->Sumw2();
    h1_qgl_gluon->Sumw2();

    vh1_qgl_quark.push_back(h1_qgl_quark);
    vh1_qgl_gluon.push_back(h1_qgl_gluon);
    
    TH1F* h1_qgl_old_quark = new TH1F(Form("qgl_old_quark_pt%d", i), "", nbins, 0., 1.0001);
    TH1F* h1_qgl_old_gluon = new TH1F(Form("qgl_old_gluon_pt%d", i), "", nbins, 0., 1.0001);
    h1_qgl_old_quark->Sumw2();
    h1_qgl_old_gluon->Sumw2();

    vh1_qgl_old_quark.push_back(h1_qgl_old_quark);
    vh1_qgl_old_gluon.push_back(h1_qgl_old_gluon);
    

    
  }






  std::string outputdir = "QGLPlots";
  system( Form("mkdir -p %s", outputdir.c_str()));

  int nentries = tree->GetEntries();
  int nentries_max = -1;
  if( nentries_max>0 && nentries>nentries_max ) nentries = nentries_max;



  for( unsigned iEntry=0; iEntry<nentries; ++iEntry ) {

    tree->GetEntry(iEntry);

    if( iEntry % 500000 == 0 ) std::cout << "Entry: " << iEntry << " / " << nentries << std::endl;

    TLorentzVector jet0;
    jet0.SetPtEtaPhiM( pt[0], eta[0], phi[0], mass[0] );

    int jetsToBeAnalyzed = 0;

    TLorentzVector jet1;

    if( selection=="dijet" ) {

      if( njet<2 ) continue;

      jet1.SetPtEtaPhiM( pt[1], eta[1], phi[1], mass[1] );
      if( jet1.Pt()<20. ) continue;
      if( fabs(jet0.DeltaPhi(jet1))<(3.14159-0.2) ) continue;


      float pt_ave = 0.5*( pt[0]+pt[1] );
      if( njet>2 )
        if( pt[2] > 0.2*pt_ave ) continue;

      jetsToBeAnalyzed = 2;

    } else if( selection=="zjet" ) {
   
      TLorentzVector z;
      for( unsigned i=0; i<ngenPart && i<200; ++i ) {
        if( genPart_pdgId[i]!=23 ) continue;
        z.SetPtEtaPhiM( genPart_pt[i], genPart_eta[i], genPart_phi[i], genPart_mass[i] );
        break;
      }
      if( z.Pt()<1. ) continue; 
      if( fabs(jet0.DeltaPhi(z))<(3.14159-0.2) ) continue;
      //if( njet>1 ) {
      //  jet1.SetPtEtaPhiM( pt[1], eta[1], phi[1], mass[1] );
      //  if( jet1.Pt() > 0.2*z.Pt() ) continue;
      //}

      jetsToBeAnalyzed = 1;

    } else {

      std::cout << "Unkown selection: " << selection << std::endl;
      exit(11);

    }


    for( unsigned i=0; i<jetsToBeAnalyzed; ++i ) {

      if( pt[i]<20. ) continue;
      if( fabs(eta[i])>2.5 ) continue;
      if( mcPt[i]<5. ) continue;
      if( btag[i] > 0.244 ) continue; // CSVL
      
        
      bool is_uds = (partonId[i]!=0 && abs(partonId[i])<4);
      bool is_g   = partonId[i]==21;
      if( !is_uds && !is_g ) continue;	// Keep only udsg

      std::vector<float> vars;
      vars.push_back( mult[i] );
      vars.push_back( ptD[i] );
      vars.push_back( axis2[i] );

      float qgl = qglc->computeQGLikelihood     ( pt[i], eta[i], vars );
      
      int ibin = -1;
      if( !getBinNumber( ptBinsC, pt[i], ibin) ) continue;


      if( ibin<0 ) {
        std::cout << "CONTINUING (pt: " << pt[i] << " )" << std::endl;
        continue;
      }


      if( is_uds ) {

        vh1_mult_quark[ibin]->Fill( mult[i], weight );
        vh1_ptD_quark[ibin]->Fill( ptD[i], weight );
        vh1_axis2_quark[ibin]->Fill( axis2[i], weight );

        vh1_qgl_quark[ibin]->Fill( qgl, weight );
        vh1_qgl_old_quark[ibin]->Fill( qgl_old[i], weight );


      } else if( is_g ) {

        vh1_mult_gluon[ibin]->Fill( mult[i], weight );
        vh1_ptD_gluon[ibin]->Fill( ptD[i], weight );
        vh1_axis2_gluon[ibin]->Fill( axis2[i], weight );

        vh1_qgl_gluon[ibin]->Fill( qgl, weight );
        vh1_qgl_old_gluon[ibin]->Fill( qgl_old[i], weight );

      }

    } // for jets
  
  } // for entries



  for( unsigned i=0; i<vh1_qgl_quark.size(); ++i ) {

    drawSingleRoC( outputdir, "vsRhoBins", ptBinsC[i], ptBinsC[i+1], "QG Likelihood", vh1_qgl_quark[i], vh1_qgl_gluon[i], "QG Likelihood (#rho bins)", vh1_qgl_old_quark[i], vh1_qgl_old_gluon[i] );

    drawQuarkVsGluon( outputdir, "mult", "Jet Multiplicity", ptBinsC[i], ptBinsC[i+1], vh1_mult_quark[i], vh1_mult_gluon[i] );
    drawQuarkVsGluon( outputdir, "ptD", "p_{T}D", ptBinsC[i], ptBinsC[i+1], vh1_ptD_quark[i], vh1_ptD_gluon[i] );
    drawQuarkVsGluon( outputdir, "axis2", "-log( #sigma_{2} )", ptBinsC[i], ptBinsC[i+1], vh1_axis2_quark[i], vh1_axis2_gluon[i] );
    drawQuarkVsGluon( outputdir, "qgl", "Quark-Gluon Likelihood", ptBinsC[i], ptBinsC[i+1], vh1_qgl_quark[i], vh1_qgl_gluon[i] );
    drawQuarkVsGluon( outputdir, "qgl_old", "Quark-Gluon Likelihood (#rho bins)", ptBinsC[i], ptBinsC[i+1], vh1_qgl_old_quark[i], vh1_qgl_old_gluon[i] );
  }


  std::string outfilename = "outfileQG_" + selection + ".root";

  TFile* outfile = TFile::Open(outfilename.c_str(), "recreate");
  outfile->cd();
  outfile->mkdir("qgl");
  outfile->cd("qgl");
  for( unsigned i=0; i<vh1_qgl_quark.size(); ++i ) {
    vh1_qgl_quark.at(i)->Write();
    vh1_qgl_gluon.at(i)->Write();
  }

  outfile->cd();

  outfile->mkdir("qgl_old");
  outfile->cd("qgl_old");
  for( unsigned i=0; i<vh1_qgl_old_quark.size(); ++i ) {
    vh1_qgl_old_quark.at(i)->Write();
    vh1_qgl_old_gluon.at(i)->Write();
  }

  outfile->cd();

  TVectorT<float> etaBins_t(etaBins.size(), &etaBins[0]);
  etaBins_t.Write("etaBins");
  TVectorT<float> ptBinsC_t(ptBinsC.size(), &ptBinsC[0]);
  ptBinsC_t.Write("ptBinsC");

  outfile->Close();
    
  return 0;

}






void drawSingleRoC( const std::string& outputdir, const std::string& saveName, float ptMin, float ptMax, const std::string& name1, TH1F* h1_qgl_quark, TH1F* h1_qgl_gluon, const std::string& name2, TH1F* h1_qgl2_quark, TH1F* h1_qgl2_gluon ) {

  TGraph* gr_qgl = getSingleRoC( "qgl", h1_qgl_quark, h1_qgl_gluon );
  TGraph* gr_qgl2 = getSingleRoC( "qgl2", h1_qgl2_quark, h1_qgl2_gluon );

  TCanvas* c1 = new TCanvas("c1", "", 600, 600 );
  c1->cd();

  TH2D* h2_axes = new TH2D("axes", "", 10, 0., 1.0001, 10, 0., 1.0001);
  h2_axes->SetYTitle( "Quark Efficiency" );
  h2_axes->SetXTitle( "Gluon Rejection" );
  h2_axes->Draw();

  TLine* diag = new TLine( 0., 1., 1., 0.);
  diag->Draw("same");

  gr_qgl->SetMarkerSize(1.5);
  gr_qgl2->SetMarkerSize(1.5);

  gr_qgl->SetMarkerColor(kOrange);
  gr_qgl2->SetMarkerColor(kRed+2);

  gr_qgl->SetMarkerStyle(20);
  gr_qgl2->SetMarkerStyle(24);


  gr_qgl->Draw("P same");
  gr_qgl2->Draw("P same");

 
  
  TLegend* legend = new TLegend( 0.2, 0.2, 0.55, 0.45, Form("%.0f < p_{T} < %.0f GeV", ptMin, ptMax) );
  legend->SetFillColor(0);
  legend->SetTextSize(0.038);
  legend->AddEntry( gr_qgl, name1.c_str(), "P" );
  legend->AddEntry( gr_qgl2, name2.c_str(), "P" );
  legend->Draw("same");

  gPad->RedrawAxis();

  c1->SaveAs( Form("%s/RoC_%s_pt%.0f.eps", outputdir.c_str(), saveName.c_str(), ptMin) );
  c1->SaveAs( Form("%s/RoC_%s_pt%.0f.png", outputdir.c_str(), saveName.c_str(), ptMin) );

  delete c1;
  delete h2_axes;

}



TGraph* getSingleRoC( const std::string& name, TH1F* h1_quark, TH1F* h1_gluon ) {

  TGraph* gr_RoC = new TGraph(0);
  gr_RoC->SetName(Form("roc_%s", name.c_str()));

  int nbins = h1_quark->GetNbinsX();

  for( unsigned ibin=1; ibin<nbins+1; ++ibin ) {

    float eff_q = h1_quark->Integral( nbins-ibin, nbins )/h1_quark->Integral( 1, nbins );
    float eff_g = h1_gluon->Integral( nbins-ibin, nbins )/h1_gluon->Integral( 1, nbins );
    
    gr_RoC->SetPoint( ibin-1, 1.-eff_g, eff_q );

  }

  return gr_RoC;

}




void drawQuarkVsGluon( const std::string& outputdir, const std::string& savename, const std::string& axisName, float ptMin, float ptMax, TH1F* h1_quark, TH1F* h1_gluon ) {

  TCanvas* c2 = new TCanvas( "c2", "", 600, 600 );
  c2->cd();

  float xMax = h1_quark->GetXaxis()->GetXmax();

  float ymax_q = h1_quark->GetMaximum()/h1_quark->Integral();
  float ymax_g = h1_gluon->GetMaximum()/h1_gluon->Integral();

  float yMax = (ymax_q>ymax_g) ? ymax_q : ymax_g;
  yMax *= 1.1;

  TH2D* h2_axes = new TH2D("axes", "", 10, 0., xMax, 10, 0., yMax );
  h2_axes->SetXTitle( axisName.c_str() );
  h2_axes->SetYTitle( "Normalized To Unity" );
  h2_axes->Draw();

  h1_quark->SetFillStyle(3004);
  h1_gluon->SetFillStyle(3005);

  h1_quark->SetFillColor(38);
  h1_gluon->SetFillColor(46);

  h1_quark->SetLineColor(38);
  h1_gluon->SetLineColor(46);

  h1_quark->SetLineWidth(2);
  h1_gluon->SetLineWidth(2);

  float xMin_legend = (savename=="qgl" || savename=="qgl_old" || savename=="qglcdf" || savename=="qgl_noQC") ? 0.3 : 0.6;
  float xMax_legend = xMin_legend + 0.35;

  TLegend* legend = new TLegend( xMin_legend, 0.7, xMax_legend, 0.9, Form("%.0f < p_{T} < %.0f GeV", ptMin, ptMax) );
  legend->SetTextSize(0.038);
  legend->SetFillColor(0);
  legend->AddEntry( h1_quark, "Quarks", "F" );
  legend->AddEntry( h1_gluon, "Gluons", "F" );
  legend->Draw("same");

  h1_quark->DrawNormalized("histo same");
  h1_gluon->DrawNormalized("histo same");


  gPad->RedrawAxis();

  c2->SaveAs( Form("%s/%s_pt%.0f.eps", outputdir.c_str(), savename.c_str(), ptMin) );
  c2->SaveAs( Form("%s/%s_pt%.0f.png", outputdir.c_str(), savename.c_str(), ptMin) );

  delete h2_axes;
  delete c2;


}



