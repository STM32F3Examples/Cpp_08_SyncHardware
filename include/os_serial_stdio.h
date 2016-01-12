#ifndef _OS_SERIAL_STDIO_H
#define _OS_SERIAL_STDIO_H

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif
	
void os_serial_init(void);

void os_serial_printf(void (*p_puts)(const char *), const char * format, ...)
	__attribute__ ((format (printf, 2, 3)));

#ifdef __cplusplus
}
#endif

#endif// _OS_SERIAL_STDIO_H
