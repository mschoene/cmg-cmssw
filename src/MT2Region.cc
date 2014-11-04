#include "../interface/MT2Region.h"

#include <iostream>
#include <string>
#include <sstream>
#include <stdio.h>
#include <cstdlib>

#include "TH1F.h"


////////////////////////////////////////////////////////
//
//                    MT2HTRegion
//
////////////////////////////////////////////////////////



MT2HTRegion::MT2HTRegion( const std::string& name ) {

  // this constructor parses the name
  // the name has to be passed in the format:
  // HT[htMin]to[htMax]_met[metMin]
  // where :
  //   - htMin, htMax and metMin have to be integers
  //   - htMax is allowed to be "Inf"
  //   - if metMax is omitted it is assumed to be 0


  std::stringstream ss(name);
  std::vector<std::string> parts;
  std::string item;
  while(std::getline(ss, item, '_')) {
    parts.push_back(item);
  }


  if( parts.size()!=1 && parts.size()!=2 ) {
    std::cout << "[MT2HTRegion]::MT2HTRegion ERROR! Unrecognized MT2HTRegion name: " << name << std::endl;
    exit(455);
  }


  int htMin(-1), htMax(-1), metMin(0);

  char htMax_char[100];
  sscanf( parts[0].c_str(), "HT%dto%s", &htMin, htMax_char);
  std::string htMax_str(htMax_char);
  if( htMax_str=="Inf" ) 
    htMax = -1;
  else
    sscanf( parts[0].c_str(), "HT%dto%d", &htMin, &htMax);


  if( parts.size()==2 )  // also a met cut
    sscanf( parts[1].c_str(), "met%d", &metMin);
  

  this->htMin  = htMin;
  this->htMax  = htMax;
  this->metMin = metMin;

}



MT2HTRegion::MT2HTRegion( const MT2HTRegion& rhs ) {

  htMin = rhs.htMin;
  htMax = rhs.htMax;
  metMin = rhs.metMin;
  HLT_selection = rhs.HLT_selection;
  
}



MT2HTRegion::MT2HTRegion( float ahtMin, float ahtMax, float ametMin, const std::string& aHLT_selection ) {

  htMin = ahtMin;
  htMax = ahtMax;
  metMin = ametMin;
  HLT_selection = aHLT_selection;

}


std::string MT2HTRegion::getName() const {

  std::string htMax_str(Form("%.0f", htMax));
  if( htMax==-1 ) htMax_str = "Inf";

  char n[512];
  if( metMin > 0. )
    sprintf( n, "HT%.0fto%s_met%.0f", htMin, htMax_str.c_str(), metMin );
  else
    sprintf( n, "HT%.0fto%s", htMin, htMax_str.c_str() );
  std::string n_str(n);

  return n_str;

}



bool MT2HTRegion::operator==( const MT2HTRegion& rhs ) const {

  return ( htMin==rhs.htMin && htMax==rhs.htMax && metMin==rhs.metMin && HLT_selection==rhs.HLT_selection ); 

}


bool MT2HTRegion::operator!=( const MT2HTRegion& rhs ) const {

  return ( htMin!=rhs.htMin || htMax!=rhs.htMax || metMin!=rhs.metMin || HLT_selection!=rhs.HLT_selection ); 

}


bool MT2HTRegion::operator<( const MT2HTRegion& rhs ) const {

  return ( htMin<rhs.htMin );

}






////////////////////////////////////////////////////////
//
//                  MT2SignalRegion
//
////////////////////////////////////////////////////////


MT2SignalRegion::MT2SignalRegion( const std::string& name ) {

  std::stringstream ss(name);
  std::vector<std::string> parts;
  std::string item;
  while(std::getline(ss, item, '_')) {
    parts.push_back(item);
  }

  if( parts.size()!=2 ) {
    std::cout << "[MT2SignalRegion]::MT2SignalRegion ERROR! Unrecognized MT2SignalRegion name: " << name << std::endl;
    exit(457);
  }


  int jMin(-1), jMax(-1);

  char jMax_char[100];
  sscanf( parts[0].c_str(), "j%dto%s", &jMin, jMax_char);
  std::string jMax_str(jMax_char);
  if( jMax_str=="Inf" ) 
    jMax = -1;
  else
    sscanf( parts[0].c_str(), "j%dto%d", &jMin, &jMax);


  int bMin(-1), bMax(-1);

  char bMax_char[100];
  sscanf( parts[1].c_str(), "b%dto%s", &bMin, bMax_char);
  std::string bMax_str(bMax_char);
  if( bMax_str=="Inf" ) 
    bMax = -1;
  else
    sscanf( parts[1].c_str(), "b%dto%d", &bMin, &bMax);


  nJetsMin  = jMin;
  nJetsMax  = jMax;
  nBJetsMin = bMin;
  nBJetsMax = bMax;
  
}




MT2SignalRegion::MT2SignalRegion(int njmin, int njmax, int nbmin, int nbmax ) {

  nJetsMin = njmin;
  nJetsMax = njmax;
  nBJetsMin = nbmin;
  nBJetsMax = nbmax;

}




MT2SignalRegion::MT2SignalRegion( const MT2SignalRegion& rhs ) {

  nJetsMin = rhs.nJetsMin;
  nJetsMax = rhs.nJetsMax;
  nBJetsMin = rhs.nBJetsMin;
  nBJetsMax = rhs.nBJetsMax;

}



std::string MT2SignalRegion::getName() const {
 
  std::string jString = getSingleSignalRegionString( "j", nJetsMin,  nJetsMax  );
  std::string bString = getSingleSignalRegionString( "b", nBJetsMin, nBJetsMax );

  std::string signal_region = jString + "_" + bString;

  return signal_region;

}



std::string MT2SignalRegion::getSingleSignalRegionString( const std::string& suffix, int n_min , int n_max ) const {

  std::string n_min_str(Form("%d", n_min));
  std::string n_max_str(Form("%d", n_max));

  if( n_max<0 ) n_max_str="Inf";
  if( n_min<0 ) n_min_str=(suffix=="j") ? "2" : "0";
  
  std::string signal_region(Form("%s%sto%s", suffix.c_str(), n_min_str.c_str(), n_max_str.c_str()));

  return signal_region;

}


bool MT2SignalRegion::operator==( const MT2SignalRegion& rhs ) const {

  return ( nJetsMin==rhs.nJetsMin && nJetsMax==rhs.nJetsMax && nBJetsMin==rhs.nBJetsMin && nBJetsMax==rhs.nBJetsMax );
 
}


bool MT2SignalRegion::operator!=( const MT2SignalRegion& rhs ) const {

  return ( nJetsMin!=rhs.nJetsMin || nJetsMax!=rhs.nJetsMax || nBJetsMin!=rhs.nBJetsMin || nBJetsMax!=rhs.nBJetsMax );
 
}


bool MT2SignalRegion::operator<( const MT2SignalRegion& rhs ) const {

  if( nJetsMin!=rhs.nJetsMin ) {
    return nJetsMin<rhs.nJetsMin;
  } else {
    return nBJetsMin<rhs.nBJetsMin;
  }

  return false;

}









void MT2Region::getBins( int &nBins, double*& bins) const {

  std::string regionName = this->getName();

/*
  if( regionName == "HTge1200_2j0b" ) {
    const int nBins_tmp                      = 6;
    bins = new double[nBins_tmp+1]{120, 150, 200, 260, 350, 550, 900};
    nBins = nBins_tmp;
  } else if( regionName == "HTge1200_2j1to2b" ) {
    const int nBins_tmp                      = 2;
    bins = new double[nBins_tmp+1]{100, 180, 350};
    nBins = nBins_tmp;
  } else if( regionName == "HTge1200_3to5j0b" ) {
    const int nBins_tmp                      = 7;
    bins = new double[nBins_tmp+1]{160, 185, 220, 270, 350, 450, 650, 1000};
    nBins = nBins_tmp;
  } else if( regionName == "HTge1200_3to5j1b" ) {
    const int nBins_tmp                      = 4;
    bins = new double[nBins_tmp+1]{150, 180, 230, 350, 550};
    nBins = nBins_tmp;
  } else if( regionName == "HTge1200_3to5j2b" ) {
    const int nBins_tmp                      = 2;
    bins = new double[nBins_tmp+1]{130, 200, 350};
    nBins = nBins_tmp;
  } else if( regionName == "HTge1200_6j0b" ) {
    const int nBins_tmp                      = 3;
    bins = new double[nBins_tmp+1]{160, 200, 300, 500};
    nBins = nBins_tmp;
  } else if( regionName == "HTge1200_6j1b" ) {
    const int nBins_tmp                      = 3;
    bins = new double[nBins_tmp+1]{150, 200, 300, 500};
    nBins = nBins_tmp;
  } else if( regionName == "HTge1200_6j2b" ) {
    const int nBins_tmp                      = 2;
    bins = new double[nBins_tmp+1]{130, 200, 350};
    nBins = nBins_tmp;
  } else if( regionName == "HTge1200_3b" ) {
    const int nBins_tmp                      = 1;
    bins = new double[nBins_tmp+1]{125, 300};
    nBins = nBins_tmp;

  // HT > 750 && HT < 1200
  } else if( regionName == "HTge750_2j0b" ) {
    const int nBins_tmp                      = 9;
    bins = new double[nBins_tmp+1]{125, 150, 180, 220, 270, 325, 425, 580, 780, 1000};
    nBins = nBins_tmp;
  } else if( regionName == "HTge750_2j1to2b" ) {
    const int nBins_tmp                      = 5;
    bins = new double[nBins_tmp+1]{100, 135, 170, 260, 450, 700};
    nBins = nBins_tmp;
  } else if( regionName == "HTge750_3to5j0b" ) {
    const int nBins_tmp                      = 9;
    bins = new double[nBins_tmp+1]{160, 185, 215, 250, 300, 370, 480, 640, 800, 1000};
    nBins = nBins_tmp;
  } else if( regionName == "HTge750_3to5j1b" ) {
    const int nBins_tmp                      = 6;
    bins = new double[nBins_tmp+1]{150, 175, 210, 270, 380, 600, 900};
    nBins = nBins_tmp;
  } else if( regionName == "HTge750_3to5j2b" ) {
    const int nBins_tmp                      = 5;
    bins = new double[nBins_tmp+1]{130, 160, 200, 270, 370, 500};
    nBins = nBins_tmp;
  } else if( regionName == "HTge750_6j0b" ) {
    const int nBins_tmp                      = 5;
    bins = new double[nBins_tmp+1]{160, 200, 250, 325, 425, 600};
    nBins = nBins_tmp;
  } else if( regionName == "HTge750_6j1b" ) {
    const int nBins_tmp                      = 4;
    bins = new double[nBins_tmp+1]{150, 190, 250, 350, 500};
    nBins = nBins_tmp;
  } else if( regionName == "HTge750_6j2b" ) {
    const int nBins_tmp                      = 4;
    bins = new double[nBins_tmp+1]{130, 170, 220, 300, 450};
    nBins = nBins_tmp;
  } else if( regionName == "HTge750_3b" ) {
    const int nBins_tmp                        = 3;
    bins = new double[nBins_tmp+1]{125, 175, 275, 450};
    nBins = nBins_tmp;

  // HT > 450 && HT < 750
  } else if( regionName == "HTge450_2j0b" ) {
    const int nBins_tmp                      = 8;
    bins = new double[nBins_tmp+1]{200, 240, 290, 350, 420, 490, 570, 650, 750};
    nBins = nBins_tmp;
  } else if( regionName == "HTge450_2j1to2b" ) {
    const int nBins_tmp                      = 6;
    bins = new double[nBins_tmp+1]{200, 250, 310, 380, 450, 550, 700};
    nBins = nBins_tmp;
  } else if( regionName == "HTge450_3to5j0b" ) {
  const int nBins_tmp                      = 8;
  bins = new double[nBins_tmp+1]{200, 240, 290, 350, 420, 490, 570, 650, 750};
    nBins = nBins_tmp;
  } else if( regionName == "HTge450_3to5j1b" ) {
    const int nBins_tmp                      = 6;
    bins = new double[nBins_tmp+1]{200, 250, 310, 380, 460, 550, 700};
    nBins = nBins_tmp;
  } else if( regionName == "HTge450_3to5j2b" ) {
    const int nBins_tmp                      = 4;
    bins = new double[nBins_tmp+1]{200, 250, 325, 425, 550};
    nBins = nBins_tmp;
  } else if( regionName == "HTge450_6j0b" ) {
    const int nBins_tmp                      = 3;
    bins = new double[nBins_tmp+1]{200, 280, 380, 520};
    nBins = nBins_tmp;
  } else if( regionName == "HTge450_6j1b" ) {
    const int nBins_tmp                      = 3;
    bins = new double[nBins_tmp+1]{200, 250, 325, 450};
    nBins = nBins_tmp;
  } else if( regionName == "HTge450_6j2b" ) {
    const int nBins_tmp                      = 3;
    bins = new double[nBins_tmp+1]{200, 250, 300, 400};
    nBins = nBins_tmp;
  } else if( regionName == "HTge450_3b" ) {
    const int nBins_tmp                        = 2;
    bins = new double[nBins_tmp+1]{200, 280, 400};
    nBins = nBins_tmp;
*/


  if( regionName == "pippo" ) {

    // put your bins here
    
  } else { // default binning

    const int nBins_tmp = 6;
    bins = new double[nBins_tmp+1];
    bins[0] = 100.; 
    bins[1] = 150.; 
    bins[2] = 200.; 
    bins[3] = 250.; 
    bins[4] = 300.; 
    bins[5] = 350.; 
    bins[6] = 400.; 
    bins[7] = 450.; 
    nBins = nBins_tmp;

  }


}
