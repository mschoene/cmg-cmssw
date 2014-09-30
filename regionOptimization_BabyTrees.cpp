#include <iostream>
#include <sstream>
#include <ctime>

#include <TH2.h>
#include <THStack.h>
#include <TLegend.h>
#include <TStyle.h>
#include <TCanvas.h>

#include <TTreeFormula.h>
#include <TTreePerfStats.h>

#include "interface/MT2Common.h"
#include "interface/MT2Region.h"
#include "interface/MT2RegionAnalysisUtilities.h"

#define treeProducerSusyFullHad_cxx
#include "treeProducerSusyFullHad.h"

int main( int argc, char* argv[] ) {


  if( argc!=2 ) {
    std::cout << "USAGE: ./inclusivePlots_BabyTrees [samplesFileName]" << std::endl;
    std::cout << "Exiting." << std::endl;
    exit(11);
  }
  
  bool isReduceTree = true;

  int start_s=clock();

  std::string sampleName(argv[1]);
  
  std::string samplesFileName = "samples/samples_" + sampleName + ".dat";
  std::cout << std::endl << std::endl;
  std::cout << "-> Loading samples from file: " << samplesFileName << std::endl;
  
  std::vector<MT2SampleBaby_basic> fSamples = MT2Common::loadSamplesBaby_basic(samplesFileName);

  
  std::string outputdir = "SignalRegionsOptimization_noSpikes";
  system(Form("mkdir -p %s", outputdir.c_str()));

  std::string outputfile = outputdir+"/SignalRegionsOptimization.root";

  TFile* outfile = new TFile(outputfile.c_str(), "RECREATE");



  std::cout << "-> Starting declaration of stack histograms..." << std::endl;

  // Declaring histograms - one per sample
  int nSamples = fSamples.size(); 
  std::cout<< "nSamples " << nSamples << std::endl;

  gStyle->SetPalette(1);
  gStyle->SetOptStat(1110);

  TH2F* h_jetMultiplicity[4][3][nSamples+5];
  //[3][3][]
  //1. low HT / medium HT / high HT
  //2. low MT2 / medium MT2 / high MT2

  std::string HTregion[] = {"lowHT_", "mediumLowHT_", "mediumHighHT_", "highHT_"};
  std::string MT2region[] = {"lowMT2_", "mediumMT2_", "highMT2_"};


  int nbinsX_jetMultiplicity = 10;
  float jetMultiplicity_binsX[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
  int nbinsY_jetMultiplicity = 5;
  float jetMultiplicity_binsY[] = {0, 1, 2, 3, 4, 5};

  for (int j = 0; j < 4; ++j)
    for (int jj = 0; jj < 3; ++jj){

      std::string name_jetMultiplicity_QCD=HTregion[j]+MT2region[jj]+"jetMultiplicity_QCD";

      h_jetMultiplicity[j][jj][nSamples]= new TH2F(name_jetMultiplicity_QCD.c_str(), "n_{b-jets} vs n_{jets}", nbinsX_jetMultiplicity, jetMultiplicity_binsX, nbinsY_jetMultiplicity, jetMultiplicity_binsY);
      h_jetMultiplicity[j][jj][nSamples]->GetXaxis()->SetTitle("N(jets)");
      h_jetMultiplicity[j][jj][nSamples]->GetYaxis()->SetTitle("N(b-jets)");
      h_jetMultiplicity[j][jj][nSamples]->Sumw2();

      std::string name_jetMultiplicity_Top=HTregion[j]+MT2region[jj]+"jetMultiplicity_Top";

      h_jetMultiplicity[j][jj][nSamples+1]= new TH2F(name_jetMultiplicity_Top.c_str(), "n_{b-jets} vs n_{jets}", nbinsX_jetMultiplicity, jetMultiplicity_binsX, nbinsY_jetMultiplicity, jetMultiplicity_binsY);
      h_jetMultiplicity[j][jj][nSamples+1]->GetXaxis()->SetTitle("N(jets)");
      h_jetMultiplicity[j][jj][nSamples+1]->GetYaxis()->SetTitle("N(b-jets)");
      h_jetMultiplicity[j][jj][nSamples+1]->Sumw2();

      std::string name_jetMultiplicity_WJets=HTregion[j]+MT2region[jj]+"jetMultiplicity_WJets";

      h_jetMultiplicity[j][jj][nSamples+2]= new TH2F(name_jetMultiplicity_WJets.c_str(), "n_{b-jets} vs n_{jets}", nbinsX_jetMultiplicity, jetMultiplicity_binsX, nbinsY_jetMultiplicity, jetMultiplicity_binsY);
      h_jetMultiplicity[j][jj][nSamples+2]->GetXaxis()->SetTitle("N(jets)");
      h_jetMultiplicity[j][jj][nSamples+2]->GetYaxis()->SetTitle("N(b-jets)");
      h_jetMultiplicity[j][jj][nSamples+2]->Sumw2();

      std::string name_jetMultiplicity_ZJets=HTregion[j]+MT2region[jj]+"jetMultiplicity_ZJets";

      h_jetMultiplicity[j][jj][nSamples+3]= new TH2F(name_jetMultiplicity_ZJets.c_str(), "n_{b-jets} vs n_{jets}", nbinsX_jetMultiplicity, jetMultiplicity_binsX, nbinsY_jetMultiplicity, jetMultiplicity_binsY);
      h_jetMultiplicity[j][jj][nSamples+3]->GetXaxis()->SetTitle("N(jets)");
      h_jetMultiplicity[j][jj][nSamples+3]->GetYaxis()->SetTitle("N(b-jets)");
      h_jetMultiplicity[j][jj][nSamples+3]->Sumw2();

      std::string name_jetMultiplicity_allBackground=HTregion[j]+MT2region[jj]+"jetMultiplicity_allBackground";

      h_jetMultiplicity[j][jj][nSamples+4]= new TH2F(name_jetMultiplicity_allBackground.c_str(), "n_{b-jets} vs n_{jets}", nbinsX_jetMultiplicity, jetMultiplicity_binsX, nbinsY_jetMultiplicity, jetMultiplicity_binsY);
      h_jetMultiplicity[j][jj][nSamples+4]->GetXaxis()->SetTitle("N(jets)");
      h_jetMultiplicity[j][jj][nSamples+4]->GetYaxis()->SetTitle("N(b-jets)");
      h_jetMultiplicity[j][jj][nSamples+4]->Sumw2();

    }



  for( unsigned i=0; i<fSamples.size(); ++i ){
    
    for (int j = 0; j<4; ++j )
      for (int jj = 0; jj < 3; ++jj){

	//Initializing histograms:
    
	std::string name_jetMultiplicity=HTregion[j]+MT2region[jj]+"jetMultiplicity_"+fSamples[i].name;
	
	h_jetMultiplicity[j][jj][i] = new TH2F(name_jetMultiplicity.c_str(), "n_{b-jets} vs n_{jets}", nbinsX_jetMultiplicity, jetMultiplicity_binsX, nbinsY_jetMultiplicity, jetMultiplicity_binsY);
	h_jetMultiplicity[j][jj][i]->GetXaxis()->SetTitle("N(jets)");
	h_jetMultiplicity[j][jj][i]->GetYaxis()->SetTitle("N(b-jets)");
	h_jetMultiplicity[j][jj][i]->Sumw2();

	if(fSamples[i].sname == "WJets"){
	  h_jetMultiplicity[j][jj][i]->SetLineColor(417);
	  h_jetMultiplicity[j][jj][i]->SetFillColor(417);
	}
	else if(fSamples[i].sname == "ZJets"){
	  h_jetMultiplicity[j][jj][i]->SetLineColor(419);
	  h_jetMultiplicity[j][jj][i]->SetFillColor(419);
	}
	else if(fSamples[i].sname == "Top"){
	  h_jetMultiplicity[j][jj][i]->SetLineColor(4);
	  h_jetMultiplicity[j][jj][i]->SetFillColor(4);
	}
	else if(fSamples[i].sname == "QCD"){
	  h_jetMultiplicity[j][jj][i]->SetLineColor(401);
	  h_jetMultiplicity[j][jj][i]->SetFillColor(401);
	}
	else if(fSamples[i].sname == "DYJets"){
	  h_jetMultiplicity[j][jj][i]->SetLineColor(3);
	  h_jetMultiplicity[j][jj][i]->SetFillColor(3);
	}
	else if(fSamples[i].sname == "GJets"){
	  h_jetMultiplicity[j][jj][i]->SetLineColor(5);
	  h_jetMultiplicity[j][jj][i]->SetFillColor(5);
	}
	else if(fSamples[i].sname == "Signal"){
	  h_jetMultiplicity[j][jj][i]->SetLineColor(1);
	  h_jetMultiplicity[j][jj][i]->SetFillColor(1);
	}
	
      }
    
    std::cout << std::endl << std::endl;
    std::cout << "-> Starting filling histograms for sample: " << fSamples[i].name << std::endl;

    TFile* file = TFile::Open(fSamples[i].file.c_str());
    TTree* tree = (TTree*)file->Get("treeProducerSusyFullHad");

    TFile* tmpFile = TFile::Open("tmp.root", "recreate");
    TTree* tree_reduced;

    if(isReduceTree){
    
      // Define selection if reducing the tree before looping over entries:

      std::ostringstream preselectionStream;
      preselectionStream << " "
			 << "(nTaus20==0 && nMuons10==0 && nElectrons10==0)"                   << " && "
			 << "(nVert > 0)"                      << " && "
	//<< "(nJet40 > 1)"                     << " && "
			 << "(ht > 450)"                       << " && "
			 << "(met_pt > 30)"                    << " && " 
      			 << "(mt2 > 150)"                      << " && "
			 << "(jet_pt[1] > 100)"                << " && "
			 << "(deltaPhiMin > 0.3)"              << " && "
			 << "(diffMetMht < 70)";
      
      TString preselection = preselectionStream.str().c_str();
      TString cuts = preselection;
      
      /*
	TFile* tmpFile = TFile::Open("tmp.root", "recreate");
	tmpFile->cd();
	TTree* tree_reduced = tree->CopyTree(cuts);
      */

      tmpFile->cd();
      tree_reduced = tree->CopyTree(cuts);
      
    }// reduceTree

    treeProducerSusyFullHad myTree;
    if(isReduceTree)
      myTree.Init(tree_reduced);
    else
      myTree.Init(tree);

    // global sample weight:
    //Double_t weight = fSamples[i].xsection * fSamples[i].kfact * fSamples[i].lumi / (fSamples[i].nevents*fSamples[i].PU_avg_weight);
    Double_t weight = fSamples[i].xsection * fSamples[i].kfact * fSamples[i].lumi / (fSamples[i].nevents);
    
    std::cout << "Weight = " << weight << std::endl;
    


    // Performance statistics
    TTreePerfStats *ps;
    
    int nentries;

    if(isReduceTree){      

      nentries = tree_reduced->GetEntries();
      ps = new TTreePerfStats("ioperf", tree_reduced);
      
    }
    else {

      nentries = tree->GetEntries(); 
      ps = new TTreePerfStats("ioperf", tree);
    
    }
    
    std::cout << std::endl << std::endl;
    std::cout << "-> Starting loop over " << nentries << " entries..." << std::endl;

    for( unsigned iEntry=0; iEntry<nentries; ++iEntry ) {

      if( iEntry % 50000 == 0 ) std::cout << "    Entry: " << iEntry << " / " << nentries << std::endl;

      myTree.GetEntry(iEntry);
      
      if(!isReduceTree){
	
	//Define event selection if NOT reducing the input tree:
	if ( myTree.nVert < 1 ) continue;

      }
      
      float ht       = myTree.ht;
      float met      = myTree.met_pt;
      float mt2      = myTree.mt2;
      int njets      = myTree.nJet40;
      int nbjets     = myTree.nBJet40;
      int nmuons     = myTree.nMuons10;
      int nelectrons = myTree.nElectrons10;
      int ntaus      = myTree.nTaus20;
      int nleptons   = nmuons+nelectrons+ntaus;
      int njet_all   = myTree.njet;
      
      if (ht < 700){
	if(mt2 > 150) {
	  h_jetMultiplicity[0][0][i]->Fill(njets, nbjets, weight);
	  if(fSamples[i].sname=="QCD" && !((fSamples[i].name=="QCD-Pt120to170" && mt2 > 150) || (fSamples[i].name=="QCD-Pt170to300" && mt2 > 200) || (fSamples[i].name=="QCD-Pt300to470" && mt2 > 225) || (fSamples[i].name=="QCD-Pt470to600" && mt2 > 225))) h_jetMultiplicity[0][0][nSamples]->Fill(njets, nbjets, weight);
	  if(fSamples[i].sname=="Top") h_jetMultiplicity[0][0][nSamples+1]->Fill(njets, nbjets, weight);
	  if(fSamples[i].sname=="WJets") h_jetMultiplicity[0][0][nSamples+2]->Fill(njets, nbjets, weight);
	  if(fSamples[i].sname=="ZJets") h_jetMultiplicity[0][0][nSamples+3]->Fill(njets, nbjets, weight);
	  if(fSamples[i].sname!="Signal" && !((fSamples[i].name=="QCD-Pt120to170" && mt2 > 150) || (fSamples[i].name=="QCD-Pt170to300" && mt2 > 200) || (fSamples[i].name=="QCD-Pt300to470" && mt2 > 225) || (fSamples[i].name=="QCD-Pt470to600" && mt2 > 225))) h_jetMultiplicity[0][0][nSamples+4]->Fill(njets, nbjets, weight);
	}
	if(mt2 > 225) {
	  h_jetMultiplicity[0][1][i]->Fill(njets, nbjets, weight);
	  if(fSamples[i].sname=="QCD" && !((fSamples[i].name=="QCD-Pt120to170" && mt2 > 150) || (fSamples[i].name=="QCD-Pt170to300" && mt2 > 200) || (fSamples[i].name=="QCD-Pt300to470" && mt2 > 225) || (fSamples[i].name=="QCD-Pt470to600" && mt2 > 225))) h_jetMultiplicity[0][1][nSamples]->Fill(njets, nbjets, weight);
          if(fSamples[i].sname=="Top") h_jetMultiplicity[0][1][nSamples+1]->Fill(njets, nbjets, weight);
          if(fSamples[i].sname=="WJets") h_jetMultiplicity[0][1][nSamples+2]->Fill(njets, nbjets, weight);
          if(fSamples[i].sname=="ZJets") h_jetMultiplicity[0][1][nSamples+3]->Fill(njets, nbjets, weight);
	  if(fSamples[i].sname!="Signal" && !((fSamples[i].name=="QCD-Pt120to170" && mt2 > 150) || (fSamples[i].name=="QCD-Pt170to300" && mt2 > 200) || (fSamples[i].name=="QCD-Pt300to470" && mt2 > 225) || (fSamples[i].name=="QCD-Pt470to600" && mt2 > 225))) h_jetMultiplicity[0][1][nSamples+4]->Fill(njets, nbjets, weight);
	}
	if(mt2 > 400) {
	  h_jetMultiplicity[0][2][i]->Fill(njets, nbjets, weight);
	  if(fSamples[i].sname=="QCD" && !((fSamples[i].name=="QCD-Pt120to170" && mt2 > 150) || (fSamples[i].name=="QCD-Pt170to300" && mt2 > 200) || (fSamples[i].name=="QCD-Pt300to470" && mt2 > 225) || (fSamples[i].name=="QCD-Pt470to600" && mt2 > 225))) h_jetMultiplicity[0][2][nSamples]->Fill(njets, nbjets, weight);
          if(fSamples[i].sname=="Top") h_jetMultiplicity[0][2][nSamples+1]->Fill(njets, nbjets, weight);
          if(fSamples[i].sname=="WJets") h_jetMultiplicity[0][2][nSamples+2]->Fill(njets, nbjets, weight);
          if(fSamples[i].sname=="ZJets") h_jetMultiplicity[0][2][nSamples+3]->Fill(njets, nbjets, weight);
	  if(fSamples[i].sname!="Signal" && !((fSamples[i].name=="QCD-Pt120to170" && mt2 > 150) || (fSamples[i].name=="QCD-Pt170to300" && mt2 > 200) || (fSamples[i].name=="QCD-Pt300to470" && mt2 > 225) || (fSamples[i].name=="QCD-Pt470to600" && mt2 > 225))) h_jetMultiplicity[0][2][nSamples+4]->Fill(njets, nbjets, weight);
	}
      }
      else if (ht > 700 && ht < 1000){
        if(mt2 > 150) {
          h_jetMultiplicity[1][0][i]->Fill(njets, nbjets, weight);
	  if(fSamples[i].sname=="QCD" && !((fSamples[i].name=="QCD-Pt120to170" && mt2 > 150) || (fSamples[i].name=="QCD-Pt170to300" && mt2 > 200) || (fSamples[i].name=="QCD-Pt300to470" && mt2 > 225) || (fSamples[i].name=="QCD-Pt470to600" && mt2 > 225))) h_jetMultiplicity[1][0][nSamples]->Fill(njets, nbjets, weight);
          if(fSamples[i].sname=="Top") h_jetMultiplicity[1][0][nSamples+1]->Fill(njets, nbjets, weight);
          if(fSamples[i].sname=="WJets") h_jetMultiplicity[1][0][nSamples+2]->Fill(njets, nbjets, weight);
          if(fSamples[i].sname=="ZJets") h_jetMultiplicity[1][0][nSamples+3]->Fill(njets, nbjets, weight);
          if(fSamples[i].sname!="Signal" && !((fSamples[i].name=="QCD-Pt120to170" && mt2 > 150) || (fSamples[i].name=="QCD-Pt170to300" && mt2 > 200) || (fSamples[i].name=="QCD-Pt300to470" && mt2 > 225) || (fSamples[i].name=="QCD-Pt470to600" && mt2 > 225))) h_jetMultiplicity[1][0][nSamples+4]->Fill(njets, nbjets, weight);
        }
        if(mt2 > 225) {
          h_jetMultiplicity[1][1][i]->Fill(njets, nbjets, weight);
	  if(fSamples[i].sname=="QCD" && !((fSamples[i].name=="QCD-Pt120to170" && mt2 > 150) || (fSamples[i].name=="QCD-Pt170to300" && mt2 > 200) || (fSamples[i].name=="QCD-Pt300to470" && mt2 > 225) || (fSamples[i].name=="QCD-Pt470to600" && mt2 > 225))) h_jetMultiplicity[1][1][nSamples]->Fill(njets, nbjets, weight);
          if(fSamples[i].sname=="Top") h_jetMultiplicity[1][1][nSamples+1]->Fill(njets, nbjets, weight);
          if(fSamples[i].sname=="WJets") h_jetMultiplicity[1][1][nSamples+2]->Fill(njets, nbjets, weight);
          if(fSamples[i].sname=="ZJets") h_jetMultiplicity[1][1][nSamples+3]->Fill(njets, nbjets, weight);
          if(fSamples[i].sname!="Signal" && !((fSamples[i].name=="QCD-Pt120to170" && mt2 > 150) || (fSamples[i].name=="QCD-Pt170to300" && mt2 > 200) || (fSamples[i].name=="QCD-Pt300to470" && mt2 > 225) || (fSamples[i].name=="QCD-Pt470to600" && mt2 > 225))) h_jetMultiplicity[1][1][nSamples+4]->Fill(njets, nbjets, weight);
        }
        if(mt2 > 400) {
          h_jetMultiplicity[1][2][i]->Fill(njets, nbjets, weight);
	  if(fSamples[i].sname=="QCD" && !((fSamples[i].name=="QCD-Pt120to170" && mt2 > 150) || (fSamples[i].name=="QCD-Pt170to300" && mt2 > 200) || (fSamples[i].name=="QCD-Pt300to470" && mt2 > 225) || (fSamples[i].name=="QCD-Pt470to600" && mt2 > 225))) h_jetMultiplicity[1][2][nSamples]->Fill(njets, nbjets, weight);
          if(fSamples[i].sname=="Top") h_jetMultiplicity[1][2][nSamples+1]->Fill(njets, nbjets, weight);
          if(fSamples[i].sname=="WJets") h_jetMultiplicity[1][2][nSamples+2]->Fill(njets, nbjets, weight);
          if(fSamples[i].sname=="ZJets") h_jetMultiplicity[1][2][nSamples+3]->Fill(njets, nbjets, weight);
          if(fSamples[i].sname!="Signal" && !((fSamples[i].name=="QCD-Pt120to170" && mt2 > 150) || (fSamples[i].name=="QCD-Pt170to300" && mt2 > 200) || (fSamples[i].name=="QCD-Pt300to470" && mt2 > 225) || (fSamples[i].name=="QCD-Pt470to600" && mt2 > 225))) h_jetMultiplicity[1][2][nSamples+4]->Fill(njets, nbjets, weight);
        }
      }
      else if (ht > 1000 && ht < 1500){
        if(mt2 > 150) {
          h_jetMultiplicity[2][0][i]->Fill(njets, nbjets, weight);
          if(fSamples[i].sname=="QCD" && !((fSamples[i].name=="QCD-Pt120to170" && mt2 > 150) || (fSamples[i].name=="QCD-Pt170to300" && mt2 > 200) || (fSamples[i].name=="QCD-Pt300to470" && mt2 > 225) || (fSamples[i].name=="QCD-Pt470to600" && mt2 > 225))) h_jetMultiplicity[2][0][nSamples]->Fill(njets, nbjets, weight);
          if(fSamples[i].sname=="Top") h_jetMultiplicity[2][0][nSamples+1]->Fill(njets, nbjets, weight);
          if(fSamples[i].sname=="WJets") h_jetMultiplicity[2][0][nSamples+2]->Fill(njets, nbjets, weight);
          if(fSamples[i].sname=="ZJets") h_jetMultiplicity[2][0][nSamples+3]->Fill(njets, nbjets, weight);
          if(fSamples[i].sname!="Signal" && !((fSamples[i].name=="QCD-Pt120to170" && mt2 > 150) || (fSamples[i].name=="QCD-Pt170to300" && mt2 > 200) || (fSamples[i].name=="QCD-Pt300to470" && mt2 > 225) || (fSamples[i].name=="QCD-Pt470to600" && mt2 > 225))) h_jetMultiplicity[2][0][nSamples+4]->Fill(njets, nbjets, weight);
        }
        if(mt2 > 225) {
          h_jetMultiplicity[2][1][i]->Fill(njets, nbjets, weight);
          if(fSamples[i].sname=="QCD" && !((fSamples[i].name=="QCD-Pt120to170" && mt2 > 150) || (fSamples[i].name=="QCD-Pt170to300" && mt2 > 200) || (fSamples[i].name=="QCD-Pt300to470" && mt2 > 225) || (fSamples[i].name=="QCD-Pt470to600" && mt2 > 225))) h_jetMultiplicity[2][1][nSamples]->Fill(njets, nbjets, weight);
          if(fSamples[i].sname=="Top") h_jetMultiplicity[2][1][nSamples+1]->Fill(njets, nbjets, weight);
          if(fSamples[i].sname=="WJets") h_jetMultiplicity[2][1][nSamples+2]->Fill(njets, nbjets, weight);
          if(fSamples[i].sname=="ZJets") h_jetMultiplicity[2][1][nSamples+3]->Fill(njets, nbjets, weight);
          if(fSamples[i].sname!="Signal" && !((fSamples[i].name=="QCD-Pt120to170" && mt2 > 150) || (fSamples[i].name=="QCD-Pt170to300" && mt2 > 200) || (fSamples[i].name=="QCD-Pt300to470" && mt2 > 225) || (fSamples[i].name=="QCD-Pt470to600" && mt2 > 225))) h_jetMultiplicity[2][1][nSamples+4]->Fill(njets, nbjets, weight);
        }
        if(mt2 > 400) {
          h_jetMultiplicity[2][2][i]->Fill(njets, nbjets, weight);
          if(fSamples[i].sname=="QCD" && !((fSamples[i].name=="QCD-Pt120to170" && mt2 > 150) || (fSamples[i].name=="QCD-Pt170to300" && mt2 > 200) || (fSamples[i].name=="QCD-Pt300to470" && mt2 > 225) || (fSamples[i].name=="QCD-Pt470to600" && mt2 > 225))) h_jetMultiplicity[2][2][nSamples]->Fill(njets, nbjets, weight);
          if(fSamples[i].sname=="Top") h_jetMultiplicity[2][2][nSamples+1]->Fill(njets, nbjets, weight);
          if(fSamples[i].sname=="WJets") h_jetMultiplicity[2][2][nSamples+2]->Fill(njets, nbjets, weight);
          if(fSamples[i].sname=="ZJets") h_jetMultiplicity[2][2][nSamples+3]->Fill(njets, nbjets, weight);
          if(fSamples[i].sname!="Signal" && !((fSamples[i].name=="QCD-Pt120to170" && mt2 > 150) || (fSamples[i].name=="QCD-Pt170to300" && mt2 > 200) || (fSamples[i].name=="QCD-Pt300to470" && mt2 > 225) || (fSamples[i].name=="QCD-Pt470to600" && mt2 > 225))) h_jetMultiplicity[2][2][nSamples+4]->Fill(njets, nbjets, weight);
        }
      }
      else if (ht > 1500){
	if(mt2 > 150) {
          h_jetMultiplicity[3][0][i]->Fill(njets, nbjets, weight);
	  if(fSamples[i].sname=="QCD" && !((fSamples[i].name=="QCD-Pt120to170" && mt2 > 150) || (fSamples[i].name=="QCD-Pt170to300" && mt2 > 200) || (fSamples[i].name=="QCD-Pt300to470" && mt2 > 225) || (fSamples[i].name=="QCD-Pt470to600" && mt2 > 225))) h_jetMultiplicity[3][0][nSamples]->Fill(njets, nbjets, weight);
          if(fSamples[i].sname=="Top") h_jetMultiplicity[3][0][nSamples+1]->Fill(njets, nbjets, weight);
          if(fSamples[i].sname=="WJets") h_jetMultiplicity[3][0][nSamples+2]->Fill(njets, nbjets, weight);
          if(fSamples[i].sname=="ZJets") h_jetMultiplicity[3][0][nSamples+3]->Fill(njets, nbjets, weight);
          if(fSamples[i].sname!="Signal" && !((fSamples[i].name=="QCD-Pt120to170" && mt2 > 150) || (fSamples[i].name=="QCD-Pt170to300" && mt2 > 200) || (fSamples[i].name=="QCD-Pt300to470" && mt2 > 225) || (fSamples[i].name=="QCD-Pt470to600" && mt2 > 225))) h_jetMultiplicity[3][0][nSamples+4]->Fill(njets, nbjets, weight);
	}
        if(mt2 > 225) {
          h_jetMultiplicity[3][1][i]->Fill(njets, nbjets, weight);
	  if(fSamples[i].sname=="QCD" && !((fSamples[i].name=="QCD-Pt120to170" && mt2 > 150) || (fSamples[i].name=="QCD-Pt170to300" && mt2 > 200) || (fSamples[i].name=="QCD-Pt300to470" && mt2 > 225) || (fSamples[i].name=="QCD-Pt470to600" && mt2 > 225))) h_jetMultiplicity[3][1][nSamples]->Fill(njets, nbjets, weight);
          if(fSamples[i].sname=="Top") h_jetMultiplicity[3][1][nSamples+1]->Fill(njets, nbjets, weight);
          if(fSamples[i].sname=="WJets") h_jetMultiplicity[3][1][nSamples+2]->Fill(njets, nbjets, weight);
          if(fSamples[i].sname=="ZJets") h_jetMultiplicity[3][1][nSamples+3]->Fill(njets, nbjets, weight);
          if(fSamples[i].sname!="Signal" && !((fSamples[i].name=="QCD-Pt120to170" && mt2 > 150) || (fSamples[i].name=="QCD-Pt170to300" && mt2 > 200) || (fSamples[i].name=="QCD-Pt300to470" && mt2 > 225) || (fSamples[i].name=="QCD-Pt470to600" && mt2 > 225))) h_jetMultiplicity[3][1][nSamples+4]->Fill(njets, nbjets, weight);
        }
        if(mt2 > 400) {
          h_jetMultiplicity[3][2][i]->Fill(njets, nbjets, weight);
	  if(fSamples[i].sname=="QCD" && !((fSamples[i].name=="QCD-Pt120to170" && mt2 > 150) || (fSamples[i].name=="QCD-Pt170to300" && mt2 > 200) || (fSamples[i].name=="QCD-Pt300to470" && mt2 > 225) || (fSamples[i].name=="QCD-Pt470to600" && mt2 > 225))) h_jetMultiplicity[3][2][nSamples]->Fill(njets, nbjets, weight);
          if(fSamples[i].sname=="Top") h_jetMultiplicity[3][2][nSamples+1]->Fill(njets, nbjets, weight);
          if(fSamples[i].sname=="WJets") h_jetMultiplicity[3][2][nSamples+2]->Fill(njets, nbjets, weight);
          if(fSamples[i].sname=="ZJets") h_jetMultiplicity[3][2][nSamples+3]->Fill(njets, nbjets, weight);
          if(fSamples[i].sname!="Signal" && !((fSamples[i].name=="QCD-Pt120to170" && mt2 > 150) || (fSamples[i].name=="QCD-Pt170to300" && mt2 > 200) || (fSamples[i].name=="QCD-Pt300to470" && mt2 > 225) || (fSamples[i].name=="QCD-Pt470to600" && mt2 > 225))) h_jetMultiplicity[3][2][nSamples+4]->Fill(njets, nbjets, weight);
	}
      }
      
      
    }// entries
    
    std::cout << "Done with sample " << fSamples[i].name << std::endl << std::endl; 

    outfile->cd();
    for ( int j=0; j < 4; ++j )
      for (int jj=0; jj < 3; ++jj){

	float overflow = 0., lastBinContent = 0.;
	
	for (int ox = 1; ox <=10; ++ox){
	  
	  lastBinContent = h_jetMultiplicity[j][jj][i]->GetBinContent(ox, 5);
	  overflow = h_jetMultiplicity[j][jj][i]->GetBinContent(ox, 6);
	  h_jetMultiplicity[j][jj][i]->SetBinContent(ox, 5, lastBinContent + overflow);
	
	}
	for (int oy = 1; oy <=5; ++oy){

          lastBinContent = h_jetMultiplicity[j][jj][i]->GetBinContent(10, oy);
          overflow = h_jetMultiplicity[j][jj][i]->GetBinContent(11, oy);
          h_jetMultiplicity[j][jj][i]->SetBinContent(10, oy, lastBinContent + overflow);

        }

	h_jetMultiplicity[j][jj][i]->Write();
	
      }

    // Performance statistics
    std::string ps_name = outputdir+"/perfstat"+fSamples[i].name+".root";
    ps->SaveAs(ps_name.c_str());
    
    delete ps;

    if(isReduceTree)
      delete tree_reduced;
    delete tree;
    
    tmpFile->Close();
    delete tmpFile;
       
    file->Close();
    delete file;


  }// samples

  outfile->cd();
  for( int s=0; s < 5; ++s ){  
  
    for( int j=0; j < 4; ++j )
      for (int jj=0; jj < 3; ++jj){
	
	float overflow = 0., lastBinContent = 0.;
	
	for (int ox = 1; ox <=10; ++ox){
	  
	  lastBinContent = h_jetMultiplicity[j][jj][nSamples+s]->GetBinContent(ox, 5);
	  overflow = h_jetMultiplicity[j][jj][nSamples+s]->GetBinContent(ox, 6);
	  h_jetMultiplicity[j][jj][nSamples+s]->SetBinContent(ox, 5, lastBinContent + overflow);
	  
	}       
	for (int oy = 1; oy <=5; ++oy){
	  
	  lastBinContent = h_jetMultiplicity[j][jj][nSamples+s]->GetBinContent(10, oy);
	  overflow = h_jetMultiplicity[j][jj][nSamples+s]->GetBinContent(11, oy);
	  h_jetMultiplicity[j][jj][nSamples+s]->SetBinContent(10, oy, lastBinContent + overflow);
	  
	}
	
	h_jetMultiplicity[j][jj][nSamples+s]->Write();
	
      }
  
  }

  outfile->Close();

  system( "rm tmp.root" );

  int stop_s=clock();
  std::cout << "time: " << (stop_s-start_s)/double(CLOCKS_PER_SEC)*1000 << std::endl;

  return 0;

}// main
