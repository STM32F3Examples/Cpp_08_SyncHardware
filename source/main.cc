#include "cmsis_os.h"                   // ARM::CMSIS:RTOS:Keil RTX
#include "SyncSerial_stm32f3.h"
#include "stm32f30x.h"                  // Device header
#include "MainApp.h"
#include "safe_stdlib.h"

void tarea1(void const * arguments); //tarea 1
osThreadId  tarea1ID;	//identificador del hilo tarea 1
osThreadDef (tarea1,osPriorityNormal,1,0);// macro para definir tareas (aputandor de la funcion, prioridad,?,?)

void tarea2(void const * arguments); //tarea 2
osThreadId  tarea2ID;	//identificador del hilo  tarea 2
osThreadDef (tarea2,osPriorityNormal,1,0);// macro para definir tareas (aputandor de la funcion, prioridad,?,?)

void tarea3(void const * arguments); //tarea 3
osThreadId  tarea3ID;	//identificador del hilo  tarea 3
osThreadDef (tarea3,osPriorityNormal,1,0);// macro para definir tareas (aputandor de la funcion, prioridad,?,?)


void tarea1Init(void);//funcion que iniciliza la tarea1
void tarea2Init(void);//funcion que iniciliza la tarea2
void tarea3Init(void);//funcion que iniciliza la tarea3
void button_mutex_init(void);// initializes button mutex

void osInitiAll(void);

void led_init(void);
void button_init(void);
void waitButtonPress(void);

SerialStream* serial;

#define SIGNAL_BUTTON 0x1
osThreadId button_thread_id;
osMutexDef (button_mutex);
osMutexId button_mutex_id;


int main(){
	osKernelInitialize();
	//Hardware initialization
	serial = new SyncSerialUSART2(9600);
	//Operating System initialization
	osInitiAll();
	serial->printf("\nSystem ready\n");
	//User application
	MainApp::main(serial);
}

void osInitiAll(void){
	safe_init();
	tarea1Init();
	tarea2Init();
	tarea3Init();
	button_mutex_init();
	osKernelStart();
}

void button_mutex_init(void){
	button_mutex_id = osMutexCreate(osMutex(button_mutex));
}

void tarea1Init(void){
	tarea1ID= osThreadCreate(osThread(tarea1),NULL);
}

void tarea2Init(void){
	tarea2ID= osThreadCreate(osThread(tarea2),NULL);
}

void tarea3Init(void){
	tarea3ID= osThreadCreate(osThread(tarea3),NULL);
}

void tarea1(void const * arguments){
	while(1){
		serial->printf("Thread: tarea1, Valve on\n");
		osDelay(10);
		serial->printf("Thread: tarea1, Valve off\n");
		osDelay(10);
	}
}

int task2Counter = 0;
int resetCounter = 0;
void tarea2(void const * arguments){
	while(1){
		if(resetCounter){
			task2Counter = 0;
			resetCounter = 0;
		}else{
			task2Counter++;
		}
	}
}

void tarea3(void const * arguments){
	while(1){
		osDelay(1000);
		serial->printf("****\nThread: tarea3, counter = %d\n****\n",task2Counter);
		resetCounter = 1;
	}
}

void led_init(void){
	//Turn on the GPIOB peripherial
	RCC->AHBENR |= RCC_AHBENR_GPIOBEN;
	//Configure PB13  as push pull ouput an set the output to high
	GPIOB->MODER &=~(GPIO_MODER_MODER13);
	GPIOB->MODER |=GPIO_MODER_MODER13_0;//output
	GPIOB->ODR |= GPIO_ODR_13;
}

void button_init(void){
	//needed for interrupt source remaping
	RCC->AHBENR |= RCC_AHBENR_GPIOCEN;

	//Configure PA13 as input with pull-up
	GPIOC->MODER &=~(GPIO_MODER_MODER13);
	GPIOC->PUPDR |= GPIO_PUPDR_PUPDR13_0;

	//configure interrupt
	RCC->APB2ENR|= RCC_APB2ENR_SYSCFGEN;//Enable sysconfig registers
	SYSCFG->EXTICR[3] |=SYSCFG_EXTICR4_EXTI13_PC;
	EXTI->IMR |= EXTI_IMR_MR13;
	EXTI->FTSR |= EXTI_FTSR_TR13;
	NVIC_EnableIRQ(EXTI15_10_IRQn);
}

void waitButtonPress(void){
	button_thread_id = osThreadGetId();
	osMutexWait(button_mutex_id, osWaitForever);
	osSignalClear(button_thread_id, SIGNAL_BUTTON);
	osSignalWait(SIGNAL_BUTTON,osWaitForever);
	osMutexRelease(button_mutex_id);
}

/**
 * Exteral interrupt handler
 */
extern "C"
{
	void EXTI15_10_IRQHandler(void){
		EXTI->PR = EXTI_PR_PR13;
		osSignalSet(button_thread_id, SIGNAL_BUTTON);
	}
}
