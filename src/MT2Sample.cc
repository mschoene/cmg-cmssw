#include "../interface/MT2Sample.h"

#include <iostream>
#include <cstdlib>
#include <fstream>
#include <string>
#include <sstream>
#include <algorithm>

#include "TFile.h"
#include "TTree.h"



MT2Sample::MT2Sample() {};



MT2Sample::~MT2Sample() {};




std::vector<MT2Sample> MT2Sample::loadSamples(const std::string& filename, int idMin, int idMax) {

  std::vector<MT2Sample> fSamples;

  char buffer[200];
  ifstream IN(filename.c_str());


  std::cout << "------------------------------------" << std::endl;
  std::cout << "Sample File  " << filename << std::endl;

  int counter(0);
  TString fPath;


  while( IN.getline(buffer, 200, '\n') ) {

    // ok = false;                                                                                                                                                                                                               
    if (buffer[0] == '#') {
      continue; // Skip lines commented with '#'                                                                                                                                                                                 
    }

    std::string sampleFilePath(buffer);
    TString sampleFilePath_tstr(sampleFilePath);
    if( !(sampleFilePath_tstr.EndsWith(".root")) ) continue;

    MT2Sample s;
    s.file = sampleFilePath;

    sampleFilePath_tstr.ReplaceAll( "/", " " );

    std::istringstream iss(sampleFilePath_tstr.Data());
    std::vector<std::string> pathDirs;
    do {
      std::string sub;
      iss >> sub;
      pathDirs.push_back(sub);
    } while (iss);

    TString rootFileName(pathDirs[pathDirs.size()-2]); // last element of vector always empty: take -2
    rootFileName.ReplaceAll( "_", " " );
 
    std::vector<std::string> fileNameParts;
    std::istringstream iss2(rootFileName.Data());
    do {
      std::string sub;
      iss2 >> sub;
      fileNameParts.push_back(sub);
    } while (iss2);

    if( fileNameParts.size()==0 ) {
      std::cout << "[MT2Sample::loadSamples] ERROR! Illegal file name: " << rootFileName << std::endl;
      exit(1101);
    }
     
    s.sname  = (fileNameParts.size()>1) ? fileNameParts[0] + "_" + fileNameParts[1] : fileNameParts[0];

    s.name = fileNameParts[0];
    for( unsigned i=1; i<fileNameParts.size() && i<5; ++i )
      s.name += "_" + fileNameParts[i];


    TFile* file = TFile::Open(s.file.c_str());
    TTree* tree = (TTree*)file->Get("mt2");

    int evt_id, evt_nEvts;
    float evt_filter, evt_kfactor, evt_xsec, evt_scale1fb;

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

    if( idMin>=0 && evt_id<idMin ) continue;
    if( idMax>=0 && evt_id>idMax ) continue;


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

  std::cout << "------------------------------------" << std::endl;

  return fSamples;

}


