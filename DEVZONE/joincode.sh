#!/bin/bash
#  Bash script to combine multiple code sections in one only header
#  by Martin Rizzo

SourFiles=()
OutputFile=output.h


#============================ PROCESS ARGUMENTS ============================#

while [ $# -gt 0 ]; do
    case "$1" in
        -o | --output)  shift ; OutputFile="$1" ;;
        -*)             echo "ERROR: Unknown option '$1'" ; exit 1 ;;
        *)              SourFiles[${#SourFiles[@]}]="$1" ;;  
    esac
    shift
done

#================================== START ==================================#

rm -f "$OutputFile"
for sour_file in ${SourFiles[@]}; do
      awk '
          /^([^#]|$)/ { p=1 }
          p==1 {
            if (match($0,/HEADER_CODE\([^\)]*/)) print substr($0,RSTART+12,RLENGTH-12) 
            else                                 print $0
          }
      ' "$sour_file" >> "$OutputFile"
done

