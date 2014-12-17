#include <iostream>
#include <sstream>
#include <fstream>
#include <iomanip>



#include "interface/MT2Sample.h"
#include "interface/MT2Region.h"
#include "interface/MT2Analysis.h"
#include "interface/MT2Estimate.h"

#define mt2_cxx
#include "../interface/mt2.h"


#include "TCanvas.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TLegend.h"
#include "TLorentzVector.h"




float lumi = 5.; // fb-1



MT2Analysis<MT2Estimate> computeYield( const MT2Sample& sample, const std::string& regionsSet, const std::string& prefix="" );
void drawCompare( const std::string& outputdir, MT2Analysis<MT2Estimate>* ZinvEstimate, MT2Analysis<MT2Estimate>* Zinv );
void drawSinglePlot( const std::string& outputdir, const std::string& name, TH1D* h1_ratio, TH1D* h1_mc );
void addPoissonError( MT2Analysis<MT2Estimate>* analysis );


int main( int argc, char* argv[] ) {


  //if( argc!=2 ) {
  //  std::cout << "USAGE: ./computeZinvFromGamma [samplesFileName]" << std::endl;
  //  std::cout << "Exiting." << std::endl;
  //  exit(11);
  //}


  std::string samplesFileName = "CSA14_Zinv";
  if( argc>1 ) {
    std::string samplesFileName_tmp(argv[1]); 
    samplesFileName = samplesFileName_tmp;
  }

  std::string samplesFile = "../samples/samples_" + samplesFileName + ".dat";

  std::cout << std::endl << std::endl;
  std::cout << "-> Loading gamma+jet samples" << std::endl;

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

  TH1::AddDirectory(kFALSE); // stupid ROOT memory allocation needs this


  std::string outputdir = "ZinvEstimateFromGamma_" + samplesFileName + "_" + regionsSet;
  system(Form("mkdir -p %s", outputdir.c_str()));

  
  MT2Analysis<MT2Estimate>* gammaJet = new MT2Analysis<MT2Estimate>( "gammaJet", regionsSet );
  for( unsigned i=0; i<samples_gammaJet.size(); ++i ) {
    (*gammaJet) += (computeYield( samples_gammaJet[i], regionsSet, "gamma_" ));
  }


  
  MT2Analysis<MT2Estimate>* qcd = new MT2Analysis<MT2Estimate>( "qcd", regionsSet );
  for( unsigned i=0; i<samples_qcd.size(); ++i ) {
    (*qcd) += (computeYield( samples_qcd[i], regionsSet, "gamma_" ));
  }

  MT2Analysis<MT2Estimate>* gamma_plus_qcd = new MT2Analysis<MT2Estimate>( "gamma_plus_qcd", regionsSet );
  *gamma_plus_qcd = *gammaJet;
  *gamma_plus_qcd += *qcd;
  
  MT2Analysis<MT2Estimate>* purity = new MT2Analysis<MT2Estimate>( "purity", regionsSet );
  *purity = *gammaJet;
  *purity /= *gamma_plus_qcd;
  purity->setName( "purity" );

  MT2Analysis<MT2Estimate>* Zinv = new MT2Analysis<MT2Estimate>( "Zinv", regionsSet );
  for( unsigned i=0; i<samples_Zinv.size(); ++i ) {
    (*Zinv) += (computeYield( samples_Zinv[i], regionsSet ));
  }
  

  //TH1::AddDirectory(kTRUE); // stupid ROOT memory allocation needs this

  //MT2Analysis<MT2Estimate>* ZgammaRatio = new MT2Analysis<MT2Estimate>( (*Zinv)/(*gammaJet) );
  //ZgammaRatio->setName("ZgammaRatio");
  MT2Analysis<MT2Estimate>* ZgammaRatio = new MT2Analysis<MT2Estimate>( "ZgammaRatio", regionsSet );
  (*ZgammaRatio) = (*Zinv);
  (*ZgammaRatio) /= (*gammaJet);


  // now that the MC ratio is done, add poisson error to gammajet sample:
  addPoissonError(gammaJet);



  //MT2Analysis<MT2Estimate>* ZinvEstimate = new MT2Analysis<MT2Estimate>( "ZinvEstimate" );
  //(*ZinvEstimate) = (*ZgammaRatio);
  //(*ZinvEstimate) *= (*gammaJet);
  MT2Analysis<MT2Estimate>* ZinvEstimate = new MT2Analysis<MT2Estimate>( (*ZgammaRatio)*(*gammaJet) );
  ZinvEstimate->setName("ZinvEstimate");

  std::string outputdirPlots = outputdir + "/plots";
  system(Form("mkdir -p %s", outputdirPlots.c_str()));


  ZinvEstimate->writeToFile( "MT2ZinvEstimate.root" );

  gammaJet->writeToFile( "prova_gammaJet.root" );
  Zinv->writeToFile( "prova_Zinv.root" );
  ZgammaRatio->writeToFile( "prova_ZgammaRatio.root" );
  ZinvEstimate->writeToFile( "prova_ZinvEstimate.root" );

  drawCompare( outputdirPlots, ZinvEstimate, Zinv );
  


  return 0;

}




MT2Analysis<MT2Estimate> computeYield( const MT2Sample& sample, const std::string& regionsSet, const std::string& prefix ) {


  std::cout << std::endl << std::endl;
  std::cout << "-> Starting computation for sample: " << sample.name << std::endl;

  TFile* file = TFile::Open(sample.file.c_str());
  TTree* tree = (TTree*)file->Get("mt2");
  
  std::cout << "-> Loaded tree: it has " << tree->GetEntries() << " entries." << std::endl;



  MT2Analysis<MT2Estimate> analysis( sample.sname, regionsSet, sample.id );

  
  MT2Tree myTree;
  myTree.loadGenStuff = false;
  myTree.Init(tree);

  int nentries = tree->GetEntries();

  float mt2;
  tree->SetBranchAddress( Form("%smt2"   , prefix.c_str()), &mt2 );
  float ht;
  tree->SetBranchAddress( Form("%sht"    , prefix.c_str()), &ht );
  float met;
  tree->SetBranchAddress( Form("%smet_pt", prefix.c_str()), &met );
  int njets;
  tree->SetBranchAddress( Form("%snJet40", prefix.c_str()), &njets );
  int nbjets;
  tree->SetBranchAddress( Form("%snBJet40", prefix.c_str()), &nbjets );


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

    if( myTree.ngamma>0 && prefix=="gamma_" ) {
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

    } else {

      if( myTree.jet_pt[1]<100. ) continue;

    }


    Double_t weight = myTree.evt_scale1fb*lumi; 

    MT2Estimate* thisEstimate = analysis.get( ht, njets, nbjets, met );
    if( thisEstimate==0 ) continue;

    thisEstimate->yield->Fill(mt2, weight );

    
  } // for entries


  analysis.finalize();
  

  delete tree;

  file->Close();
  delete file;
  
  return analysis;

}



void drawCompare( const std::string& outputdir, MT2Analysis<MT2Estimate>* ZinvEstimate, MT2Analysis<MT2Estimate>* Zinv ) {

  std::set<MT2HTRegion> htRegions = Zinv->getHTRegions();
  std::set<MT2SignalRegion> signalRegions = Zinv->getSignalRegions();

    for( std::set<MT2HTRegion>::iterator iHT=htRegions.begin(); iHT!=htRegions.end(); ++iHT ) {
      for( std::set<MT2SignalRegion>::iterator iSR=signalRegions.begin(); iSR!=signalRegions.end(); ++iSR ) {
  
        MT2Region thisRegion(*iHT, *iSR);

        MT2Estimate* est_ratio = ZinvEstimate->get(thisRegion); 
        MT2Estimate* est_mc    = Zinv->get(thisRegion); 

        drawSinglePlot( outputdir, thisRegion.getName(), est_ratio->yield, est_mc->yield );

      }
    }
}



void drawSinglePlot( const std::string& outputdir, const std::string& name, TH1D* h1_ratio, TH1D* h1_mc ) {

  TCanvas* c1 = new TCanvas("c11", "", 600, 600);
  c1->cd();

  h1_ratio->SetLineColor(kRed);
  h1_ratio->SetLineWidth(2);

  h1_mc->SetMarkerColor(kBlue);
  h1_mc->SetMarkerStyle(20);
  h1_mc->SetMarkerSize(2);

  h1_ratio->Draw();
  h1_mc->Draw("p same");
  
  c1->SaveAs( Form("%s/estVsMc_%s.eps", outputdir.c_str(), name.c_str()) );
  c1->SaveAs( Form("%s/estVsMc_%s.png", outputdir.c_str(), name.c_str()) );

  delete c1;

}




void addPoissonError( MT2Analysis<MT2Estimate>* analysis ) {


  std::set<MT2Region> regions = analysis->getRegions();

  for( std::set<MT2Region>::iterator iR = regions.begin(); iR!=regions.end(); ++iR ) {

      TH1D* h1 = analysis->get(*iR)->yield;

      for( unsigned ibin=1; ibin<h1->GetXaxis()->GetNbins()+1; ++ibin ) {

        int nData = (int) h1->GetBinContent(ibin);
        h1->SetBinContent(ibin, nData);
        if( nData==0 )
          h1->SetBinError(ibin, 1.);
        else
          h1->SetBinError(ibin, sqrt((float)nData));

      }  // for bins

  }// for regions

}
