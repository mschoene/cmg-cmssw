#include <iostream>

#include "interface/MT2Analysis.h"
#include "interface/MT2Estimate.h"
#include "interface/MT2Region.h"



int main() {


  // create a signal region structure with the 8TeV definitions of signal regions:
  // "pippo" is a name that will figure in the histogram names
  // "8TeV" is the name of a given signal region set (for now i've implemented the one used at 8 tev and the recent proposal by mario - called "13TeV")
  // 702 is an optional argument: it's the id of the sample (so 702 in this case is DYJetsToLL_M-50_HT-100to200)
  MT2Analysis<MT2Estimate> *analysis = new MT2Analysis<MT2Estimate>("pippo", "13TeV_onlyHT", 702);
  // ^ the above command creates the whole HT x SR structure, and instantiates one MT2EstimateSyst per region

  analysis->printRegions();


  MT2Region* region = new MT2Region( 450., 575., 200., 2, 2, 1, 1 );

  std::cout << "Trying to match: " << region->getName() << std::endl;

  MT2Region* match = analysis->matchRegion(*region);

  if( match!=0 )
    std::cout << "Matched to: " << match->getName() << std::endl;
  else
    std::cout << "No Match" << std::endl;

  return 0;

}

