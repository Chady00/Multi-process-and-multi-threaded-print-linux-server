
# Multi-process-and-multi-threaded-print-linux-server
You can either test using a makefile, or type the following commands:

    //FIFO threads
    gcc prod_con-2_FIFO.c -o my_program_FIFO -lpthread
    ./my_program_FIFO <number of producers> <number of consumers>
    
    //LIFO threads
    gcc prod_con-2-LIFO.c -o my_program_LIFO -lpthread
    ./my_program_LIFO <number of producers> <number of consumers>



> **The code consists of :**
> 

 - **Signal Handling:** a signal handler function sigint_handler() that catches the SIGINT signal (Ctrl+C) and cancels all threads, destroys
   semaphores, and mutex, and exits gracefully.
 - **Producer Thread Function:** responsible for generating random jobs (represented by size variable) and adding them to the buffer. It uses
   semaphores (full_sem and empty_sem) to control the buffer size and
   mutex (buffer_mutex) to protect the critical section while accessing
   the buffer. It also keeps track of the number of producers using a
   global variable number_of_producers.
 - **Consumer Thread Function:** responsible for dequeuing items from the buffer and processing them. It uses semaphores (full_sem and
   empty_sem) to control the buffer size and mutex (buffer_mutex) to
   protect the critical section while accessing the buffer. It also
   checks if all producers have exited using the global variable
   number_of_producers and exits the thread gracefully.
 - **Main Function:** initializes the buffer, semaphores, and mutex. It validates the command-line arguments for the number of producers and
   consumers. It creates and starts the producer and consumer threads
   using the pthread_create() function.
 - **Command-line Arguments:** The code validates the command-line arguments to ensure that there are exactly 3 arguments (including the
   program name), and the values of num_producers and num_consumers are
   greater than or equal to 1.

## Explanation

> ***Logic used to identify the terminating condition:***

The terminating condition in the provided code is based on the assumption that the producer will eventually finish enqueuing all jobs and exit, and the consumer threads will continue running until they have dequeued and processed all jobs from the buffer.
There are several potential cases.

 - The first case is that the number of producers entered in through
   command line is greater or equal than number of consumers, in that
   case the producers will produce print requests, and the available
   consumers will consume once the queue is no longer empty.
 - The second case is that the number of consumers is greater than the
   number of producers, in that case the producers will produce print
   requests, and consumers will process them accordingly, and once all
   producers finish their required jobs, all threads are terminated, and
   semaphores are cleared.
 - Third case, if the user enters a number of arguments other than 2
   arguments (representing the number of producers and consumers
   correspondingly), the code will eventually terminate.

![image](https://user-images.githubusercontent.com/84717550/231606296-9ce8c0ee-f244-4225-9ca8-887862ea9bd9.png)

 - Last handled case, if the user forces safe termination through signal
   ( CTRL + C ) during runtime

![image](https://user-images.githubusercontent.com/84717550/231606330-8f5a8c86-6f03-4983-9ab3-55005d2b51e4.png)

**Semaphores** and other **book-keeping variables** are shared between threads using global variables. Global variables are accessible to all threads in the process, allowing them to be shared and modified by multiple threads concurrently.
*For example:*

*Semaphores (empty, full, and mutex) are declared as global variables and are accessed and modified by both the producer and consumer threads.

A global struct is created to keep the book-keeping values such as producer_thread which indicates which producer request to print.**

    typedef struct
    {
        buffer_t value;      // Value stored in the buffer
        int producer_thread; // Thread number of the producer that pushed the value
    } buffer_item_t;
    
    buffer_item_t buffer[SIZE];

**The signal handler sigint_handler()** is used to handle the SIGINT signal, which is sent to the program when the user presses Ctrl+C in the terminal. The purpose of the signal handler is to perform graceful termination of the program by canceling all threads, destroying semaphores, releasing allocated memory, and exiting the program. It cancels threads, and cleans-up / exits.

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


**Buffer_index** is supposed to keep track of the next index in the queue to be printed.
For instance, in LIFO implementation, dequeue from the queue is performed from the back which corresponds to buffer_index-1:

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


****The only adjustment in FIFO implementation is that the first index is always read, while shifting the queue elements to the left each time and decrementing the buffer_index:****

    buffer_item_t dequeuebuffer()
    {
        buffer_item_t item;
        if (buffer_index > 0)
        {
            item.value = buffer[0].value;
            item.producer_thread = buffer[0].producer_thread;
            avgWaitTime+=(clock()-waiting[0])*1000/ CLOCKS_PER_SEC;
            // Shift all elements one position to the left
            for (int i = 0; i < buffer_index - 1; i++)
            {
                buffer[i] = buffer[i + 1];
            }
    
            buffer_index--; // Decrement buffer_index after updating the array
            return item;
        }
        // Add a return statement in case the if condition is not met
        pthread_exit(0);
        buffer_item_t empty_item = {-1}; // Assumes buffer_item_t is a struct
        return empty_item;
    }

> Execution Time and Average waiting time

The **execution time** is the time required for all producer/consumer threads to finish execution ( including time to request + time to process request ).
In FIFO, since the average waiting time for items in the buffer tends to be more balanced among all the items, as they are dequeued in the order they are enqueued.
That leads to an average waiting time less than LIFO implementation.
However in LIFO, the average waiting time for items in the buffer can be skewed towards the items that are pushed last onto the stack, as they are the first ones to be popped.
The execution time is expected to be the same in both cases, but it can vary depending on many factors.
For example, in LIFO, by adding 4 producers and 1 consumer ( to allow a proper delay in consumption ).

    Producer 6 added 831 to buffer 
    Execution time: 17393.487000 milliseconds 
    Average waiting time 7.433000 milliseconds 

**However in FIFO, the average waiting time is so much less:**

    Consumer 1 dequeue 0,658 from buffer
    Producer 6 added 624 to buffer 
    Execution time. 16349.140000 milliseconds
    Average waiting time, 1.653000 milliseconds 

> 2D plots

Here are some 2D plots representing the relation between the number of consumers and producers using LIFO:
Since the number of jobs are randomized for each producer, in order to obtain correct results, I assumed a fixed number of jobs = 3

![image](https://user-images.githubusercontent.com/84717550/231606622-8a6ea83f-afcf-4545-afa5-e3b21a178826.png)
![image](https://user-images.githubusercontent.com/84717550/231606664-daa3f82f-c358-4885-b16e-434e072d1b71.png)
![image](https://user-images.githubusercontent.com/84717550/231606778-cf3f5680-36f4-4c00-8fc6-879b62bf1e63.png)
![image](https://user-images.githubusercontent.com/84717550/231606806-7f39ae5b-8768-4710-b066-73d1c29253b1.png)
![image](https://user-images.githubusercontent.com/84717550/231606849-e10aca1b-7cea-4b86-af75-621a409948fc.png)

 - We notice a slight unexpected variation due to the random delay in
   both producers and consumers. We also notice a noticeable decrement
   in the execution time as the number of producers increase. The
   average waiting time is less in the FIFO implementation compared to
   the LIFO implementation as mentioned before.
