#ifndef DIBTYPES_H
#define DIBTYPES_H

unsigned int Get_systime(void);


//#define _ZORAN_SYSTEM_
#include <linux/types.h>
//#include <stdio.h>
#ifdef _ZORAN_SYSTEM_

#else

#include <linux/version.h>
#include <linux/string.h>
#include <linux/slab.h>
#include <linux/delay.h>


#define MemAlloc(size)       kmalloc(size, GFP_KERNEL)
#define	MemFree(chunk, size) kfree(chunk)

#define DibMemAlloc MemAlloc
#define DibMemFree  MemFree

#define DibMoveMemory memmove
#define DibZeroMemory(pointer, size) memset(pointer, 0, size)
#define DibSetMemory  memset
// Sleep
#define DibMSleep(v) msleep(v)
#define DibUSleep(v) msleep((v+999)/1000)

// JUST for zoran system
// please change the right systime to your system
#define systime Get_systime




#endif

typedef double REAL;

#if 0
typedef unsigned char uint8_t;
typedef char int8_t;

typedef unsigned int uint32_t;
typedef int int32_t;

typedef  unsigned short uint16_t;
typedef   short int16_t;
#endif

#define _DIBCOM0190_PMU_INCLUDE_    0
#define DIBCOM_DEBUG_INFO

typedef int  DIB_LOCK;

#define DibInitLock(lock)
#define DibFreeLock(lock)
#define DibAcquireLock(lock)
#define DibReleaseLock(lock)
#define DibAllocateLock(lock)
#define DibDeAllocateLock(lock)


typedef int DIB_EVENT;
#define DibAllocateEvent(event)
#define DibDeAllocateEvent(event)
#define DibInitNotificationEvent(event)
#define DibSetEvent(event)
#define DibResetEvent( a )
#define DibWaitForEvent(event, timeout)

#ifdef DIBCOM_TESTING
struct dibI2CAccess;
extern void debug_i2c_write(struct dibI2CAccess *msg);
#endif

#endif
