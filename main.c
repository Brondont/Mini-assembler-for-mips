#include <stdio.h>
#include <stdlib.h>
#include "parsefile.h"

int main(int argc, char **argv)
{
  FILE *file = fopen("./mips1.asm", "r");
  if (!file)
  {
    printf("failed to read file.");
    return -1;
  }
  int passTime = 0;

  parseFile(file, passTime);

  passTime = 1;

  // rewind(file);
  // parseFile(file, passTime);
  return 0;
}