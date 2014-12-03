#!/bin/bash

if [ "$#" -ne 1 ]; then
    echo "=== ERROR ==="
    echo "You need to provide one single argument to the script, ie the name of the input configuration file"
    exit;
fi

if [ ! -f $1 ]; then
    echo "File "$1 "not found! Quitting..."
    exit;
fi


rand1=$RANDOM
rand2=$RANDOM
rand3=$RANDOM
rand4=$RANDOM

echo "Reading config file"$1"..." 
configfile=$1
configfile_secured=$rand1.cfg

# check if the file contains something we don't want
if egrep -q -v '^#|^[^ ]*=[^;]*' "$configfile"; then
  echo "Config file is unclean, cleaning it..." 
  # filter the original to a new file
  egrep '^#|^[^ ]*=[^;&]*'  "$configfile" > "$configfile_secured"
  configfile="$configfile_secured"
fi

source $configfile; rm -f $rand1.cfg;
echo "Following configuration will be used:"  
echo "====================================="  
echo "doSkimming"=$doSkimming 
echo "doPruning"=$doPruning 
echo "inputDir"=$inputDir 
echo "inputFilter"=$inputFilter 
echo "outputDir"=$outputDir
echo "skimmingSelection"=$skimmingSelection
echo "branchesToPrune"=$branchesToPrune
echo "====================================="  
echo "" 


if [ -d $outputDir ]; then
    echo "Output directory "$outputDir" already exists. Quitting...";
    exit;
fi


outputSkimming=dirOutSkimming_$rand2
if [ "$doSkimming" = true ]; then
    echo "Running skimming... "
    mkdir $outputSkimming
    python skimBabies.py $inputDir $outputSkimming "$skimmingSelection"  --filter="$inputFilter"
fi

outputPruning=dirOutPruning_$rand3
if [ "$doPruning" = true ]; then
    if [ "$doSkimming" = true ]; then
      inputDir=$outputSkimming
      inputFilter=${inputFilter/'.root'}
    fi
    python pruneBabies.py $inputDir $outputPruning "$branchesToPrune"  --filter="$inputFilter"
fi

if [[ "$doSkimming" = true && ! "$doPruning" = true ]]; then
    echo "cleaning/moving temp folders...";
    mv $outputSkimming $outputDir;
elif [[ ! "$doSkimming" = true &&  "$doPruning" = true ]]; then
    echo "cleaning/moving temp folders...";
    mv $outputPruning $outputDir;
elif [[ "$doSkimming" = true &&  "$doPruning" = true ]]; then
    echo "cleaning/moving temp folders...";
    mv $outputPruning $outputDir;
    rm -r $outputSkimming;
fi

echo "Find your files in: " $outputDir
