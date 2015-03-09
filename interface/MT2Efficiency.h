#ifndef MT2Efficiency_h
#define MT2Efficiency_h

#include "MT2Estimate.h"
#include "MT2Analysis.h"
#include "TEfficiency.h"



class MT2Efficiency : public MT2Estimate {

 public:


  MT2Efficiency( const std::string& aname, const MT2Region& aregion );
  MT2Efficiency( const MT2Efficiency& rhs );
  MT2Efficiency( const std::string& name, const MT2Estimate& pass, const MT2Estimate& all );
  virtual ~MT2Efficiency();

  static MT2Analysis<MT2Efficiency>* createAnalysis( const std::string& aname, const std::string& regionsSet, MT2Analysis<MT2Estimate>* pass, MT2Analysis<MT2Estimate>* all );

  virtual void setName( const std::string& newName );
 
  TEfficiency* eff;

  //const MT2Efficiency& operator=( const MT2Efficiency& rhs );
  //MT2Efficiency operator+( const MT2Efficiency& rhs ) const;
  //MT2Efficiency operator/( const MT2Efficiency& rhs ) const;
  //MT2Efficiency operator*( const MT2Efficiency& rhs ) const;
  //const MT2Efficiency& operator+=( const MT2Efficiency& rhs );
  //const MT2Efficiency& operator/=( const MT2Efficiency& rhs );
  //const MT2Efficiency& operator*=( const MT2Efficiency& rhs );


  virtual void getShit( TFile* file, const std::string& path );

  virtual void write() const {
    eff->Write();
  }

  //virtual void print(const std::string& ofs);

 private:

};





#endif
