/**
 * Project 1 
 * Assembler code fragment for LC-2K 
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MAXLINELENGTH 1000

int readAndParse(FILE *, char *, char *, char *, char *, char *);
int isNumber(char *);
int forR(char *opcode, char *reg0, char *reg1, char *dest);
int forI(char *opcode, char *reg0, char *reg1, char *offset, int pc, int tlabels, char labels[][7], int *labeladds);
int forJ(char *reg0, char *reg1);

int
main(int argc, char *argv[])
{
    char *inFileString, *outFileString;
    FILE *inFilePtr, *outFilePtr;
    char label[MAXLINELENGTH], opcode[MAXLINELENGTH], arg0[MAXLINELENGTH],
            arg1[MAXLINELENGTH], arg2[MAXLINELENGTH];

    if (argc != 3) {
        printf("error: usage: %s <assembly-code-file> <machine-code-file>\n",
            argv[0]);
        exit(1);
    }

    inFileString = argv[1];
    outFileString = argv[2];

    inFilePtr = fopen(inFileString, "r");
    if (inFilePtr == NULL) {
        printf("error in opening %s\n", inFileString);
        exit(1);
    }
    outFilePtr = fopen(outFileString, "w");
    if (outFilePtr == NULL) {
        printf("error in opening %s\n", outFileString);
        exit(1);
    }

    // /* here is an example for how to use readAndParse to read a line from
    //     inFilePtr */
    // if (! readAndParse(inFilePtr, label, opcode, arg0, arg1, arg2) ) {
    //     /* reached end of file */
    // }

    /* this is how to rewind the file ptr so that you start reading from the
        beginning of the file */
    // rewind(inFilePtr);

    // /* after doing a readAndParse, you may want to do the following to test the
    //     opcode */
    // if (!strcmp(opcode, "add")) {
    //     /* do whatever you need to do for opcode "add" */
    // }

    // HAVE A DATA TABLE
    // BUILD LABELS AND FIND DUPLICATES. IF NONE CONTINUE, ELSE EXIT(1)
    // REWIND IF NEEDED

    char labels[MAXLINELENGTH][7]; // data table
    int labeladds[MAXLINELENGTH];
    int tlabels = 0;

    //going through first time to find the labels and check
    for (int a = 0; readAndParse(inFilePtr, label, opcode, arg0, arg1, arg2); a++){
        if (strcmp(label, "")){
            for (int i = 0; i < tlabels; i++){
                if (!strcmp(label, labels[i])){
                    printf("ERROR: DUPLICATE LABEL FOUND\n");
                    exit(1);
                }
            }

            labeladds[tlabels] = a;
            strcpy(labels[tlabels++], label);
        }
    }// end for loop for storing labels and finding duplicates

    //make sure to rewind after the first time going through
    rewind(inFilePtr);


    int pc = 0;

    while (readAndParse(inFilePtr, label, opcode, arg0, arg1, arg2)){

        int mc = 0;
        // printf("%s, %s, %s, %s, %s\n", label, opcode, arg0, arg1, arg2);

        if (!strcmp(opcode, "add")){ // 000
            mc = forR(opcode, arg0, arg1, arg2);
        }
        else if (!strcmp(opcode, "nor")){ // 001
            // printf("went into nor\n");
            mc = forR(opcode, arg0, arg1, arg2);
        }
        else if (!strcmp(opcode, "lw")){ // 010
            // printf("went into lw\n");
            mc = forI(opcode, arg0, arg1, arg2, pc, tlabels, labels, labeladds);
        }
        else if (!strcmp(opcode, "sw")){ // 011
            // printf("went into sw\n");
            mc = forI(opcode, arg0, arg1, arg2, pc, tlabels, labels, labeladds);
        }
        else if (!strcmp(opcode, "beq")){ // 100
            // printf("went into beq\n");
            mc = forI(opcode, arg0, arg1, arg2, pc, tlabels, labels, labeladds);
        }
        else if (!strcmp(opcode, "jalr")){ // 101
            // printf("went into jalr\n");
            // mc = (5<<22) | (atoi(arg0)<<19) | (atoi(arg1)<<16);
            mc = forJ(arg0, arg1);
        }
        else if (!strcmp(opcode, "halt")){ // 110
            // printf("went into halt\n");
            mc = (6<<22);
        }
        else if (!strcmp(opcode, "noop")){ // 111
            // printf("went into noop\n");
            mc = (7<<22);
        }
        else if (!strcmp(opcode, ".fill")){ // .fill
            // printf("went into fill\n");
            if (isNumber(arg0)) mc = atoi(arg0);
            else{
                for (int i = 0; i < tlabels; i++){
                    // printf("this is arg0: %s\n", arg0);
                    // printf("%s == %d\n", labels[i], labeladds[i]);
                    if (!strcmp(labels[i], arg0)){
                        printf("label was found\n");
                        mc = labeladds[i];
                        break;
                    }
                    else{
                        printf("Looking for the label\n");
                        printf("%d/%d labels\n", i, tlabels);
                    }
                    if (i == 5){
                        //if it somehow gets out here then the label is probably not present
                        printf("The label probably does not exist\n");
                    }
                }
            }
        }
        else{
            printf("PC: %d << INVALID OPCODE: %s\n", pc, opcode);
            exit(1);
        }

        fprintf(outFilePtr, "%d\n", mc);
        pc++;
    }

    return(0);
}




/*
 * Read and parse a line of the assembly-language file.  Fields are returned
 * in label, opcode, arg0, arg1, arg2 (these strings must have memory already
 * allocated to them).
 *
 * Return values:
 *     0 if reached end of file
 *     1 if all went well
 *
 * exit(1) if line is too long.
 */
int
readAndParse(FILE *inFilePtr, char *label, char *opcode, char *arg0, char *arg1, char *arg2)
{
    char line[MAXLINELENGTH];
    char *ptr = line;

    /* delete prior values */
    label[0] = opcode[0] = arg0[0] = arg1[0] = arg2[0] = '\0';

    /* read the line from the assembly-language file */
    if (fgets(line, MAXLINELENGTH, inFilePtr) == NULL) {
	/* reached end of file */
        return(0);
    }

    /* check for line too long */
    if (strlen(line) == MAXLINELENGTH-1) {
        printf("error: line too long\n");
        exit(1);
    }

    /* is there a label? */
    ptr = line;
    if (sscanf(ptr, "%[^\t\n ]", label)) {
	/* successfully read label; advance pointer over the label */
        ptr += strlen(label);
    }

    /*
     * Parse the rest of the line.  Would be nice to have real regular
     * expressions, but scanf will suffice.
     */
    sscanf(ptr, "%*[\t\n\r ]%[^\t\n\r ]%*[\t\n\r ]%[^\t\n\r ]%*[\t\n\r ]%[^\t\n\r ]%*[\t\n\r ]%[^\t\n\r ]",
        opcode, arg0, arg1, arg2);
    return(1);
}

int
isNumber(char *string)
{
    /* return 1 if string is a number */
    int i;
    return( (sscanf(string, "%d", &i)) == 1);
}


int regcheck(char *reg){
    // printf("%s\n", reg);
    // printf("atoi test\n");
    int thereg = atoi(reg);


    // printf("atoi works\n");
    if (!isNumber(reg)){
        // printf("is not a number\n");
        return 0;
    }
    if (thereg < 0 || thereg > 7){
        // printf("is not between 0 and 7\n");
        return 0;
    }

    // printf("returning 1\n");
    return 1;
}

int
forJ(char *reg0, char *reg1){
    if (!regcheck(reg0) || !regcheck(reg1)){
        printf("ERROR in the register\n");
        exit(1);
    }

    int mc = 0;
    mc = (5<<22) | (atoi(reg0)<<19) | (atoi(reg1)<<16);
    return mc;
}//end forJ

int
forR(char *opcode, char *reg0, char *reg1, char *dest){

    if (!regcheck(reg0) || !regcheck(reg1) || !regcheck(dest)){
        printf("ERROR in the register\n");
        exit(1);
    }

    int mc = 0;

    if (!strcmp(opcode, "add")) mc = 0<<22;
    else if (!strcmp(opcode, "nor")) mc = 1<<22;
    else printf("ERROR WITH THE OPCODE NUMBER\n");

    mc |= (atoi(reg0) << 19) | (atoi(reg1) << 16) | (atoi(dest) << 0);

    return mc;
}//end forR

int
forI(char *opcode, char *reg0, char *reg1, char *offset, int pc, int tlabels, char labels[][7], int *labeladds){

    if (!regcheck(reg0) || !regcheck(reg1)){
        printf("ERROR in the register\n");
        exit(1);
    }
    
    int mc = 0, finoff = 0;

    if (!strcmp(opcode, "lw")) mc = 2<<22;
    else if (!strcmp(opcode, "sw")) mc = 3<<22;
    else if (!strcmp(opcode, "beq")) mc = 4<<22;
    else printf("ERROR WITH THE OPCODE NUMBER2\n");

    mc |= (atoi(reg0)<<19) | (atoi(reg1)<<16);

    if (isNumber(offset)){ //check if numeric offset
        // printf("here numeric\n");
        int arg2 = atoi(offset);
        if (arg2 <= -32769 || arg2 >= 32768){
            printf("ERROR TOO BIG OR SMALL\n");
            exit(1);
        }
        if (arg2 >= 0 || !strcmp(opcode, ".fill")){
            mc |= arg2;
            return mc;
        }
        arg2 &= 0xFFFF;
        mc += arg2;
        return mc;
    }
    else{ //if not a numeric offset, then it is a symbolic offset
        // printf("non numeric\n");
        for (int i = 0; i < tlabels; i++){
            // printf("%s\n", offset);
            if (!strcmp(offset, labels[i])){
                if (!strcmp(opcode, "lw") || !strcmp(opcode, "sw")){
                    // printf("this is labeladds: %d\n", labeladds[i]);
                    finoff = labeladds[i];
                    mc |= finoff;
                } 
                else{ //beq
                    // printf("beq at all\n");
                    finoff = labeladds[i];
                    int dbeq = finoff - pc - 1;

                    dbeq &= 0xFFFF;
                    mc |= dbeq;

                    // if (finoff < 0){
                    //     mc |= (65535 + finoff + 1);
                    // }
                    // else{
                    //     mc |= (finoff);
                    // }
                }
                return mc;
            }
        }

        //no offset
        printf("ERROR CANT FIND LABEL\n");
        exit(1);
    }//end else if
    return mc;
}// end forI