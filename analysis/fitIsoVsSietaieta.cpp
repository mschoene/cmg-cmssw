#include "interface/MT2Sample.h"
#include "interface/MT2Region.h"
#include "interface/MT2Analysis.h"
#include "interface/MT2EstimateTree.h"
#include "interface/MT2DrawTools.h"


#define mt2_cxx
#include "../interface/mt2.h"


#include "TLorentzVector.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TF1.h"
#include "TRandom3.h"
#include "TProfile.h"
#include "TCanvas.h"


float lumi = 4.; //fb-1
bool dummyData = true;



MT2Analysis<MT2EstimateTree> computeYield( const MT2Sample& sample, const std::string& regionsSet, const std::string& prompt_fake="all");



int main( int argc, char* argv[] ) {


  std::string regionsSet = "13TeV_inclusive";

  std::string samplesFileName = "PHYS14_v2_Zinv_noSietaieta";

  std::string samplesFile = "../samples/samples_" + samplesFileName + ".dat";
  
  std::vector<MT2Sample> samples = MT2Sample::loadSamples(samplesFile, 100, 199); // QCD only
  if( samples.size()==0 ) {
    std::cout << "There must be an error: didn't find any files in " << samplesFile << "!" << std::endl;
    exit(1209);
  }


  TH1::AddDirectory(kFALSE); // stupid ROOT memory allocation needs this


  std::string outputdir = "ZinvGammaPurityFromData_" + samplesFileName + "_" + regionsSet;
  system(Form("mkdir -p %s", outputdir.c_str()));


  
  MT2Analysis<MT2EstimateTree>* templatesFake = new MT2Analysis<MT2EstimateTree>( "templatesFake", regionsSet );


  for( unsigned i=0; i<samples.size(); ++i ) {
    (*templatesFake) += (computeYield( samples[i], regionsSet, "fake"  ));
  }


  std::string outfilename = "IsoFit_" + samplesFileName + "_" + regionsSet + ".root";
  templatesFake->writeToFile(outfilename);
  
  TH1::AddDirectory(kTRUE); // stupid ROOT memory allocation needs this
  
  TTree* tree = templatesFake->get(*(templatesFake->getRegions().begin()))->tree;

  MT2DrawTools::setStyle();


  TProfile* hp_iso_vs_sigma = new TProfile("iso_vs_sigma", "", 50, 0., 0.02);
  tree->Project( "iso_vs_sigma", "iso:sietaieta", "", "profile" );

  TF1* f1 = new TF1("f1", "[0] + [1]*x", 0.0085, 0.019);
  //TF1* f1 = new TF1("f1", "[0] + [1]*x + [2]*x*x", 0.0085, 0.018);
  f1->SetLineColor(kRed);
  f1->SetParameter( 0, 0.024 );
  f1->SetParameter( 1, 0.1 );
  hp_iso_vs_sigma->Fit( f1, "R+" );

  
  // add to file
  TFile* file = TFile::Open(outfilename.c_str(), "update");
  hp_iso_vs_sigma->Write();
  file->Close();


  TCanvas* c1 = new TCanvas("c1", "", 600, 600);
  c1->cd();

  TH2D* h2_axes = new TH2D("axes", "", 10, 0.008, 0.02, 10, 0., 0.3 );
  h2_axes->SetXTitle( "Photon #sigma_{i#eta i#eta}" );
  h2_axes->SetYTitle( "Photon Relative Charged Isolation" );
  h2_axes->Draw();

  TPaveText* labelTop = MT2DrawTools::getLabelTop();
  labelTop->Draw("same");

  TPaveText* fakeLabel = new TPaveText( 0.7, 0.2, 0.9, 0.3, "brNDC");
  fakeLabel->SetTextAlign(31);
  fakeLabel->SetFillColor(0);
  fakeLabel->SetTextSize(0.035);
  fakeLabel->AddText("Fake Photons");
  fakeLabel->AddText("Barrel Only");
  fakeLabel->Draw("same");

  hp_iso_vs_sigma->SetMarkerStyle(20);
  hp_iso_vs_sigma->SetMarkerSize(1.3);
  hp_iso_vs_sigma->SetLineColor(kBlack);
  hp_iso_vs_sigma->SetMarkerColor(kBlack);
  hp_iso_vs_sigma->Draw("P same");

  TLine* lineCut = new TLine( 0.01, 0., 0.01, 0.3 );
  lineCut->Draw("same");

  gPad->RedrawAxis();

  c1->SaveAs(Form("IsoFit_%s_%s.eps", samplesFileName.c_str(), regionsSet.c_str()));
  c1->SaveAs(Form("IsoFit_%s_%s.pdf", samplesFileName.c_str(), regionsSet.c_str()));
  c1->SaveAs(Form("IsoFit_%s_%s.png", samplesFileName.c_str(), regionsSet.c_str()));
  

  return 0;

}






MT2Analysis<MT2EstimateTree> computeYield( const MT2Sample& sample, const std::string& regionsSet, const std::string& prompt_fake ) {


  std::cout << std::endl << std::endl;
  std::cout << "-> Starting computation for sample: " << sample.name << std::endl;

  TFile* file = TFile::Open(sample.file.c_str());
  TTree* tree = (TTree*)file->Get("mt2");
  
  std::cout << "-> Loaded tree: it has " << tree->GetEntries() << " entries." << std::endl;



  MT2Analysis<MT2EstimateTree> analysis( sample.sname, regionsSet, sample.id );
  MT2EstimateTree::addVar( &analysis, "iso" );
  MT2EstimateTree::addVar( &analysis, "sietaieta" );

  
  MT2Tree myTree;
  myTree.loadGenStuff = false;
  myTree.Init(tree);

  int nentries = tree->GetEntries();




  for( unsigned iEntry=0; iEntry<nentries; ++iEntry ) {

    if( iEntry % 50000 == 0 ) std::cout << "    Entry: " << iEntry << " / " << nentries << std::endl;

    myTree.GetEntry(iEntry);

    // template control region (use low-significance regions):
    if( myTree.gamma_ht > 1000. ) continue;
    if( myTree.gamma_mt2 < 200.) continue;
    if( myTree.gamma_mt2 > 300. ) continue;
    if( myTree.met_pt > 100.) continue;
    //if( myTree.gamma_nJet40 > 4) continue;
    if( myTree.gamma_nBJet40 > 1) continue;
    //if( myTree.gamma_ht>1000. && sample.id==204 ) continue; // remove high-weight spikes (remove GJet_400to600 leaking into HT>1000)



    if( myTree.nMuons10 > 0) continue;
    if( myTree.nElectrons10 > 0 ) continue;
    if( myTree.nPFLep5LowMT > 0) continue;
    if( myTree.nPFHad10LowMT > 0) continue;

    if( myTree.gamma_deltaPhiMin<0.3 ) continue;
    if( myTree.gamma_diffMetMht>0.5*myTree.gamma_met_pt ) continue;
  
    if( myTree.nVert==0 ) continue;

    if( myTree.gamma_nJet40<2 ) continue;

    if( myTree.ngamma==0 ) continue;



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


    float sietaieta = myTree.gamma_sigmaIetaIeta[0];

    // for now only EB:
    if( fabs(gamma.Eta())>1.4 ) continue;
    // safety upper thresh on sietaieta:
    if( sietaieta>0.02 ) continue;


    // remove prompt photons from QCD (remove double counting)
    if( sample.id>=100 && sample.id<199 ) {
      int mcMatchId = myTree.gamma_mcMatchId[0];
      float genIso = myTree.gamma_genIso[0];
      if((mcMatchId==22 || mcMatchId==7) && genIso<5.) continue;
    }
    


    Double_t weight = myTree.evt_scale1fb*lumi; 

    MT2EstimateTree* thisEstimate = analysis.get( myTree.gamma_ht, myTree.gamma_nJet40, myTree.gamma_nBJet40, myTree.gamma_met_pt );
    if( thisEstimate==0 ) continue;

    thisEstimate->yield->Fill(myTree.gamma_mt2, weight );

    float iso = myTree.gamma_chHadIso[0]/myTree.gamma_pt[0];

    thisEstimate->assignTree(myTree, lumi*myTree.evt_scale1fb);
    thisEstimate->assignVar( "sietaieta", sietaieta );
    thisEstimate->assignVar( "iso", iso );
    thisEstimate->tree->Fill();

    
  } // for entries


  analysis.finalize();
  

  delete tree;


  file->Close();
  delete file;
  
  return analysis;

}





