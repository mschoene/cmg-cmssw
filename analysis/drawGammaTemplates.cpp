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




void compareRegions( const std::string& outputdir, std::vector<MT2Region> regions, MT2Analysis<MT2EstimateZinvGamma>* templ, bool isPrompt, const std::string& suffix="" );


int main( int argc, char* argv[] ) {


  std::string samples = "CSA14_Zinv";
  if( argc>1 ) {
    std::string samples_tmp(argv[1]); 
    samples = samples_tmp;
  }


  std::string regionsSet = "13TeV_CSA14";

  MT2DrawTools::setStyle();

  std::string outputdir = "Plots_GammaTemplates_" + samples + "_" + regionsSet;
  system( Form("mkdir -p %s", outputdir.c_str()) );

  MT2Analysis<MT2EstimateZinvGamma>* templatesPrompt = MT2Analysis<MT2EstimateZinvGamma>::readFromFile("gammaTemplates_" + samples +  "_" + regionsSet + ".root", "templates");
  MT2Analysis<MT2EstimateZinvGamma>* templatesFake = MT2Analysis<MT2EstimateZinvGamma>::readFromFile("gammaTemplates_" + samples + "_" + regionsSet + ".root", "templatesFake");


  //std::set<MT2Region> regions = templatesPrompt->getRegions();

  //for( std::set<MT2Region>::iterator iR=regions.begin(); iR!=regions.end(); ++iR ) {
  //  drawComparison( outputdir, "gJet_vs_qcd", "Photon Charged Isolation", "GeV", *iR, templatesPrompt, "Prompts in #gamma+Jet", templatesFake, "Prompts in QCD");
  //}

  

  std::vector<MT2Region> r_lowHT_vs_njet;
  r_lowHT_vs_njet.push_back( MT2Region( 450., 575., 2,  3, 0, 0 ) );
  r_lowHT_vs_njet.push_back( MT2Region( 450., 575., 4, -1, 0, 0 ) );
  r_lowHT_vs_njet.push_back( MT2Region( 450., 575., 2,  3, 1, 1 ) );
  r_lowHT_vs_njet.push_back( MT2Region( 450., 575., 4, -1, 1, 1 ) );

  compareRegions( outputdir, r_lowHT_vs_njet, templatesPrompt, true  );
  compareRegions( outputdir, r_lowHT_vs_njet, templatesFake , false );

  std::vector<MT2Region> r_medHT_vs_njet;
  r_medHT_vs_njet.push_back( MT2Region( 575., 1000., 2,  3, 0, 0 ) );
  r_medHT_vs_njet.push_back( MT2Region( 575., 1000., 4, -1, 0, 0 ) );
  r_medHT_vs_njet.push_back( MT2Region( 575., 1000., 2,  3, 1, 1 ) );
  r_medHT_vs_njet.push_back( MT2Region( 575., 1000., 4, -1, 1, 1 ) );

  compareRegions( outputdir, r_medHT_vs_njet, templatesPrompt, true );
  compareRegions( outputdir, r_medHT_vs_njet, templatesFake, false );

  std::vector<MT2Region> r_highHT_vs_njet;
  r_highHT_vs_njet.push_back( MT2Region( 1000., -1., 2,  3, 0, 0 ) );
  r_highHT_vs_njet.push_back( MT2Region( 1000., -1., 4, -1, 0, 0 ) );
  r_highHT_vs_njet.push_back( MT2Region( 1000., -1., 2,  3, 1, 1 ) );
  r_highHT_vs_njet.push_back( MT2Region( 1000., -1., 4, -1, 1, 1 ) );

  compareRegions( outputdir, r_highHT_vs_njet, templatesPrompt, true );
  compareRegions( outputdir, r_highHT_vs_njet, templatesFake, false );


  //std::vector<MT2Region> r_medHT_vs_nbjet;
  //r_medHT_vs_nbjet.push_back( MT2Region( 575., 1000., 200., 2, 3, 0, 0 ) );
  //r_medHT_vs_nbjet.push_back( MT2Region( 575., 1000., 200., 2, 3, 1, 1 ) );
  //r_medHT_vs_nbjet.push_back( MT2Region( 575., 1000., 200., 2, 3, 2, 2, 200., 200., 400., false ) );

  //compareRegions( outputdir, r_medHT_vs_nbjet, templatesPrompt, templatesFake, "_vsb" );


  //std::vector<MT2Region> r_highHT_vs_nbjet;
  //r_highHT_vs_nbjet.push_back( MT2Region( 1000., -1., 30., 2, 3, 0, 0 ) );
  //r_highHT_vs_nbjet.push_back( MT2Region( 1000., -1., 30., 2, 3, 1, 1 ) );
  //r_highHT_vs_nbjet.push_back( MT2Region( 1000., -1., 30., 2, 3, 2, 2, 200., 200., 400., false ) );

  //compareRegions( outputdir, r_highHT_vs_nbjet, templatesPrompt, templatesFake, "_vsb" );


  std::vector<MT2Region> r_vsHT;
  //r_highHT_vs_njet.push_back( MT2Region( 1000., -1., 30., 2, -1, 0, 0 ) );
  r_vsHT.push_back( MT2Region( 450., 575. , 2, 3, 0, 0 ) );
  r_vsHT.push_back( MT2Region( 575., 1000., 2, 3, 0, 0 ) );
  r_vsHT.push_back( MT2Region( 1000., -1. , 2, 3, 0, 0 ) );

  compareRegions( outputdir, r_vsHT, templatesPrompt, true );
  compareRegions( outputdir, r_vsHT, templatesFake , false );



  std::vector<MT2Region> r_vsHT2;
  //r_highHT_vs_njet.push_back( MT2Region( 1000., -1., 30., 2, -1, 0, 0 ) );
  r_vsHT2.push_back( MT2Region( 450., 575. , 4, -1, 0, 0 ) );
  r_vsHT2.push_back( MT2Region( 575., 1000., 4, -1, 0, 0 ) );
  r_vsHT2.push_back( MT2Region( 1000., -1. , 4, -1, 0, 0 ) );

  compareRegions( outputdir, r_vsHT2, templatesPrompt, true );
  compareRegions( outputdir, r_vsHT2, templatesFake , false );

  return 0;

}



void compareRegions( const std::string& outputdir, std::vector<MT2Region> regions, MT2Analysis<MT2EstimateZinvGamma>* templ, bool isPrompt, const std::string& suffix ) {
//void compareRegions( const std::string& outputdir, std::vector<MT2Region> regions, MT2Analysis<MT2EstimateZinvGamma>* templ, MT2Analysis<MT2EstimateZinvGamma>* templ_qcd, const std::string& suffix ) {


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
  markers.push_back( 23 );
  markers.push_back( 24 );
  markers.push_back( 25 );
  markers.push_back( 26 );
  


  TCanvas* c1 = new TCanvas( "c1", "", 600, 600 );
  TCanvas* c1_log = new TCanvas( "c1_log", "", 600, 600 );
  c1_log->SetLogy();

  float xMax = 1.;
  //float xMax = (isPrompt) ? 8. : 50.;

  TH2D* h2_axes = new TH2D( "axes", "", 10, 0., xMax, 10, 0., 0.85 );
  h2_axes->SetXTitle( "Photon Relative Charged Isolation" );
  h2_axes->SetYTitle( "Normalized to Unity" );
  c1->cd();
  h2_axes->Draw("");


  float yMax_log = (isPrompt) ? 5. : 50.;
  float yMin_log = (isPrompt) ? 0.0001 : 0.001;
  
  TH2D* h2_axes_log = new TH2D( "axes_log", "", 10, 0., xMax, 10, yMin_log, yMax_log );
  h2_axes_log->SetXTitle( "Photon Relative Charged Isolation" );
  h2_axes_log->SetYTitle( "Normalized to Unity" );
  c1_log->cd();
  h2_axes_log->Draw("");

  std::string legendTitle = (loopOnHT) ? regions[0].sigRegion()->getNiceName() : regions[0].htRegion()->getNiceName();

  TLegend* legend = new TLegend( 0.45, 0.9-0.08*regions.size(), 0.9, 0.9, legendTitle.c_str() );
  legend->SetTextSize(0.038);
  legend->SetFillColor(0);
  

  for( unsigned i=0; i<regions.size(); ++i ) {

    MT2EstimateZinvGamma* thisEstimate = templ->get( regions[i] );
    if( thisEstimate==0 ) {
      std::cout << "ERROR! Didn't find estimate for region: " << regions[i].getName() << " ! Exiting." << std::endl;
      exit(119);
    }

    TH1D* thisTempl = thisEstimate->iso;
    thisTempl->SetLineColor(colors[i]);
    thisTempl->SetLineWidth(2);

    c1->cd();
    thisTempl->DrawNormalized("same" );
    c1_log->cd();
    thisTempl->DrawNormalized("same" );

    if( loopOnHT )
      legend->AddEntry( thisTempl, regions[i].htRegion()->getNiceName().c_str(), "L" );
    else
      legend->AddEntry( thisTempl, regions[i].sigRegion()->getNiceName().c_str(), "L" );

  }



  TPaveText* labelTop = MT2DrawTools::getLabelTop();

  TPaveText* labelPrompt = new TPaveText( 0.2, 0.8, 0.49, 0.9, "brNDC" );
  labelPrompt->SetFillColor(0);
  labelPrompt->SetTextSize(0.035);
  labelPrompt->SetTextAlign(11); // align left
  if( isPrompt )
    labelPrompt->AddText( "Prompt" );
  else
    labelPrompt->AddText( "Fake" );
  labelPrompt->AddText( "Photons" );


  c1->cd();
  labelTop->Draw("same");
  labelPrompt->Draw("same");
  legend->Draw("same");
  gPad->RedrawAxis();

  c1_log->cd();
  labelTop->Draw("same");
  labelPrompt->Draw("same");
  legend->Draw("same");
  gPad->RedrawAxis();


  std::string saveName = (loopOnHT) ? regions[0].sigRegion()->getName() : regions[0].htRegion()->getName();

  std::string prefix = (isPrompt) ? "Prompt" : "Fake";

  c1->SaveAs( Form("%s/templ%s_%s%s.eps", outputdir.c_str(), prefix.c_str(), saveName.c_str(), suffix.c_str()) );
  c1->SaveAs( Form("%s/templ%s_%s%s.pdf", outputdir.c_str(), prefix.c_str(), saveName.c_str(), suffix.c_str()) );
  c1->SaveAs( Form("%s/templ%s_%s%s.png", outputdir.c_str(), prefix.c_str(), saveName.c_str(), suffix.c_str()) );

  c1_log->SaveAs( Form("%s/templ%s_%s%s_log.eps", outputdir.c_str(), prefix.c_str(), saveName.c_str(), suffix.c_str()) );
  c1_log->SaveAs( Form("%s/templ%s_%s%s_log.pdf", outputdir.c_str(), prefix.c_str(), saveName.c_str(), suffix.c_str()) );
  c1_log->SaveAs( Form("%s/templ%s_%s%s_log.png", outputdir.c_str(), prefix.c_str(), saveName.c_str(), suffix.c_str()) );

  delete c1;
  delete c1_log;
  delete h2_axes;
  delete h2_axes_log;

}
