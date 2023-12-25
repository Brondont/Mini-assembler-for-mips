#include <stdio.h>
#include <stdlib.h>
#include "parsefile.h"

int main(int argc, char **argv)
{
  if (argc <= 1)
  {
    printf("\n Wrong input format. \n ./main (mipsfilename).asm \n");
    return -1;
  }

  label labels[100];
  FILE *file = fopen(argv[1], "r");
  if (!file)
  {
    printf("\n failed to read file. \n");
    return -1;
  }

  FILE *outFile = fopen("./assembledCode.txt", "w");
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
