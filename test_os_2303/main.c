/*----------------------------------------------------------------------------
 * CMSIS-RTOS 'main' function template
 *---------------------------------------------------------------------------*/

#define osObjectsPublic                     // define objects in main module
#include "osObjects.h"                      // RTOS object definitions
#include "LPC17xx.h"                    // Device header
#include "GLCD_Config.h"                // Keil.MCB1700::Board Support:Graphic LCD
#include "Board_GLCD.h"                 // ::Board Support:Graphic LCD
#include "RTE_Device.h"                 // Keil::Device:Startup
#include "cmsis_os.h"                   // ARM::CMSIS:RTOS:Keil RTX
#include "CAN_CNA.h"       
#include "math.h"
#include "GPIO_LPC17xx.h"               // Keil::Device:GPIO
#include "stdio.h"

extern GLCD_FONT GLCD_Font_6x8;
extern GLCD_FONT GLCD_Font_16x24;

void Conversion_DA(int sortie);


void Thread1 (void const *argument);                             // thread function
osThreadId t1;                                          // thread id
osThreadDef (Thread1, osPriorityHigh, 1, 0);                   // thread object

void Thread2 (void const *argument);                             // thread function
osThreadId t2;                                          // thread id
osThreadDef (Thread2, osPriorityNormal, 1, 0);                   // thread object

int n=0;
/*
 * main: initialize and start the system
 */

int main (void) {
  osKernelInitialize ();                    // initialize CMSIS-RTOS
  t1 = osThreadCreate (osThread(Thread1), NULL);
	t2 = osThreadCreate (osThread(Thread2), NULL);
	GLCD_Initialize();
	GLCD_ClearScreen();
	GLCD_SetFont(&GLCD_Font_16x24);
  // initialize peripherals here

  // create 'thread' functions that start executing,
  // example: tid_name = osThreadCreate (osThread(name), NULL);
  ADC_Init();	
	DAC_Init();
	
	LPC_SC->PCONP |= (1<<1); //allume le timer 0 (facultatif, déjà allumé après un reset)

LPC_TIM0->PR =  0;
LPC_TIM0->MR0 = 1249; // 44.1 kHz

LPC_TIM0->MCR = 3;		/*reset compteur si MR0=COUNTER + interruption*/

LPC_TIM0->TCR = 1; 		//démarre le comptage

	NVIC_SetPriority(TIMER0_IRQn,31);
	NVIC_EnableIRQ(TIMER0_IRQn);
	
	
  osKernelStart ();                         // start thread execution 
}

void TIMER0_IRQHandler(void)
{
		LPC_TIM0->IR |= 1<<0;   // drapeau IT Timer0
	osSignalSet (t1, 0x01);
	
	osSignalSet (t2, 0x02);
	//osDelay(1000);
	
	
}

void Thread1 (void const *argument) 
{
	
	int en,i=1000;
	char tab[50];
  while (1) 
	{
	osSignalWait(0x01, osWaitForever);
	for(n=0;n<5000;n++)
		{
	en = 2048*sin(n*0.13823);

	Conversion_DA(en);

		}
		osDelay(i);
		sprintf(tab,"Blabla %04d",i);	// du blabla
		GLCD_DrawString(100,100,tab);
		

	 if(i<=1000 & i>100)
	 {
		i=i-100;
	 }
	 else if (i<=100 & i>0)
	 {
		i=i-10;
	 }
	 else if (i==0)
	 {
		for(n=0;n<10000;n++)
		{
		en = 2048*sin(n*0.13823);
		Conversion_DA(en);
	  }
   }
  }
}

void Thread2 (void const *argument) 
{
  while (1) 
	{
	osSignalWait(0x02, osWaitForever);
  Conversion_DA(-2048);
  }
}

