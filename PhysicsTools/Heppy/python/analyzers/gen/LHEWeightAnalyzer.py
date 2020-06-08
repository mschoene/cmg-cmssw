from PhysicsTools.Heppy.analyzers.core.Analyzer import Analyzer
from PhysicsTools.Heppy.analyzers.core.AutoHandle import AutoHandle
import PhysicsTools.HeppyCore.framework.config as cfg
from DataFormats.FWLite import Handle,Lumis
from ROOT.gen import WeightsInfo

class LHEWeightAnalyzer( Analyzer ):
    """Read the WeightsInfo objects of the LHE branch and store them
       in event.LHE_weights list.

       If the WeightsInfo.id is a string, replace it with an integer.

       So far the only allowed string formats are "mg_reweight_X",
       which gets stored as str(10000+int(X)),
       and "rwgt_X",
       which gets stored as str(20000+int(X)),

       If w.id is an unknown string or anything but a string or int,
       a RuntimeError is raised.
    """
    def __init__(self, cfg_ana, cfg_comp, looperName ):
        super(LHEWeightAnalyzer,self).__init__(cfg_ana,cfg_comp,looperName)
        self.savePSweights = getattr(self.cfg_ana,"savePSweights",False)
        self.LHEWeightsNames=[]

    def declareHandles(self):
        super(LHEWeightAnalyzer, self).declareHandles()

#        if self.doPDFVars or self.doPDFWeights:
#            self.mchandles['pdfstuff'] = AutoHandle( 'generator', 'GenEventInfoProduct' )

        self.mchandles['LHEweights'] = AutoHandle('externalLHEProducer',
                                                  'LHEEventProduct',
                                                  mayFail=True,
                                                  fallbackLabel='source',
                                                  lazy=False )
        if self.cfg_ana.useLumiInfo:
            self.genLumiHandle = Handle("GenLumiInfoHeader")
            if self.savePSweights: raise RuntimeError, 'this combination of options in LHEWeightAnalyzer is not supported'

        if self.cfg_ana.useLumiInfo or self.savePSweights:
            self.mchandles['GenInfo'] = AutoHandle('generator',
                                                    'GenEventInfoProduct',
                                                    mayFail=True,
                                                    fallbackLabel='source',
                                                    lazy=False )
        if self.cfg_ana.doFastSim:
            self.mchandles['GenInfo'] = AutoHandle( ('generator','',''), 'GenEventInfoProduct' )

    def beginLoop(self, setup):
        super(LHEWeightAnalyzer,self).beginLoop(setup)
        
        if self.cfg_ana.useLumiInfo:
            lumis = Lumis(self.cfg_comp.files)
            for lumi in lumis:
                if lumi.getByLabel('generator',self.genLumiHandle):
                    weightNames = self.genLumiHandle.product().weightNames()
                    for wn in weightNames:  #direct cast is not working properly, copy of elements is needed
                        self.LHEWeightsNames.append(wn)
                    break




    def process(self, event):
        self.readCollections( event.input )
        
        # if not MC, nothing to do
        if not self.cfg_comp.isMC:
            return True

        # Add LHE weight info
        event.LHE_weights = []
        event.LHE_originalWeight = 1.0


        if self.cfg_ana.doFastSim:
#            if self.mchandles['GenInfo'].isValid():
            event.LHE_originalWeight = self.mchandles['GenInfo'].product().weights()[1]
            for cnt,w in enumerate(self.mchandles['GenInfo'].product().weights()):
                # print cnt,w
                if cnt>0:
                    newweight = WeightsInfo()
                    newweight.id = str(1000+cnt)
                    newweight.wgt = w
                    event.LHE_weights.append(newweight)
#                event.LHE_weights.append(w)

        

        if self.mchandles['LHEweights'].isValid() and not self.cfg_ana.useLumiInfo and not self.cfg_ana.doFastSim:
            print "using external LHEweights, which are valid"
            event.LHE_originalWeight = self.mchandles['LHEweights'].product().originalXWGTUP()

            for w in self.mchandles['LHEweights'].product().weights():
                # Check if id is string or int and convert to int if it's a string
                try:
                    int(w.id)
                    event.LHE_weights.append(w)
                except ValueError:
                    if not type(w.id) == str:
                        raise RuntimeError('Non int or string type for LHE weight id')

                    newweight = WeightsInfo()
                    newweight.wgt = w.wgt
                    if w.id.startswith('mg_reweight'):
                        newid = str(10000 + int(w.id.rsplit('_',1)[1]))
                        newweight.id = newid
                    elif w.id.startswith('rwgt'):
                        newid = str(20000 + int(w.id.rsplit('_',1)[1]))
                        newweight.id = newid

                    else: raise RuntimeError('Unknown string id in LHE weights')
                    event.LHE_weights.append(newweight)

            if self.savePSweights and self.mchandles['GenInfos'].isValid():
                for cnt,w in enumerate(self.mchandles['GenInfos'].product().weights()):
                    newweight = WeightsInfo()
                    newweight.id = str(30000+cnt)
                    newweight.wgt = w
                    event.LHE_weights.append(newweight)

        if self.cfg_ana.useLumiInfo and self.mchandles['GenInfos'].isValid() :          
            print "using LumiInfo, which are valid"
            for cnt,w in enumerate(self.mchandles['GenInfos'].product().weights()[1:10]):
                weight= WeightsInfo()
                weight.wgt=w
                idstr=self.LHEWeightsNames[cnt+1].split(',')[1]
                weight.id=str(10000 + int(idstr[6:]) )
                event.LHE_weights.append(weight)
                
        return True

setattr(LHEWeightAnalyzer,"defaultConfig",
    cfg.Analyzer(LHEWeightAnalyzer,
#                 useLumiInfo = True,
                 useLumiInfo = False,
                 doFastSim = False,
    )
)








# from PhysicsTools.Heppy.analyzers.core.Analyzer import Analyzer
# from PhysicsTools.Heppy.analyzers.core.AutoHandle import AutoHandle
# import PhysicsTools.HeppyCore.framework.config as cfg
# from DataFormats.FWLite import Handle, Lumis, Runs
# from ROOT.gen import WeightsInfo


# class LHEWeightAnalyzer( Analyzer ):
#     """Read the WeightsInfo objects of the LHE branch and store them
#        in event.LHE_weights list.

#        If the WeightsInfo.id is a string, replace it with an integer.

#        So far the only allowed string formats are "mg_reweight_X",
#        which gets stored as str(10000+int(X)),
#        and "rwgt_X",
#        which gets stored as str(20000+int(X)),

#        If w.id is an unknown string or anything but a string or int,
#        a RuntimeError is raised.
#     """
#     def __init__(self, cfg_ana, cfg_comp, looperName ):
#         super(LHEWeightAnalyzer,self).__init__(cfg_ana,cfg_comp,looperName)
#         self.savePSweights = getattr(self.cfg_ana,"savePSweights",False)
#         self.LHEWeightsNames=[]

#     def declareHandles(self):
#         super(LHEWeightAnalyzer, self).declareHandles()


# #        self.mchandles['LHEweights'] = AutoHandle('generator',

# #        self.mchandles['LHEweights'] = AutoHandle('externalLHEProducer',
# #                                                  'GenEventInfoProduct',
# #                                                  'LHEEventProduct',
# #                                                  mayFail=True,
# #                                                  fallbackLabel='source',
# #                                                  lazy=False )
        
# #        if self.cfg_ana.useLumiInfo:
# #        self.genLumiHandle = Handle("GenLumiInfoHeader")

#         self.mchandles['lhe'] = AutoHandle( 'generator', 
#                                             'GenLumiInfoHeader', 
#                                             mayFail = True, 
#                                             lazy = False )

#            # self.genLumiHandleTest = AutoHandle('generator',
#             #                                        'GenLumiInfoHeader',
#             #                                        mayFail=True,
#              #                                       fallbackLabel='source',
#              #                                       lazy=False )

# #           self.genLumiHandle = Handle("GenLumiInfoHeader") #this was in
#         #if self.savePSweights: raise RuntimeError, 'this combination of options in LHEWeightAnalyzer is not supported'
            
#  #           print self.genLumiHandle

#         #        if self.cfg_ana.useLumiInfo or self.savePSweights:
#         self.mchandles['GenInfos'] = AutoHandle('generator',
#                                                 'GenEventInfoProduct',
#                                                 mayFail=True,
#                                                 fallbackLabel='source',
#                                                 lazy=False )
#         #        print "self info", self.mchandles['GenInfos']

# #        self.mchandles['GenInfo'] = AutoHandle( ('generator','',''), 'GenEventInfoProduct' )
#         self.genLumiHandle = Handle("GenLumiInfoHeader")


#     def beginLoop(self, setup):
#         super(LHEWeightAnalyzer,self).beginLoop(setup) 

# #        if self.cfg_ana.doLHE and self.cfg_ana.useLumiInfo:
# #        lumis = Lumis(self.cfg_comp.files)
# #        for lumi in lumis:
# #            if lumi.getByLabel('generator',self.genLumiHandle):
# #                self.LHEInfos.append( self.genLumiHandle.product().configDescription() )


#    #    print "begin loop" 
#     #    super(LHEWeightAnalyzer,self).beginLoop(setup)
#      #   print "super ", super(LHEWeightAnalyzer,self).beginLoop(setup)
        
# #        if self.cfg_ana.useLumiInfo:


#       #  self.run = Runs(self.cfg_comp.files[0])
#        # runsHandle = Handle('LHERunInfoProduct')
# #       # print self.run.getByLabel('externalLHEProducer', runsHandle)
#        # self.run.getByLabel('externalLHEProducer', runsHandle)
    

#         lumis = Lumis(self.cfg_comp.files)
#         print "lumis ", lumis
#         for lumi in lumis:
#             print lumi
#             if lumi.getByLabel('generator',self.genLumiHandle):
#                 self.LHEInfos.append( self.genLumiHandle.product().configDescription() )
                
# #             print "for ", lumi, "in lumis ", lumis
# #                 #print lumi.getByLabel('generator',self.mchandles)

# # #            print self.genLumiHandle

# # #            print lumi.getByLabel('generator',self.genLumiHandle) #MS this is false


# #             #print " validity ", self.genLumiHandle.product().configDescription()
# #             if not self.mchandles['lhe'].isValid():
# #                 if not hasattr(self,"warned_already"):
# #                     print "ERROR: Missing LHE header in file"
# #                     self.warned_already = True
# #             #return
# #                     lheprod = self.mchandles['lhe'].configDescription();#product()

# #             #                print lumi.getByLabel('generator',self.genLumiHandleTest) #MS this is false
# # #                print self.genLumiHandleTest.isValid()
# # #               print lumi.genLumiHandleTest #MS this is false
# # #            print self.genLumiHandle.product().weightNames()
# # #            print self.genLumiHandle.product().weightNames().size()
# # #            print self.genLumiHandle.product().weightNames().length()

# #            print self.genLumiHandle.product().weightNames().data()
#              #                if lumi.getByLabel('generator',self.mchandles): #MS this is false
# #            if lumi.getByLabel('generator',self.genLumiHandle): #MS this is false
#             weightNames = self.genLumiHandle.product().weightNames()
#             print "first get names"
#             print self.genLumiHandle.product()
#             print self.genLumiHandle.product().weightNames()
#             print "weightnames    ", weightNames
#             for wn in weightNames:  #direct cast is not working properly, copy of elements is needed
#                 print "weight = ",wn 
#                 self.LHEWeightsNames.append(wn)
#             #                 break
            
#     def process(self, event):
#         self.readCollections( event.input )
        
#         if not self.cfg_comp.isMC:
#             return True

#         # Add LHE weight info
#         event.LHE_weights = []
#         event.LHE_originalWeight = 1.0
        
# #        print self.mchandles['LHEweights']
# #        print self.mchandles['LHEweights'].isValid()

#         # if self.mchandles['LHEweights'].isValid() and not self.cfg_ana.useLumiInfo:

#         #     event.LHE_originalWeight = self.mchandles['LHEweights'].product().originalXWGTUP()
#         #     #event.LHE_originalWeight = self.mchandles['LHEweights'].product().weights()

#         #     for w in self.mchandles['LHEweights'].product().weights():
#         #         # Check if id is string or int and convert to int if it's a string
#         #         print "weight ID"
#         #         print w.pdf.id()

#         #         print w.id
#         #         #print "inv version", ind(w.id)
#         #         try:
#         #             int(w.id)
#         #             event.LHE_weights.append(w)
#         #         except ValueError:
#         #             if not type(w.id) == str:
#         #                 raise RuntimeError('Non int or string type for LHE weight id')

#         #             newweight = WeightsInfo()
#         #             newweight.wgt = w.wgt

#         #             print newweight
#         #             print newweight.wgt

#         #             if w.id.startswith('mg_reweight'):
#         #                 newid = str(10000 + int(w.id.rsplit('_',1)[1]))
#         #                 newweight.id = newid
#         #             elif w.id.startswith('rwgt'):
#         #                 newid = str(20000 + int(w.id.rsplit('_',1)[1]))
#         #                 newweight.id = newid

#         #             else: raise RuntimeError('Unknown string id in LHE weights')
#         #             event.LHE_weights.append(newweight)

#         #     if self.savePSweights and self.mchandles['GenInfos'].isValid():
#         #         for cnt,w in enumerate(self.mchandles['GenInfos'].product().weights()):
#         #             newweight = WeightsInfo()
#         #             newweight.id = str(30000+cnt)
#         #             newweight.wgt = w
#         #             event.LHE_weights.append(newweight)

# #        if self.cfg_ana.useLumiInfo:# and self.mchandles['GenInfos'].isValid() :          
# #        if self.cfg_ana.useLumiInfo and self.mchandles['GenInfos'].isValid() :          
#         for cnt,w in enumerate(self.mchandles['GenInfos'].product().weights()[1:10]):
#             weight= WeightsInfo()
#             weight.wgt=w
            
#             print "weight  ", weight
#             print "w       ", w

#             print self.LHEWeightsNames
#             print self.LHEWeightsNames[cnt]
#             print self.LHEWeightsNames[cnt+1]
#             idstr=self.LHEWeightsNames[cnt+1].split(',')[1]

#             print idstr

# #            weight.id=str(10000 + int(idstr[6:]) )
            
# #                print "weight  ",weight
# #                print "w       ", w
# #                print self.mchandles['GenInfos'].product().weights()[1:10]
# #                print self.mchandles['GenInfos'].product().weights()[1:100]

# #                print "weight.id  ",weight.id
# #                print self.LHEWeightsNames
# #                print self.LHEWeightsNames[cnt]

#  #               idstr=self.LHEWeightsNames[cnt+1].split(',')[1]
#  #               weight.id=str(10000 + int(idstr[6:]) )
#             event.LHE_weights.append(weight)
                
#             return True

# setattr(LHEWeightAnalyzer,"defaultConfig",
#     cfg.Analyzer(LHEWeightAnalyzer,
#                  useLumiInfo = True,
# #                 useLumiInfo = False,
#     )
# )
