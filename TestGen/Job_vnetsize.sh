
Dkey=64
Dspeed=16
Dvnet=160
Dpnet=980

	for vnet in 8 16 32 64 128 
	do
		sudo ./TestGen $Dkey $Dspeed $vnet $Dpnet 
	done
