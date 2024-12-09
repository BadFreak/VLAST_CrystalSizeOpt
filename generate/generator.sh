#!/bin/sh
path="$PWD"
cd $path

particle=gamma
energy=5000
condor_out="$path/out/${particle}/${energy}MeV"
condor_err="$path/err/${particle}/${energy}MeV"
condor_log="$path/log/${particle}/${energy}MeV"
condor_sh="$path/run/${particle}/${energy}MeV"
condor_mac="$path/mac/${particle}/${energy}MeV"
condor_root="$path/root/${particle}/${energy}MeV"

mkdir -p $condor_out $condor_err $condor_log $condor_sh $condor_mac $condor_root
rm -f $condor_out/* $condor_err/* $condor_log/* $condor_sh/* $condor_mac/* 

JobNum=1
n=1
while [ "$n" -le "$JobNum" ]
do
	output_file=${n}
#--------Create mac file------------------------------
	echo "Create ${n} mac file"
	echo \
	"
	/control/verbose 0
	/run/verbose 0
	/event/verbose 0
	
	/mydet/generator fGParticleSource
	/gps/particle ${particle}
	/gps/energy ${energy} MeV
	/gps/direction 0 0 1
		
	/gps/pos/type Plane
	/gps/pos/shape Circle
	/gps/pos/radius 30 mm
	/gps/pos/centre 0 0 0 mm

	/run/beamOn 10000

	" > $condor_mac/event_${output_file}.mac
	
#--------Create .sh file end------------------------------
	echo \
	"#!/bin/sh
		#---- Write your command here------
	
	    ../build/VLASTCrystalSizeOpt $condor_mac/event_${output_file}.mac $condor_root/${output_file}.root
	
		#---- Write your command here------
	
	" > $condor_sh/${output_file}.sh
	
	source ${condor_sh}/${output_file}.sh > ${condor_log}/${output_file}.log &
	let "n+=1"
done


