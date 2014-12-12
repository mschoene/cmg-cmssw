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

  std::string ofs = "TTbar.log";
  analysisTop->print(ofs);

  return 0;

}
