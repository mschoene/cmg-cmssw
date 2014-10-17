#include <iostream>
#include <cmath>

#include "TFile.h"
#include "TCanvas.h"
#include "TTree.h"
#include "TChain.h"
#include "TH2D.h"
#include "TH1D.h"
#include "TLegend.h"

#include "CommonTools/DrawBase.h"
#include "QGDev/qgMiniTuple/localQGLikelihoodCalculator/localQGLikelihoodCalculator2.h"


struct FlavorHistos {

  TH1F* h1_njets;
  TH1F* h1_nquarks_partonFlavour;
  TH1F* h1_nquarks_partonId;
  TH1F* h1_nquarks_qgCut;
  TH1F* h1_nquarks_qgLikeli;

  TH1F* h1_nquarksOverNjets;

  std::vector<TH1F*> vh1_qgl;

  std::vector<TH1F*> vh1_qgl_q;
  std::vector<TH1F*> vh1_qgl_g;
  std::vector<TH1F*> vh1_qgl_c;
  std::vector<TH1F*> vh1_qgl_b;

};


FlavorHistos getFlavorHistos( QGLikelihoodCalculator2* qglc, TTree* tree, const std::string& name );
void drawFlavorHistos( const std::string& outputdir, FlavorHistos fh, const std::string& savename, const std::string& axisName );
float getMaxNorm( TH1F* h1 );
void compareMultHistos( const std::string& outputdir, FlavorHistos fh1, const std::string& name1, const std::string& axisName1, FlavorHistos fh2, const std::string& name2, const std::string& axisName2 );
void compareQglHistos( const std::string& outputdir, FlavorHistos fh_signal1, const std::string& signal1, FlavorHistos fh_signal2, const std::string& signal2, FlavorHistos fh_bg, const std::string& bg );
void compareSingleHistos( const std::string& outputdir, const std::string& savename, const std::string& axisName, TH1F* h1_signal1, const std::string& legendName_signal1, TH1F* h1_signal2, const std::string& legendName_signal2, TH1F* h1_bg, const std::string& legendName_bg );
void writeHistosToFile( TFile* file, FlavorHistos fh, const std::string& name );



int main() {

  DrawBase* db = new DrawBase("qgStudies");

  std::string dir = "/scratch/pandolf/CSA14_babytrees_QG/skim_mt2_200/";


  TChain* tree_wjets = new TChain("mt2");
  tree_wjets->Add(Form("%s/WJetsToLNu_HT*_PU_S14_POSTLS170_babytree_QG_post_skim.root/mt2", dir.c_str()));

  std::string signal1 = "T1qqqq_2J_mGl1000_mLSP800";
  std::string signal2 = "T1qqqq_2J_mGl1400_mLSP100";
  std::string signal1_forPlots = "T1qqqq (1000, 800)";
  std::string signal2_forPlots = "T1qqqq (1400, 100)";
  std::string bg = "wjets";
  std::string bg_forPlots = "W+Jets";

  TChain* tree_signal1 = new TChain("mt2");
  tree_signal1->Add(Form("%s/SMS_%s_PU_S14_POSTLS170_babytree_QG_post_skim.root/mt2", dir.c_str(), signal1.c_str()));
  
  TChain* tree_signal2 = new TChain("mt2");
  tree_signal2->Add(Form("%s/SMS_%s_PU_S14_POSTLS170_babytree_QG_post_skim.root/mt2", dir.c_str(), signal2.c_str()));

  std::string qgfileName = "pdfsOnlyPt.root";
  QGLikelihoodCalculator2* qglc = new QGLikelihoodCalculator2(qgfileName); 

  std::string outputdir = "QGStudiesPlots";
  system( Form("mkdir -p %s", outputdir.c_str()) );

  FlavorHistos fh_signal1 = getFlavorHistos(qglc, tree_signal1, signal1); 
  drawFlavorHistos( outputdir, fh_signal1, signal1, signal1_forPlots );

  FlavorHistos fh_signal2 = getFlavorHistos(qglc, tree_signal2, signal2); 
  drawFlavorHistos( outputdir, fh_signal2, signal2, signal2_forPlots );

  FlavorHistos fh_wjets   = getFlavorHistos(qglc, tree_wjets, bg );
  drawFlavorHistos( outputdir, fh_wjets, bg, bg_forPlots );


  compareMultHistos( outputdir, fh_signal1, signal1, signal1_forPlots, fh_wjets, bg, bg_forPlots );
  compareMultHistos( outputdir, fh_signal2, signal2, signal2_forPlots, fh_wjets, bg, bg_forPlots );

  compareQglHistos( outputdir, fh_signal1, signal1_forPlots, fh_signal2, signal2_forPlots, fh_wjets, bg_forPlots );


  compareSingleHistos( outputdir, "nquarksOverNjets", "N(quarks) / N(jets)", fh_signal1.h1_nquarksOverNjets, signal1_forPlots, fh_signal2.h1_nquarksOverNjets, signal2_forPlots, fh_wjets.h1_nquarksOverNjets, bg_forPlots );

  std::cout << "N(quarks) / N(jets)" << std::endl;
  std::cout << signal1 << ":  " << fh_signal1.h1_nquarksOverNjets->GetMean() << std::endl;
  std::cout << signal2 << ":  " << fh_signal2.h1_nquarksOverNjets->GetMean() << std::endl;
  std::cout << bg << ":  " << fh_wjets.h1_nquarksOverNjets->GetMean() << std::endl;

  TFile* outfile = TFile::Open("FlavorHistos.root", "recreate");
  outfile->cd();
  writeHistosToFile( outfile, fh_signal1, signal1 );
  writeHistosToFile( outfile, fh_signal2, signal2 );
  writeHistosToFile( outfile, fh_wjets, bg );
  outfile->Close();

  return 0;

}


FlavorHistos getFlavorHistos( QGLikelihoodCalculator2* qglc, TTree* tree, const std::string& name ) {


  float evt_scale1fb;
  tree->SetBranchAddress( "evt_scale1fb", &evt_scale1fb );
  float mt2;
  tree->SetBranchAddress( "mt2", &mt2 );
  float ht;
  tree->SetBranchAddress( "ht", &ht );
  int nJet40;
  tree->SetBranchAddress( "nJet40", &nJet40 );
  int nBJet40;
  tree->SetBranchAddress( "nBJet40", &nBJet40 );
  int njet;
  tree->SetBranchAddress( "njet", &njet );
  float jet_pt[100];
  tree->SetBranchAddress( "jet_pt", jet_pt );
  float jet_eta[100];
  tree->SetBranchAddress( "jet_eta", jet_eta );
  int jet_mcFlavour[100];
  tree->SetBranchAddress( "jet_mcFlavour", jet_mcFlavour );
  int jet_partonId[100];
  tree->SetBranchAddress( "jet_partonId", jet_partonId );
  float jet_ptd[100];
  tree->SetBranchAddress( "jet_ptd", jet_ptd );
  int jet_mult[100];
  tree->SetBranchAddress( "jet_mult", jet_mult );
  float jet_axis2[100];
  tree->SetBranchAddress( "jet_axis2", jet_axis2 );
  int jet_id[100];
  tree->SetBranchAddress( "jet_id", jet_id );
  int jet_puId[100];
  tree->SetBranchAddress( "jet_puId", jet_puId );


  int nbins = 15;
  TH1F* h1_njets                 = new TH1F(Form("njets_%s",                 name.c_str()), "", nbins, -0.5, (float)nbins-0.5);
  TH1F* h1_nquarks_partonFlavour = new TH1F(Form("nquarks_partonFlavour_%s", name.c_str()), "", nbins, -0.5, (float)nbins-0.5);
  TH1F* h1_nquarks_partonId      = new TH1F(Form("nquarks_partonId_%s",      name.c_str()), "", nbins, -0.5, (float)nbins-0.5);
  TH1F* h1_nquarks_qgCut          = new TH1F(Form("nquarks_qgCut_%s",          name.c_str()), "", nbins, -0.5, (float)nbins-0.5);
  TH1F* h1_nquarks_qgLikeli      = new TH1F(Form("nquarks_qgLikeli_%s",      name.c_str()), "", nbins, -0.5, (float)nbins-0.5);
  h1_njets                ->Sumw2();
  h1_nquarks_partonFlavour->Sumw2();
  h1_nquarks_partonId     ->Sumw2();
  h1_nquarks_qgCut         ->Sumw2();
  h1_nquarks_qgLikeli     ->Sumw2();


  TH1F* h1_nquarksOverNjets      = new TH1F(Form("nquarksOverNjets_%s",      name.c_str()), "", 50, 0., 2.);
  h1_nquarksOverNjets->Sumw2();


  std::vector<TH1F*> vh1_qgl;
  std::vector<TH1F*> vh1_qgl_q;
  std::vector<TH1F*> vh1_qgl_g;
  std::vector<TH1F*> vh1_qgl_c;
  std::vector<TH1F*> vh1_qgl_b;

  for( unsigned i=0; i<10; ++i ) {

    TH1F* h1_new = new TH1F(Form("qgl_%s_%d", name.c_str(), i), "", 50, 0., 1.0001);
    h1_new->Sumw2();
    vh1_qgl.push_back(h1_new);

    TH1F* h1_new_q = new TH1F(Form("qgl_q_%s_%d", name.c_str(), i), "", 50, 0., 1.0001);
    h1_new_q->Sumw2();
    vh1_qgl_q.push_back(h1_new_q);

    TH1F* h1_new_g = new TH1F(Form("qgl_g_%s_%d", name.c_str(), i), "", 50, 0., 1.0001);
    h1_new_g->Sumw2();
    vh1_qgl_g.push_back(h1_new_g);

    TH1F* h1_new_c = new TH1F(Form("qgl_c_%s_%d", name.c_str(), i), "", 50, 0., 1.0001);
    h1_new_c->Sumw2();
    vh1_qgl_c.push_back(h1_new_c);

    TH1F* h1_new_b = new TH1F(Form("qgl_b_%s_%d", name.c_str(), i), "", 50, 0., 1.0001);
    h1_new_b->Sumw2();
    vh1_qgl_b.push_back(h1_new_b);

  }


  int nentries = tree->GetEntries();
 
  std::cout << "-> Starting loop on: " << name << std::endl;

  for( unsigned iEntry=0; iEntry<nentries; ++iEntry ) {

    tree->GetEntry(iEntry);

    if( iEntry % 10000 == 0 ) std::cout << "  Entry: " << iEntry << " / " << nentries << std::endl;

    if( ht < 450. ) continue;
    if( mt2 < 200. ) continue;
    if( nJet40 < 2 ) continue;
    if( nBJet40 != 0 ) continue;

    int njets = 0;
    int nquarks_partonFlavour = 0;
    int nquarks_partonId = 0;
    int nquarks_qgCut = 0;
    int nquarks_qgLikeli = 0;


    for( unsigned i=0; i<njet && i<100; ++i ) {

      if( jet_pt[i]<40. ) continue; 
      if( fabs(jet_eta[i])>2.5 ) continue; 
      if( jet_id[i]==0 ) continue; 
      if( jet_puId[i]==0 ) continue; 

      std::vector<float> vars;
      vars.push_back( jet_mult[i] );
      vars.push_back( jet_ptd[i] );
      vars.push_back( jet_axis2[i] );
      float qgl = qglc->computeQGLikelihood( jet_pt[i], jet_eta[i], vars );

      if( njets<10 ) {

        vh1_qgl[njets]->Fill( qgl, evt_scale1fb );

        if( jet_partonId[i]!=0 && abs(jet_partonId[i])<4 ) { //uds
          vh1_qgl_q[njets]->Fill( qgl, evt_scale1fb );
        } else if( jet_partonId[i]==21 ) {
          vh1_qgl_g[njets]->Fill( qgl, evt_scale1fb );
        } else if( abs(jet_partonId[i])==4 ) {
          vh1_qgl_c[njets]->Fill( qgl, evt_scale1fb );
        } else if( abs(jet_partonId[i])==5 ) {
          vh1_qgl_b[njets]->Fill( qgl, evt_scale1fb );
        } 

      } // if njets < 10
      

      njets++;
      if( abs(jet_mcFlavour[i])<4 && jet_mcFlavour[i]!=0 ) nquarks_partonFlavour++;
      if( abs(jet_partonId[i])<4 && jet_partonId[i]!=0 ) nquarks_partonId++;
      if( qgl>0.1 ) nquarks_qgCut++;

    } // for jets

    h1_njets->Fill( njets, evt_scale1fb );
    h1_nquarks_partonFlavour->Fill( nquarks_partonFlavour, evt_scale1fb );
    h1_nquarks_partonId->Fill( nquarks_partonId, evt_scale1fb );
    h1_nquarks_qgCut->Fill( nquarks_qgCut, evt_scale1fb );

    h1_nquarksOverNjets->Fill( (float)nquarks_qgCut/njets, evt_scale1fb );

  } // for entries



  FlavorHistos fh;
  fh.h1_njets = h1_njets;
  fh.h1_nquarks_partonFlavour = h1_nquarks_partonFlavour;
  fh.h1_nquarks_partonId = h1_nquarks_partonId;
  fh.h1_nquarks_qgCut = h1_nquarks_qgCut;
  fh.h1_nquarks_qgLikeli = 0;

  fh.h1_nquarksOverNjets = h1_nquarksOverNjets;

  fh.vh1_qgl = vh1_qgl;

  fh.vh1_qgl_q = vh1_qgl_q;
  fh.vh1_qgl_g = vh1_qgl_g;
  fh.vh1_qgl_c = vh1_qgl_c;
  fh.vh1_qgl_b = vh1_qgl_b;

  return fh;
  
}
  


void drawFlavorHistos( const std::string& outputdir, FlavorHistos fh, const std::string& savename, const std::string& axisName ) {

  TCanvas* c1 = new TCanvas("c1", "", 600, 600 );
  c1->cd();

  float xMax = fh.h1_njets->GetXaxis()->GetXmax();

  float yMax_njets = getMaxNorm(fh.h1_njets);
  float yMax_nquarks_partonId = getMaxNorm(fh.h1_nquarks_partonId);
  float yMax_nquarks_qgCut = getMaxNorm(fh.h1_nquarks_qgCut);

  float yMax = yMax_njets;
  if( yMax_nquarks_partonId>yMax ) yMax = yMax_nquarks_partonId;
  if( yMax_nquarks_qgCut>yMax ) yMax = yMax_nquarks_qgCut;
  yMax *= 1.2;

  TH2D* h2_axes = new TH2D("axes", "", 10, -0.5, xMax, 10, 0., yMax );
  h2_axes->SetXTitle("Jet Multiplicity");
  h2_axes->SetYTitle("Normalized to Unity");
  h2_axes->Draw();

  fh.h1_njets->SetLineWidth(2);
  fh.h1_njets->SetLineColor(kBlack);

  fh.h1_nquarks_partonId->SetLineWidth(2);
  fh.h1_nquarks_partonId->SetLineColor(kRed+1);

  fh.h1_nquarks_qgCut->SetLineWidth(2);
  fh.h1_nquarks_qgCut->SetLineColor(38);


  TLegend* legend = new TLegend( 0.55, 0.65, 0.9, 0.9, axisName.c_str() );
  legend->SetTextSize(0.035);
  legend->SetFillColor(0);
  legend->AddEntry( fh.h1_njets, "Jets", "L" );
  legend->AddEntry( fh.h1_nquarks_partonId, "Quarks (MC Truth)", "L" );
  legend->AddEntry( fh.h1_nquarks_qgCut, "Quarks (QGL>0.5)", "L" );
  legend->Draw("same");

  fh.h1_njets->DrawNormalized("histo same");
  fh.h1_nquarks_partonId->DrawNormalized("histo same");
  fh.h1_nquarks_qgCut->DrawNormalized("histo same");

  gPad->RedrawAxis();

  c1->SaveAs(Form("%s/mult_%s.eps", outputdir.c_str(), savename.c_str()));
  c1->SaveAs(Form("%s/mult_%s.png", outputdir.c_str(), savename.c_str()));

  delete c1;
  delete h2_axes;

}


float getMaxNorm( TH1F* h1 ) {
 
  return h1->GetMaximum()/h1->Integral();

}



void compareMultHistos( const std::string& outputdir, FlavorHistos fh1, const std::string& name1, const std::string& axisName1, FlavorHistos fh2, const std::string& name2, const std::string& axisName2 ) {

  TCanvas* c1 = new TCanvas("c1", "", 600, 600 );
  c1->cd();

  float xMax = fh1.h1_njets->GetXaxis()->GetXmax();

  float yMax_njets = getMaxNorm(fh2.h1_njets);
  float yMax_nquarks_partonId = getMaxNorm(fh2.h1_nquarks_partonId);
  float yMax_nquarks_qgCut = getMaxNorm(fh2.h1_nquarks_qgCut);

  float yMax = yMax_njets;
  //if( yMax_nquarks_partonId>yMax ) yMax = yMax_nquarks_partonId;
  if( yMax_nquarks_qgCut>yMax ) yMax = yMax_nquarks_qgCut;
  yMax *= 1.2;

  TH2D* h2_axes = new TH2D("axes", "", 10, -0.5, xMax, 10, 0., yMax );
  h2_axes->SetXTitle("Jet Multiplicity");
  h2_axes->SetYTitle("Normalized to Unity");
  h2_axes->Draw();

  fh1.h1_njets->SetLineWidth(2);
  fh1.h1_njets->SetLineColor(46);
  fh1.h1_nquarks_qgCut->SetLineWidth(2);
  fh1.h1_nquarks_qgCut->SetLineStyle(2);
  fh1.h1_nquarks_qgCut->SetLineColor(kRed+2);

  fh2.h1_njets->SetLineWidth(2);
  fh2.h1_njets->SetLineColor(38);
  fh2.h1_nquarks_qgCut->SetLineWidth(2);
  fh2.h1_nquarks_qgCut->SetLineStyle(2);
  fh2.h1_nquarks_qgCut->SetLineColor(kBlue+2);

  //fh.h1_nquarks_partonId->SetLineWidth(2);
  //fh.h1_nquarks_partonId->SetLineColor(kRed+1);



  TLegend* legend = new TLegend( 0.37, 0.65, 0.9, 0.9 );
  legend->SetTextSize(0.035);
  legend->SetFillColor(0);
  legend->AddEntry( fh1.h1_njets, Form("%s (nJets)", axisName1.c_str()), "L" );
  legend->AddEntry( fh1.h1_nquarks_qgCut, Form("%s (QG Cut)", axisName1.c_str()), "L" );
  legend->AddEntry( fh2.h1_njets, Form("%s (nJets)", axisName2.c_str()), "L" );
  legend->AddEntry( fh2.h1_nquarks_qgCut, Form("%s (QG Cut)", axisName2.c_str()), "L" );
  legend->Draw("same");

  fh1.h1_njets->DrawNormalized("histo same");
  fh1.h1_nquarks_qgCut->DrawNormalized("histo same");
  fh2.h1_njets->DrawNormalized("histo same");
  fh2.h1_nquarks_qgCut->DrawNormalized("histo same");

  gPad->RedrawAxis();

  c1->SaveAs(Form("%s/%s_vs_%s.eps", outputdir.c_str(), name1.c_str(), name2.c_str()));
  c1->SaveAs(Form("%s/%s_vs_%s.png", outputdir.c_str(), name1.c_str(), name2.c_str()));

  delete c1;
  delete h2_axes;


}



void compareQglHistos( const std::string& outputdir, FlavorHistos fh_signal1, const std::string& signal1, FlavorHistos fh_signal2, const std::string& signal2, FlavorHistos fh_bg, const std::string& bg ) {

  std::vector<std::string> ranking;
  ranking.push_back("First");
  ranking.push_back("Second");
  ranking.push_back("Third");
  ranking.push_back("Fourth");
  ranking.push_back("Fifth");
  ranking.push_back("Sixth");
  ranking.push_back("Seventh");
  ranking.push_back("Eighth");
  ranking.push_back("Ninth");
  ranking.push_back("Tenth");

  for( unsigned i=0; i<10; ++i ) {

    TH1F* h1_bg = fh_bg.vh1_qgl[i];
    TH1F* h1_signal1 = fh_signal1.vh1_qgl[i];
    TH1F* h1_signal2 = fh_signal2.vh1_qgl[i];

    float qFrac_bg = fh_bg.vh1_qgl_q[i]->Integral()/h1_bg->Integral();
    float qFrac_signal1 = fh_signal1.vh1_qgl_q[i]->Integral()/h1_signal1->Integral();
    float qFrac_signal2 = fh_signal2.vh1_qgl_q[i]->Integral()/h1_signal2->Integral();

    std::string legendName_signal1(Form("%s (%.0f%% quarks)", signal1.c_str(), 100.*qFrac_signal1));
    std::string legendName_signal2(Form("%s (%.0f%% quarks)", signal2.c_str(), 100.*qFrac_signal2));
    std::string legendName_bg(Form("%s (%.0f%% quarks)", bg.c_str(), 100.*qFrac_bg));

    std::string axisName(Form("%s Jet QGLD", ranking[i].c_str()));

    compareSingleHistos( outputdir, Form("qgl_jet%d", i), axisName, h1_signal1, legendName_signal1, h1_signal2, legendName_signal2, h1_bg, legendName_bg );

  }

}


void compareSingleHistos( const std::string& outputdir, const std::string& savename, const std::string& axisName, TH1F* h1_signal1, const std::string& legendName_signal1, TH1F* h1_signal2, const std::string& legendName_signal2, TH1F* h1_bg, const std::string& legendName_bg ) {


  TCanvas* c2 = new TCanvas("c2", "", 600, 600 );
  c2->cd();

  h1_signal1->SetLineColor(46);
  h1_signal1->SetLineColor(kRed+2);

  h1_signal1->SetLineWidth(2);
  h1_signal2->SetLineWidth(2);

  h1_bg->SetFillColor(kOrange);

  float yMaxRel_bg = getMaxNorm(h1_bg);
  float yMaxRel_signal1 = getMaxNorm(h1_signal1);
  float yMaxRel_signal2 = getMaxNorm(h1_signal2);
  float yMax = yMaxRel_bg;
  if( yMaxRel_signal1>yMax ) yMax = yMaxRel_signal1;
  if( yMaxRel_signal2>yMax ) yMax = yMaxRel_signal2;
  yMax*=1.2;

  float xMin = h1_bg->GetXaxis()->GetXmin();
  float xMax = h1_bg->GetXaxis()->GetXmax();

  TH2D* h2_axes = new TH2D("axes", "", 10, xMin, xMax, 10, 0., yMax );
  h2_axes->SetXTitle(axisName.c_str());
  h2_axes->SetYTitle("Normalized to Unity");
  h2_axes->Draw();

  TLegend* legend = new TLegend( 0.28, 0.65, 0.65, 0.9 );
  legend->SetFillColor(0);
  legend->SetTextSize(0.035);
  legend->AddEntry( h1_signal1, legendName_signal1.c_str(), "L" );
  legend->AddEntry( h1_signal2, legendName_signal2.c_str(), "L" );
  legend->AddEntry( h1_bg, legendName_bg.c_str(), "F" );
  legend->Draw("same");

  h1_bg->DrawNormalized("histo same");
  h1_signal1->DrawNormalized("histo same");
  h1_signal2->DrawNormalized("histo same");

  gPad->RedrawAxis();

  c2->SaveAs(Form("%s/%s.eps", outputdir.c_str(), savename.c_str()));
  c2->SaveAs(Form("%s/%s.png", outputdir.c_str(), savename.c_str()));

  delete c2;
  delete h2_axes;

}



void writeHistosToFile( TFile* file, FlavorHistos fh, const std::string& name ) {

  file->cd();
  file->mkdir(name.c_str());
  file->cd(name.c_str());

  fh.h1_njets->Write();
  fh.h1_nquarks_partonFlavour->Write();
  fh.h1_nquarks_partonId->Write();
  fh.h1_nquarks_qgCut->Write();
  //fh.h1_nquarks_qgLikeli->Write();

  fh.h1_nquarksOverNjets->Write();


  for( unsigned i=0; i<fh.vh1_qgl.size(); ++i ) {

    fh.vh1_qgl[i]->Write();

    fh.vh1_qgl_q[i]->Write();
    fh.vh1_qgl_g[i]->Write();
    fh.vh1_qgl_c[i]->Write();
    fh.vh1_qgl_b[i]->Write();

  }
  
  file->cd();

}
