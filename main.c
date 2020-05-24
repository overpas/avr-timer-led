#include <stdlib.h>
#include <string.h>
#include "FreeRTOS.h"
#include "task.h"
#include <stdbool.h>

#define S1 PC5
#define S2 PC4

void changeSymbolTask();
void displayTask();
void pollButtonsTask();
void showM1(int pos);
void showM2(int pos);

char m2d[] = {
	0b10000000,
	0b01000000,
	0b00100000,
	0b00010000,
	0b00000010,
	0b00000001
};

char m2b[] = {
	0b00000001,
	0b00000010,
	0b00000100,
	0b00001000,
	0b00010000,
	0b00100000
};

char m1d[] = {
	0b11111110,
	0b11111101,
	0b11101111,
	0b11011111,
	0b10111111,
	0b01111111,
	0b11111111,
	0b11111111,
	0b11111111,
	0b11111111,
	0b11111111,
	0b11111111,
	0b11111111
};

char m1b[] = {
	0b00111111,
	0b00111111,
	0b00111111,
	0b00111111,
	0b00111111,
	0b00111111,
	0b00111110,
	0b00111101,
	0b00111011,
	0b00110111,
	0b00101111,
	0b00011111,
	0b00111111
};

xTaskHandle displayTaskHandle = NULL;

int currentSymbolIndex = 0;
int pB = 0, pD = 0;

int display_interval = 150;

char pushedButton = 0;


void initialize()
{
	DDRD = 0b11111111;
	DDRB = 0b11111111;
}

int main()
{
	initialize();

	xTaskCreate(changeSymbolTask, NULL, 
		configMINIMAL_STACK_SIZE, NULL, 3, NULL);

	xTaskCreate(pollButtonsTask, NULL, 
		configMINIMAL_STACK_SIZE, NULL, 4, NULL);

	xTaskCreate(displayTask, NULL, 
		configMINIMAL_STACK_SIZE, NULL, 4, &displayTaskHandle);

	vTaskStartScheduler();

	return 0;
}

void displayTask()
{
	int displayTaskInterval = 3;

	while(1)
	{
		PORTB = pB;
		PORTD = pD;

		vTaskDelay(displayTaskInterval);
	}
}

void changeSymbolTask()
{
	while(1)
	{
		if (pushedButton == 1){
			currentSymbolIndex++;
			if (currentSymbolIndex>12) currentSymbolIndex = 0;

			showM1(currentSymbolIndex);
		}
		else if (pushedButton == 2){
			currentSymbolIndex++;
			if (currentSymbolIndex>5) currentSymbolIndex = 0;
			showM2(currentSymbolIndex);
		}

		vTaskDelay(display_interval);
	}
}

void pollButtonsTask()
{
	int pollButtonInterval = 10;

	while(1)
	{
		if((PINC & (1 << S1)) == 0)
		{
			pushedButton = 1;
			display_interval = 100;
			vTaskResume(displayTaskHandle);
		}
		else if((PINC & (1 << S2)) == 0)
		{
			pushedButton = 2;
			display_interval = 1050;
			vTaskResume(displayTaskHandle);
		}
		else
		{
			PORTB = 0b00000000;
			PORTD = 0b00000000;
	
			pushedButton = 0;
			currentSymbolIndex = 0;
			display_interval = 150;
			
			vTaskSuspend(displayTaskHandle);
		}
		
		vTaskDelay(pollButtonInterval);
	}
}


void showM1(int pos)
{
	pD = m1d[pos];
	pB = m1b[pos];
}

void showM2(int pos){
	pD = m2d[pos];
	pB = m2b[pos];
}




