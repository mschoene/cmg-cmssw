#ifndef MT2EstimateTree_h
#define MT2EstimateTree_h

#include "MT2Estimate.h"
#include "MT2Analysis.h"
#include "TTree.h"
#include "mt2.h"


class MT2EstimateTree : public MT2Estimate {

 public:

  MT2EstimateTree( const std::string& aname, const MT2Region& aregion );
  MT2EstimateTree( const MT2EstimateTree& rhs );
  virtual ~MT2EstimateTree();

  void initTree();
  virtual void setName( const std::string& newName );

  void assign( const MT2Tree& mt2tree, float w );
  void fillTree( const MT2Tree& mt2tree, float w );

  int run;
  int lumi;
  int evt;
  float weight;
  int id;

  float mt2;
  float ht;
  float met;
  int nJets;
  int nBJets;
 
  TTree* tree;

  const MT2EstimateTree& operator=( const MT2EstimateTree& rhs );
  MT2EstimateTree operator+( const MT2EstimateTree& rhs ) const;
  const MT2EstimateTree& operator+=( const MT2EstimateTree& rhs );

  //MT2EstimateTree operator/ ( float k ) const;
  //MT2EstimateTree operator* ( float k ) const;
  //const MT2EstimateTree& operator/=( float k );
  //const MT2EstimateTree& operator*=( float k );



  virtual void finalize() {
  }


  virtual void getShit( TFile* file, const std::string& path );

  virtual void write() const;

  virtual void print(const std::string& ofs);

 private:

};





#endif
