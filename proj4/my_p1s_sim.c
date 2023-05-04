/*
 * Project 1
 * EECS 370 LC-2K Instruction-level simulator
 *
 * Make sure *not* to modify printState or any of the associated functions
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
// #include "cache.c"

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

extern void cache_init(int blockSize, int numSets, int blocksPerSet);
extern int cache_access(int addr, int write_flag, int write_data);
extern void printStats();
static stateType state;
static int num_mem_accesses = 0;
int mem_access(int addr, int write_flag, int write_data) {
    ++num_mem_accesses;
    if (write_flag) {
        state.mem[addr] = write_data;
    }
    return state.mem[addr];
}
int get_num_mem_accesses(){
	return num_mem_accesses;
}

int main(int argc, char *argv[])
{
    char line[MAXLINELENGTH];
    FILE *filePtr;

    // printf("THIS IS SIZE: %d\n", argc);
    // printf("Arguments:\n");
    // for (int i = 0; i < argc; i++) {
    //     printf("Argument %d: %s\n", i, argv[i]);
    // }

    if (argc != 5) {
        printf("error: usage: %s <machine-code file>\n", argv[0]);
        exit(1);
    }

    filePtr = fopen(argv[1], "r");
    if (filePtr == NULL) {
        printf("error: can't open file %s\n", argv[1]);
        perror("fopen");
        exit(1);
    }
    // printf("TESTIUNG 65\n");
    int block_size = atoi(argv[2]);
    int num_sets = atoi(argv[3]);
    int blocks_per_set = atoi(argv[4]);
    // printf("TESTIUNG 65\n");
    // printf("THIS IS CHECK ARGS %d, %d, %d\n", atoi(argv[2]), atoi(argv[3]), atoi(argv[4]));
    cache_init(block_size, num_sets, blocks_per_set);

    /* read the entire machine-code file into memory */
    for (state.numMemory = 0; fgets(line, MAXLINELENGTH, filePtr) != NULL; state.numMemory++) {
        if (sscanf(line, "%d", state.mem+state.numMemory) != 1) {
            printf("error in reading address %d\n", state.numMemory);
            exit(1);
        }
        // printf("memory[%d]=%d\n", state.numMemory, state.mem[state.numMemory]);
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
    state.pc = 0;

    while(!halt){
        // printf("SIMULATOR: this is instruction fetch\n");
        int instruction = cache_access(state.pc, 0, 0);
        (void)instruction;
        // printState(&state);
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
            // printf("SIMULATOR: THIS IN LW\n");
            state.reg[regb] = cache_access(state.reg[rega] + OS, 0, 0);
            ++state.pc;
        }
        else if (opcode == 3){ //sw
            // printf("SIMULATOR: THIS IN SW\n");
            if (state.numMemory <= (state.reg[rega] + OS)) {
                state.numMemory = state.reg[rega] + OS + 1;
            }
            // printf("THIS IS FIRST: %d and SECOND %d\n", state.reg[rega] + OS, state.reg[regb]);
            cache_access(state.reg[rega] + OS, 1, state.reg[regb]);
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
    printStats();
    printf("$$$ Main memory words accessed: ");
    printf("%d\n", num_mem_accesses);

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