/**
 * Project 2
 * LC-2K Linker
 */

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAXSIZE 300
#define MAXLINELENGTH 1000
#define MAXFILES 6

typedef struct FileData FileData;
typedef struct SymbolTableEntry SymbolTableEntry;
typedef struct RelocationTableEntry RelocationTableEntry;
typedef struct CombinedFiles CombinedFiles;

struct SymbolTableEntry {
   int offset;
   char label[7];
   char location;
};

struct RelocationTableEntry {
   int offset;
   int file;
   char inst[7];
   char label[7];
};

struct FileData {
   int num_text;
   int num_data;
   int symbolTableSize;
   int relocationTableSize;
   int textStartingLine;
   int dataStartingLine;
   int text[MAXSIZE];
   int data[MAXSIZE];
   SymbolTableEntry symbolTable[MAXSIZE];
   RelocationTableEntry relocTable[MAXSIZE];
};

struct CombinedFiles {
   int text[MAXSIZE];
   int data[MAXSIZE];
   SymbolTableEntry symbolTable[MAXSIZE];
   RelocationTableEntry relocationTable[MAXSIZE];
   int num_text;
   int num_data;
   int symbolTableSize;
   int relocationTableSize;
};

bool check_global(char *c);
int find_label(FileData files[MAXFILES], char *label_in, int file_size);
int change(int initial, int in);

int main(int argc, char *argv[]) {
   char *inFileString, *outFileString;
   FILE *inFilePtr, *outFilePtr;
   int i = 0, j = 0, k = 0, fileNum = argc - 2, total_size = 0;

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
   for (i = 0; i < fileNum; i++) {
      inFileString = argv[i + 1];

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

      files[i].num_text = sizeText;
      files[i].num_data = sizeData;
      total_size += sizeText;
      total_size += sizeData;
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
         files[i].relocTable[j].file = i;
      }
      fclose(inFilePtr);
   }  // end reading files

   // *** INSERT YOUR CODE BELOW ***
   //    Begin the linking process
   //    Happy coding!!!

   files[0].textStartingLine = 0;
   for (i = 1; i < fileNum; ++i) {
      files[i].textStartingLine = files[i - 1].textStartingLine + files[i - 1].num_text;
   }

   files[0].dataStartingLine = files[fileNum - 1].textStartingLine + files[fileNum - 1].num_text;
   for (i = 1; i < fileNum; ++i) {
      files[i].dataStartingLine = files[i - 1].dataStartingLine + files[i - 1].num_data;
   }

   int iter = 0;

   for (i = 0; i < fileNum;) {
      for (j = i + 1; j < fileNum; ++j) {
         for (k = 0; k < files[j].symbolTableSize; ++k) {
            if ((!strcmp(files[i].symbolTable[iter].label, files[j].symbolTable[k].label))) {
               if (files[i].symbolTable[iter].location != 'U') {
                  if (files[j].symbolTable[k].location != 'U') {
                     printf("1: Duplicate Defined Global Label: %s and %s\n", files[i].symbolTable[iter].label, files[j].symbolTable[k].label);
                     exit(1);
                  }
               }
            }
         }
      }

      ++iter;

      if (files[i].symbolTableSize <= iter) {
         iter = 0;
         ++i;
      }
   }

   for (i = 0; i < fileNum; ++i) {
      for (j = 0; j < files[i].symbolTableSize; ++j) {
         if (!strcmp("Stack", files[i].symbolTable[j].label)) {
            if (files[i].symbolTable[j].location != 'U') {
               printf("Stack label defined by an object file\n");
               exit(1);
            }
         }
      }
   }

   printf("this is filenum %d\n", files[0].relocationTableSize);
//    printf("this is symtablesize %d\n", files.symbolTableSize);
//    printf("this is relocable size %d\n", files.relocTableSize);
   
   for (int i= 0; i < fileNum; i++){
      printf("this is relocation table %d is size: %d\n", i, files[0].relocationTableSize);
      printf("this is symtablesize %d\n", files[0].symbolTableSize);
      printf("this is relocable size %d\n", files[0].relocationTableSize);
   }
   printf("end\n");

   for (i = 0; i < fileNum; ++i) {
      for (j = 0; j < files[i].relocationTableSize; ++j) {
         int reloc_offset = files[i].relocTable[j].offset;
         printf("this is reloc offset %d\n", reloc_offset);

         if ((!strcmp("lw", files[i].relocTable[j].inst)) || (!strcmp("sw", files[i].relocTable[j].inst))) {
            int text_offset = files[i].text[reloc_offset], size = 0, temp = 0;

            // if local label
            if (!check_global(files[i].relocTable[j].label)) {
               char hex[370];
               if (text_offset % 0b10000 < 0b1010) {
                  hex[0] = text_offset % 0b10000 + 0b110000;

               } else {
                  hex[0] = text_offset % 0b10000 + 0b110111;
               }
               temp = hex[0] - '0';

               for (k = 0; k < fileNum; ++k) {
                  if (k != i) {
                     if (i != 0) {
                        if (k <= i) {
                           size += files[k].num_text;
                           size += files[k].num_data;
                        } else {
                           size += files[k].num_text;
                        }
                     } else {
                        size += files[k].num_text;
                     }
                  }
               }
               printf("THIS IS TEXT OFFSET: %d, temp: %d, size: %d\n", text_offset, temp, size);
               printf("This is change: %d\n", change(text_offset, temp + size));
               files[i].text[reloc_offset] = change(text_offset, temp + size);
            }
            // if global label
            else {
               int temp_offset = 0;

               if (strcmp("Stack", files[i].relocTable[j].label)) {
                  temp_offset = find_label(files, files[i].relocTable[j].label, fileNum);
                  files[i].text[reloc_offset] = change(text_offset, temp_offset);

               } else {
                  temp_offset = find_label(files, "Stack", fileNum);
                  files[i].text[reloc_offset] = text_offset + total_size + temp;
               }
            }
         }
         // If .fill
         else if ((!strcmp(files[i].relocTable[j].inst, ".fill"))) {
            // if local
            if (!check_global(files[i].relocTable[j].label)) {
               int original_loc = files[i].data[reloc_offset], temp = 0;

               for (k = i - 1; k > -1; --k) {
                  if (original_loc < files[i].num_text) {
                     temp += files[k].num_text;
                  } else {
                     temp += files[k].num_data + files[k].num_text;
                  }
               }
               temp += original_loc;
               files[i].data[reloc_offset] = temp;
            }
            // if Global
            else {
               if (strcmp("Stack", files[i].relocTable[j].label)) {
                  files[i].data[reloc_offset] = find_label(files, files[i].relocTable[j].label, fileNum);
               } else {
                  files[i].data[reloc_offset] = total_size + find_label(files, files[i].relocTable[j].label, fileNum);
               }
            }
         }
      }
   }

   for (i = 0; i < fileNum; ++i) {
      for (j = 0; j < files[i].num_text; ++j) {
         fprintf(outFilePtr, "%d\n", files[i].text[j]);
      }
   }
   for (i = 0; i < fileNum; ++i) {
      for (j = 0; j < files[i].num_data; ++j) {
         fprintf(outFilePtr, "%d\n", files[i].data[j]);
      }
   }

   return 0;
}  // END OF MAIN

bool check_global(char *c) {  // DONE
   if (c[0] < 'A' || c[0] > 'Z') {
      return false;
   } else {
      return true;
   }
}

int find_label(FileData files[MAXFILES], char *label_in, int file_size) {  // DONE
   int result = -370;
   int i, j;

   for (i = 0; i < file_size; ++i) {
      for (j = 0; j < files[i].symbolTableSize; ++j) {
         // if label matches
         if ((!strcmp(label_in, files[i].symbolTable[j].label))) {
            if (files[i].symbolTable[j].location == 'D') {
               result = files[i].symbolTable[j].offset + files[i].dataStartingLine;
            }
            if (files[i].symbolTable[j].location == 'T') {
               result = files[i].symbolTable[j].offset + files[i].textStartingLine;
            } else {
               if (!strcmp(label_in, "Stack")) {
                  result = 0;
               }
            }
         }
      }
   }

   // if undefined
   if (result == -370) {
      if (!strcmp(label_in, "Stack")) {
         result = 0;
      } else if (strcmp(label_in, "Stack")) {
         printf("2: Undefined Global Label: %s\n", label_in);
         exit(1);
      }
   }

   return result;
}

int change(int initial, int in) {
   int in_b[370];
   int initial_b[370];

   int i = 0, j = 0, k = 0, a = 0, result = 0;

   for (i = 0; initial >= 1; ++i) {
      initial_b[i] = initial % 2;
      initial = initial / 2;
   }

   for (j = 0; in >= 1; ++j) {
      in_b[j] = in % 2;
      in = in / 2;
   }

   for (k = 0; k < j; ++k) {
      initial_b[k] = in_b[k];
   }

   for (a = i - 1; a > -1; --a) {
      result = result << 1;
      result = result | initial_b[a];
   }
   return result;
}