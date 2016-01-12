#include "SyncSerial_stm32f3.h"
#include "retarget_stm32f3.h"
#include "cmsis_os.h"
#include "safe_stdlib.h"
#include "dma_usart2.h"

osMutexDef (usart2_tx_mutex);
osMutexId usart2_tx_mutex_id;


#define SIGNAL_USART2_TX 0x2
osThreadId usart2_thread_id;

int usart2_tx_remainingData = 0;
int usart2_tx_isSingleChar = 0;
const char* usart2_tx_dataPtr;

SyncSerialUSART2::SyncSerialUSART2(int baudrate){
	usart2_tx_mutex_id = osMutexCreate(osMutex(usart2_tx_mutex));
	dma_and_usart2_init(baudrate);
}

void SyncSerialUSART2::sendChar(char ch){
	osMutexWait(usart2_tx_mutex_id, osWaitForever);
	usart2_thread_id = osThreadGetId();
	usart2_tx_remainingData = 1;
	USART2_sendCharWithInterrupt(ch);
	osSignalWait(SIGNAL_USART2_TX, osWaitForever);
	osMutexRelease(usart2_tx_mutex_id);
}

char SyncSerialUSART2::getChar(void){
	return USART2_getChar();
}

void SyncSerialUSART2::puts(const char * pString){
	osMutexWait(usart2_tx_mutex_id, osWaitForever);
	usart2_thread_id = osThreadGetId();
	dma_usart2_puts(pString);
	osSignalWait(SIGNAL_USART2_TX, osWaitForever);
	osMutexRelease(usart2_tx_mutex_id);
}

void SyncSerialUSART2::printf(const char * format ,...){
	char * tempBuffer;
	int bufferSize;

	va_list args;
	va_start(args, format);

	bufferSize = vsnprintf(NULL, 0,format, args);
	bufferSize++;
	tempBuffer = (char*) safe_malloc(bufferSize);
	vsnprintf(tempBuffer,bufferSize,format,args);

	va_end(args);

	osMutexWait(usart2_tx_mutex_id, osWaitForever);
	usart2_thread_id = osThreadGetId();

	dma_usart2_nputs(tempBuffer,(bufferSize-1));

	osSignalWait(SIGNAL_USART2_TX, osWaitForever);
	osMutexRelease(usart2_tx_mutex_id);

	safe_free(tempBuffer);
}

/**
 * Exteral interrupt handler
 */
extern "C"
{
	void USART2_tx_callback(void){
		usart2_tx_remainingData--;
		if(usart2_tx_remainingData == 0){
			USART2_disableAndClean_it_tx();
			osSignalSet(usart2_thread_id, SIGNAL_USART2_TX);
		}
	}
}

/**
 * Exteral interrupt handler
 */
extern "C"
{
	void dma_usart2_tx_callback(void){
		osSignalSet(usart2_thread_id, SIGNAL_USART2_TX);
	}
}
