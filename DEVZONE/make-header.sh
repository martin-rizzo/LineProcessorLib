##!/bin/bash
#  Bash script to combine the multiple code sections in one only header
#  by Martin Rizzo

ScriptDir=$( dirname -- "${BASH_SOURCE[0]}" )
InputDir="./sections"
OutputDir=".."


combine_code() {
    local sour_file
    for sour_file in "$@"; do
      awk '
          /^([^#]|$)/ { p=1 }
          p==1 {
            if (match($0,/HEADER_CODE\([^\)]*/)) print substr($0,RSTART+12,RLENGTH-12) 
            else                                 print $0
          }
      ' "$InputDir/$sour_file"
    done
}

print_line_count() {
    local line_count=$(awk 'END{print NR}' "$1")
    printf '%-14s %s lines of code\n' "$1" "$line_count"
}


#================================== START ==================================#

#
# set the working directory to the
# directory where the script is stored
#
cd -- "$ScriptDir" &> /dev/null

#
# make the UTF8 version
#
header="$OutputDir/linepro.h"
rm "$header"
combine_code head-utf.h includes.h types.h iface.h internal.c iface.c > "$header"
print_line_count "$header"

#
# make the ASCII version
#
header="$OutputDir/lineproa.h"
rm "$header"
combine_code head-asc.h includes.h types.h iface.h internal.c iface.c > "$header"
print_line_count "$header"

