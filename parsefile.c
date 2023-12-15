#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

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
    {"srl", "000010"},
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
    {"lui", "001111"},
    {"beq", "000100"},
    {"slti", "001010"},
    {"addi", "001000"},
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

  char *j = strpbrk(i, " \n#,\t\0"); // pointer to the end of the instruction

  // point to the rest of the string
  if (j)
    *instructionSet = strpbrk(j, ".$,");

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

void getBin(int num, char *string, int padding)
{

  *(string + padding) = '\0';

  long pos;
  if (padding == 5)
    pos = 0x10;
  else if (padding == 16)
    pos = 0x8000;
  else if (padding == 26)
    pos = 0x2000000;
  else if (padding == 32)
    pos = 0x80000000;

  long mask = pos << 1;
  while (mask >>= 1)
    *string++ = !!(mask & num) + '0';
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

void parseFile(FILE *file, FILE *outFile, int passTime, int *status)
{
  char line[MAX_LINE_LENGTH + 1];
  char *instruction = NULL;
  char *instructionSet = NULL;
  int isDataSection = 0;
  int isTextSection = 0;
  int programCounter = 0;

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
    programCounter++;
    // checking the line length
    if (passTime == 0)
    {
      if (lineLength == MAX_LINE_LENGTH + 1)
      {
        printf("\n exceeded maximum line length. \n at line: %d \n", programCounter);
        *status = 0;
        return;
      }

      instruction = parseInstruction(line, &instructionSet);
      // printf("\n %s %s \n", instruction, instructionSet);

      // check syntax
      if (!instruction || *instruction == '#')
        continue;
      // check sections
      if (strcmp(instruction, ".text") == 0)
      {
        if (instructionSet)
        {
          printf("\n incorrect Segment declaration \n at line: %d \n", programCounter);
          *status = 0;
          return;
        }
        if (isTextSection)
        {
          printf("\n Can only have 1 .text section \n at line: %d \n", programCounter);
          *status = 0;
          return;
        }
        isDataSection = 0;
        isTextSection = 1;
        continue;
      }
      if (strcmp(instruction, ".data") == 0)
      {
        if (instructionSet)
        {
          printf("\n incorrect Segment declaration \n at line: %d \n", programCounter);
          *status = 0;
          return;
        }
        if (isDataSection)
        {
          printf("\n Can only have 1 .data section \n at line: %d", programCounter);
          *status = 0;
          return;
        }
        isTextSection = 0;
        isDataSection = 1;
        continue;
      }

      if (isDataSection)
      {
        if (!strpbrk(instruction, ":"))
        {
          printf("\n Only variables can be declared in .data section. \n at line: %d \n", programCounter);
          *status = 0;
          return;
        }
      }

      if (isTextSection)
      {
        char *isLabel = strpbrk(instruction, ":");
        if (isLabel && instructionSet)
        {
          printf("\n Can't have directives in the text section. \n at line: %d \n", programCounter);
          *status = 0;
          return;
        }
        if (!instructionType(instruction) && !isLabel)
        {
          *status = 0;
          printf("\n invalid instruction type: \"%s\" \n at line: %d \n", instruction, programCounter);
          return;
        }
      }
    }
    // start translating
    if (passTime == 1)
    {
      instruction = parseInstruction(line, &instructionSet);

      if (!instruction || *instruction == '#')
        continue;

      char *isLabelOrData = strpbrk(instruction, ":");
      if (isLabelOrData || strcmp(instruction, ".text") == 0 || strcmp(instruction, ".data") == 0)
        continue;
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
      }
    }
  }
  return;
}