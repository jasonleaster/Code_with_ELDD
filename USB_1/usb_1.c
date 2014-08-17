/**************************************************************
code writer 	: EOF
code date	: 2014.08.18
code file	: ./USB_1/usb_1.c
e-mail		: jasonleaster@gmail.com

code purpose:
	This is a demo for how to code for a USB device driver.

	If there is something wrong with my code, please touch 
me by e-mail. Thank you. 	

*************************************************************/
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/usb.h>
#include <linux/module.h>
#include <linux/moduleparam.h>

#define VENDOR_ID	0x08f7
#define PRODUCT_ID	0x0002
#define DEVICE_NAME	"usb_1"

MODULE_AUTHOR("EOF");
MODULE_LICENSE("Dual BSD/GPL");

static struct usb_device_id id_table[] = 
{
	{ USB_DEVICE(VENDOR_ID, PRODUCT_ID)},
	{ },
};

MODULE_DEVICE_TABLE(usb,id_table);

static int usb_probe(struct usb_interface * interface,
		     const struct usb_device_id *id)
{
	dev_info(&interface->dev,"USB device now attached\n");

	return 0;
}

static void usb_disconnect(struct usb_interface* interface)
{
	dev_info(&interface->dev,"USB device now disconnect\n");
}

static struct usb_driver usb_driver = 
{
	.name	=	DEVICE_NAME,
	.probe	=	usb_probe,
	.disconnect =	usb_disconnect,
	.id_table   =	id_table,
};

static int usb_init(void)
{
	return usb_register(&usb_driver);
}

static void usb_exit(void)
{
	usb_deregister(&usb_driver);
}

module_init(usb_init);
module_exit(usb_exit);
