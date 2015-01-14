#include "interface/MT2Sample.h"
#include "interface/MT2Region.h"
#include "interface/MT2Analysis.h"
#include "interface/MT2EstimateZinvGamma.h"


#define mt2_cxx
#include "../interface/mt2.h"


#include "TLorentzVector.h"
#include "TH1F.h"


float lumi = 5.; //fb-1



MT2Analysis<MT2EstimateZinvGamma> computeYield( const MT2Sample& sample, const std::string& regionsSet );




int main( int argc, char* argv[] ) {


  std::string samplesFileName = "CSA14_Zinv";
  if( argc>1 ) {
    std::string samplesFileName_tmp(argv[1]); 
    samplesFileName = samplesFileName_tmp;
  }

  std::string samplesFile = "../samples/samples_" + samplesFileName + ".dat";
  
  std::vector<MT2Sample> samples_gammaJet = MT2Sample::loadSamples(samplesFile, "GJets");
  if( samples_gammaJet.size()==0 ) {
    std::cout << "There must be an error: didn't find any gamma+jet files in " << samplesFile << "!" << std::endl;
    exit(1209);
  }

  std::cout << std::endl << std::endl;
  std::cout << "-> Loading QCD samples" << std::endl;

  std::vector<MT2Sample> samples_qcd = MT2Sample::loadSamples(samplesFile, "QCD");
  if( samples_qcd.size()==0 ) {
    std::cout << "There must be an error: didn't find any QCD files in " << samplesFile << "!" << std::endl;
    exit(1205);
  }
  

  std::cout << std::endl << std::endl;
  std::cout << "-> Loading Zinv samples" << std::endl;

  std::vector<MT2Sample> samples_Zinv = MT2Sample::loadSamples(samplesFile, "ZJetsToNuNu");
  if( samples_Zinv.size()==0 ) {
    std::cout << "There must be an error: didn't find any Zinv files in " << samplesFile << "!" << std::endl;
    exit(1207);
  }


  std::string regionsSet = "13TeV_CSA14";
  //std::string regionsSet = "13TeV_ZinvGammaPurity";

  TH1::AddDirectory(kFALSE); // stupid ROOT memory allocation needs this


  std::string outputdir = "ZinvGammaPurity_" + samplesFileName;
  system(Form("mkdir -p %s", outputdir.c_str()));

  
  MT2Analysis<MT2EstimateZinvGamma>* gammaJet = new MT2Analysis<MT2EstimateZinvGamma>( "gammaJet", regionsSet );
  for( unsigned i=0; i<samples_gammaJet.size(); ++i ) {
    (*gammaJet) += (computeYield( samples_gammaJet[i], regionsSet ));
  }


  
  MT2Analysis<MT2EstimateZinvGamma>* qcd = new MT2Analysis<MT2EstimateZinvGamma>( "qcd", regionsSet );
  for( unsigned i=0; i<samples_qcd.size(); ++i ) {
    (*qcd) += (computeYield( samples_qcd[i], regionsSet ));
  }

  MT2Analysis<MT2EstimateZinvGamma>* gamma_plus_qcd = new MT2Analysis<MT2EstimateZinvGamma>( "gamma_plus_qcd", regionsSet );
  *gamma_plus_qcd = *gammaJet;
  *gamma_plus_qcd += *qcd;



  return 0;

}









MT2Analysis<MT2EstimateZinvGamma> computeYield( const MT2Sample& sample, const std::string& regionsSet ) {


  std::cout << std::endl << std::endl;
  std::cout << "-> Starting computation for sample: " << sample.name << std::endl;

  TFile* file = TFile::Open(sample.file.c_str());
  TTree* tree = (TTree*)file->Get("mt2");
  
  std::cout << "-> Loaded tree: it has " << tree->GetEntries() << " entries." << std::endl;



  MT2Analysis<MT2EstimateZinvGamma> analysis( sample.sname, regionsSet, sample.id );

  
  MT2Tree myTree;
  myTree.loadGenStuff = false;
  myTree.Init(tree);

  int nentries = tree->GetEntries();

  float mt2;
  tree->SetBranchAddress( "gamma_mt2"    , &mt2 );
  float ht;
  tree->SetBranchAddress( "gamma_ht"     , &ht );
  float met;
  tree->SetBranchAddress( "gamma_met_pt" , &met );
  int njets;
  tree->SetBranchAddress( "gamma_nJet40" , &njets );
  int nbjets;
  tree->SetBranchAddress( "gamma_nBJet40", &nbjets );


  for( unsigned iEntry=0; iEntry<nentries; ++iEntry ) {

    if( iEntry % 50000 == 0 ) std::cout << "    Entry: " << iEntry << " / " << nentries << std::endl;

    myTree.GetEntry(iEntry);

    if( myTree.nMuons10 > 0) continue;
    if( myTree.nElectrons10 > 0 ) continue;
    if( myTree.nPFLep5LowMT > 0) continue;
    if( myTree.nPFHad10LowMT > 0) continue;

    if( myTree.jet_pt[1]<100. ) continue;
    if( myTree.deltaPhiMin<0.3 ) continue;
    if( myTree.diffMetMht>0.5*myTree.met_pt ) continue;
  
    if( myTree.nVert==0 ) continue;

    if( njets<2 ) continue;

    if( myTree.ngamma==0 ) continue;

    TLorentzVector gamma;
    gamma.SetPtEtaPhiM( myTree.gamma_pt[0], myTree.gamma_eta[0], myTree.gamma_phi[0], myTree.gamma_mass[0] );
    float found_pt = 0.;
    int foundjet = 0;
    for( unsigned i=0; i<myTree.njet; ++i ) {
      TLorentzVector thisjet;
      thisjet.SetPtEtaPhiM( myTree.jet_pt[i], myTree.jet_eta[i], myTree.jet_phi[i], myTree.jet_mass[i] );
      if( gamma.DeltaR(thisjet)>0.4 ) foundjet++;
      if( foundjet==2 ) {
        found_pt = thisjet.Pt();
        break;
      }
    }
    if( found_pt<100. ) continue;


    Double_t weight = myTree.evt_scale1fb*lumi; 

    MT2EstimateZinvGamma* thisEstimate = analysis.get( ht, njets, nbjets, met );
    if( thisEstimate==0 ) continue;

    thisEstimate->yield->Fill(mt2, weight );
    
    if( myTree.gamma_mcMatchId[0]==22 )
      thisEstimate->template_prompt->Fill(myTree.gamma_chHadIso[0], weight );
    else if( myTree.gamma_mcMatchId[0]==7 )
      thisEstimate->template_fake->Fill(myTree.gamma_chHadIso[0], weight );
    else if( myTree.gamma_mcMatchId[0]==0 )
      thisEstimate->template_unmatched->Fill(myTree.gamma_chHadIso[0], weight );

    
  } // for entries


  analysis.finalize();
  

  delete tree;

  file->Close();
  delete file;
  
  return analysis;

}


