#!/bin/bash


lcov  -d ../build/libsqtp/CMakeFiles/sqtp.dir  -b ../libsqtp/ --no-external  -rc lcov_branch_coverage=1 -t sqtp_ut -c -o sqtp_i.info

genhtml -o ./sqtp_result --prefix=`pwd` --branch-coverage --function-coverage sqtp_i.info 