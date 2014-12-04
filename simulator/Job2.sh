
if [ $# -ne 1 ]; then
	echo need to specify "fattree" or "jellyfish"
	exit
else

Dkey=64
Dspeed=128
Dvnet=80
Dpnet=980
Dtopo=../TopoGen/$1.txt

for speed in 1 2 4 8 16 32 64 128 256
#for speed in 1 
do 
	for key in 64 128 256
	#for key in 64 
	do
		sudo ./simulator $key $speed $Dvnet $Dpnet DMT $Dtopo &
		sudo ./simulator $key $speed $Dvnet $Dpnet DMTe2e $Dtopo & 
		sudo ./simulator $key $speed $Dvnet $Dpnet DMTe3e $Dtopo & 
		sudo ./simulator $key $speed $Dvnet $Dpnet SMT $Dtopo & 
		sudo ./simulator $key $speed $Dvnet $Dpnet IDEAL $Dtopo & 
		sudo ./simulator $key $speed $Dvnet $Dpnet OVX $Dtopo & 
	done
done

fi
