#!/bin/bash
import re
import sys

pattern_struct=re.compile(r'^\s*struct\s+(\w+)\s+\/\/([\u4E00-\u9FA5A-Za-z0-9_\s]+)')
pattern_field = re.compile(r'^\s*([a-zA-z0-9<>]+)\s*(\w+)+\s*;\s*\/\/([\u4E00-\u9FA5A-Za-z0-9_\s]*)$')


class class_meta_info:
    def __init__(self):
        self.struct_name=''
        self.struct_des=''
meta_infos=dict()

def paser_one_file(path):
    f=open(path)
    status='get_struct'
    for line in f:
        line=line.strip()
        if line=='':
            continue
        if status=='get_struct':
            ret=pattern_struct.search(line)
            #print(line,ret)
            if ret!=None:
                print(ret.group(1),ret.group(2))   
                status='get_fields'
        elif status=='get_fields':
            ret=pattern_field.search(line)
            
            if ret!=None:
                print(ret.group(1),ret.group(2),ret.group(3))
            else:
                if line=='};':
                    status='get_struct'
                elif line=='{':pass
                else:
                    print("ERR:"+line)

if __name__=="__main__":
    #input_file=sys.argv[1]
    #paser_one_file(input_file)
    paser_one_file('/Users/hehong/git/sqtp_cpp/tools/gen_code/input.h')