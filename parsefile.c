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
    {"$ra", "11111"},
    {NULL, 0}};

struct
{
  char *name;
  char *function;
} rInstructions[] = {
    {"add", "100000"},
    {"sub", "100010"},
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
  {
    char *rest = strpbrk(j, "#.$,\"-0123456789 ");
    if (rest)
    {
      // remvoe leading white space
      rest += strspn(rest, " ");
      // remove ending white space
      int i = strlen(rest) - 1;
      while (i > 0 && (isspace(rest[i]) || rest[i] == '\n'))
        i--;
      rest[i + 1] = '\n';
      rest[i + 2] = '\0';
      *instructionSet = rest;
    }
  }

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

char *binaryToHex(char *binString)
{
  if (!binString)
    return NULL;

  int binStringLength = strlen(binString);

  if (binStringLength % 4 != 0)
    return NULL;

  char *hexString = (char *)malloc(binStringLength / 4 + 1);
  hexString[binStringLength / 4] = '\0';

  for (int i = 0, j = 0; i < binStringLength; i += 4, j++)
  {
    char hexDigit = 0;
    for (int k = 0; k < 4; k++)
    {
      hexDigit = (hexDigit << 1) | (binString[i + k] - '0');
    }
    hexString[j] = (hexDigit < 10) ? (hexDigit + '0') : (hexDigit - 10 + 'A');
  }
  return hexString;
}

void rFormat(char *instruction, char rs[5], char rt[5], char rd[5], int shamnt, FILE *outFile)
{
  char *opCode = "000000";
  char *rsBin = registerAddress(rs);
  char *rtBin = registerAddress(rt);
  char *rdBin = registerAddress(rd);
  char shamntBin[6];
  getBin(shamnt, shamntBin, 5);
  char *function = instructionAddress(instruction);
  char *machineCode = (char *)malloc(33);
  strcpy(machineCode, opCode);
  strcat(machineCode, rsBin);
  strcat(machineCode, rtBin);
  strcat(machineCode, rdBin);
  strcat(machineCode, shamntBin);
  strcat(machineCode, function);
  char *hexMachineCode = binaryToHex(machineCode);
  free(machineCode);
  fprintf(outFile, "\t\t0x%s\n", hexMachineCode);
  free(hexMachineCode);
}

void iFormat(char *instruction, char rs[5], char rt[5], int immediate, FILE *outFile)
{
  char *rsBin = registerAddress(rs);
  char *rtBin = registerAddress(rt);
  char immediateBin[17];
  getBin(immediate, immediateBin, 16);
  char *codeOp = instructionAddress(instruction);
  char *machineCode = (char *)malloc(33);
  strcpy(machineCode, codeOp);
  strcat(machineCode, rsBin);
  strcat(machineCode, rtBin);
  strcat(machineCode, immediateBin);
  char *hexMachineCode = binaryToHex(machineCode);
  free(machineCode);
  fprintf(outFile, "\t\t0x%s\n", hexMachineCode);
  free(hexMachineCode);
}

void jFormat(char *instruction, int immediate, FILE *outFile)
{
  char *codeOp = instructionAddress(instruction);
  char immediateBin[27];
  getBin(immediate, immediateBin, 26);
  char *machineCode = (char *)malloc(33);
  strcpy(machineCode, codeOp);
  strcat(machineCode, immediateBin);
  char *hexMachineCode = binaryToHex(machineCode);
  free(machineCode);
  fprintf(outFile, "\t\t0x%s\n", hexMachineCode);
  free(hexMachineCode);
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

      if (!instruction || *instruction == '#')
        continue;

      // check sections
      if (strcmp(instruction, ".text") == 0)
      {
        if (instructionSet)
        {
          for (int i = 0; instructionSet[i] != '\0'; i++)
          {
            if (instructionSet[i] == '#')
              break;
            if (isalpha(instructionSet[i]))
            {
              printf("\n incorrect Segment declaration \n at line: %d \n", lineNumber);
              *status = 0;
              return;
            }
          }
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
          for (int i = 0; instructionSet[i] != '\0'; i++)
          {
            if (instructionSet[i] == '#')
              break;
            if (isalpha(instructionSet[i]))
            {
              printf("\n incorrect Segment declaration \n at line: %d \n", lineNumber);
              *status = 0;
              return;
            }
          }
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
        char *isLabel = strpbrk(instruction, ":");
        if (!isLabel)
        {
          printf("\n Only variables can be declared in .data section. \n at line: %d \n", lineNumber);
          *status = 0;
          return;
        }
        else
        {
          // extract directive from data label and size will be in instructionSet
          char *directive = parseInstruction(instructionSet, &instructionSet);
          if (strcmp(directive, ".word") == 0)
          {
            // push program counter to point to the next 4 free bytes of memory
            while (programCounter % 4 != 0)
              programCounter++;
            strncpy(labels[labelIndex].label, instruction, isLabel - instruction);
            labels[labelIndex].address = programCounter;
            // increment program counter till we run out of .words (for arrays)
            while (instructionSet && parseInstruction(instructionSet, &instructionSet))
            {
              programCounter += 4;
            }
          }
          else if (strcmp(directive, ".asciiz") == 0)
          {
            strncpy(labels[labelIndex].label, instruction, isLabel - instruction);
            labels[labelIndex].address = programCounter;
            int size = strlen(instructionSet) - 3 + 1; // -3 for new line character and " " that are stored in instructionSet turns into -2 after adding in NULL character
            programCounter += size;
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
            for (int i = 0; instructionSet[i] != '\0'; i++)
            {
              if (instructionSet[i] == '#')
                break;
              if (isalpha(instructionSet[i]))
              {
                printf("\n Can't have directives for labels in the text section. \n at line: %d \n", lineNumber);
                *status = 0;
                return;
              }
            }
          }
          // is label add it to the map
          strncpy(labels[labelIndex].label, instruction, isLabel - instruction);
          labels[labelIndex].address = programCounter;
          labelIndex++;
          // set end of array
          labels[labelIndex].label[0] = '\0';
        }
        else
        {
          if ((!instructionType(instruction) != 0 && strcmp(instruction, "syscall") != 0))
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

      if (strpbrk(instruction, ":"))
        continue;

      char instType = instructionType(instruction);
      fprintf(outFile, "0x%x: ", programCounter);
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
        // instruction of type shift amount
        else if (strcmp(instruction, "sll") == 0 || strcmp(instruction, "srl") == 0)
        {

          char *key = NULL;

          char keys[3][4];

          for (int i = 0; i < 3; i++)
          {
            key = parseInstruction(instructionSet, &instructionSet);
            strcpy(keys[i], key);
            free(key);
          }
          rFormat(instruction, NULL, keys[1], keys[0], atoi(keys[2]), outFile);
        }
        else if (strcmp(instruction, "jr") == 0)
        {
          char *key = parseInstruction(instructionSet, &instructionSet);
          rFormat(instruction, key, "00000", "00000", 0, outFile);
          free(key);
        }
      }
      // if I format
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
          int32_t immediate = atoi(keys[2]);
          if (strcmp(instruction, "slti") == 0)
          {
            iFormat(instruction, keys[1], keys[0], immediate, outFile);
          }
          else
          {

            if (immediate < 0)
            {
              // andi and ori have pseudo instruction for negative numbers
              if (strcmp(instruction, "ori") == 0 || strcmp(instruction, "andi") == 0)
              {
                char upperBits[17];
                char lowerBits[17];
                char binaryImmediate[33];
                getBin(immediate, binaryImmediate, 32);

                strncpy(upperBits, binaryImmediate, 16);
                strncpy(lowerBits, binaryImmediate + 16, 16);
                upperBits[16] = '\0';
                lowerBits[16] = '\0';

                // lui gets upperbits
                int immediate = getDec(upperBits);
                iFormat("lui", "$at", "$at", immediate, outFile);

                // printing address to the file for visuals
                programCounter += 4;
                fprintf(outFile, "0x%x: ", programCounter);

                // ori gets the lower bits
                immediate = getDec(lowerBits);
                iFormat("ori", "$at", "$at", immediate, outFile);

                programCounter += 4;
                fprintf(outFile, "0x%x: ", programCounter);

                // remove the i at the end of instruction
                instruction[strlen(instruction) - 1] = '\0';
                rFormat(instruction, keys[1], "$at", keys[0], 0, outFile);
              }
              else
              {
                iFormat(instruction, keys[1], keys[0], immediate, outFile);
              }
            }
            // immediate is within the 16 bit range
            else if (immediate <= 65535)
            {
              iFormat(instruction, keys[1], keys[0], immediate, outFile);
            }
            // immediate overflows the 16 bit range
            else
            {
              char upperBits[17];
              char lowerBits[17];
              char binaryImmediate[33];
              getBin(immediate, binaryImmediate, 32);
              // seperate upper bits and lower bits
              strncpy(upperBits, binaryImmediate, 16);
              strncpy(lowerBits, binaryImmediate + 16, 16);
              upperBits[16] = '\0';
              lowerBits[16] = '\0';

              // lui gets upperbits
              int immediate = getDec(upperBits);
              iFormat("lui", "$at", "$at", immediate, outFile);

              // printing address to the file for visuals
              programCounter += 4;
              fprintf(outFile, "0x%x: ", programCounter);

              // ori gets the lower bits
              immediate = getDec(lowerBits);
              iFormat("ori", "$at", "$at", immediate, outFile);

              programCounter += 4;
              fprintf(outFile, "0x%x: ", programCounter);

              // remove the i at the end of instruction
              instruction[strlen(instruction) - 1] = '\0';
              rFormat(instruction, keys[1], "$at", keys[0], 0, outFile);
            }
          }
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
          iFormat(instruction, keys[0], keys[1], (labelAddress(labels, keys[2]) - programCounter - 4) / 4, outFile);
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

          // I have to do this or otherwise it wont work and i dont understand why
          char upperBits[17];
          char lowerBits[17];

          strncpy(upperBits, variableAddressBinary, 16);
          strncpy(lowerBits, variableAddressBinary + 16, 16);
          upperBits[16] = '\0';
          lowerBits[16] = '\0';

          // lui gets the upper bits
          int immediate = getDec(upperBits);
          iFormat("lui", "00000", "$at", immediate, outFile);

          // printing address to the file for visuals
          fprintf(outFile, "0x%x: ", programCounter + 4);

          // ori gets the lower bits
          immediate = getDec(lowerBits);
          iFormat("ori", "$at", keys[0], immediate, outFile);
        }
      }
      // if J format
      else if (instType == 'j')
      {
        char *key = parseInstruction(instructionSet, &instructionSet);
        int addressLabel = labelAddress(labels, key);
        addressLabel >>= 2;
        jFormat(instruction, addressLabel, outFile);
        free(key);
      }

      if (strcmp(instruction, "syscall") == 0)
      {
        fprintf(outFile, "\t\t0x0000000C\n");
      }
      if (strcmp(instruction, "la") == 0)
        programCounter += 8;
      else
        programCounter += 4;
    }
  }
  return;
}