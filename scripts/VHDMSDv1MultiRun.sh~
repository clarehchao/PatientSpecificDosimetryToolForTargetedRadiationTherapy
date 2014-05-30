#!/bin/bash
# TASK: execute VHDMSDv1 with appropriate input parameters to sumbit single/multiple simulations to a given linux machine
# Input arguments:
# $1: source organ name
# $2: first run #, run1
# $3: last run #, run2

nwdir=/home/clare
myg4dir=$nwdir/G4.9.6.p02work
runname="./VHDMSDv1"
rdirname=VoxelizedHumanDoseMultiSDv1-build
isReg=0
GEOdir=$nwdir/G4INPUT/GeometryIM/G4IM
SRCMPdir=$nwdir/G4INPUT/SourceMap
isSRCMPsparse=0
RUNdir=$myg4dir/$rdirname

GEOname=ufh00f_1
ORGANname=$1
SRCMPname=$GEOname/$ORGANname
run1=$2
run2=$3
PARname=I131
macfile=$RUNdir/macro/I131_Emphysics2.mac
isElectron=1
isPhoton=1
isroot=1

#make the appropriate directories for the simulation
dirtag="pCellFlux"

for (( n=$run1; n<=$run2; n++ ))
do
	datadirname=$RUNdir/data/GEO_$GEOname/SRCMP_$SRCMPname/$PARname/Run$n
	if [ "$isroot" -eq "1" ]
	then
		echo "isroot is 1!!"
		rootoutdir=$datadirname/BustOutRoot
		rootdatdir=$datadirname/RootData
		echo $rootoutdir
		echo $rootdatdir
		#mkdir -p $rootoutdir
		#mkdir -p $rootdatdir
	else
		for p in Edep 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28
		do
			if [ "$p" == "Edep" ]
			then
				dirname=$datadirname/Edep_MultiSD
			elif [ "$p" -lt "10" ]
			then
				dirname=$datadirname/$dirtag"0"$p
				echo $dirname
			else
				dirname=$datadirname/$dirtag$p
				echo $dirname
			fi
			mkdir -p $dirname
		done
	fi
 	$runname $isReg $GEOdir $GEOname $SRCMPdir $SRCMPname $isSRCMPsparse $datadirname $isElectron $isPhoton $isroot $macfile > $datadirname/log.txt
	echo "Finish VHDMSDv1 run #$n [$ORGANname]! good job bucko!"

	#if [ "$n" -ne "$run2" ]
	#then 
	#	echo "~~Give me a break, break me a piece of that kit-kat bar~~ 6-mins!"
        #	sleep 6m  #sleep/delay for 6 minutes
	#fi
done
