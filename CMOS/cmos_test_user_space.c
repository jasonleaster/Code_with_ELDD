/*********************************************************************
code writer : EOF
code date : 2014.08.16
code file : cmos_test_user_space.c
e-mail	  : jasonleaster@gmail.com

code purpose:
	This is a demo for user how to use CMOS device driver.
Unfortunately, there are some bugs I can't fix it up.

#BUG1--2014.08.16:

	You know that data store in CMOS register as BCD-code.If you want
to represent it as deciminal, we could transform two-bits BCD-code into
deciminal number by this way:

	Deciminal number = ((BCD-code&0xF0)>>4)*10+(BCD-code)&0x0F;

But there is something strange, it does work in 'second' which in my
code is cmos[0].

	If you have someidea to fix it up, please touch me. Thank you.


#BUG2--2014.08.16:
	
	The day value is smaller than the real time by one day.
Eg:
	Today is 08.16, but it print out 'day:15'.God know it...

*********************************************************************/
#include <stdio.h>
#include <fcntl.h>

#define BUFSIZE 100
#define CMOS_TO_BE_READ 80

int main()
{
	int fd = 0;
	int counter = 0;
	unsigned char cmos[CMOS_TO_BE_READ];
	unsigned char buf[BUFSIZE];

	if((fd = open("/dev/cmos0",O_RDONLY)) < 0)
	{
		printf("fopen failed!\n");
		return 0;
	}

	if(read(fd,cmos,CMOS_TO_BE_READ) != CMOS_TO_BE_READ)
	{
		printf("read error\n");
		return 0;
	}

	counter += sprintf(buf+counter,"Time now: ");
	counter += sprintf(buf+counter,"Year: 20%d ",((cmos[9]&0xF0)>>4)*10 + (cmos[9]&0x0F));
	counter += sprintf(buf+counter,"month: %d ",((cmos[8]&0xF0)>>4)*10 + (cmos[8]&0x0F));
	counter += sprintf(buf+counter,"day: %d ",((cmos[7]&0xF0)>>4)*10 + (cmos[7]&0x0F));
	counter += sprintf(buf+counter,"hour: %d ",((cmos[4]&0xF0)>>4)*10 + (cmos[4]&0x0F));
	counter += sprintf(buf+counter,"minute: %d ",((cmos[2]&0xF0)>>4)*10 + (cmos[2]&0x0F));
	counter += sprintf(buf+counter,"second: %d \n",((cmos[0]&0xF0)>>4)*10 + (cmos[0]&0x0F));
		
	printf("%s",buf);

	close(fd);

	return 0;
}
