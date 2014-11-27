/*
How to run:
root -l
.L postProcessing.C+
run("samples_50ns_miniaod.txt", "treeProducerSusyFullHad", "/scratch/mmasciov/ToPostProcess/", "/scratch/mmasciov/PostProcessED/", "_babytree.root")
*/

#include <sstream>
#include <fstream>
#include <cmath>
#include <cstdlib>
#include <ctime>

#include <string>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include "TFile.h"
#include "TTree.h"
#include "TBranch.h"
#include "TString.h"

using namespace std;


int postProcessing(string inputFile="input.root",
		   string outputFile="output.root",
		   string treeName="treeProducerSusyFullHad",
		   float filter=1.0, float kfactor=1.0, float xsec=-1.0, int id=1 );


int run(string sampleFileName="samples_50ns_miniaod.txt",
	string treeName="treeProducerSusyFullHad", 
	string inputPath = "/scratch/mmasciov/ToPostProcess/", 
	string outputPath = "/scratch/mmasciov/PostProcessED/",  
	string fileExtension = "_babytree.root"){
  
  int start_s=clock();

  ifstream IN(sampleFileName.c_str());

  if(!IN)
    system(Form("wget https://mangano.web.cern.ch/mangano/public/MECCA/%s", sampleFileName.c_str()));

  char buffer[300];

  std::cout << "------------------------------------" << std::endl;
  std::cout << "Sample File  " << sampleFileName << std::endl;

  int counter(0);

  while( IN.getline(buffer, 300, '\n') ) {

    if (buffer[0] == '#' || buffer[0] == '\n' || buffer[0] == ' ')
      continue;

    ++counter;
    std::cout << "# sample " << counter << std::endl;
    std::cout << buffer << std::endl;

    int id;
    char datasetName[300];
    float xs, filter, kfactor;
    sscanf(buffer, "%d\t/%s\t%f\t%f\t%f\n", &id, datasetName, &xs, &filter, &kfactor);
    std::cout << id << "\t" << xs << "\t" << filter << "\t" << "\t" << kfactor << std::endl;

    std::string dataset(datasetName);
    std::size_t length = dataset.find("_Tune");

    char fileName[300];
    std::size_t nameLength = dataset.copy(fileName, length, 0);
    fileName[nameLength]='\0';

    std::string file(fileName);
    std::string inputFile = inputPath + file + fileExtension;
    std::string outputFile = outputPath + file + fileExtension;
    std::cout << inputFile << std::endl;
    std::cout << outputFile << std::endl;

    postProcessing(inputFile, outputFile, treeName, filter, kfactor, xs, id);

  }
  
  int stop_s=clock();
  cout << "time: " << (stop_s-start_s)/double(CLOCKS_PER_SEC)*1000 << endl;
  
  return 0;
}

int postProcessing(string inputFile,
		   string outputFile,
		   string treeName,
		   float filter, float kfactor, float xsec, int id){

  //TString infname, TString outfile, unsigned int events, 
  //Float_t xsec, Float_t kfactor,
  //Float_t filt_eff, bool SortBasketsByEntry ) {
  
  cout << "Processing File " << inputFile << endl;

  TFile *f = TFile::Open(inputFile.c_str(), "READ");
  if (! f || f->IsZombie()) {
    cout << "File does not exist!" << endl;
    return 1;
  }
  
  TTree* t = (TTree*)f->Get(treeName.c_str());
  if (! t || t->IsZombie()) {
    cout << "Tree does not exist!" << endl;
    return 2;
  }
        
  
  // This line should be uncommented for all the branches that we want to overwrite.
  // If the branch is not in the input tree, we don't need this.
  //
  //t->SetBranchStatus("scale1fb", 0);

  TFile *out = TFile::Open(outputFile.c_str(), "RECREATE");
  TTree *clone = new TTree("mt2", "post processed baby tree for mt2 analysis");

  clone = t->CloneTree(-1, "fast");
  clone->SetName("mt2");

  /*
  if(SortBasketsByEntry)
    clone = t->CloneTree(-1, "fastSortBasketsByEntry");
  else 
    clone = t->CloneTree(-1, "fast");
  */

  //-------------------------------------------------------------

  //Calculate scaling factor and put variables into tree 
  int events = t->GetEntries();
  float scale1fb = xsec*kfactor*1000*filter/(Float_t)events;
 
  /*
  if(isdata){
	scale1fb = 1.0;
	cout<< "Data file. scale1fb: " << scale1fb << endl;
	
  }else{
	cout << "scale1fb: " << scale1fb << endl; 
  }
  */

  TBranch* b1 = clone->Branch("evt_scale1fb", &scale1fb, "evt_scale1fb/F");
  TBranch* b2 = clone->Branch("evt_xsec", &xsec, "evt_xsec/F");
  TBranch* b3 = clone->Branch("evt_kfactor", &kfactor, "evt_kfactor/F");
  TBranch* b4 = clone->Branch("evt_filter", &filter, "evt_filter/F");
  TBranch* b5 = clone->Branch("evt_nEvts", &events, "evt_nEvts/I");
  TBranch* b6 = clone->Branch("evt_id", &id, "evt_id/I");

  Int_t nentries = t->GetEntries();
  for(Int_t i = 0; i < nentries; i++) {
    b1->Fill();
    b2->Fill();
    b3->Fill();
    b4->Fill();
    b5->Fill();
    b6->Fill();
  }
  //-------------------------------------------------------------

  delete t;
  f->Close();
  
  //out->cd();
  clone->Write();
  delete clone;
  out->Close();

  
  return 0;
  
}
