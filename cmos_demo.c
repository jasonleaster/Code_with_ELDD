/************************************************************
code writer : EOF
code date : 2014.08.15
code file : cmos_demo.c
e-mail:	jasonleaster@gmail.com

code purpose:
	This code is a demo for how to build a CMOS-driver
step by step.

!BUG:
	I don't know why I can't 'rmmod ./cmos_demo.ko' totally.
I can't give me a explaination about why can not unload this module.

	Port 0x70-0x74 can not be release ?

	If you have idea with my bug, please touch me by e-mail.
Thank you all the time.

************************************************************/
#include <linux/module.h>
#include <linux/moduleparam.h>

#include <linux/fs.h>
#include <linux/init.h> 
#include <linux/cdev.h>  
#include <linux/slab.h> /* kmalloc() */
#include <linux/kernel.h> /* printk */
#include <linux/device.h>
#include <linux/ioport.h>
#include <linux/kdev_t.h>
#include <linux/types.h>
#include <linux/ioport.h>

#include <asm/uaccess.h>

int cmos_major = 0;//defualt
int cmos_minor = 0;

module_param(cmos_major,int,S_IRUGO);
module_param(cmos_minor,int,S_IRUGO);

//static dev_t	cmos_dev_number;	/* Allocated device number */
struct class	*cmos_class;	/* Tie with the device model */

#define NUM_CMOS_BANKS	2	
#define CMOS_BANK_SIZE	(0xFF*8)
#define DEVICE_NAME	"Jason_cmos"

#define CMOS_BANK0_INDEX_PORT	0x70
#define CMOS_BANK0_DATA_PORT	0x71
#define CMOS_BANK1_INDEX_PORT	0x72
#define CMOS_BANK1_DATA_PORT	0x73

unsigned char addrports[NUM_CMOS_BANKS] = {CMOS_BANK0_INDEX_PORT,CMOS_BANK1_INDEX_PORT};
unsigned char dataports[NUM_CMOS_BANKS] = {CMOS_BANK0_DATA_PORT,CMOS_BANK1_DATA_PORT};

MODULE_AUTHOR("Jason Leaster");
MODULE_LICENSE("Dual BSD/GPL");

unsigned char port_data_in(unsigned char offset,int bank)
{
	unsigned char data;
	
	if(unlikely(bank >= NUM_CMOS_BANKS))
	{
		printk("Unknow CMOS Bank\n");
		return 0;
	}
	else
	{
		outb(offset,addrports[bank]);
		data = inb(dataports[bank]);
	}

	return data;
}

void port_data_out(unsigned char offset,unsigned char data,int bank)
{
	if(unlikely(bank >= NUM_CMOS_BANKS))
	{
		printk("Unknow CMOS BANK\n");
		return ;
	}
	else
	{
		outb(offset,addrports[bank]);
		outb(data,dataports[bank]);
	}

	return ;
}

struct cmos_dev
{
	unsigned short current_pointer; /*Current point within the bank*/
	
	unsigned int size;	/*size of the bank*/
	int bank_number;	/*Size of bank*/
	struct cdev cdev;
	char name[10];		/*Name of I/O regeion*/
	
	/* ... */
	
}*cmos_devp;


int cmos_open(struct inode *inode,struct file* file)
{
	struct cmos_dev* cmos_devp;
	
	cmos_devp = container_of(inode->i_cdev,struct cmos_dev,cdev);
	
	file->private_data = cmos_devp;
	
	cmos_devp->size = CMOS_BANK_SIZE;
	
	/* Initialize pointer -- current_pointer */
	cmos_devp->current_pointer = 0;

	return 0;
}

int cmos_release(struct inode* inode,struct file* file)
{
	struct cmos_dev *cmos_devp = file->private_data;

	cmos_devp->current_pointer = 0;

	return 0;
}

ssize_t cmos_read(struct file *file,char* buf,size_t count,loff_t *ppos)
{
	struct cmos_dev *cmos_devp = file->private_data;

	char data[CMOS_BANK_SIZE];
	unsigned char mask;
	
	int xferred = 0,i =0,l,zero_out;

	/*
	** if we run 'read' first time, 'start_byte' must be 0 
	*/
	int start_byte = cmos_devp->current_pointer/8;
	int start_bit = cmos_devp->current_pointer%8;

	if(cmos_devp->current_pointer >= cmos_devp->size)
	{
		return 0;
	}

	/* 
	** 	If 'count' is bigger than the max size of the space left,
	** you can't read data beyond 'cmos_devp->size', so assign the 
	** count by value of the space left.
	*/

	if(cmos_devp->current_pointer + count > cmos_devp->size)
	{
		count = cmos_devp->size - cmos_devp->current_pointer;
	}

	while( xferred < count)
	{
		data[i] = port_data_in(start_byte,cmos_devp->bank_number) >> start_bit;

		xferred += (8-start_bit);

		if((start_bit) && (count + start_bit > 8))
		{
			data[i] |= (port_data_in(start_byte+1,cmos_devp->bank_number) << (8-start_bit));
			xferred += start_bit;
		}

		start_byte++;

		i++;
	}

	if(xferred > count)
	{
		zero_out = xferred - count;
		mask = 1 << (8-zero_out);

		for(l = 0; l < zero_out; l++)
		{
			data[i-l] &= ~mask;
			mask <<= 1;
		}

		xferred = count;
	}

	if(!xferred)
	{
		return -EIO;
	}

	if(copy_to_user(buf,(void*)data,((xferred/8)+1)) != 0)
	{
		return -EIO;
	}

	cmos_devp->current_pointer += xferred;

	return xferred;
}

ssize_t cmos_write(struct file * file,const char *buf,size_t count,loff_t *ppos)
{
	struct cmos_dev* cmos_devp = file->private_data;
	
	int xferred = 0,i = 0,l ,end_l,start_l;
	char *kbuf,tmp_kbuf;
	unsigned char tmp_data = 0,mask;

	int start_byte = cmos_devp->current_pointer/8;
	int start_bit  = cmos_devp->current_pointer%8;

	if(cmos_devp->current_pointer >= cmos_devp->size)
	{
		return 0;
	} 

	if(cmos_devp->current_pointer + count > cmos_devp->size)
	{
		count = cmos_devp->size - cmos_devp->current_pointer;
	}

	kbuf = kmalloc((count/8)+1,GFP_KERNEL);
	
	if(kbuf == NULL)
	{
		return -ENOMEM;
	}
	
	if(copy_from_user(kbuf,buf,(count/8)+1))
	{
		kfree(kbuf);
		return -EFAULT;
	}

	while(xferred < count)
	{
		tmp_data = port_data_in(start_byte,cmos_devp->bank_number);
	
		mask = 1<<start_bit;
		end_l = 8;
	
		if((count - xferred) < (8-start_bit))
		{
			end_l = (count - xferred) + start_bit;
		}

		for(l = start_bit; l < end_l;l++)
		{
			tmp_data &= ~mask;
			mask <<= 1;
		}

		tmp_kbuf = kbuf[i];
		mask = 1 << end_l;

		for(l = end_l; l < 8; l++)
		{
			tmp_kbuf &= ~mask;
			mask <<=1;
		}

		port_data_out(start_byte,tmp_data | (tmp_kbuf << start_bit),cmos_devp->bank_number);
		
		xferred += (end_l - start_bit);

		if((xferred < count) && (start_bit) && (count + start_bit > 8))
		{
			tmp_data = port_data_in(start_byte+1,cmos_devp->bank_number);
			start_l = ((start_bit + count) % 8);
			mask = l << start_l;
			for(l = 0;l < start_l;l++)
			{
				mask >>= 1;
				tmp_data &= ~mask;
			}

			port_data_out((start_byte+1),tmp_data | (kbuf[i] >> (8 - start_bit)), cmos_devp->bank_number);

			xferred += start_l;
		}

		start_byte++;
		
		i++;
	}

	if(!xferred)
	{
		return -EIO;
	}

	cmos_devp->current_pointer += xferred;

	return xferred;
}



static loff_t cmos_llseek(struct file* file,loff_t offset,int orig)
{
	struct cmos_dev * cmos_devp = file->private_data;

	switch(orig)
	{
		case 0:
			if(offset >= cmos_devp->size)
			{
				return -EINVAL;
			}
			
			cmos_devp->current_pointer = offset;
			break;

		case 1:
			if((cmos_devp->current_pointer + offset) >= cmos_devp->size)
			{
				return -EINVAL;
			}

			cmos_devp->current_pointer = offset;
			break;

		case 2:
			return -EINVAL;

		default:
			return -EINVAL;		
	}

	return (cmos_devp->current_pointer);
}



static struct file_operations cmos_fops = 
{
	.owner	=	THIS_MODULE,
	.open	=	cmos_open,
	.release=	cmos_release,
	.read	=	cmos_read,
	.write	=	cmos_write,
	.llseek	=	cmos_llseek,

	/* wait for implementation */
//	.ioctl	=	cmos_ioctl,
};



int  cmos_init(void)
{
	int i;

	dev_t devno = 0;
	
	if(alloc_chrdev_region(&devno,cmos_minor,NUM_CMOS_BANKS,DEVICE_NAME) < 0)
	{
		printk(KERN_DEBUG "Can't regiester device\n");
		return -1;
	}

	cmos_major = MAJOR(devno);
	
	/*********I don't know what this is************/
	cmos_class = class_create(THIS_MODULE,DEVICE_NAME);
	
	release_region(0x70,0x8);

	for(i = 0;i < NUM_CMOS_BANKS;i++)
	{
		cmos_devp = kmalloc(sizeof(struct cmos_dev),GFP_KERNEL);
		if(!cmos_devp)
		{
			printk("Bad Kmalloc\n");
			return 1;
		}

		sprintf(cmos_devp->name,"cmos%d",i);
	
		/* 
		** request for two port one for address and the other for data
		*/
		if(!(request_region(addrports[i],2,cmos_devp->name)))
		{
			printk("cmos: I/O port 0x%x is not free.\n",addrports[i]);

			return -EIO;
		}

		cmos_devp->bank_number = i;

		/* 
		** initialization for character device by function cdev_init
		*/
		cdev_init(&cmos_devp->cdev,&cmos_fops);
		cmos_devp->cdev.owner = THIS_MODULE;

		if(cdev_add(&cmos_devp->cdev,(devno + i),1))
		{
			printk("Bad cdev\n");

			return 1;
		}

		device_create(cmos_class,NULL,(devno + i),NULL,"cmos%d",i);
		
	}

	printk("CMOS Driver Initialized.\n");

	return 0;
}

void cmos_cleanup(void)
{
	int i;
	
	dev_t  devno = MKDEV(cmos_major,cmos_minor);
	
	cdev_del(&cmos_devp->cdev);
	
	unregister_chrdev_region(devno,NUM_CMOS_BANKS);
	
	for(i = 0;i < NUM_CMOS_BANKS;i++)
	{
		device_destroy(cmos_class,MKDEV(MAJOR(devno),i));
		
		release_region(addrports[i],2);
	}

	class_destroy(cmos_class);
	
	return ;
}

module_init(cmos_init);
module_exit(cmos_cleanup);
