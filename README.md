# System-Programming
Kernel threads project
Solving a side effects of multitasking

in Kernel_threads :

MUTEX:
I used mutex for synchronization of our threads
In computer programming, a mutual exclusion object (mutex) is a program object that allows multiple program threads to share the same resource, such as file access, but not simultaneously. When a program is started, a mutex is created with a unique name.
Binary Semaphore:
Semaphore is a simply a variable. This variable is used to solve critical section problem and to achieve process synchronization in the multi processing environment.
Binary semaphore can take the value 0 & 1. 

in ReadWritSemaphore:
Any number of readers can read from the shared resource simultaneously, but only one writer can write to the shared resource. When a writer is writing data to the resource, no other process can access the resource.


useful links : 

file:///Users/alikhanokas/Downloads/x569%20(1).html
https://github.com/torvalds/linux/blob/master/kernel/locking/semaphore.c
https://github.com/torvalds/linux/blob/master/kernel/kthread.c

