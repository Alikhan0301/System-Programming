#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/kthread.h>
#include <linux/delay.h>
#include <linux/mutex.h>
#include <linux/random.h>

static struct task_struct *thread_test;
DEFINE_MUTEX(my_mutex);


static int counter = 0;


// Our function for kernel thread (function that will be executed by kernel thread)
// Added mutex for avoiding some side effects of multitasking
static int thread_function(void *arg)
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
    do_exit(0);
    return 0;
}

// Module Initialization
static int __init init_thread(void)
{
    printk(KERN_INFO "Thread is creating\n");
    // Creating the kernel thread with name thread1
    thread_test = kthread_run(thread_function, NULL, "thread1");
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

