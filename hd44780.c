/**
 * @file	hd44780.c 
 * @author	Daniel Obermaier, Victor Nagy, Markus Fischer
 * @date	01. June 2016
 * @version	0.1
 * @brief	loadable kernel module character device driver for support a simple 2x16 lcd display.
 * within the lcd display is a commonly used HD44780 controller implemented.
 * @see https://www.sparkfun.com/datasheets/LCD/HD44780.pdf datasheet for this hd44780 controller  	  
 */

#include <linux/module.h>	//core header for loading LKMs into the kernel
#include <linux/moduleparam.h>
#include <linux/kernel.h>	//for prink priority macros
#include <linux/init.h>		//for entry/exit macros to mark up functions e.g. __init __exit
#include <linux/fs.h>		//header for the linux file system support
#include <linux/cdev.h>
#include <linux/types.h>
#include <linux/device.h>	//header to support the kernel module driver
#include <linux/cdev.h>
#include <linux/gpio.h>
#include <linux/delay.h>
#include <linux/ctype.h>
#include <asm/uaccess.h>	//required for copy_to_user() function
#include <asm/errno.h>
#include <linux/wait.h>
#include <linux/hrtimer.h>
#include <linux/ktime.h>
#include <linux/interrupt.h>

#include "ioctl_header.h"	//includes self-written IOCTL macros and linux/ioctl.h as well

MODULE_AUTHOR("Daniel Obermaier <mailto:dan.obermaier@gmail.com>");			// author(s) -> visible when using modinfo
MODULE_DESCRIPTION("driver for LCD Display with HD44780 controller");		// describtion -> visible when using modinfo
MODULE_LICENSE("GPL");														// license type -> affects available functionality
MODULE_VERSION("0.1");														// version number

static int major = 0;
static int minor = 0;
static int count = 1;

dev_t dev = 0;

static int dev_open = 0; //is device open? used to prevent multiple access

static struct cdev *driver_object;
static struct class *hd44780_class;
static struct device *hd44780_dev;
static char textbuffer[1024];

/*function prototypes start*/
static void write_nibble(int regist, int value);
static void write_lcd(int regist, int value);
static int gpio_request_output(int nr);
static int exit_display(void);

/*special lkm function prototypes*/
static void __exit mod_exit(void);
static int __init init_display(void);

//prototype functions for the character driver (callbacks to/from user)
static int driver_open(struct inode* inode, struct file *fp);
static int driver_release(struct inode* inode, struct file *fp);
static long driver_ioctl(struct file *fp, unsigned int cmd, unsigned long arg);
static ssize_t driver_write(struct file *instance, const char __user *user, size_t cnt, loff_t *offset);
/*function prototypes end*/


/** @brief Devices are represented as file structures in the kernel. 
 * file_operation struct from /linux/fs.h lists the various callback 
 * functions which can be associated with file operations 
*/
static struct file_operations fops = {
	.owner = THIS_MODULE,
	.open = driver_open,
	.write = driver_write,
	.unlocked_ioctl = driver_ioctl,
	.release = driver_release
};

//module parameters -> allow arguments to be passed to modules
module_param(major, int, S_IRUGO | S_IWUSR);
module_param(count, int, S_IRUGO | S_IWUSR);

static void write_nibble(int regist, int value){

	gpio_set_value(7, regist);
	
	gpio_set_value(25, value & 0x1); //DATABIT 4
	gpio_set_value(24, value & 0x2); //DATABIT 5
	gpio_set_value(23, value & 0x4); //DATABIT 6
	gpio_set_value(18, value & 0x8); //DATABIT 7

	gpio_set_value(8, 1); //enabled to write values
	
	udelay(40);

	gpio_set_value(8, 0); //disabled to write values
}


static void write_lcd(int regist, int value){
	write_nibble(regist, value >> 4); //HIGH-Nibble logic
	write_nibble(regist, value & 0xf); //LOW-Nibble logic
}


static int gpio_request_output(int nr){
	
	char gpio_name[12];
	int err;

	snprintf( gpio_name, sizeof(gpio_name), "rpi-gpio-%d", nr);
	err = gpio_request(nr, gpio_name);

	if(err){
		printk("gpio request for %s failed with %d\n", gpio_name, err);
		return -1;
	}
	err = gpio_direction_output(nr, 0);
	if(err){
		printk("gpio direction output failed %d\n", err);
		gpio_free(nr);
		return -1;
	}
	return 0;
}


static int init_display(void){

printk("initialize display\n");

if(gpio_request_output(7) == -1){
	 return -EIO;
}
if(gpio_request_output(8) == -1){
	goto free7;
}
if(gpio_request_output(18) == -1){
	goto free8;
}
if(gpio_request_output(23) == -1){
	goto free18;
}
if(gpio_request_output(24) == -1){
	goto free23;
}
if(gpio_request_output(25) == -1){
	goto free24;
}

msleep(15);
write_nibble(0, 0x3);
msleep(5);
write_nibble(0, 0x3);
udelay(100);
write_nibble(0, 0x3);
msleep(5);
write_nibble(0, 0x2);
msleep(5);
write_lcd(0, 0x28);	//Command: 4-Bit Mode, 2 lines
msleep(2);
write_lcd(0, 0x01);
msleep(2);

write_lcd(0, 0x0c);	 //display on, cursor off, blink off
write_lcd(0, 0xc0);
//TESTS!!!!!!!!!!!!!!!!!

//write_lcd(0, 0x41);
//write_lcd(1, 'M');
//write_lcd(1, 'l');
//write_lcd(1, 'l');
//write_lcd(1, 'o');

	return 0;

free24: gpio_free(24);
free23: gpio_free(23);
free18: gpio_free(18);
free8: gpio_free(8);
free7: gpio_free(7);
	return -EIO;
}


static int exit_display(void){
	printk("exit display called\n");
	gpio_free(25);
	gpio_free(24);
	gpio_free(23);
	gpio_free(18);
	gpio_free(8);
	gpio_free(7);
	return 0;
}

static int driver_open(struct inode* inode, struct file *fp){

dev_open++;	//increment counter	
	//increment usage count

return 0;	
}


static int driver_release(struct inode* inode, struct file *fp){

dev_open--;	
	//decrement usage counter, or else the module cannot be closed properly

return 0;
}

/** @brief function is called when device is being written from user space
 * @param: pointer to a file instance
 * @param: buffer contains the string to write onto the device
 * @param: size of array that is being passed in the const char buffer
 * @param: offset if required
 */
static ssize_t driver_write(struct file *instance, const char __user *user, size_t cnt, loff_t *offset){

	unsigned long not_copied; 
	unsigned long to_copy;
	int i;

	char msg_from_user[26] = { 0 };

	to_copy = min(cnt, sizeof(textbuffer));
	not_copied = copy_from_user(textbuffer, user, to_copy);

	write_lcd(0, 0x80);

	for(i = 0; i < to_copy && textbuffer[i]; i++){
		if(isprint(textbuffer[i])){
			write_lcd(1, textbuffer[i]);
		}
		if( i == 15){
			write_lcd(0, 0xc0);
		}
	}

	if(copy_from_user(msg_from_user, user, cnt)) {
		printk("failed copy from user");
	}
return to_copy-not_copied;
}


static long driver_ioctl(struct file *fp, unsigned int cmd, unsigned long arg){
int retval = 0;
int value = 5;

switch(cmd){
	case IOCTL_WRITE:
//		put_user(value, (int *)arg);
		printk("IOCTL_WRITE was selected\n");
		break;
//	case IOCTL_CLEAR:
//		put_user(value, (int *)arg);
//		printk("IOCTL_CLEAR was selected\n");
//		break;
	default:
		printk("no argument was selected\n");
		retval = -EFAULT;
		break;
	}
return retval;
}

/** @brief kernel module initialization function
 * the static keyword restricts the visibility of the function within this C file
 * __init macro means that for a built-in driver (not a kernel module!) 
 * is only used at initialization time and that it can be discarded and its memory freed after.
 * @return returns 0 if successful
 */
static int __init mod_init(void){
int error = 0; 	

dev = MKDEV(major, minor);

if(register_chrdev_region(MKDEV(major, 0),count,"hd44780") < 0){
	printk("devicenumber(255, 0) in use!\n");
	return -EIO;
}
else{
	error = alloc_chrdev_region(&dev, 0, count, "hd44780");
	major = MAJOR(dev);
}

driver_object = cdev_alloc();	/* registered object reserved*/

if(driver_object == NULL){
	goto free_device_number;
}

driver_object->owner = THIS_MODULE;
driver_object->ops = &fops;
	
if(cdev_add(driver_object, dev, 1)){
	goto free_cdev;
}

hd44780_class = class_create(THIS_MODULE, "hd44780");

if(IS_ERR(hd44780_class)){
	pr_err("hd44780: no udev support!\n");
	goto free_cdev;
}

hd44780_dev = device_create(hd44780_class, NULL, dev, NULL, "%s", "hd44780");
dev_info(hd44780_dev, "mod_init called\n");

if(init_display() == 0){
	return 0;
}

free_cdev:
	kobject_put(&driver_object->kobj);
free_device_number:
	unregister_chrdev_region(dev, 1);
	printk("mod_init failed\n");
	return -EIO;
}


/** @brief lkm exit function
 * similiar to initialization function, it is static. __exit macro notifies 
 * if code is used for a built-in driver (not a lkm) that this function is not required.
 */ 
static void __exit mod_exit(void){
	dev_info(hd44780_dev, "mod_exit called\n");
	exit_display();
	device_destroy(hd44780_class, dev);
	class_destroy(hd44780_class);
	cdev_del(driver_object);
	unregister_chrdev_region(dev, 1);
	return;
}


/** @brief a module must use the module_init() and module_exit() macros from linux/init.h
 * which identify the initialization function at insertion time and the 
 * cleanup function (as listed above)
 */
module_init(mod_init);
module_exit(mod_exit);


