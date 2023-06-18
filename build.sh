#!/bin/bash


for mydir in build;do
   if [ ! -d $mydir ]; then
      mkdir $mydir
   fi
done

cd build
rm -rf ./*
if [ $# -eq 1 ];then
   build_type=$1
   if [ "$build_type" = "d" ];then
      echo "build debug version"
      cmake  -DCMAKE_BUILD_TYPE=Debug .. 
   else
      echo "build release version"
      cmake  -DCMAKE_BUILD_TYPE=Release .. 
   fi
else #default build as debug
     echo "build debug version"
      cmake  -DCMAKE_BUILD_TYPE=Debug .. 
fi 

make -j

cd -
