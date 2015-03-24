#include <pthread.h> 
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>


typedef struct arguments 
{
  int value;
} arguments;


void *opA(void *arg)
{
  int value, i;
  
  arguments *argument;
  argument = (arguments*)arg;
  
  value = argument->value;
  
  for (i = 0; i < value; i++)
  {
    printf("Thread A executing opA\t==\n");
  }


  return NULL;
}

void *opB(void *arg)
{
  int value, i;

  arguments *argument;
  argument = (struct arguments *)arg;

  value = argument->value;
  for (i = 0; i < value; i++)
  {
    printf("Thread B executing opB\t\t==\n");
  }

  return NULL;
}

void main(int argc, char* argv[])
{
  if (argc < 2)
  {
    printf("USAGE: takeTurns2 [INT]\n");
    exit(EXIT_FAILURE);
  }
  // Get the argument
  int value = atoi(argv[1]);
  
  // Set up threads and their arguments.
  void *retvalA;
  void *retvalB;
  pthread_t threadA;
  pthread_t threadB;
  arguments *arg;
  arg = (struct arguments *)malloc(sizeof(struct arguments));
  arg->value = value;

  
  // Create the threads
  if (pthread_create(&threadA, NULL, opA, (void *) arg))
  {
    fprintf(stderr, "Error while creating thread\n");
    exit(EXIT_FAILURE);
  }
  if (pthread_create(&threadB, NULL, opB, (void *) arg))
  {
    fprintf(stderr, "Error while creating thread\n");
    exit(EXIT_FAILURE);
  }



  // Wait for both threads to finish
  if (pthread_join(threadA, &retvalA))
  {
    fprintf(stderr, "Error while waiting for thread\n");
    exit(EXIT_FAILURE);
  }
  if (pthread_join(threadB, &retvalB))
  {
    fprintf(stderr, "Error while waiting for threadd\n");
    exit(EXIT_FAILURE);
  } 
}
