#include <stdio.h>
#include <stdlib.h>
#include "parsefile.h"

int main(int argc, char **argv)
{
  label labels[100];
  FILE *file = fopen("./mips1.asm", "r");
  if (!file)
  {
    printf("\n failed to read file. \n");
    return -1;
  }

  FILE *outFile = fopen("./mips1.txt", "w");
  if (!outFile)
  {
    printf("\n failed to compile try again. \n");
    return -2;
  }

  int passTime = 0;
  int status = 1;
  parseFile(file, outFile, passTime, labels, &status);

  if (!status)
  {
    printf("\n Failed to assemble file. \n");
    return -3;
  }

  passTime = 1;
  rewind(file);
  parseFile(file, outFile, passTime, labels, &status);

  fclose(file);
  fclose(outFile);
  return 0;
}
