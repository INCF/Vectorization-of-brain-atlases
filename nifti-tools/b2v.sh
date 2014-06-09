ulimit -s unlimited

if [ $# -eq 0 ]
  then
    echo "Usage: /b2v.sh path_to_filename.png [path_to_outputfileName.svg]"
fi

if [ $# -eq 1 ]
  then
    $(dirname "$(readlink -f "$0")")/b2v $1
fi

if [ $# -eq 2 ]
  then
    $(dirname "$(readlink -f "$0")")/b2v $1 $2
fi
