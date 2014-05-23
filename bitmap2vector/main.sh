MYPWD=$(pwd)
cd src
octave main.m $1 $MYPWD $2
cd $MYPWD