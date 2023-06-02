/**
 * Project 1 
 * Assembler code fragment for LC-2K 
 */

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>

#define MAXLINELENGTH 1000


struct symbol {
    char name[7];
    char type[2];
    int line;
};

struct symbol symbols[MAXLINELENGTH];

struct relocation {
    char opcode[7];
    char name[7];
    int line;  
};

struct relocation relocations[MAXLINELENGTH];
// char defsymnum[MAXLINELENGTH];

int readAndParse(FILE *, char *, char *, char *, char *, char *);
int isNumber(char *);
int forR(char *opcode, char *reg0, char *reg1, char *dest);
int forI(char *opcode, char *reg0, char *reg1, char *offset, int pc, int tlabels, char labels[][7], int *labeladds);
int forJ(char *reg0, char *reg1);

int
main(int argc, char *argv[])
{
    // printf("at the beginning\n");
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

    // int data[MAXLINELENGTH];

    // printf("start 1\n");

    int symnum = 0, datanum = 0, textnum = 0, relnum = 0;
    int linenum = 0, start = -1;
    //going through first time to find the labels and check
    for (int a = 0; readAndParse(inFilePtr, label, opcode, arg0, arg1, arg2); a++){
        // printf("this is label %s opcode %s arg0 %s arg1 %s arg2 %s \n", label, opcode, arg0, arg1, arg2);
        if (!strcmp(opcode, ".fill") && start < 0){
            start = a;
        }

        if (strcmp(label, "")){
            // printf("it goes in here\n");
            for (int i = 0; i < tlabels; i++){
                if (!strcmp(label, labels[i])){
                    printf("ERROR: DUPLICATE LABEL FOUND\n");
                    exit(1);
                }
            }

            labeladds[tlabels] = a;
            strcpy(labels[tlabels++], label); 

            // Check if label is a symnumal
            if(isupper(label[0])){
                if (!strcmp(opcode, ".fill")){
                    symbols[symnum].line = a - start;
                    strcpy(symbols[symnum].type, "D");
                }
                else{
                    symbols[symnum].line = a;
                    strcpy(symbols[symnum].type, "T");
                }
                strcpy(symbols[symnum].name, label);
                // printf("%s %s %d\n", symbols[symnum].name, symbols[symnum].type, symbols[symnum].line);
                symnum++;
            }
        }

        if (!strcmp(opcode, ".fill")){
            // printf("at fill\n");
            datanum++;
            if (!isNumber(arg0)){
                relocations[relnum].line = a - start;
                strcpy(relocations[relnum].opcode, opcode);
                strcpy(relocations[relnum++].name, arg0);
            }
        }
        else if (
        !strcmp(opcode, "add") || !strcmp(opcode, "nor")
        || !strcmp(opcode, "lw") || !strcmp(opcode, "sw") 
        || !strcmp(opcode, "beq") || !strcmp(opcode, "jalr") 
        || !strcmp(opcode, "noop") || !strcmp(opcode, "halt")){
        ++textnum;
        }
        else{
            exit(1);
        }
        
        // printf("lw or sw\n");
        // printf("%d", relnum);
        if (!strcmp(opcode, "lw") | !strcmp(opcode, "sw")){
            // printf("in sw and lw\n");
            if (!isNumber(arg2)){
                relocations[relnum].line = a;
                // printf("inside1 %d\n", a);
                strcpy(relocations[relnum].opcode, opcode);
                // printf("inside2\n");
                strcpy(relocations[relnum++].name, arg2);
            }
        }
        linenum++;



    }// end for loop for storing labels and finding duplicates

    for (int i = 0; i < symnum; i++){
        printf("name: %s type: %s line: %d\n", symbols[i].name, symbols[i].type, symbols[i].line);
        // printf("hellow\n");
    }
    
    // printf("first rewind\n");
    // printf("each iteration %s\n", symbols[0].name);
    // printf("%d %d %d %d\n", textnum, datanum, symnum, relnum);
    //make sure to rewind after the first time going through
    rewind(inFilePtr);

    // int symnum = 0;
    int check = 0, defined = 0;
    linenum = 0;
    for (int a = 0; readAndParse(inFilePtr, label, opcode, arg0, arg1, arg2); a++){
        printf("this is label %d\n", a);
        // printf("%s \n", label);
        
        if (!strcmp(opcode, ".fill")){
            // // Check if label is a symnumal
            // if(isupper(label[0])){
            //     if (!strcmp(opcode, ".fill")){
            //         symbols[symnum].line = a - start;
            //         strcpy(symbols[symnum].type, "D");
            //     }
            //     else{
            //         symbols[symnum].line = a;
            //         strcpy(symbols[symnum].type, "T");
            //     }
            //     strcpy(symbols[symnum].name, label);
            //     // printf("%s %s %d\n", symbols[symnum].name, symbols[symnum].type, symbols[symnum].line);
            //     symnum++;
            // }  
            printf("this is arg0 %s\n", arg0);
            if (isupper(arg0[0])){
                // printf("it went in fill is upper for arg0 \n");
                
                for (int i = 0; i < tlabels; i++){
                    if (!strcmp(labels[i], arg0)){
                        check = 1;
                    }
                }
                for (int i = 0; i < symnum; i++){
                    if (!strcmp(symbols[i].name, arg0)){
                        defined = 1;
                    }
                }
                if (check == 0 && defined == 0){
                    printf("this is symnum: %d and this is symnum: %d in %d\n", symnum, symnum, a);
                    strcpy(symbols[symnum].type, "U");
                    strcpy(symbols[symnum].name, arg0);
                    symbols[symnum++].line = 0;
                    printf("symnum added in fill check");
                }
                else{
                    check = 0;
                    defined = 0;
                }
            }
        }
        else if (!strcmp(opcode, "lw") || !strcmp(opcode, "sw")) {
            // Check if label is a symnumal
            // if (isupper(label[0])){
            //     if (!strcmp(opcode, ".fill")){
            //         symbols[symnum].line = a - start;
            //         strcpy(symbols[symnum].type, "D");
            //     }
            //     else{
            //         symbols[symnum].line = a;
            //         strcpy(symbols[symnum].type, "T");
            //     }
            //     strcpy(symbols[symnum].name, label);
            //     // printf("%s %s %d\n", symbols[symnum].name, symbols[symnum].type, symbols[symnum].line);
            //     symnum++;
            // }  
            if (!isNumber(arg2)){
                if (isupper(arg2[0])){
                    for (int i = 0; i < tlabels; i++){
                        if (!strcmp(labels[i], label)){
                            check = 1;
                        }
                    }
                    for (int i = 0; i < symnum; i++){
                        if (!strcmp(symbols[i].name, arg2)){
                            defined = 1;
                            printf("here 2\n");
                        }
                    }
                    // printf("%d this is check\n", check);
                    if (check == 0 && defined == 0){
                        strcpy(symbols[symnum].type, "U");
                        strcpy(symbols[symnum].name, arg2);
                        symbols[symnum++].line = 0;
                        printf("symnum added in lw check %d\n", a);
                        // printf("%s\n", symbols[0].name);
                        // printf("this printed\n");
                    }
                    else{
                        check = 0;
                        defined = 0;
                    }
                }
            }
        }
        else if (!strcmp(opcode, "beq")){
            if (!isNumber(arg2) && isupper(arg2[0])){
                int check = 1;
                for (int i = 0; i < tlabels; i++){
                    if (!strcmp(labels[i], arg2)){
                        check = 0;
                    }
                }
                if (check){
                    printf("ERROR: with undefined beq\n");
                    exit(1);
                }
            }
        }
        for (int i = 0; i < symnum; i++){
            printf("name: %s type: %s line: %d\n", symbols[i].name, symbols[i].type, symbols[i].line);
            // printf("hellow\n");
        }
    }
    // printf("each iteration: %s\n", symbols[0].name);
    // printf("this is symnum: %d and this is symnum: %d\n", symnum, symnum);
    // printf("%s\n", symbols[0].name);
    // symnum += symnum;
    // printf("%d %d %d %d\n", textnum, datanum, symnum, relnum);
    fprintf(outFilePtr,"%d %d %d %d\n", textnum, datanum, symnum, relnum);
    rewind(inFilePtr);


    int pc = 0;

    // printf("made it to the final read and parse\n");
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
                        // printf("label was found\n");
                        mc = labeladds[i];
                        break;
                    }
                    else{
                        // printf("Looking for the label\n");
                        // printf("%d/%d labels\n", i, tlabels);
                    }
                    if (i == 5){
                        //if it somehow gets out here then the label is probably not present
                        // printf("The label probably does not exist\n");
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
    // printf("it is going to print these things\n");
    // printf("%d symnum \n", symnum);
    // printf("%d relnum\n", relnum);
    // printf("what is this\n");
    for (int i = 0; i < symnum; i++){
        fprintf(outFilePtr, "%s %s %d\n", symbols[i].name, symbols[i].type, symbols[i].line);
        printf("name: %s type: %s line: %d\n", symbols[i].name, symbols[i].type, symbols[i].line);
        // printf("hellow\n");
    }
    for (int i = 0; i < relnum; i++) {
        fprintf(outFilePtr, "%d %s %s\n", relocations[i].line, relocations[i].opcode, relocations[i].name);
        // printf("%d %s %s\n", relocations[i].line, relocations[i].opcode, relocations[i].name);
        // printf("hello\n");
    }
    // printf("at the end returning things\n");

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
        if (isupper(offset[0]) && (!strcmp(opcode, "lw") || !strcmp(opcode, "sw") || !strcmp(opcode, ".fill"))){
            mc |= finoff;
            return mc;
        }

        //no offset
        printf("ERROR CANT FIND LABEL %s\n", opcode);
        exit(1);
    }//end else if
    return mc;
}// end forI