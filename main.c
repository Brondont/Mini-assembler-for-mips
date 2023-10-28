#include <stdio.h>
#include <stdlib.h>

#include "./parseFile.c";

int main(int argc, char **argv)
{
  FILE *file = fopen("./mips1.asm", "r");
  if (!file)
  {
    printf("failed to read file.");
    return -1;
  }
  int passTime = 0;

  parse_file(file, passTime);

  passTime = 1;

  rewind(file);
  parse_file(file, passTime);
  return 0;
}