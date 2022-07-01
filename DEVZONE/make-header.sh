##!/bin/bash
#  Bash script to combine the multiple code sections in one only header
#  by Martin Rizzo

ScriptDir=$( dirname -- "${BASH_SOURCE[0]}" )
InputDir="./sections"
OutputDir=".."


combine_code() {
    local sour_file
    for sour_file in "$@"; do
      awk '/^([^#]|$)/ { p=1 } p==1' "$InputDir/$sour_file"
    done
}

print_line_count() {
    local line_count=$(awk 'END{print NR}' "$1")
    echo "$1 $line_count lines"
}


cd -- "$ScriptDir" &> /dev/null

header_file="pepe.h"
combine_code head.h types.h iface.h internal.c iface.c > "$header_file"
print_line_count "$header_file"
