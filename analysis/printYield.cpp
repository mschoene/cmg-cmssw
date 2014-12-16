#include "interface/MT2Analysis.h"
#include "interface/MT2EstimateSyst.h"

#include <iostream>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <string>

int main() {

  std::string firstInputFile  = "/scratch/mmasciov/CMSSW_7_0_6_patch3_MT2Analysis2015/src/MT2Analysis2015/analysis/EventYields_mc_CSA14_dummy/analyses.root";
 
  MT2Analysis<MT2EstimateSyst>* analysisTop = MT2Analysis<MT2EstimateSyst>::readFromFile(firstInputFile.c_str(), "Top");

  MT2Analysis<MT2EstimateSyst>* analysisWJets = MT2Analysis<MT2EstimateSyst>::readFromFile(firstInputFile.c_str(), "WJets");

  MT2Analysis<MT2EstimateSyst>* analysisZJets = MT2Analysis<MT2EstimateSyst>::readFromFile(firstInputFile.c_str(), "ZJets");

  MT2Analysis<MT2EstimateSyst>* analysisQCD = MT2Analysis<MT2EstimateSyst>::readFromFile(firstInputFile.c_str(), "QCD");

  MT2Analysis<MT2EstimateSyst>* analysisSum = new MT2Analysis<MT2EstimateSyst>( (*analysisTop) );
  (*analysisSum) += (*analysisWJets);
  (*analysisSum) += (*analysisZJets);
  (*analysisSum) += (*analysisQCD);
  analysisSum->setName("FullBkg");

  //std::string firstInputFile  = "/scratch/mmasciov/CMSSW_7_0_6_patch3_MT2Analysis2015/src/MT2Analysis2015/analysis/ZJets_Synch.root";
  //MT2Analysis<MT2EstimateSyst>* analysisTop = MT2Analysis<MT2EstimateSyst>::readFromFile(firstInputFile.c_str(), "LostLepton");

  std::string ofsTop = "Top_Synch.log";
  analysisTop->print(ofsTop);

  std::string ofsWJets = "WJets_Synch.log";
  analysisWJets->print(ofsWJets);

  std::string ofsZJets = "ZJets_Synch.log";
  analysisZJets->print(ofsZJets);

  std::string ofsQCD = "QCD_Synch.log";
  analysisQCD->print(ofsQCD);

  std::string ofsFullBkg = "FullBkg_Synch.log";
  analysisSum->print(ofsFullBkg);

  return 0;

}
