/*********************************************************************
code writer : EOF
code date : 2014.08.16
code file : rtc_test.c
e-mail	  : jasonleaster@gmail.com

code purpose:
	This is a demo for user how to use RTC device driver.

	You should know that data store in RTC register as BCD-code.
If you want to represent it as deciminal, we could transform two-bits 
BCD-code into deciminal number by this way:

	Deciminal number = ((BCD-code&0xF0)>>4)*10+(BCD-code)&0x0F;

	If there is something wrong with my code, 
please touch me by e-mail. Thank you.

*********************************************************************/


#include <stdio.h>
#include <fcntl.h>

int main()
{
	int buf[10];

	char output[BUFSIZ] = {0,};

	int fd = 0;
	int counter = 0;

	if((fd = open("/dev/rtc_0",O_RDONLY)) < 0)
	{
		printf("open failed!\n");
		return 0;
	}

	if(read(fd,buf,sizeof(buf)) <= 0)
	{
		printf("read failed\n");
		goto failed;
	}

	counter += sprintf(output+counter,"Time now: ");

	counter += sprintf(output+counter,"Year: 20%d ", ((buf[5]&0xF0)>>4)*10 + (buf[5]&0x0F));
	counter += sprintf(output+counter,"month: %d ",  ((buf[4]&0xF0)>>4)*10 + (buf[4]&0x0F));
	counter += sprintf(output+counter,"day: %d ",    ((buf[3]&0xF0)>>4)*10 + (buf[3]&0x0F));
	counter += sprintf(output+counter,"hour: %d ",   ((buf[2]&0xF0)>>4)*10 + (buf[2]&0x0F));
	counter += sprintf(output+counter,"minute: %d ", ((buf[1]&0xF0)>>4)*10 + (buf[1]&0x0F));
	counter += sprintf(output+counter,"second: %d\n",((buf[0]&0xF0)>>4)*10 + (buf[0]&0x0F));
		
	printf("%s\n",output);


failed:
	close(fd);
	
	return 0;
}
