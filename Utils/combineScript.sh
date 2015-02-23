for i in `ls -d $1/datacards_T*`; do echo $i;  x=`echo $i | awk 'BEGIN{FS="[_/]";OFS="_"}{print $7, $8, $9, $10, $5}'`; combineCards.py $i/datacard_HT*txt > $i/datacard_combine.txt; combine -M Asymptotic $i/datacard_combine.txt | tee $i/limit.log; done
rm -r roostats*
