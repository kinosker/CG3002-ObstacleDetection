/*
 * myHcSonar.c
 *
 * Created: 9/27/2015 2:47:27 AM
 *  Author: tienlong
 */ 
#include <myHcSonar.h>

volatile int ms_tickStart;
volatile int ms_tickLapsed;
volatile int ms_tickEnd;

volatile int us_tickEnd;
volatile int us_tickStart;
volatile int us_tickLapsed;
SemaphoreHandle_t semaReadReady;


ISR(PCINT0_vect) //Digital pin 50
{
		
	if(HC_Echo_Read)
	{
		us_tickStart = myTimer_Read();
		ms_tickStart = xTaskGetTickCountFromISR();
	}	
	else if (!(HC_Echo_Read))
	{

		if ((ms_tickEnd = xTaskGetTickCountFromISR()) < ms_tickStart)
			ms_tickEnd += 65535;
		
		if ((us_tickEnd = myTimer_Read()) < us_tickStart) // if smaller then add...
			us_tickEnd += MAX_TICKS;
		
			
			
		us_tickLapsed = us_tickEnd - us_tickStart;
		ms_tickLapsed = ms_tickEnd - ms_tickStart;
		
		xSemaphoreGiveFromISR(semaReadReady, pdFALSE);
	}	
}

void myHcSonar_Init()
{
	HC_Trigger_Init(); // set trigger as output
	HC_Echo_Init(); // set echo as input
		
	PCICR |= (1<<PCIE0); // enable control for PCINT 7:0
	PCMSK0 |= (1<<PCINT3); // set interrupt for PCINT3
	
	semaReadReady = xSemaphoreCreateBinary();
}

void myHcSonar_Start()
{
	HcSonar_TriggerStart();
	
	myTimer_DelayMicro2(30); // delay 20 micro seconds for the pulse
	
	HcSonar_TriggerStop();

}

int myHcSonar_Read()
{
	int usLength, msLength;
	
	myHcSonar_Start();	
	xSemaphoreTake(semaReadReady, portMAX_DELAY);
	
	usLength = us_tickLapsed / 14;
	msLength = ms_tickLapsed * 17;
	return usLength + msLength;
	
}