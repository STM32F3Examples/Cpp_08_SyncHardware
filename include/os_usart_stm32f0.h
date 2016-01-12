#ifndef _OS_USART_STM32F0_H
#define _OS_USART_STM32F0_H

#ifdef __cplusplus
extern "C" {
#endif

void os_usart1_init(int baudrate);
//void os_usart2_puts(const char* pData);
int os_usart1_gets(char *pBuffer);

#ifdef __cplusplus
}
#endif


#endif// _OS_USART_STM32F0_H
