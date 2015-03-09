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




class PurityFit {

 public:
  PurityFit( const std::string& aNiceName, const std::string& aRegionsSet, MT2Analysis<MT2Estimate>* apurity, int amarker, int acolor ) {
    niceName = aNiceName;
    regionsSet = aRegionsSet;
    purity = apurity;
    marker = amarker;
    color = acolor;
  }

  std::string niceName;
  std::string regionsSet;
  MT2Analysis<MT2Estimate>* purity;
  int marker;
  int color;

 private:

};




void doAllPurityPlots( const std::string& outputdir, const std::string& samples, const std::string& mc_or_data, const std::string& purityName );



int main( int argc, char* argv[] ) {


  MT2DrawTools::setStyle();

  std::string mc_or_data = "MC";
  if( argc>1 ) {
    mc_or_data = std::string(argv[1]);
  }

  //std::string samples = "CSA14_Zinv";
  std::string samples = "PHYS14_v2_Zinv";

  std::string outputdir = "PurityFitPlots" + mc_or_data + "_" + samples;
  system( Form("mkdir -p %s", outputdir.c_str() ));

  
  doAllPurityPlots( outputdir, samples, mc_or_data, "purityLoose" ); 
  doAllPurityPlots( outputdir, samples, mc_or_data, "purity" ); 

  return 0;

}





void doAllPurityPlots( const std::string& outputdir, const std::string& samples, const std::string& mc_or_data, const std::string& purityName ) {


  //MT2Analysis<MT2Estimate>* purityMC = MT2Analysis<MT2Estimate>::readFromFile( "GammaControlRegion_CSA14_Zinv_13TeV_inclusive/purityMC.root" );
  MT2Analysis<MT2Estimate>* purityMC = MT2Analysis<MT2Estimate>::readFromFile( "GammaControlRegion_" + samples + "_13TeV_CSA14/purityMC.root", purityName );

  std::vector< PurityFit > fits;
  if( mc_or_data=="MC" ) {
    fits.push_back( PurityFit( "All Bins"  , "13TeV_CSA14"     , MT2Analysis<MT2Estimate>::readFromFile("PurityFitsMC_" + samples + "_13TeV_CSA14/purityFit_"     + samples + "_13TeV_CSA14.root"    , purityName), 20, kRed+2 ));
    fits.push_back( PurityFit( "HT Bins"   , "13TeV_onlyHT"    , MT2Analysis<MT2Estimate>::readFromFile("PurityFitsMC_" + samples + "_13TeV_onlyHT/purityFit_"    + samples + "_13TeV_onlyHT.root"   , purityName), 21, 29 ));
    fits.push_back( PurityFit( "Jet Bins"  , "13TeV_onlyJet"   , MT2Analysis<MT2Estimate>::readFromFile("PurityFitsMC_" + samples + "_13TeV_onlyJets/purityFit_"  + samples + "_13TeV_onlyJets.root" , purityName), 24, kAzure ));
    fits.push_back( PurityFit( "Inclusive" , "13TeV_inclusive" , MT2Analysis<MT2Estimate>::readFromFile("PurityFitsMC_" + samples + "_13TeV_inclusive/purityFit_" + samples + "_13TeV_inclusive.root", purityName), 25, kOrange+1 ));
  } else {
    fits.push_back( PurityFit( "Inclusive (MC)"  , "13TeV_inclusive" , MT2Analysis<MT2Estimate>::readFromFile("PurityFitsMC_"   + samples + "_13TeV_inclusive/purityFit_" + samples + "_13TeV_inclusive.root", purityName), 21, 29 ));
    fits.push_back( PurityFit( "Inclusive (Data)", "13TeV_inclusive" , MT2Analysis<MT2Estimate>::readFromFile("PurityFitsData_" + samples + "_13TeV_inclusive/purityFit_" + samples + "_13TeV_inclusive.root", purityName), 20, kOrange+1 ));
  }


  std::set<MT2Region> regions = purityMC->getRegions();

  for( std::set<MT2Region>::iterator iR=regions.begin(); iR!=regions.end(); ++iR ) {

    if( iR->nBJetsMin()>1 ) continue;

    TCanvas* c1 = new TCanvas( "c1", "", 600, 600 );
    c1->cd();


    TH1D* thisPurityMC = purityMC->get( *iR )->yield;
    thisPurityMC->SetLineColor( kBlack );
    thisPurityMC->SetLineWidth( 2 );


    float yMin = (purityName=="purity") ? 0.5 : 0.;

    TH2D* axes = new TH2D( "axes", "", 10, thisPurityMC->GetXaxis()->GetXmin(), thisPurityMC->GetXaxis()->GetXmax(), 10, yMin, 1.0001 );
    axes->SetXTitle( "M_{T2} [GeV]");
    axes->SetYTitle( "Photon Purity" );
    axes->Draw("");

    TPaveText* labelTop = MT2DrawTools::getLabelTop();
    labelTop->Draw("same");


    thisPurityMC->Draw("same");

    //TLegend* legend = new TLegend( 0.175, 0.2, 0.65, 0.2+0.06*(fits.size()+1.) );
    float xMin_legend = (mc_or_data=="MC") ? 0.65 : 0.58;
    TLegend* legend = new TLegend( xMin_legend, 0.2, 0.9, 0.2+0.06*(fits.size()+1.) );
    legend->SetTextSize(0.038); 
    legend->SetFillColor(0);
    legend->AddEntry( thisPurityMC, "MC Purity", "L" );
   
    for( unsigned i=0; i<fits.size(); ++i ) {

      TH1D* thisPurityFit = fits[i].purity->get( *iR )->yield;
      thisPurityFit->SetMarkerStyle( fits[i].marker );
      thisPurityFit->SetMarkerColor( fits[i].color );
      thisPurityFit->SetLineColor( fits[i].color );
      thisPurityFit->SetMarkerSize( 1.3 );
      thisPurityFit->Draw("Psame");

      legend->AddEntry( thisPurityFit, fits[i].niceName.c_str(), "P" );

    }

    legend->Draw("same");

    std::vector<std::string> regionNames = iR->getNiceNames();
    //TPaveText* labelRegion = new TPaveText( 0.57, 0.2, 0.92, 0.35, "brNDC" );
    TPaveText* labelRegion = new TPaveText( 0.23, 0.18, 0.48, 0.29, "brNDC" );
    labelRegion->SetTextSize(0.034); 
    labelRegion->SetFillColor(0);
    for( unsigned i=0; i<regionNames.size(); ++i ) labelRegion->AddText( regionNames[i].c_str() );
    labelRegion->Draw("same");

    gPad->RedrawAxis();

    c1->SaveAs( Form("%s/fits_%s_%s.eps", outputdir.c_str(), purityName.c_str(), iR->getName().c_str()) );
    c1->SaveAs( Form("%s/fits_%s_%s.png", outputdir.c_str(), purityName.c_str(), iR->getName().c_str()) );
    c1->SaveAs( Form("%s/fits_%s_%s.pdf", outputdir.c_str(), purityName.c_str(), iR->getName().c_str()) );

    delete c1;
    delete axes;

  } // for regions


}
