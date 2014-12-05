
key=$1
speed=$2
Dvnet=$3
Dpnet=$4
Dtopo=$5


sudo rm fin/Finish_key${key}_speed${speed}.txt

sudo ./simulator $key $speed $Dvnet $Dpnet DMT $Dtopo 
sudo ./simulator $key $speed $Dvnet $Dpnet DMTe2e $Dtopo  
sudo ./simulator $key $speed $Dvnet $Dpnet DMTe3e $Dtopo  
sudo ./simulator $key $speed $Dvnet $Dpnet SMT $Dtopo  
sudo ./simulator $key $speed $Dvnet $Dpnet IDEAL $Dtopo  
sudo ./simulator $key $speed $Dvnet $Dpnet OVX $Dtopo  

sudo touch fin/Finish_key${key}_speed${speed}.txt
