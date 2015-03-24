#include <pthread.h> 
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define CONSUME_FROM_P1 0
#define CONSUME_FROM_P2 1

/**
 * Type definitions
 */

typedef struct consumer
{
  int consumed;     // The total amount of units consumed.
  int *ptr;         // Points to the next place to consume.
} consumer;

typedef struct producer // We will use 
{
  int produced;     // The total amount of units produced.
  int buffersize;   // Max size of the buffer.
  int *buffer;      // Beginning of the buffer.
  int *ptr;         // Pointer to the next place to produce in the buffer.
} producer;


typedef struct arguments 
{
  int value;
} arguments;


/**
 * Global variables
 */

// Producers
producer producer1;
producer producer2;

// Consumers
consumer consumer1;

// Semaphores
sem_t mutex1;
sem_t mutex2;
sem_t empty1;
sem_t empty2;
sem_t full1;
sem_t full2;


void insertitem(struct producer *p)
{
  // Add an item to where the producers pointer is pointing to.
  *p->ptr = 123;
  
  // Increment the pointer.
  // Recall that this is a circular array.
  if (p->ptr - p->buffer == p->buffersize-1)
    p->ptr = p->buffer;
  else
    p->ptr++;
}

// Remove an item from one of the buffers.
int removeitem(struct producer *p)
{
  int item;
  // Find the item to the left of the producer's pointer and decrement the pointer.
  // Recall that this is a circular array.
  if (p->ptr - p->buffer == 0)
    p->ptr = p->buffer + p->buffersize-1;
  else
    p->ptr--;
  
  // Delete the item
  item = *p->ptr;
  *p->ptr = 0;

  return item;
}



// Produce from producer1
void *produce1(void *arg)
{
  int toproduce, i;
  
  arguments *argument;
  argument = (arguments*)arg;
  
  toproduce = argument->value;
  


  int item;
  for (producer1.produced = 0; producer1.produced < toproduce; producer1.produced++)
  { 
    sem_wait(&empty1);
    sem_wait(&mutex1);
    // insert item
    insertitem(&producer1);
    sem_post(&mutex1);
    sem_post(&full1);
  }

  return NULL;
}

// Produce with producer2
void *produce2(void *arg)
{
  int toproduce, i;
  
  arguments *argument;
  argument = (arguments*)arg;
  
  toproduce = argument->value;
  


  int item;
  for (producer2.produced = 0; producer2.produced < toproduce; producer2.produced++)
  { 
    sem_wait(&empty2);
    sem_wait(&mutex2);
    // insert item
    insertitem(&producer2);
    sem_post(&mutex2);
    sem_post(&full2);
  }

  return NULL;
}



// Consumes from producer1 and producer2 until there is nothing left
void *consume(void *arg)
{
  int toconsume, i;

  arguments *argument;
  argument = (struct arguments *)arg;

  toconsume = argument->value;
  

  int consumefrom; // Keeps track of whether to consume from the first or second producer
  bool success;
  int item;
  for (consumer1.consumed = 0; consumer1.consumed < toconsume; consumer1.consumed++)
  {
    success = false;
    // Decide whether to consume from producer1 or producer2
    while (success == false)
    {
      if (sem_trywait(&full1) == 0) 
      {
        consumefrom = CONSUME_FROM_P1;
        success = true;
      }
      else if (sem_trywait(&full2) == 0)
      {
        consumefrom = CONSUME_FROM_P2;
        success = true;
      }
    }

    if (consumefrom == CONSUME_FROM_P1) // Consume from producer1
    {
      sem_wait(&mutex1);
      // remove the item
      item = removeitem(&producer1);
      sem_post(&mutex1);
      sem_post(&empty1);
      // consume the item
      printf("Consumed %d from producer1\n", item);
    }
    else                                // Consume from producer2
    {
      sem_wait(&mutex2);
      // remove the item
      item = removeitem(&producer2);
      sem_post(&mutex2);
      sem_post(&empty2);
      // consume the item
      printf("Consumed %d from producer2\n", item);
    }
  }





  return NULL;
}

int main(int argc, char* argv[])
{
  // Check the arguments
  if (argc < 5)
  {
    printf("USAGE: twoProducers [bufsize1] [bufsize2] [num of items to be produced by p1] [num of items to be produced by p2]\n");
    exit(EXIT_FAILURE);
  } 
  
  // Get the arguments.
  int bufsize1 = atoi(argv[1]);   // Buffer size for producer 1
  int bufsize2 = atoi(argv[2]);   // Buffer size for producer 2
  int toproduce1 = atoi(argv[3]); // Amount of units to be produced by producer 1
  int toproduce2 = atoi(argv[4]); // Amount of units to be produced by producer 2
 

  /**
   * Initialize semaphores and producers/consumers
   */

  // semaphores
  sem_init(&mutex1, 0, 1);
  sem_init(&mutex2, 0, 1);
  sem_init(&empty1, 0, bufsize1);
  sem_init(&empty2, 0, bufsize2);
  sem_init(&full1, 0, 0);
  sem_init(&full2, 0, 0);


  
  // producers
  producer1.buffersize = bufsize1;
  producer1.buffer = (int *) calloc (bufsize1, sizeof(int));
  producer1.ptr = producer1.buffer;
  producer1.produced = 0;

  producer2.buffersize = bufsize2;
  producer2.buffer = (int *) calloc (bufsize2, sizeof(int));
  producer2.ptr = producer2.buffer;
  producer2.produced = 0;

  // consumers
  consumer1.ptr = producer1.buffer; // Set to producer1 initially.
  consumer1.consumed = 0;

  /**
   * Set up threads and their arguments.
   */
  // pthreads
  pthread_t threadproducer1;
  pthread_t threadproducer2;
  pthread_t threadconsumer1;

  // return values
  void *retvalproducer1;
  void *retvalproducer2;
  void *retvalconsumer1;

  // arguments
  arguments *argproducer1;
  arguments *argproducer2;
  arguments *argconsumer1;
  
  // initialize arguments
  argproducer1 = (struct arguments *)malloc(sizeof(struct arguments));
  argproducer2 = (struct arguments *)malloc(sizeof(struct arguments));
  argconsumer1 = (struct arguments *)malloc(sizeof(struct arguments));

  argproducer1->value = toproduce1;
  argproducer2->value = toproduce2;
  argconsumer1->value = toproduce1 + toproduce2;

  
  /**
   * Create the threads
   */

  // producer1
  if (pthread_create(&threadproducer1, NULL, produce1, (void *) argproducer1))
  {
    fprintf(stderr, "Error while creating thread\n");
    exit(EXIT_FAILURE);
  }

  // producer2
  if (pthread_create(&threadproducer2, NULL, produce2, (void *) argproducer2))
  {
    fprintf(stderr, "Error while creating thread\n");
    exit(EXIT_FAILURE);
  }

  // consumer1
  if (pthread_create(&threadconsumer1, NULL, consume, (void *) argconsumer1))
  {
    fprintf(stderr, "Error while creating thread\n");
    exit(EXIT_FAILURE);
  }


  /**
   * Wait for threads to finish
   */

  // producer1
  if (pthread_join(threadproducer1, &retvalproducer1))
  {
    fprintf(stderr, "Error while waiting for thread\n");
    exit(EXIT_FAILURE);
  }
  // producer2
  if (pthread_join(threadproducer2, &retvalproducer2))
  {
    fprintf(stderr, "Error while waiting for thread\n");
    exit(EXIT_FAILURE);
  }
  // consumer1
  if (pthread_join(threadconsumer1, &retvalconsumer1))
  {
    fprintf(stderr, "Error while waiting for thread\n");
    exit(EXIT_FAILURE);
  }

  return 0;
}
