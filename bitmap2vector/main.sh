MYPWD=$(pwd)
cd src
octave main.m $1 $MYPWD
cd $MYPWD