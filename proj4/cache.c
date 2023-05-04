/*
 * EECS 370, University of Michigan
 * Project 4: LC-2K Cache Simulator
 * Instructions are found in the project spec.
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define MAX_CACHE_SIZE 256
#define MAX_BLOCK_SIZE 256

// **Note** this is a preprocessor macro. This is not the same as a function.
// Powers of 2 have exactly one 1 and the rest 0's, and 0 isn't a power of 2.
#define is_power_of_2(val) (val && !(val & (val - 1)))


/*
 * Accesses 1 word of memory.
 * addr is a 16-bit LC2K word address.
 * write_flag is 0 for reads and 1 for writes.
 * write_data is a word, and is only valid if write_flag is 1.
 * If write flag is 1, mem_access does: state.mem[addr] = write_data.
 * The return of mem_access is state.mem[addr].
 */
extern int mem_access(int addr, int write_flag, int write_data);

/*
 * Returns the number of times mem_access has been called.
 */
extern int get_num_mem_accesses(void);

//Use this when calling printAction. Do not modify the enumerated type below.
enum actionType
{
    cacheToProcessor,
    processorToCache,
    memoryToCache,
    cacheToMemory,
    cacheToNowhere
};

/* You may add or remove variables from these structs */
typedef struct blockStruct
{
    int data[MAX_BLOCK_SIZE];
    int dirty;
    int lruLabel;
    int tag;
    int valid;
    int set;
} blockStruct;

typedef struct cacheStruct
{
    blockStruct blocks[MAX_CACHE_SIZE];
    int blockSize;
    int numSets;
    int blocksPerSet;
} cacheStruct;

/* Global Cache variable */
cacheStruct cache;

void printAction(int, int, enum actionType);
void printCache(void);
void updateLRU(int set, int block);

/*
 * Set up the cache with given command line parameters. This is
 * called once in main(). You must implement this function.
 */
void cache_init(int blockSize, int numSets, int blocksPerSet)
{
    if (blockSize <= 0 || numSets <= 0 || blocksPerSet <= 0) {
        printf("error: input parameters must be positive numbers\n");
        exit(1);
    }
    if (blocksPerSet * numSets > MAX_CACHE_SIZE) {
        printf("error: cache must be no larger than %d blocks\n", MAX_CACHE_SIZE);
        exit(1);
    }
    if (blockSize > MAX_BLOCK_SIZE) {
        printf("error: blocks must be no larger than %d words\n", MAX_BLOCK_SIZE);
        exit(1);
    }
    if (!is_power_of_2(blockSize)) {
        printf("warning: blockSize %d is not a power of 2\n", blockSize);
    }
    if (!is_power_of_2(numSets)) {
        printf("warning: numSets %d is not a power of 2\n", numSets);
    }
    printf("Simulating a cache with %d total lines; each line has %d words\n",
        numSets * blocksPerSet, blockSize);
    printf("Each set in the cache contains %d lines; there are %d sets\n",
        blocksPerSet, numSets);

    // Your code here
    cache.blockSize = blockSize;
    cache.numSets = numSets;
    cache.blocksPerSet = blocksPerSet;
    for (int i = 0; i < numSets; i++){
        for (int j = 0; j < blocksPerSet; j++){
            cache.blocks[i*blocksPerSet + j].dirty = 0;
            cache.blocks[i*blocksPerSet + j].set = i;
            cache.blocks[i*blocksPerSet + j].valid = 0;
            cache.blocks[i*blocksPerSet + j].lruLabel = j;
        }
    }
    return;
}

/*
 * Access the cache. This is the main part of the project,
 * and should call printAction as is appropriate.
 * It should only call mem_access when absolutely necessary.
 * addr is a 16-bit LC2K word address.
 * write_flag is 0 for reads (fetch/lw) and 1 for writes (sw).
 * write_data is a word, and is only valid if write_flag is 1.
 * The return of mem_access is undefined if write_flag is 1.
 * Thus the return of cache_access is undefined if write_flag is 1.
 */
// int cache_access(int addr, int write_flag, int write_data)
// {
//     /* The next line is a placeholder to connect the simulator to
//     memory with no cache. You will remove this line and implement
//     a cache which interfaces between the simulator and memory. */
//     return mem_access(addr, write_flag, write_data);
// }
int cache_access(int addr, int write_flag, int write_data) {
    // Find tag bits, block bits and set bits
    int blockoffset = addr % cache.blockSize;
    int setIndex = (addr / cache.blockSize) % cache.numSets;
    int tagger = addr / (cache.blockSize * cache.numSets);

    // Search for hit in cache
    for (int i = setIndex * cache.blocksPerSet; i < (setIndex + 1) * cache.blocksPerSet; i++) {
        if (cache.blocks[i].tag == tagger && cache.blocks[i].valid == 1) {
            // Hit! Update LRU and handle write_flag
            // printf("Hit!\n");
            updateLRU(setIndex, i - setIndex * cache.blocksPerSet);
            if (write_flag) {
                printAction(addr, 1, processorToCache);
                cache.blocks[i].dirty = 1;
                cache.blocks[i].data[blockoffset] = write_data;
                return cache.blocks[i].data[blockoffset];
            } else {
                printAction(addr, 1, cacheToProcessor);
                return cache.blocks[i].data[blockoffset];
            }
        }
    }

    // Miss! Search for empty block in cache
    // this had an error, mem to cache in miss fixes 4-7 after 1-1.
    for (int i = setIndex * cache.blocksPerSet; i < (setIndex + 1) * cache.blocksPerSet; i++) {
        if (cache.blocks[i].valid == 0) {
            // Empty block found! Load data from memory
            // printf("Miss! Empty!\n");
            printAction(addr - blockoffset, cache.blockSize, memoryToCache);
            cache.blocks[i].valid = 1;
            cache.blocks[i].tag = tagger;
            updateLRU(setIndex, i - setIndex * cache.blocksPerSet);
            for (int j = 0; j < cache.blockSize; ++j) {
                cache.blocks[i].data[j] = mem_access(addr + j - blockoffset, 0, 0);
            }

            // Handle write_flag
            if (write_flag) {
                printAction(addr, 1, processorToCache);
                cache.blocks[i].dirty = 1;
                cache.blocks[i].data[blockoffset] = write_data;
                return cache.blocks[i].data[blockoffset];
            } else {
                printAction(addr, 1, cacheToProcessor);
                return cache.blocks[i].data[blockoffset];
            }
        }
    }

    // No empty blocks, evict LRU
    // printf("Miss! Evicting LRU...\n");
    int evictIndex = setIndex * cache.blocksPerSet;
    for (int i = setIndex * cache.blocksPerSet + 1; i < (setIndex + 1) * cache.blocksPerSet; i++) {
        if (cache.blocks[i].lruLabel > cache.blocks[evictIndex].lruLabel) {
            evictIndex = i;
        }
    }
    // Check if the evicted block is dirty, if so, write back to memory
    if (cache.blocks[evictIndex].dirty == 1) {
        printAction(cache.blocks[evictIndex].tag * (cache.blockSize * cache.numSets) + setIndex * cache.blockSize, cache.blockSize, cacheToMemory);
        for (int j = 0; j < cache.blockSize; ++j) {
            mem_access(cache.blocks[evictIndex].tag * (cache.blockSize * cache.numSets) + setIndex * cache.blockSize + j, 1, cache.blocks[evictIndex].data[j]);
        }
    }
    else{
        printAction(cache.blocks[evictIndex].tag * (cache.blockSize * cache.numSets) + setIndex * cache.blockSize, cache.blockSize, cacheToNowhere);
    }

    // Load data from memory to evicted block
    printAction(addr - blockoffset, cache.blockSize, memoryToCache);
    cache.blocks[evictIndex].tag = tagger;
    cache.blocks[evictIndex].valid = 1;
    cache.blocks[evictIndex].dirty = 0;
    updateLRU(setIndex, evictIndex - setIndex * cache.blocksPerSet);
    for (int j = 0; j < cache.blockSize; ++j) {
        cache.blocks[evictIndex].data[j] = mem_access(addr + j - blockoffset, 0, 0);
    }

    // Handle write_flag
    if (write_flag) {
        printAction(addr, 1, processorToCache);
        cache.blocks[evictIndex].dirty = 1;
        cache.blocks[evictIndex].data[blockoffset] = write_data;
        return cache.blocks[evictIndex].data[blockoffset];
    } else {
        printAction(addr, 1, cacheToProcessor);;
        return cache.blocks[evictIndex].data[blockoffset];
    }
}

/*
 * print end of run statistics like in the spec. **This is not required**,
 * but is very helpful in debugging.
 * This should be called once a halt is reached.
 * DO NOT delete this function, or else it won't compile.
 * DO NOT print $$$ in this function
 */
void printStats(void)
{
    printCache();
    return;
}

/*
 * Log the specifics of each cache action.
 *
 *DO NOT modify the content below.
 * address is the starting word address of the range of data being transferred.
 * size is the size of the range of data being transferred.
 * type specifies the source and destination of the data being transferred.
 *  -    cacheToProcessor: reading data from the cache to the processor
 *  -    processorToCache: writing data from the processor to the cache
 *  -    memoryToCache: reading data from the memory to the cache
 *  -    cacheToMemory: evicting cache data and writing it to the memory
 *  -    cacheToNowhere: evicting cache data and throwing it away
 */
void printAction(int address, int size, enum actionType type)
{
    printf("$$$ transferring word [%d-%d] ", address, address + size - 1);

    if (type == cacheToProcessor) {
        printf("from the cache to the processor\n");
    }
    else if (type == processorToCache) {
        printf("from the processor to the cache\n");
    }
    else if (type == memoryToCache) {
        printf("from the memory to the cache\n");
    }
    else if (type == cacheToMemory) {
        printf("from the cache to the memory\n");
    }
    else if (type == cacheToNowhere) {
        printf("from the cache to nowhere\n");
    }
    else {
        printf("Error: unrecognized action\n");
        exit(1);
    }

}

/*
 * Prints the cache based on the configurations of the struct
 * This is for debugging only and is not graded, so you may
 * modify it, but that is not recommended.
 */
void printCache(void)
{
    printf("\ncache:\n");
    for (int set = 0; set < cache.numSets; ++set) {
        printf("\tset %i:\n", set);
        for (int block = 0; block < cache.blocksPerSet; ++block) {
            printf("\t\t[ %i ]: {", block);
            for (int index = 0; index < cache.blockSize; index++) {
                printf(" %i", cache.blocks[set * cache.blocksPerSet + block].data[index]);
            }
            printf(" }\n");
        }
    }
    printf("end cache\n");
}
void updateLRU(int set, int block)
{
    for (int i = 0; i < cache.blocksPerSet; i++) {
        if (cache.blocks[set * cache.blocksPerSet + i].lruLabel < cache.blocks[set * cache.blocksPerSet + block].lruLabel) {
            cache.blocks[set * cache.blocksPerSet + i].lruLabel++;
        }
    }
    cache.blocks[set * cache.blocksPerSet + block].lruLabel = 0;
}