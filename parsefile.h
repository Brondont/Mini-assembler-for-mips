#pragma once

#include <stdio.h>

typedef struct label
{
  char label[300];
  int address;
} label;

void parseFile(FILE *file, FILE *outFile, int passTime, label labels[100], int *status);