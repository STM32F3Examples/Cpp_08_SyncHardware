#ifndef _DMA_USART2_H
#define _DMA_USART2_H

#ifdef __cplusplus
extern "C" {
#endif

void dma_and_usart2_init(int baudrate);

void dma_channel_usart2_init(void);

void usart2_enable_dma(void);

void dma_usart2_irq_enable(void);

void dma_usart2_irq_disable(void);

void dma_usart2_puts(const char* pString);

void dma_usart2_nputs(const char* pString, int stringSize);

void dma_usart2_waitUntilComplete(void);

void dma_usart2_tx_callback(void);

#ifdef __cplusplus
}
#endif

#endif// _DMA_USART2_H
