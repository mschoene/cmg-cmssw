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



int main() {


  MT2DrawTools::setStyle();

  std::string outputdir = "PurityFitPlots";
  system( Form("mkdir -p %s", outputdir.c_str() ));

  MT2Analysis<MT2Estimate>* purityMC = MT2Analysis<MT2Estimate>::readFromFile( "GammaControlRegion_CSA14_Zinv_13TeV_CSA14/purityMC.root" );

  std::vector< PurityFit > fits;
  fits.push_back( PurityFit( "Full Templates", "13TeV_CSA14"   , MT2Analysis<MT2Estimate>::readFromFile("PurityFits_CSA14_Zinv_13TeV_CSA14/purityFit_CSA14_Zinv_13TeV_CSA14.root"      ), 20, 46 ));
  fits.push_back( PurityFit( "HT Templates"  , "13TeV_onlyHT"  , MT2Analysis<MT2Estimate>::readFromFile("PurityFits_CSA14_Zinv_13TeV_onlyHT/purityFit_CSA14_Zinv_13TeV_onlyHT.root"    ), 21, 29 ));
  fits.push_back( PurityFit( "Jet Templates" , "13TeV_onlyJet" , MT2Analysis<MT2Estimate>::readFromFile("PurityFits_CSA14_Zinv_13TeV_onlyJets/purityFit_CSA14_Zinv_13TeV_onlyJets.root"), 24, 38 ));


  std::set<MT2Region> regions = purityMC->getRegions();

  for( std::set<MT2Region>::iterator iR=regions.begin(); iR!=regions.end(); ++iR ) {

    if( iR->nBJetsMin()>1 ) continue;

    TCanvas* c1 = new TCanvas( "c1", "", 600, 600 );
    c1->cd();


    TH1D* thisPurityMC = purityMC->get( *iR )->yield;
    thisPurityMC->SetLineColor( kBlack );
    thisPurityMC->SetLineWidth( 2 );

    TH2D* axes = new TH2D( "axes", "", 10, thisPurityMC->GetXaxis()->GetXmin(), thisPurityMC->GetXaxis()->GetXmax(), 10, 0., 1. );
    axes->SetXTitle( "MT_{2} [GeV]");
    axes->SetYTitle( "Photon Purity" );
    axes->Draw("");

    thisPurityMC->Draw("same");

    TLegend* legend = new TLegend( 0.55, 0.2, 0.9, 0.2+0.06*(fits.size()+1.) );
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

    TPaveText* labelTop = MT2DrawTools::getLabelTop();
    labelTop->Draw("same");

    std::vector<std::string> regionNames = iR->getNiceNames();
    TPaveText* labelRegion = new TPaveText( 0.23, 0.2, 0.48, 0.35, "brNDC" );
    labelRegion->SetTextSize(0.034); 
    labelRegion->SetFillColor(0);
    for( unsigned i=0; i<regionNames.size(); ++i ) labelRegion->AddText( regionNames[i].c_str() );
    labelRegion->Draw("same");

    gPad->RedrawAxis();

    c1->SaveAs( Form("%s/fits_%s.eps", outputdir.c_str(), iR->getName().c_str()) );
    c1->SaveAs( Form("%s/fits_%s.png", outputdir.c_str(), iR->getName().c_str()) );
    c1->SaveAs( Form("%s/fits_%s.pdf", outputdir.c_str(), iR->getName().c_str()) );

    delete c1;
    delete axes;

  } // for regions


  return 0;

}
