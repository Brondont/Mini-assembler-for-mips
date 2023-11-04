#include <stdio.h>
#include <stdlib.h>
#include "parsefile.h"

int main(int argc, char **argv)
{
  FILE *file = fopen("./mips1.asm", "r");
  if (!file)
  {
    printf("\n failed to read file. \n");
    return -1;
  }
  int passTime = 0;
  int status = 1;
  parseFile(file, passTime, &status);
  if (!status)
  {
    printf("\n Failed to assemble file. \n");
    return -1;
  }
  passTime = 1;

  // rewind(file);
  // parseFile(file, passTime);
  return 0;
}
