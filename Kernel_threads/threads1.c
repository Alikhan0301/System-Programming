#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/kthread.h>
#include <linux/delay.h>

static struct task_struct *thread_st;


// Our function for kernel thread (function that will be executed by kernel thread)
static int thread_fn(void *arg)
{
    while (!kthread_should_stop()){
        printk(KERN_INFO "Thread is running\n");
        ssleep(5);
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
    thread_st = kthread_run(thread_fn, NULL, "thread1");
    if (thread_st)
        printk(KERN_INFO "Thread was successfully created\n");
    else
        printk(KERN_ERR "Thread creation is failed !!\n");
    return 0;
}
// Module Exit
static void __exit cleanup_thread(void)
{
    printk(KERN_INFO "Cleaning Up\n");
    if (thread_st){
        kthread_stop(thread_st);
        printk(KERN_INFO "Thread stopped");
    }
}
MODULE_LICENSE("GPL");
module_init(init_thread);
module_exit(cleanup_thread);

