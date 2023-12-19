#!/bin/bash
#用来统计覆盖率脚本

##  libsqtp 覆盖率统计
lcov  -d ../build/libsqtp/CMakeFiles/sqtp.dir  -b ../libsqtp/ --no-external  -rc lcov_branch_coverage=1 -t sqtp_ut -c -o sqtp_i.info

genhtml -o ./sqtp_result --prefix=`pwd` --branch-coverage --function-coverage sqtp_i.info 

cd ./sqtp_result
python -m SimpleHTTPServer 8000
cd ..