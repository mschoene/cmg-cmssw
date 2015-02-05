#include "interface/MT2Analysis.h"
#include "interface/MT2EstimateSyst.h"

#include <iostream>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <string>

int main() {

  std::string topInputFile  = "/scratch/mmasciov/CMSSW_7_2_3_MT2Analysis2015/src/MT2Analysis2015/analysis/Top_CR_phys14.root";
  MT2Analysis<MT2EstimateSyst>* analysisTop = MT2Analysis<MT2EstimateSyst>::readFromFile(topInputFile.c_str());

  std::string wjetsInputFile  = "/scratch/mmasciov/CMSSW_7_2_3_MT2Analysis2015/src/MT2Analysis2015/analysis/WJets_CR_phys14.root";
  MT2Analysis<MT2EstimateSyst>* analysisWJets = MT2Analysis<MT2EstimateSyst>::readFromFile(wjetsInputFile.c_str());

  MT2Analysis<MT2EstimateSyst>* analysisSum = new MT2Analysis<MT2EstimateSyst>( (*analysisTop) );
  (*analysisSum) += (*analysisWJets);
  analysisSum->setName("llep");
  
  analysisSum->writeToFile("llep_CR_PHYS14.root", "RECREATE");

  std::string ofsTop = "Top_CR_phys14.log";
  analysisTop->print(ofsTop);
  
  std::string ofsWJets = "WJets_CR_phys14.log";
  analysisWJets->print(ofsWJets);
  
  std::string ofsFullBkg = "llep_CR_phys14.log";
  analysisSum->print(ofsFullBkg);

  std::string SMST1bbbb_lDM_InputFile  = "/scratch/mmasciov/CMSSW_7_2_3_MT2Analysis2015/src/MT2Analysis2015/analysis/SMS_T1bbbb_1000_900_CR_phys14.root";
  MT2Analysis<MT2EstimateSyst>* analysisSignal_lDM = MT2Analysis<MT2EstimateSyst>::readFromFile(SMST1bbbb_lDM_InputFile.c_str());
  
  std::string SMST1bbbb_hDM_InputFile  = "/scratch/mmasciov/CMSSW_7_2_3_MT2Analysis2015/src/MT2Analysis2015/analysis/SMS_T1bbbb_1500_100_CR_phys14.root";
  MT2Analysis<MT2EstimateSyst>* analysisSignal_hDM = MT2Analysis<MT2EstimateSyst>::readFromFile(SMST1bbbb_hDM_InputFile.c_str());

  std::string ofsT1bbbb_hDM = "SMST1bbbb_hDM_CR_phys14.log";
  analysisSignal_hDM->print(ofsT1bbbb_hDM);
  
  std::string ofsT1bbbb_lDM = "SMST1bbbb_lDM_CR_phys14.log";
  analysisSignal_lDM->print(ofsT1bbbb_lDM);

  return 0;

}
