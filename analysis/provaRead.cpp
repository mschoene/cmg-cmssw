#include "interface/MT2Analysis.h"
#include "interface/MT2EstimateSyst.h"



int main() {

  TH1::AddDirectory(kFALSE); // stupid ROOT memory allocation needs this

  MT2Analysis<MT2EstimateSyst>* analysis = MT2Analysis<MT2EstimateSyst>::readFromFile("prova.root");
  analysis->writeToFile( "provaRead.root" );

  MT2Analysis<MT2EstimateSyst>* analysis2 = MT2Analysis<MT2EstimateSyst>::readFromFile("prova2.root");
  *analysis += *analysis2;
  analysis->writeToFile( "provaRead2.root" );

  //MT2Analysis<MT2EstimateSyst>* analysisSum = new MT2Analysis<MT2EstimateSyst>( *analysis + *analysis2 );
  MT2Analysis<MT2EstimateSyst>* analysisSum = new MT2Analysis<MT2EstimateSyst>( "provaSum", "13TeV_CSA14" );
  *analysisSum = *analysis + *analysis2;
  analysisSum->writeToFile( "provaSum.root" );

  return 0;

}

