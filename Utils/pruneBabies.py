#! /usr/bin/python

from ROOT import TFile, TTree




def pruneBaby( fname, dir, prunedir, pruneMap ):

  fullname = dir+"/"+fname
  file = TFile(fullname)
  tree = file.Get("mt2")

  doLoop = any("kill" not in s for s in pruneMap.values())

  print "-> Pruning " + fullname
  if doLoop:
    print "-> (Will need to do loop because of the presence of selection cuts - might take a while)"

  
  if doLoop:

    #for i in pruneMap:
    #  if pruneMap[i] == "kill":
    #    tree.SetBranchStatus( i+"_*", 0 )
    #    tree.SetBranchStatus( "n"+i, 0 )
    #else:
    #  cuts = prunMap.split("&&")
    #  for icut in cuts:
    #    if "==" in icut:
    print "wrong"

  else :

    for i in pruneMap:
      tree.SetBranchStatus( i+"_*", 0 )
      tree.SetBranchStatus( "n"+i, 0 )

    newfilename = fname.split(".root")[0] + "_prune.root"
    newfile = TFile(prunedir+"/"+newfilename, "recreate")
    newtree = tree.CloneTree()
    newtree.Write("", 5)
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
   if len(args)==0:
     print "ERROR! Script needs at least one argument to run:"
     print "   python skimBabies.py [input_directory] [output_subdirectory=\"noGenPart\"] [configFile=\"pruneBranches.txt\"]"
     exit()

   dir = args[0]
   subdir = "noGenPart"
   if len(args)>1 : subdir = args[1]
   configFile = "pruneBranches.txt"
   if len(args)>2 : configFile = args[2]

   if options.filter !="" :
     print "-> Skimming only files containing: " + str(options.filter)


   print "-> Opening pruning config file: " + configFile
   f = open(configFile, 'r')

   pruneMap = dict()
   doLoop = False

   for i in f:
     if i.startswith("#"): 
       continue
     thisRule = i.split()
     if len(thisRule)==1:
       pruneMap[thisRule[0]] = "kill"
     elif len(thisRule)==2:
       pruneMap[thisRule[0]] = thisRule[1]
     else :
       selection = thisRule[1]
       for i in range(2,len(thisRule)):
         selection += thisRule[i]
       pruneMap[thisRule[0]] = selection
       #print "Invalid config file! The file format needs to be:"
       #print "[branch_prefix] [prune_selection]"
       #print "where 'branch_prefix' is the name of the collection you want to prune (eg. jet, genPart...)"
       #print "and 'prune_selection' can be a set of cuts, or can be left blank to kill the whole branches"
       #exit()


   prunedir = dir + "/" + subdir
   os.system("mkdir -p " + prunedir)

   files = os.listdir(dir)

   for f in files:
     if ".root" in f:
       if options.filter in f:
         pruneBaby(f, dir, prunedir, pruneMap)

   print "Find pruned babies in " + prunedir

