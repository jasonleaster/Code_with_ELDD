/************************************************************
code writer :	EOF
code date   :	2014.08.26
e-mail	    :	jasonleaster@gmail.com
code file   :	rtc_time.c

code purpose:

	This is code is for RTC-driver in s3c6410-ARM.

#BUG-1! 2014.08.27  afternoon
	I am sorry about there is a bug waited to be fixed up.

	When we print out the "day-message", we can't get the 
right one.It always print out digital number '1'.I don't know
what happens...

	If you could fix it up, please touch me by e-mail.
Thank you.


@BUG-1 fixed up

	I mixed up with DATE and DAY in register.Now it work 
correctly.
	
	If you still find something wrong with my code, please 
touch me. Thank you.

************************************************************/

#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>

#include <linux/cdev.h>		/* for 'struct cdev'*/
#include <linux/kdev_t.h>	/* for 'MAJOR MINOR' */
#include <linux/fs.h>
#include <linux/kernel.h>	/* for 'printk()' */
#include <linux/slab.h>		/* for 'kmalloc()'*/
#include <linux/types.h>	/* for 'ssize_t'*/
#include <linux/proc_fs.h>
#include <asm/io.h>		/* for 'inl & outl'*/
#include <asm/uaccess.h>	/* for 'copy_to_user'*/
#include <linux/ioport.h>	/* for 'request_region()' 'ioremap()' */

#include <linux/device.h>	/* for 'struct class'*/

#include "EOF_rtc.h"


#define DEBUG

unsigned int rtc_major	=	RTC_MAJOR;
unsigned int rtc_minor	=	0;

unsigned long t_msg_addr[MESSAGE] = 
{
	RTC_SECOND_REG,
	RTC_MINUTE_REG,
	RTC_HOUR_REG,
	RTC_DATE_REG,
	RTC_MONTH_REG,
	RTC_YEAR_REG
};

module_param(rtc_major,int,S_IRUGO);
module_param(rtc_minor,int,S_IRUGO);

MODULE_AUTHOR("EOF");
MODULE_LICENSE("Dual BSD/GPL");

/* allocated in 'void rtc_init(void)' */
struct rtc_dev* rtc_devices;
struct class* rtc_class;

static int rtc_proc_show(char* buf,char** start,off_t offset,int count,int* eof,void* data)
{

#ifdef DEBUG
	printk(KERN_ALERT "calling rtc_proc_show...\n");
#endif
	char k_output[1024];
	
	struct rtc_dev* dev = rtc_devices;
	int device_num = 0;
	int length = 0;
	int temp = 0;
	int foo  = 0;

	if(mutex_lock_interruptible(&dev->mutex))
	{
		return -ERESTARTSYS;
	}

	for(device_num = 0;device_num < DEVICE_NUM;device_num++)
	{
		length += sprintf(k_output+length,"### rtc%d ###\n",device_num);

		for(temp = 0; temp < MESSAGE;temp++)
		{
			foo = readl(ioremap(t_msg_addr[temp],sizeof(int)));
			length += sprintf(k_output+length," temp: %d time: %d\n",temp, ((foo & 0xF0)>>4)*10 + (foo & 0x0F) );
		}	
	}

	printk(KERN_ALERT "%s\n",k_output);

	*eof = 1;
	mutex_unlock(&dev->mutex);

	return 0;
}


static void rtc_create_proc(void)
{
	struct proc_dir_entry * entry;

	entry	= create_proc_read_entry("rtc_reader",0,NULL,rtc_proc_show,NULL);

	if(!entry)
	{
		printk(KERN_ALERT "line:%d 'proc_create_read_entry()' failed!\n",__LINE__);
	}
}

static void rtc_remove_proc(void)
{
	remove_proc_entry("rtc_reader",NULL);
}

ssize_t rtc_read(struct file* filp,char __user* buf,const size_t count,loff_t* f_pos)
{
#ifdef DEBUG
	printk(KERN_ALERT "rtc_read ing... using ioremap...");
#endif
	struct rtc_dev* dev = filp->private_data;
	int temp  = 0;
	int ret	  = 0;

	if(mutex_lock_interruptible(&dev->mutex))
	{
		return -ERESTARTSYS;
	}
	else
	{
#ifdef DEBUG
		printk(KERN_ALERT "Lock in rtc_read!\n");
#endif
	}

	for(temp = 0; temp < MESSAGE;temp++)
	{
		/*
		** Hey,penguins...Attention! Do not use the physic address directly.
		** You shoult remap it into virtual addres by 'ioremap()'.
		*/
		//dev->buf[temp] = inl(t_msg_addr[temp]);
		dev->buf[temp] = readl(ioremap(t_msg_addr[temp],sizeof(int)));
#ifdef DEBUG
		printk(KERN_ALERT "dev->buf[%d] : %x\n",temp,dev->buf[temp]);
#endif
	}	

	if((ret = copy_to_user(buf,dev->buf,count)))
	{
		printk(KERN_ALERT "copy_to_user failed!\n");
		temp = temp - ret;
		goto out_read;
	}

out_read:
	mutex_unlock(&dev->mutex);
#ifdef DEBUG
		printk(KERN_ALERT "Lock released rtc_read!\n");
#endif

	return temp;
}

ssize_t rtc_write(struct file* filp,const char __user* buf,size_t count,loff_t* f_ops)
{
#ifdef DEBUG
	printk(KERN_ALERT "writing rtc ...\n");
#endif
	struct rtc_dev* dev = filp->private_data;
	int ret = 0;
	int temp = 0;
	
	if(mutex_lock_interruptible(&dev->mutex))
	{
		return -ERESTARTSYS;
	}

	if((ret = copy_from_user(dev->buf,buf,sizeof(*(dev->buf)) * MESSAGE)))
	{
		temp = temp - ret;
		goto out_write;
	}

	for(temp = 0; temp < MESSAGE;temp++)
	{
		outl(dev->buf[temp],(ioremap(t_msg_addr[temp],sizeof(int))) );
	}	


out_write:
	mutex_unlock(&dev->mutex);

	return 0;
}

int rtc_open(struct inode* inode,struct file* filp)
{
#ifdef DEBUG
	printk(KERN_ALERT "rtc_open ing...!\n");
#endif
	struct rtc_dev* dev;
	
	dev = container_of(inode->i_cdev,struct rtc_dev,cdev);

	filp->private_data = dev;

	return 0;
}

int rtc_release(struct inode * inode, struct file* filp)
{
#ifdef DEBUG
	printk(KERN_ALERT "rtc Released!\n");
#endif
	return 0;
}

struct file_operations rtc_fops =
{
	.owner	=	THIS_MODULE,
	.read	=	rtc_read,
	/*
	** It's dangerous and unnecessory to write something into RTC's register, the company which created your 'RTC' have finished 'write-work'.Generally, you shouldn't change it.
	** But I also implement a method that how to write message
	** into RTC's register.
	*/
	.write	=	rtc_write,
	.open	=	rtc_open,
	.release=	rtc_release,
};

static void rtc_setup_cdev(struct rtc_dev* dev,int indev)
{
	int err = 0;
	int dev_num = MKDEV(rtc_major,rtc_minor + indev);

	cdev_init(&dev->cdev,&rtc_fops);
	dev->cdev.owner	=	THIS_MODULE;
	dev->cdev.ops	=	&rtc_fops;
	err =  cdev_add(&dev->cdev,dev_num,1);

	if(err)
	{
		printk(KERN_ALERT "Error in adding rtc%d, err value:%d\n",indev,err);
	}
	else
	{
#ifdef DEBUG
		printk(KERN_ALERT "rtc_%d setted!\n",indev);
#endif
	}
}

void rtc_clean(void)
{
	int temp = 0;
	dev_t	dev_num = 0;

	dev_num = MKDEV(rtc_major,rtc_minor);

	if(rtc_devices)
	{
		for(temp = 0;temp > DEVICE_NUM;temp++)
		{
			cdev_del(&rtc_devices[temp].cdev);

			device_destroy(rtc_class,MKDEV(MAJOR(dev_num),temp));
#ifdef DEBUG
			printk(KERN_ALERT "rtc%d!\n",temp);
#endif
		}

		kfree(rtc_devices);
	}

	rtc_remove_proc();

	unregister_chrdev_region(dev_num,DEVICE_NUM);

	release_region(RTC_PORT_BASE,PORT_NUM);

}

int rtc_init(void)
{

	/* get our needed resource */
	if(!request_region(RTC_PORT_BASE,PORT_NUM,DEVICE_NAME))
	{
		printk(KERN_ALERT "rtc: can't request address:%p\n",(void*)RTC_PORT_BASE);

		return -ENODEV;
	}

	int ret = 0;
	int temp = 0;
	dev_t	dev_num = 0;

	ret = alloc_chrdev_region(&dev_num,rtc_minor,DEVICE_NUM,DEVICE_NAME);

	rtc_major = MAJOR(dev_num);

	rtc_class = class_create(THIS_MODULE,DEVICE_NAME);

	if(ret < 0)
	{
		printk(KERN_ALERT "rtc: can't get major %d\n",rtc_major);

		return ret;
	}

	/* allocate the device -- we can't have them static, as the number
	* can be specified at load time
 	*/	
	rtc_devices = kmalloc(DEVICE_NUM*sizeof(struct rtc_dev),GFP_KERNEL);

	if(!rtc_devices)
	{
		ret = -ENOMEM;
		goto fail;
	}

	memset(rtc_devices,0,DEVICE_NUM * sizeof(struct rtc_dev));

	/* Initialize the device */
	for(temp = 0;temp < DEVICE_NUM;temp++)
	{
		mutex_init(&rtc_devices[temp].mutex);
		rtc_setup_cdev(&rtc_devices[temp],temp);
	
		device_create(rtc_class,NULL,(dev_num + temp),NULL,"rtc_%d",temp);
	}

	rtc_create_proc();
	

#ifdef DEBUG
	printk(KERN_ALERT "rtc registed!\n");
#endif

	return 0;

fail:
	rtc_clean();
	return 0;
}

module_init(rtc_init);
module_exit(rtc_clean);
