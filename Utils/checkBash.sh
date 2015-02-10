for i in $(ls /afs/cern.ch/user/m/mmasciov/CMSSW_7_2_3_PhysicsTools/src/CMGTools/TTHAnalysis/cfg/06Feb2015/); do 
    s="ZJetsToNuNu_HT600toInf"
    if [[ $i == $s* ]]
	then n=${i#*k}; Y=0; for m in $(ls /pnfs/psi.ch/cms/trivcat/store/user/mmasciov/PHYS14_production/06Feb2015/$s/mt2_*); do m=${m#*mt2_}; m=${m%.root}; if [[ $m == $n ]]; then Y=1; fi; done; if [[ $Y != 1 ]]; then echo $n; fi;
    fi
done;