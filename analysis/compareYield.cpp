#include "interface/MT2Analysis.h"
#include "interface/MT2EstimateSyst.h"
#include "interface/MT2DrawTools.h"

#include <iostream>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <string>


#include "TTreeFormula.h"
#include "TH1D.h"
#include "TH2D.h"
#include "THStack.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "TPaveText.h"
#include "TGraphAsymmErrors.h"

float lumi =5.0; //fb-1 

void drawYields( const std::string& outputdir, MT2Analysis<MT2EstimateSyst>* data, std::vector<MT2Analysis<MT2EstimateSyst>* > bgYields );

int main() {

  TH1::AddDirectory(kFALSE); // stupid ROOT memory allocation needs this

  std::string outputdir = "/scratch/mmasciov/CMSSW_7_0_6_patch3_MT2Analysis2015/src/MT2Analysis2015/analysis/YieldComparison/";

  //std::string firstInputFile = "/shome/mmasciov/analyses_fp.root";
  std::string firstInputFile  = "/scratch/mmasciov/CMSSW_7_0_6_patch3_MT2Analysis2015/src/MT2Analysis2015/analysis/EventYields_mc_CSA14_dummy/analyses.root";
  std::string secondInputFile = "/scratch/mmasciov/CMSSW_7_0_6_patch3_MT2Analysis2015/src/MT2Analysis2015/analysis/MT2LostLeptonEstimate_v0.root";

  MT2Analysis<MT2EstimateSyst>* analysisTop = MT2Analysis<MT2EstimateSyst>::readFromFile(firstInputFile.c_str(), "Top");
 
  MT2Analysis<MT2EstimateSyst>* analysisWJets = MT2Analysis<MT2EstimateSyst>::readFromFile(firstInputFile.c_str(), "WJets");
 
  MT2Analysis<MT2EstimateSyst>* analysisSum = new MT2Analysis<MT2EstimateSyst>( (*analysisTop) );
  (*analysisSum) += (*analysisWJets);
  analysisSum->setName("fromRegioEventYield");
  analysisSum->writeToFile( "LostLeptonFromRegionEventYield.root" );

  MT2Analysis<MT2EstimateSyst>* analysisLostLepton = MT2Analysis<MT2EstimateSyst>::readFromFile( secondInputFile.c_str() );

  std::vector< MT2Analysis<MT2EstimateSyst> *> bgEstimate;
  bgEstimate.push_back( analysisLostLepton );

  drawYields(outputdir.c_str(), analysisSum, bgEstimate );

  return 0;

}

void drawYields( const std::string& outputdir, MT2Analysis<MT2EstimateSyst>* fromRegionEventYield, std::vector< MT2Analysis<MT2EstimateSyst> *> bgYields ) {

  MT2DrawTools::setStyle();

  system(Form("mkdir -p %s", outputdir.c_str()));

  std::vector<int> colors;
//  if( bgYields.size()==3 ) { // estimates
//    colors.push_back(402);
//    colors.push_back(430);
//    colors.push_back(418);
//  } else { // mc
//    colors.push_back(401); // qcd
//    colors.push_back(417); // w+jets
//    colors.push_back(419); // z+jets
//    colors.push_back(855); // top
//    //colors.push_back(); // other
//  }
  colors.push_back(430);
  

  std::set<MT2HTRegion> HTRegions = fromRegionEventYield->getHTRegions();
  std::set<MT2SignalRegion> signalRegions = fromRegionEventYield->getSignalRegions();


  for( std::set<MT2HTRegion>::iterator iHT = HTRegions.begin(); iHT!=HTRegions.end(); ++iHT ) {

    for( std::set<MT2SignalRegion>::iterator iSR = signalRegions.begin(); iSR!=signalRegions.end(); ++iSR ) {

      std::string fullPath = outputdir;

      MT2Region thisRegion( (*iHT), (*iSR) );

      TH1D* h1_fromRegionEventYield = fromRegionEventYield->get(thisRegion)->yield;

      TFile* histoFile = TFile::Open( Form("%s/histograms_%s.root", fullPath.c_str(), thisRegion.getName().c_str()), "recreate" );
      histoFile->cd();
      h1_fromRegionEventYield->Write();
      h1_fromRegionEventYield->SetMarkerStyle(20);
      h1_fromRegionEventYield->SetMarkerSize(1.6);

      TCanvas* c1 = new TCanvas( "c1", "", 600, 600 );
      c1->cd();

      float xMin = h1_fromRegionEventYield->GetXaxis()->GetXmin();
      float xMax = h1_fromRegionEventYield->GetXaxis()->GetXmax();
      float yMax1 = h1_fromRegionEventYield->GetMaximum()*1.5;
      float yMax2 = 1.2*(h1_fromRegionEventYield->GetMaximum() + h1_fromRegionEventYield->GetBinError(h1_fromRegionEventYield->GetMaximumBin()));
      float yMax = (yMax1>yMax2) ? yMax1 : yMax2;

      TH2D* h2_axes = new TH2D("axes", "", 10, xMin, xMax, 10, 0., yMax );
      h2_axes->SetXTitle("M_{T2} [GeV]");
      h2_axes->SetYTitle("Entries");

      h2_axes->Draw();

      THStack bgStack("bgStack", "");
      for( unsigned i=0; i<bgYields.size(); ++i ) { // reverse ordered stack is prettier
        int index = bgYields.size() - i - 1;
        TH1D* h1_bg = bgYields[index]->get(thisRegion)->yield;
        h1_bg->SetFillColor( colors[index] );
        h1_bg->SetLineColor( kBlack );
        bgStack.Add(h1_bg);
      }

      std::vector<std::string> niceNames = thisRegion.getNiceNames();

      for( unsigned i=0; i<niceNames.size(); ++i ) {

        float yMax = 0.9-(float)i*0.05;
        float yMin = yMax - 0.05;
        TPaveText* regionText = new TPaveText( 0.18, yMin, 0.55, yMax, "brNDC" );
        regionText->SetTextSize(0.035);
        regionText->SetTextFont(42);
        regionText->SetFillColor(0);
        regionText->SetTextAlign(11);
        regionText->AddText( niceNames[i].c_str() );
        regionText->Draw("same");

      }


      TLegend* legend = new TLegend( 0.7, 0.9-(bgYields.size()+1)*0.06, 0.93, 0.9 );
      legend->SetTextSize(0.038);
      legend->SetTextFont(42);
      legend->SetFillColor(0);
      legend->AddEntry( h1_fromRegionEventYield, "MC only", "P" );
      histoFile->cd();
      for( unsigned i=0; i<bgYields.size(); ++i ) {
        TH1D* h1_bg = bgYields[i]->get(thisRegion)->yield;
        legend->AddEntry( h1_bg, bgYields[i]->fullName.c_str(), "F" );
        h1_bg->Write();
      }

      histoFile->Close();

      legend->Draw("same");
      bgStack.Draw("histo same");
      h1_fromRegionEventYield->Draw("p, same");

      TPaveText* labelTop = MT2DrawTools::getLabelTop(lumi);
      labelTop->Draw("same");

      gPad->RedrawAxis();

      c1->SaveAs( Form("%s/mt2_%s.eps", fullPath.c_str(), thisRegion.getName().c_str()) );
      c1->SaveAs( Form("%s/mt2_%s.png", fullPath.c_str(), thisRegion.getName().c_str()) );

      delete c1;
      delete h2_axes;

    }  // for signal regions

  } // for HT regions

}
