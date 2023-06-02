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
		}

		// read in data section
		int data;
		for (j = 0; j < sizeData; j++) {
			fgets(line, MAXLINELENGTH, inFilePtr);
			data = atoi(line);
			files[i].data[j] = data;
		}

		// read in the symbol table
		char label[7];
		char type;
		int addr;
		for (j = 0; j < sizeSymbol; j++) {
			fgets(line, MAXLINELENGTH, inFilePtr);
			sscanf(line, "%s %c %d",
					label, &type, &addr);
			files[i].symbolTable[j].offset = addr;
			strcpy(files[i].symbolTable[j].label, label);
			files[i].symbolTable[j].location = type;
		}

		// read in relocation table
		char opcode[7];
		for (j = 0; j < sizeReloc; j++) {
			fgets(line, MAXLINELENGTH, inFilePtr);
			sscanf(line, "%d %s %s",
					&addr, opcode, label);
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
	SymbolTableEntry Utable[MAXSIZE];
	combiner.symTableSize = 0;
	combiner.textSize = 0;
	combiner.dataSize = 0;

	for (int i = 0, j = 0; j < argc - 2; j++) {
		files[j].textStartingLine = combiner.textSize;
		for (int  a = 0; a < files[j].textSize; a++, i++) {
			combiner.text[i] = files[j].text[a];
			combiner.textSize++;
		}
	}
	
	for (int i = 0, j = 0; j < argc - 2; j++) {
		files[j].dataStartingLine = combiner.textSize + combiner.dataSize;
		for (int  a = 0; a < files[j].dataSize; a++, i++) {
			combiner.data[i] = files[j].data[a] + files[j].textSize;
			combiner.dataSize++;
		}
	}
	
	int tempdata = 0, temptext = 0, uind = 0;
    for (int i = 0; i < argc - 2; i++){
        for (int j = 0; j < files[i].symbolTableSize; j++){
			for (int a = 0; a < combiner.symTableSize; a++){
				if (!strcmp(combiner.symTable[a].label, files[i].symbolTable[j].label) && files[i].symbolTable[j].location != 'U'){
					printf("Duplicated defined global label.\n");
					exit(1);
				}  
            }
            if (!strcmp(files[i].symbolTable[j].label, "Stack") && files[i].symbolTable[j].location != 'U'){
                printf("Stack defined by object file.\n");
                exit(1);
            }
			if (files[i].symbolTable[j].location == 'U'){
				Utable[uind].location = files[i].symbolTable[j].location;
				strcpy(Utable[uind].label, files[i].symbolTable[j].label);
				uind++;
			}
			else{
				if (files[i].symbolTable[j].location == 'D'){
					printf("THIS IS THE COMPONENTS: %d; %d; %d\n", files[i].symbolTable[j].offset, tempdata, combiner.textSize);
					printf("THIS IS OFFSETS: %d\n", combiner.symTable[combiner.symTableSize].offset);
					combiner.symTable[combiner.symTableSize].offset = files[i].symbolTable[j].offset + tempdata;
				}
				else if (files[i].symbolTable[j].location == 'T'){
					combiner.symTable[combiner.symTableSize].offset = files[i].symbolTable[j].offset + temptext;
				}
				else{
					printf("Location not found\n");
					exit(1);
				}
				strcpy(combiner.symTable[combiner.symTableSize].label, files[i].symbolTable[j].label);
				combiner.symTable[combiner.symTableSize].location = files[i].symbolTable[j].location;
				combiner.symTableSize++;
			}
        }
		tempdata += files[i].dataSize;
		temptext += files[i].textSize;
    }

	// The stack array starts at the implicit label Stack and extends to larger addresses,
	// which is why the linker inserts the Stack label as the last line in the final executable..
	strcpy(combiner.symTable[combiner.symTableSize].label, "Stack");
	combiner.symTable[combiner.symTableSize].location = 'D';
	combiner.symTable[combiner.symTableSize].offset = combiner.textSize + combiner.dataSize;
	combiner.symTableSize++;

	for(int i = 0; i < uind; ++i){
		int findone = 0;
		for(int j = 0; j < combiner.symTableSize; ++j){
			if(!strcmp(Utable[i].label, combiner.symTable[j].label)){
				findone = 1;
			}
		}
		if(!findone){
			printf("Undefined label\n");
			exit(1);
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

    printf("%d %d %d %d\n", combiner.textSize, combiner.dataSize, combiner.symTableSize, combiner.relocTableSize);
	for (int i = 0; i < combiner.symTableSize; i++){
		printf("THIS IS offset at %d: %d at %s\n", i, combiner.symTable[i].offset, combiner.symTable[i].label);
	}

	for (int i = 0; i < argc - 2; i++){
		for (int j = 0; j < files[i].relocationTableSize; j++){
			int relocoff = 0;
			if (isupper(files[i].relocTable[j].label[0])){ //GLOBAL
				if (!strcmp(files[i].relocTable[j].label, "Stack")){
					relocoff = combiner.textSize + combiner.dataSize;
					if(!strcmp(files[i].relocTable[j].inst, ".fill")){
						printf("in the stack fill\n");
						printf("this is i and j: %d and %d\n", i , j);
						printf("this is combiner data %d\n", combiner.data[files[i].dataStartingLine - combiner.textSize + files[i].relocTable[j].offset]);
						combiner.data[files[i].dataStartingLine - combiner.textSize + files[i].relocTable[j].offset] += relocoff;
						printf("this is combiner data %d\n", combiner.data[files[i].dataStartingLine - combiner.textSize + files[i].relocTable[j].offset]);
					}
					else{
						printf("in the not stack fill\n");
						combiner.text[files[i].textStartingLine + files[i].relocTable[j].offset] += relocoff;
					}
					printf("ths wil continue\n");
					continue;
				}
				for (int k = 0; k < combiner.symTableSize; k++){
					if (!strcmp(combiner.symTable[k].label, files[i].relocTable[j].label)){
						if(combiner.symTable[k].location == 'T'){
							relocoff = combiner.symTable[k].offset;
                            printf("this is offset: %d+++++++++++++++++++\n", relocoff);
						}
						else if(combiner.symTable[k].location == 'D'){
                            printf("IN THE D SECITOSDJBHB: %d and %d",combiner.symTable[k].offset, combiner.textSize );
							relocoff = combiner.symTable[k].offset + combiner.textSize;
                            printf("this is offset IN d: %d++++++++++++++++++++\n", relocoff);
						}
					}
				}
				if(!strcmp(files[i].relocTable[j].inst, ".fill")){
					combiner.data[files[i].relocTable[j].offset + files[i].dataStartingLine - combiner.textSize] = relocoff;
					printf("in the .fill %d\n", combiner.data[files[i].relocTable[j].offset + files[i].dataStartingLine - combiner.textSize]);
				}
				else{ //LW AND SW
					printf("this is else %d\n", combiner.data[files[i].relocTable[j].offset + files[i].dataStartingLine - combiner.textSize]);
					printf("replcacefsdhjfvs: %d\n", combiner.text[files[i].relocTable[j].offset + files[i].textStartingLine]);
					combiner.text[files[i].relocTable[j].offset + files[i].textStartingLine] = combiner.text[files[i].relocTable[j].offset + files[i].textStartingLine] - (combiner.text[files[i].relocTable[j].offset + files[i].textStartingLine] & 0xFFFF) + relocoff;
				}			}
			else{ // LOCAL
				int offloc = (!strcmp(files[i].relocTable[j].inst, ".fill")) ? files[i].data[files[i].relocTable[j].offset] : files[i].text[files[i].relocTable[j].offset] & 0xFFFF;
				if (offloc < files[i].textSize){
					relocoff = offloc + files[i].textStartingLine;
				}
				else{
					printf("in the d\n");
					// printf("THESE ARE COMPONENTS: %d; %d; %d; %d\n");
					// files[i].data[files[i].relocTable[j].offset] += (combiner.textSize - files[i].textSize) + files[i].dataStartingLine;
					relocoff = offloc + files[i].dataStartingLine - files[i].textSize;
				}
				if(!strcmp(files[i].relocTable[j].inst, ".fill")){
					combiner.data[files[i].relocTable[j].offset + files[i].dataStartingLine - combiner.textSize] = relocoff;
					printf("in the .fill %d\n", combiner.data[files[i].relocTable[j].offset + files[i].dataStartingLine - combiner.textSize]);
				}
				else{ //LW AND SW
					printf("this is else %d\n", combiner.data[files[i].relocTable[j].offset + files[i].dataStartingLine - combiner.textSize]);
					printf("replcacefsdhjfvs: %d\n", combiner.text[files[i].relocTable[j].offset + files[i].textStartingLine]);
					combiner.text[files[i].relocTable[j].offset + files[i].textStartingLine] = combiner.text[files[i].relocTable[j].offset + files[i].textStartingLine] - (combiner.text[files[i].relocTable[j].offset + files[i].textStartingLine] & 0xFFFF) + relocoff;
				}
			}
		}
	}

	for (int i = 0; i < combiner.textSize; i++)
	{
		fprintf(outFilePtr,"%d\n",combiner.text[i]);
		// fprintf(outFilePtr,"%d\n", 999);
	}
	for (int i = 0; i < combiner.dataSize; i++)
	{
		fprintf(outFilePtr,"%d\n",combiner.data[i]);
	}
} // main
