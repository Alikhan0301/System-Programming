#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/kthread.h>
#include <linux/delay.h>
#include <linux/mutex.h>
#include <linux/random.h>
#include <linux/semaphore.h>


/*
	Semaphore
	// Operations
	void down((&sem);
	int down_interruptible(&sem);
	int down_trylock(&sem);
	void up(&sem);
*/


//static struct my_sem;

static struct task_struct *thread_test;

DEFINE_MUTEX(my_mutex);
DEFINE_SEMAPHORE(my_sem);


static int counter = 0;


// Our function for kernel thread (function that will be executed by kernel thread)
// Added mutex for avoiding some side effects of multitasking
static int thread_function_with_mutex(void *arg)
{
    while (!kthread_should_stop()){
        mutex_lock(&my_mutex);

        counter++;
        printk(KERN_INFO "Task number %d is started\n", counter);
	
        ssleep(5);
        printk(KERN_INFO "Task number %d is finished\n", counter);

        mutex_unlock(&my_mutex);
    }
    printk(KERN_INFO "Thread is stopping\n");
    printk(KERN_INFO "Function with mutex is successfully completed !");
    do_exit(0);
    return 0;
}
static int thread_function_with_semaphore(void *arg)
{
    while (!kthread_should_stop()){
        if (down_interruptible(&my_sem))
            break;
        counter++;
        printk(KERN_INFO "Task number %d is started\n", counter);
        ssleep(5);
        printk(KERN_INFO "Task number %d is finished\n", counter);

        up(&my_sem);
    }
    printk(KERN_INFO "Thread is stopping\n");
    printk(KERN_INFO "Function with binary semaphore is successfully completed !");
    do_exit(0);
    return 0;
}

// Module Initialization
static int __init init_thread(void)
{
    // Binary semaphore
    sema_init(&my_sem, 1);
    
    printk(KERN_INFO "Thread is creating\n");
    // Creating the kernel thread with name thread1
    thread_test = kthread_run(thread_function_with_semaphore, NULL, "thread1");

    if (thread_test){
        printk(KERN_INFO "Thread was successfully created\n");
    }
    else{
        printk(KERN_ERR "Thread creation is failed !!\n");
	
    }
    return 0;
}
// Module Exit
static void __exit cleanup_thread(void)
{
    printk(KERN_INFO "Cleaning Up\n");
    if (thread_test){
        kthread_stop(thread_test);
        printk(KERN_INFO "Thread stopped");
    }
}
MODULE_LICENSE("GPL");
module_init(init_thread);
module_exit(cleanup_thread);

