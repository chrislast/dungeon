#!/bin/bash -eu
#
# for filename in *.bmp; 
#	do printf "\nconst unsigned char ${filename/.bmp}[] = { %s0xFF};\n" 
#			"$(
#				for val in $(od -tx1 -An -v $filename); do 
#				printf "0x$val, ";
#				done)";
#	done > bmp.txt

if [ ! -r "$1" -o ! -r "$2" -o -z "$3" ]; then
	echo "usage: $0 <bitmap_file1> <bitmap_file2> <output_file>"
	exit 1
fi

rm -f "$3"

length=$(($(wc -c < $1)))
length2=$(($(wc -c < $2)))

if [ $length -ne $length2 ]; then
	echo "bitmap file sizes are not identical"
	exit 2
fi

printf "combining %s and %s into %s" "$1" "$2" "$3"

bmp1=($(eval echo $(od -tu1 -An -v "$1")))
bmp2=($(eval echo $(od -tu1 -An -v "$2")))
bitmap_data_start=$((bmp1[10]+(bmp1[11]+(bmp1[12]+(bmp1[13]<<4)<<4)<<4)))

for i in $(seq 0 $((length-1))); do
	c1=${bmp1[$i]}
	c2=${bmp2[$i]}
	c3=$(( (c1&0xCC) | ((c2&0xCC)>>2)))
	if [ $i -lt $bitmap_data_start ]; then
		c4=$(printf %x ${bmp1[$i]})
	else
		c4=$(printf %x $c3)
	fi
	printf "\x${c4}" >> $3
    printf "."
done


