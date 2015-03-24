#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

void op(int procNum, int repetitions)
{
  int i;
  for (i = 0; i < repetitions; i++)
  {
    printf("Thread %d executing op\n", procNum);
  }
}

int main(int argc, char* argv[])
{
  if (argc < 3)
  {
    printf("USAGE: takeTurns1 [num of executions per process] [num of processes]\n");
    exit(EXIT_FAILURE);
  }

  // Get the arguments
  int repetitions = atoi(argv[1]);    // Holds the number of executions per process
  int numofprocesses = atoi(argv[2]); // Holds the number of processes.
  int i;
  pid_t pid;
  
  // Fork the appropriate amount of processes.
  for (i = 0; i < numofprocesses; i++)
  {
    pid = fork();
    if (pid > 0) // Parent
    {
      // Do nothing.
      wait(0);
    }
    else // Child
    {
      op(i+1, repetitions);
      break;
    }
  }
  
  return 0;
}
