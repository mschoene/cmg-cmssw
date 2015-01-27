#include <cmath>
#include <iostream>

#ifndef __CINT__
#include "RooGlobalFunc.h"
#endif
#include "RooRealVar.h"
#include "RooDataSet.h"
#include "RooDataHist.h"
#include "RooHistPdf.h"
#include "RooAddPdf.h"
#include "TCanvas.h"
#include "TAxis.h"
#include "TH1D.h"
#include "TRandom3.h"
#include "RooPlot.h"

using namespace RooFit;









int main() {


  TH1D* h1_templPrompt = new TH1D("h1_templPrompt", "", 100, 0., 10.);
  TH1D* h1_templFake   = new TH1D("h1_templFake"  , "", 100, 0., 10.);

  TRandom3* rand = new TRandom3(13);

  int nentries = 100000;
  //float weight = 1./((float)nentries);

  for( unsigned i=0; i<nentries; ++i ) {

    h1_templPrompt->Fill( fabs(rand->Gaus(0., 1.)) );
    h1_templFake  ->Fill( rand->Uniform(10.) );

  }




  // Declare observable x
  RooRealVar x("x", "x", 0., 10.) ;

  RooDataHist templPrompt("templPrompt", "", x, h1_templPrompt);
  RooDataHist templFake  ("templFake"  , "", x, h1_templFake  );
  //RooDataHist templPrompt("templPrompt", "", RooArgList(x), h1_templPrompt);
  //RooDataHist templFake  ("templFake"  , "", RooArgList(x), h1_templFake  );

  RooHistPdf pdfPrompt("pdfPrompt", "", x, templPrompt );
  RooHistPdf pdfFake  ("pdfFake"  , "", x, templFake   );

  RooRealVar sigFrac("sigFrac","fraction of signal",0.7,0.,1.) ;
  RooAddPdf  model("model","", RooArgList(pdfPrompt,pdfFake), sigFrac) ;


  RooDataSet *data = model.generate(x,500);
  //RooDataSet *data("data", "", x);

  //float fracPrompt_true = 0.7;
  //int nData = 500;

  //for( unsigned i=0; i<nData; ++i ) {

  //  float coin = rand->Uniform(1.);

  //  if( coin < fracPrompt_true )
  //    data->add( fabs(rand->Gaus(0., 1.)) );
  //  else
  //    data->add( rand->Uniform(10.) );

  //}

  model.fitTo(*data); 

  RooPlot* xframe = x.frame(Title("Example of composite pdf=sig+bkg"));
  data->plotOn(xframe);
  model.plotOn(xframe);

  // Overlay the background component of model with a dashed line
  model.plotOn(xframe,Components(pdfFake),LineStyle(kDashed)) ;

  TCanvas* c1 = new TCanvas("c1", "", 600, 600);
  gPad->SetLeftMargin(0.15);
  xframe->GetYaxis()->SetTitleOffset(1.4); 
  xframe->Draw();

  c1->SaveAs("provaRoofit.eps");

  std::cout << "-> Fitted signal fraction: " << sigFrac.getVal() << " +- " << sigFrac.getError() << std::endl;

  return 0;

}
