#ifndef MT2Analysis_h
#define MT2Analysis_h


#include <map>
#include "interface/MT2AnalysisManager.h"
#include "interface/MT2Region.h"
#include "interface/MT2Estimate.h"
#include "interface/MT2EstimateSyst.h"


class MT2Analysis {

 public:

  MT2Analysis( const std::string& name, int id, MT2AnalysisManager* manager=0 );

  void createStructure( MT2AnalysisManager* manager );

  void addOverflow();

  void add(const MT2Analysis& rhs);
  MT2Analysis operator+( const MT2Analysis& rhs ) const;
  const MT2Analysis& operator=( const MT2Analysis& rhs );


  std::map<MT2Region,MT2EstimateSyst*> estimates;
  std::map<MT2Region,MT2Estimate*> simTruthEstimates;

  std::string name;
  int id;

 private:


};


#endif
