#include <iostream>
#include <sstream>
#include <fstream>
#include <iomanip>


#include <TTreeFormula.h>

#include "interface/MT2Sample.h"
#include "interface/MT2Region.h"
#include "interface/MT2Analysis.h"
#include "interface/MT2EstimateSyst.h"
//#include "interface/MT2RegionAnalysisUtilities.h"

#define mt2_cxx
#include "interface/mt2.h"









MT2Analysis<MT2EstimateSyst>* computeYield( const std::string& outputdir, const MT2Sample& sample, const std::string& regionsSet );
MT2Analysis<MT2EstimateSyst>* mergeYields( std::vector< MT2Analysis<MT2EstimateSyst> *> EventYield, const std::string& name, int id_min, int id_max=-1 );

std::vector<TH1D*> getYieldHistos( const std::string& prefix, MT2Analysis<MT2EstimateSyst>* EventYield_tot, MT2Analysis<MT2EstimateSyst>* EventYield_bg, std::ofstream& logfile );
//std::vector<TH1D*> getSimTruthYieldHistos( const std::string& prefix, const std::string& fakeID, MT2AnalysisManager* manager, MT2YieldAnalysis* EventYield_tot );




int main( int argc, char* argv[] ) {


  if( argc!=2 ) {
    std::cout << "USAGE: ./regionEventYields_postBabyTrees [samplesFileName]" << std::endl;
    std::cout << "Exiting." << std::endl;
    exit(11);
  }

  std::string sampleName(argv[1]);

  std::string samplesFileName = "../samples/samples_" + sampleName + ".dat";
  std::cout << std::endl << std::endl;
  std::cout << "-> Loading samples from file: " << samplesFileName << std::endl;

  std::vector<MT2Sample> fSamples = MT2Sample::loadSamples(samplesFileName);
  if( fSamples.size()==0 ) {
    std::cout << "There must be an error: samples is empty!" << std::endl;
    exit(1209);
  }


  std::string regionsSet = "8TeV";
  //MT2Analysis<MT2EstimateSyst>* manager = new MT2Analysis<MT2EstimateSyst>(regionsSet);


  TH1::AddDirectory(kFALSE); // stupid ROOT memory allocation needs this


  std::string outputdir = "EventYields_" + sampleName + "_" + regionsSet;
  system(Form("mkdir -p %s", outputdir.c_str()));

  
  std::vector< MT2Analysis<MT2EstimateSyst>* > EventYield;
  for( unsigned i=0; i<fSamples.size(); ++i )
    EventYield.push_back( computeYield( outputdir, fSamples[i], regionsSet ) );
  

  system( "rm tmp.root" );



  std::cout << "-> Done looping on samples. Start merging." << std::endl;

  //MT2Analysis<MT2EstimateSyst>* EventYield_data  = mergeYields( EventYield, "data", 1, 99 );
  //MT2Analysis<MT2EstimateSyst>* EventYield_top   = mergeYields( EventYield, "Top", 300, 499 );
  MT2Analysis<MT2EstimateSyst>* EventYield_qcd   = mergeYields( EventYield, "QCD", 100, 199 );
  //MT2Analysis<MT2EstimateSyst>* EventYield_wjets = mergeYields( EventYield, "WJets", 500, 599 );
  //MT2Analysis<MT2EstimateSyst>* EventYield_other = mergeYields( EventYield, "DY_VV", 600, 899 );

  //MT2Analysis<MT2EstimateSyst>* EventYield_signal = mergeYields( EventYield, "Signal", 1000, 10000 );

  //std::cout << "-> Done merging. Let me add a couple of them together." << std::endl;
  //
  //MT2Analysis<MT2EstimateSyst>* EventYield_topW   = (EventYield_top!=0 && EventYield_wjets!=0) ? new MT2Analysis<MT2EstimateSyst>(*EventYield_top  + *EventYield_wjets) : 0;
  //MT2Analysis<MT2EstimateSyst>* EventYield_bg     = (EventYield_qcd!=0 && EventYield_other!=0) ? new MT2Analysis<MT2EstimateSyst>(*EventYield_qcd + *EventYield_other) : 0;
  //MT2Analysis<MT2EstimateSyst>* EventYield_allMC  = (EventYield_topW!=0 && EventYield_other!=0) ? new MT2Analysis<MT2EstimateSyst>(*EventYield_topW + *EventYield_qcd + *EventYield_other) : 0;



  std::cout << "-> Done merging. Start computing event yields." << std::endl;




  //if( EventYield_allMC!=0  ) EventYield_allMC ->writeToFile( outputdir + "/yield_allMC.root"  ); 
  //if( EventYield_topW!=0   ) EventYield_topW  ->writeToFile( outputdir + "/yield_topW.root"   ); 
  //if( EventYield_bg!=0     ) EventYield_bg    ->writeToFile( outputdir + "/yield_bg.root"     ); 
  //if( EventYield_data!=0   ) EventYield_data  ->writeToFile( outputdir + "/yield_data.root"   ); 
  //if( EventYield_signal!=0 ) EventYield_signal->writeToFile( outputdir + "/yield_signal.root" ); 
  if( EventYield_qcd!=0 ) EventYield_qcd->writeToFile( outputdir + "/yield_qcd.root" );

  TFile* outfile = TFile::Open(Form("%s/EventYields_%s.root", outputdir.c_str(), sampleName.c_str()), "recreate");
  outfile->cd();
  
  std::ofstream logfile;
  logfile.open(Form("%s/EventYields_%s.txt", outputdir.c_str(), sampleName.c_str()));
  


  // For data yield:
  //std::vector<TH1D*> vh1_data = getYieldHistos( "EventYield_data", "", HTRegions, signalRegions, EventYield_data, EventYield_allMC, logfile );
  
  // For topW yield:
  //std::vector<TH1D*> vh1_data = getYieldHistos( "EventYield_topW", "", HTRegions, signalRegions, EventYield_topW, EventYield_bg, logfile );
  
  // For qcd yield
  std::vector<TH1D*> vh1_qcd = getYieldHistos( "EventYield_qcd", EventYield_qcd, 0, logfile ); 

  // For signal yield:
  //std::vector<TH1D*> vh1_signal = getYieldHistos( "EventYield_signal", EventYield_signal, 0, logfile ); 

  // For all SM yield:
  ////std::vector<TH1D*> vh1_mc   = getYieldHistos( "EventYield_MC", "", HTRegions, signalRegions, EventYield_allMC, EventYield_bg, logfile );
  //std::vector<TH1D*> vh1_mc   = getYieldHistos( "EventYield_WJets", "", HTRegions, signalRegions, EventYield_wjets, EventYield_bg, logfile );

  ////std::vector<TH1D*> vh1_sim  = getSimTruthYieldHistos( "SimulationTruthEventYield", "", HTRegions, signalRegions, EventYield_allMC );
  //std::vector<TH1D*> vh1_sim  = getSimTruthYieldHistos( "SimulationTruthEventYield", "", HTRegions, signalRegions, EventYield_signal );
 

  for( unsigned i=0; i<EventYield_qcd->getHTRegions().size(); ++i ) {

    //vh1_signal[i]->Write();
    vh1_qcd[i]->Write();

    //vh1_data[i]->Write();
    //
    //vh1_mc  [i]->Write();
    //
    //vh1_sim [i]->Write();
 
  }


  outfile->Close();



  return 0;

}




MT2Analysis<MT2EstimateSyst>* computeYield( const std::string& outputdir, const MT2Sample& sample, const std::string& regionsSet ) {


  std::cout << std::endl << std::endl;
  std::cout << "-> Starting computation for sample: " << sample.name << std::endl;

  TFile* file = TFile::Open(sample.file.c_str());
  TTree* tree = (TTree*)file->Get("mt2");
  

  std::ostringstream preselectionStream;
  preselectionStream << " " 
                     << "(nTaus20==0 && nMuons10==0 && nElectrons10==0)" << " && "
                     << "(nVert > 0)"                      << " && "
                     << "(nJet40 > 1)"                     << " && "
                     << "(jet_pt[1] > 100)"                << " && "
                     << "(ht > 450)"                       << " && "
                     << "((ht < 750 && met_pt > 200) || (ht > 750 && met_pt > 30))" << " && "
                     << "(deltaPhiMin > 0.3)"              << " && " 
                     << "(diffMetMht < 70)"                << "&&"
                     << "(mt2 > 50)" ;

  

  TString preselection = preselectionStream.str().c_str();
  TString cuts = preselection;

  TFile* tmpFile = TFile::Open("tmp.root", "recreate");
  tmpFile->cd();

  std::cout << "-> Skimming tree by applying preselection." << std::endl;
  TTree* tree_reduced = tree->CopyTree(cuts);

  MT2Tree myTree;
  myTree.Init(tree_reduced);

    

  bool isData = false;

  // global sample weight:
  ////Double_t weight = sample.xsection * sample.kfact * sample.lumi / (sample.nevents*sample.PU_avg_weight);
  //Double_t weight = sample.xsection * sample.kfact * sample.lumi / (sample.nevents);
  //cout << endl << "Weight " << weight <<endl; 


  std::cout << "-> Setting up MT2Analysis with name: " << sample.sname << std::endl;
  MT2Analysis<MT2EstimateSyst>* analysis = new MT2Analysis<MT2EstimateSyst>( sample.sname, regionsSet, sample.id );

  

  int nentries = tree_reduced->GetEntries();

  ofstream ofs("events.log");


  for( unsigned iEntry=0; iEntry<nentries; ++iEntry ) {

    if( iEntry % 50000 == 0 ) std::cout << "    Entry: " << iEntry << " / " << nentries << std::endl;

    myTree.GetEntry(iEntry);

    int evt   = myTree.evt;
    float ht   = myTree.ht;
    float met  = myTree.met_pt;
    float mt2  = myTree.mt2;
    int njets  = myTree.nJet40;
    int nbjets = myTree.nBJet40;

    Double_t weight = myTree.evt_scale1fb; 

    float fullweight_btagUp = weight;
    float fullweight_btagDown = weight;



    MT2EstimateSyst* thisEstimate = analysis->get( ht, njets, nbjets, met );
    if( thisEstimate==0 ) continue;

    if( isData ) {
      //// fix HLT selection here
      //TTreeFormula* this_ttf = new TTreeFormula(Form("ttf_hlt_%s", HTRegions[iHT].getName().c_str()), HTRegions[iHT].HLT_selection.c_str(), tree);
      //ttf_hlt.push_back(this_ttf);
    }   

          ofs << evt << " " << ht << " " << njets << " " << nbjets << " " << met << " " << weight << " " <<  thisEstimate->region->getName() << std::endl;

    thisEstimate->yield         ->Fill(300., weight );
    thisEstimate->yield_btagUp  ->Fill(300., fullweight_btagUp );
    thisEstimate->yield_btagDown->Fill(300., fullweight_btagDown );

    
  } // for entries

  ofs.close();

  analysis->finalize();
  
  delete tree;
  delete tree_reduced;

  tmpFile->Close();
  delete tmpFile;

  file->Close();
  delete file;
  
  return analysis;

}








MT2Analysis<MT2EstimateSyst>* mergeYields( std::vector<MT2Analysis<MT2EstimateSyst> *> EventYield, const std::string& name, int id_min, int id_max ) {

  if( id_max<0 ) id_max=id_min;

  MT2Analysis<MT2EstimateSyst>* return_EventYield = 0;

  for( unsigned i=0; i<EventYield.size(); ++i ) {

    if( EventYield[i]->id >= id_min && EventYield[i]->id <= id_max ) {

        if( return_EventYield==0 )
          return_EventYield = EventYield[i];
          //return_EventYield = new MT2Analysis<MT2EstimateSyst>(*(EventYield[i]));
        else 
          return_EventYield->add(*(EventYield[i]));
	
	std::cout << "E 1" << std::endl;

    }

  } // for EventYield


  return return_EventYield;

}







std::vector<TH1D*> getYieldHistos( const std::string& prefix, MT2Analysis<MT2EstimateSyst>* EventYield_tot, MT2Analysis<MT2EstimateSyst>* EventYield_bg, std::ofstream& logfile ) {


  std::set<MT2HTRegion> HTRegions = EventYield_tot->getHTRegions();
  std::set<MT2SignalRegion> signalRegions = EventYield_tot->getSignalRegions();


  int nBins = signalRegions.size();


  std::vector<TH1D*> histos;
  
  std::cout << std::endl << std::endl;
  std::cout << "Event yield for sample " << prefix.c_str() << std::endl;
  
  logfile << std::endl << std::endl <<  "Event yield for sample " << prefix.c_str() << std::endl;
  
  for( std::set<MT2HTRegion>::iterator iHT = HTRegions.begin(); iHT!=HTRegions.end(); ++iHT ) {

    TH1D* h1 = new TH1D(Form("%s_%s", prefix.c_str(), iHT->getName().c_str()), "", nBins, 0., nBins);
    h1->Sumw2();

    //float EventYield_sum = 0;
    //float statErr_sum = 0;

    std::cout << std::endl << std::endl;
    std::cout << "HT region: " << iHT->getName() << std::endl;
    logfile << std::endl << "HT region: " << iHT->getName() << std::endl;

    int iBin = 0;
    
    for( std::set<MT2SignalRegion>::iterator iSR = signalRegions.begin(); iSR!=signalRegions.end(); ++iSR ) {
  
      iBin += 1;

      MT2Region thisRegion( &(*iHT), &(*iSR) );

      h1->GetXaxis()->SetBinLabel(iBin, iSR->getName().c_str());

      MT2EstimateSyst* thisEstimate_tot = EventYield_tot->get( &thisRegion );

      if( thisEstimate_tot==0 ) {
        std::cout << "This should not be possible!" << std::endl;
        exit(93);
      }

      TH1D* histo = thisEstimate_tot->yield;
      int nBins = histo->GetNbinsX();
      double err_histo = 0;
      float tot = histo->IntegralAndError(0, nBins+1, err_histo);

      //int tot = 0;
      //
      //if(EventYield_tot!=0){
      //	MT2RegionAnalysis* thisYield = EventYield_tot->getRegion( thisRegion.getName() );
      //	
      //	cout<< "DEBUG1" << endl;
      //	
      //  (TH1D*) (thisYield->yield)->Clone();
      //	
      //	cout<< "DEBUG2"<< endl;
      //	
      //}
      //else ;

      //double err_histo = 0;
      //float  tot = (EventYield_tot!=0 && EventYield_tot->f[fakeID.c_str()]!=0) ? EventYield_tot->f[fakeID.c_str()]->getRegion(thisRegion.getName())->yield->IntegralAndError(0, 2, err_histo) : 0.;
      //float  bg = (EventYield_bg !=0 && EventYield_bg ->f[fakeID.c_str()]!=0) ? EventYield_bg ->f[fakeID.c_str()]->getRegion(thisRegion.getName())->yield->Integral() : 0.;
      //float  bg_btagUp = (EventYield_bg !=0 && EventYield_bg ->f[fakeID.c_str()]!=0) ? EventYield_bg ->f[fakeID.c_str()]->getRegion(thisRegion.getName())->yield_btagUp->Integral() : 0.;
      //float  bg_btagDown = (EventYield_bg !=0 && EventYield_bg ->f[fakeID.c_str()]!=0) ? EventYield_bg ->f[fakeID.c_str()]->getRegion(thisRegion.getName())->yield_btagDown->Integral() : 0.;

      //float EventYield = tot-bg;
      
      float EventYield = tot;
      h1->SetBinContent( iBin, EventYield );
      h1->SetBinError( iBin, err_histo );

      //cout << "tot " << tot << endl;
      
      ////float  tot = (EventYield_tot!=0) ? EventYield_tot->getRegion(thisRegion.getName())->yield->Integral() : 0.;      
      //float  bg  = (EventYield_bg !=0) ? EventYield_bg->getRegion(thisRegion.getName())->yield->Integral() : 0.;
      //float  bg_btagUp   = (EventYield_bg !=0) ? EventYield_bg->getRegion(thisRegion.getName())->yield_btagUp->Integral() : 0.;
      //float  bg_btagDown = (EventYield_bg !=0) ? EventYield_bg->getRegion(thisRegion.getName())->yield_btagDown->Integral() : 0.;
      //
      //float EventYield = (tot-bg);
      //
      //cout<< "Credo il problema sia qui sopra.";
      //
      //h1->SetBinContent( iBin, EventYield );

      std::cout << std::endl;
      std::cout << "Signal region: " << iSR->getName().c_str() << std::endl;  
     
      //std::cout << "tot: " << tot << std::endl;
      //std::cout << "bg: " << bg << std::endl;
      
      std::cout << "yield: " << EventYield << std::endl;
      std::cout << "hist_err: " << err_histo << std::endl;
           
      if(EventYield >= 10)
	logfile << std::fixed << std::setprecision(1) << EventYield << " $\\pm$ " << err_histo << " & ";
      else if(EventYield < 10)
	logfile << std::fixed << std::setprecision(2) << EventYield << " $\\pm$ " << err_histo << " & ";

      //float statErr = sqrt(tot);
      //float sysErr_dbg = 0.5*bg; // 50% uncert on BG estimate
      //float bg_SFerrUp = fabs(bg_btagUp-bg);
      //float bg_SFerrDown = fabs(bg_btagDown-bg);
      //float bg_SFerr = 0.5*(bg_SFerrUp+bg_SFerrDown);
      //float sysErr_btag = bg_SFerr;

      //float sysErr_dLL = nWT_goodrecoevt_dLL*(1.*effLept_err/(effLept*effLept*effMT));//additional error due to double lost with 100% uncert

      //float sysErr = sqrt( sysErr_dbg*sysErr_dbg + sysErr_btag*sysErr_btag );
      //std::cout << "sysErr_dbg: " << sysErr_dbg << std::endl;
      //std::cout << "sysErr_btag: " << sysErr_btag << std::endl;
      //std::cout << "sysErr: " << sysErr<< std::endl;
      //std::cout << "statErr: " << statErr<< std::endl; 

      //h1->SetBinError( iBin, sqrt(statErr*statErr + sysErr*sysErr) );
      h1->SetBinError( iBin, err_histo );

    }

    histos.push_back(h1);

  } // for

  return histos;

}





/*
std::vector<TH1D*> getSimTruthYieldHistos( const std::string& prefix, const std::string& fakeID, MT2AnalysisManager* manager, MT2YieldAnalysis* EventYield_tot ) {


  std::vector<MT2HTRegion> HTRegions = manager->getHTRegions();
  std::vector<MT2SignalRegion> signalRegions = manager->getSignalRegions();


  int nHistos = HTRegions.size();
  int nBins = signalRegions.size();


  
  std::vector<TH1D*> histos;

  for( int i=0; i<nHistos; ++i ) {

    TH1D* h1 = new TH1D(Form("%s_%s", prefix.c_str(), HTRegions[i].name.c_str()), "", nBins, 0., nBins);
    h1->Sumw2();

    for( int j=0; j<nBins; ++j ) {
  
      int iBin = j+1;

      MT2Region thisRegion( &HTRegions[i], &signalRegions[j] );

      h1->GetXaxis()->SetBinLabel(iBin, signalRegions[j].getName().c_str());

      float totYield = (EventYield_tot!=0) ? EventYield_tot->f[fakeID.c_str()]->getRegionGen(thisRegion.getName())->yield->Integral() : 0.;

      h1->SetBinContent( iBin, totYield );

    }

    histos.push_back(h1);

  } // for

  return histos;

}


*/


