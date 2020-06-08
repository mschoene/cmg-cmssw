import ROOT
import os.path
#ROOT.gSystem.Load("libEgammaAnalysisElectronTools")

ROOT.gSystem.Load("libRecoEgammaEgammaTools")
#RecoEgamma/EgammaTools/src/PhotonEnergyCalibrator.cc


class Run2PhotonCalibrator:
    def __init__(self, corrFile ):
        #    def __init__(self, data, isMC, isSync=False):
        self.random = ROOT.TRandom3()
        self.random.SetSeed(0) # make it really random across different jobs

        self.photonEnergyCalibratorRun2 =  ROOT.PhotonEnergyCalibrator( corrFile )

        self.photonEnergyCalibratorRun2.initPrivateRng( self.random )

    def correct(self,photon,run,isMC):
        photon.uncalibratedP4 = photon.p4(photon.getCandidateP4type())
        photon.uncalibratedP4Error = photon.getCorrectedEnergyError(photon.getCandidateP4type())

#        seedRecHit = photon.recHits.find(seedDetId);
#        print seedRecHit
#        self.photonEnergyCalibratorRun2.calibrate(photon.physObj, int(run), seedId , 1, isMC )

        self.photonEnergyCalibratorRun2.calibrate(photon.physObj, int(run), photon.recHits(), isMC )
#        self.photonEnergyCalibratorRun2.calibrate(photon.physObj, int(run), photon.recHits(), 0, isMC )
#        self.photonEnergyCalibratorRun2.calibrate(photon.physObj, int(run), photon, photon.embedRecHits, 1, isMC )
        

        return True

#include "DataFormats/EcalRecHit/interface/EcalRecHit.h"
#../../../PhysicsTools/PatAlgos/plugins/PATPhotonProducer.h:#include "DataFormats/EcalRecHit/interface/EcalRecHitCollections.h"



# class Run2PhotonCalibrator:
#     def __init__(self, isMC, isSync=False):
# #    def __init__(self, data, isMC, isSync=False):
#         self.random = ROOT.TRandom3()
#         self.random.SetSeed(0) # make it really random across different jobs

#         passing = bool(isMC)
#         self.photonEnergyCalibratorRun2 = ROOT.PhotonEnergyCalibratorRun2( )  #isMC, isSync )#, str(data) )
#         #        self.photonEnergyCalibratorRun2 = ROOT.PhotonEnergyCalibratorRun2( bool(isMC) )  #isMC, isSync )#, str(data) )
#         self.photonEnergyCalibratorRun2.initPrivateRng(self.random)

#     def correct(self,photon,run):
#         photon.uncalibratedP4 = photon.p4(photon.getCandidateP4type())
#         photon.uncalibratedP4Error = photon.getCorrectedEnergyError(photon.getCandidateP4type())
#         self.photonEnergyCalibratorRun2.calibrate(photon.physObj, int(run))
#         return True

