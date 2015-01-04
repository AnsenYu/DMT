
rm Touch/Data/*.json
rm Data/*.json
for speed in 1 2 4 8 16 32 64 128 256
#for speed in 1 
do 
	sh Job2.sh $speed &
done

