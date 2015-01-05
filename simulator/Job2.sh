
if [ $# -ne 1 ]; then
	echo need to specify "fattree" or "jellyfish" or "NO_TOPO"
	exit
else

Dkey=64
Dspeed=128
Dvnet=80
Dpnet=980

if [ $1 = 'NO_TOPO' ]; then
	Dtopo=$1
else
	Dtopo=../TopoGen/$1.txt
fi
echo Dtopo is $Dtopo

sudo rm Result/*.txt

#for speed in 1 2 4 8 16 32 64 128 256
for speed in 1 2 4 8 16 32 64 128
#for speed  in 8
#for speed in 1 2 4 
do 
	#for key in 8 16 32 64 128 256
	for key in 8 16 32 64 128
	#for key in 64
	do
		if [ $Dtopo = 'NO_TOPO' ]; then
			for pnet in 320 160 80 40 20
			do
				sudo sh ./JobChild.sh $key $speed $Dvnet $pnet $Dtopo & 
			done
		else
			sudo sh ./JobChild.sh $key $speed $Dvnet $Dpnet $Dtopo & 
		fi
	done
done

fi
