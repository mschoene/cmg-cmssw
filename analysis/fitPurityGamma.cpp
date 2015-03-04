#include <cmath>
#include <iostream>

#ifndef __CINT__
#include "RooGlobalFunc.h"
#endif
#include "RooRealVar.h"
#include "RooDataSet.h"
#include "RooDataHist.h"
#include "RooHistPdf.h"
#include "RooAddPdf.h"
#include "TCanvas.h"
#include "TAxis.h"
#include "TH2D.h"
#include "TH1D.h"
#include "TRandom3.h"
#include "RooPlot.h"

#include "../interface/MT2Analysis.h"
#include "../interface/MT2Region.h"
#include "../interface/MT2Estimate.h"
#include "../interface/MT2EstimateZinvGamma.h"

using namespace RooFit;




//void fitPurity( const std::string& outputdir, TH1D* purity, RooRealVar* x, std::vector<RooDataSet*> data, std::vector<TH1D*> templPrompt, std::vector<TH1D*> templFake );
void fitPurity( const std::string& outputdir, TH1D* purity, RooRealVar* x, std::vector<RooDataSet*> data, TH1D* templPrompt, TH1D* templFake );
void fitSinglePurity( const std::string& outputdir, float& purity, float& purityError, RooRealVar* x, RooDataSet* h1_data, TH1D* h1_templPrompt, TH1D* h1_templFake );





int main( int argc, char* argv[] ) {


  std::string samples = "PHYS14_v2_Zinv";
  //std::string samples = "CSA14_Zinv";

  if( argc==1 ) {
    std::cout << "-> You need to pass me the regions set name. Here are some suggestions: " << std::endl;
    std::cout << "  13TeV_CSA14" << std::endl;
    std::cout << "  13TeV_onlyHT" << std::endl;
    std::cout << "  13TeV_onlyJets" << std::endl;
    std::cout << "  13TeV_inclusive" << std::endl;
    exit(101);
  }


  std::string regionsSet = "13TeV_CSA14";
  if( argc>1 ) {
    std::string regionsSet_tmp(argv[1]); 
    regionsSet = regionsSet_tmp;
  }

  std::string mc_or_data = "MC";
  if( argc>2 ) {
    mc_or_data = std::string(argv[2]);
  }


  TH1::AddDirectory(kFALSE);


  //MT2Analysis<MT2EstimateZinvGamma>* gammaJet_data = MT2Analysis<MT2EstimateZinvGamma>::readFromFile( "GammaControlRegion_CSA14_Zinv_13TeV_CSA14/data.root", "gammaCR" );
  MT2Analysis<MT2EstimateZinvGamma>* gammaJet_data = MT2Analysis<MT2EstimateZinvGamma>::readFromFile( "GammaControlRegion_" + samples + "_13TeV_CSA14/mc.root", "gammaCR" );

  //std::string templateSamples = "CSA14_Zinv";
  MT2Analysis<MT2EstimateZinvGamma>* templates_prompt = MT2Analysis<MT2EstimateZinvGamma>::readFromFile( "gammaTemplatesMC_" + samples + "_" + regionsSet + ".root", "templatesPrompt" );
  //MT2Analysis<MT2EstimateZinvGamma>* templates_prompt = MT2Analysis<MT2EstimateZinvGamma>::readFromFile( "gammaTemplates" + mc_or_data + "_" + samples + "_" + regionsSet + ".root", "templatesPrompt" );
  MT2Analysis<MT2EstimateZinvGamma>* templates_fake   = MT2Analysis<MT2EstimateZinvGamma>::readFromFile( "gammaTemplates" + mc_or_data + "_" + samples + "_" + regionsSet + ".root", "templatesFake" );


  std::string outputdir = "PurityFits" + mc_or_data + "_" + samples + "_" + regionsSet;
  system( Form( "mkdir -p %s", outputdir.c_str()) );

  std::set<MT2Region> regions = gammaJet_data->getRegions();

  MT2Analysis<MT2Estimate>* purity = new MT2Analysis<MT2Estimate>( "purity", "13TeV_CSA14" );


  for( std::set<MT2Region>::iterator iR=regions.begin(); iR!=regions.end(); ++iR ) {

    if( iR->nBJetsMin()>1 ) continue;

    MT2EstimateZinvGamma* thisEstimate = gammaJet_data->get( *iR );

    MT2EstimateZinvGamma* templatePrompt = templates_prompt->get( *(templates_prompt->matchRegion( *iR )) );
    MT2EstimateZinvGamma* templateFake   = templates_fake  ->get( *(templates_fake  ->matchRegion( *iR )) );

    MT2Estimate* thisPurityFit = purity->get( *iR );
    std::string name = thisPurityFit->yield->GetName();


    fitPurity( outputdir, thisPurityFit->yield, thisEstimate->x_, thisEstimate->iso_bins, templatePrompt->iso, templateFake->iso);

    thisPurityFit->yield->SetName( name.c_str() );

  }
    

  purity->writeToFile( outputdir + "/purityFit_" + samples + "_" + regionsSet + ".root" );

  return 0;

}





//void fitPurity( const std::string& outputdir, TH1D* h1_purity, RooRealVar* x, std::vector<RooDataSet*> data, std::vector<TH1D*> templPrompt, std::vector<TH1D*> templFake ) {
void fitPurity( const std::string& outputdir, TH1D* h1_purity, RooRealVar* x, std::vector<RooDataSet*> data, TH1D* templPrompt, TH1D* templFake ) {


  for( unsigned i=0; i<data.size(); ++i ) {

    int ibin = i+1;

    float purity, purityError;
    fitSinglePurity( outputdir, purity, purityError, x, data[i], templPrompt, templFake );

    h1_purity->SetBinContent( ibin, purity );
    h1_purity->SetBinError  ( ibin, purityError );

  }


  return;

}


void fitSinglePurity( const std::string& outputdir, float& purity, float& purityError, RooRealVar* x, RooDataSet* data, TH1D* h1_templPrompt, TH1D* h1_templFake ) {


  if( data->sumEntries() == 0. ) {
    purity=-1;
    purityError=0.;
    return;
  }


  RooDataHist templPrompt("templPrompt", "", *x, h1_templPrompt);
  RooDataHist templFake  ("templFake"  , "", *x, h1_templFake  );

  RooHistPdf pdfPrompt("pdfPrompt", "", *x, templPrompt );
  RooHistPdf pdfFake  ("pdfFake"  , "", *x, templFake   );

  RooRealVar sigFrac("promptFrac","fraction of prompt",0.9,0.,1.) ;
  RooAddPdf  model("model","", RooArgList(pdfPrompt,pdfFake), sigFrac) ;


  float xMaxFit = 0.299;
  x->setRange( "fittingRange", 0., xMaxFit );
  model.fitTo(*data, SumW2Error(kTRUE), Range("fittingRange")); 

  int binMax = h1_templFake->FindBin( xMaxFit );
  float correctionFactor = h1_templFake->Integral( 1, binMax ) / h1_templFake->Integral();
  purity = sigFrac.getVal() * correctionFactor;
  purityError = sigFrac.getError();

  RooPlot* xframe = x->frame();
  data->plotOn(xframe, Binning(h1_templPrompt->GetNbinsX(),  h1_templPrompt->GetXaxis()->GetXmin(), h1_templPrompt->GetXaxis()->GetXmax()) );
  model.plotOn(xframe);

  // Overlay the background component of model with a dashed line
  model.plotOn(xframe,Components(pdfFake),LineStyle(kDashed)) ;

  TCanvas* c1 = new TCanvas("c1", "", 600, 600);
  gPad->SetLeftMargin(0.15);
  TH2D* h2_axes = new TH2D("axes", "", 10, 0., xMaxFit, 10, 0., xframe->GetMaximum()*1.2 );
  h2_axes->Draw();
  xframe->GetYaxis()->SetTitleOffset(1.4); 
  xframe->Draw("same");

  c1->SaveAs(Form("%s/purityFit_%s.eps", outputdir.c_str(), data->GetName()));
  c1->SaveAs(Form("%s/purityFit_%s.png", outputdir.c_str(), data->GetName()));

  delete c1;
  delete xframe;

  return;

}
