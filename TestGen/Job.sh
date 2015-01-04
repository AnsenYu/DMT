Dkey=64
Dspeed=128
Dvnet=80
Dpnet=980

rm Touch/Data/*
rm Data/*
for speed in 1 2 4 8 16 32 64 128 256
#for speed in 1 
do 
	for key in 8 16 32 64 128 
	#for key in 8 
	do
		sudo ./TestGen $key $speed $Dvnet $Dpnet &
	done
done

