/*
 * Project 1
 * EECS 370 LC-2K Instruction-level simulator
 *
 * Make sure *not* to modify printState or any of the associated functions
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// Machine Definitions
#define NUMMEMORY 65536 /* maximum number of words in memory */
#define NUMREGS 8 /* number of machine registers */

// File
#define MAXLINELENGTH 1000 /* MAXLINELENGTH is the max number of characters we read */

typedef struct stateStruct {
    int pc;
    int mem[NUMMEMORY];
    int reg[NUMREGS];
    int numMemory;
} stateType;

void printState(stateType *);

int convertNum(int);

void argin(stateType *state, int *opcode, int *rega, int *regb, int *dest, int *OS);

int main(int argc, char *argv[])
{
    char line[MAXLINELENGTH];
    stateType state;
    FILE *filePtr;

    if (argc != 2) {
        printf("error: usage: %s <machine-code file>\n", argv[0]);
        exit(1);
    }

    filePtr = fopen(argv[1], "r");
    if (filePtr == NULL) {
        printf("error: can't open file %s", argv[1]);
        perror("fopen");
        exit(1);
    }

    /* read the entire machine-code file into memory */
    for (state.numMemory = 0; fgets(line, MAXLINELENGTH, filePtr) != NULL; state.numMemory++) {
        if (sscanf(line, "%d", state.mem+state.numMemory) != 1) {
            printf("error in reading address %d\n", state.numMemory);
            exit(1);
        }
        printf("memory[%d]=%d\n", state.numMemory, state.mem[state.numMemory]);
    }

    for (int i = 0; i < NUMREGS; ++i) {
        state.reg[i] = 0;
    }

    //Your code starts here

    //GET ALL THE INSTRUCTION. KEEP TRACK OF PC AND NUMBER OF INSTRUCTIONS
    //THIS COULD BE USING CASES TO FIND WHICH OPCODES ARE USED

    //PRINT NUMBER OF INSTRUCTIONS AT END AND FINAL STATE
    //FOR THE INSTRUCTIONS TO SIMULATE, U CAN EITHER DO IT IN THERE DIRECTLY OR LIKE 1A MAKE ANOTHER FUNCTION OUTSIDE
    //THAT HELPS THE DIFFERENT INSTRUCTION TYPES (IRJO)

    //MAKE SURE TO KEEP TRACK OF OFFSET AS WELL AND FIND WAYS TO TRANSLATE THE REGISTERS(AB C/DESTINATION)

    int halt = 0, cinstr = 0;
    int opcode, rega, regb, dest, OS;

    while(!halt){
        printState(&state);
        argin(&state, &opcode, &rega, &regb, &dest, &OS);

        if (opcode == 0){ //add
            state.reg[dest] = state.reg[rega] + state.reg[regb];
            ++state.pc;
        }
        else if (opcode == 1){ //nor
            state.reg[dest] = ~(state.reg[rega] | state.reg[regb]);
            ++state.pc;
        }
        else if (opcode == 2){ //lw
            state.reg[regb] = state.mem[state.reg[rega] + OS];
            ++state.pc;
        }
        else if (opcode == 3){ //sw
            state.mem[state.reg[rega] + OS] = state.reg[regb];
            ++state.pc;
        }
        else if (opcode == 4){ //beq
            if (state.reg[rega] == state.reg[regb]) 
                state.pc = state.pc + OS + 1;
            else ++state.pc;
        }
        else if (opcode == 5){ //jalr
            state.reg[regb] = state.pc + 1;
            state.pc = state.reg[rega];
        }
        else if (opcode == 6){ //halt
            halt = 1;
            ++state.pc;
        }
        else if (opcode == 7){ //noop
            ++state.pc;
        }
        ++cinstr;
    }//end while

    printf("machine halted\ntotal of %d instructions executed\n", cinstr);
    printf("final state of machine:\n");
    printState(&state);

    return(0);
}

void printState(stateType *statePtr)
{
    int i;
    printf("\n@@@\nstate:\n");
    printf("\tpc %d\n", statePtr->pc);
    printf("\tmemory:\n");
    for (i=0; i<statePtr->numMemory; i++) {
              printf("\t\tmem[ %d ] %d\n", i, statePtr->mem[i]);
    }
    printf("\tregisters:\n");
    for (i=0; i<NUMREGS; i++) {
              printf("\t\treg[ %d ] %d\n", i, statePtr->reg[i]);
    }
    printf("end state\n");
}

int convertNum(int num)
{
    /* convert a 16-bit number into a 32-bit Linux integer */
    if (num & (1<<15) ) {
        num -= (1<<16);
    }
    return(num);
}

void argin(stateType *state, int *opcode, int *rega, int *regb, int *dest, int *OS){
    *opcode = (state->mem[state->pc] & 29360128) >> 22;
    *rega = (state->mem[state->pc] & 3670016) >> 19;
    *regb = (state->mem[state->pc] & 458752) >> 16;
    *dest = (state->mem[state->pc] & 7) >> 0;
    *OS = convertNum(state->mem[state->pc] & 65535) >> 0;
}