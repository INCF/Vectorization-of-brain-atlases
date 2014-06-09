ulimit -s unlimited

if [ $# -eq 0 ]
  then
    echo "[for help]:./b2v -h"
fi

if [ $# -eq 1 ]
  then
    $(dirname "$(readlink -f "$0")")/bin/b2v $1
fi

if [ $# -eq 2 ]
  then
    $(dirname "$(readlink -f "$0")")/bin/b2v $1 $2
fi
