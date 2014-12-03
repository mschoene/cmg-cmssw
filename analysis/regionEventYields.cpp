#include <iostream>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <string>


#include "TTreeFormula.h"
#include "TH1D.h"
#include "TH2D.h"
#include "THStack.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "TPaveText.h"
#include "TGraphAsymmErrors.h"


#include "interface/MT2Sample.h"
#include "interface/MT2Region.h"
#include "interface/MT2Analysis.h"
#include "interface/MT2EstimateSyst.h"
#include "interface/MT2DrawTools.h"

#define mt2_cxx
#include "interface/mt2.h"



bool dummyAnalysis = true;



class MT2Config {

 public:

  MT2Config( const std::string& configFileName );

  std::string regionsSet()    const { return regionsSet_; };
  std::string mcSamples()     const { return mcSamples_; };
  std::string dataSamples()   const { return dataSamples_; };
  std::string lostLeptonTag() const { return lostLeptonTag_; };
  std::string qcdTag()        const { return qcdTag_; };
  std::string zinvTag()       const { return zinvTag_; };

  bool useMC() {
    return mcSamples_!="";
  }

 private:

  std::string regionsSet_;
  std::string mcSamples_;
  std::string dataSamples_;
  std::string lostLeptonTag_;
  std::string qcdTag_;
  std::string zinvTag_;

};





MT2Analysis<MT2EstimateSyst>* computeYield( const MT2Sample& sample, const std::string& regionsSet );
MT2Analysis<MT2EstimateSyst>* mergeYields( std::vector< MT2Analysis<MT2EstimateSyst> *> EventYield, const std::string& regionsSet, const std::string& name, int id_min, int id_max=-1, const std::string& legendName="" );

std::vector<TH1D*> getYieldHistos( const std::string& prefix, MT2Analysis<MT2EstimateSyst>* EventYield_tot, MT2Analysis<MT2EstimateSyst>* EventYield_bg, std::ofstream& logfile );
//std::vector<TH1D*> getSimTruthYieldHistos( const std::string& prefix, const std::string& fakeID, MT2AnalysisManager* manager, MT2YieldAnalysis* EventYield_tot );

void drawYields( const std::string& outputdir, MT2Analysis<MT2EstimateSyst>* data, std::vector<MT2Analysis<MT2EstimateSyst>* > bgYields );



int main( int argc, char* argv[] ) {


  if( argc!=2 ) {
    std::cout << "USAGE: ./regionEventYields_postBabyTrees [configFileName]" << std::endl;
    std::cout << "Exiting." << std::endl;
    exit(11);
  }


  std::string configFileName(argv[1]);


  std::string outputdir = "EventYields_" + configFileName;
  if( dummyAnalysis ) outputdir += "_dummy";
  system(Form("mkdir -p %s", outputdir.c_str()));

  MT2Config cfg("cfgs/" + configFileName + ".txt");


  TH1::AddDirectory(kFALSE); // stupid ROOT memory allocation needs this


  std::vector<MT2Analysis<MT2EstimateSyst>* > bgYields;



  if( cfg.useMC() ) { // use MC BG estimates


    std::string samplesFileName = "../samples/samples_" + cfg.mcSamples() + ".dat";
    std::cout << std::endl << std::endl;
    std::cout << "-> Loading samples from file: " << samplesFileName << std::endl;

    std::vector<MT2Sample> fSamples = MT2Sample::loadSamples(samplesFileName);
    if( fSamples.size()==0 ) {
      std::cout << "There must be an error: samples is empty!" << std::endl;
      exit(1209);
    }


    
    std::vector< MT2Analysis<MT2EstimateSyst>* > EventYield;
    for( unsigned i=0; i<fSamples.size(); ++i )
      EventYield.push_back( computeYield( fSamples[i], cfg.regionsSet() ) );
    

    system( "rm tmp.root" );


    std::cout << "-> Done looping on samples. Start merging." << std::endl;

    MT2Analysis<MT2EstimateSyst>* EventYield_top   = mergeYields( EventYield, cfg.regionsSet(), "Top", 300, 499 );
    MT2Analysis<MT2EstimateSyst>* EventYield_qcd   = mergeYields( EventYield, cfg.regionsSet(), "QCD", 100, 199 );
    MT2Analysis<MT2EstimateSyst>* EventYield_wjets = mergeYields( EventYield, cfg.regionsSet(), "WJets", 500, 599, "W + Jets" );
    MT2Analysis<MT2EstimateSyst>* EventYield_other = mergeYields( EventYield, cfg.regionsSet(), "DY_VV", 600, 899, "Other" );

    bgYields.push_back( EventYield_top );
    bgYields.push_back( EventYield_qcd );
    bgYields.push_back( EventYield_wjets );
    bgYields.push_back( EventYield_other );

  } else { // use data driven BG estimates

    MT2Analysis<MT2EstimateSyst>* lostLepton = MT2Analysis<MT2EstimateSyst>::readFromFile("MT2LostLeptonEstimate_" + cfg.lostLeptonTag() + ".root");

    MT2Analysis<MT2EstimateSyst>* qcd = MT2Analysis<MT2EstimateSyst>::readFromFile("MT2QCDEstimate_" + cfg.qcdTag() + ".root");

    MT2Analysis<MT2EstimateSyst>* zinv = MT2Analysis<MT2EstimateSyst>::readFromFile("MT2ZinvEstimate_" + cfg.zinvTag() + ".root");

    bgYields.push_back( lostLepton );
    bgYields.push_back( qcd );
    bgYields.push_back( zinv );

  }



  MT2Analysis<MT2EstimateSyst>* data = new MT2Analysis<MT2EstimateSyst>( "data", cfg.regionsSet() );

  if( dummyAnalysis ) { // use same as MC

    for( unsigned i=0; i<bgYields.size(); ++i ) *data += *(bgYields[i]);

  } else {

    std::string samplesFile_data = "../samples/samples_" + cfg.dataSamples() + ".dat";

    std::cout << std::endl << std::endl;
    std::cout << "-> Loading data from file: " << samplesFile_data << std::endl;

    std::vector<MT2Sample> samples_data = MT2Sample::loadSamples(samplesFile_data);
    if( samples_data.size()==0 ) {
      std::cout << "There must be an error: samples_data is empty!" << std::endl;
      exit(1209);
    }

    for( unsigned i=0; i<samples_data.size(); ++i ) (*data) += *(computeYield( samples_data[i], cfg.regionsSet() ));

  }


  drawYields( outputdir, data, bgYields );

  return 0;

}


/*
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
*/



MT2Analysis<MT2EstimateSyst>* computeYield( const MT2Sample& sample, const std::string& regionsSet ) {


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

  //ofstream ofs("events.log");


  for( unsigned iEntry=0; iEntry<nentries; ++iEntry ) {

    if( iEntry % 50000 == 0 ) std::cout << "    Entry: " << iEntry << " / " << nentries << std::endl;

    myTree.GetEntry(iEntry);

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

    //      ofs << evt << " " << ht << " " << njets << " " << nbjets << " " << met << " " << weight << " " <<  thisEstimate->region->getName() << std::endl;

    thisEstimate->yield         ->Fill(mt2, weight );
    thisEstimate->yield_btagUp  ->Fill(mt2, fullweight_btagUp );
    thisEstimate->yield_btagDown->Fill(mt2, fullweight_btagDown );

    
  } // for entries

  //ofs.close();

  analysis->finalize();
  
  delete tree;
  delete tree_reduced;

  tmpFile->Close();
  delete tmpFile;

  file->Close();
  delete file;
  
  return analysis;

}








MT2Analysis<MT2EstimateSyst>* mergeYields( std::vector<MT2Analysis<MT2EstimateSyst> *> EventYield, const std::string& regionsSet, const std::string& name, int id_min, int id_max, const std::string& legendName ) {

  if( id_max<0 ) id_max=id_min;

  MT2Analysis<MT2EstimateSyst>* return_EventYield = new MT2Analysis<MT2EstimateSyst>(name, regionsSet, id_min, legendName);

  for( unsigned i=0; i<EventYield.size(); ++i ) {

    if( EventYield[i]->id >= id_min && EventYield[i]->id <= id_max ) {

        *(return_EventYield) += *(EventYield[i]);
        //if( return_EventYield==0 )
        //  //return_EventYield = new MT2Analysis<MT2EstimateSyst>(*(EventYield[i]));
        //else 
        //  return_EventYield->add(*(EventYield[i]));

    }

  } // for EventYield


  return return_EventYield;

}




void drawYields( const std::string& outputdir, MT2Analysis<MT2EstimateSyst>* data, std::vector< MT2Analysis<MT2EstimateSyst> *> bgYields ) {


  MT2DrawTools::setStyle();

  std::vector<int> colors;
  if( bgYields.size()==3 ) {
    colors.push_back(402); 
    colors.push_back(430); 
    colors.push_back(418); 
  } else {
    colors.push_back(401); 
    colors.push_back(417); 
    colors.push_back(419); 
    colors.push_back(855); 
  }


  std::set<MT2HTRegion> HTRegions = data->getHTRegions();
  std::set<MT2SignalRegion> signalRegions = data->getSignalRegions();

  
  for( std::set<MT2HTRegion>::iterator iHT = HTRegions.begin(); iHT!=HTRegions.end(); ++iHT ) {


    for( std::set<MT2SignalRegion>::iterator iSR = signalRegions.begin(); iSR!=signalRegions.end(); ++iSR ) {

      std::string fullPath = outputdir + "/" + iHT->getName() + "/" + iSR->getName();
      std::string mkdircommand = "mkdir -p " + fullPath;
      system( mkdircommand.c_str() );

  
      MT2Region thisRegion( (*iHT), (*iSR) );


      TH1D* h1_data = data->get(thisRegion)->yield;

      TFile* histoFile = TFile::Open( Form("%s/histograms_%s.root", fullPath.c_str(), thisRegion.getName().c_str()), "recreate" );
      histoFile->cd();
      h1_data->Write();

      TGraphAsymmErrors* gr_data = MT2DrawTools::getPoissonGraph(h1_data);
      gr_data->SetMarkerStyle(20);
      gr_data->SetMarkerSize(1.6);
  
      TCanvas* c1 = new TCanvas( "c1", "", 600, 600 );
      c1->cd();

      float xMin = h1_data->GetXaxis()->GetXmin();
      float xMax = h1_data->GetXaxis()->GetXmax();
      float yMax = h1_data->GetMaximum()*1.5;
  
      TH2D* h2_axes = new TH2D("axes", "", 10, xMin, xMax, 10, 0., yMax );
      h2_axes->SetXTitle("M_{T2} [GeV]");
      h2_axes->SetYTitle("Entries");

      h2_axes->Draw();
     
      THStack bgStack("bgStack", "");
      for( unsigned i=0; i<bgYields.size(); ++i ) {
        TH1D* h1_bg = bgYields[i]->get(thisRegion)->yield;
        h1_bg->SetFillColor( colors[i] );
        bgStack.Add(h1_bg);
      }


      std::pair<std::string, std::string> legendNames = thisRegion.getNiceNames();

      TPaveText* regionText = new TPaveText( 0.18, 0.8, 0.55, 0.9, "brNDC" );
      regionText->SetTextSize(0.035);
      regionText->SetTextFont(42);
      regionText->SetFillColor(0);
      regionText->AddText( Form("#splitline{%s}{%s}", legendNames.first.c_str(), legendNames.second.c_str()) );
      regionText->Draw("same");
      

      TLegend* legend = new TLegend( 0.6, 0.9-(bgYields.size()+1)*0.06, 0.93, 0.9 );
      legend->SetTextSize(0.038);
      legend->SetTextFont(42);
      legend->SetFillColor(0);
      if( dummyAnalysis )
        legend->AddEntry( gr_data, "Dummy", "P" );
      else
        legend->AddEntry( gr_data, "Data", "P" );
      histoFile->cd();
      for( unsigned i=0; i<bgYields.size(); ++i ) {  // reverse order in legend is prettier
        int index = bgYields.size() - i - 1;
        TH1D* h1_bg = bgYields[index]->get(thisRegion)->yield;
        legend->AddEntry( h1_bg, bgYields[index]->fullName.c_str(), "F" );
        h1_bg->Write();
      }

      histoFile->Close();

      legend->Draw("same");
      bgStack.Draw("histo same");
      gr_data->Draw("p same");

      TPaveText* labelTop = MT2DrawTools::getLabelTop();
      labelTop->Draw("same");

      gPad->RedrawAxis();

      c1->SaveAs( Form("%s/mt2_%s.eps", fullPath.c_str(), thisRegion.getName().c_str()) );
      c1->SaveAs( Form("%s/mt2_%s.png", fullPath.c_str(), thisRegion.getName().c_str()) );

      delete c1;
      delete h2_axes;

    }  // for signal regions

  } // for HT regions


  // save also MT2Analyses:
  data->writeToFile(outputdir + "/analyses.root");
  for( unsigned i=0; i<bgYields.size(); ++i )
    bgYields[i]->writeToFile(outputdir + "/analyses.root", "UPDATE");
  //std::string analysesDir = outputdir + "/analyses";
  //std::string mkdir_command2 = "mkdir -p " + analysesDir;
  //system(mkdir_command2.c_str());
  //data->writeToFile(analysesDir + "/data.root");
  //for( unsigned i=0; i<bgYields.size(); ++i )
  //  bgYields[i]->writeToFile(analysesDir + "/" + bgYields[i]->name + ".root");

  //std::cout << "-> Saved MT2Analyses to " + analysesDir << std::endl;

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

      MT2Region thisRegion( (*iHT), (*iSR) );

      h1->GetXaxis()->SetBinLabel(iBin, iSR->getName().c_str());

      MT2EstimateSyst* thisEstimate_tot = EventYield_tot->get( thisRegion );

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


MT2Config::MT2Config( const std::string& configFileName ) {

  std::cout << std::endl;
  std::cout << "-> Reading config file: " << configFileName << std::endl;
  std::cout << std::endl;

  regionsSet_ = "";
  mcSamples_ = "";
  dataSamples_ = "";
  lostLeptonTag_ = "";
  qcdTag_ = "";
  zinvTag_ = "";

  ifstream IN(configFileName.c_str());
  char buffer[200];
  char StringValue[1000];


  while( IN.getline(buffer, 200, '\n') ) {

    if (buffer[0] == '#') {
      continue; // Skip lines commented with '#'                                                                                                                                                                                 
    }

    std::cout << buffer << std::endl;

    char name_c[200];
    sscanf(buffer, "%s %s", name_c, StringValue);
    std::string name(name_c);

    if( name=="regionsSet" )
      regionsSet_ = std::string(StringValue);
    else if( name=="mcSamples" )
      mcSamples_ = std::string(StringValue);
    else if( name=="dataSamples" )
      dataSamples_ = std::string(StringValue);
    else if( name=="lostLeptonTag" )
      lostLeptonTag_ = std::string(StringValue);
    else if( name=="qcdTag" )
      qcdTag_ = std::string(StringValue);
    else if( name=="zinvTag" )
      zinvTag_ = std::string(StringValue);

  } // while getline

  if( mcSamples_=="" && lostLeptonTag_=="" && qcdTag_=="" && zinvTag_=="" ) {
    std::cout << "[MT2Config] ERROR! Config file missing BG estimates!" << std::endl;
    exit(333);
  }

  if( mcSamples_!="" && ( lostLeptonTag_!="" || qcdTag_!="" || zinvTag_!="" ) ) {
    std::cout << "[MT2Config] ERROR! Config file must have either a mcSamples line OR the lostLeptonTag/qcdTag/zinvTag lines. Not both!" << std::endl;
    exit(335);
  }

  if( mcSamples_=="" && !( lostLeptonTag_!="" || qcdTag_!="" || zinvTag_!="" ) ) {
    std::cout << "[MT2Config] ERROR! All three data-driven BG estimate tags need to be specified in the config (lostLeptonTag/qcdTag/zinvTag)!" << std::endl;
    exit(337);
  }

  std::cout << std::endl;
     
}
