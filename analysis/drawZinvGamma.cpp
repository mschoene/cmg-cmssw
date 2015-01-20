#include <iostream>

#include "TCanvas.h"
#include "TLegend.h"
#include "TH2D.h"
#include "TH1D.h"
#include "TFile.h"

#include "../interface/MT2Analysis.h"
#include "../interface/MT2Region.h"
#include "../interface/MT2Estimate.h"
#include "../interface/MT2DrawTools.h"




void compareRegions( const std::string& outputdir, std::vector<MT2Region> regions, MT2Analysis<MT2Estimate>* analysis, const std::string& suffix="" );


int main( int argc, char* argv[] ) {


  std::string dir = "ZinvEstimateFromGamma_CSA14_Zinv_13TeV_CSA14";
  if( argc>1 ) {
    std::string dir_tmp(argv[1]); 
    dir = dir_tmp;
  }

  MT2DrawTools::setStyle();

  std::string outputdir = dir + "/plots";
  system( Form("mkdir -p %s", outputdir.c_str()) );

  MT2Analysis<MT2Estimate>* zgammaRatio = MT2Analysis<MT2Estimate>::readFromFile(dir + "/mc.root", "ZgammaRatio");
  //zgammaRatio->printRegions();

  std::vector<MT2Region> r_lowHT_vs_njet;
  r_lowHT_vs_njet.push_back( MT2Region( 450., 575., 200., 2,  3, 0, 0 ) );
  r_lowHT_vs_njet.push_back( MT2Region( 450., 575., 200., 4, -1, 0, 0 ) );
  r_lowHT_vs_njet.push_back( MT2Region( 450., 575., 200., 2,  3, 1, 1 ) );
  r_lowHT_vs_njet.push_back( MT2Region( 450., 575., 200., 4, -1, 1, 1 ) );

  compareRegions( outputdir, r_lowHT_vs_njet, zgammaRatio );

  std::vector<MT2Region> r_medHT_vs_njet;
  r_medHT_vs_njet.push_back( MT2Region( 575., 1000., 200., 2, 3, 0, 0 ) );
  r_medHT_vs_njet.push_back( MT2Region( 575., 1000., 200., 4, -1, 0, 0 ) );
  r_medHT_vs_njet.push_back( MT2Region( 575., 1000., 200., 2,  3, 1, 1 ) );
  r_medHT_vs_njet.push_back( MT2Region( 575., 1000., 200., 4, -1, 1, 1 ) );

  compareRegions( outputdir, r_medHT_vs_njet, zgammaRatio );

  std::vector<MT2Region> r_highHT_vs_njet;
  r_highHT_vs_njet.push_back( MT2Region( 1000., -1., 30., 2, 3, 0, 0 ) );
  r_highHT_vs_njet.push_back( MT2Region( 1000., -1., 30., 4, -1, 0, 0 ) );
  r_highHT_vs_njet.push_back( MT2Region( 1000., -1., 30., 2,  3, 1, 1 ) );
  r_highHT_vs_njet.push_back( MT2Region( 1000., -1., 30., 4, -1, 1, 1 ) );

  compareRegions( outputdir, r_highHT_vs_njet, zgammaRatio );


  std::vector<MT2Region> r_vsHT;
  //r_highHT_vs_njet.push_back( MT2Region( 1000., -1., 30., 2, -1, 0, 0 ) );
  r_vsHT.push_back( MT2Region( 450., 575. , 200., 2, 3, 0, 0 ) );
  r_vsHT.push_back( MT2Region( 575., 1000., 200., 2, 3, 0, 0 ) );
  r_vsHT.push_back( MT2Region( 1000., -1. ,  30., 2, 3, 0, 0 ) );

  compareRegions( outputdir, r_vsHT, zgammaRatio );



  std::vector<MT2Region> r_vsHT2;
  //r_highHT_vs_njet.push_back( MT2Region( 1000., -1., 30., 2, -1, 0, 0 ) );
  r_vsHT2.push_back( MT2Region( 450., 575. , 200., 4, -1, 0, 0 ) );
  r_vsHT2.push_back( MT2Region( 575., 1000., 200., 4, -1, 0, 0 ) );
  r_vsHT2.push_back( MT2Region( 1000., -1. ,  30., 4, -1, 0, 0 ) );

  compareRegions( outputdir, r_vsHT2, zgammaRatio );


  return 0;

}



void compareRegions( const std::string& outputdir, std::vector<MT2Region> regions, MT2Analysis<MT2Estimate>* analysis, const std::string& suffix ) {

  bool loopOnHT=true;

  if( regions.size()>1 ) 
    if( (*(regions[0].htRegion())) == (*(regions[1].htRegion())) ) loopOnHT=false;



  std::vector<int> colors;
  colors.push_back( 46 );
  colors.push_back( 29 );
  colors.push_back( 38 );
  colors.push_back( 42 );
  colors.push_back( kRed );
  colors.push_back( kBlack );
  
  std::vector<int> markers;
  markers.push_back( 21 );
  markers.push_back( 20 );
  markers.push_back( 25 );
  markers.push_back( 24 );
  markers.push_back( 26 );
  
  TCanvas* c1 = new TCanvas( "c1", "", 600, 600 );
  c1->cd();

  TH2D* h2_axes = new TH2D( "axes", "", 10, 200., 1000., 10, 0., 1.5 );
  h2_axes->SetXTitle( "M_{T2} [GeV]" );
  h2_axes->SetYTitle( "Z/#gamma Ratio" );
  h2_axes->Draw("");
  

  std::string legendTitle = (loopOnHT) ? regions[0].sigRegion()->getNiceName() : regions[0].htRegion()->getNiceNames()[0];

  TLegend* legend = new TLegend( 0.2, 0.9-0.07*regions.size(), 0.55, 0.9, legendTitle.c_str() );
  legend->SetTextSize(0.038);
  legend->SetFillColor(0);
  

  for( unsigned i=0; i<regions.size(); ++i ) {

    MT2Estimate* thisEstimate = analysis->get( regions[i] );
    if( thisEstimate==0 ) {
      std::cout << "ERROR! Didn't find estimate for region: " << regions[i].getName() << " ! Exiting." << std::endl;
      exit(119);
    }

    TH1D* thisRatio = thisEstimate->yield;

    thisRatio->SetMarkerSize(1.);
    thisRatio->SetMarkerStyle(markers[i]);
    thisRatio->SetMarkerColor(colors[i]);
    //thisRatio->SetLineColor(kBlack);

    thisRatio->SetLineWidth(2);
    thisRatio->SetLineColor(colors[i]);

    thisRatio->Draw("same" );

    if( loopOnHT )
      legend->AddEntry( thisRatio, regions[i].htRegion()->getNiceNames()[0].c_str(), "PL" );
    else
      legend->AddEntry( thisRatio, regions[i].sigRegion()->getNiceName().c_str(), "PL" );

  }


  legend->Draw("same");

  TPaveText* labelTop = MT2DrawTools::getLabelTop();
  labelTop->Draw("same");

  gPad->RedrawAxis();

  std::string saveName = (loopOnHT) ? regions[0].sigRegion()->getName() : regions[0].htRegion()->getName();

  c1->SaveAs( Form("%s/ZgammaRatio_%s%s.eps", outputdir.c_str(), saveName.c_str(), suffix.c_str()) );
  c1->SaveAs( Form("%s/ZgammaRatio_%s%s.pdf", outputdir.c_str(), saveName.c_str(), suffix.c_str()) );
  c1->SaveAs( Form("%s/ZgammaRatio_%s%s.png", outputdir.c_str(), saveName.c_str(), suffix.c_str()) );

  delete c1;
  delete h2_axes;

}
