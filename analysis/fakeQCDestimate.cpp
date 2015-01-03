#include "interface/MT2Analysis.h"
#include "interface/MT2EstimateSyst.h"

#include <iostream>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <string>

int main() {

  std::string inputFile  = "/scratch/mmasciov/CMSSW_7_0_6_patch3_MT2Analysis2015/src/MT2Analysis2015/analysis/MT2QCDEstimate_v1.root";
 
  MT2Analysis<MT2EstimateSyst>* analysisQCD = MT2Analysis<MT2EstimateSyst>::readFromFile(inputFile.c_str());
  analysisQCD->setName("QCD");
  analysisQCD->writeToFile("MT2QCDEstimate_v1.root");  

  return 0;

}
