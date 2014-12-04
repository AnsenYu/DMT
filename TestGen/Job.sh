Dkey=64
Dspeed=128
Dvnet=80
Dpnet=980

for speed in 1 2 4 8 16 32 64 128 256
do 
	for key in 64 128 256
	do
		sudo ./TestGen $key $speed $Dvnet $Dpnet &
	done
done

