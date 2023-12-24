#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>

#include "parsefile.h"

#define MAX_LINE_LENGTH 256

struct
{
  char *name;
  char *address;
} registerMap[] = {
    {"$zero", "00000"},
    {"$at", "00001"},
    {"$v0", "00010"},
    {"$v1", "00011"},
    {"$a0", "00100"},
    {"$a1", "00101"},
    {"$a2", "00110"},
    {"$a3", "00111"},
    {"$t0", "01000"},
    {"$t1", "01001"},
    {"$t2", "01010"},
    {"$t3", "01011"},
    {"$t4", "01100"},
    {"$t5", "01101"},
    {"$t6", "01110"},
    {"$t7", "01111"},
    {"$s0", "10000"},
    {"$s1", "10001"},
    {"$s2", "10010"},
    {"$s3", "10011"},
    {"$s4", "10100"},
    {"$s5", "10101"},
    {"$s6", "10110"},
    {"$s7", "10111"},
    {"$t8", "11000"},
    {"$t9", "11001"},
    {NULL, 0}};

struct
{
  char *name;
  char *function;
} rInstructions[] = {
    {"add", "100000"},
    {"sub", "100001"},
    {"and", "100100"},
    {"or", "100101"},
    {"sll", "000000"},
    {"slt", "101010"},
    {"jr", "001000"},
    {NULL, 0}};

struct
{
  char *name;
  char *op;
} iInstructions[] = {
    {"lw", "100011"},
    {"sw", "101011"},
    {"andi", "001100"},
    {"ori", "001101"},
    {"beq", "000100"},
    {"slti", "001010"},
    {"addi", "001000"},
    {"lui", "001111"},
    {"la", 0},
    {NULL, 0}};

struct
{
  char *name;
  char *op;
} jInstructions[] = {
    {"j", "000010"},
    {"jal", "000011"},
    {NULL, 0}};

char *parseInstruction(char *line, char **instructionSet)
{
  char *instruction = NULL;
  *instructionSet = NULL;
  int instructionLength = 0;
  int instructionSetLength = 0;

  // storing the length of white space before instruction
  instructionLength = strspn(line, " ,\n\t\0");

  char *i = line + instructionLength; // pointer to the start of the instruction

  char *j = strpbrk(i, " \n#,()\t\0"); // pointer to the end of the instruction

  // point to the rest of the string
  if (j)
    *instructionSet = strpbrk(j, ".$,\"-0123456789");

  // Create instruction string
  instructionLength = j - i;
  if (instructionLength == 0)
    return NULL;

  instruction = (char *)malloc(instructionLength + 1);
  if (!instruction)
    return NULL;

  // Copy the instruction into its variable
  strncpy(instruction, i, instructionLength);
  instruction[instructionLength] = '\0'; // Identify the end of the string
  return instruction;
}

char instructionType(char *instruction)
{
  if (!instruction)
    return 0;

  for (int i = 0; rInstructions[i].name; i++)
    if (strcmp(rInstructions[i].name, instruction) == 0)
      return 'r';

  for (int i = 0; iInstructions[i].name; i++)
    if (strcmp(iInstructions[i].name, instruction) == 0)
      return 'i';

  for (int i = 0; jInstructions[i].name; i++)
    if (strcmp(jInstructions[i].name, instruction) == 0)
      return 'j';

  return 0;
}

char *registerAddress(char *regKey)
{
  if (!regKey)
  {
    return "00000";
  }
  for (int i = 0; registerMap[i].name; i++)
  {
    if (strcmp(regKey, registerMap[i].name) == 0)
    {
      return registerMap[i].address;
    }
  }
  return "00000";
}

char *instructionAddress(char *instruction)
{
  if (!instruction)
    return NULL;
  for (int i = 0; rInstructions[i].name; i++)
    if (strcmp(rInstructions[i].name, instruction) == 0)
      return rInstructions[i].function;
  for (int i = 0; iInstructions[i].name; i++)
    if (strcmp(iInstructions[i].name, instruction) == 0)
      return iInstructions[i].op;
  for (int i = 0; jInstructions[i].name; i++)
    if (strcmp(jInstructions[i].name, instruction) == 0)
      return jInstructions[i].op;
  return NULL;
}

int labelAddress(label labels[100], char *desiredLabel)
{
  if (!desiredLabel)
    return 0;
  for (int i = 0; labels[i].label[0] != '\0'; i++)
  {
    if (strcmp(labels[i].label, desiredLabel) == 0)
    {
      return labels[i].address;
    }
  }
  return 0;
}

void getBin(int num, char *string, int padding)
{
  *(string + padding) = '\0';

  long long pos;
  if (padding == 5)
    pos = 0x10;
  else if (padding == 16)
    pos = 0x8000;
  else if (padding == 26)
    pos = 0x2000000;
  else if (padding == 32)
    pos = 0x80000000;

  long long mask = pos;
  while (mask > 0)
  {
    *string++ = !!(mask & num) + '0';
    mask >>= 1;
  }
}

int getDec(char *bin)
{

  int b, k, m, n;
  int len, sum = 0;

  // Length - 1 to accomodate for null terminator
  len = strlen(bin) - 1;

  // Iterate the string
  for (k = 0; k <= len; k++)
  {

    // Convert char to numeric value
    n = (bin[k] - '0');

    // Check the character is binary
    if ((n > 1) || (n < 0))
    {
      return 0;
    }

    for (b = 1, m = len; m > k; m--)
      b *= 2;

    // sum it up
    sum = sum + n * b;
  }

  return sum;
}

void rFormat(char *instruction, char rs[5], char rt[5], char rd[5], int shamnt, FILE *outFile)
{
  char *opcode = "000000";
  char *rsBin = registerAddress(rs);
  char *rtBin = registerAddress(rt);
  char *rdBin = registerAddress(rd);
  char shamntBin[6];
  getBin(shamnt, shamntBin, 5);
  char *function = instructionAddress(instruction);
  fprintf(outFile, "%s %s %s %s %s %s\n", opcode, rsBin, rtBin, rdBin, shamntBin, function);
}

void iFormat(char *instruction, char rs[5], char rt[5], int immediate, FILE *outFile)
{
  char *rsBin = registerAddress(rs);
  char *rtBin = registerAddress(rt);
  char immediateBin[17];
  getBin(immediate, immediateBin, 16);
  char *codeOp = instructionAddress(instruction);
  fprintf(outFile, "%s %s %s %s\n", codeOp, rsBin, rtBin, immediateBin);
}

void parseFile(FILE *file, FILE *outFile, int passTime, label labels[100], int *status)
{
  char line[MAX_LINE_LENGTH + 1];
  char *instruction = NULL;
  char *instructionSet = NULL;
  int isDataSection = 0;
  int isTextSection = 0;
  int lineNumber = 0;
  int labelIndex = 0;
  int programCounter = 0x00400000;

  while (fgets(line, sizeof(line), file))
  {
    if (instruction)
      free(instruction);

    int lineLength = strlen(line);

    // Handles last line issue
    if (lineLength > 0 && line[lineLength - 1] != '\n')
    {
      line[lineLength] = '\n';
      line[lineLength + 1] = '\0';
    }

    lineNumber++;

    // check syntax lexical..
    if (passTime == 0)
    {
      // checking the line length
      if (lineLength == MAX_LINE_LENGTH + 1)
      {
        printf("\n exceeded maximum line length. \n at line: %d \n", lineNumber);
        *status = 0;
        return;
      }

      instruction = parseInstruction(line, &instructionSet);
      // printf("\n %s %x\n", instruction, programCounter);

      // check syntax
      if (!instruction || *instruction == '#')
        continue;

      // check sections
      if (strcmp(instruction, ".text") == 0)
      {
        if (instructionSet)
        {
          printf("\n incorrect Segment declaration \n at line: %d \n", lineNumber);
          *status = 0;
          return;
        }
        if (isTextSection)
        {
          printf("\n Can only have 1 .text section \n at line: %d \n", lineNumber);
          *status = 0;
          return;
        }
        isDataSection = 0;
        isTextSection = 1;
        programCounter = 0x00400000;
        continue;
      }
      else if (strcmp(instruction, ".data") == 0)
      {
        if (instructionSet)
        {
          printf("\n incorrect Segment declaration \n at line: %d \n", lineNumber);
          *status = 0;
          return;
        }
        if (isDataSection)
        {
          printf("\n Can only have 1 .data section \n at line: %d", lineNumber);
          *status = 0;
          return;
        }
        isTextSection = 0;
        isDataSection = 1;
        programCounter = 0x10010000;
        continue;
      }

      if (isDataSection)
      {
        if (!strpbrk(instruction, ":"))
        {
          printf("\n Only variables can be declared in .data section. \n at line: %d \n", lineNumber);
          *status = 0;
          return;
        }
        else
        {
          const int instructionSize = strlen(instruction);
          char label[instructionSize];
          strcpy(label, instruction);
          label[instructionSize - 1] = '\0';
          strcpy(labels[labelIndex].label, label);
          labels[labelIndex].address = programCounter;
          // extract directive from data label and size will be in instructionSet
          char *directive = parseInstruction(instructionSet, &instructionSet);
          if (strcmp(directive, ".word") == 0)
          {
            // increment program counter till we run out of .words (for arrays)
            while (instructionSet && parseInstruction(instructionSet, &instructionSet))
            {
              programCounter += 4;
            }
          }
          else if (strcmp(directive, ".asciiz") == 0)
          {
            int size = strlen(instructionSet) - 3; // -3 for new line character and " " that are stored in instructionSet
            programCounter += size - 1;
          }
          labelIndex++;
          // set end of array
          labels[labelIndex].label[0] = '\0';
        }
      }
      else if (isTextSection)
      {
        char *isLabel = strpbrk(instruction, ":");
        if (isLabel)
        {
          if (instructionSet)
          {
            printf("\n Can't have directives in the text section. \n at line: %d \n", lineNumber);
            *status = 0;
            return;
          }
          // add labels
          const int instructionSize = strlen(instruction);
          char label[instructionSize];
          strcpy(label, instruction);
          label[instructionSize - 1] = '\0';
          strcpy(labels[labelIndex].label, label);
          labels[labelIndex].address = programCounter;
          labelIndex++;
          // set end of array
          labels[labelIndex].label[0] = '\0';
        }
        if ((!instructionType(instruction) && strcmp(instruction, "la") != 0) && !isLabel)
        {
          *status = 0;
          printf("\n invalid instruction type: \"%s\" \n at line: %d \n", instruction, lineNumber);
          return;
        }

        if (strcmp(instruction, "la") == 0)
          programCounter += 8;
        else
          programCounter += 4;
      }
    }
    // start translating
    if (passTime == 1)
    {
      instruction = parseInstruction(line, &instructionSet);

      if (!instruction || *instruction == '#')
        continue;

      // Getting to .text section
      if (strcmp(instruction, ".text") == 0)
      {
        isTextSection = 1;
        continue;
      }
      if (!isTextSection)
        continue;

      if (strcmp(instruction, ".data") == 0)
        break;

      // Don't translate labels and increment programCounter accordingly
      if (strpbrk(instruction, ":"))
      {
        programCounter += 4;
        continue;
      }
      char instType = instructionType(instruction);

      // if R format
      if (instType == 'r')
      {
        // if r format of shape rd, rs, rt registers
        if (strcmp(instruction, "add") == 0 || strcmp(instruction, "sub") == 0 || strcmp(instruction, "and") == 0 || strcmp(instruction, "or") == 0 || strcmp(instruction, "slt") == 0)
        {
          char *key = NULL;

          char keys[3][4];

          for (int i = 0; i < 3; i++)
          {
            key = parseInstruction(instructionSet, &instructionSet);
            strcpy(keys[i], key);
            free(key);
          }
          rFormat(instruction, keys[1], keys[2], keys[0], 0, outFile);
        }
        else
        {
          // instruction of type shift amount
          char *key = NULL;

          char keys[3][4];

          for (int i = 0; i < 3; i++)
          {
            key = parseInstruction(instructionSet, &instructionSet);
            strcpy(keys[i], key);
            free(key);
          }
          rFormat(instruction, keys[1], keys[0], NULL, atoi(keys[2]), outFile);
        }
      }
      else if (instType == 'i')
      {
        // type rt rs immediate
        if (strcmp(instruction, "andi") == 0 || strcmp(instruction, "ori") == 0 || strcmp(instruction, "slti") == 0 || strcmp(instruction, "addi") == 0)
        {
          char *key = NULL;

          char keys[3][4];

          for (int i = 0; i < 3; i++)
          {
            key = parseInstruction(instructionSet, &instructionSet);
            strcpy(keys[i], key);
            free(key);
          }
          iFormat(instruction, keys[1], keys[0], atoi(keys[2]), outFile);
        }
        // type rt immediate rs
        else if (strcmp(instruction, "lw") == 0 || strcmp(instruction, "sw") == 0)
        {
          char *key = NULL;

          char keys[3][4];

          for (int i = 0; i < 3; i++)
          {
            key = parseInstruction(instructionSet, &instructionSet);
            strcpy(keys[i], key);
            free(key);
          }
          iFormat(instruction, keys[2], keys[0], atoi(keys[1]), outFile);
        }
        // type rs rt label
        else if (strcmp(instruction, "beq") == 0)
        {
          char *key = NULL;

          char keys[3][MAX_LINE_LENGTH];

          for (int i = 0; i < 3; i++)
          {
            key = parseInstruction(instructionSet, &instructionSet);
            strcpy(keys[i], key);
            free(key);
          }
          iFormat(instruction, keys[0], keys[1], (labelAddress(labels, keys[2]) - programCounter - 8) / 4, outFile);
        }
        // pseudo code instruction
        else if (strcmp(instruction, "la") == 0)
        {
          char *key = NULL;

          char keys[2][MAX_LINE_LENGTH];

          for (int i = 0; i < 2; i++)
          {
            key = parseInstruction(instructionSet, &instructionSet);
            strcpy(keys[i], key);
            free(key);
          }
          int variableAddress = labelAddress(labels, keys[1]);
          char variableAddressBinary[33];
          getBin(variableAddress, variableAddressBinary, 32);

          char upperBits[17];
          char lowerBits[17];

          strncpy(lowerBits, variableAddressBinary, 16);
          strncpy(upperBits, variableAddressBinary + 16, 16);

          int immediate = getDec(lowerBits);
          iFormat("lui", "00000", "$at", immediate, outFile);

          immediate = getDec(upperBits);
          iFormat("ori", "$at", keys[0], immediate, outFile);
        }
      }
      else if (instType == 'j')
      {
        // continue for j type
      }

      if (strcmp(instruction, "la") == 0)
        programCounter += 8;
      else
        programCounter += 4;
    }
  }
  return;
}