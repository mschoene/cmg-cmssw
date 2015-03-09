#include "../interface/MT2Efficiency.h"
#include "../interface/MT2Estimate.h"

#include <iostream>
#include <fstream>





MT2Efficiency::MT2Efficiency( const std::string& aname, const MT2Region& aregion ) : MT2Estimate( aname, aregion ) {

  eff = new TEfficiency(); // can i do better than this? default constructor is bad
  eff->SetName( this->getHistoName("eff").c_str() );

}



MT2Efficiency::MT2Efficiency( const std::string& name, const MT2Estimate& pass, const MT2Estimate& all ) : MT2Estimate( name, *(pass.region) ) {

  eff = new TEfficiency( *(pass.yield), *(all.yield) );
  eff->SetName( this->getHistoName("eff").c_str() );

}




MT2Efficiency::MT2Efficiency( const MT2Efficiency& rhs ) : MT2Estimate( rhs.getName(), *(rhs.region) ) {

  eff = new TEfficiency( *(rhs.eff) );

}



MT2Efficiency::~MT2Efficiency() {

  delete eff;

}




MT2Analysis<MT2Efficiency>* MT2Efficiency::createAnalysis( const std::string& aname, const std::string& regionsSet, MT2Analysis<MT2Estimate>* pass, MT2Analysis<MT2Estimate>* all ) {

  MT2Analysis<MT2Efficiency>* analysis = new MT2Analysis<MT2Efficiency>( aname, regionsSet );

  std::set<MT2Region> regions = analysis->getRegions();

  for( std::set<MT2Region>::iterator iR=regions.begin(); iR!=regions.end(); ++iR ) {

    MT2Estimate*   thisPass = pass     ->get( *iR );
    MT2Estimate*   thisAll  = all      ->get( *iR );
    MT2Efficiency* thisEff  = analysis ->get( *iR );

    std::string oldName(thisEff->eff->GetName());
    thisEff->eff = new TEfficiency( *(thisPass->yield), *(thisAll->yield) );
    thisEff->eff->SetName( oldName.c_str() );

  } // for regions

  return analysis;

}



void MT2Efficiency::setName( const std::string& newName ) {

  MT2Estimate::setName(newName);

  eff->SetName( this->getHistoName("eff").c_str() );

}





void MT2Efficiency::getShit( TFile* file, const std::string& path ) {

  eff = (TEfficiency*)file->Get(Form("%s/%s", path.c_str(), eff->GetName()));


}


//void MT2Efficiency::print(const std::string& ofs){
//
//  MT2Estimate::print( ofs );
//
//}



//const MT2Efficiency& MT2Efficiency::operator=( const MT2Efficiency& rhs ) {
//
//  if( this->yield == 0 ) { // first time
//
//    this->setName(rhs.getName());
//
//    this->region = new MT2Region(*(rhs.region));
//
//    this->yield = new TH1D(*(rhs.yield));
//    this->yield_btagUp = new TH1D(*(rhs.yield_btagUp));
//    this->yield_btagDown = new TH1D(*(rhs.yield_btagDown));
//
//  } else { // keep name and histo name, just make histogram identical
//
//    if( this->region!=0 ) delete this->region;
//    this->region = new MT2Region(*(rhs.region));
//
//    std::string oldName = this->yield->GetName();
//    delete this->yield;
//    this->yield = new TH1D(*(rhs.yield));
//    this->yield->SetName(oldName.c_str());
//
//    std::string oldName_btagUp = this->yield_btagUp->GetName();
//    delete this->yield_btagUp;
//    this->yield_btagUp = new TH1D(*(rhs.yield_btagUp));
//    this->yield_btagUp->SetName(oldName_btagUp.c_str());
//
//    std::string oldName_btagDown = this->yield_btagDown->GetName();
//    delete this->yield_btagDown;
//    this->yield_btagDown = new TH1D(*(rhs.yield_btagDown));
//    this->yield_btagDown->SetName(oldName_btagDown.c_str());
//
//  }
//
//  return *this;
//
//}




//MT2Efficiency MT2Efficiency::operator+( const MT2Efficiency& rhs ) const{
//
//
//  if( *(this->region) != *(rhs.region) ) {
//    std::cout << "[MT2Efficiency::operator+] ERROR! Can't add MT2Efficiency with different MT2Regions!" << std::endl;
//    exit(113);
//  }
//
//  //MT2Efficiency result(*this);
//  //result.yield->Add(rhs.yield);
//  //result.yield_btagUp->Add(rhs.yield_btagUp);
//  //result.yield_btagDown->Add(rhs.yield_btagDown);
//
//  MT2Efficiency result(*this);
//  result.yield->Add(rhs.yield);
//  result.yield_btagUp->Add(rhs.yield_btagUp);
//  result.yield_btagDown->Add(rhs.yield_btagDown);
//
//  //return *this;
//  return result;
//  
//}
//
//
//MT2Efficiency MT2Efficiency::operator/( const MT2Efficiency& rhs ) const{
//
//
//  if( *(this->region) != *(rhs.region) ) {
//    std::cout << "[MT2Efficiency::operator/] ERROR! Can't divide MT2Efficiency with different MT2Regions!" << std::endl;
//    exit(113);
//  }
//
//  MT2Efficiency result(*this);
//  result.yield->Divide(rhs.yield);
//  result.yield_btagUp->Divide(rhs.yield_btagUp);
//  result.yield_btagDown->Divide(rhs.yield_btagDown);
//
//  //return *this;
//  return result;
//
//}
//
//
//MT2Efficiency MT2Efficiency::operator*( const MT2Efficiency& rhs ) const{
//
//
//  if( *(this->region) != *(rhs.region) ) {
//    std::cout << "[MT2Efficiency::operator*] ERROR! Can't multiply MT2Efficiency with different MT2Regions!" << std::endl;
//    exit(113);
//  }
//
//  MT2Efficiency result(*this);
//  result.yield->Multiply(rhs.yield);
//  result.yield_btagUp->Multiply(rhs.yield_btagUp);
//  result.yield_btagDown->Multiply(rhs.yield_btagDown);
//
//  //return *this;
//  return result;
//
//}
//
//
//MT2Efficiency MT2Efficiency::operator*( float k ) const{
//
//  MT2Efficiency result(this->getName(), *(this->region) );
//  result.yield = new TH1D(*(this->yield));
//  result.yield->Scale(k);
//
//  result.yield_btagUp = new TH1D(*(this->yield_btagUp));
//  result.yield_btagUp->Scale(k);
//
//  result.yield_btagDown = new TH1D(*(this->yield_btagDown));
//  result.yield_btagDown->Scale(k);
//
//  return result;
//
//}
//
//
//
//MT2Efficiency MT2Efficiency::operator/( float k ) const{
//
//  MT2Efficiency result(this->getName(), *(this->region) );
//  result.yield = new TH1D(*(this->yield));
//  result.yield->Scale(1./k);
//
//  result.yield_btagUp = new TH1D(*(this->yield_btagUp));
//  result.yield_btagUp->Scale(1./k);
//
//  result.yield_btagDown = new TH1D(*(this->yield_btagDown));
//  result.yield_btagDown->Scale(1./k);
//
//  return result;
//
//
//}
//
//
//
//const MT2Efficiency& MT2Efficiency::operator+=( const MT2Efficiency& rhs ) {
//
//  this->yield->Add(rhs.yield);
//  this->yield_btagUp->Add(rhs.yield_btagUp);
//  this->yield_btagDown->Add(rhs.yield_btagDown);
//  return (*this);
//
//}
//
//const MT2Efficiency& MT2Efficiency::operator/=( const MT2Efficiency& rhs ) {
//
//  this->yield->Divide(rhs.yield);
//  this->yield_btagUp->Divide(rhs.yield_btagUp);
//  this->yield_btagDown->Divide(rhs.yield_btagDown);
//  return (*this);
//
//}
//
//const MT2Efficiency& MT2Efficiency::operator*=( const MT2Efficiency& rhs ) {
//
//  this->yield->Multiply(rhs.yield);
//  this->yield_btagUp->Multiply(rhs.yield_btagUp);
//  this->yield_btagDown->Multiply(rhs.yield_btagDown);
//  return (*this);
//
//}

