
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
		sudo sh ./JobChild.sh $key $speed $Dvnet $Dpnet $Dtopo & 
	done
done

fi
