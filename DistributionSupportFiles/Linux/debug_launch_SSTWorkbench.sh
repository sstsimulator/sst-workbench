#!/bin/sh
##appname=`basename $0 | sed s,\.sh$,,`
appname=SSTWorkbench

echo "DEBUG-appname="$appname > output.txt

dirname=`dirname $0`
tmp="${dirname#?}"

echo "DEBUG-dirname="$dirname >> output.txt
echo "DEBUG-tmp="$tmp >> output.txt


if [ "${dirname%$tmp}" != "/" ]; then
    dirname=$PWD/$dirname
fi
LD_LIBRARY_PATH=$dirname/lib
export LD_LIBRARY_PATH

echo >> output.txt
echo >> output.txt
echo "--- DEBUG RUN ---">> output.txt

#$dirname/$appname "$@"
$dirname/$appname 2>> output.txt


echo >> output.txt
echo >> output.txt
echo "--- DEBUG ENV ---">> output.txt

env | sort >> output.txt

echo >> output.txt
echo >> output.txt
echo "--- DEBUG LDD ---">> output.txt

ldd $dirname/$appname >> output.txt

