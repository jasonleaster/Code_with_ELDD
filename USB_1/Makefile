ifneq	($(KERNELRELEASE),)

	obj-m	:=	usb_1.o
else

	KERNELDIR	?= /lib/modules/$(shell uname -r)/build
	PWD	:=	$(shell pwd)
	SUBDIR	:=	/usr/src/linux-headers-3.13.0-32-generic

modules:
	$(MAKE) -C $(SUBDIR) M=$(PWD)

clean:
	mv -f  *.order *.o  .*.cmd *.ko *.mod.c Module*   /home/jasonleaster/.trash/
	rm -rf .tmp_versions Modules.symvers

endif
