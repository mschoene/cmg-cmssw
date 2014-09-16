#include <iostream>
#include <sstream>

#include "MT2tree.hh"
#include "helper/Utilities.hh"
#include "Utilities.hh"

#include <TTreeFormula.h>

#include "interface/MT2Common.h"
#include "interface/MT2Region.h"
#include "interface/MT2LostLeptonUtilities.h"



int main( int argc, char* argv[] ) {


  if( argc!=2 ) {
    std::cout << "USAGE: ./regionEventYields [samplesFileName]" << std::endl;
    std::cout << "Exiting." << std::endl;
    exit(11);
  }


  std::string sampleName(argv[1]);

  std::string samplesFileName = "samples/samples_" + sampleName + ".dat";
  std::cout << std::endl << std::endl;
  std::cout << "-> Loading samples from file: " << samplesFileName << std::endl;

  std::vector<MT2Sample> fSamples = MT2Common::loadSamples(samplesFileName);



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
  
  long int EventYield = 0;

  //Loop alll over the samples:
  for( unsigned i=0; i<fSamples.size(); ++i ){

    std::cout << std::endl << std::endl;
    std::cout << "-> Starting event yield computation for sample: " << fSamples[i].name << std::endl;

    TFile* file = TFile::Open(fSamples[i].file.c_str());
    TTree* tree = (TTree*)file->Get("MassTree");
  
    std::ostringstream preselectionStream;
    preselectionStream << " " 
		       << "(NTausIDLoose3Hits==0)"                   << " && "
		       << "(misc.Jet0Pass ==1)"                      << " && "
		       << "(misc.Jet1Pass ==1)"                      << " && "
		       << "(misc.Vectorsumpt < 70)"                  << " && " 
		       << "(misc.MinMetJetDPhi4Pt40 >0.3)"           << " && "
		       << "(misc.MET>30.)";
    //if( fFast ) preselectionStream << " && (misc.MT2>=100.) ";//lowest border in MT2

    // add "base" cuts:
    preselectionStream << " && " 
		       << "(misc.PassJet40ID ==1)"                             << " && "
		       << "((misc.HBHENoiseFlag == 0 || misc.ProcessID==10))"  << " && " // signal samples (fastsim) do not have this filter
		       << "(misc.CSCTightHaloIDFlag == 0)"                     << " && "
		       << "(misc.trackingFailureFlag==0)"                      << " && "
		       << "(misc.eeBadScFlag==0)"                              << " && "
		       << "(misc.EcalDeadCellTriggerPrimitiveFlag==0)"         << " && "
		       << "(misc.TrackingManyStripClusFlag==0)"                << " && "
		       << "(misc.TrackingTooManyStripClusFlag==0)"             << " && "
		       << "(misc.TrackingLogErrorTooManyClustersFlag==0)"      << " && "
		       << "(misc.CrazyHCAL==0)";
    preselectionStream << " && (misc.MET/misc.CaloMETRaw<=2.)";//HO cut

  

    //std::ostringstream oneLeptonStream;
    //oneLeptonStream << "((NEles==1 && NMuons==0) || (NEles==0 && NMuons==1))";
    //TString oneLeptonCuts = oneLeptonStream.str().c_str();


    TString preselection = preselectionStream.str().c_str();
    TString cuts = preselection;


    //bool requireRecoLepton = !(sample.sname=="Top" || sample.sname=="Wtolnu"); // only these used for efficiencies
    //if( requireRecoLepton ) cuts = cuts + " && " + oneLeptonCuts;
  

    TFile* tmpFile = TFile::Open("tmp.root", "recreate");
    tmpFile->cd();
    TTree* tree_reduced = tree->CopyTree(cuts);

    EventYield += tree_reduced->GetEntries();

    delete tree;
    delete tree_reduced;

    tmpFile->Close();
    delete tmpFile;

    file->Close();
    delete file;

  }
  
  cout << "Total event yield: " << EventYield << endl;

  return 0;

}
