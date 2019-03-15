#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/kthread.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <asm/uaccess.h>
#include <linux/semaphore.h>
#include <linux/sched.h>
#include <linux/delay.h>

#define number_of_minors 1

static dev_t dev;
static struct cdev char_dev;
static struct class * myClass; // class that this device should be registered to
static struct task_struct *task;

static struct rw_semaphore rwsem;

// FROM API of char device : open, release, read, write, file_operations
/*
    In read and write
 filp - the file pointer
 count - the size of the requested data transfer.
 buff argument points to the user buffer holding the data to be written or the empty buffer where the newly read data should be placed
 offp - a pointer to a “long offset type” object that indicates the file position the user is accessing.
 
 */



int open(struct inode *inode, struct file *filp){
    printk(KERN_INFO "Opening the file\n");
    task = current;
    return 0;
}
ssize_t read(struct file *filp, char *buff, size_t count, loff_t *offp){
    printk("We are inside reading\n");
    down_read(&rwsem);
    printk(KERN_INFO "We are in the Semophore read !!\n");
    printk("Waiting some time before release\n");
    ssleep(40);
    up_read(&rwsem);
    return 0;
}

ssize_t write(struct file *filp, const char *buff, size_t count, loff_t *offp){
    printk(KERN_INFO "We are inside writing\n");
    printk(KERN_INFO "Waiting for access to file\n");
    down_write(&rwsem);
    
    printk(KERN_INFO "We are in the Semophore write !!\n");
    
    up_write(&rwsem);
    return count;
}
int release(struct inode *inode, struct file *filp){
    printk(KERN_INFO "Release the file\n");
    return 0;
}
struct file_operations fops =
{
    read:    read,
    write:   write,
    open:    open,
    release: release
};
// /////

// Read and Write Semaphore
int rwSem_init(void){
    int result;
    struct device *dev_1;
    // first minor 0, number_of_minors 1
    {
        // Preparing the device and handling some ERRORS:
        if ((result = alloc_chrdev_region(&dev, 0, number_of_minors, "rwSem")) < 0){
            printk(KERN_INFO "Error occurred in allocation of range");
            /*
	     register a range of char device numbers
             alloc_chrdev_region
             params:
                 dev - output parameter for first assigned number
                 baseminor - first of the requested range of minor numbers
                 count - the number of minor numbers required
                 name - the name of the associated device or driver
             */
            return result;
        }
        // Initialize our char dev with this fops
        cdev_init(&char_dev, &fops);
        // Adding the device to our system
        if ((result = cdev_add(&char_dev, dev, number_of_minors)) < 0){
            unregister_chrdev_region(dev, number_of_minors);
            /*
             unregister_chrdev_region:
             params:
                 from -  the first in the range of numbers to unregister
                 count  - the number of device numbers to unregister
             */
            printk(KERN_INFO "Error occurred in adding a char device to the system!");
            return result;
        }
        // Creating the class
        if (IS_ERR(myClass = class_create(THIS_MODULE, "ccdev"))){
            cdev_del(&char_dev);//removes a cdev from the system
	    //class_create - creates a struct class structure
            unregister_chrdev_region(dev, number_of_minors);
            printk(KERN_INFO "Error occurred in class creation !");
            return PTR_ERR(myClass);
        }
        // Device creating
        if (IS_ERR(dev_1 = device_create(myClass, NULL, dev, NULL, "alikhan%d", 0))){
            /*
		
                device_create
                params:
                    class
                    parent (NULL in our case)
                    dev_t - for the char device to be added
                    string for the device's name
             */
            class_destroy(myClass);
            cdev_del(&char_dev);
            unregister_chrdev_region(dev, number_of_minors);
            printk(KERN_INFO "Error occurred in device creation !");
            return PTR_ERR(dev_1);
        }
    }
    // init our rwsem
    init_rwsem(&rwsem);
    return 0;
}

void rwSem_cleanup(void)
{
    printk(KERN_INFO "We are in the cleaning up module\n");
    device_destroy(myClass, dev);
    class_destroy(myClass);
    cdev_del(&char_dev);
    unregister_chrdev_region(dev, number_of_minors);
}

module_init(rwSem_init);
module_exit(rwSem_cleanup);
MODULE_LICENSE("GPL");
