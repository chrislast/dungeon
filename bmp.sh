#!/bin/bash -eux
#
# for filename in *.bmp; 
#	do printf "\nconst unsigned char ${filename/.bmp}[] = { %s0xFF};\n" 
#			"$(
#				for val in $(od -tx1 -An -v $filename); do 
#				printf "0x$val, ";
#				done)";
#	done > bmp.txt

bmp=($(eval echo $(od -tu1 -An -v "$1")))

size=${#bmp[@]}

# Show elements for debug
#for i in $(seq 0 $((size-1))); do printf "Element [%d]: %d\n" $i ${bmp[$i]}; done

# Check that we are looking at a bitmap file
if [ "${bmp[0]}" != "66" -o "${bmp[1]}" != "77" ]; then 
	echo $1 is not a bitmap file ${bmp[0]}${bmp[1]} && exit; 
fi
bitmap_data_start=$((${bmp[10]}+${bmp[11]}*256+${bmp[12]}*256*256+${bmp[13]}*256*256*256))
width=${bmp[18]}
height=${bmp[22]}
echo size=$size width=$width height=$height data@$bitmap_data_start bmp=${bmp[@]}

ypos=height
xpos=0
screenw=$(((width-1)/4+1))
screeny=$((ypos/8))
screenx=$((xpos+screenw*screeny))
threshold=9
mask=$((height%8))
mask=$((0x01 << $mask))
j=${bmp[10]}                  # byte 10 contains the offset where image data can be found
skip=(0 3 2 1)
declare -a Screen
 
loopc=$((width*height/2))
for i in $(seq 0 $loopc); do
	Screen[$((screenx+0))]=$((${Screen[$((screenx+0))]:-0}))
	Screen[$((screenx+1))]=$((${Screen[$((screenx+1))]:-0}))
	Screen[$((screenx+2))]=$((${Screen[$((screenx+2))]:-0}))
	Screen[$((screenx+3))]=$((${Screen[$((screenx+3))]:-0}))
	# the left pixel is in the upper 4 bits
	if [ $(((bmp[j]>>4)&0xF)) -gt $threshold ]; then
		Screen[$((screenx))]=$((Screen[screenx]|mask))
	fi
	screenx=$((screenx + 1))
	Screen[$screenx]=$((${Screen[$screenx]:-0}))
	# the right pixel is in the lower 4 bits
	if [ $((bmp[j]&0xF)) -gt $threshold ]; then
		Screen[$((screenx))]=$((Screen[screenx]|mask))
	fi
	screenx=$((screenx + 1))
    j=$((j + 1))
    if [ $((i%(width/2))) -eq 0 ]; then  #  at the end of a row
		if [ $mask -gt 1 ]; then
			mask=$((mask>>1))
		else
			mask=$((0x80))
			screeny=$((screeny - 1))
		fi
    fi
    screenx=$((width*screeny))
    # bitmaps are 32-bit word aligned
	j=$((j + skip[$(((width/2)%4))]))
done

size2=${#Screen[@]}
# Show screen elements for debug
for i in $(seq 0 $((size2-1))); do printf "Element [%d]: %d\n" $i ${Screen[$i]}; done


echo ============ ${#Screen[@]} ===========
echo ${Screen[@]}