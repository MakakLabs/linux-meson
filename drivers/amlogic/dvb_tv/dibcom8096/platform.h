#ifndef PLATFORM_PLATFORM_H
#define PLATFORM_PLATFORM_H

#include "config.h"

  #include "platform_dibcom.h"
/* define a INFO_TEXT in your platform.h and get rid of the strings */
#ifndef INFO_TEXT
#define INFO_TEXT(x) x
#endif


#ifdef __cplusplus
extern "C" {
#endif

#if defined(__BYTE_ORDER)
#if __BYTE_ORDER == __LITTLE_ENDIAN
#define from_big_endian16(v) bswap_16(v)
#define from_big_endian32(v) bswap_32(v)
#define from_little_endian16(v) (v)
#define from_little_endian32(v) (v)
#else
#define from_big_endian16(v) (v)
#define from_big_endian32(v) (v)
#define from_little_endian16(v) bswap_16(v)
#define from_little_endian32(v) bswap_32(v)
#endif
#else
#if 0
static inline uint16_t from_big_endian16(uint16_t v)
{
	uint16_t w=0x1;
	/* little endian */
	if (*(uint8_t *)&w == 1)
		return (((v>>8)&0xff) | ((v&0xff)<<8));
	/* big endian */
	return v;
}

static inline uint32_t from_big_endian32(uint32_t v)
{
	uint16_t w=0x1;
	/* little endian */
	if (*(uint8_t *)&w == 1)
		return ((v&0xff000000)>>24) | ((v&0x00ff0000)>>8) | ((v&0x0000ff00)<<8) | ((v&0x000000ff)<<24);
	/* big endian */
	return v;
}
#endif
#endif



#ifdef __cplusplus
}
#endif


#endif
