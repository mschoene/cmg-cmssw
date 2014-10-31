#include <iostream>
#include <map>
#include <string>
#include "TFile.h"


#include "interface/MT2Analysis.h"
#include "interface/MT2EstimateSyst.h"



int main() {

  // create a signal region structure with the 8TeV definitions of signal regions:
  MT2Analysis<MT2EstimateSyst> *analysis = new MT2Analysis<MT2EstimateSyst>("pippo", "8TeV", 702);

  float ht = 1200.;
  float met = 10.;
  int njets = 4;
  int nbjets = 1;
  float mt2 = 305.;

  float weight = 1.;
  float weight_btagUp   = 1.05;
  float weight_btagDown = 0.95;

  MT2EstimateSyst* est = analysis->get(ht,njets,nbjets,met);
  if( est!=0 ) {
    est->yield->Fill( mt2, weight );
    est->yield_btagUp->Fill( mt2, weight_btagUp );
    est->yield_btagDown->Fill( mt2, weight_btagDown );
  }


  // write the histograms to a file:
  analysis->writeToFile("prova.root");

  return 0;

}

