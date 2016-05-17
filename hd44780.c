#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/gpio.h>
#include <linux/delay.h>
#include <linux/ctype.h>
#include <asm/uaccess.h>

static dev_t hd44780_dev_number = MKDEV(255, 0);
static struct cdev *driver_object;
static struct class *hd44780_class;
static struct device *hd44780_dev;
static char textbuffer[1024];


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
	nibble_write(reg, value >> 4); //HIGH-Nibble logic
	nibble_write(reg, value & 0xf); //LOW-Nibble logic
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


static int initialize_disp(){

	printk("initialize display\n");

	if(gpio_request_output(7) == -1){
		 return -EI0;
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
nibble_write(0, 0x3);
msleep(5);
nibble_write(0, 0x3);
udelay(100);
nibble_write(0, 0x3);
msleep(5);
nibble_write(0, 0x2);
msleep(5);
lcd_write(0, 0x28);	//Command: 4-Bit Mode, 2 lines
msleep(2);
lcd_write(0, 0x01);
msleep(2);

lcd_write(0, 0x0c);	 //display on, cursor off, blink off
lcd_write(0, 0xc0);
lcd_write(1, 'H');
lcd_write(1, 'i');
	return 0;

free24: gpio_free(24);
free23: gpio_free(23);
free18: gpio_free(18);
free8: gpio_free(8);
free7: gpio_free(7);
	return -EI0;
}


static int display_exit(){
	printk("exit display called\n");
	gpio_free(25);
	gpio_free(24);
	gpio_free(23);
	gpio_free(18);
	gpio_free(8);
	gpio_free(7);
	return 0;
}


static ssize_t driver_write(struct file *instance, const char __user *user, size_t cnt, loff_t *offset){

	unsigned long not_copied = copy_from_user(textbuffer, user, to_copy);
	unsigned long to_copy = min(cnt, sizeof(textbuffer));

	lcd_write(0, 0x80);

	for(int i = 0; i < to_copy && textbuffer[i]; i++){
		if(isprint(textbuffer[i])){
			lcd_write(1, textbuffer[i]));
		}
		if( i == 15){
			lcd_write(0, 0xc0);
		}
		return to_copy-not_copied;
	}

	static struct file_operations fops = {
		.owner = THIS_MODULE,
		.write = driver_write,
	};

	static int __init mod_init(){
		if(register_chrdev_region(hd44780_dev_number, 1, "hd44780") < 0){
			printk("devicenumber(255, 0) in use!\n");
			return -EI0;
	}
	driver_object = cdev_alloc();	/* registered object reserved*/

	if(driver_object == NULL){
		goto free_device_number;
	}
	driver_object->owner = THIS_MODULE;
	driver_object->ops = &fops;
	if(cdev_add(driver_object, hd44780_dev_number, 1)){
		goto free_cdev;
	}
	hd44780_class = class_create(THIS_MODULE, "hd44780");
	if(IS_ERR(hd44780_class)){
		pr_err("hd44780: no udev support!\n");
		goto free_cdev;
	}
	hd44780_dev = device_create(hd44780_class, NULL, hd44780_dev_number, NULL, "%s", "hd44780");
	dev_info(hd44780_dev, "mod_init called\n");
	if(display_init() == 0){
		return 0;
	}

	free_cdev:
		kobject_put(&driver_object->kobj);
	free_device_number:
		unregister_chrdev_region(hd44780_dev_number, 1);
		printk("mod_init failed\n");
		return -EI0;
}

static void __exit mod_exit(){
	dev_info(hd44780_dev, "mod_exit called\n");
	display_exit();
	device_destroy(hd44780_class, hd44780_dev_number);
	class_destroy(hd44780_class);
	cdev_del(driver_object);
	unregister_chrdev_region(hd44780_dev_number, 1);
	return;
}

module_init(mod_init);
module_exit(mod_exit);
MODULE_LICENSE("GPL");


