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
			printf("THIS IS DATA: %d and %d\n", files[i].data[j], data);
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
		
		printf("THIS IS SIZE1: %d\n", files[j].textSize);
		files[j].textStartingLine = combiner.textSize;
		for (int  a = 0; a < files[j].textSize; a++, i++) {
			
			combiner.text[i] = files[j].text[a];
			combiner.textSize++;
		}
	}
	printf("THIS IS SIZE: %d\n", combiner.textSize);
	
	for (int i = 0, j = 0; j < argc - 2; j++) {
		printf("this is: %d\n", argc - 2);
		files[j].dataStartingLine = combiner.textSize + combiner.dataSize;
		for (int  a = 0; a < files[j].dataSize; a++, i++) {
			
			combiner.data[i] = files[j].data[a] + files[j].textSize;
			printf("this is combiner data after: %d ---------------------\n", combiner.data[i]);
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
			} //for test1 it needs to be +11 by +3, but it is currently +1
			else {
				combiner.symTable[combiner.symTableSize].offset = files[i].symbolTable[j].offset + temptext;
				
			}
            combiner.symTableSize++;
            t++;
			
			//moved above
			printf("this is added offset: %d\n", files[i].symbolTable[j].offset);
			printf("this is the datasize: %d\n", files[i].dataSize);
			// tempdata += files[i].symbolTable[j].offset + files[t].dataSize;
			// temptext += files[i].symbolTable[j].offset + files[t].textSize;
			// temptext += files[t].textSize;
        }
		tempdata += files[i].dataSize;
		temptext += files[i].textSize;
    }

    printf("%d %d %d %d\n", combiner.textSize, combiner.dataSize, combiner.symTableSize, combiner.relocTableSize);
	for (int i =0; i < combiner.symTableSize; i++){
		printf("THIS IS offset at %d: %d at %s\n", i, combiner.symTable[i].offset, combiner.symTable[i].label);
	}

	printf("THIS IS AGFTER COMBIENR\n");
	printf("combinerdatasize: %d\n", combiner.dataSize);
	

    for (int i = 0; i < argc - 2; i++){
        for (int j = 0; j < files[i].relocationTableSize; j++){
			printf("THIS IS %d and j: %d AND LABEL: %s\n", files[i].relocationTableSize, j, files[i].relocTable[j].label);
			printf("this is firsty letter to check for globalahdsnfhusdgvfd: %s\n", files[i].relocTable[j].label);
			int reloc_offset = files[i].relocTable[j].offset;
         	printf("this is reloc offset %d\n", reloc_offset);
            if (!strcmp(files[i].relocTable[j].inst, ".fill")){
				if(!strcmp(files[i].relocTable[j].label, "Stack")){
					reloc_offset = combiner.textSize + combiner.dataSize;
					if(!strcmp(files[i].relocTable[j].inst, ".fill")){
						printf("in the stack fill\n");
						printf("this is combiner data1 %d\n", combiner.data[files[i].relocTable[j].offset]);
						// printf("this is combiner data %d\n", 1526);
						combiner.data[files[i].dataStartingLine - combiner.textSize + files[i].relocTable[j].offset] = reloc_offset;
						printf("this is i and j: %d and %d\n", i, j);
						files[j -i].data[reloc_offset] = combiner.data[files[i].dataStartingLine - combiner.textSize + files[i].relocTable[j].offset];
						printf("this is combiner data %d\n", combiner.data[files[i].dataStartingLine - combiner.textSize + files[i].relocTable[j].offset]);
					}
					else{
						printf("in the not stack fill\n");
						combiner.text[files[i].textStartingLine + files[i].relocTable[j].offset] += reloc_offset;
					}
					printf("THIS WILL CONTINUEDBYUSDGVSAVGHSVFGHSVFGHSVTFYUBSDHFJOSDBYUFBDS\n");
					continue;
				}
                if ((files[i].relocTable[j].label[0] <= 'Z' && files[i].relocTable[j].label[0] >= 'A')){
					printf("	FILL GLOBALS	\n");
					// if fill add on data for stack offset else add text. or add stack offset to data and to text if not
					// add to text if not fill global stack
					printf("THIS IS BEFORE: %d\n", files[i].text[files[i].relocTable[j].offset]);
					int big = 0, FGoffset = 0;
					printf("this is firsty letter to check for globalahdsnfhusdgvfd: %s\n", files[i].relocTable[j].label);
					for (int k = 0; k < combiner.symTableSize; k++) {
						if (strcmp(combiner.symTable[k].label, files[i].relocTable[j].label) == 0) {
							// modify between these
							if (combiner.symTable[k].location == 'T'){
								FGoffset = combiner.symTable[k].offset;
								printf("this is FGOFFSET: %d", FGoffset);
							}
							else if (combiner.symTable[k].location == 'D'){
								FGoffset = combiner.symTable[k].offset;
								printf("this is FGOFFSET in D: %d", FGoffset);
							}
							//modify between these
							printf("THIS IS K: %d\n", k);
							big = k;
							break;
						}
					}
					if (big == combiner.symTableSize){
						printf("does it go into 257 big check\n");
						if (!strcmp(files[i].relocTable[j].label,"Stack")){
							files[i].data[files[i].relocTable[j].offset] = combiner.dataSize + combiner.textSize;
							printf("260 MODIFYING DATA VAL: %d\n", files[i].relocTable[j].offset);
							// files[i].data[files[i].relocTable[j].offset] = 9999;
						}
						else {
							printf("Undefined global label used in .fill: %s\n",files[i].relocTable[j].label);
							exit(1);
						}
					}
					else{
						// files[i].data[files[i].relocTable[j].offset] += offglobal(combiner.relocTable[j].label, files, combiner, i, (argc - 2), j, big);
						files[i].data[files[i].relocTable[j].offset] = FGoffset;
						printf("it went into this fgodffset else MODIFYING DATA VAL:MODIFYING DATA VAL: %d\n", FGoffset);
					}
					printf("THIS IS after: %d\n", files[i].text[files[i].relocTable[j].offset]);
                }
                else{//for the locals
					printf("	FILL LOCALS	\n");
					printf("testing: dfashjfgsu fgdwshujfvsed: %d\n", files[i].data[files[i].relocTable[j].offset]);
					printf("THIS IS BEFORE: %d and %d\n", files[i].text[files[i].relocTable[j].offset], files[i].relocTable[j].offset);
					printf("and this: %d and %d\n", (files[i].data[files[i].relocTable[j].offset]&0xFFFF), files[i].textSize);

					int FGoffset = 0;
					printf("this is label ppppppppppppppppppppppppppppppppppp %s\n", files[i].relocTable[j].label);
					if (!strcmp(files[i].relocTable[j].label, "Stack")){
						FGoffset = combiner.textSize + combiner.dataSize;
						combiner.data[files[i].dataStartingLine - combiner.textSize + files[i].relocTable[j].offset] += FGoffset;
						printf("this is final data %d\n", combiner.data[files[i].dataStartingLine - combiner.textSize + files[i].relocTable[j].offset]);
					}


					if ((files[i].data[files[i].relocTable[j].offset]&0xFFFF) < files[i].textSize){
						printf("+++++++++++++++++++++++++++++++++++++++++BEFORE: %d and %d\n", files[i].data[files[i].relocTable[j].offset], files[i].relocTable[j].offset);
						files[i].data[files[i].relocTable[j].offset] += files[i].textStartingLine;
						// files[i].data[files[i].relocTable[j].offset] += files[i].dataStartingLine;
						printf("+++++++++++++++++++++++++++++++++++++++++after1 MODIFYING DATA VAL:: %d\n", files[i].data[files[i].relocTable[j].offset]);
						printf("in here 1\n");
					}
					// lD
					else{
						printf("in the d\n");
						// printf("THESE ARE COMPONENTS: %d; %d; %d; %d\n");
						files[i].data[files[i].relocTable[j].offset] += (combiner.textSize - files[i].textSize) + files[i].dataStartingLine;
						printf("MODIFYING DATA VAL: %d", files[i].data[files[i].relocTable[j].offset]);
						// 6/1 modify of ths line
						// files[i].data[files[i].relocTable[j].offset] -= files[i].relocTable[j].offset;
					}

					printf("THIS IS after: %d\n", files[i].text[files[i].relocTable[j].offset]);
                }
            }
            else{// lw sw
                if ((combiner.relocTable[j].label[0] <= 'Z' && combiner.relocTable[j].label[0] >= 'A')){
					printf("	FILL GLOBALS	\n");
					// if fill add on data for stack offset else add text. or add stack offset to data and to text if not
					// add to text if not fill global stack
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
						printf("this is going in the stack\n");
						if (!strcmp(files[i].relocTable[j].label,"Stack")){
							files[i].data[files[i].relocTable[j].offset] = combiner.dataSize + combiner.textSize;
							printf("MODIIFYU DATA GVAL: %d", files[i].data[files[i].relocTable[j].offset]);
						}
						else {
							printf("Undefined global label used in .fill: %s\n",files[i].relocTable[j].label);
							exit(1);
						}
					}
					else{
						files[i].data[files[i].relocTable[j].offset] += offglobal(combiner.relocTable[j].label, files, combiner, i, (argc - 2), j, big);
					}
					printf("THIS IS after : %d\n", files[i].text[files[i].relocTable[j].offset]);
                }
				printf("this is label check: %s\n", files[i].relocTable[j].label);
                if (files[i].relocTable[j].label[0] <= 'Z' && files[i].relocTable[j].label[0] >= 'A'){
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
							printf("312: %d, %d, %d\n", files[i].data[files[i].relocTable[j].offset], combiner.dataSize, combiner.textSize);
							files[i].text[files[i].relocTable[j].offset] += combiner.dataSize + combiner.textSize;
							// printf("this is after")
						}
						else {
							printf("Undefined global label used in lw/sw aya: %s\n",files[i].relocTable[j].label);
							exit(1);
						}
					}
                    else{
						printf("HERE2 and %d and the curr opffset %d\n", j, files[i].text[files[i].relocTable[j].offset]);
						//error is here latest
                        files[i].text[files[i].relocTable[j].offset] = offglobal(combiner.relocTable[j].label, files, combiner, i, (argc - 2), j, big);
                    }
					printf("THIS IS after: %d\n", files[i].text[files[i].relocTable[j].offset]);
                }
                else{//for the locals 
					printf("	LWSW LOCALS	\n");
					printf("THIS IS BEFORE: %d\n", files[i].text[files[i].relocTable[j].offset]); //offset error

					printf("this is before 293\n");
					if ((files[i].text[files[i].relocTable[j].offset]&0xFFFF) < files[i].textSize){
						printf("it goes in 295\n");
						files[i].text[files[i].relocTable[j].offset] += files[i].textStartingLine;
						
					}
					// D
					else{
						printf("in the else\n");
						// printf("THIS IS VALUE TO OFFSET: %d\n", (combiner.textSize - files[i].textSize) + files[i].textStartingLine);

						printf("THIS IS VALUE TO OFFSET: %d and data start %d and tesize %d\n", (files[i].text[files[i].relocTable[j].offset]&0xFFFF), files[i].dataStartingLine, files[i].textSize);
						// files[i].text[files[i].relocTable[j].offset] += (combiner.textSize - files[i].textSize) + files[i].textStartingLine;
						//PROBLEM IS HERE NEEXT TO DSDFBAHSJDBS FIX
						int maskeder = (files[i].text[files[i].relocTable[j].offset]&0xFFFF) - files[i].textSize + files[i].dataStartingLine;
						printf("hmmm: %d, %d, %d\n", maskeder, combiner.text[files[i].relocTable[j].offset + files[i].textStartingLine] & 0xFFFF, combiner.text[files[i].relocTable[j].offset + files[i].textStartingLine]);
						
						combiner.text[files[i].relocTable[j].offset + files[i].textStartingLine] -= combiner.text[files[i].relocTable[j].offset + files[i].textStartingLine] & 0xFFFF;
						combiner.text[files[i].relocTable[j].offset + files[i].textStartingLine] += maskeder;
						files[i].text[files[i].relocTable[j].offset] = combiner.text[files[i].relocTable[j].offset + files[i].textStartingLine];
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
			printf("THIS IS: %d\n", combiner.text[tsize + i]);
		}
		for (int i = 0; i < files[k].dataSize; i++)
		{
			combiner.data[dsize + i] = files[k].data[i];
			printf("this is i and j: %d and %d\n", i, j);
			printf("THIS IS DATA [%d]: %d\n", i, files[k].data[i]);
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
		printf("THIS IS DHSVDGSHABDGYSAUDSAHDGSADHAS: %d and %d\n", j, i);
		// temp = files[i].text[files[i].relocTable[j].offset] + combiner.symTable[big].offset - (files[i].textSize + (files[i].symbolTable[small].offset & 0xFFFF)) + j;
		temp = files[i].text[files[i].relocTable[j].offset] + combiner.symTable[big].offset - (combiner.text[files[i].relocTable[j].offset + files[i].textStartingLine] & 0xFFFF);
		printf("comvi: %d, %d\n", combiner.symTable[big].offset, combiner.textSize);
		printf("replcacefsdhjfvs: %d\n", combiner.text[files[i].relocTable[j].offset + files[i].textStartingLine]);
		printf("THIS IS REPLACE: %d", combiner.text[files[i].relocTable[j].offset + files[i].textStartingLine] & 0xFFFF);
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