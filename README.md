<span style="color:blue">
# Multi-process-and-multi-threaded-print-linux-server
 </span>
<span style="color:red">
> **The code consists of :**
</span>
 **Signal Handling:** a signal handler function sigint_handler() that catches the SIGINT signal (Ctrl+C) and cancels all threads, destroys semaphores, and mutex, and exits gracefully.

**Producer Thread Function:** responsible for generating random jobs (represented by size variable) and adding them to the buffer. It uses semaphores (full_sem and empty_sem) to control the buffer size and mutex (buffer_mutex) to protect the critical section while accessing the buffer. It also keeps track of the number of producers using a global variable number_of_producers.

**Consumer Thread Function:** responsible for dequeuing items from the buffer and processing them. It uses semaphores (full_sem and empty_sem) to control the buffer size and mutex (buffer_mutex) to protect the critical section while accessing the buffer. It also checks if all producers have exited using the global variable number_of_producers and exits the thread gracefully.
**Main Function:** initializes the buffer, semaphores, and mutex. It validates the command-line arguments for the number of producers and consumers. It creates and starts the producer and consumer threads using the pthread_create() function.

**Command-line Arguments:** The code validates the command-line arguments to ensure that there are exactly 3 arguments (including the program name), and the values of num_producers and num_consumers are greater than or equal to 1.


## Explanation

> ***Logic used to identify the terminating condition:***

The terminating condition in the provided code is based on the assumption that the producer will eventually finish enqueuing all jobs and exit, and the consumer threads will continue running until they have dequeued and processed all jobs from the buffer.
There are several potential cases.
 The first case is that the number of producers entered in through command line is greater or equal than number of consumers, in that case the producers will produce print requests, and the available consumers will consume once the queue is no longer empty. 

The second case is that the number of consumers is greater than the number of producers, in that case the producers will produce print requests, and consumers will process them accordingly, and once all producers finish their required jobs, all threads are terminated, and semaphores are cleared.

Third case, if the user enters a number of arguments other than 2 arguments (representing the number of producers and consumers correspondingly), the code will eventually terminate.

![image](https://user-images.githubusercontent.com/84717550/231606296-9ce8c0ee-f244-4225-9ca8-887862ea9bd9.png)

Last handled case, if the user forces safe termination through signal ( CTRL + C ) during runtime
![image](https://user-images.githubusercontent.com/84717550/231606330-8f5a8c86-6f03-4983-9ab3-55005d2b51e4.png)

**Semaphores** and other **book-keeping variables** are shared between threads using global variables. Global variables are accessible to all threads in the process, allowing them to be shared and modified by multiple threads concurrently.
*For example:*
*Semaphores (empty, full, and mutex) are declared as global variables and are accessed and modified by both the producer and consumer threads.
A global struct is created to keep the book-keeping values such as producer_thread which indicates which producer request to print.**

![image](https://user-images.githubusercontent.com/84717550/231606397-204c5731-f267-4f42-9289-8876875b9744.png)

**The signal handler sigint_handler()** is used to handle the SIGINT signal, which is sent to the program when the user presses Ctrl+C in the terminal. The purpose of the signal handler is to perform graceful termination of the program by canceling all threads, destroying semaphores, releasing allocated memory, and exiting the program. It cancels threads, and cleans-up / exits.

![image](https://user-images.githubusercontent.com/84717550/231606429-abacb9f4-96f1-42c9-91f3-9fe4dfb653b4.png)


**Buffer_index** is supposed to keep track of the next index in the queue to be printed.
For instance, in LIFO implementation, dequeue from the queue is performed from the back which corresponds to buffer_index-1:
![image](https://user-images.githubusercontent.com/84717550/231606468-88ad72ee-5025-4139-964a-5b4f0c2080b0.png)

*The only adjustment in FIFO implementation is that the first index is always read, while shifting the queue elements to the left each time and decrementing the buffer_index:*

![image](https://user-images.githubusercontent.com/84717550/231606488-84a06bdf-f353-4f6e-a91f-532c6d35747a.png)
<span style="color:red">
> Execution Time and Average waiting time
</span>
The **execution time** is the time required for all producer/consumer threads to finish execution ( including time to request + time to process request ).
In FIFO, since the average waiting time for items in the buffer tends to be more balanced among all the items, as they are dequeued in the order they are enqueued.
That leads to an average waiting time less than LIFO implementation.
However in LIFO, the average waiting time for items in the buffer can be skewed towards the items that are pushed last onto the stack, as they are the first ones to be popped.
The execution time is expected to be the same in both cases, but it can vary depending on many factors.
For example, in LIFO, by adding 4 producers and 1 consumer ( to allow a proper delay in consumption ).

![image](https://user-images.githubusercontent.com/84717550/231606535-d70d197f-c7c5-47d4-bbea-47a4e4a9d123.png)
However in FIFO, the average waiting time is so much less:
![image](https://user-images.githubusercontent.com/84717550/231606553-1c583a0c-9bdc-417f-a00a-7baf38b18953.png)
<span style="color:red">
> 2D plots
</span>
Here are some 2D plots representing the relation between the number of consumers and producers using LIFO:
Since the number of jobs are randomized for each producer, in order to obtain correct results, I assumed a fixed number of jobs = 3

![image](https://user-images.githubusercontent.com/84717550/231606622-8a6ea83f-afcf-4545-afa5-e3b21a178826.png)
![image](https://user-images.githubusercontent.com/84717550/231606664-daa3f82f-c358-4885-b16e-434e072d1b71.png)
![image](https://user-images.githubusercontent.com/84717550/231606778-cf3f5680-36f4-4c00-8fc6-879b62bf1e63.png)
![image](https://user-images.githubusercontent.com/84717550/231606806-7f39ae5b-8768-4710-b066-73d1c29253b1.png)
![image](https://user-images.githubusercontent.com/84717550/231606849-e10aca1b-7cea-4b86-af75-621a409948fc.png)

We notice a slight unexpected variation due to the random delay in both producers and consumers. We also notice a noticeable decrement in the execution time as the number of producers increase.
The average waiting time is less in the FIFO implementation compared to the LIFO implementation as mentioned before.

