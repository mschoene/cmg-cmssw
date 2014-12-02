#!/bin/env python 

import os
import sys
import getopt

from copyToSE import *

def main(argv):
    
    try:
        opts, args = getopt.getopt(argv, "h", ["help"])
        
    except getopt.GetoptError:
        print 'Unknown option! \nUsage: python copyBabyTreesToSE <inputPath> <sePath>'
        sys.exit()
        
    for opt in opts:
        if opt == '-h':
            print 'Help: python copyBabyTreesToSE <inputPath> <sePath>'
            sys.exit()

    if len(args) < 2:
        print 'Need at least two arguments...\nUsage: python copyBabyTreesToSE <inputPath> <sePath>'
        sys.exit()

    elif len(args) > 2:
        print 'Too many arguments...\nUsage: python copyBabyTreesToSE <inputPath> <sePath>'
        sys.exit()

    localDirPost = args[0]
    seDirPost = args[1]
    
    #localDirPost = '/scratch/mmasciov/endNovember/PostProcess/'                                                #seDirPost = 'mmasciov/POSTPROCESSING/endNOV/'

    #print args

    print '\nCAVEAT: did you initialize you VOMS credentials? If not, please do it now!\n'

    copyToSE(localDirPost, seDirPost)

if __name__ == "__main__":
    main(sys.argv[1:])
