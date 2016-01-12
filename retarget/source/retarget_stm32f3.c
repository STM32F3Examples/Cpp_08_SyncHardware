#include "retarget_stm32f3.h"
#include "stm32f30x.h"                  // Device header

void USART2_init(int baudrate){
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	
	/*Enable peripheral clock for GPIOA*/
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA,ENABLE);
	/*Enable peripheral clock for UART2*/
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
	
	/*GPIOA Configuration PA2 as TX PA3 as RX*/
	GPIO_StructInit(&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_2|GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_UP;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	/*Connect USART2 pins to AF2*/
	//TX=PA2
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource2,GPIO_AF_7);
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource3,GPIO_AF_7);
	
	USART_StructInit(&USART_InitStructure);
	USART_InitStructure.USART_BaudRate=baudrate;
	USART_InitStructure.USART_WordLength=USART_WordLength_8b;
	USART_InitStructure.USART_StopBits=USART_StopBits_1;
	USART_InitStructure.USART_Parity=USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl=USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode=USART_Mode_Tx|USART_Mode_Rx;
	USART_Init(USART2,&USART_InitStructure);
	USART_Cmd(USART2,ENABLE);
	
	NVIC_EnableIRQ(USART2_IRQn);
}


void USART2_sendChar(char ch){
	while(!USART_GetFlagStatus(USART2,USART_FLAG_TXE));
	
	USART_SendData(USART2,ch);
}

void USART2_sendCharWithInterrupt(char ch){
	while(!USART_GetFlagStatus(USART2,USART_FLAG_TXE));
	USART_SendData(USART2,ch);
	USART_ITConfig(USART2, USART_IT_TXE, ENABLE);
}

void USART2_disableAndClean_it_tx(void){
		USART_ITConfig(USART2, USART_IT_TXE, DISABLE);
		USART_ClearITPendingBit(USART2,USART_IT_TXE);
}

void USART2_enable_it_tx(void){
		USART_ITConfig(USART2, USART_IT_TXE, ENABLE);
}

char USART2_getChar(void){
	if(USART_GetFlagStatus(USART2,USART_FLAG_ORE)) USART_ClearFlag(USART2,USART_FLAG_ORE); 
	
	while(!USART_GetFlagStatus(USART2,USART_FLAG_RXNE));
	return USART_ReceiveData(USART2);
}



void __attribute__((weak)) USART2_tx_callback(void){}

#ifdef __cplusplus
extern "C" {
#endif
void USART2_IRQHandler(void){
	if(USART_GetITStatus(USART2, USART_IT_TXE)){
		USART2_tx_callback();
	}
}
#ifdef __cplusplus
}
#endif


void usart1_init(int baudrate){
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	
	/*Enable peripheral clock for GPIOA*/
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA,ENABLE);
	/*Enable peripheral clock for UART1*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);
	
	/*GPIOA Configuration PA9 as TX PA10 as RX*/
	GPIO_StructInit(&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_9|GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_UP;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	/*Connect USART2 pins to AF7*/
	//PA9=USART1_TX
	//PA10=USART1_RX
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_7);
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_7);
	
	USART_StructInit(&USART_InitStructure);
	USART_InitStructure.USART_BaudRate=baudrate;
	USART_InitStructure.USART_WordLength=USART_WordLength_8b;
	USART_InitStructure.USART_StopBits=USART_StopBits_1;
	USART_InitStructure.USART_Parity=USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl=USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode=USART_Mode_Tx|USART_Mode_Rx;
	USART_Init(USART1,&USART_InitStructure);
	USART_Cmd(USART1,ENABLE);
}


void USART1_sendChar(char ch){
	while(!USART_GetFlagStatus(USART1,USART_FLAG_TXE));
	
	USART_SendData(USART1,ch);
}

char USART1_getChar(void){
	if(USART_GetFlagStatus(USART1,USART_FLAG_ORE)) USART_ClearFlag(USART1,USART_FLAG_ORE); 
	
	while(!USART_GetFlagStatus(USART1,USART_FLAG_RXNE));
	return USART_ReceiveData(USART1);
}

char* usart1_rx_buffer;
int usart1_rx_stringLength;
void (*usart1_rx_callback)(int);
volatile int usart1_rx_inProgress;

void usart1_async_gets(char* pString, void (*rx_complete_callback)(int)){
	//Prepare data
	usart1_rx_callback = rx_complete_callback;
	usart1_rx_buffer = pString;
	usart1_rx_stringLength = 0;
	usart1_rx_inProgress = 1;
	//Enable RX interrupt
	USART_ClearITPendingBit(USART1, USART_IT_RXNE);
	NVIC_EnableIRQ(USART1_IRQn);
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
}

static void rx_dummy_callback(int string_length){
}

int usart1_sync_gets(char* pString){
	usart1_async_gets(pString, rx_dummy_callback);
	while(usart1_rx_inProgress);
	return usart1_rx_stringLength;
}

#ifdef __cplusplus
extern "C" {
#endif

void USART1_IRQHandler(void){
	if(USART_GetITStatus(USART1,USART_IT_RXNE) == SET){
		char rx_data = USART_ReceiveData(USART1);
		if(usart1_rx_inProgress){
			if(rx_data == '\r' || rx_data == '\n'){
				usart1_rx_buffer[usart1_rx_stringLength] = '\0';
				usart1_rx_callback(usart1_rx_stringLength);
				usart1_rx_inProgress = 0;
				USART_ITConfig(USART1, USART_IT_RXNE, DISABLE);
			}else{
				usart1_rx_buffer[usart1_rx_stringLength] = rx_data;
				usart1_rx_stringLength++;
			}
		}
	}
}

#ifdef __cplusplus
}
#endif
