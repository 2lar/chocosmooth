#!/bin/bash


# Reset
NC='\033[0m'       # Text Reset

# Colors
BLACK='\033[0;30m'        # Black
RED='\033[0;31m'          # Red
GREEN='\033[0;32m'        # Green
YELLOW='\033[0;33m'       # Yellow
BLUE='\033[0;34m'         # Blue
PURPLE='\033[0;35m'       # Purple
CYAN='\033[0;36m'         # Cyan
WHITE='\033[0;37m'        # White


usage() {
    echo -e "${RED}Usage: $0 <assembly-code-file> <mc-file>"
    echo -e "\tCompiles assembly code to object files"
    echo -e "\tThen links object files to produce .mc output file"
}


if [[ $# -lt 2 ]]; then
    usage
    exit 1
fi


# arguments
args="${@:1:$(($#))}"


# # check if want to bypass error
# if [[ ${@: -1} == "--force" ]]; then
#     # set arguments to not include '--force'
#     args="${@:1:$(($#-1))}"
# else
#     # Stop on errors
#     # See https://vaneyckt.io/posts/safer_bash_scripts_with_set_euxo_pipefail/
#     set -Eeuo pipefail
# fi


# check if input files are valid
idx=1
for string in $args; do
    if [[ $idx -eq $# ]]; then
        if [[ ! $string == *.mc ]]; then
            echo -e "${RED}Last argument must be .mc file"
            exit 1
        fi
    elif [[ ! $string == *.as ]]; then
        echo -e "${RED}Arguments must be .as files, with last being .mc file"
        exit 1
    fi
    ((idx += 1))
done


# compile assembler (should be up to date anyways)
make assembler


# create obj files for each .as file
idx=1
echo -e "${BLUE}running ./assembler $args${NC}"
for string in $args; do
    if [[ idx -ne $# ]]; then
        base=${string%.*}
        outfile="${base}.obj"
        echo -e "${PURPLE}Compiiling ./assembler $string $outfile${NC}"
        ./assembler $string $outfile

        exit_status=$?
        if [[ exit_status -ne 0 ]]; then
            echo -e "${RED}Failed to assemble $string"
            exit 1
        fi
    fi
    ((idx += 1))
done


# compile linker
make linker


# do not run linker if compile errors
exit_status=$?
if [[ exit_status -ne 0 ]]; then
    echo -e "${RED}Failed to compile linker.c"
    exit 1
fi


# run linker
idx=1
linked=""
echo -e "${BLUE}running ./linker $args${NC}${YELLOW}"
for string in $args; do
    if [[ idx -ne $# ]]; then
        base=${string%.*}
        infile="${base}.obj"
        linked="${linked} ${infile}"
    fi
    ((idx += 1))
done
linked="${linked} ${@: -1}"
./linker $linked


# get exit status and print to stdout
exit_status=$?
exit_color="${RED}"
if [[ exit_status -eq 0 ]]; then
    exit_color="${GREEN}"
fi
echo -e "${exit_color}Exited with status $exit_status${NC}"