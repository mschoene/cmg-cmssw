#include "interface/MT2Analysis.h"
#include "interface/MT2EstimateSyst.h"

#include <iostream>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <string>

int main() {

  std::string topInputFile  = "/scratch/mmasciov/CMSSW_7_0_6_patch3_MT2Analysis2015/src/MT2Analysis2015/analysis/TTJets_CR_pfHad.root";
 
  MT2Analysis<MT2EstimateSyst>* analysisTop = MT2Analysis<MT2EstimateSyst>::readFromFile(topInputFile.c_str());

  std::string wjetsInputFile  = "/scratch/mmasciov/CMSSW_7_0_6_patch3_MT2Analysis2015/src/MT2Analysis2015/analysis/WJets_CR_pfHad.root";

  MT2Analysis<MT2EstimateSyst>* analysisWJets = MT2Analysis<MT2EstimateSyst>::readFromFile(wjetsInputFile.c_str());

  MT2Analysis<MT2EstimateSyst>* analysisSum = new MT2Analysis<MT2EstimateSyst>( (*analysisTop) );
  (*analysisSum) += (*analysisWJets);
  analysisSum->setName("llep");

  std::string ofsTop = "Top_CR_pfHad.log";
  analysisTop->print(ofsTop);
  
  std::string ofsWJets = "WJets_CR_pfHad.log";
  analysisWJets->print(ofsWJets);
  
  std::string ofsFullBkg = "llep_CR_pfHad.log";
  analysisSum->print(ofsFullBkg);
  
  return 0;

}
