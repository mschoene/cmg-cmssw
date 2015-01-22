#include <fstream>
#include <cmath>

#include "TFile.h"
#include "TH1D.h"
#include "TList.h"
#include "TObject.h"

#include "interface/MT2Analysis.h"
#include "interface/MT2Estimate.h"




void writeToTemplateFile( TFile* file, MT2Analysis<MT2Estimate>* analysis, float err_uncorr );
void writeToTemplateFile_poisson( TFile* file, MT2Analysis<MT2Estimate>* analysis, const std::string& name="stat" );
MT2Analysis<MT2Estimate>* get( const std::string& name, std::vector< MT2Analysis<MT2Estimate>* > analyses, const std::string& name1, const std::string& name2="", const std::string& name3="", const std::string& name4="" );



int main( int argc, char* argv[] ) {


  if( argc!=2 ) {
    std::cout << "USAGE: ./createDatacards [dir]" << std::endl;
    exit(113);
  } 


  std::string dir( argv[1] );
  std::string mc_fileName = dir + "/analyses.root";


  bool useMC_qcd  = true;
  bool useMC_zinv = false;
  bool useMC_llep = true;

  float err_qcd_corr    = 0.0;
  float err_qcd_uncorr  = 1.0; // 100% of QCD MC yield
  float err_llep_corr   = 0.;
  float err_llep_uncorr = 0.075;
  float err_zinv_corr   = 0.2; // 20% on Z/gamma ratio
  float err_zinv_uncorr = -1.; // will take histogram bin error
  float err_sig_corr    = 0.1;
  float err_sig_uncorr  = 0.;


  MT2Analysis<MT2Estimate>* data  = MT2Analysis<MT2Estimate>::readFromFile( mc_fileName, "data" );
  MT2Analysis<MT2Estimate>* qcd;
  if( useMC_qcd )
    qcd = MT2Analysis<MT2Estimate>::readFromFile( mc_fileName, "QCD"  );
  else
    qcd = MT2Analysis<MT2Estimate>::readFromFile( "MT2QCDEstimate.root" );
  qcd->setName("qcd");

  //////CHANGE HERE for ITERATION 1
  //MT2Analysis<MT2Estimate>* zinv  = MT2Analysis<MT2Estimate>::readFromFile( mc_fileName, "ZJets");
  //zinv->setName("zinv");
  //MT2Analysis<MT2Estimate>* wjets = MT2Analysis<MT2Estimate>::readFromFile( mc_fileName, "WJets");
  //MT2Analysis<MT2Estimate>* top   = MT2Analysis<MT2Estimate>::readFromFile( mc_fileName, "Top"  );
  //MT2Analysis<MT2Estimate>* llep = new MT2Analysis<MT2Estimate>( *top + *wjets );
  //llep->setName( "llep" );
  ////*llep += *wjets;
  
  MT2Analysis<MT2Estimate>* zinv;
  if( useMC_zinv )
    zinv = MT2Analysis<MT2Estimate>::readFromFile( mc_fileName, "ZJets");
  else
    zinv = MT2Analysis<MT2Estimate>::readFromFile( "ZinvEstimateFromGamma_CSA14_Zinv_13TeV_CSA14/MT2ZinvEstimate.root", "ZinvEstimate");
  zinv->setName("zinv");
  zinv->addToFile( mc_fileName, true );


  MT2Analysis<MT2Estimate>* llep;
  if( useMC_llep ) {
    MT2Analysis<MT2Estimate>* wjets = MT2Analysis<MT2Estimate>::readFromFile( mc_fileName, "WJets");
    MT2Analysis<MT2Estimate>* top   = MT2Analysis<MT2Estimate>::readFromFile( mc_fileName, "Top");
    llep = new MT2Analysis<MT2Estimate>( (*wjets) + (*top) );
  } else {
    llep = MT2Analysis<MT2Estimate>::readFromFile( "MT2LostLeptEstimate.root" );
  }
  llep->setName( "llep" );
  llep->addToFile( mc_fileName, true );




  std::cout << std::endl << std::endl;
  std::cout << "-> Creating BG templates file..." << std::endl;
  TFile* file_templateBG = TFile::Open(Form("%s/bkg_templates.root", dir.c_str()), "recreate");
  writeToTemplateFile( file_templateBG,  qcd, err_qcd_uncorr  );
  writeToTemplateFile( file_templateBG, llep, err_llep_uncorr );
  writeToTemplateFile( file_templateBG, zinv, err_zinv_uncorr );
  //writeToTemplateFile_poisson( file_templateBG, zinv );
  file_templateBG->Close();
  std::cout << "-> Created BG templates file: " << file_templateBG->GetName() << std::endl;



  std::cout << std::endl << std::endl;
  std::cout << "-> Creating signal templates file..." << std::endl;
  std::vector<MT2Analysis<MT2Estimate>*> signals = MT2Analysis<MT2Estimate>::readAllFromFile( mc_fileName, "SMS" );
  TFile* file_templateSignal = TFile::Open(Form("%s/sig_templates.root", dir.c_str()), "recreate");
  for( unsigned i=0; i<signals.size(); ++i ) 
    writeToTemplateFile( file_templateSignal, signals[i], err_sig_uncorr );
  file_templateSignal->Close();
  std::cout << "-> Created signal templates file: " << file_templateSignal->GetName() << std::endl;

  std::cout << std::endl << std::endl;
  std::cout << "-> Creating data_obs file..." << std::endl;
  TFile* file_data_obs = TFile::Open(Form("%s/data_obs.root", dir.c_str()), "recreate");
  data->setName("data_obs");
  writeToTemplateFile( file_data_obs, data, 0. );
  file_data_obs->Close();
  std::cout << "-> Created data_obs file: " << file_data_obs->GetName() << std::endl;






  std::set<MT2HTRegion> htRegions = data->getHTRegions();
  std::set<MT2SignalRegion> sigRegions = data->getSignalRegions();

  for( int isig=-1; isig<(int)signals.size(); ++isig ) { // loop on signals (first iteration with isig=-1 will create the templates)

    std::string path;
    if( isig==-1 )
      path = dir; // these are the templates
    else {
      path = dir + "/datacards_" + signals[isig]->getName();
      system(Form("mkdir -p %s", path.c_str()));
      std::cout << "-> Creating datacards in: " << path << std::endl;
    }
  
    for( std::set<MT2HTRegion>::iterator iHT=htRegions.begin(); iHT!=htRegions.end(); ++iHT ) {
      for( std::set<MT2SignalRegion>::iterator iSR=sigRegions.begin(); iSR!=sigRegions.end(); ++iSR ) {

       MT2Region thisRegion(*iHT, *iSR);


       TH1D* this_data = data->get(thisRegion)->yield;
       TH1D* this_qcd  = qcd ->get(thisRegion)->yield;
       TH1D* this_zinv = zinv->get(thisRegion)->yield;
       TH1D* this_llep = llep->get(thisRegion)->yield;

       TH1D* this_signal = 0;
       if( isig>-1 ) this_signal = signals[isig]->get(thisRegion)->yield;


       std::string datacardName( path + "/datacard_" + thisRegion.getName() + ".txt");
       ofstream datacard( datacardName.c_str() );


       datacard << "imax 1" << std::endl;
       datacard << "jmax 3" << std::endl;
       datacard << "kmax *" << std::endl;
       datacard << "-------------" << std::endl;
       datacard << std::endl << std::endl;
       
       datacard << "shapes ";
       if( this_signal!=0 )
         datacard << signals[isig]->getName();
       else
         datacard << "sig";
       datacard  << " " << thisRegion.getName() << " " << dir << "/sig_templates.root yield_$PROCESS_$CHANNEL $SYSTEMATIC" << std::endl;
       datacard << "shapes qcd "      << thisRegion.getName() << " " << dir << "/bkg_templates.root yield_$PROCESS_$CHANNEL $SYSTEMATIC" << std::endl;
       datacard << "shapes zinv "     << thisRegion.getName() << " " << dir << "/bkg_templates.root yield_$PROCESS_$CHANNEL $SYSTEMATIC" << std::endl;
       datacard << "shapes llep "     << thisRegion.getName() << " " << dir << "/bkg_templates.root yield_$PROCESS_$CHANNEL $SYSTEMATIC" << std::endl;
       datacard << "shapes data_obs " << thisRegion.getName() << " " << dir << "/data_obs.root yield_$PROCESS_$CHANNEL" << std::endl;

       datacard << "-------------" << std::endl;


       datacard << std::endl << std::endl;
       datacard << "bin  " << thisRegion.getName() << std::endl;
       datacard << "observation  " << this_data->Integral(1,this_data->GetXaxis()->GetNbins()) << std::endl;
       datacard << "-------------" << std::endl;
       datacard << std::endl << std::endl;

       // sig qcd zinv llep
       datacard << "bin \t" << thisRegion.getName() << "\t" << thisRegion.getName() << "\t" << thisRegion.getName() << "\t" << thisRegion.getName() << std::endl;
       datacard << "process \t ";
       if( this_signal!=0 )
         datacard << signals[isig]->getName();
       else
         datacard << "sig ";
       datacard << " \t qcd \t zinv \t llep" << std::endl;
       datacard << "process \t 0 \t 1 \t 2 \t 3" << std::endl;
       datacard << "rate \t ";
       if( this_signal!=0 )
         datacard << this_signal->Integral();
       else
         datacard << "XXX";
       datacard << " \t " << this_qcd->Integral() << " \t " << this_zinv->Integral() << " \t " << this_llep->Integral() << std::endl;
       datacard << "-------------" << std::endl;

       datacard << "syst_sig    lnN    " << 1.+err_sig_corr << " - - -" << std::endl;

       int N_llep = (int)this_llep->Integral();
       float llep_stat_err = (N_llep>0) ? 1./sqrt((float)N_llep) : 0.;
       float llep_tot_err = sqrt( llep_stat_err*llep_stat_err + 0.15*0.15 );
       llep_tot_err+=1.;
       datacard << "syst_llep_corr_" << thisRegion.getName() << "  lnN   - - - " << llep_tot_err << std::endl;





       // QCD SYSTEMATICS:

       if( this_qcd->Integral()>0. ) {
         for( unsigned i=1; i<this_qcd->GetNbinsX()+1; ++i ) {
           if( this_qcd->GetBinContent(i)==0. ) continue;
           datacard << this_qcd->GetName() << "_bin_" << i << " shapeN2 - 1 - -" << std::endl;
         }
       }




       // Z INVISIBLE SYSTEMATICS:

       if( this_zinv->Integral()>0. ) {

         if( thisRegion.nBJetsMin()<2 ) { // 0 and 1 btag

           // correlated:
           datacard << "syst_zinv_corr lnN \t - - " << 1.+err_zinv_corr << " -" << std::endl;
           //if( thisRegion.nBJetsMin()>0 ) {
           //  datacard << "syst_zinv_Z1b_" << thisRegion.getName() << " lnN \t - - 1.3 -" << std::endl;
           //}

         }

         // uncorrelated:
         for( unsigned i=1; i<this_zinv->GetNbinsX()+1; ++i ) {
           if( this_zinv->GetBinContent(i)==0. ) continue;
           datacard << this_zinv->GetName() << "_bin_" << i << " shapeN2 - - 1 -" << std::endl; // this is 1/sqrt(k*N)
         }


       } // if zinv




       // LOST LEPTON SYSTEMATICS:

       if( this_llep->Integral()>0. ) {
         for( unsigned i=1; i<this_llep->GetNbinsX()+1; ++i ) {
           if( this_zinv->GetBinContent(i)==0. ) continue;
           datacard << this_llep->GetName() << "_bin_" << i << " shapeN2 - - - 1" << std::endl;
         }
       }


       datacard.close();

       if( isig==-1 )
         std::cout << "-> Created template datacard: " << datacardName << std::endl;
       

      }
    }

  }

  return 0;

} 



MT2Analysis<MT2Estimate>* get( const std::string& name, std::vector< MT2Analysis<MT2Estimate>* > analyses, const std::string& name1, const std::string& name2, const std::string& name3, const std::string& name4 ) {


  std::cout << "Looking for: " << name << std::endl;
  MT2Analysis<MT2Estimate>* returnAnalysis = new MT2Analysis<MT2Estimate>( name, analyses[0]->getHTRegions(), analyses[0]->getSignalRegions() );

  for( unsigned i=0; i<analyses.size(); ++i ) {

    if( analyses[i]->getName() == name1 || analyses[i]->getName() == name2 || analyses[i]->getName() == name3 || analyses[i]->getName() == name4 ) {
      std::cout << "  added: " << analyses[i]->getName() << std::endl;
      (*returnAnalysis) += (*analyses[i]);
    }

  }

  return returnAnalysis;

}




void writeToTemplateFile( TFile* file, MT2Analysis<MT2Estimate>* analysis, float err_uncorr ) {

  // err_uncorr is the bin-by-bin error
  // if it's zero, no error will be assigned
  // if it's > 0., it needs to be set as a fractional error (eg. 0.03 will give a 3% error)
  // if it's negative (-1), the histogram bin error will be used

  file->cd();

  TString analysisName(analysis->getName());

  std::set<MT2HTRegion> htRegions = analysis->getHTRegions();
  std::set<MT2SignalRegion> sigRegions = analysis->getSignalRegions();
  
  for( std::set<MT2HTRegion>::iterator iHT=htRegions.begin(); iHT!=htRegions.end(); ++iHT ) {
    for( std::set<MT2SignalRegion>::iterator iSR=sigRegions.begin(); iSR!=sigRegions.end(); ++iSR ) {
  
      MT2Region thisRegion(*iHT, *iSR);

      TH1D* h1 = analysis->get( thisRegion )->yield;

      if(h1->Integral() == 0.){
        for( int b=1; b < h1->GetNbinsX()+1; ++b)
          h1->SetBinContent(b, analysisName.Contains("SMS") ? 1e-4 : 1e-2);
      }
      
      h1->Write();

      if( err_uncorr==0. ) continue;

      for( unsigned iBin=1; iBin<h1->GetNbinsX()+1; ++iBin ) {

        float binContent = h1->GetBinContent(iBin);

        float thisErrUncorr = (err_uncorr>0.) ? err_uncorr : h1->GetBinError(iBin)/binContent;

        TH1D* h1_binUp = new TH1D(*h1);
        h1_binUp->SetName(Form("%s_bin_%dUp", h1->GetName(), iBin));
        h1_binUp->SetBinContent( iBin, binContent*( 1. + thisErrUncorr ) );
        h1_binUp->Write();

        TH1D* h1_binDown = new TH1D(*h1);
        h1_binDown->SetName(Form("%s_bin_%dDown", h1->GetName(), iBin));
        h1_binDown->SetBinContent( iBin, binContent/( 1. + thisErrUncorr ) );
        h1_binDown->Write();

      } // for bins

    } // for SR
  } // for HT

}



void writeToTemplateFile_poisson( TFile* file, MT2Analysis<MT2Estimate>* analysis, const std::string& name ) {

  file->cd();

  std::set<MT2HTRegion> htRegions = analysis->getHTRegions();
  std::set<MT2SignalRegion> sigRegions = analysis->getSignalRegions();
  
  for( std::set<MT2HTRegion>::iterator iHT=htRegions.begin(); iHT!=htRegions.end(); ++iHT ) {
    for( std::set<MT2SignalRegion>::iterator iSR=sigRegions.begin(); iSR!=sigRegions.end(); ++iSR ) {
  
      MT2Region thisRegion(*iHT, *iSR);

      TH1D* h1 = (TH1D*) (analysis->get( thisRegion )->yield->Clone());
      std::string oldName(h1->GetName());
      h1->SetName(Form("%s_%s", oldName.c_str(), name.c_str()));

      h1->Write();

      int nBJetsMin = thisRegion.nBJetsMin();
      if( nBJetsMin>=2 ) continue;

      //////CHANGE HERE for ITERATION 1
      //Fake uncertainty
      //float k = (nBJetsMin==0) ? 2. : 20.;

      //Real uncertainty
      float k = 1.;

      for( unsigned iBin=1; iBin<h1->GetNbinsX()+1; ++iBin ) {

        float binContent = h1->GetBinContent(iBin);
        int N_zinv = (int)binContent;
        float error = (N_zinv>0) ? 1./sqrt(k*N_zinv) : 0.;

        TH1D* h1_binUp = new TH1D(*h1);
        h1_binUp->SetName(Form("%s_bin_%dUp", h1->GetName(), iBin));
        h1_binUp->SetBinContent( iBin, binContent*( 1. + error ) );
        h1_binUp->SetLineColor(kGreen);
        h1_binUp->Write();

        TH1D* h1_binDown = new TH1D(*h1);
        h1_binDown->SetName(Form("%s_bin_%dDown", h1->GetName(), iBin));
        h1_binDown->SetBinContent( iBin, binContent/( 1. + error ) );
        h1_binDown->SetLineColor(kRed);
        h1_binDown->Write();

      } // for bins


      h1->SetName(oldName.c_str());

    } // for SR
  } // for HT

}


