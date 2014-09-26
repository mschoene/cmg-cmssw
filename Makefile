
CC = g++
CFLAGS = -Wall -c -g -fpermissive

ROOFIT_INCLUDE := $(shell cd $(CMSSW_BASE); scram tool info roofitcore | grep INCLUDE= | sed 's|INCLUDE=||')
ROOFIT_LIBDIR := $(shell cd $(CMSSW_BASE); scram tool info roofitcore | grep LIBDIR= | sed 's|LIBDIR=||')

INCLUDES = -I. -I$(ROOTSYS)/include -I$(ROOFIT_INCLUDE)/ -I$(CMSSW_BASE)/src -I$(CMSSW_BASE)/src/CommonTools -I$(CMSSW_BASE)/src/UserCode/mmasciov/ -I$(CMSSW_BASE)/src/CondFormats/JetMETObjects/interface 

ROOTSYS ?= ERROR_RootSysIsNotDefined

ROOTFLAG = `${ROOTSYS}/bin/root-config --cflags --libs`

EXTRALIBS := -L$(ROOTSYS)/lib -L$(ROOFIT_LIBDIR)/ -L$(CMSSW_BASE)/lib/slc6_amd64_gcc481/ -L/swshare/cms/slc6_amd64_gcc481/cms/cmssw/CMSSW_7_0_6_patch3/lib/slc6_amd64_gcc481/ -L/swshare/cms/slc6_amd64_gcc481/cms/cmssw-patch/CMSSW_7_0_6_patch3/lib/slc6_amd64_gcc481 -lHtml -lMathCore -lGenVector -lMinuit -lEG -lRooFitCore -lRooFit -lPhysics -L/swshare/cms/slc6_amd64_gcc481/external/zlib/1.2.3/lib -lz -lTreePlayer




all: regionEventYields_BabyTrees inclusivePlots_BabyTrees inclusivePlots_QCD regionEventYields_postBabyTrees regionOptimization_BabyTrees

regionEventYields_BabyTrees: regionEventYields_BabyTrees.cpp MT2Region.o MT2RegionAnalysisUtilities.o MT2Common.o
	$(CC) -Wall $(INCLUDES) -o regionEventYields_BabyTrees regionEventYields_BabyTrees.cpp MT2Region.o MT2RegionAnalysisUtilities.o MT2Common.o $(ROOTFLAG) $(EXTRALIBS)

inclusivePlots_BabyTrees: inclusivePlots_BabyTrees.cpp MT2Region.o MT2RegionAnalysisUtilities.o MT2Common.o
	$(CC) -Wall $(INCLUDES) -o inclusivePlots_BabyTrees inclusivePlots_BabyTrees.cpp MT2Region.o MT2RegionAnalysisUtilities.o MT2Common.o $(ROOTFLAG) $(EXTRALIBS)

inclusivePlots_QCD: inclusivePlots_QCD.cpp MT2Region.o MT2RegionAnalysisUtilities.o MT2Common.o
	$(CC) -Wall $(INCLUDES) -o inclusivePlots_QCD inclusivePlots_QCD.cpp MT2Region.o MT2RegionAnalysisUtilities.o MT2Common.o $(ROOTFLAG) $(EXTRALIBS)

regionEventYields_postBabyTrees: regionEventYields_postBabyTrees.cpp MT2Region.o MT2RegionAnalysisUtilities.o MT2Common.o
	$(CC) -Wall $(INCLUDES) -o regionEventYields_postBabyTrees regionEventYields_postBabyTrees.cpp MT2Region.o MT2RegionAnalysisUtilities.o MT2Common.o $(ROOTFLAG) $(EXTRALIBS) -fpermissive

regionOptimization_BabyTrees: regionOptimization_BabyTrees.cpp MT2Region.o MT2RegionAnalysisUtilities.o MT2Common.o
	$(CC) -Wall $(INCLUDES) -o regionOptimization_BabyTrees regionOptimization_BabyTrees.cpp MT2Region.o MT2RegionAnalysisUtilities.o MT2Common.o $(ROOTFLAG) $(EXTRALIBS)


MT2Common.o: src/MT2Common.cc
	$(CC) -Wall $(INCLUDES) -c src/MT2Common.cc $(ROOTFLAG) $(EXTRALIBS)

MT2Region.o: src/MT2Region.cc
	$(CC) -Wall $(INCLUDES) -c src/MT2Region.cc $(ROOTFLAG) $(EXTRALIBS)

MT2LostLeptonUtilities.o: src/MT2LostLeptonUtilities.cc MT2Region.o
	$(CC) -Wall $(INCLUDES) -c src/MT2LostLeptonUtilities.cc MT2Region.o $(ROOTFLAG) $(EXTRALIBS)

MT2RegionAnalysisUtilities.o: src/MT2RegionAnalysisUtilities.cc MT2Region.o
	$(CC) -Wall $(INCLUDES) -c src/MT2RegionAnalysisUtilities.cc MT2Region.o $(ROOTFLAG) $(EXTRALIBS)


DrawBase.o: $(CMSSW_BASE)/src/CommonTools/DrawBase.C
	$(CC) -Wall $(INCLUDES) -c $(CMSSW_BASE)/src/CommonTools/DrawBase.C $(ROOTFLAG) $(EXTRALIBS)

fitTools.o: $(CMSSW_BASE)/src/CommonTools/fitTools.C
	$(CC) -Wall $(INCLUDES) -c $(CMSSW_BASE)/src/CommonTools/fitTools.C $(ROOTFLAG) $(EXTRALIBS)



clean:
	rm *.o
