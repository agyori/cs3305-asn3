#include <pthread.h> 
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>

/* Global Variables */

// Create the semaphores
sem_t semA;
sem_t semB;

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
    sem_wait(&semA);
    printf("Thread A executing opA\n");
    sem_post(&semB);
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
    sem_wait(&semB);
    printf("Thread B executing opB\n");
    sem_post(&semA);
  }

  return NULL;
}

void main(int argc, char* argv[])
{
  if (argc < 2)
  {
    printf("USAGE: takeTurns1 [INT]\n");
    exit(EXIT_FAILURE);
  }
  // Get the argument
  int value = atoi(argv[1]);
  // Initialize semaphores
  sem_init(&semA, 0, 1);
  sem_init(&semB, 0, 0);
  
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
