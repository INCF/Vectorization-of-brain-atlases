#!/bin/bash
pngFolder=${1%/}
svgFolder=${2%/}
if [ ! -d $pngFolder ]; then
    echo "Error: png folder path incorrect."
fi
if [[ ! -d $svgFolder ]]; then
    mkdir $svgFolder
fi
for pngFile in $pngFolder/*.png; do 
	filename=$(basename "$pngFile"); 
	filename="${filename%.*}";
	./b2v.sh $pngFile $svgFolder/$filename.svg;  
done