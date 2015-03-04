#include <iostream>

#include "TCanvas.h"
#include "TLegend.h"
#include "TH2D.h"
#include "TFile.h"
#include "TLine.h"
#include "TTree.h"
#include "TH1D.h"
#include "TGraph.h"
#include "TChain.h"

#include "../interface/MT2DrawTools.h"




TGraph* getRoC( TH1D* h1_prompt, TH1D* h1_fake );
void drawTemplatesVsMT2( const std::string& outputdir, const std::string& varName, TTree* tree );
void drawVsMT2( const std::string& outputdir, const std::string& varName, const std::string& name, std::vector<TH1D*> histos, std::vector<float> bins );


int main() {


  MT2DrawTools::setStyle();

  std::string outputdir = "IsoPlots";
  system(Form("mkdir -p %s", outputdir.c_str()));

  //TH1::AddDirectory(kFALSE); // stupid ROOT memory allocation needs this

  TFile* file = TFile::Open("GenIsoCheck_PHYS14_v2_Zinv_noSietaieta_13TeV_inclusive/genIso.root");
  TTree* tree_qcd = (TTree*)file->Get("qcd/HT450toInf_j2toInf_b0toInf/tree_qcd_HT450toInf_j2toInf_b0toInf");
  TTree* tree_gjet = (TTree*)file->Get("gjet/HT450toInf_j2toInf_b0toInf/tree_gjet_HT450toInf_j2toInf_b0toInf");

  TChain* tree = new TChain("t");
  tree->Add( "GenIsoCheck_PHYS14_v2_Zinv_noSietaieta_13TeV_inclusive/genIso.root/qcd/HT450toInf_j2toInf_b0toInf/tree_qcd_HT450toInf_j2toInf_b0toInf");
  tree->Add( "GenIsoCheck_PHYS14_v2_Zinv_noSietaieta_13TeV_inclusive/genIso.root/gjet/HT450toInf_j2toInf_b0toInf/tree_gjet_HT450toInf_j2toInf_b0toInf");




  int nbins = 1200;
  float xmin = 0.;
  float xmax = 1.2;

  TH1D* h1_iso_prompt = new TH1D("iso_prompt", "", nbins, xmin, xmax );
  h1_iso_prompt->Sumw2();
  TH1D* h1_iso_fake = new TH1D("iso_fake", "", nbins, xmin, xmax );
  h1_iso_fake->Sumw2();

  TH1D* h1_isoCP_prompt = new TH1D("isoCP_prompt", "", nbins, xmin, xmax );
  h1_isoCP_prompt->Sumw2();
  TH1D* h1_isoCP_fake = new TH1D("isoCP_fake", "", nbins, xmin, xmax );
  h1_isoCP_fake->Sumw2();

  TH1D* h1_isoCN_prompt = new TH1D("isoCN_prompt", "", nbins, xmin, xmax );
  h1_isoCN_prompt->Sumw2();
  TH1D* h1_isoCN_fake = new TH1D("isoCN_fake", "", nbins, xmin, xmax );
  h1_isoCN_fake->Sumw2();

  TH1D* h1_isoCPN_prompt = new TH1D("isoCPN_prompt", "", nbins, xmin, xmax );
  h1_isoCPN_prompt->Sumw2();
  TH1D* h1_isoCPN_fake = new TH1D("isoCPN_fake", "", nbins, xmin, xmax );
  h1_isoCPN_fake->Sumw2();


//// putting NGIg in BG:
//tree->Project( "iso_prompt", "iso", "weight*(   mcMatchId==22 && genIso<5.   && (sietaieta<0.01 || (sietaieta>0.02 && sietaieta<0.027)))" );
//tree->Project( "iso_fake"  , "iso", "weight*((!(mcMatchId==22 && genIso<5.)) && (sietaieta<0.01 || (sietaieta>0.02 && sietaieta<0.027)))" );

//tree->Project( "isoCP_prompt", "isoCP", "weight*(   mcMatchId==22 && genIso<5.   && (sietaieta<0.01 || (sietaieta>0.02 && sietaieta<0.027)))" );
//tree->Project( "isoCP_fake"  , "isoCP", "weight*((!(mcMatchId==22 && genIso<5.)) && (sietaieta<0.01 || (sietaieta>0.02 && sietaieta<0.027)))" );

//tree->Project( "isoCN_prompt", "isoCN", "weight*(   mcMatchId==22 && genIso<5.   && (sietaieta<0.01 || (sietaieta>0.02 && sietaieta<0.027)))" );
//tree->Project( "isoCN_fake"  , "isoCN", "weight*((!(mcMatchId==22 && genIso<5.)) && (sietaieta<0.01 || (sietaieta>0.02 && sietaieta<0.027)))" );

//tree->Project( "isoCPN_prompt", "isoCPN", "weight*(   mcMatchId==22 && genIso<5.   && (sietaieta<0.01 || (sietaieta>0.02 && sietaieta<0.027)))" );
//tree->Project( "isoCPN_fake"  , "isoCPN", "weight*((!(mcMatchId==22 && genIso<5.)) && (sietaieta<0.01 || (sietaieta>0.02 && sietaieta<0.027)))" );



//// putting NGIg in signal:
//tree->Project( "iso_prompt", "iso", "weight*(mcMatchId==22 && (sietaieta<0.01 || (sietaieta>0.02 && sietaieta<0.027)))" );
//tree->Project( "iso_fake", "iso", "weight*(mcMatchId==0 && (sietaieta<0.01 || (sietaieta>0.02 && sietaieta<0.027)))" );

//tree->Project( "isoCP_prompt", "isoCP", "weight*(mcMatchId==22 && (sietaieta<0.01 || (sietaieta>0.02 && sietaieta<0.027)))" );
//tree->Project( "isoCP_fake", "isoCP", "weight*(mcMatchId==0 && (sietaieta<0.01 || (sietaieta>0.02 && sietaieta<0.027)))" );

//tree->Project( "isoCN_prompt", "isoCN", "weight*(mcMatchId==22 && (sietaieta<0.01 || (sietaieta>0.02 && sietaieta<0.027)))" );
//tree->Project( "isoCN_fake", "isoCN", "weight*(mcMatchId==0 && (sietaieta<0.01 || (sietaieta>0.02 && sietaieta<0.027)))" );

//tree->Project( "isoCPN_prompt", "isoCPN", "weight*(mcMatchId==22 && (sietaieta<0.01 || (sietaieta>0.02 && sietaieta<0.027)))" );
//tree->Project( "isoCPN_fake", "isoCPN", "weight*(mcMatchId==0 && (sietaieta<0.01 || (sietaieta>0.02 && sietaieta<0.027)))" );


  // ignoring NGIg:
  tree->Project( "iso_prompt", "iso", "weight*( mcMatchId==22 && genIso<5.   && (sietaieta<0.01 || (sietaieta>0.02 && sietaieta<0.027)))" );
  tree->Project( "iso_fake"  , "iso", "weight*( mcMatchId==0                 && (sietaieta<0.01 || (sietaieta>0.02 && sietaieta<0.027)))" );

  tree->Project( "isoCP_prompt", "isoCP", "weight*( mcMatchId==22 && genIso<5. && (sietaieta<0.01 || (sietaieta>0.02 && sietaieta<0.027)))" );
  tree->Project( "isoCP_fake"  , "isoCP", "weight*( mcMatchId==0               && (sietaieta<0.01 || (sietaieta>0.02 && sietaieta<0.027)))" );

  tree->Project( "isoCN_prompt", "isoCN", "weight*( mcMatchId==22 && genIso<5. && (sietaieta<0.01 || (sietaieta>0.02 && sietaieta<0.027)))" );
  tree->Project( "isoCN_fake"  , "isoCN", "weight*( mcMatchId==0               && (sietaieta<0.01 || (sietaieta>0.02 && sietaieta<0.027)))" );

  tree->Project( "isoCPN_prompt", "isoCPN", "weight*( mcMatchId==22 && genIso<5. && (sietaieta<0.01 || (sietaieta>0.02 && sietaieta<0.027)))" );
  tree->Project( "isoCPN_fake"  , "isoCPN", "weight*( mcMatchId==0               && (sietaieta<0.01 || (sietaieta>0.02 && sietaieta<0.027)))" );



//tree_gjet->Project( "iso_prompt", "iso", "weight*(mcMatchId==22 && (sietaieta<0.01 || (sietaieta>0.02 && sietaieta<0.027)))" );
//tree_qcd->Project( "iso_fake", "iso", "weight*(mcMatchId==0 && (sietaieta<0.01 || (sietaieta>0.02 && sietaieta<0.027)))" );

//tree_gjet->Project( "isoCP_prompt", "isoCP", "weight*(mcMatchId==22 && (sietaieta<0.01 || (sietaieta>0.02 && sietaieta<0.027)))" );
//tree_qcd->Project( "isoCP_fake", "isoCP", "weight*(mcMatchId==0 && (sietaieta<0.01 || (sietaieta>0.02 && sietaieta<0.027)))" );

//tree_gjet->Project( "isoCN_prompt", "isoCN", "weight*(mcMatchId==22 && (sietaieta<0.01 || (sietaieta>0.02 && sietaieta<0.027)))" );
//tree_qcd->Project( "isoCN_fake", "isoCN", "weight*(mcMatchId==0 && (sietaieta<0.01 || (sietaieta>0.02 && sietaieta<0.027)))" );

//tree_gjet->Project( "isoCPN_prompt", "isoCPN", "weight*(mcMatchId==22 && (sietaieta<0.01 || (sietaieta>0.02 && sietaieta<0.027)))" );
//tree_qcd->Project( "isoCPN_fake", "isoCPN", "weight*(mcMatchId==0 && (sietaieta<0.01 || (sietaieta>0.02 && sietaieta<0.027)))" );


  TGraph* roc_iso = getRoC( h1_iso_prompt, h1_iso_fake );
  TGraph* roc_isoCP = getRoC( h1_isoCP_prompt, h1_isoCP_fake );
  TGraph* roc_isoCN = getRoC( h1_isoCN_prompt, h1_isoCN_fake );
  TGraph* roc_isoCPN = getRoC( h1_isoCPN_prompt, h1_isoCPN_fake );


  TCanvas* c1 = new TCanvas("c1", "", 600, 600 );
  c1->cd();

  //// NGIg in BG:
  //TH2D* h2_axes = new TH2D("axes", "", 10, 0.5, 1.0001, 10, 0.98, 1.0001);
  //// NGIg in signal:
  //TH2D* h2_axes = new TH2D("axes", "", 10, 0.75, 1.0001, 10, 0.75, 1.0001);
  // NGIg ignored:
  TH2D* h2_axes = new TH2D("axes", "", 10, 0.75, 1.0001, 10, 0.95, 1.0001);
  h2_axes->SetXTitle("Fake Photon Rejection");
  h2_axes->SetYTitle("Prompt Photon Efficiency");
  h2_axes->Draw();


  roc_iso->SetMarkerStyle(20);
  roc_iso->SetMarkerSize(1.6);
  roc_iso->SetMarkerColor(kBlack);

  roc_isoCP->SetMarkerStyle(24);
  roc_isoCP->SetMarkerSize(1.6);
  roc_isoCP->SetMarkerColor(kBlack);

  roc_isoCN->SetMarkerStyle(20);
  roc_isoCN->SetMarkerSize(1.6);
  roc_isoCN->SetMarkerColor(46);

  roc_isoCPN->SetMarkerStyle(24);
  roc_isoCPN->SetMarkerSize(1.6);
  roc_isoCPN->SetMarkerColor(46);

  TLegend* legend = new TLegend( 0.2, 0.2, 0.5, 0.48 );
  legend->SetFillColor(0);
  legend->SetTextSize(0.038);
  legend->AddEntry( roc_iso, "Charged", "P" );
  legend->AddEntry( roc_isoCP, "Charged + Photon", "P" );
  legend->AddEntry( roc_isoCN, "Charged + Neutral", "P" );
  legend->AddEntry( roc_isoCPN, "Charged + Photon + Neutral", "P" );
  legend->Draw("same");

  TLine* diag = new TLine( 0., 1., 1., 0. );
  diag->Draw("same");

  TPaveText* labelTop = MT2DrawTools::getLabelTop();
  labelTop->Draw("same");


  roc_iso->Draw("psame");
  roc_isoCP->Draw("psame");
  roc_isoCN->Draw("psame");
  roc_isoCPN->Draw("psame");


  gPad->RedrawAxis();

  c1->SaveAs(Form("%s/isoROC.png", outputdir.c_str()));
  c1->SaveAs(Form("%s/isoROC.pdf", outputdir.c_str()));
  c1->SaveAs(Form("%s/isoROC.eps", outputdir.c_str()));

  TFile* rocFile = TFile::Open("rocFile.root", "recreate");
  rocFile->cd();
  roc_iso->Write();
  roc_isoCP->Write();
  h1_iso_prompt->Write();
  h1_iso_fake->Write();
  h1_isoCP_prompt->Write();
  h1_isoCP_fake->Write();
  rocFile->Close();
  

  drawTemplatesVsMT2( outputdir, "iso", tree );
  drawTemplatesVsMT2( outputdir, "isoCP", tree );

  return 0;

}



void drawTemplatesVsMT2( const std::string& outputdir, const std::string& varName, TTree* tree ) {

  // now templates vs MT2
  std::vector<float> bins;
  bins.push_back(200.);
  bins.push_back(300.);
  bins.push_back(400.);
  bins.push_back(600.);
  bins.push_back(1000.);
  bins.push_back(1500.);

  std::vector<TH1D*> templates_prompt;
  std::vector<TH1D*> templates_fake;
  std::vector<TH1D*> templates_NGIg;

  std::vector<TH1D*> templatesAbs_prompt;
  std::vector<TH1D*> templatesAbs_fake;
  std::vector<TH1D*> templatesAbs_NGIg;

  float k = (varName=="iso") ? 1. : 2.;

  int nBinsPlusOne = 12;
  Double_t isoBins[nBinsPlusOne];
  isoBins[0]  = k*0.;
  isoBins[1]  = k*0.005;
  isoBins[2]  = k*0.01;
  isoBins[3]  = k*0.02;
  isoBins[4]  = k*0.03;
  isoBins[5]  = k*0.04;
  isoBins[6]  = k*0.05;
  isoBins[7]  = k*0.06;
  isoBins[8]  = k*0.07;
  isoBins[9]  = k*0.08;
  isoBins[10] = k*0.09;
  isoBins[11] = k*0.1;



  for( unsigned i=0; i<bins.size()-1; ++i ) {

    std::string promptName(Form("prompt%d", i));
    std::string fakeName(Form("fake%d", i));
    std::string NGIgName(Form("NGIg%d", i));

    TH1D* h1_prompt = new TH1D(promptName.c_str(), "", nBinsPlusOne-1, isoBins);
    TH1D* h1_fake   = new TH1D(  fakeName.c_str(), "", nBinsPlusOne-1, isoBins);
    TH1D* h1_NGIg   = new TH1D(  NGIgName.c_str(), "", nBinsPlusOne-1, isoBins);
    
    h1_prompt->Sumw2();
    h1_fake  ->Sumw2();
    h1_NGIg  ->Sumw2();
    
    tree->Project( promptName.c_str(), varName.c_str(), Form("weight*( mt2>%f && mt2<%f && mcMatchId==22 && genIso<5. && (sietaieta<0.01 || (sietaieta>0.02 && sietaieta<0.027)))", bins[i], bins[i+1]) );
    tree->Project(   NGIgName.c_str(), varName.c_str(), Form("weight*( mt2>%f && mt2<%f && mcMatchId==22 && genIso>5. && (sietaieta<0.01 || (sietaieta>0.02 && sietaieta<0.027)))", bins[i], bins[i+1]) );
    tree->Project(   fakeName.c_str(), varName.c_str(), Form("weight*( mt2>%f && mt2<%f && mcMatchId==0               && (sietaieta<0.01 || (sietaieta>0.02 && sietaieta<0.027)))", bins[i], bins[i+1]) );

    templates_prompt.push_back( h1_prompt );
    templates_fake  .push_back( h1_fake );
    templates_NGIg  .push_back( h1_NGIg );



    std::string promptNameAbs(Form("prompt_abs%d", i));
    std::string fakeNameAbs(Form("fake_abs%d", i));
    std::string NGIgNameAbs(Form("NGIg_abs%d", i));


    int nBinsAbs = (varName=="iso") ? 20  : 30;
    int xMaxAbs  = (varName=="iso") ? 30. : 60.;
    
    TH1D* h1_abs_prompt = new TH1D(promptNameAbs.c_str(), "", nBinsAbs, 0., xMaxAbs);
    TH1D* h1_abs_fake   = new TH1D(  fakeNameAbs.c_str(), "", nBinsAbs, 0., xMaxAbs);
    TH1D* h1_abs_NGIg   = new TH1D(  NGIgNameAbs.c_str(), "", nBinsAbs, 0., xMaxAbs);
    
    h1_abs_prompt->Sumw2();
    h1_abs_fake  ->Sumw2();
    h1_abs_NGIg  ->Sumw2();
    
    tree->Project( promptNameAbs.c_str(), Form("%s*ptGamma", varName.c_str()), Form("weight*( mt2>%f && mt2<%f && mcMatchId==22 && genIso<5. && (sietaieta<0.01 || (sietaieta>0.02 && sietaieta<0.027)))", bins[i], bins[i+1]) );
    tree->Project(   NGIgNameAbs.c_str(), Form("%s*ptGamma", varName.c_str()), Form("weight*( mt2>%f && mt2<%f && mcMatchId==22 && genIso>5. && (sietaieta<0.01 || (sietaieta>0.02 && sietaieta<0.027)))", bins[i], bins[i+1]) );
    tree->Project(   fakeNameAbs.c_str(), Form("%s*ptGamma", varName.c_str()), Form("weight*( mt2>%f && mt2<%f && mcMatchId==0               && (sietaieta<0.01 || (sietaieta>0.02 && sietaieta<0.027)))", bins[i], bins[i+1]) );

    templatesAbs_prompt.push_back( h1_abs_prompt );
    templatesAbs_fake  .push_back( h1_abs_fake );
    templatesAbs_NGIg  .push_back( h1_abs_NGIg );

  }


  drawVsMT2( outputdir, varName, "prompt", templates_prompt, bins );
  drawVsMT2( outputdir, varName, "fake"  , templates_fake  , bins );
  drawVsMT2( outputdir, varName, "NGIg"  , templates_NGIg  , bins );

  drawVsMT2( outputdir, varName, "prompt", templatesAbs_prompt, bins );
  drawVsMT2( outputdir, varName, "fake"  , templatesAbs_fake  , bins );
  drawVsMT2( outputdir, varName, "NGIg"  , templatesAbs_NGIg  , bins );

  for( unsigned i=0; i<templates_prompt.size(); ++i ) {
    delete templates_prompt[i];
    delete templates_fake[i];
    delete templates_NGIg[i];
    delete templatesAbs_prompt[i];
    delete templatesAbs_fake[i];
    delete templatesAbs_NGIg[i];
  }

}



TGraph* getRoC( TH1D* h1_prompt, TH1D* h1_fake ) {

  TGraph* gr = new TGraph(0);

  int nBins = h1_prompt->GetNbinsX()+1;

  for( unsigned iBin=1; iBin<nBins; ++iBin ) {

    float eff_prompt = h1_prompt->Integral(1, iBin)/h1_prompt->Integral(1, nBins+1);
    float eff_fake   = h1_fake  ->Integral(1, iBin)/h1_fake  ->Integral(1, nBins+1);

  //  std::cout << "ibin: " << iBin << std::endl;
  //  std::cout <<  "eff_prompt: " <<  eff_prompt << std::endl;
  //  std::cout <<  "eff_fake  : " <<  eff_fake   << std::endl;

    gr->SetPoint( iBin-1, 1.-eff_fake, eff_prompt );

  }

  return gr;

}


void drawVsMT2( const std::string& outputdir, const std::string& varName, const std::string& name, std::vector<TH1D*> histos, std::vector<float> bins ) {

  bool isPrompt = (name!="fake");
  float xMax = histos[0]->GetXaxis()->GetXmax();
  bool isAbs = (xMax>2.);

  std::vector<int> colors;
  colors.push_back( 46 );
  colors.push_back( 29 );
  colors.push_back( 38 );
  colors.push_back( 42 );
  colors.push_back( kGray+2 );
  

  TCanvas* c1 = new TCanvas( "c2", "", 600, 600 );
  TCanvas* c1_log = new TCanvas( "c2_log", "", 600, 600 );
  c1_log->SetLogy();


  std::string isoLongName = (varName=="iso") ? "Charged" : "Charged + Photon";

  float yMax = (isPrompt) ? 1. : 0.2;
  TH2D* h2_axes = new TH2D( "axes", "", 10, 0., xMax, 10, 0., yMax );
  if( isAbs )
    h2_axes->SetXTitle( Form("%s Isolation [GeV]", isoLongName.c_str()) );
  else
    h2_axes->SetXTitle( Form("Relative %s Isolation", isoLongName.c_str()) );
  h2_axes->SetYTitle( "Normalized to Unity" );
  c1->cd();
  h2_axes->Draw("");


  float yMax_log, yMin_log;
  if( name=="prompt" ) {
    yMin_log = 0.0001;
    yMax_log = 5.;
  } else if( name=="fake" ) {
    yMin_log = 0.001;
    yMax_log = 50.;
  } else if( name=="NGIg" ) {
    yMin_log = 0.001;
    yMax_log = 50.;
  }
  
  
  TH2D* h2_axes_log = new TH2D( "axes_log", "", 10, 0., xMax, 10, yMin_log, yMax_log );
  if( isAbs )
    h2_axes_log->SetXTitle( Form("%s Isolation [GeV]", isoLongName.c_str()) );
  else
    h2_axes_log->SetXTitle( Form("Relative %s Isolation", isoLongName.c_str()) );
  h2_axes_log->SetYTitle( "Normalized to Unity" );
  c1_log->cd();
  h2_axes_log->Draw("");


  int maxHistos = 4;
  TLegend* legend = new TLegend( 0.45, 0.91-0.07*maxHistos, 0.9, 0.91 );
  legend->SetTextSize(0.038);
  legend->SetFillColor(0);
  

  for( unsigned i=0; i<histos.size(); ++i ) {

    if( i>=maxHistos ) break;

    histos[i]->SetLineColor(colors[i]);
    histos[i]->SetLineWidth(2);
    legend->AddEntry( histos[i], Form("%.0f < M_{T2} < %.0f GeV", bins[i], bins[i+1]), "L" );

    c1->cd();
    histos[i]->DrawNormalized("same");

    c1_log->cd();
    histos[i]->DrawNormalized("same");

  }


  TPaveText* labelTop = MT2DrawTools::getLabelTop();

  float xMin_label;
  float yMin_label;
  if( name=="prompt" ) {
    xMin_label = 0.75;
    yMin_label = 0.2;
  } else if( name=="fake" || name=="NGIg" ) {
    xMin_label = 0.2;
    yMin_label = 0.8;
  }
  
  TPaveText* labelPrompt     = new TPaveText( 0.22, 0.8, 0.49, 0.9, "brNDC" );
  TPaveText* labelPrompt_log = new TPaveText( xMin_label, yMin_label, xMin_label+0.2, yMin_label+0.1, "brNDC" );
  labelPrompt   ->SetFillColor(0);
  labelPrompt   ->SetTextSize(0.035);
  labelPrompt   ->SetTextAlign(11); // align left
  labelPrompt_log->SetFillColor(0);
  labelPrompt_log->SetTextSize(0.035);
  labelPrompt_log->SetTextAlign(11); // align left
  if( name=="prompt" ) {
    labelPrompt    ->AddText( "Prompt" );
    labelPrompt_log->AddText( "Prompt" );
  } else if( name=="fake" ) {
    labelPrompt    ->AddText( "Fake" );
    labelPrompt_log->AddText( "Fake" );
  } else if( name=="NGIg" ) {
    labelPrompt    ->AddText( "Non-GenIso" );
    labelPrompt_log->AddText( "Non-GenIso" );
  }
  labelPrompt    ->AddText( "Photons" );
  labelPrompt_log->AddText( "Photons" );


  c1->cd();
  labelTop->Draw("same");
  labelPrompt->Draw("same");
  legend->Draw("same");
  gPad->RedrawAxis();

  c1_log->cd();
  labelTop->Draw("same");
  labelPrompt_log->Draw("same");
  legend->Draw("same");
  gPad->RedrawAxis();


  std::string suffix = "";
  if( isAbs ) suffix = "Abs";

  c1->SaveAs( Form("%s/templ_%s%s_%s_vsMT2.eps", outputdir.c_str(), varName.c_str(), suffix.c_str(), name.c_str()) );
  c1->SaveAs( Form("%s/templ_%s%s_%s_vsMT2.pdf", outputdir.c_str(), varName.c_str(), suffix.c_str(), name.c_str()) );
  c1->SaveAs( Form("%s/templ_%s%s_%s_vsMT2.png", outputdir.c_str(), varName.c_str(), suffix.c_str(), name.c_str()) );

  c1_log->SaveAs( Form("%s/templ_%s%s_%s_vsMT2_log.eps", outputdir.c_str(), varName.c_str(), suffix.c_str(), name.c_str()) );
  c1_log->SaveAs( Form("%s/templ_%s%s_%s_vsMT2_log.pdf", outputdir.c_str(), varName.c_str(), suffix.c_str(), name.c_str()) );
  c1_log->SaveAs( Form("%s/templ_%s%s_%s_vsMT2_log.png", outputdir.c_str(), varName.c_str(), suffix.c_str(), name.c_str()) );


  delete c1;
  delete c1_log;
  delete h2_axes;
  delete h2_axes_log;




}
