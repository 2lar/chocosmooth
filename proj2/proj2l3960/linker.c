/**
 * Project 2
 * LC-2K Linker
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define MAXSIZE 300
#define MAXLINELENGTH 1000
#define MAXFILES 6

typedef struct FileData FileData;
typedef struct SymbolTableEntry SymbolTableEntry;
typedef struct RelocationTableEntry RelocationTableEntry;
typedef struct CombinedFiles CombinedFiles;

struct SymbolTableEntry {
	char label[7];
	char location;
	int offset;
};

struct RelocationTableEntry {
	int offset;
	char inst[7];
	char label[7];
	int file;
};

struct FileData {
	int textSize;
	int dataSize;
	int symbolTableSize;
	int relocationTableSize;
	int textStartingLine; // in final executable
	int dataStartingLine; // in final executable
	int text[MAXSIZE];
	int data[MAXSIZE];
	SymbolTableEntry symbolTable[MAXSIZE];
	RelocationTableEntry relocTable[MAXSIZE];
};

struct CombinedFiles {
	int text[MAXSIZE];
	int data[MAXSIZE];
	SymbolTableEntry     symTable[MAXSIZE];
	RelocationTableEntry relocTable[MAXSIZE];
	int textSize;
	int dataSize;
	int symTableSize;
	int relocTableSize;
};

int offglobal(char*, FileData files[MAXFILES], CombinedFiles combiner, int, int, int, int);

int main(int argc, char *argv[])
{
	char *inFileString, *outFileString;
	FILE *inFilePtr, *outFilePtr; 
	int i, j;

	if (argc <= 2) {
		printf("error: usage: %s <obj file> ... <output-exe-file>\n",
				argv[0]);
		exit(1);
	}

	outFileString = argv[argc - 1];

	outFilePtr = fopen(outFileString, "w");
	if (outFilePtr == NULL) {
		printf("error in opening %s\n", outFileString);
		exit(1);
	}

	FileData files[MAXFILES];

  // read in all files and combine into a "master" file
	for (i = 0; i < argc - 2; i++) {
		inFileString = argv[i+1];

		inFilePtr = fopen(inFileString, "r");
		printf("opening %s\n", inFileString);

		if (inFilePtr == NULL) {
			printf("error in opening %s\n", inFileString);
			exit(1);
		}

		char line[MAXLINELENGTH];
		int sizeText, sizeData, sizeSymbol, sizeReloc;

		// parse first line of file
		fgets(line, MAXSIZE, inFilePtr);
		sscanf(line, "%d %d %d %d",
				&sizeText, &sizeData, &sizeSymbol, &sizeReloc);

		files[i].textSize = sizeText;
		files[i].dataSize = sizeData;
		files[i].symbolTableSize = sizeSymbol;
		files[i].relocationTableSize = sizeReloc;

		// read in text section
		int instr;
		for (j = 0; j < sizeText; j++) {
			fgets(line, MAXLINELENGTH, inFilePtr);
			instr = atoi(line);
			files[i].text[j] = instr;
			printf("THIS IS TEXT: %d\n", files[i].text[j]);
		}

		// read in data section
		int data;
		for (j = 0; j < sizeData; j++) {
			fgets(line, MAXLINELENGTH, inFilePtr);
			data = atoi(line);
			files[i].data[j] = data;
			printf("THIS IS DATA: %d\n", files[i].data[j]);
		}

		// read in the symbol table
		char label[7];
		char type;
		int addr;
		for (j = 0; j < sizeSymbol; j++) {
			fgets(line, MAXLINELENGTH, inFilePtr);
			sscanf(line, "%s %c %d", label, &type, &addr);
			files[i].symbolTable[j].offset = addr;
			strcpy(files[i].symbolTable[j].label, label);
			files[i].symbolTable[j].location = type;
		}

		// read in relocation table
		char opcode[7];
		for (j = 0; j < sizeReloc; j++) {
			fgets(line, MAXLINELENGTH, inFilePtr);
			sscanf(line, "%d %s %s", &addr, opcode, label);
			files[i].relocTable[j].offset = addr;
			strcpy(files[i].relocTable[j].inst, opcode);
			strcpy(files[i].relocTable[j].label, label);
			files[i].relocTable[j].file	= i;
		}
		fclose(inFilePtr);
	} // end reading files

	// *** INSERT YOUR CODE BELOW ***
	//    Begin the linking process
	//    Happy coding!!!

	CombinedFiles combiner;
	combiner.textSize = 0;
	combiner.dataSize = 0;

	for (int i = 0, j = 0; j < argc - 2; j++) {
		files[j].textStartingLine = i;
		printf("THIS IS SIZE: %d\n", files[j].textSize);
		for (int  a = 0; a < files[j].textSize; a++, i++) {
			combiner.text[i] = files[j].text[a];
			combiner.textSize++;
		}
	}
	printf("THIS IS SIZE: %d\n", combiner.textSize);
	
	for (int i = 0, j = 0; j < argc - 2; j++) {
		files[j].dataStartingLine = i;
		for (int  a = 0; a < files[j].dataSize; a++, i++) {
			combiner.data[i] = files[j].data[a];
			combiner.dataSize++;
		}
	}

	int tempdata = 0, temptext = 0;
    for (int t = 0, i = 0; i < argc - 2; i++){
        for (int j = 0; j < files[i].symbolTableSize; j++){
            if (files[i].symbolTable[j].location == 'U') continue;
            if (!strcmp(files[i].symbolTable[j].label, "Stack")){
                printf("Stack defined by object file.\n");
                exit(1);
            }
            for (int a = 0; a < combiner.symTableSize; a++){
				if (!strcmp(combiner.symTable[a].label, files[i].symbolTable[j].label) && combiner.symTable[a].location != 'U'){
					printf("Duplicated defined global label.\n");
					exit(1);
				}  
            }
            strcpy(combiner.symTable[t].label, files[i].symbolTable[j].label);
            combiner.symTable[t].location = files[i].symbolTable[j].location;
			if (files[i].symbolTable[j].location == 'D'){
				printf("THIS IS THE COMPONENTS: %d; %d; %d\n", files[i].symbolTable[j].offset, tempdata, combiner.textSize);
				printf("THIS IS OFFSETS: %d\n", combiner.symTable[combiner.symTableSize].offset);
				combiner.symTable[combiner.symTableSize].offset = files[i].symbolTable[j].offset + combiner.textSize + tempdata;
			}
			else {
				combiner.symTable[combiner.symTableSize].offset = files[i].symbolTable[j].offset + temptext;
				
			}
            combiner.symTableSize++;
            t++;
			
        }
		tempdata += files[t].dataSize;
		temptext += files[t].textSize;
    }

    printf("%d %d %d %d\n", combiner.textSize, combiner.dataSize, combiner.symTableSize, combiner.relocTableSize);
	for (int i =0; i < combiner.symTableSize; i++){
		printf("THIS IS offset at %d: %d at %s\n", i, combiner.symTable[i].offset, combiner.symTable[i].label);
	}

	printf("THIS IS AGFTER COMBIENR\n");
	printf("combinerdatasize: %d\n", combiner.dataSize);

    for (int i = 0; i < argc - 2; i++){
        for (int j = 0; j < files[i].relocationTableSize; j++){
            if (!strcmp(files[i].relocTable[j].inst, ".fill")){
                if (isupper(combiner.relocTable[j].label[0])){
					printf("	FILL GLOBALS	\n");
					printf("THIS IS BEFORE: %d\n", files[i].text[files[i].relocTable[j].offset]);
					int big = 0;
					for (int k = 0; k < combiner.symTableSize; k++) {
						if (strcmp(combiner.symTable[k].label, files[i].relocTable[j].label) == 0) {
							printf("THIS IS K: %d\n", k);
							big = k;
							break;
						}
					}
					if (big == combiner.symTableSize){
						if (!strcmp(files[i].relocTable[j].label,"Stack")){
							files[i].data[files[i].relocTable[j].offset] = combiner.dataSize + combiner.textSize;
						}
						else {
							printf("Undefined global label used in .fill: %s\n",files[i].relocTable[j].label);
							exit(1);
						}
					}
					else{
						files[i].data[files[i].relocTable[j].offset] += offglobal(combiner.relocTable[j].label, files, combiner, i, (argc - 2), j, big);
					}
					printf("THIS IS after: %d\n", files[i].text[files[i].relocTable[j].offset]);
                }
                else{//for the locals
					printf("	FILL LOCALS	\n");
					printf("THIS IS BEFORE: %d and %d\n", files[i].text[files[i].relocTable[j].offset], files[i].relocTable[j].offset);
					if ((files[i].data[files[i].relocTable[j].offset]&0xFFFF) < files[i].textSize){
						files[i].data[files[i].relocTable[j].offset] += files[i].textStartingLine;
					}
					// lD
					else{
						printf("in the d\n");
						// printf("THESE ARE COMPONENTS: %d; %d; %d; %d\n");
						files[i].data[files[i].relocTable[j].offset] += (combiner.textSize - files[i].textSize) + files[i].dataStartingLine;
					}
					printf("THIS IS after: %d\n", files[i].text[files[i].relocTable[j].offset]);
                }
            }
            else{// lw sw
				printf("this is label check: %s\n", files[i].relocTable[j].label);
                if (isupper(files[i].relocTable[j].label[0])){
					printf("	SWKW GLOBALS	\n");
					printf("THIS IS BEFORE: %d\n", files[i].text[files[i].relocTable[j].offset]);
					int big = 0;
					for (int k = 0; k < combiner.symTableSize; k++) {
						printf("this is combinert label: %s\n", combiner.symTable[k].label);
						if (strcmp(combiner.symTable[k].label, files[i].relocTable[j].label) == 0) {
							// big = k;
							printf("THIS IS K: %d\n", k);
							break;
						}
						big++;
					}
					if (big == combiner.symTableSize){
						printf("HERE1\n");
						if (!strcmp(files[i].relocTable[j].label,"Stack")){
							files[i].data[files[i].relocTable[j].offset] += combiner.dataSize + combiner.textSize;
						}
						else {
							printf("Undefined global label used in lw/sw aya: %s\n",files[i].relocTable[j].label);
							exit(1);
						}
					}
                    else{
						printf("HERE2\n");
                        files[i].text[files[i].relocTable[j].offset] = offglobal(combiner.relocTable[j].label, files, combiner, i, (argc - 2), j, big);
                    }
					printf("THIS IS after: %d\n", files[i].text[files[i].relocTable[j].offset]);
                }
                else{//for the locals 
					printf("	LWSW LOCALS	\n");
					printf("THIS IS BEFORE: %d\n", files[i].text[files[i].relocTable[j].offset]);
					if ((files[i].text[files[i].relocTable[j].offset]&0xFFFF) < files[i].textSize){
						files[i].text[files[i].relocTable[j].offset] += files[i].textStartingLine;
						
					}
					// D
					else{
						files[i].text[files[i].relocTable[j].offset] += (combiner.textSize - files[i].textSize) + files[i].dataStartingLine;
					}
					printf("THIS IS after: %d\n", files[i].text[files[i].relocTable[j].offset]);
                }
            }
        }
    }

	for (int tsize = 0, dsize = 0, k = 0; k < argc - 2; k++){
		for (int i = 0; i < files[k].textSize; i++)
		{
			combiner.text[tsize + i] = files[k].text[i];
		}
		for (int i = 0; i < files[k].dataSize; i++)
		{
			combiner.data[dsize + i] = files[k].data[i];
		}
		tsize += files[k].textSize;
		dsize += files[k].dataSize;
	}

	for (int i = 0; i < combiner.textSize; i++)
	{
		fprintf(outFilePtr,"%d\n",combiner.text[i]);
	}
	for (int i = 0; i < combiner.dataSize; i++)
	{
		fprintf(outFilePtr,"%d\n",combiner.data[i]);
	}
	// fprintf(outFilePtr, "CHECKING\n");


} //end main

int offglobal(char* label, FileData files[MAXFILES], CombinedFiles combiner, int i, int size, int j, int big){
	int temp = -1;
	int small = 0;
	printf("THIS IS J: %d\n", j);
	for (int a = 0; a < files[i].symbolTableSize; a++) {
		if (strcmp(files[i].symbolTable[a].label, files[i].relocTable[j].label) == 0) {
			small = a;
			break;
		}
		if (small == files[i].symbolTableSize) exit(1);
	}
	if (files[i].symbolTable[small].location == 'D'){
		temp = files[i].text[files[i].relocTable[j].offset] + combiner.symTable[big].offset - (files[i].textSize + (files[i].symbolTable[small].offset & 0xFFFF)) + i;
	}
	else{
		printf("IN T\n");
		printf("THIS IS BIG AND SMALL: %d and %d\n", big, small);
		printf("this is offsets: %d and %d\n", files[i].text[files[i].relocTable[j].offset], combiner.symTable[big].offset);
		temp = files[i].text[files[i].relocTable[j].offset] + combiner.symTable[big].offset - (files[i].symbolTable[small].offset & 0xFFFF);
	}
    if (temp < 0){
        printf("Undefined global label error.\n");
        exit(1);
    }
    return temp;
}

//done