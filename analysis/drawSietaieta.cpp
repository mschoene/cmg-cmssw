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







int main( int argc, char* argv[] ) {



  MT2DrawTools::setStyle();


  TFile* file = TFile::Open("gammaTemplatesDummy_PHYS14_v2_Zinv_13TeV_inclusive.root");
  TH1D* h1_sigma = (TH1D*)file->Get("templatesAll/HT450toInf_j2toInf_b0toInf/sietaieta_templatesAll_HT450toInf_j2toInf_b0toInf");

  TCanvas* c1 = new TCanvas("c1", "", 600, 600);
  c1->cd();

  h1_sigma->Rebin(2);
  h1_sigma->SetFillColor(46);
  h1_sigma->SetLineColor(kBlack);

  float yMax = 1.1*h1_sigma->GetMaximum();
  TH2D* h2_axes = new TH2D("axes", "", 10, 0., 0.035, 10, 0., yMax );
  h2_axes->SetXTitle("Photon #sigma_{i#eta i#eta}");
  h2_axes->SetYTitle("Events");
  h2_axes->Draw();

  float xMaxEB_SB = 0.0125;
  TBox* box_eb = new TBox( 0.01, 0., xMaxEB_SB, yMax );
  box_eb->SetFillColor(17); 
  box_eb->Draw("same");

  TBox* box_ee = new TBox( 0.03, 0., 0.035, yMax );
  box_ee->SetFillColor(17); 
  box_ee->Draw("same");

  h1_sigma->Draw("same");


  TLine* lineEB = new TLine( 0.01, 0., 0.01, yMax );
  lineEB->Draw("same");

  TLine* lineEB_SB = new TLine( xMaxEB_SB, 0., xMaxEB_SB, yMax );
  lineEB_SB->Draw("same");

  TLine* lineEE = new TLine( 0.03, 0., 0.03, yMax );
  lineEE->Draw("same");

  TPaveText* labelTop = MT2DrawTools::getLabelTop(4.);
  labelTop->Draw("same");

  gPad->RedrawAxis();

  c1->SaveAs("sietaieta.eps");
  c1->SaveAs("sietaieta.pdf");
  c1->SaveAs("sietaieta.png");

  // print yields
  int ebMin    = h1_sigma->FindBin(0.);
  int ebMax    = h1_sigma->FindBin(0.01);
  int ebMax_SB = h1_sigma->FindBin(0.014);
  int eeMin    = h1_sigma->FindBin(0.02);
  int eeMax    = h1_sigma->FindBin(0.03);
  int eeMax_SB = h1_sigma->FindBin(0.035);

  float nEventsSR_EB = h1_sigma->Integral( ebMin, ebMax );
  float nEventsSR_EE = h1_sigma->Integral( eeMin, eeMax );
  float nEventsSB_EB = h1_sigma->Integral( ebMax, ebMax_SB );
  float nEventsSB_EE = h1_sigma->Integral( eeMax, eeMax_SB );

  std::cout << std::endl;
  std::cout << "Expected Number of events in 4 fb-1:" << std::endl;
  std::cout << "EB SR: " << nEventsSR_EB << "   EB SB: " << nEventsSB_EB << std::endl;
  std::cout << "EE SR: " << nEventsSR_EE << "   EE SB: " << nEventsSB_EE << std::endl;

  return 0;

}



