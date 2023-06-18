#!/bin/bash

host=wtc@170.0.40.74
#rsync -var ./*  wtc@$host:/home/wtc/hh/

rsync -avr ./* -e "ssh -p22001" ${host}:/home/wtc/hh/sqtp