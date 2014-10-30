#include "../interface/MT2Analysis.h"





MT2Analysis::MT2Analysis( const std::string& aname, int aid, MT2AnalysisManager* manager ) {

  name = aname;
  id = aid;

  if( manager!=0 )
    this->createStructure( manager );

}




void MT2Analysis::createStructure( MT2AnalysisManager* manager ) {

  estimates         = manager->createAnalysisEstimates<MT2EstimateSyst>( name );
  simTruthEstimates = manager->createAnalysisEstimates<MT2Estimate>( "simTruth_"+name );

}




void MT2Analysis::addOverflow() {

  for( std::map<MT2Region,MT2EstimateSyst*>::iterator it=estimates.begin(); it!=estimates.end(); ++it ) 
    it->second->addOverflow();
  for( std::map<MT2Region,MT2Estimate*>::iterator it=simTruthEstimates.begin(); it!=simTruthEstimates.end(); ++it ) 
    it->second->addOverflow();

}



const MT2Analysis& MT2Analysis::operator=( const MT2Analysis& rhs ) {

  this->name = rhs.name;
  this->id = rhs.id;
  this->estimates = rhs.estimates;
  this->simTruthEstimates = rhs.simTruthEstimates;

  return *this;

}



void MT2Analysis::add(const MT2Analysis& rhs) {

  (*this) = (*this) + rhs;

}



MT2Analysis MT2Analysis::operator+( const MT2Analysis& rhs ) const {

  MT2Analysis* result;

  if( estimates.size()==0 ) {

    result = new MT2Analysis(rhs);

  } else if( rhs.estimates.size()==0 ) {

    result = new MT2Analysis(*this);

  } else {

    std::string newName = this->name + "_" + rhs.name;

    result = new MT2Analysis(newName, -1); // mixed


    for(std::map< MT2Region, MT2EstimateSyst*>::const_iterator i=estimates.begin(); i!=estimates.end(); ++i) {
      for(std::map< MT2Region, MT2EstimateSyst*>::const_iterator j=rhs.estimates.begin(); j!=rhs.estimates.end(); ++j) {
        if( i->first != j->first ) continue;
        result->estimates[i->first] = new MT2EstimateSyst(*(i->second) + *(j->second));
      }
    }

    for(std::map< MT2Region, MT2Estimate*>::const_iterator i=simTruthEstimates.begin(); i!=simTruthEstimates.end(); ++i) {
      for(std::map< MT2Region, MT2Estimate*>::const_iterator j=rhs.simTruthEstimates.begin(); j!=rhs.simTruthEstimates.end(); ++j) {
        if( i->first != j->first ) continue;
        result->simTruthEstimates[i->first] = new MT2Estimate(*(i->second) + *(j->second));
      }
    }

  } // else


  return *result;

}
