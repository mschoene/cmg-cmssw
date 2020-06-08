from PhysicsTools.Heppy.physicsobjects.PhysicsObject import *
from math import exp
import re

import ROOT

class Photon(PhysicsObject ):

    def __init__(self, *args, **kwargs):
        '''Initializing rho to None. The user is responsible for setting it to the right value 
        to get the rho-corrected isolation.'''
        super(Photon, self).__init__(*args, **kwargs)
        self._physObjInit()

    def _physObjInit(self):
        self.rho = None


    def hOVERe(self):
        return self.physObj.hadTowOverEm() 

    def r9(self):
        return self.physObj.r9()

    def sigmaIetaIeta(self):
        return self.physObj.sigmaIetaIeta()

    def full5x5_r9(self):
        return self.physObj.full5x5_r9()

    def full5x5_sigmaIetaIeta(self):
        return self.physObj.full5x5_sigmaIetaIeta()

    def chargedHadronIso(self, corr=None):
        isoCharged = self.ftprAbsIsoCharged03 if hasattr(self,'ftprAbsIsoCharged03') else self.physObj.chargedHadronIso()
        if corr is None or corr == "": return isoCharged
        elif corr == "rhoArea": return max(isoCharged-self.rho*self.EffectiveArea03[0],0)
        else: raise RuntimeError("Photon isolation correction '%s' not yet implemented in Photon.py" % corr)

    def neutralHadronIso(self, corr=None):
        isoNHad = self.ftprAbsIsoNHad03 if hasattr(self,'ftprAbsIsoNHad03') else self.physObj.neutralHadronIso()
        if corr is None or corr == "": return isoNHad
        elif corr == "rhoArea": return max(isoNHad-self.rho*self.EffectiveArea03[1],0)
        else: raise RuntimeError("Photon isolation correction '%s' not yet implemented in Photon.py" % corr)

    def photonIso(self, corr=None):
        isoPho = self.ftprAbsIsoPho03 if hasattr(self,'ftprAbsIsoPho03') else self.physObj.photonIso()
        if corr is None or corr == "": return isoPho
        elif corr == "rhoArea": return max(isoPho-self.rho*self.EffectiveArea03[2],0)
        else: raise RuntimeError("Photon isolation correction '%s' not yet implemented in Photon.py" % corr)

    def photonIDCSA14(self, name, sidebands=False):
        keepThisPhoton = True
        sigmaThresh  = 999
        hovereThresh = 999
        if name == "PhotonCutBasedIDLoose_CSA14":
            if abs(self.physObj.eta())<1.479 :
                sigmaThresh  = 0.010
                hovereThresh = 0.0559
            else :
                sigmaThresh  = 0.030
                hovereThresh = 0.049
        elif name == "PhotonCutBasedIDLoose_PHYS14":
            if abs(self.physObj.eta())<1.479 :
                sigmaThresh  = 0.0106
                hovereThresh = 0.048
            else :
                sigmaThresh  = 0.0266
                hovereThresh = 0.069
        else :
          print "WARNING! Unkown photon ID! Will return true!" 
          return True

        if sidebands:
          if abs(self.physObj.eta())<1.479 :
            sigmaThresh = 0.015
          else :
            sigmaThresh = 0.035

        if self.full5x5_sigmaIetaIeta() > sigmaThresh  : keepThisPhoton = False
        if self.hOVERe()                > hovereThresh : keepThisPhoton = False

        return keepThisPhoton

    def CutBasedIDWP( self, name):
        # recommeneded PHYS14 working points from POG
        WPs = {
        # https://twiki.cern.ch/twiki/bin/viewauth/CMS/CutBasedPhotonIdentificationRun2#Pointers_for_PHYS14_selection_im
        "POG_PHYS14_25ns_Loose": {"conversionVeto": [True,True], "H/E":[0.028,0.093],"sigmaIEtaIEta":[0.0107,0.0272],
        "chaHadIso":[2.67,1.79],"neuHadIso":[[7.23,0.0028,0.5408],[8.89,0.01725]],"phoIso":[[2.11,0.0014],[3.09,0.0091]]},
        
        # https://twiki.cern.ch/twiki/bin/view/CMS/CutBasedPhotonIdentificationRun2?rev=11
        "POG_PHYS14_25ns_Loose_old": {"conversionVeto": [True,True], "H/E":[0.048,0.069],"sigmaIEtaIEta":[0.0106,0.0266],
        "chaHadIso":[2.56,3.12],"neuHadIso":[[3.74,0.0025,0.],[17.11,0.0118,0.]],"phoIso":[[2.68,0.001],[2.70,0.0059]]},
        
        "POG_PHYS14_25ns_Medium": {"conversionVeto": [True,True], "H/E":[0.012,0.023],"sigmaIEtaIEta":[0.0100,0.0267],
        "chaHadIso":[1.79,1.09],"neuHadIso":[[0.16,0.0028,0.5408],[4.31,0.0172]],"phoIso":[[1.90,0.0014],[1.90,0.0091]]},
        
        "POG_PHYS14_25ns_Tight": {"conversionVeto": [True,True], "H/E":[0.010,0.015],"sigmaIEtaIEta":[0.0100,0.0265],
        "chaHadIso":[1.66,1.04],"neuHadIso":[[0.14,0.0028,0.5408],[3.89,0.0172]],"phoIso":[[1.40,0.0014],[1.40,0.0091]]},

        # https://twiki.cern.ch/twiki/bin/viewauth/CMS/CutBasedPhotonIdentificationRun2#SPRING15_selections_bunch_crossing
        "POG_SPRING15_50ns_Loose": {"conversionVeto": [True,True], "H/E":[0.05,0.05],"sigmaIEtaIEta":[0.0103,0.0277],
        "chaHadIso":[2.44,1.84],"neuHadIso":[[2.57,0.0044,0.5809],[4.00, 0.0040,0.9402]],"phoIso":[[1.92,0.0043],[2.15,0.0041]]},

        "POG_SPRING15_50ns_Medium": {"conversionVeto": [True,True], "H/E":[0.05,0.05],"sigmaIEtaIEta":[0.0100,0.0267],
        "chaHadIso":[1.31,1.25],"neuHadIso":[[0.60,0.0044,0.5809],[1.65, 0.0040,0.9402]],"phoIso":[[1.33,0.0043],[1.02,0.0041]]},

        "POG_SPRING15_50ns_Tight": {"conversionVeto": [True,True], "H/E":[0.05,0.05],"sigmaIEtaIEta":[0.0100,0.0267],
        "chaHadIso":[0.91,0.65],"neuHadIso":[[0.33,0.0044,0.5809],[0.93, 0.0040,0.9402]],"phoIso":[[0.61,0.0043],[0.54,0.0041]]},

        # https://twiki.cern.ch/twiki/bin/viewauth/CMS/CutBasedPhotonIdentificationRun2#SPRING15_selections_25_ns
        "POG_SPRING15_25ns_Loose": {"conversionVeto": [True,True], "H/E":[0.05,0.05],"sigmaIEtaIEta":[0.0102,0.0274],
        "chaHadIso":[3.32,1.97],"neuHadIso":[[1.92,0.014,0.000019],[11.86, 0.0139,0.000025]],"phoIso":[[0.81,0.0053],[0.83,0.0034]]},

        "POG_SPRING15_25ns_Medium": {"conversionVeto": [True,True], "H/E":[0.05,0.05],"sigmaIEtaIEta":[0.0102,0.0268],
        "chaHadIso":[1.37,1.10],"neuHadIso":[[1.06,0.014,0.000019],[2.69, 0.0139,0.000025]],"phoIso":[[0.28,0.0053],[0.39,0.0034]]},

        "POG_SPRING15_25ns_Tight": {"conversionVeto": [True,True], "H/E":[0.05,0.05],"sigmaIEtaIEta":[0.0100,0.0268],
        "chaHadIso":[0.76,0.56],"neuHadIso":[[0.97,0.014,0.000019],[2.09, 0.0139,0.000025]],"phoIso":[[0.08,0.0053],[0.16,0.0034]]},

        "POG_Spring16_Medium": {"conversionVeto": [True,True], "H/E":[0.0396,0.0219],"sigmaIEtaIEta":[0.01022,0.03001],
        "chaHadIso":[0.441,0.442],"neuHadIso":[[2.725,0.0148,0.000017],[1.715,0.0163,0.000014]],"phoIso":[[2.571,0.0047],[3.863, 0.0034]]},

        ##loose 16 cut based
        "POG_Spring16_Loose": {"conversionVeto": [True,True], "H/E":[0.0597,0.0481],"sigmaIEtaIEta":[0.01031, 0.03013 ],
        "chaHadIso":[1.295,1.011],"neuHadIso":[[10.910,0.0148,0.000017],[5.931,0.0163,0.000014]],"phoIso":[[3.630,0.0047],[6.641, 0.0034]]},

        # https://twiki.cern.ch/twiki/bin/viewauth/CMS/CutBasedPhotonIdentificationRun2#CSA14_selections_for_20_bx_25_sc
        "POG_CSA14_25ns_Loose": {"conversionVeto": [True,True], "H/E":[0.553,0.062],"sigmaIEtaIEta":[0.0099,0.0284],
        "chaHadIso":[2.49,1.04],"neuHadIso":[[15.43,0.007],[19.71,0.0129]],"phoIso":[[9.42,0.0033],[11.88,0.0108]]},
        
        "POG_CSA14_25ns_Medium": {"conversionVeto": [True,True], "H/E":[0.058,0.020],"sigmaIEtaIEta":[0.0099,0.0268],
        "chaHadIso":[1.91,0.82],"neuHadIso":[[4.66,0.007],[14.65,0.0129]],"phoIso":[[4.29,0.0033],[4.06,0.0108]]},
        
        "POG_CSA14_25ns_Tight": {"conversionVeto": [True,True], "H/E":[0.019,0.016],"sigmaIEtaIEta":[0.0099,0.0263],
        "chaHadIso":[1.61,0.69],"neuHadIso":[[3.98,0.007],[4.52,0.0129]],"phoIso":[[3.01,0.0033],[3.61,0.0108]]},

        ##medium 16 cut based
        "POG_Spring16_Medium": {"conversionVeto": [True,True], "H/E":[0.0396,0.0219],"sigmaIEtaIEta":[0.01022,0.03001],
        "chaHadIso":[0.441,0.442],"neuHadIso":[[2.725,0.0148,0.000017],[1.715,0.0163,0.000014]],"phoIso":[[2.571,0.0047],[3.863, 0.0034]]},

        ##loose 16 cut based
        "POG_Spring16_Loose": {"conversionVeto": [True,True], "H/E":[0.0597,0.0481],"sigmaIEtaIEta":[0.01031, 0.03013 ],
        "chaHadIso":[1.295,1.011],"neuHadIso":[[10.910,0.0148,0.000017],[5.931,0.0163,0.000014]],"phoIso":[[3.630,0.0047],[6.641, 0.0034]]},

        ##loose 17 cut based
        "POG_Spring17_Loose": {"conversionVeto": [True,True], "H/E":[0.04596,0.0590],"sigmaIEtaIEta":[0.0106, 0.0272 ],
        "chaHadIso":[1.694,2.089],"neuHadIso":[[24.032,0.01512,0.00002259],[19.722,0.0117,0.000023]],"phoIso":[[2.876,0.004017],[4.162, 0.0037]]},

#        "POG_Spring17_Loose": {"conversionVeto": [True,True], "H/E":[0.105,0.029],"sigmaIEtaIEta":[0.0103, 0.0276 ],
#        "chaHadIso":[2.839,2.150],"neuHadIso":[[9.188,0.0126,0.000026],[10.471,0.0119,0.000025]],"phoIso":[[2.956,0.0035],[4.895, 0.0040]]},

        }
        
        baseWP = re.split('_',name)
        if "looseSieie" in baseWP[-1]: 
            baseWP.pop()
            WPs["_".join(baseWP)]["sigmaIEtaIEta"] = [0.0103,0.0276]

        return WPs["_".join(baseWP)]


    def etaRegionID(self):
        #return 0 if the photon is in barrel and 1 if in endcap
        if abs(self.physObj.eta())<1.479 :
            idForBarrel = 0
        else:
            idForBarrel = 1
        return idForBarrel

    def calScaledIsoValueLin(self,offset,slope):
        return slope*self.pt()+offset

    def calScaledIsoValueQuadr(self,offset,term_1,term_2):
        return offset + term_1*self.pt() + term_2*pow(self.pt(),2)

    def calScaledIsoValueExp(self,offset,slope_exp,offset_exp):
        return offset + exp(slope_exp*self.pt()+offset_exp)

    def passPhotonID(self,name,conversionSafe_eleVeto=False):
        
        idForBarrel = self.etaRegionID()
        passPhotonID = True

        if self.CutBasedIDWP(name)["conversionVeto"][idForBarrel]:
            if ( self.physObj.passElectronVeto()==False ):
                #            if (conversionSafe_eleVeto==False and self.physObj.hasPixelSeed()) or (conversionSafe_eleVeto==True and self.physObj.passElectronVeto()==False):
                passPhotonID = False

        if self.CutBasedIDWP(name)["H/E"][idForBarrel] < self.hOVERe():
            passPhotonID = False

        if self.CutBasedIDWP(name)["sigmaIEtaIEta"][idForBarrel] < self.full5x5_sigmaIetaIeta():
            passPhotonID = False

        return passPhotonID

    def passPhotonIso(self,name,isocorr):

        idForBarrel = self.etaRegionID()
        passPhotonIso = True

        if self.CutBasedIDWP(name)["chaHadIso"][idForBarrel] < self.chargedHadronIso(isocorr):
            passPhotonIso = False

        if "POG_PHYS14_25ns" in name and idForBarrel == 0:
            if self.calScaledIsoValueExp(*self.CutBasedIDWP(name)["neuHadIso"][idForBarrel]) < self.neutralHadronIso(isocorr):
                passPhotonIso = False
        elif "POG_SPRING15_50ns" in name:
             if self.calScaledIsoValueExp(*self.CutBasedIDWP(name)["neuHadIso"][idForBarrel]) < self.neutralHadronIso(isocorr):
                 passPhotonIso = False
        elif "POG_SPRING15_25ns" in name:
             if self.calScaledIsoValueQuadr(*self.CutBasedIDWP(name)["neuHadIso"][idForBarrel]) < self.neutralHadronIso(isocorr):
                 passPhotonIso = False
             if self.calScaledIsoValueLin(*self.CutBasedIDWP(name)["phoIso"][idForBarrel]) < self.photonIso(isocorr):
                 passPhotonIso = False
        elif "POG_Spring16" or "POG_Spring17" in name:
             if self.calScaledIsoValueQuadr(*self.CutBasedIDWP(name)["neuHadIso"][idForBarrel]) < self.neutralHadronIso(isocorr):
                 passPhotonIso = False
             if self.calScaledIsoValueLin(*self.CutBasedIDWP(name)["phoIso"][idForBarrel]) < self.photonIso(isocorr):
                 passPhotonIso = False
        else:
            if self.calScaledIsoValueLin(*self.CutBasedIDWP(name)["neuHadIso"][idForBarrel]) < self.neutralHadronIso(isocorr):
                passPhotonIso = False
            if self.calScaledIsoValueLin(*self.CutBasedIDWP(name)["phoIso"][idForBarrel]) < self.photonIso(isocorr):
                passPhotonIso = False
        
        return passPhotonIso

    pass

setattr(ROOT.pat.Photon, "recoPhotonIso", ROOT.reco.Photon.photonIso)
setattr(ROOT.pat.Photon, "recoNeutralHadronIso", ROOT.reco.Photon.neutralHadronIso)
setattr(ROOT.pat.Photon, "recoChargedHadronIso", ROOT.reco.Photon.chargedHadronIso)
