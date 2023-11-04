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
    {"zero", "00000"},
    {"at", "00001"},
    {"v0", "00010"},
    {"v1", "00011"},
    {"a0", "00100"},
    {"a1", "00101"},
    {"a2", "00110"},
    {"a3", "00111"},
    {"t0", "01000"},
    {"t1", "01001"},
    {"t2", "01010"},
    {"t3", "01011"},
    {"t4", "01100"},
    {"t5", "01101"},
    {"t6", "01110"},
    {"t7", "01111"},
    {"s0", "10000"},
    {"s1", "10001"},
    {"s2", "10010"},
    {"s3", "10011"},
    {"s4", "10100"},
    {"s5", "10101"},
    {"s6", "10110"},
    {"s7", "10111"},
    {"t8", "11000"},
    {"t9", "11001"},
    {NULL, 0}};

struct
{
  char *name;
  char *address;
} rInstruction[] = {
    {"add", "100000"}};

char *parseInstruction(char *line, char **instructionSet)
{
  char *instruction = NULL;
  *instructionSet = NULL;
  int instructionLength = 0;
  int instructionSetLength = 0;

  // storing the length of white space before instruction
  instructionLength = strspn(line, " \n\t\0");

  char *i = line + instructionLength; // pointer to the start of the instruction

  char *j = strpbrk(i, " #\n\t\0"); // pointer to the end of the instruction

  if (j != 0)
  {
    instructionSetLength = (line + strlen(line) - 1) - j;
    if (instructionSetLength != 0)
    {
      *instructionSet = (char *)malloc(instructionSetLength);
      if (*instructionSet)
        strncpy(*instructionSet, j, instructionSetLength);
      (*instructionSet)[instructionSetLength] = '\0';
    }
  }

  // Create instruction string
  instructionLength = j - i;
  if (instructionLength == 0)
    return NULL;

  instruction = (char *)malloc(instructionLength);
  if (!instruction)
    return NULL;

  // Copy the instruction into its variable
  strncpy(instruction, i, instructionLength);
  instruction[instructionLength] = '\0'; // Identify the end of the string
  return instruction;
}

void parseFile(FILE *file, int passTime, int *status)
{
  char line[MAX_LINE_LENGTH + 1];
  char *instruction = NULL;
  char *instructionSet = NULL;
  int isDataSection = 0;
  int isTextSection = 0;

  // reading line by line
  while (fgets(line, sizeof(line), file))
  {
    int lineLength = strlen(line);
    if (lineLength == 0)
      continue;
    // checking the line length
    if (strlen(line) == MAX_LINE_LENGTH)
    {
      printf("exceeded maximum line length.");
      return;
    }

    // Handles last line issue
    if (line[lineLength - 1] != '\n')
    {
      line[lineLength] = '\n';
      line[lineLength + 1] = '\0';
    }

    instruction = parseInstruction(line, &instructionSet);

    // printf("%s", line);
    // printf("%s%s\n", instruction, instructionSet);

    if (passTime == 0)
    {
      if (!instruction || *instruction == '#')
        continue;
      // check sections
      if (strcmp(instruction, ".text") == 0)
      {
        if (isTextSection)
        {
          printf("\n Can only have 1 .text section \n");
          *status = 0;
          return;
        }
        isTextSection = 1;
        continue;
      }
      if (strcmp(instruction, ".data") == 0)
      {
        if (isDataSection)
        {
          printf("\n Can only have 1 .data section \n");
          *status = 0;
          return;
        }
        isDataSection = 1;
        continue;
      }
      if (isDataSection)
      {
        if (!strpbrk(instruction, ":"))
        {
          printf("\n Only variables can be declared in .data section. \n");
          *status = 0;
          return;
        }
      }
      if (isTextSection)
      {
        char *isLabel = strpbrk(instruction, ":");
        if ()
        {
          printf("\n Can't have directives in the text section. \n");
          *status = 0;
          return;
        }
      }
    }
    free(instruction);
    free(instructionSet);
  }
  return;
}