#include "interface/MT2Sample.h"
#include "interface/MT2Region.h"
#include "interface/MT2Analysis.h"
#include "interface/MT2EstimateZinvGamma.h"


#define mt2_cxx
#include "../interface/mt2.h"


#include "TLorentzVector.h"
#include "TH1F.h"


float lumi = 5.; //fb-1



MT2Analysis<MT2EstimateZinvGamma> computeYield( const MT2Sample& sample, const std::string& regionsSet, bool onlyPrompt );




int main( int argc, char* argv[] ) {

  if( argc==1 ) {
    std::cout << "-> You need to pass me the regions set name. Here are some suggestions: " << std::endl;
    std::cout << "  13TeV_CSA14" << std::endl;
    std::cout << "  13TeV_onlyHT" << std::endl;
    std::cout << "  13TeV_onlyJets" << std::endl;
    std::cout << "  13TeV_inclusive" << std::endl;
    exit(101);
  }

  //std::string regionsSet = "13TeV_onlyHT";
  std::string regionsSet = "13TeV_CSA14";
  //std::string regionsSet = "13TeV_inclusive";
  //std::string regionsSet = "13TeV_ZinvGammaPurity";
  if( argc>1 ) {
    std::string regionsSet_tmp(argv[1]); 
    regionsSet = regionsSet_tmp;
  }

  std::string samplesFileName = "PHYS14_v2_Zinv";
  //std::string samplesFileName = "CSA14_Zinv";

  std::string samplesFile = "../samples/samples_" + samplesFileName + ".dat";
  
  std::vector<MT2Sample> samples_gammaJet = MT2Sample::loadSamples(samplesFile, "GJets");
  if( samples_gammaJet.size()==0 ) {
    std::cout << "There must be an error: didn't find any gamma+jet files in " << samplesFile << "!" << std::endl;
    exit(1209);
  }

  std::cout << std::endl << std::endl;
  std::cout << "-> Loading QCD samples" << std::endl;

  std::vector<MT2Sample> samples_qcd = MT2Sample::loadSamples(samplesFile, "QCD");
  



  TH1::AddDirectory(kFALSE); // stupid ROOT memory allocation needs this


  std::string outputdir = "ZinvGammaPurity_" + samplesFileName + "_" + regionsSet;
  system(Form("mkdir -p %s", outputdir.c_str()));


  
  MT2Analysis<MT2EstimateZinvGamma>* templatesPrompt = new MT2Analysis<MT2EstimateZinvGamma>( "templatesPrompt", regionsSet );
  MT2Analysis<MT2EstimateZinvGamma>* templatesFake   = new MT2Analysis<MT2EstimateZinvGamma>( "templatesFake", regionsSet );
  MT2Analysis<MT2EstimateZinvGamma>* templatesPrompt_qcd   = new MT2Analysis<MT2EstimateZinvGamma>( "templatesPrompt_qcd", regionsSet );


  for( unsigned i=0; i<samples_gammaJet.size(); ++i ) {
    (*templatesPrompt) += (computeYield( samples_gammaJet[i], regionsSet, true  ));
    //(*templatesFake)   += (computeYield( samples_gammaJet[i], regionsSet, false ));
  }

  
  for( unsigned i=0; i<samples_qcd.size(); ++i ) {
    (*templatesPrompt_qcd) += (computeYield( samples_qcd[i], regionsSet, true  ));
    (*templatesFake)   += (computeYield( samples_qcd[i], regionsSet, false ));
  }


  std::string templateFileName = "gammaTemplates_" + samplesFileName + "_" + regionsSet + ".root";
  templatesPrompt->writeToFile(templateFileName);
  templatesFake->addToFile(templateFileName);
  templatesPrompt_qcd->addToFile(templateFileName);


  return 0;

}









MT2Analysis<MT2EstimateZinvGamma> computeYield( const MT2Sample& sample, const std::string& regionsSet, bool onlyPrompt ) {


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




  for( unsigned iEntry=0; iEntry<nentries; ++iEntry ) {

    if( iEntry % 50000 == 0 ) std::cout << "    Entry: " << iEntry << " / " << nentries << std::endl;

    myTree.GetEntry(iEntry);

    if( myTree.gamma_ht>1000. && sample.id==204 ) continue; // remove high-weight spikes (remove GJet_400to600 leaking into HT>1000)

    if( myTree.gamma_mt2 < 200.) continue;

    if( myTree.nMuons10 > 0) continue;
    if( myTree.nElectrons10 > 0 ) continue;
    if( myTree.nPFLep5LowMT > 0) continue;
    if( myTree.nPFHad10LowMT > 0) continue;

    if( myTree.gamma_deltaPhiMin<0.3 ) continue;
    if( myTree.gamma_diffMetMht>0.5*myTree.gamma_met_pt ) continue;
  
    if( myTree.nVert==0 ) continue;

    if( myTree.gamma_nJet40<2 ) continue;

    if( myTree.ngamma==0 ) continue;

    if( myTree.gamma_idCutBased[0]==0 ) continue;
    //if( myTree.gamma_chHadIso[0]+myTree.gamma_neuHadIso[0] > 10. ) continue;
    



    TLorentzVector gamma;
    gamma.SetPtEtaPhiM( myTree.gamma_pt[0], myTree.gamma_eta[0], myTree.gamma_phi[0], myTree.gamma_mass[0] );
    int closestJet = -1;
    float deltaRmin = 0.4;
    for( unsigned i=0; i<myTree.njet; ++i ) {
      if( fabs(myTree.jet_eta[i])>2.5 ) continue;
      if( myTree.jet_pt[i]<40. ) continue;
      TLorentzVector thisjet;
      thisjet.SetPtEtaPhiM( myTree.jet_pt[i], myTree.jet_eta[i], myTree.jet_phi[i], myTree.jet_mass[i] );
      float thisDeltaR = gamma.DeltaR(thisjet);
      if( thisDeltaR<deltaRmin ) {
        deltaRmin = thisDeltaR;
        closestJet = i;
      }
    }
    float found_pt = 0.;
    int jet_counter = 0;
    for( unsigned i=0; i<myTree.njet; ++i ) {
      if( i==closestJet ) continue;
      if( fabs(myTree.jet_eta[i])>2.5 ) continue;
      if( myTree.jet_pt[i]<40. ) continue;
      jet_counter++;
      if( jet_counter==2 ) {
        found_pt = myTree.jet_pt[i];
        break;
      }
    }

    if( found_pt<100. ) continue;




    Double_t weight = myTree.evt_scale1fb*lumi; 

    MT2EstimateZinvGamma* thisEstimate = analysis.get( myTree.gamma_ht, myTree.gamma_nJet40, myTree.gamma_nBJet40, myTree.gamma_met_pt );
    if( thisEstimate==0 ) continue;

    thisEstimate->yield->Fill(myTree.gamma_mt2, weight );

    float iso = myTree.gamma_chHadIso[0]/myTree.gamma_pt[0];
    int mcMatchId = myTree.gamma_mcMatchId[0];
    float genIso = myTree.gamma_genIso[0];

    bool isPrompt = ((mcMatchId==22 || mcMatchId==7) && genIso<5.);
    
    if(  onlyPrompt && !isPrompt ) continue;
    if( !onlyPrompt &&  isPrompt ) continue;

    thisEstimate->fillIso( iso, weight, myTree.gamma_mt2 );

    
  } // for entries


  analysis.finalize();
  

  delete tree;


  file->Close();
  delete file;
  
  return analysis;

}


