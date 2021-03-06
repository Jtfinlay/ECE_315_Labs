#include "predef.h"
#include <stdio.h>
#include <ctype.h>
#include <startnet.h>
#include <autoupdate.h>
#include <smarttrap.h>
#include <taskmon.h>
#include <NetworkDebug.h>
#include <sim5234.h>
#include <cfinter.h>
#include "keypad.h"
#include "lcd.h"
#include <string.h>

#define SECOND_LINE_START 0x28

#define EPPAR_RISING_EDGE 0x4
#define EPDDR_INPUT 0xFFF0
#define EPIER_ENABLE 0x2
#define EPFR_RESET 0X2

extern "C" {
	void UserMain(void * pd);
	void IRQIntInit(void);
	void SetIntc(int intc, long func, int vector, int level, int prio);

}

const char * AppName="James and Andrew vs Evil";

struct KeypadButtonTuple {
	const char * Text;
	unsigned char Number;
}



Keypad  myKeypad;
Lcd		myLCD;

/* Instantiate your Queue objects here */
OS_Q inputQueue;


void UserMain(void * pd) {
	BYTE err = OS_NO_ERR;

	InitializeStack();
	OSChangePrio(MAIN_PRIO);
	EnableAutoUpdate();
	StartHTTP();
	EnableTaskMonitor();

#ifndef _DEBUG
	EnableSmartTraps();
#endif

#ifdef _DEBUG
	InitializeNetworkGDB_and_Wait();
#endif

	iprintf("Application started: %s\n", AppName);

	iprintf("Initializing keypad and screen\n");
	myKeypad.Init();
	myLCD.Init(LCD_BOTH_SCR);
	myLCD.PrintString(LCD_UPPER_SCR, "ECE315 Lab #2 Fall 2013");
	OSTimeDly(TICKS_PER_SECOND*1);

	int i = 3;


	/* Initialize your queue and interrupt here */
	struct KeypadButtonTuple *button; // Button struct for queue
	char *msg // String for printing
	err = OSQInit(&inputQueue, start, 10*sizeof(struct KeypadButtonTuple)); // Initialize queue
	if(err != OS_NO_ERR) {
		iprintf("Error initializing queue");
		exit(0);
	}

	IRQIntInit();

	myLCD.Clear(LCD_BOTH_SCR);


	while (1) {
		/* Insert your queue usage stuff, parsing and scrolling control here */
		/* You may also choose to do a quick poll of the Data Avail line
		 * of the encoder to convince yourself that the keypad encoder works.
		 */

		OSTimeDly(TICKS_PER_SECOND*0.25);
		
		// Grab new button press from queue and create string to be printed
		button = (struct KeypadButtonTuple*) OSQPend(&inputQueue, 0, &err);
		snprintf(msg, 15, "Button:%i->%s", button->Number, button->Text);

		/* Print string.
		 * i is based on which line of the LCD screen we are currently on
		 */
		switch(i) {
		case 0:
			i++;
			myLCD.MoveCursor(LCD_UPPER_SCR, SECOND_LINE_START);
			myLCD.PrintString(LCD_UPPER_SCR, msg);
			break;
		case 1:
			i++;
			myLCD.Home(LCD_LOWER_SCR);
			myLCD.PrintString(LCD_LOWER_SCR, msg);
			break;
		case 2:
			i++;
			myLCD.MoveCursor(LCD_LOWER_SCR, SECOND_LINE_START);
			myLCD.PrintString(LCD_LOWER_SCR, msg);
			break;
		case 3:
		// At bottom of Screen, resetting i and clearing screen to move to top.
			i = 0;
			myLCD.Clear(LCD_BOTH_SCR);
			myLCD.Home(LCD_UPPER_SCR);
			myLCD.PrintString(LCD_UPPER_SCR, msg);
			break;
		}

	}
}

/* The INTERRUPT MACRO handles the house keeping for the vector table
 * and interrupt model.
 * Unfortunately the 0x2500 magic number must stay due to the MACRO syntax
 * The 0x2500 refers to the status register value that the microprocessor
 * will have during the interrupt.
 * Consult the NetBurner Runtime Libraries manual for information on this MACRO
 * Restrictions on what you can and cannot call inside an interrupt service routine
 * are listed there as well */

INTERRUPT(out_irq_pin_isr, 0x2500){

	sim.eport.epfr |= EPFR_RESET;

	struct KeypadButtonTuple press;
	press.Number = myKeypad.GetNewButtonNumber();
	press.Text = myKeypad.GetLastButtonString();

	OSQPost(&inputQueue,(void *)&press);

}

/* The registers that you need to initialize to get
 * interrupts functioning are:
 * sim.eport.eppar
 * sim.eport.epddr
 * sim.eport.epier
 *
 * You also need to call SetIntC with the correct parameters.
 * SetIntC is also documented in the NetBurner Runtime manual.
 *
 * Access the edge port registers by treating them as any variable
 * sim.eport.eppar = blah;
 *
 * The documentation on the interrupt controller will contain the information
 * on how to signal to the processor that it should return to normal processing.
 */
void IRQIntInit(void) {
	sim.eport.eppar |= EPPAR_RISING_EDGE; //b0100
	sim.eport.epddr &= EPDDR_INPUT; //b0000
	sim.eport.epier |= EPIER_ENABLE; //b0010

	SetIntc(0, (long) out_irq_pin_isr, 1, 1, 1);
}


