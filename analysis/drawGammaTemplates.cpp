#include <iostream>

#include "TCanvas.h"
#include "TLegend.h"
#include "TH2D.h"
#include "TH1D.h"
#include "TFile.h"

#include "../interface/MT2Analysis.h"
#include "../interface/MT2Region.h"
#include "../interface/MT2EstimateZinvGamma.h"
#include "../interface/MT2DrawTools.h"




void compareRegions( const std::string& outputdir, std::vector<MT2Region> regions, MT2Analysis<MT2EstimateZinvGamma>* analysis, const std::string& suffix="" );


int main() {

  MT2DrawTools::setStyle();

  std::string outputdir = "Plots_GammaTemplates";
  system( Form("mkdir -p %s", outputdir.c_str()) );

  MT2Analysis<MT2EstimateZinvGamma>* zgammaRatio = MT2Analysis<MT2EstimateZinvGamma>::readFromFile("templates_gJet_SAVE.root");
  //zgammaRatio->printRegions();

  std::vector<MT2Region> r_lowHT_vs_njet;
  r_lowHT_vs_njet.push_back( MT2Region( 450., 575., 200., 2,  3, 0, 0 ) );
  r_lowHT_vs_njet.push_back( MT2Region( 450., 575., 200., 4, -1, 0, 0 ) );

  compareRegions( outputdir, r_lowHT_vs_njet, zgammaRatio );

  std::vector<MT2Region> r_medHT_vs_njet;
  r_medHT_vs_njet.push_back( MT2Region( 575., 1000., 200., 2, 3, 0, 0 ) );
  r_medHT_vs_njet.push_back( MT2Region( 575., 1000., 200., 4, -1, 0, 0 ) );

  compareRegions( outputdir, r_medHT_vs_njet, zgammaRatio );

  std::vector<MT2Region> r_highHT_vs_njet;
  r_highHT_vs_njet.push_back( MT2Region( 1000., -1., 30., 2, 3, 0, 0 ) );
  r_highHT_vs_njet.push_back( MT2Region( 1000., -1., 30., 4, -1, 0, 0 ) );

  compareRegions( outputdir, r_highHT_vs_njet, zgammaRatio );


  std::vector<MT2Region> r_highHT_vs_nbjet;
  //r_highHT_vs_njet.push_back( MT2Region( 1000., -1., 30., 2, -1, 0, 0 ) );
  r_highHT_vs_nbjet.push_back( MT2Region( 1000., -1., 30., 2, 3, 0, 0 ) );
  r_highHT_vs_nbjet.push_back( MT2Region( 1000., -1., 30., 2, 3, 1, 1 ) );
  r_highHT_vs_nbjet.push_back( MT2Region( 1000., -1., 30., 2, 3, 2, 2, 200., 200., 400., false ) );

  compareRegions( outputdir, r_highHT_vs_nbjet, zgammaRatio, "_vsb" );


  std::vector<MT2Region> r_vsHT;
  //r_highHT_vs_njet.push_back( MT2Region( 1000., -1., 30., 2, -1, 0, 0 ) );
  r_vsHT.push_back( MT2Region( 450., 575. , 200., 2, 3, 0, 0 ) );
  r_vsHT.push_back( MT2Region( 575., 1000., 200., 2, 3, 0, 0 ) );
  r_vsHT.push_back( MT2Region( 1000., -1. ,  30., 2, 3, 0, 0 ) );

  compareRegions( outputdir, r_vsHT, zgammaRatio );

  return 0;

}



void compareRegions( const std::string& outputdir, std::vector<MT2Region> regions, MT2Analysis<MT2EstimateZinvGamma>* analysis, const std::string& suffix ) {


  bool loopOnHT=true;

  if( regions.size()>1 ) 
    if( (*(regions[0].htRegion())) == (*(regions[1].htRegion())) ) loopOnHT=false;


  std::vector<int> colors;
  colors.push_back( 46 );
  colors.push_back( 29 );
  colors.push_back( 38 );
  colors.push_back( 19 );
  colors.push_back( kRed );
  colors.push_back( kBlack );
  
  std::vector<int> markers;
  markers.push_back( 21 );
  markers.push_back( 20 );
  markers.push_back( 23 );
  markers.push_back( 24 );
  markers.push_back( 25 );
  markers.push_back( 26 );
  
  TCanvas* c1 = new TCanvas( "c1", "", 600, 600 );
  c1->cd();

  TH2D* h2_axes = new TH2D( "axes", "", 10, 0., 10., 10, 0., 0.85 );
  h2_axes->SetXTitle( "Photon Charged Isolation [GeV]" );
  h2_axes->SetYTitle( "Normalized to Unity" );
  h2_axes->Draw("");
  

  std::string legendTitle = (loopOnHT) ? regions[0].sigRegion()->getNiceName() : regions[0].htRegion()->getNiceNames()[0];

  TLegend* legend = new TLegend( 0.45, 0.9-0.1*regions.size(), 0.9, 0.9, legendTitle.c_str() );
  legend->SetTextSize(0.038);
  legend->SetFillColor(0);
  

  for( unsigned i=0; i<regions.size(); ++i ) {

    MT2EstimateZinvGamma* thisEstimate = analysis->get( regions[i] );
    if( thisEstimate==0 ) {
      std::cout << "ERROR! Didn't find estimate for region: " << regions[i].getName() << " ! Exiting." << std::endl;
      exit(119);
    }

    TH1D* thisTemplate = thisEstimate->template_prompt;

    //thisTemplate->SetMarkerSize(1.5);
    //thisTemplate->SetMarkerStyle(markers[i]);
    //thisTemplate->SetMarkerColor(colors[i]);
    thisTemplate->SetLineColor(colors[i]);
    thisTemplate->SetLineWidth(2);
    thisTemplate->DrawNormalized("same" );

    if( loopOnHT )
      legend->AddEntry( thisTemplate, regions[i].htRegion()->getNiceNames()[0].c_str(), "L" );
    else
      legend->AddEntry( thisTemplate, regions[i].sigRegion()->getNiceName().c_str(), "L" );

  }


  legend->Draw("same");

  TPaveText* labelTop = MT2DrawTools::getLabelTop();
  labelTop->Draw("same");

  gPad->RedrawAxis();

  std::string saveName = (loopOnHT) ? regions[0].sigRegion()->getName() : regions[0].htRegion()->getName();

  c1->SaveAs( Form("%s/prompt_%s%s.eps", outputdir.c_str(), saveName.c_str(), suffix.c_str()) );
  c1->SaveAs( Form("%s/prompt_%s%s.png", outputdir.c_str(), saveName.c_str(), suffix.c_str()) );



  // and now log:
  c1->Clear();
  c1->SetLogy();

  TH2D* h2_axes_log = new TH2D( "axes_log", "", 10, 0., 10., 10, 0.01, 1.1 );
  h2_axes_log->SetXTitle( "Photon Charged Isolation [GeV]" );
  h2_axes_log->SetYTitle( "Normalized to Unity" );
  h2_axes_log->Draw("");
  


  for( unsigned i=0; i<regions.size(); ++i ) {

    MT2EstimateZinvGamma* thisEstimate = analysis->get( regions[i] );
    if( thisEstimate==0 ) {
      std::cout << "ERROR! Didn't find estimate for region: " << regions[i].getName() << " ! Exiting." << std::endl;
      exit(119);
    }

    TH1D* thisTemplate = thisEstimate->template_prompt;

    //thisTemplate->SetMarkerSize(1.5);
    //thisTemplate->SetMarkerStyle(markers[i]);
    //thisTemplate->SetMarkerColor(colors[i]);
    thisTemplate->SetLineColor(colors[i]);
    thisTemplate->SetLineWidth(2);
    thisTemplate->DrawNormalized("same" );

    if( loopOnHT )
      legend->AddEntry( thisTemplate, regions[i].htRegion()->getNiceNames()[0].c_str(), "L" );
    else
      legend->AddEntry( thisTemplate, regions[i].sigRegion()->getNiceName().c_str(), "L" );

  }


  legend->Draw("same");

  TPaveText* labelTop = MT2DrawTools::getLabelTop();
  labelTop->Draw("same");

  gPad->RedrawAxis();

  std::string saveName = (loopOnHT) ? regions[0].sigRegion()->getName() : regions[0].htRegion()->getName();

  c1->SaveAs( Form("%s/prompt_%s%s.eps", outputdir.c_str(), saveName.c_str(), suffix.c_str()) );
  c1->SaveAs( Form("%s/prompt_%s%s.png", outputdir.c_str(), saveName.c_str(), suffix.c_str()) );

  delete c1;
  delete h2_axes;

}
