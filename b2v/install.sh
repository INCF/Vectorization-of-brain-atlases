make -f  $(dirname $0)/makefile
SCRIPTPATH=$( cd $(dirname $0) ; pwd -P )
sudo ln -s  $SCRIPTPATH/b2v.sh /usr/bin/b2v