#include "interface/MT2Analysis.h"
#include "interface/MT2EstimateSyst.h"



int main() {

  MT2Analysis<MT2EstimateSyst>* analysis = MT2Analysis<MT2EstimateSyst>::readFromFile("prova.root");

  analysis->writeToFile( "prova2.root" );

  return 0;

}

