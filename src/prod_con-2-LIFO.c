#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>

#define SIZE 20
#define NUMB_THREADS 6
#define PRODUCER_LOOPS 2

typedef int buffer_t;

typedef struct
{
    buffer_t value;      // Value stored in the buffer
    int producer_thread; // Thread number of the producer that pushed the value
} buffer_item_t;

buffer_item_t buffer[SIZE];

volatile int terminate_threads = 0;
int buffer_index;
int number_of_producers = 0;
int num_producers = 0;
int num_consumers = 0;
pthread_t thread[100];
int thread_numb[100];
clock_t waiting[200];

double execution_time=0;
double avgWaitTime = 0;
long totalWaitTime = 0;

pthread_mutex_t buffer_mutex;
/* initially buffer will be empty.  full_sem
   will be initialized to buffer SIZE, which means
   SIZE number of producer threads can write to it.
   And empty_sem will be initialized to 0, so no
   consumer can read from buffer until a producer
   thread posts to empty_sem */
sem_t full_sem;  /* when 0, buffer is full */
sem_t empty_sem; /* when 0, buffer is empty. Kind of
                    like an index for the buffer */

// signal handler
void sigint_handler(int sig)
{
    // Cancel all threads
    printf("\nReceived SIGINT signal. Terminating all threads and Destroying semaphores...\n");
    for (int i = 1; i < (num_consumers * 2) + 1; i++)
    {
        pthread_cancel(thread[i]);
        i++;
    }

    sem_destroy(&full_sem);
    sem_destroy(&empty_sem);
    pthread_mutex_destroy(&buffer_mutex);

    exit(0);
}

void insertbuffer(buffer_t value, int thread_numb)
{
    if (buffer_index < SIZE)
    {
        buffer[buffer_index].value = value;
        buffer[buffer_index].producer_thread = thread_numb;
        waiting[thread_numb]=clock();
        buffer_index++; // Increment buffer_index after updating the array
    }
    else
    {
        printf("Buffer overflow\n");
    }
}

buffer_item_t dequeuebuffer()
{
    buffer_item_t item;
    if (buffer_index > 0)
    {

        item.value = buffer[--buffer_index].value;
        item.producer_thread = buffer[buffer_index].producer_thread;
        avgWaitTime+=(clock()-waiting[buffer_index])*1000/ CLOCKS_PER_SEC;
        return item;
    }
    // Add a return statement in case the if condition is not met
    pthread_exit(0);
    buffer_item_t empty_item = {-1}; // Assumes buffer_item_t is a struct
    return empty_item;
}

void *producer(void *thread_n)
{
    int thread_numb = *(int *)thread_n;
    buffer_t size;
    int i = 0;
    int num_jobs = rand() % 20 + 1; // Random number of jobs between 1 and 20
   
    long waitingTime = 0;
    clock_t lastClock;
    while (i++ < num_jobs)
    {
        
        srand(time(NULL));                      // Seed random number generator
        float delay = rand() % 900000 + 100000; // Generate random delay between 0.1 - 1 sec
        usleep(delay);  // Sleep for random delay
        execution_time+=(double)delay /1000;
       
   
        size = rand() % 901 + 100; // between 100 and 1000

        lastClock = clock();
        sem_wait(&full_sem);                // sem=0: wait. sem>0: go and decrement it
        waitingTime += clock() - lastClock;
     
        pthread_mutex_lock(&buffer_mutex); /* protecting critical section */
        if (buffer_index < SIZE)
        {
            insertbuffer(size, thread_numb);
            printf("Producer %d added %d to buffer\n", thread_numb, size);
        }
        else
        {
            printf("Buffer overflow\n");
        }
        pthread_mutex_unlock(&buffer_mutex);

        sem_post(&empty_sem); // post (increment) emptybuffer semaphore
    }
    totalWaitTime += waitingTime; // Add waiting time for this thread

    number_of_producers--;

    if (number_of_producers == 0)
    {
        for (i = 1; i < (num_consumers * 2) + 1; i++)
        {
            pthread_cancel(thread[i]);
            i++;
        }
    }

    pthread_exit(0);
}

void *consumer(void *thread_n)
{
    int thread_numb = *(int *)thread_n;
    buffer_item_t value;
    long waitingTime = 0;
    while (1)
    {
     
        sem_wait(&empty_sem);
        pthread_mutex_lock(&buffer_mutex);
        value = dequeuebuffer(value);
        pthread_mutex_unlock(&buffer_mutex);

      
        sem_post(&full_sem);
        printf("Consumer %d dequeue %d,%d from buffer\n", thread_numb, value.producer_thread, value.value);

        // Check if all producers have exited
        if (number_of_producers == 0)
        {
            sem_post(&full_sem);
            pthread_mutex_unlock(&buffer_mutex);
            pthread_exit(0);
        }
        srand(time(NULL));                    // Seed random number generator
        int delay = rand() % 900000 + 100000; // Generate random delay between 0.1 - 1 sec
        usleep(delay);                        // Sleep for random delay
    }
}

int main(int argc, char *argv[])
{
    if (argc != 3 || atoi(argv[1]) < 1 || atoi(argv[2]) < 1)
    {
        printf("Invalid Arguments\nUsage: %s <num_producers> <num_consumers>\n", argv[0]);
        return 1;
    }
    buffer_index = 0;
    num_producers = atoi(argv[1]);
    num_consumers = atoi(argv[2]);
    number_of_producers = num_producers; // assign a global variable to keep track of producers

    // define a signal handler
    signal(SIGINT, sigint_handler);

    pthread_mutex_init(&buffer_mutex, NULL);
    sem_init(&full_sem, // sem_t *sem
             0,         // int pshared. 0 = shared between threads of process,  1 = shared between processes
             SIZE);     // unsigned int value. Initial value
    sem_init(&empty_sem,
             0,
             0);
    /* full_sem is initialized to buffer size because SIZE number of
       producers can add one element to buffer each. They will wait
       semaphore each time, which will decrement semaphore value.
       empty_sem is initialized to 0, because buffer starts empty and
       consumer cannot take any element from it. They will have to wait
       until producer posts to that semaphore (increments semaphore
       value) */

    //thread = malloc(sizeof(pthread_t) * (num_consumers + num_producers));
    //thread_numb = malloc(sizeof(int) * (num_consumers + num_producers));

    // pthread_t thread[NUMB_THREADS];
    // int thread_numb[NUMB_THREADS];
    int i;

    for (i = 0; i < num_producers * 2; i++)
    {
        thread_numb[i] = i;
        pthread_create(&thread[i], NULL, producer, &thread_numb[i]);
        i++;
    }

    for (i = 1; i < (num_consumers * 2) + 1; i++)
    {
        thread_numb[i] = i;
        pthread_create(&thread[i], NULL, consumer, &thread_numb[i]);
        i++;
    }

    clock_t start_time = clock();
    for (i = 0; i < num_producers + num_consumers; i++)
    {
        pthread_join(thread[i], NULL);
    }
    clock_t end_time = clock();
    execution_time+= (double)(end_time - start_time) *1000/ CLOCKS_PER_SEC;
    printf("Execution time: %f milliseconds\n", execution_time);

    avgWaitTime = totalWaitTime / (num_producers);
   
    double avgWaitTimeMs = avgWaitTime*1000/CLOCKS_PER_SEC;
    printf("Average waiting time: %f milliseconds\n", avgWaitTimeMs); 
    
    pthread_mutex_destroy(&buffer_mutex);
    sem_destroy(&full_sem);
    sem_destroy(&empty_sem);

    return 0;
}