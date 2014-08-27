/**************************************************
code writer :	EOF
code date   :	2014.08.26
e-mail	    :	jasonleaster@gmail.com

code purpose:
	This header file is used for ...

	It's convenient to use this MACRO but not
address number directly.


**************************************************/
#ifndef _EOF_RTC_H
#define _EOF_RTC_H

#include <mach/map.h> /* for 'S3C64XX_PA_RTC' */

#define RTC_CON		(S3C64XX_PA_RTC + 0x40)

#define RTC_SECOND_REG	(S3C64XX_PA_RTC + 0x70)
#define RTC_MINUTE_REG	(S3C64XX_PA_RTC + 0x74)
#define RTC_HOUR_REG	(S3C64XX_PA_RTC + 0x78)
#define RTC_DATE_REG	(S3C64XX_PA_RTC + 0x7C)
#define RTC_DAY_REG	(S3C64XX_PA_RTC + 0x80)
#define RTC_MONTH_REG	(S3C64XX_PA_RTC + 0x84)
#define RTC_YEAR_REG	(S3C64XX_PA_RTC + 0x88)

/*
** second minute hour day month and year.
** There are six messages waited to be read.
*/
#define MESSAGE		6

#define DEVICE_NAME	"rtc_time"
/*
**	create four device for fun, demo for one driver but drive different devices 
*/
#define DEVICE_NUM	1
#define PORT_NUM	36
#define RTC_PORT_BASE	S3C64XX_PA_RTC

#define RTC_MAJOR	0 /*by dynamical */

/*
**	It's fantastic to use a structure to abstract a device!
*/
struct rtc_dev
{
	struct cdev cdev;
	struct mutex mutex;

	/*
	** used for storing time message 
	*/
	int buf[1024];
};

#endif
