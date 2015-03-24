#include <stdio.h>
#include <stdlib.h>


int main()
{
  int *buffer;
  int size = 4;
  buffer = (int *) calloc (size, sizeof(int));

  buffer[0] = 4;
  buffer[1] = 5;
  buffer[2] = 6;
  buffer[3] = 7;

  int *ptr;
  ptr = buffer;

  printf ("%d\n", ptr - buffer);
  printf("ptr: %d\n", *ptr);
  
  ptr = ptr + 3;

  printf("%d\n", ptr - buffer);
  printf("ptr: %d\n", *ptr);

  return 0;
}
