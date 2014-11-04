#include "../interface/MT2Common.h"
#include "../interface/MT2Region.h"

#include <iostream>
#include <cstdlib>
#include <fstream>



MT2Common::MT2Common() {};



MT2Common::~MT2Common() {};




std::string MT2Common::getSignalRegion( float ht_min, int njet_min, int njet_max, int nbjet_min, int nbjet_max) {

  char htString_ch[64];
  sprintf( htString_ch, "HTge%.0f", ht_min );
  std::string htString(htString_ch);

  std::string jString = MT2Common::getSingleSignalRegionString( njet_min , njet_max  , "j");
  std::string bString = MT2Common::getSingleSignalRegionString( nbjet_min, nbjet_max , "b");

  std::string signal_region = htString + "_" + jString + bString;

  return signal_region;

}


std::string MT2Common::getSingleSignalRegionString( int n_min , int n_max, const std::string& suffix ) {

  if( n_max==999 ) n_max=n_min;

  char signal_region_ch[64];
  if( n_max!=n_min )
    sprintf( signal_region_ch, "%dto%d%s", n_min, n_max, suffix.c_str() );
  else 
    sprintf( signal_region_ch, "%d%s", n_min, suffix.c_str() );
  
  std::string signal_region(signal_region_ch);

  return signal_region;

}







std::vector<MT2Sample> MT2Common::loadSamples(const std::string& filename) {

  std::vector<MT2Sample> fSamples;

  char buffer[200];
  ifstream IN(filename.c_str());

  //char ParName[100];                                                                                                                                                                                                           
  char StringValue[1000];

  std::cout << "------------------------------------" << std::endl;
  std::cout << "Sample File  " << filename << std::endl;

  int counter(0);
  TString fPath;


  while( IN.getline(buffer, 200, '\n') ) {

    // ok = false;                                                                                                                                                                                                               
    if (buffer[0] == '#') {
      continue; // Skip lines commented with '#'                                                                                                                                                                                 
    }

    if( !strcmp(buffer, "GENERAL") ) {

      IN.getline(buffer, 200, '\n');
      sscanf(buffer, "Path\t%s", StringValue);
      fPath = StringValue;

      std::cout << " ----  " << std::endl;
      std::cout << "  Path " << fPath << std::endl;

    }

    if( !strcmp(buffer, "SAMPLE")){

      MT2Sample s;
      IN.getline(buffer, 200, '\n');
      sscanf(buffer, "Name\t%s", StringValue);
      s.name = TString(StringValue);

      IN.getline(buffer, 200, '\n');
      sscanf(buffer, "SName\t%s", StringValue);
      s.sname = TString(StringValue);

      IN.getline(buffer, 200, '\n');
      sscanf(buffer, "File\t%s", StringValue);
      TString fileName =fPath+StringValue;
      s.file = fileName;



      TFile* file = TFile::Open(s.file.c_str());
      TTree* tree = (TTree*)file->Get("mt2");

      int evt_id;
      float evt_nEvts, evt_filter, evt_kfactor, evt_xsec, evt_scale1fb;

      tree->SetBranchAddress("evt_id",&evt_id);
      tree->SetBranchAddress("evt_nEvts",&evt_nEvts);
      tree->SetBranchAddress("evt_filter",&evt_filter);
      tree->SetBranchAddress("evt_kfactor",&evt_kfactor);
      tree->SetBranchAddress("evt_xsec",&evt_xsec);
      tree->SetBranchAddress("evt_scale1fb",&evt_scale1fb);
      
      tree->GetEntry(0);

      s.id       = evt_id;
      s.nevents  = evt_nEvts;
      s.xsection = evt_xsec;
      s.filter   = evt_filter;
      s.kfact    = evt_kfactor;
      s.scale1fb = evt_scale1fb;

      // Manually set variables
      s.PU_avg_weight = 1;
      s.lumi = 1000; // 1/fb
      
      std::cout << " ---- " << std::endl;
      std::cout << "  New sample added: " << s.name << std::endl;
      std::cout << "   Sample no.      " << counter << std::endl;
      std::cout << "   Short name:     " << s.sname << std::endl;
      std::cout << "   File:           " << s.file << std::endl;
      std::cout << "   ID:             " << s.id << std::endl;
      std::cout << "   Events:         " << s.nevents  << std::endl;
      std::cout << "   Xsection:       " << s.xsection << std::endl;
      std::cout << "   Lumi:           " << s.lumi << std::endl;
      std::cout << "   Kfactor:        " << s.kfact << std::endl;
      std::cout << "   Filter:         " << s.filter << std::endl;
      std::cout << "   Scale to 1/fb:  " << s.scale1fb << std::endl;
      std::cout << "   Avg PU weight:  " << s.PU_avg_weight << std::endl;
      //std::cout << "   type:           " << s.type << std::endl;
      //std::cout << "   Color:          " << s.color << std::endl;
      fSamples.push_back(s);
      file->Close();
      tree = 0;
      counter++;
    
    }

  }

  std::cout << "------------------------------------" << std::endl;

  return fSamples;

}


