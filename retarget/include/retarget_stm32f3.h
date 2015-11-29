#ifndef _RETARGET_STM32F3_H
#define _RETARGET_STM32F3_H

#ifdef __cplusplus
extern "C" {
#endif
void USART1_init(int baudrate);

void USART1_sendChar(char ch);

char USART1_getChar(void);


void USART2_init(int baudrate);

void USART2_sendChar(char ch);

char USART2_getChar(void);

void USART2_sendCharWithInterrupt(char ch);

void USART2_tx_callback(void);

void USART2_disableAndClean_it_tx(void);

void USART2_enable_it_tx(void);

#ifdef __cplusplus
}
#endif

#endif

