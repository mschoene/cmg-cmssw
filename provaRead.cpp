#include <iostream>
#include <map>
#include <string>
#include "TFile.h"


#include "interface/MT2Analysis.h"
#include "interface/MT2EstimateSyst.h"



int main() {

  std::set<MT2Analysis<MT2Estimate>*> analyses = MT2Analysis<MT2Estimate>::readFromFile("prova.root");
  //std::set<MT2Analysis<MT2EstimateSyst>*> analyses = MT2Analysis<MT2EstimateSyst>::readFromFile("prova.root");

  (*analyses.begin())->writeToFile( "prova2.root" );

  return 0;

}

