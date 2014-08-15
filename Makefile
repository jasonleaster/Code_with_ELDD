ifneq	($(KERNELRELEASE),)
	
	obj-m	:= cmos_demo.o
else
	
	KERNELDIR ?= /lib/modules/$(shell uname -r)/build
	PWD	:= /$(shell pwd)
	SUBDIR  := /usr/src/linux-headers-3.13.0-32-generic

modules:
	$(MAKE) -C $(SUBDIR) M=$(PWD) modules

clean:
	rm *.o *.ko *.order *.symvers *.mod.c
endif
