from PhysicsTools.Heppy.analyzers.core.Analyzer import Analyzer
from PhysicsTools.Heppy.analyzers.core.AutoHandle import AutoHandle
from PhysicsTools.Heppy.physicsobjects.Electron import Electron
from PhysicsTools.Heppy.physicsobjects.Muon import Muon
from PhysicsTools.Heppy.physicsobjects.Photon import Photon

from PhysicsTools.HeppyCore.utils.deltar import bestMatch
import PhysicsTools.HeppyCore.framework.config as cfg
from PhysicsTools.HeppyCore.utils.deltar import * 
from PhysicsTools.Heppy.physicsutils.genutils import *


import math, os
from ROOT import heppy, TLorentzVector
import math
import copy


class PhotonCleaner( Analyzer ):

    def __init__(self, cfg_ana, cfg_comp, looperName ):
        super(PhotonCleaner,self).__init__(cfg_ana,cfg_comp,looperName)

        self.lepPtMin = getattr(self.cfg_ana, 'minLepPt', -1)
        self.lepSelCut = getattr(self.cfg_ana, 'lepSelCut', lambda lep : True)
        self.eleGammaDR =  getattr(self.cfg_ana, 'eleGammaDR', 1.0)
        self.muGammaDR =  getattr(self.cfg_ana, 'muGammaDR', 0.5)

        if not hasattr(self.cfg_ana ,"collectionPostFix"):self.cfg_ana.collectionPostFix=""


    #----------------------------------------
    # DECLARATION OF HANDLES OF LEPTONS STUFF   
    #----------------------------------------
        

    def beginLoop(self, setup):
        super(PhotonCleaner,self).beginLoop(setup)
        self.counters.addCounter('events')
        count = self.counters.counter('events')
        count.register('all events')
    

    def process(self, event):
        self.readCollections( event.input )
        self.counters.counter('events').inc('all events')

        LeptonCleanedPhotons = []
        photons = []
        if hasattr(event, 'selectedPhotons'):
            photons = [ g for g in event.selectedPhotons ] 

        leptons = []
        if hasattr(event, 'selectedLeptons'):
            leptons = [ l for l in event.selectedLeptons if l.pt() > self.lepPtMin and self.lepSelCut(l) ]

#remove leptons from photon collection

        if len(leptons)>0:
            for gamma in photons:
                isNotLep = True
                for lep in leptons:
                    dr = deltaR(gamma.eta(),gamma.phi(), lep.eta(),lep.phi())
                    if ( dr < 1.0 and abs(lep.pdgId())==11 ): #electron
                        isNotLep = False
                    if ( dr < 0.5 and abs(lep.pdgId())==13):  #muon
                        isNotLep = False
                if isNotLep:
                    LeptonCleanedPhotons.append(gamma)
                    
        #self.selectedLeptons  = [ l for l in event.selectedLeptons if l not in self.discardedLeptons ]
        self.selectedPhotons  = [ l for l in LeptonCleanedPhotons ]

        setattr(event,"selectedPhotons"          +self.cfg_ana.collectionPostFix, self.selectedPhotons          ) 

            
        return True



#A default config
setattr(PhotonCleaner,"defaultConfig",cfg.Analyzer(
#    verbose=False,
    class_object=PhotonCleaner,
    # input collections

    minLepPt = 10,
    lepSelCut = lambda lep : True,

    eleGammaDR = 1.0,
    muGammaDR = 0.5,

    collectionPostFix = ""
    )
)
