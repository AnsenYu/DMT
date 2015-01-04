
Dkey=64
Dspeed=128
Dvnet=80
Dpnet=980

	for key in 8 16 32 64 128 
	do
		sudo ./TestGen $key $1 $Dvnet $Dpnet 
	done
