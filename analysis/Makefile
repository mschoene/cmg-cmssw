CC = g++
CFLAGS = -Wall -c -g -fpermissive

ROOFIT_INCLUDE := $(shell cd $(CMSSW_BASE); scram tool info roofitcore | grep INCLUDE= | sed 's|INCLUDE=||')
ROOFIT_LIBDIR := $(shell cd $(CMSSW_BASE); scram tool info roofitcore | grep LIBDIR= | sed 's|LIBDIR=||')

INCLUDES = -I. -I.. -I$(ROOTSYS)/include -I$(ROOFIT_INCLUDE)/ -I$(CMSSW_BASE)/src -I$(CMSSW_BASE)/src/CommonTools -I$(CMSSW_BASE)/src/UserCode/mmasciov/ -I$(CMSSW_BASE)/src/CondFormats/JetMETObjects/interface 

ROOTSYS ?= ERROR_RootSysIsNotDefined

ROOTFLAG = `${ROOTSYS}/bin/root-config --cflags --libs`

EXTRALIBS := -L$(ROOTSYS)/lib -L$(ROOFIT_LIBDIR)/ -L$(CMSSW_BASE)/lib/slc6_amd64_gcc481/ -L/swshare/cms/slc6_amd64_gcc481/cms/cmssw/CMSSW_7_0_6_patch3/lib/slc6_amd64_gcc481/ -L/swshare/cms/slc6_amd64_gcc481/cms/cmssw-patch/CMSSW_7_0_6_patch3/lib/slc6_amd64_gcc481 -lHtml -lMathCore -lGenVector -lMinuit -lEG  -lPhysics -L/swshare/cms/slc6_amd64_gcc481/external/zlib/1.2.3/lib -lz -lTreePlayer




all: computeLostLepton_BabyTrees regionEventYields_BabyTrees regionEventYields_postBabyTrees inclusivePlots_BabyTrees inclusivePlots_QCD regionOptimization_BabyTrees

computeZinvFromZmumu: computeZinvFromZmumu.cpp MT2Region.o MT2Sample.o MT2EstimateSyst.o MT2Estimate.o
	$(CC) -Wall $(INCLUDES) -o computeZinvFromZmumu computeZinvFromZmumu.cpp MT2Region.o MT2Sample.o MT2EstimateSyst.o MT2Estimate.o $(ROOTFLAG) $(EXTRALIBS)

computeLostLepton_BabyTrees: computeLostLepton_BabyTrees.cpp MT2Region.o MT2LostLeptonUtilities.o MT2Sample.o
	$(CC) -Wall $(INCLUDES) -o computeLostLepton_BabyTrees computeLostLepton_BabyTrees.cpp MT2Region.o MT2LostLeptonUtilities.o MT2Sample.o $(ROOTFLAG) $(EXTRALIBS)

regionEventYields: regionEventYields.cpp MT2Region.o MT2Sample.o MT2EstimateSyst.o MT2Estimate.o
	$(CC) -Wall $(INCLUDES) -o regionEventYields regionEventYields.cpp MT2Region.o MT2Sample.o MT2EstimateSyst.o MT2Estimate.o $(ROOTFLAG) $(EXTRALIBS) -fpermissive

inclusivePlots_BabyTrees: inclusivePlots_BabyTrees.cpp MT2Region.o MT2RegionAnalysisUtilities.o MT2Sample.o
	$(CC) -Wall $(INCLUDES) -o inclusivePlots_BabyTrees inclusivePlots_BabyTrees.cpp MT2Region.o MT2RegionAnalysisUtilities.o MT2Sample.o $(ROOTFLAG) $(EXTRALIBS)

inclusivePlots_QCD: inclusivePlots_QCD.cpp MT2Region.o MT2RegionAnalysisUtilities.o MT2Sample.o
	$(CC) -Wall $(INCLUDES) -o inclusivePlots_QCD inclusivePlots_QCD.cpp MT2Region.o MT2RegionAnalysisUtilities.o MT2Sample.o $(ROOTFLAG) $(EXTRALIBS)

regionOptimization_BabyTrees: regionOptimization_BabyTrees.cpp MT2Region.o MT2RegionAnalysisUtilities.o MT2Sample.o
	$(CC) -Wall $(INCLUDES) -o regionOptimization_BabyTrees regionOptimization_BabyTrees.cpp MT2Region.o MT2RegionAnalysisUtilities.o MT2Sample.o $(ROOTFLAG) $(EXTRALIBS)

qgStudies: qgStudies.cpp localQGLikelihoodCalculator2.o DrawBase.o fitTools.o localQGLikelihoodCalculator.o
	$(CC) -Wall $(INCLUDES) -o qgStudies qgStudies.cpp localQGLikelihoodCalculator2.o DrawBase.o fitTools.o localQGLikelihoodCalculator.o  $(ROOTFLAG) $(EXTRALIBS)

checkQGL: checkQGL.cpp localQGLikelihoodCalculator2.o DrawBase.o fitTools.o localQGLikelihoodCalculator.o
	$(CC) -Wall $(INCLUDES) -o checkQGL checkQGL.cpp localQGLikelihoodCalculator2.o DrawBase.o fitTools.o localQGLikelihoodCalculator.o  $(ROOTFLAG) $(EXTRALIBS)

check_mht: check_mht.cpp 
	$(CC) -Wall $(INCLUDES) -o check_mht check_mht.cpp   $(ROOTFLAG) $(EXTRALIBS)

prova: prova.cpp MT2Region.o MT2Sample.o MT2Estimate.o MT2EstimateSyst.o
	$(CC) -Wall $(INCLUDES) -o prova prova.cpp MT2Region.o MT2Sample.o MT2Estimate.o MT2EstimateSyst.o $(ROOTFLAG) $(EXTRALIBS)

provaRead: provaRead.cpp MT2Region.o MT2Sample.o MT2Estimate.o MT2EstimateSyst.o
	$(CC) -Wall $(INCLUDES) -o provaRead provaRead.cpp MT2Region.o MT2Sample.o MT2Estimate.o MT2EstimateSyst.o $(ROOTFLAG) $(EXTRALIBS)






MT2Sample.o: ../src/MT2Sample.cc 
	$(CC) -Wall $(INCLUDES) -c ../src/MT2Sample.cc $(ROOTFLAG) $(EXTRALIBS)

MT2Region.o: ../src/MT2Region.cc
	$(CC) -Wall $(INCLUDES) -c ../src/MT2Region.cc $(ROOTFLAG) $(EXTRALIBS)

MT2Estimate.o: ../src/MT2Estimate.cc MT2Region.o
	$(CC) -Wall $(INCLUDES) -c ../src/MT2Estimate.cc MT2Region.o $(ROOTFLAG) $(EXTRALIBS)

MT2EstimateSyst.o: ../src/MT2EstimateSyst.cc MT2Region.o MT2Estimate.o
	$(CC) -Wall $(INCLUDES) -c ../src/MT2EstimateSyst.cc MT2Region.o MT2Estimate.o $(ROOTFLAG) $(EXTRALIBS)

MT2LostLeptonUtilities.o: ../src/MT2LostLeptonUtilities.cc MT2Region.o
	$(CC) -Wall $(INCLUDES) -c ../src/MT2LostLeptonUtilities.cc MT2Region.o $(ROOTFLAG) $(EXTRALIBS)

MT2RegionAnalysisUtilities.o: ../src/MT2RegionAnalysisUtilities.cc MT2Region.o
	$(CC) -Wall $(INCLUDES) -c ../src/MT2RegionAnalysisUtilities.cc MT2Region.o $(ROOTFLAG) $(EXTRALIBS)





DrawBase.o: $(CMSSW_BASE)/src/CommonTools/DrawBase.C
	$(CC) -Wall $(INCLUDES) -c $(CMSSW_BASE)/src/CommonTools/DrawBase.C $(ROOTFLAG) $(EXTRALIBS)

fitTools.o: $(CMSSW_BASE)/src/CommonTools/fitTools.C
	$(CC) -Wall $(INCLUDES) -c $(CMSSW_BASE)/src/CommonTools/fitTools.C $(ROOTFLAG) $(EXTRALIBS)


localQGLikelihoodCalculator2.o: $(CMSSW_BASE)/src/QGDev/qgMiniTuple/localQGLikelihoodCalculator/localQGLikelihoodCalculator2.cc
	$(CC) -Wall $(INCLUDES) -c $(CMSSW_BASE)/src/QGDev/qgMiniTuple/localQGLikelihoodCalculator/localQGLikelihoodCalculator2.cc $(ROOTFLAG) $(EXTRALIBS)

localQGLikelihoodCalculator.o: $(CMSSW_BASE)/src/QGDev/qgMiniTuple/localQGLikelihoodCalculator/localQGLikelihoodCalculator.cc
	$(CC) -Wall $(INCLUDES) -c $(CMSSW_BASE)/src/QGDev/qgMiniTuple/localQGLikelihoodCalculator/localQGLikelihoodCalculator.cc $(ROOTFLAG) $(EXTRALIBS)



clean:
	rm *.o
