from PhysicsTools.Heppy.analyzers.core.Analyzer import Analyzer
from PhysicsTools.Heppy.analyzers.core.AutoHandle import AutoHandle
from PhysicsTools.Heppy.physicsobjects.PhysicsObjects import Jet
from PhysicsTools.HeppyCore.utils.deltar import * 
from PhysicsTools.HeppyCore.statistics.counter import Counter, Counters
from PhysicsTools.Heppy.physicsutils.JetReCalibrator import Type1METCorrector, setFakeRawMETOnOldMiniAODs
import PhysicsTools.HeppyCore.framework.config as cfg

import copy
import ROOT
from math import hypot

from copy import deepcopy



from PhysicsTools.PatUtils.tools.runMETCorrectionsAndUncertainties import runMetCorAndUncFromMiniAOD


class METEEModified( Analyzer ):
    def __init__(self, cfg_ana, cfg_comp, looperName ):
        super(METEEModified,self).__init__(cfg_ana,cfg_comp,looperName)

#        self.recalibrateMET   = cfg_ana.recalibrate
#        self.applyJetSmearing = cfg_comp.isMC and cfg_ana.applyJetSmearing
#        self.old74XMiniAODs         = cfg_ana.old74XMiniAODs
#        self.jetAnalyzerPostFix = getattr(cfg_ana, 'jetAnalyzerPostFix', '')
#        if self.recalibrateMET in [ "type1", True ]:
#            if self.recalibrateMET == "type1":
#                self.type1METCorrector = Type1METCorrector(self.old74XMiniAODs)
#        elif self.recalibrateMET != False:
#            raise RuntimeError("Unsupported value %r for option 'recalibrate': allowed are True, False, 'type1'" % self.recalibrateMET)

    def declareHandles(self):
        super(METEEModified, self).declareHandles()
      #  self.handles['met'] = AutoHandle( self.cfg_ana.metCollection, 'std::vector<pat::MET>' )
     #   self.handles['metUncor'] = AutoHandle( self.cfg_ana.metCollection, 'std::vector<pat::MET>' )

    def beginLoop(self, setup):
        super(METEEModified,self).beginLoop(setup)
        self.counters.addCounter('events')
        count = self.counters.counter('events')
        count.register('all events')

    def process(self, event):

        import FWCore.ParameterSet.Config as cms
        process = cms.Process('corrMETs')

        process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')
        from Configuration.AlCa.GlobalTag import GlobalTag
        process.GlobalTag = GlobalTag( process.GlobalTag, "94X_dataRun2_ReReco_EOY17_v6")
        
        runMetCorAndUncFromMiniAOD (
            process,
            isData = not( self.cfg_comp.isMC ), # false for MC
            fixEE2017 = True,
            #            postfix = "",
            postfix = "ModifiedMET",
            )

        print "before proc"
        process.p = cms.Path( process.fullPatMetSequenceModifiedMET )

        return True


setattr(METEEModified,"defaultConfig", cfg.Analyzer(
    class_object = METEEModified,

#    metCollection     = "slimmedMETsModifiedMET",
#    noPUMetCollection = "slimmedMETsModifiedMET",
    )
)
