/**
 * Project 2
 * LC-2K Linker
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

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
	int textStartingLine; // in combiner executable
	int dataStartingLine; // in combiner executable
	int text[MAXSIZE];
	int data[MAXSIZE];
	SymbolTableEntry symbolTable[MAXSIZE];
	RelocationTableEntry relocTable[MAXSIZE];
};

struct CombinedFiles {
	int text[MAXSIZE];
	int data[MAXSIZE];
	SymbolTableEntry symTable[MAXSIZE];
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

	int numFiles = argc - 2;
	CombinedFiles combiner;

	// set up starting lines
	combiner.textSize = 0;
	for(int i = 0; i < numFiles; ++i){
		files[i].textStartingLine = combiner.textSize;
		combiner.textSize += files[i].textSize;
	}

	combiner.dataSize = 0;
	for(int i = 0; i < numFiles; ++i){
		files[i].dataStartingLine = combiner.dataSize + combiner.textSize;
		combiner.dataSize += files[i].dataSize;
	}

	//check for duplicate globals
	combiner.symTableSize = 0;
	int currTextLine = 0;
	int currDataLine = 0;
	SymbolTableEntry Utable[MAXSIZE];
	int uind = 0;

	for(int i = 0; i < numFiles; ++i){
		for(int j = 0; j < files[i].symbolTableSize; ++j){
			for(int k = 0; k < combiner.symTableSize; ++k){
				if(files[i].symbolTable[j].location != 'U' && !strcmp(files[i].symbolTable[j].label, combiner.symTable[k].label)){
					printf("Duplicate Label");
					exit(1);
				}
			}
			// Check if [Stack] is defined in Text or Data
			if(files[i].symbolTable[j].location != 'U' && !strcmp(files[i].symbolTable[j].label, "Stack")){
				printf("Can not define [Stack]");
				exit(1);
			}

			if(files[i].symbolTable[j].location == 'T'){
				combiner.symTable[combiner.symTableSize].location = files[i].symbolTable[j].location;
				combiner.symTable[combiner.symTableSize].offset = currTextLine + files[i].symbolTable[j].offset;

				strcpy(combiner.symTable[combiner.symTableSize].label, files[i].symbolTable[j].label);
				++combiner.symTableSize;
			}
			else if(files[i].symbolTable[j].location == 'D'){
				combiner.symTable[combiner.symTableSize].location = files[i].symbolTable[j].location;
				combiner.symTable[combiner.symTableSize].offset = currDataLine + files[i].symbolTable[j].offset;

				strcpy(combiner.symTable[combiner.symTableSize].label, files[i].symbolTable[j].label);
				++combiner.symTableSize;
			}
			else if(files[i].symbolTable[j].location == 'U'){
				Utable[uind].location = files[i].symbolTable[j].location;
				strcpy(Utable[uind].label, files[i].symbolTable[j].label);
				++uind;
			}
			else{
				printf("Unrecognized Location");
				exit(1);
			}
		}
		currTextLine += files[i].textSize;
		currDataLine += files[i].dataSize;
	}

	// define [Stack] label
	strcpy(combiner.symTable[combiner.symTableSize].label, "Stack");
	combiner.symTable[combiner.symTableSize].location = 'D';
	combiner.symTable[combiner.symTableSize].offset = combiner.textSize + combiner.dataSize; // inconsistent location
	++combiner.symTableSize;

	// check for undefined labels
	for(int i = 0; i < uind; ++i){
		int def = 0;
		for(int j = 0; j < combiner.symTableSize; ++j){
			if(!strcmp(Utable[i].label, combiner.symTable[j].label)){
				def = 1;
			}
		}
		if(!def){
			printf("Undefined label");
			exit(1);
		}
	}
	//Move Text and Data into Combined Files
	int index1 = 0;
	int index2 = 0;
	for(int i = 0; i < numFiles; ++i){
		for(int j = 0; j < files[i].textSize; ++j){
			combiner.text[index1] = files[i].text[j];
			++index1;
		}
		for(int k = 0; k < files[i].dataSize; ++k){
			combiner.data[index2] = files[i].data[k];
			++index2;
		}
	}
	// Checking
	if(index1 != combiner.textSize || index2 != combiner.dataSize){
		printf("Mismatch Sizes");
		exit(1);
	}

	// Relocate
	for(int i = 0; i < numFiles; ++i){
		for(int j = 0; j < files[i].relocationTableSize; ++j){
			// check if stack
			unsigned short int offset = 0;
			if(!strcmp(files[i].relocTable[j].label, "Stack")){
				offset = combiner.textSize + combiner.dataSize;
				if(!strcmp(files[i].relocTable[j].inst, ".fill")){
					// datastartline goes to far
					combiner.data[files[i].dataStartingLine - combiner.textSize + files[i].relocTable[j].offset] += offset;
				}
				else{
					combiner.text[files[i].textStartingLine + files[i].relocTable[j].offset] += offset;
				}
				// move on
				continue;
			}
			// check if global 
			if(files[i].relocTable[j].label[0] <= 'Z' && files[i].relocTable[j].label[0] >= 'A'){
				for(int k = 0; k < combiner.symTableSize; ++k){
					if(!strcmp(combiner.symTable[k].label, files[i].relocTable[j].label)){
						if(combiner.symTable[k].location == 'T'){
							offset = combiner.symTable[k].offset;
						}
						else if(combiner.symTable[k].location == 'D'){
							offset = combiner.symTable[k].offset + combiner.textSize;
						}
					}
				}
			}
			else{ // if local variable
				//find the original offset value
				unsigned short int ogoffset = 0;
				if(!strcmp(files[i].relocTable[j].inst, ".fill")){
					ogoffset = files[i].data[files[i].relocTable[j].offset];
				}
				else{
					//take bits 15-0
					ogoffset = files[i].text[files[i].relocTable[j].offset] & 0xFFFF;
				}

				// set new offset value
				if(ogoffset < files[i].textSize){
					offset = files[i].textStartingLine + ogoffset;
				}
				else{
					offset = ogoffset - files[i].textSize + files[i].dataStartingLine;
				}
			}
			// actual relocating part
			unsigned short int replace = 0;
			if(!strcmp(files[i].relocTable[j].inst, ".fill")){
				// dataStartingLine goes too far
				combiner.data[files[i].relocTable[j].offset + files[i].dataStartingLine - combiner.textSize] = offset;
			}
			else{ // lw and sw instructions
				replace = combiner.text[files[i].relocTable[j].offset + files[i].textStartingLine] & 0xFFFF;
				combiner.text[files[i].relocTable[j].offset + files[i].textStartingLine] -= replace;
				combiner.text[files[i].relocTable[j].offset + files[i].textStartingLine] += offset;
			}
		}
	}

	//print to file
	for(int i = 0; i < combiner.textSize; ++i){
		fprintf(outFilePtr, "%d\n", combiner.text[i]);
	}
	for(int j = 0; j < combiner.dataSize; ++j){
		fprintf(outFilePtr, "%d\n", combiner.data[j]);
	}

	fclose(outFilePtr);
	return(0);
} // main
