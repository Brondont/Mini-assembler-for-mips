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

char *parseInstruction(char *line)
{
  char *instruction = NULL;
  int length = 0;

  // skips all the white space and updates line length
  length = strspn(line, " \n\t");

  char *i = line + length; // pointer to the start of the instruction

  char *j = strpbrk(i, " \n\t"); // pointer to the end of the instruction

  // TODO: handle comments and instruction inputs # this is a comment in assembly
  //  might need a seperate function for them
  //

  // Create instruction string
  length = j - i;
  instruction = (char *)malloc(length);
  if (!instruction)
    return NULL;

  // Copy the instruction into its variable
  strncpy(instruction, i, length);
  instruction[length] = '\0'; // Identify the end of the string
  return instruction;
}

void parseFile(FILE *file, int passTime)
{
  char line[MAX_LINE_LENGTH + 1];
  char *instruction = NULL;
  int isDataSection = 0;
  int isTextSection = 0;

  // reading line by line
  while (fgets(line, sizeof(line), file))
  {
    // checking the line length
    if (strlen(line) == MAX_LINE_LENGTH)
    {
      printf("exceeded maximum line length.");
      return;
    }
    instruction = parseInstruction(line);

    if (passTime == 0)
    {
      if (!instruction || *instruction == '#')
        continue;
      if (strcmp(instruction, ".text") == 0)
        isTextSection = 1;
      if (strcmp(instruction, ".data") == 0)
        isDataSection = 1;
    }
    printf("%d %d", isDataSection, isTextSection);
  }
  return;
}