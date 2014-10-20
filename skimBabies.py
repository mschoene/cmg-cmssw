#! /usr/bin/python

from ROOT import TFile, TTree




def skimBaby( fname, dir, skimdir ):

  fullname = dir+"/"+fname
  file = TFile(fullname)
  tree = file.Get("mt2")

  print "-> Skimming " + fullname
  newfilename = fname.split(".root")[0] + "_skim.root"
  newfile = TFile(skimdir+"/"+newfilename, "recreate")
  newtree = tree.CopyTree("mt2>200. && ht>450.")
  newfile.cd()
  newtree.Write()
  newfile.Close()



if __name__ == '__main__':

   import os
   import sys

   from optparse import OptionParser

   parser = OptionParser()
   parser.usage = ""
   parser.add_option("-f","--filter", dest="filter",
                      default="",
                      help="skim only selected dataset")

   (options,args) = parser.parse_args()

   dir = args[0]

   if options.filter !="" :
     print "-> Skimming only files containing: " + str(options.filter)


   skimdir = dir + "/skim_mt2_200"
   os.system("mkdir -p " + skimdir)

   files = os.listdir(dir)

   for f in files:
     if ".root" in f:
       if options.filter in f:
         skimBaby(f, dir, skimdir)

   print "Find skimmed babies in " + skimdir

