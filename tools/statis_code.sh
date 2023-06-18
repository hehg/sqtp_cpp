#!/bin/bash

function ss
{
    find ../libsqtp -name "*.cpp" -o -name "*.h"|xargs wc -l
    find ../libsqbz -name "*.cpp" -o -name "*.h"|xargs wc -l
    find ../liboffer -name "*.cpp" -o -name "*.h"|xargs wc -l
    find ../libframe -name "*.cpp" -o -name "*.h"|xargs wc -l
}

ss|grep total|awk -F " " 'BEGIN{a=0} {a=a+$1;} END{print "total lines= ",a}'