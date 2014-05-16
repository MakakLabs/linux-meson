#include "debug.h"
#include "i2c.h"
#include <stdarg.h>


unsigned int Get_systime(void)
{
	struct timeval tv;
	
	do_gettimeofday(&tv);

	 return (tv.tv_sec * 10000) + (tv.tv_usec / 100);
}

void debug_printf_line(const struct dibDebugObject *dbg,  const char *fmt, ...)
{
#if 1
    char buf[256];
    va_list args;

    if (dbg)
    {
        #ifdef DIBCOM_DEBUG_INFO
        printk("%-12s: ", dbg->prefix);
        #endif
    }
    else
    {
        #ifdef DIBCOM_DEBUG_INFO
        printk("%-12s: ", "DBG");
        #endif
    }

    va_start(args,fmt);
    vsprintf(buf,fmt,args);
    va_end(args);

#ifdef DIBCOM_DEBUG_INFO
    printk(buf);
    printk("\n");
#endif
#endif
}

void debug_printf(const char *fmt, ...)
{
#if 1
	char buf[256];
    va_list args;

    va_start(args,fmt);
    vsprintf(buf,fmt,args);
    va_end(args);

#ifdef DIBCOM_DEBUG_INFO
    printk(buf);
#endif
#endif
}

#ifdef DIBCOM_TESTING

void debug_dump_i2c(struct dibI2CAccess *msg)
{
#if 0
	int i;
#ifdef DIBCOM_DEBUG_INFO
	printf("%c %d ", msg->rx == NULL ? 'w' : 'r', msg->addr);
#endif
	for (i = 0; i < msg->txlen; i++)
	{
#ifdef DIBCOM_DEBUG_INFO
		printf("0x%02x ", msg->tx[i]);
#endif
	}
#ifdef DIBCOM_DEBUG_INFO
	printf("\n");
#endif
#endif
}

#endif
