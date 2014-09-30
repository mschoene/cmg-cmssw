#include <iostream>
#include <sstream>

#include <TTreeFormula.h>
#include <TMath.h>

#include "interface/MT2Common.h"
#include "interface/MT2Region.h"
#include "interface/MT2LostLeptonUtilities.h"

#define treeProducerSusyFullHad_cxx
#include "treeProducerSusyFullHad.h"

bool fFast = true;
//bool fbTagReweight = true;
//bool fIncludeTaus = true;



MT2LostLeptonEstimate* computeLostLepton( const MT2SampleBaby_basic& sample, std::vector<MT2HTRegion> HTRegions, std::vector<MT2SignalRegion> signalRegions );
std::vector<MT2LeptonTypeLLEstimate*> getLeptonTypeLLEstimate( std::vector<std::string> leptType, TTree* tree, MT2SampleBaby_basic sample, std::vector<MT2HTRegion> HTRegions, std::vector<MT2SignalRegion> signalRegions );
//void getBTagScaleFactor( treeProducerSusyFullHad myTree, MT2Region* region, float& btagSF, float& btagSFerr );
MT2LostLeptonEstimate* mergeEstimates( std::vector<MT2LostLeptonEstimate*> llest, const std::string& n1, const std::string& n2="", const std::string& n3="", const std::string& n4="", const std::string& n5="" );
std::vector<TH1D*> getPredictionHistos( const std::string& prefix, const std::string& leptType, std::vector<MT2HTRegion> HTRegions, std::vector<MT2SignalRegion> signalRegions, MT2LostLeptonEstimate* ll_tot, MT2LostLeptonEstimate* ll_bg, MT2LostLeptonEstimate* ll_eff );
std::vector<TH1D*> getSimTruthHistos( const std::string& prefix, const std::string& leptType, std::vector<MT2HTRegion> HTRegions, std::vector<MT2SignalRegion> signalRegions, MT2LostLeptonEstimate* ll_tot );
int getPdgId( const std::string leptType );


int main( int argc, char* argv[] ) {


  if( argc!=2 ) {
    std::cout << "USAGE: ./computeLostLepton_BabyTrees [samplesFileName]" << std::endl;
    std::cout << "Exiting." << std::endl;
    exit(11);
  }


  std::string sampleName(argv[1]);

  std::string samplesFileName = "samples/samples_" + sampleName + ".dat";
  std::cout << std::endl << std::endl;
  std::cout << "-> Loading samples from file: " << samplesFileName << std::endl;

  std::vector<MT2SampleBaby_basic> fSamples = MT2Common::loadSamplesBaby_basic(samplesFileName);



  std::ostringstream HLT_metHT;
  HLT_metHT << "( ( ("
      << "trigger.HLT_PFMET150_v2 == 1 || trigger.HLT_PFMET150_v3 == 1 || trigger.HLT_PFMET150_v4 == 1 || "
      << "trigger.HLT_PFMET150_v5 == 1 || trigger.HLT_PFMET150_v6 == 1 || trigger.HLT_PFMET150_v7 == 1 )"
      << "||("
      << "trigger.HLT_PFHT350_PFMET100_v3==1 || trigger.HLT_PFHT350_PFMET100_v4==1 || trigger.HLT_PFHT350_PFMET100_v5==1 || "
      << "trigger.HLT_PFHT350_PFMET100_v6==1 || trigger.HLT_PFHT350_PFMET100_v7==1 || trigger.HLT_PFNoPUHT350_PFMET100_v1==1 || "
      << "trigger.HLT_PFNoPUHT350_PFMET100_v3==1 || trigger.HLT_PFNoPUHT350_PFMET100_v4==1 ) ) &&TOBTECTagger<=8&&ExtraBeamHaloFilter==0)";

  std::ostringstream HLT_HT;
  HLT_HT << "( ("
      << "trigger.HLT_PFHT650_v5 == 1 || trigger.HLT_PFHT650_v6 == 1 || trigger.HLT_PFHT650_v7 == 1 || "
      << "trigger.HLT_PFHT650_v8 == 1 || trigger.HLT_PFHT650_v9 == 1 || "
      << "trigger.HLT_PFNoPUHT650_v1 == 1 || trigger.HLT_PFNoPUHT650_v3 == 1 || trigger.HLT_PFNoPUHT650_v4 == 1) &&TOBTECTagger<=8&&ExtraBeamHaloFilter==0)";



  std::vector<MT2HTRegion> HTRegions;
  HTRegions.push_back(MT2HTRegion("lowHT",    450.,    750., 200., HLT_metHT.str()));
  HTRegions.push_back(MT2HTRegion("mediumHT", 750.,   1200.,  30., HLT_HT.str()));
  HTRegions.push_back(MT2HTRegion("highHT",  1200., 100000.,  30., HLT_HT.str()));

  std::vector<MT2SignalRegion> signalRegions;
  signalRegions.push_back(MT2SignalRegion(2, 2, 0, 0));  // 2j0b
  signalRegions.push_back(MT2SignalRegion(2, 2, 1, 2));  // 2j1to2b
  signalRegions.push_back(MT2SignalRegion(3, 5, 0, 0));  // 3to5j0b
  signalRegions.push_back(MT2SignalRegion(3, 5, 1, 1));  // 3to5j1b
  signalRegions.push_back(MT2SignalRegion(3, 5, 2, 2));  // 3to5j2b
  signalRegions.push_back(MT2SignalRegion(6, -1, 0, 0));  // 6j0b
  signalRegions.push_back(MT2SignalRegion(6, -1, 1, 1));  // 6j1b
  signalRegions.push_back(MT2SignalRegion(6, -1, 2, 2));  // 6j2b
  signalRegions.push_back(MT2SignalRegion(-1, -1, 3, -1));  // 3b


  TH1::AddDirectory(kFALSE); // stupid ROOT memory allocation needs this

  
  std::vector<MT2LostLeptonEstimate*> llest;
  for( unsigned i=0; i<fSamples.size(); ++i )
    llest.push_back( computeLostLepton( fSamples[i], HTRegions, signalRegions ) );
  

  system( "rm tmp.root" );

  std::cout << "-> Done looping on samples. Start merging." << std::endl;

  MT2LostLeptonEstimate* ll_data  = mergeEstimates( llest, "HT-Data", "MET-Data" );
  //MT2LostLeptonEstimate* ll_data  = mergeEstimates( llest, "HT-Data", "MET-Data" );
  MT2LostLeptonEstimate* ll_top   = mergeEstimates( llest, "Top" );
  MT2LostLeptonEstimate* ll_qcd   = mergeEstimates( llest, "QCD" );
  MT2LostLeptonEstimate* ll_wjets = mergeEstimates( llest, "Wtolnu" );
  MT2LostLeptonEstimate* ll_other = mergeEstimates( llest, "DY", "VV" );
  
  MT2LostLeptonEstimate* ll_topW   = new MT2LostLeptonEstimate(*ll_top  + *ll_wjets);
  MT2LostLeptonEstimate* ll_bg     = new MT2LostLeptonEstimate(*ll_qcd  + *ll_other);
  MT2LostLeptonEstimate* ll_allMC  = new MT2LostLeptonEstimate(*ll_topW + *ll_bg);

  std::cout << "-> Done merging. Start computing preditions." << std::endl;


  std::vector<TH1D*> vh1_data_ele = getPredictionHistos( "Prediction", "Ele", HTRegions, signalRegions, ll_data, ll_bg, ll_topW );
  std::vector<TH1D*> vh1_data_mu  = getPredictionHistos( "Prediction", "Muo", HTRegions, signalRegions, ll_data, ll_bg, ll_topW );

  std::vector<TH1D*> vh1_mc_ele   = getPredictionHistos( "PredictionMC", "Ele", HTRegions, signalRegions, ll_allMC, ll_bg, ll_topW );
  std::vector<TH1D*> vh1_mc_mu    = getPredictionHistos( "PredictionMC", "Muo", HTRegions, signalRegions, ll_allMC, ll_bg, ll_topW );

  std::vector<TH1D*> vh1_sim_ele = getSimTruthHistos( "SimulationTruth", "Ele", HTRegions, signalRegions, ll_topW );
  std::vector<TH1D*> vh1_sim_mu  = getSimTruthHistos( "SimulationTruth", "Muo", HTRegions, signalRegions, ll_topW );



  std::string outputdir = "LostLeptonFiles";
  system(Form("mkdir -p %s", outputdir.c_str()));

  
  TFile* outfile = TFile::Open(Form("%s/LostLepton_%s.root", outputdir.c_str(), sampleName.c_str()), "recreate");
  outfile->cd();

  for( unsigned i=0; i<HTRegions.size(); ++i ) {

    vh1_data_ele[i]->Write();
    vh1_data_mu [i]->Write();

    vh1_mc_ele  [i]->Write();
    vh1_mc_mu   [i]->Write();

    vh1_sim_ele [i]->Write();
    vh1_sim_mu  [i]->Write();

  }


  outfile->Close();

  std::cout << "-> Done computing preditions. Written to file: " << outfile->GetName() << std::endl;

  return 0;

}




MT2LostLeptonEstimate* computeLostLepton( const MT2SampleBaby_basic& sample, std::vector<MT2HTRegion> HTRegions, std::vector<MT2SignalRegion> signalRegions ) {


  std::cout << std::endl << std::endl;
  std::cout << "-> Starting computation for sample: " << sample.name << std::endl;

  TFile* file = TFile::Open(sample.file.c_str());
  TTree* tree = (TTree*)file->Get("treeProducerSusyFullHad");
  
  treeProducerSusyFullHad myTree;
  myTree.Init(tree);


  std::ostringstream preselectionStream;
  preselectionStream << " " 
    //<< "(nTaus20==0 && nMuons10==0 && nElectrons10==0)" << " && "
                     << "(nVert > 0)"                      << " && "
                     << "(nJet40 > 1)"                     << " && "
                     << "(jet_pt[1] > 100)"                << " && "
                     << "(ht > 450)"                       << " && "
                     << "((ht < 750 && met_pt > 200) || (ht > 750 && met_pt > 30))" << " && "
                     << "(deltaPhiMin > 0.3)"              << " && " 
                     << "(diffMetMht < 70)"                << " && "
                     << "(mt2 > 50)";
  if( fFast ) preselectionStream << " && (mt2 >= 100.) "; //lowest border in MT2

  //// add "base" cuts:
  //preselectionStream << " && " 
  //  << "(misc.PassJet40ID ==1)"                             << " && "
  //  << "((misc.HBHENoiseFlag == 0 || misc.ProcessID==10))"  << " && " // signal samples (fastsim) do not have this filter
  //  << "(misc.CSCTightHaloIDFlag == 0)"                     << " && "
  //  << "(misc.trackingFailureFlag==0)"                      << " && "
  //  << "(misc.eeBadScFlag==0)"                              << " && "
  //  << "(misc.EcalDeadCellTriggerPrimitiveFlag==0)"         << " && "
  //  << "(misc.TrackingManyStripClusFlag==0)"                << " && "
  //  << "(misc.TrackingTooManyStripClusFlag==0)"             << " && "
  //  << "(misc.TrackingLogErrorTooManyClustersFlag==0)"      << " && "
  //  << "(misc.CrazyHCAL==0)";
  //preselectionStream << " && (misc.MET/misc.CaloMETRaw<=2.)";//HO cut

  

  std::ostringstream oneLeptonStream;
  oneLeptonStream << "((nElectrons10==1 && nMuons10==0) || (nElectrons10==0 && nMuons10==1))";
  TString oneLeptonCuts = oneLeptonStream.str().c_str();




  TString preselection = preselectionStream.str().c_str();
  TString cuts = preselection;


  bool requireRecoLepton = !(sample.sname=="Top" || sample.sname=="WJets"); // only these used for efficiencies
  if( requireRecoLepton ) cuts = cuts + " && " + oneLeptonCuts;
  

  TFile* tmpFile = TFile::Open("tmp.root", "recreate");
  tmpFile->cd();
  TTree* tree_reduced = tree->CopyTree(cuts);


  std::vector<std::string> leptType;
  leptType.push_back("Ele");
  leptType.push_back("Muo");

  std::vector<MT2LeptonTypeLLEstimate*> v_llest = getLeptonTypeLLEstimate( leptType, tree_reduced, sample, HTRegions, signalRegions );

  MT2LostLeptonEstimate* llest = new MT2LostLeptonEstimate(sample.name, sample.sname);
  for( unsigned i=0; i<leptType.size(); ++i ) 
    llest->l[ leptType[i].c_str() ] = v_llest[i];

  delete tree;
  delete tree_reduced;

  tmpFile->Close();
  delete tmpFile;

  file->Close();
  delete file;
  

  return llest;

}




std::vector<MT2LeptonTypeLLEstimate*> getLeptonTypeLLEstimate( std::vector<std::string> leptType, TTree* tree, MT2SampleBaby_basic sample, std::vector<MT2HTRegion> HTRegions, std::vector<MT2SignalRegion> signalRegions ) {


  bool isData = (sample.sname=="data");

  // global sample weight:
  Double_t weight = sample.xsection * sample.kfact * sample.lumi / (sample.nevents*sample.PU_avg_weight);


  // return estimate:
  std::vector<int> pdgIdLept;
  std::vector<MT2LeptonTypeLLEstimate*> llest;
  for( unsigned i=0; i<leptType.size(); ++i ) {
    std::string fullName = leptType[i] + "_" + sample.name;
    MT2LeptonTypeLLEstimate* this_llest = new MT2LeptonTypeLLEstimate( fullName, sample.sname, HTRegions, signalRegions );
    llest.push_back( this_llest );
    pdgIdLept.push_back( getPdgId(leptType[i]) );
  }

  // instantiate HLT TTreeFormulas
  std::vector<TTreeFormula*> ttf_hlt;
  if( isData ) {
    for( unsigned iHT=0; iHT<HTRegions.size(); ++iHT ) {
      TTreeFormula* this_ttf = new TTreeFormula(Form("ttf_hlt_%s", HTRegions[iHT].getName().c_str()), HTRegions[iHT].HLT_selection.c_str(), tree);
      ttf_hlt.push_back(this_ttf);
    }
  }


  //MT2tree* fMT2tree = new MT2tree();
  //tree->SetBranchAddress("MT2tree", &fMT2tree);

  treeProducerSusyFullHad myTree;
  myTree.Init(tree);


  int nentries = tree->GetEntries();

  float fullweight = weight;
  float fullweight_btagUp = weight;
  float fullweight_btagDown = weight;


  for( unsigned iEntry=0; iEntry<nentries; ++iEntry ) {

    if( iEntry % 50000 == 0 ) std::cout << "    Entry: " << iEntry << " / " << nentries << std::endl;

    myTree.GetEntry(iEntry);

    //if(myTree.ht>=750.&&myTree.ht<1200.&&myTree.nJet40==2&&myTree.nBJet40>=1&&myTree.mt2>=135.&&myTree.mt2<=170.&&sample.sname=="Top"&&myTree.lumi149&&myTree.event==44699) continue;//sample T_t_highHT - fix for one 8 TeV MC sample with crazy weight

    float ht   = myTree.ht;
    float met  = myTree.met_pt;
    float mt2  = myTree.mt2;
    int njets  = myTree.nJet40;
    int nbjets = myTree.nBJet40;
    
    fullweight = weight;
    fullweight_btagUp = weight;
    fullweight_btagDown = weight;

    //StartFromHere


    //int ngenleptot = fMT2tree->GenNumLeptFromW(1113,0,1000, fIncludeTaus);
    int ngenleptot = 0;
    for (int gl=0; gl < myTree.nGenLep; ++gl)
      if(fabs(myTree.GenLep_sourceId[gl]) == 24) ++ngenleptot;
    for (int gl=0; gl < myTree.nGenLepFromTau; ++gl)
      if(fabs(myTree.GenLepFromTau_sourceId[gl]) == 24) ++ngenleptot;

    bool foundRegion = false;

    for( unsigned iHT=0; iHT<HTRegions.size() && !(foundRegion); ++iHT ) {

      if( ht<HTRegions[iHT].htMin || ht>=HTRegions[iHT].htMax || met<HTRegions[iHT].metMin ) continue;

      if( isData ) { // apply HLT
        if( ttf_hlt[iHT]->EvalInstance()==0. ) continue;
      }


      for( unsigned iSR=0; iSR<signalRegions.size() && !(foundRegion); ++iSR ) {

        if( signalRegions[iSR].nJetsMin  >= 0  &&  njets  < signalRegions[iSR].nJetsMin ) continue;
        if( signalRegions[iSR].nJetsMax  >= 0  &&  njets  > signalRegions[iSR].nJetsMax ) continue;
        if( signalRegions[iSR].nBJetsMin >= 0  &&  nbjets < signalRegions[iSR].nBJetsMin ) continue;
        if( signalRegions[iSR].nBJetsMax >= 0  &&  nbjets > signalRegions[iSR].nBJetsMax ) continue;


        // found region

        MT2Region thisRegion( &HTRegions[iHT], &signalRegions[iSR] );


        // compute weight
        if( !isData ) {

          float puweight =  myTree.puWeight;

	  ////// FIXME
	  //float btagSF, btagSFerr;
          //getBTagScaleFactor( myTree, &thisRegion, btagSF, btagSFerr );
	  float btagSF = 1., btagSFerr = 0.;
	  //////

          fullweight          = weight * puweight * btagSF;
          fullweight_btagUp   = weight * puweight * (btagSF + btagSFerr);
          fullweight_btagDown = weight * puweight * (btagSF - btagSFerr);

        }


        for( unsigned iLept=0; iLept<leptType.size(); ++iLept ) {

          float mt = 0.;
          bool hasRecoLep = false;
          if( leptType[iLept]=="Ele" ) {
            hasRecoLep = (myTree.nElectrons10==1 && myTree.nMuons10==0);
	    mt = (hasRecoLep) ? sqrt(myTree.lep_mass[0]*myTree.lep_mass[0] + myTree.lep_pt[0]*myTree.lep_pt[0]) : 0.;
          } else if( leptType[iLept]=="Muo" ) {
            hasRecoLep = (myTree.nElectrons10==0 && myTree.nMuons10==1);
	    mt = (hasRecoLep) ? sqrt(myTree.lep_mass[0]*myTree.lep_mass[0] + myTree.lep_pt[0]*myTree.lep_pt[0]) : 0.;
          }


          MT2SingleLLEstimate*     thisPred     = llest[iLept]->getRegion   ( thisRegion.getName() );
          MT2SingleLLEstimateBase* thisSimTruth = llest[iLept]->getRegionGen( thisRegion.getName() );


          if( hasRecoLep ) {

            thisPred->effMT_tot->Fill( mt2, fullweight );


            if( mt<100. ) {

              thisPred->effMT_pass->Fill( mt2, fullweight );

              thisPred->yield->Fill( mt2, fullweight );
              thisPred->yield_btagUp->Fill( mt2, fullweight_btagUp );
              thisPred->yield_btagDown->Fill( mt2, fullweight_btagDown );

            }

          } 

          //bool genlept = fMT2tree->GenLeptFromW(pdgIdLept[iLept], 0 , 1000,fIncludeTaus);
	  //bool hasGenLept = (genlept) && (ngenleptot==1);
	  
	  bool hasGenLept = (ngenleptot==1);

          if( hasGenLept ) {
            thisPred->effLept_tot->Fill( mt2, fullweight );
            if( hasRecoLep ) {
              thisPred->effLept_pass->Fill( mt2, fullweight );
            }
          }
 
          bool noRecoLep = (myTree.nElectrons10==0 && myTree.nMuons10==0);
          //if( genlept && noRecoLep ) thisSimTruth->yield->Fill( mt2, fullweight );
	  if( ngenleptot==1 && noRecoLep ) thisSimTruth->yield->Fill( mt2, fullweight );

        } // for leptType


        foundRegion = true; // found region, move on

      }  // for iSR

    }  // for iHT

  } // for entries


  for( unsigned i=0; i<llest.size(); ++i )
    llest[i]->addOverflow();


  delete tree;
  
  return llest;

}





MT2LostLeptonEstimate* mergeEstimates( std::vector<MT2LostLeptonEstimate*> llest, const std::string& n1, const std::string& n2, const std::string& n3, const std::string& n4, const std::string& n5 ) {

  std::vector<std::string> snames;
  snames.push_back(n1);
  snames.push_back(n2);
  snames.push_back(n3);
  snames.push_back(n4);
  snames.push_back(n5);

  std::string newname = (snames.size()>0) ? (snames[0]) : "";
  for( unsigned i=1; i<snames.size(); ++i ) newname += "_" + snames[i];
  newname = "merge_" + newname;

  MT2LostLeptonEstimate* return_llest = new MT2LostLeptonEstimate(newname);

  for( unsigned i=0; i<llest.size(); ++i ) {

    for( unsigned iname=0; iname<snames.size(); ++iname ) {

      if( llest[i]->SName == snames[iname] ) {
        return_llest->add(*(llest[i]));
      }

    } // for snames

  } // for llest


  return return_llest;

}








std::vector<TH1D*> getPredictionHistos( const std::string& prefix, const std::string& leptType, std::vector<MT2HTRegion> HTRegions, std::vector<MT2SignalRegion> signalRegions, MT2LostLeptonEstimate* ll_tot, MT2LostLeptonEstimate* ll_bg, MT2LostLeptonEstimate* ll_eff ) {


  int nHistos = HTRegions.size();
  int nBins = signalRegions.size();


  
  std::vector<TH1D*> histos;

  for( unsigned i=0; i<nHistos; ++i ) {

    TH1D* h1 = new TH1D(Form("%s_%s_%s", prefix.c_str(), leptType.c_str(), HTRegions[i].name.c_str()), "", nBins, 0., nBins);
    h1->Sumw2();

    for( unsigned j=0; j<nBins; ++j ) {
  
      int iBin = j+1;

      MT2Region thisRegion( &HTRegions[i], &signalRegions[j] );

      h1->GetXaxis()->SetBinLabel(iBin, signalRegions[j].getName().c_str());

      float tot = (ll_tot!=0 && ll_tot->l[leptType.c_str()]!=0) ? ll_tot->l[leptType.c_str()]->getRegion(thisRegion.getName())->yield->Integral() : 0.;
      float  bg = (ll_bg !=0 && ll_bg ->l[leptType.c_str()]!=0) ? ll_bg ->l[leptType.c_str()]->getRegion(thisRegion.getName())->yield->Integral() : 0.;
      float  bg_btagUp = (ll_bg !=0 && ll_bg ->l[leptType.c_str()]!=0) ? ll_bg ->l[leptType.c_str()]->getRegion(thisRegion.getName())->yield_btagUp->Integral() : 0.;
      float  bg_btagDown = (ll_bg !=0 && ll_bg ->l[leptType.c_str()]!=0) ? ll_bg ->l[leptType.c_str()]->getRegion(thisRegion.getName())->yield_btagDown->Integral() : 0.;

      float  effMT      = (ll_eff!=0 && ll_eff->l[leptType.c_str()]!=0) ? ll_eff->l[leptType.c_str()]->getRegion(thisRegion.getName())->effMT()  ->GetEfficiency(1) : 0.;
      float  effLept    = (ll_eff!=0 && ll_eff->l[leptType.c_str()]!=0) ? ll_eff->l[leptType.c_str()]->getRegion(thisRegion.getName())->effLept()->GetEfficiency(1) : 0.;

      float effMT_err   = (ll_eff!=0 && ll_eff->l[leptType.c_str()]!=0) ? ll_eff->l[leptType.c_str()]->getRegion(thisRegion.getName())->effMT()  ->GetEfficiencyErrorLow(1) : 0.; 
      float effLept_err = (ll_eff!=0 && ll_eff->l[leptType.c_str()]!=0) ? ll_eff->l[leptType.c_str()]->getRegion(thisRegion.getName())->effLept()->GetEfficiencyErrorLow(1) : 0.;

      float pred = (effLept>0. && effMT>0.) ? (tot-bg)*(1.-effLept)/(effLept*effMT) : (tot-bg);

      h1->SetBinContent( iBin, pred );

std::cout << "tot: " << tot << std::endl;
std::cout << "bg: " << bg << std::endl;
std::cout << "effMT: " << effMT << std::endl;
std::cout << "effLept: " << effLept << std::endl;
std::cout << "pred: " << pred << std::endl;

      float statErr = sqrt(tot)*(1.-effLept)/(effLept*effMT);
      float sysErr_deffLept = (tot-bg)*effLept_err/(effLept*effLept*effMT);
      float sysErr_deffMT = (tot-bg)*effMT_err*(1.-effLept)/(effLept*effMT*effMT);
      float sysErr_dbg = 0.5*bg*(1.-effLept)/(effLept*effMT); // 50% uncert on BG estimate
      float sysErr_MT = 0.05*(1.-effLept)/(effLept*effMT*effMT); // 5% uncert on MT cut
      float bg_SFerrUp = fabs(bg_btagUp-bg);
      float bg_SFerrDown = fabs(bg_btagDown-bg);
      float bg_SFerr = 0.5*(bg_SFerrUp+bg_SFerrDown);
      float sysErr_btag = bg_SFerr*(1-effLept)/(effLept*effMT);

      //float sysErr_dLL = nWT_goodrecoevt_dLL*(1.*effLept_err/(effLept*effLept*effMT));//additional error due to double lost with 100% uncert

      float sysErr = sqrt( sysErr_deffLept*sysErr_deffLept + sysErr_deffMT*sysErr_deffMT + sysErr_dbg*sysErr_dbg + sysErr_MT*sysErr_MT + sysErr_btag*sysErr_btag );
std::cout << "sysErr_deffLept: " << sysErr_deffLept << std::endl;
std::cout << "sysErr_deffMT: " << sysErr_deffMT << std::endl;
std::cout << "sysErr_dbg: " << sysErr_dbg << std::endl;
std::cout << "sysErr_MT: " << sysErr_MT << std::endl;
std::cout << "sysErr_btag: " << sysErr_btag << std::endl;
std::cout << "sysErr: " << sysErr<< std::endl;
      //if(mtefferr==1) pred_error_sys  = sqrt(  pow((nData-bg) *(prob_nomt_err_sys/(prob_nomt*prob_nomt*mteff)),2) + pow(rel_sys_uncert_bg*bg*(1.-prob_nomt)/(prob_nomt*mteff),2));
      //if(fbTagError) pred_error_sys = sqrt( pow(pred_error_sys,2) + pow(bg_SFerr *(1-prob_nomt)/(prob_nomt*mteff),2) );
      //pred_error_sys = sqrt(pred_error_sys*pred_error_sys +    pow((frelMTerr*(1.-prob_nomt)/(prob_nomt*mteff*mteff)),2) );//addtional MT cut error
      //pred_error_sys = sqrt(pred_error_sys*pred_error_sys + pow(nWT_goodrecoevt_dLL*(fdoubeLLerr*prob_nomt_err_sys/(prob_nomt*prob_nomt*mteff)),2) );//additional error due to float lost lepton

      h1->SetBinError( iBin, sqrt(statErr*statErr + sysErr*sysErr) );

    }

    histos.push_back(h1);

  } // for

  return histos;

}





std::vector<TH1D*> getSimTruthHistos( const std::string& prefix, const std::string& leptType, std::vector<MT2HTRegion> HTRegions, std::vector<MT2SignalRegion> signalRegions, MT2LostLeptonEstimate* ll_tot ) {


  int nHistos = HTRegions.size();
  int nBins = signalRegions.size();


  
  std::vector<TH1D*> histos;

  for( unsigned i=0; i<nHistos; ++i ) {

    TH1D* h1 = new TH1D(Form("%s_%s_%s", prefix.c_str(), leptType.c_str(), HTRegions[i].name.c_str()), "", nBins, 0., nBins);
    h1->Sumw2();

    for( unsigned j=0; j<nBins; ++j ) {
  
      int iBin = j+1;

      MT2Region thisRegion( &HTRegions[i], &signalRegions[j] );

      h1->GetXaxis()->SetBinLabel(iBin, signalRegions[j].getName().c_str());

      float totYield = (ll_tot!=0 && ll_tot->l[leptType.c_str()]!=0) ? ll_tot->l[leptType.c_str()]->getRegionGen(thisRegion.getName())->yield->Integral() : 0.;

      h1->SetBinContent( iBin, totYield );

    }

    histos.push_back(h1);

  } // for

  return histos;

}




//void getBTagScaleFactor( treeProducerSusyFullHad myTree, MT2Region* region, float& btagSF, float& btagSFerr ) {
//
//  int nbjetsmin = region->nBJetsMin();
//  int nbjetsmax = region->nBJetsMax();
//
//  if( nbjetsmin != nbjetsmax ) {  // use "ge"
//
//    if( nbjetsmin >= 3 ) {
//
//      btagSF = fMT2tree->SFWeight.BTagCSV40ge3;
//      btagSFerr = fMT2tree->SFWeight.BTagCSV40ge3Error;
//
//    } else if( nbjetsmin == 2 ) {
//
//      btagSF = fMT2tree->SFWeight.BTagCSV40ge2;
//      btagSFerr = fMT2tree->SFWeight.BTagCSV40ge2Error;
//
//    } else if( nbjetsmin == 1 ) {
//
//      btagSF = fMT2tree->SFWeight.BTagCSV40ge1;
//      btagSFerr = fMT2tree->SFWeight.BTagCSV40ge1Error;
//
//    } else { // use "eq"
//
//      btagSF = 1.;
//      btagSFerr = 0.;
//
//    }
//
//  } else { // if nbjetsmin==nbjetsmax
//
//    if( nbjetsmin >= 3 ) {
//
//      btagSF = fMT2tree->SFWeight.BTagCSV40eq3;
//      btagSFerr = fMT2tree->SFWeight.BTagCSV40eq3Error;
//
//    } else if( nbjetsmin == 2 ) {
//
//      btagSF = fMT2tree->SFWeight.BTagCSV40eq2;
//      btagSFerr = fMT2tree->SFWeight.BTagCSV40eq2Error;
//
//    } else if( nbjetsmin == 1 ) {
//
//      btagSF = fMT2tree->SFWeight.BTagCSV40eq1;
//      btagSFerr = fMT2tree->SFWeight.BTagCSV40eq1Error;
//
//    } else {
//
//      btagSF = 1.;
//      btagSFerr = 0.;
//
//    }
//
//  }
//
//
//}



int getPdgId( const std::string leptType ) {

  int pdgIdLept = -999;
  if(leptType=="Ele") {
    pdgIdLept = 11;
  } else if(leptType=="Muo") {
    pdgIdLept = 13;
  } else {
    std::cout << "Unkown lepton type: " << leptType << "! Exiting." << std::endl;
    exit(111);
  }

  return pdgIdLept;

}
