#!/bin/bash

if [ "$#" -ne 1 ]; then
    echo "=== ERROR ==="
    echo "You need to provide one single argument to the script, ie the name of the input root file"
    exit;
fi


rand1=$RANDOM
rand2=$RANDOM
rand3=$RANDOM
rand4=$RANDOM
rand5=$RANDOM


# --- Here I define the root script that dump the size of all branches
cat <<EOF > rootMacro_$rand1.C
#include "TFile.h"
#include "TTree.h"
#include "TBranch.h"

#include <string>
#include <iostream>
#include <fstream>
#include <cstdlib>


using namespace std;
void rootMacro_${rand1}(string inputFileName="mt2_tree.root"){
  TFile *inputFile = TFile::Open(inputFileName.c_str());
  TTree* tree = (TTree*)inputFile->Get("mt2");

  TBranch *branch1 = 0;
  TObjArray *fBranches = tree->GetListOfBranches();

  ofstream outputTable;
  outputTable.open("outputTable.$rand2.txt");
  for(Int_t i=0; i<fBranches->GetEntries(); i++) {
    branch1 = (TBranch*) fBranches->At(i);
    Long64_t sizeBranch = branch1->GetZipBytes("*");
    outputTable << setiosflags(ios::left) << setw(30) << branch1->GetName() << " , "
		<< sizeBranch << endl; //size in byte
  }
  outputTable.close();
}
EOF
root.exe -b -q -l rootMacro_${rand1}.C\(\"$1\"\)


# --- parsing of the txt file containing the size of each branch
IFS=$'\n' 
cat outputTable.$rand2.txt |awk '{print "#"$0}'> outputTable.prefix.$rand3.txt

list=($(cat outputTable.prefix.$rand3.txt |grep _ |sort| awk -F_ '{print $1}'| sort |uniq))
totSize=($(cat outputTable.$rand2.txt | awk  '{sum+=$3} END {print sum}'))


rm -f outputTableFormatted.$rand4.txt
for i in $(seq ${#list[*]}); do
    string=${list[$i]}_
    size=`cat outputTable.prefix.$rand3.txt |grep "$string" | awk '{sum+=$3} END {print sum}'`
    frac=`echo "$size/$totSize*100"|bc -l`
    echo -e "${list[$i]} "$frac >> outputTableFormatted.$rand4.txt;
done
cat outputTableFormatted.$rand4.txt |awk '{printf "%4.2f %s %30s\n", $2,"%", $1}'| \
    sort -n -r|grep -v "0.00" > outputTableFormatted.final.$rand5.txt


# --- Here is the final output
echo "===============================";
cat outputTableFormatted.final.$rand5.txt |sed s/#//
echo "=== all above add up to: "; cat outputTableFormatted.final.$rand5.txt |awk '{sum+=$1} END {print sum" %"}';

#cleaning
rm rootMacro_$rand1.C
rm outputTable.$rand2.txt
rm outputTable.prefix.$rand3.txt
rm outputTableFormatted.$rand4.txt
rm outputTableFormatted.final.$rand5.txt
