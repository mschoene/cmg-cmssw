#include <iostream>
#include <sstream>

#include <TH2.h>
#include <THStack.h>
#include <TLegend.h>
#include <TStyle.h>
#include <TCanvas.h>

//#include "helper/Utilities.hh"
//#include "Utilities.hh"

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

  std::string sampleName(argv[1]);
  
  std::string samplesFileName = "samples/samples_" + sampleName + ".dat";
  std::cout << std::endl << std::endl;
  std::cout << "-> Loading samples from file: " << samplesFileName << std::endl;
  
  std::vector<MT2SampleBaby_basic> fSamples = MT2Common::loadSamplesBaby_basic(samplesFileName);

  
  std::string outputdir = "InclusivePlots/QCD";
  system(Form("mkdir -p %s", outputdir.c_str()));


  std::cout << "-> Starting declaration of stack histograms..." << std::endl;

  // Declaring stack histogram - one per variable
  THStack* h_njets_stack = new THStack("h_njets_stack", "");

  THStack* h_nbjets_stack = new THStack("h_nbjets_stack", "");

  THStack* h_nleptons_stack = new THStack("h_nleptons_stack", "");

  THStack* h_ht_stack = new THStack("h_ht_stack", "");

  THStack* h_met_stack = new THStack("h_met_stack", "");

  THStack* h_mt2_stack = new THStack("h_mt2_stack", "");

  THStack* h_jetpt_stack = new THStack("h_jetpt_stack", "");


  // Declaring histograms - one per sample
  int nSamples = fSamples.size(); 
  
  std::cout<< "nSamples " << nSamples << std::endl;

  TH1F* h_njets[nSamples+1];
  
  TH1F* h_nbjets[nSamples+1];
  
  TH1F* h_nleptons[nSamples+1];

  TH1F* h_ht[nSamples+1];

  TH1F* h_met[nSamples+1];

  TH1F* h_mt2[nSamples+1];

  TH1F* h_jetpt[nSamples+1];

  
  for( unsigned i=0; i<fSamples.size(); ++i ){
    
    //Initializing histograms:
    
    std::string name_njets="njets"+fSamples[i].name;
    h_njets[i] = new TH1F(name_njets.c_str(), "number of jets", 12, 0, 12 );
    h_njets[i]->GetXaxis()->SetTitle("N(jets)");
    h_njets[i]->GetYaxis()->SetTitle("Events");
    if(fSamples[i].name == "QCD-Pt120to170"){
      h_njets[i]->SetLineColor(401);
      h_njets[i]->SetFillColor(401);
    }
    else if(fSamples[i].name == "QCD-Pt170to300"){
      h_njets[i]->SetLineColor(402);
      h_njets[i]->SetFillColor(402);
    }
    else if(fSamples[i].name == "QCD-Pt300to470"){
      h_njets[i]->SetLineColor(403);
      h_njets[i]->SetFillColor(403);
    }
    else if(fSamples[i].name == "QCD-Pt470t600"){
      h_njets[i]->SetLineColor(404);
      h_njets[i]->SetFillColor(404);
    }
    else if(fSamples[i].name == "QCD-Pt600to800"){
      h_njets[i]->SetLineColor(405);
      h_njets[i]->SetFillColor(405); 
    }
    else if(fSamples[i].name == "QCD-Pt800to1000"){
      h_njets[i]->SetLineColor(406);
      h_njets[i]->SetFillColor(406);
    }
    else if(fSamples[i].name == "QCD-Pt1000to1400"){
      h_njets[i]->SetLineColor(407);
      h_njets[i]->SetFillColor(407);
    }
    else if(fSamples[i].name == "QCD-Pt1400to1800"){
      h_njets[i]->SetLineColor(408);
      h_njets[i]->SetFillColor(408);
    }
    else if(fSamples[i].name == "QCD-Pt1800"){
      h_njets[i]->SetLineColor(409);
      h_njets[i]->SetFillColor(409);
    }

    std::string name_nbjets="nbjets"+fSamples[i].name;
    h_nbjets[i] = new TH1F(name_nbjets.c_str(), "number of b-jets", 6, 0, 6 );
    h_nbjets[i]->GetXaxis()->SetTitle("N(b-jets)");
    h_nbjets[i]->GetYaxis()->SetTitle("Events");
    if(fSamples[i].name == "QCD-Pt120to170"){
      h_nbjets[i]->SetLineColor(401);
      h_nbjets[i]->SetFillColor(401);
    }
    else if(fSamples[i].name == "QCD-Pt170to300"){
      h_nbjets[i]->SetLineColor(402);
      h_nbjets[i]->SetFillColor(402);
    }
    else if(fSamples[i].name == "QCD-Pt300to470"){
      h_nbjets[i]->SetLineColor(403);
      h_nbjets[i]->SetFillColor(403);
    }
    else if(fSamples[i].name == "QCD-Pt470t600"){
      h_nbjets[i]->SetLineColor(404);
      h_nbjets[i]->SetFillColor(404);
    }
    else if(fSamples[i].name == "QCD-Pt600to800"){
      h_nbjets[i]->SetLineColor(405);
      h_nbjets[i]->SetFillColor(405); 
    }
    else if(fSamples[i].name == "QCD-Pt800to1000"){
      h_nbjets[i]->SetLineColor(406);
      h_nbjets[i]->SetFillColor(406);
    }
    else if(fSamples[i].name == "QCD-Pt1000to1400"){
      h_nbjets[i]->SetLineColor(407);
      h_nbjets[i]->SetFillColor(407);
    }
    else if(fSamples[i].name == "QCD-Pt1400to1800"){
      h_nbjets[i]->SetLineColor(408);
      h_nbjets[i]->SetFillColor(408);
    }
    else if(fSamples[i].name == "QCD-Pt1800"){
      h_nbjets[i]->SetLineColor(409);
      h_nbjets[i]->SetFillColor(409);
    }

    std::string name_nleptons="nleptons"+fSamples[i].name;
    h_nleptons[i] = new TH1F(name_nleptons.c_str(), "number of leptons", 10, 0, 10 );
    h_nleptons[i]->GetXaxis()->SetTitle("N(leptons)");
    h_nleptons[i]->GetYaxis()->SetTitle("Events");
    if(fSamples[i].name == "QCD-Pt120to170"){
      h_nleptons[i]->SetLineColor(401);
      h_nleptons[i]->SetFillColor(401);
    }
    else if(fSamples[i].name == "QCD-Pt170to300"){
      h_nleptons[i]->SetLineColor(402);
      h_nleptons[i]->SetFillColor(402);
    }
    else if(fSamples[i].name == "QCD-Pt300to470"){
      h_nleptons[i]->SetLineColor(403);
      h_nleptons[i]->SetFillColor(403);
    }
    else if(fSamples[i].name == "QCD-Pt470t600"){
      h_nleptons[i]->SetLineColor(404);
      h_nleptons[i]->SetFillColor(404);
    }
    else if(fSamples[i].name == "QCD-Pt600to800"){
      h_nleptons[i]->SetLineColor(405);
      h_nleptons[i]->SetFillColor(405);
    }
    else if(fSamples[i].name == "QCD-Pt800to1000"){
      h_nleptons[i]->SetLineColor(406);
      h_nleptons[i]->SetFillColor(406); 
    }
    else if(fSamples[i].name == "QCD-Pt1000to1400"){
      h_nleptons[i]->SetLineColor(407);
      h_nleptons[i]->SetFillColor(407); 
    }
    else if(fSamples[i].name == "QCD-Pt1400to1800"){
      h_nleptons[i]->SetLineColor(408);
      h_nleptons[i]->SetFillColor(408); 
    }
    else if(fSamples[i].name == "QCD-Pt1800"){
      h_nleptons[i]->SetLineColor(409);
      h_nleptons[i]->SetFillColor(409); 
    }
    
    std::string name_ht="ht"+fSamples[i].name;
    h_ht[i] = new TH1F(name_ht.c_str(), "H_{T}", 300, 0, 3000 );
    h_ht[i]->GetXaxis()->SetTitle("H_{T} [GeV]");
    h_ht[i]->GetYaxis()->SetTitle("Events/10 GeV");
    if(fSamples[i].name == "QCD-Pt120to170"){
      h_ht[i]->SetLineColor(401);
      h_ht[i]->SetFillColor(401);
    }
    else if(fSamples[i].name == "QCD-Pt170to300"){
      h_ht[i]->SetLineColor(402);
      h_ht[i]->SetFillColor(402);
    }
    else if(fSamples[i].name == "QCD-Pt300to470"){
      h_ht[i]->SetLineColor(403);
      h_ht[i]->SetFillColor(403);
    }
    else if(fSamples[i].name == "QCD-Pt470t600"){
      h_ht[i]->SetLineColor(404);
      h_ht[i]->SetFillColor(404);
    }
    else if(fSamples[i].name == "QCD-Pt600to800"){
      h_ht[i]->SetLineColor(405);
      h_ht[i]->SetFillColor(405); 
    }
    else if(fSamples[i].name == "QCD-Pt800to1000"){
      h_ht[i]->SetLineColor(406);
      h_ht[i]->SetFillColor(406);
    }
    else if(fSamples[i].name == "QCD-Pt1000to1400"){
      h_ht[i]->SetLineColor(407);
      h_ht[i]->SetFillColor(407);
    }
    else if(fSamples[i].name == "QCD-Pt1400to1800"){
      h_ht[i]->SetLineColor(408);
      h_ht[i]->SetFillColor(408);
    }
    else if(fSamples[i].name == "QCD-Pt1800"){
      h_ht[i]->SetLineColor(409);
      h_ht[i]->SetFillColor(409);
    }

    std::string name_met="met"+fSamples[i].name;
    h_met[i] = new TH1F(name_met.c_str(), "E_{T}^{miss}", 100, 0, 1000 );
    h_met[i]->GetXaxis()->SetTitle("E_{T}^{miss} [GeV]");
    h_met[i]->GetYaxis()->SetTitle("Events/10 GeV");
    if(fSamples[i].name == "QCD-Pt120to170"){
      h_met[i]->SetLineColor(401);
      h_met[i]->SetFillColor(401);
    }
    else if(fSamples[i].name == "QCD-Pt170to300"){
      h_met[i]->SetLineColor(402);
      h_met[i]->SetFillColor(402);
    }
    else if(fSamples[i].name == "QCD-Pt300to470"){
      h_met[i]->SetLineColor(403);
      h_met[i]->SetFillColor(403);
    }
    else if(fSamples[i].name == "QCD-Pt470t600"){
      h_met[i]->SetLineColor(404);
      h_met[i]->SetFillColor(404);
    }
    else if(fSamples[i].name == "QCD-Pt600to800"){
      h_met[i]->SetLineColor(405);
      h_met[i]->SetFillColor(405); 
    }
    else if(fSamples[i].name == "QCD-Pt800to1000"){
      h_met[i]->SetLineColor(406);
      h_met[i]->SetFillColor(406);
    }
    else if(fSamples[i].name == "QCD-Pt1000to1400"){
      h_met[i]->SetLineColor(407);
      h_met[i]->SetFillColor(407);
    }
    else if(fSamples[i].name == "QCD-Pt1400to1800"){
      h_met[i]->SetLineColor(408);
      h_met[i]->SetFillColor(408);
    }
    else if(fSamples[i].name == "QCD-Pt1800"){
      h_met[i]->SetLineColor(409);
      h_met[i]->SetFillColor(409);
    }

    std::string name_mt2="mt2"+fSamples[i].name;;
    h_mt2[i] = new TH1F(name_mt2.c_str(), "M_{T2}", 100, 0, 1000 );
    h_mt2[i]->GetXaxis()->SetTitle("M_{T2} [GeV]");
    h_mt2[i]->GetYaxis()->SetTitle("Events/10 GeV");
    if(fSamples[i].name == "QCD-Pt120to170"){
      h_mt2[i]->SetLineColor(401);
      h_mt2[i]->SetFillColor(401);
    }
    else if(fSamples[i].name == "QCD-Pt170to300"){
      h_mt2[i]->SetLineColor(402);
      h_mt2[i]->SetFillColor(402);
    }
    else if(fSamples[i].name == "QCD-Pt300to470"){
      h_mt2[i]->SetLineColor(403);
      h_mt2[i]->SetFillColor(403);
    }
    else if(fSamples[i].name == "QCD-Pt470t600"){
      h_mt2[i]->SetLineColor(404);
      h_mt2[i]->SetFillColor(404);
    }
    else if(fSamples[i].name == "QCD-Pt600to800"){
      h_mt2[i]->SetLineColor(405);
      h_mt2[i]->SetFillColor(405); 
    }
    else if(fSamples[i].name == "QCD-Pt800to1000"){
      h_mt2[i]->SetLineColor(406);
      h_mt2[i]->SetFillColor(406);
    }
    else if(fSamples[i].name == "QCD-Pt1000to1400"){
      h_mt2[i]->SetLineColor(407);
      h_mt2[i]->SetFillColor(407);
    }
    else if(fSamples[i].name == "QCD-Pt1400to1800"){
      h_mt2[i]->SetLineColor(408);
      h_mt2[i]->SetFillColor(408);
    }
    else if(fSamples[i].name == "QCD-Pt1800"){
      h_mt2[i]->SetLineColor(409);
      h_mt2[i]->SetFillColor(409);
    }

    std::string name_jetpt="jetpt"+fSamples[i].name;;
    h_jetpt[i] = new TH1F(name_jetpt.c_str(), "p_{T} (jet)", 300, 0, 3000 );
    h_jetpt[i]->GetXaxis()->SetTitle("p_{T}(jet) [GeV]");
    h_jetpt[i]->GetYaxis()->SetTitle("Events/10 GeV");
    if(fSamples[i].name == "QCD-Pt120to170"){
      h_jetpt[i]->SetLineColor(401);
      h_jetpt[i]->SetFillColor(401);
    }
    else if(fSamples[i].name == "QCD-Pt170to300"){
      h_jetpt[i]->SetLineColor(402);
      h_jetpt[i]->SetFillColor(402);
    }
    else if(fSamples[i].name == "QCD-Pt300to470"){
      h_jetpt[i]->SetLineColor(403);
      h_jetpt[i]->SetFillColor(403);
    }
    else if(fSamples[i].name == "QCD-Pt470t600"){
      h_jetpt[i]->SetLineColor(404);
      h_jetpt[i]->SetFillColor(404);
    }
    else if(fSamples[i].name == "QCD-Pt600to800"){
      h_jetpt[i]->SetLineColor(405);
      h_jetpt[i]->SetFillColor(405); 
    }
    else if(fSamples[i].name == "QCD-Pt800to1000"){
      h_jetpt[i]->SetLineColor(406);
      h_jetpt[i]->SetFillColor(406);
    }
    else if(fSamples[i].name == "QCD-Pt1000to1400"){
      h_jetpt[i]->SetLineColor(407);
      h_jetpt[i]->SetFillColor(407);
    }
    else if(fSamples[i].name == "QCD-Pt1400to1800"){
      h_jetpt[i]->SetLineColor(408);
      h_jetpt[i]->SetFillColor(408);
    }
    else if(fSamples[i].name == "QCD-Pt1800"){
      h_jetpt[i]->SetLineColor(409);
      h_jetpt[i]->SetFillColor(409);
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
	//<< "(nTaus20==0 && nMuons10==0 && nElectrons10==0)"                   << " && "
			 << "(nVert > 0)"                      << " && "
			 << "(nJet40 > 1)"                     << " && "
			 << "(ht > 450)"                       << " && "
			 << "(mt2 > 50)";
        //		       << "(jet_pt[1] > 100)"                << " && "
        //		       << "(deltaPhiMin > 0.3)"              << " && "
        //		       << "(diffMetMht < 70)";
      
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
    
    //std::cout << "Weight = " << weight << std::endl;
    //cout<<fSamples[i].xsection<<endl;
    //cout<<fSamples[i].kfact<<endl;
    //cout<<fSamples[i].lumi<<endl;
    //cout<<fSamples[i].nevents<<endl;
    //cout<<fSamples[i].PU_avg_weight<<endl;

    int nentries;

    // Performance statistics
    TTreePerfStats *ps;
    
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
      
      h_njets[i]    ->Fill(njets, weight);
      h_nbjets[i]   ->Fill(nbjets, weight);
      h_nleptons[i] ->Fill(nleptons, weight);
      h_ht[i]       ->Fill(ht, weight);
      h_met[i]      ->Fill(met, weight);
      h_mt2[i]      ->Fill(mt2, weight);

      h_jetpt[i]  ->Fill(myTree.jet_pt[0], weight);
      
      /*for (int jeti=0; jeti<njets; ++jeti)
	//h_jetpt[i]  ->Fill(myTree.jet_pt[jeti]);
	if(myTree.jet_mcPt[jeti] > 0)
	h_jetpt[i]  ->Fill(myTree.jet_mcPt[jeti]);*/
      
      
    }// entries
    
    std::cout << "Done with sample " << fSamples[i].name << std::endl << std::endl; 
 
    h_njets_stack    ->Add(h_njets[i]);
    h_nbjets_stack   ->Add(h_nbjets[i]);
    h_nleptons_stack ->Add(h_nleptons[i]);
    h_ht_stack       ->Add(h_ht[i]);
    h_met_stack      ->Add(h_met[i]);
    h_mt2_stack      ->Add(h_mt2[i]);
    h_jetpt_stack    ->Add(h_jetpt[i]);

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

  system( "rm tmp.root" );

  // Declaring empty histos for legend definition
  TH1F* hQCD1 = new TH1F("QCD1", "QCD1", 1, 0, 1);
  hQCD1->SetFillColor(401);
  
  TH1F* hQCD2 = new TH1F("QCD2", "QCD2", 1, 0, 1);
  hQCD2->SetFillColor(402);

  TH1F* hQCD3 = new TH1F("QCD3", "QCD3", 1, 0, 1);
  hQCD3->SetFillColor(403);

  TH1F* hQCD4 = new TH1F("QCD4", "QCD4", 1, 0, 1);
  hQCD4->SetFillColor(404);

  TH1F* hQCD5 = new TH1F("QCD5", "QCD5", 1, 0, 1);
  hQCD5->SetFillColor(405);

  TH1F* hQCD6 = new TH1F("QCD6", "QCD6", 1, 0, 1);
  hQCD6->SetFillColor(406);

  TH1F* hQCD7 = new TH1F("QCD7", "QCD7", 1, 0, 1);
  hQCD7->SetFillColor(407);

  TH1F* hQCD8 = new TH1F("QCD8", "QCD8", 1, 0, 1);
  hQCD8->SetFillColor(408);

  TH1F* hQCD9 = new TH1F("QCD9", "QCD9", 1, 0, 1);
  hQCD9->SetFillColor(409);


  TLegend* Leg = new TLegend(.65,.65,.85,.85, "QCD");
  Leg->SetFillColor(0);
  Leg->AddEntry(hQCD1, "Pt120to170", "F");
  Leg->AddEntry(hQCD2, "Pt170to300", "F");
  Leg->AddEntry(hQCD3, "Pt300to470", "F");
  Leg->AddEntry(hQCD4, "Pt470to600", "F");
  Leg->AddEntry(hQCD5, "Pt600to800", "F");
  Leg->AddEntry(hQCD6, "Pt800to1000", "F");
  Leg->AddEntry(hQCD7, "Pt1000to1400", "F");
  Leg->AddEntry(hQCD8, "Pt1400to1800", "F");
  Leg->AddEntry(hQCD9, "Pt1800", "F");

  TCanvas* c1=new TCanvas("c1", "c1", 600, 600);
  c1->cd();
  h_njets_stack ->Draw("hist");
  h_njets_stack ->GetXaxis()->SetTitle("N(jets)");
  h_njets_stack ->GetYaxis()->SetTitle("Events");
  h_njets_stack ->GetYaxis()->SetTitleOffset(1.5);
  h_njets_stack ->Draw("hist");

  Leg->Draw("same");
  
  c1->Update();

  c1->SaveAs(Form("%s/inclusiveNJets.eps", outputdir.c_str()));
  
  TCanvas* c2=new TCanvas("c2", "c2", 600, 600);
  c2->cd();
  h_nbjets_stack ->Draw("hist");
  h_nbjets_stack ->GetXaxis()->SetTitle("N(b-jets)");
  h_nbjets_stack ->GetYaxis()->SetTitle("Events");
  h_nbjets_stack ->GetYaxis()->SetTitleOffset(1.5);
  h_nbjets_stack ->Draw("hist");

  Leg->Draw("same");
  
  c2->Update();
  c2->SaveAs(Form("%s/inclusiveNbjets.eps", outputdir.c_str()));

  TCanvas* c3=new TCanvas("c3", "c3", 600, 600);
  c3->cd();
  h_nleptons_stack ->Draw("hist");
  h_nleptons_stack ->GetXaxis()->SetTitle("N(leptons)");
  h_nleptons_stack ->GetYaxis()->SetTitle("Events");
  h_nleptons_stack ->GetYaxis()->SetTitleOffset(1.5);
  h_nleptons_stack ->Draw("hist");
  
  Leg->Draw("same");

  c3->Update();
  c3->SaveAs(Form("%s/inclusiveNleptons.eps", outputdir.c_str()));

  TCanvas* c4=new TCanvas("c4", "c4", 600, 600);
  c4->cd();
  gPad->SetLogy();

  h_ht_stack ->Draw("hist");
  h_ht_stack ->GetXaxis()->SetTitle("H_{T} [GeV]");
  h_ht_stack ->GetYaxis()->SetTitle("Events/10 GeV");
  h_ht_stack ->GetYaxis()->SetTitleOffset(1.5);
  h_ht_stack ->SetMinimum(0.1);
  h_ht_stack ->Draw("hist");

  Leg->Draw("same");

  c4->Update();
  c4->SaveAs(Form("%s/HTinclusive.eps", outputdir.c_str()));

  TCanvas* c5=new TCanvas("c5", "c5", 600, 600);
  c5->cd();
  gPad->SetLogy();

  h_met_stack ->Draw("hist");
  h_met_stack ->GetXaxis()->SetTitle("E_{T}^{miss} [GeV]");
  h_met_stack ->GetYaxis()->SetTitle("Events/10 GeV");
  h_met_stack ->GetYaxis()->SetTitleOffset(1.5);
  h_met_stack ->SetMinimum(0.1);
  h_met_stack ->Draw("hist");
  
  Leg->Draw("same");
  
  c5->Update();
  c5->SaveAs(Form("%s/METinclusive.eps", outputdir.c_str()));

  TCanvas* c6=new TCanvas("c6", "c6", 600, 600);
  c6->cd();
  gPad->SetLogy();

  h_mt2_stack ->Draw("hist");
  h_mt2_stack ->GetXaxis()->SetTitle("M_{T2} [GeV]");
  h_mt2_stack ->GetYaxis()->SetTitle("Events/10 GeV");
  h_mt2_stack ->GetYaxis()->SetTitleOffset(1.5);
  h_mt2_stack ->SetMinimum(0.1);
  h_mt2_stack ->Draw("hist");

  Leg->Draw("same");

  c6->Update();
  c6->SaveAs(Form("%s/MT2inclusive.eps", outputdir.c_str()));

  TCanvas* c7=new TCanvas("c7", "c7", 600, 600);
  c7->cd();
  gPad->SetLogy();

  h_jetpt_stack ->Draw("hist");
  h_jetpt_stack ->GetXaxis()->SetTitle("MC p_{T}(jet) [GeV]");
  h_jetpt_stack ->GetYaxis()->SetTitle("Events/10 GeV");
  h_jetpt_stack ->GetYaxis()->SetTitleOffset(1.5);
  h_jetpt_stack ->SetMinimum(0.1);
  h_jetpt_stack ->Draw("hist");

  Leg->Draw("same");

  c7->Update();
  c7->SaveAs(Form("%s/LeadingJetPt_inclusive.eps", outputdir.c_str()));


  return 0;

}// main
