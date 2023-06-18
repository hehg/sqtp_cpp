#!/bin/bash

#sqtp_dir=/home/wtc/sqtp_libs
sqtp_dir=~/sqtp_libs

if [ ! -d $sqtp_dir ]
then
  echo "mkdir $sqtp_dir"
  mkdir $sqtp_dir
fi
libdirs="libsqtp libprotocol libbz libfe4ed liboffer"
echo "release include files"
for a in $libdirs
do
    echo "$a"
    rm -rf $sqtp_dir/$a
    mkdir $sqtp_dir/$a
    cp -r ./$a/include  $sqtp_dir/$a/
done
cp -r ./libsqtp/third $sqtp_dir/libsqtp/

rm -rf $sqtp_dir/lib
cp -r ./lib  $sqtp_dir
