#!/bin/bash

# Colors + Reset
NC='\033[0m'       # Text Reset
BLACK='\033[0;30m'        # Black
RED='\033[0;31m'          # Red
GREEN='\033[0;32m'        # Green
YELLOW='\033[0;33m'       # Yellow
BLUE='\033[0;34m'         # Blue
PURPLE='\033[0;35m'       # Purple
CYAN='\033[0;36m'         # Cyan
WHITE='\033[0;37m'        # White


usage() {
    echo -e "${RED}Usage: ${BLUE}$0 <assembly-code file> <blockSizeInWords> <numberOfSets> <blocksPerSet>"
}


clear


if [[ $# -ne 4 ]] && [[ $# -ne 5 ]]; then
    usage
    exit 1
fi


# check if input files are valid
re='^[0-9]+$'
if [[ ! $1 == *.as ]]; then
    echo -e "${RED}First argument must be .as file"
    usage
    exit 1
fi
if [[ ! $2 =~ $re ]]; then
    echo -e "${RED}Second argument must be a number"
    usage
    exit 1
fi
if [[ ! $3 =~ $re ]]; then
    echo -e "${RED}Third argument must be a number"
    usage
    exit 1
fi
if [[ ! $4 =~ $re ]]; then
    echo -e "${RED}Fourth argument must be a number"
    usage
    exit 1
fi
if [[ $# -eq 5 ]]; then
    if [[ ! $5 == "-c" ]] && [[ ! $5 == "--clear" ]]; then
        echo -e "${RED}Fifth argument can only be -c or --clear"
        usage
        exit 1
    fi
fi


# compile assembler
echo -en "${BLACK}"
make -s assembler


# check for compile error
ex=$?
if [[ $ex -ne 0 ]]; then
    echo -e "${RED}Failed to compile assembler"
    exit 1
fi


# create obj files for .as file
base=$1
base=${base%.*}
outfile="${base}.obj"
./assembler $1 $outfile


# check for execute error
ex=$?
if [[ $ex -ne 0 ]]; then
    echo -e "${RED}Failed to execute assembler"
    exit 1
fi


# # compile linker
# echo -en "${BLACK}"
# make -s linker


# # check for compile error
# ex=$?
# if [[ $ex -ne 0 ]]; then
#     echo -e "${RED}Failed to compile linker"
#     exit 1
# fi


# # run linker
# base=$outfile
# base=${base%.*}
# outfile="${base}.mc"
# base="${base}.obj"
# ./linker $base $outfile 1> /dev/null


# # check for execute error
# ex=$?
# if [[ $ex -ne 0 ]]; then
#     echo -e "${RED}Failed to execute linker"
#     exit 1
# fi


# compile simulator with the cache simulator
echo -en "${PURPLE}"
make -s simulator
# gcc cache.c my_p1s_sim.c -lm -o simulator


# check for compile error
ex=$?
if [[ $ex -ne 0 ]]; then
    echo -e "${RED}Failed to compile simulator with cache"
    exit 1
fi


# run simulator with cache
output="a.out"
echo -e "${BLACK}./simulator $outfile $2 $3 $4 > $output"
echo -ne "${YELLOW}"
./simulator $outfile $2 $3 $4 > $output


# check for execute error
ex=$?
if [[ $ex -ne 0 ]]; then
    echo -e "${RED}Failed to execute simulator"

    # remove intermediate files: .obj and .mc files
    if [[ $# -eq 5 ]]; then
        rm *.obj
        rm *.mc
    fi
    exit 1
fi


# get exit status and print to stdout
exit_color="${RED}"
if [[ exit_status -eq 0 ]]; then
    exit_color="${GREEN}"
fi
echo -e "${exit_color}Exited with status $ex${NC}"


# remove intermediate files: .obj and .mc files
if [[ $# -eq 5 ]]; then
    rm *.obj
    rm *.mc
fi