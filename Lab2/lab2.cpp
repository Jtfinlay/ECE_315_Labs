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

extern "C" {
void UserMain(void * pd);
void IRQIntInit(void);
void SetIntc(int intc, long func, int vector, int level, int prio);
}

const char * AppName="James and Andrew vs Evil";

Keypad  myKeypad;
Lcd		myLCD;

/* Instantiate your Queue objects here */


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

    myKeypad.Init();
    myLCD.Init(LCD_BOTH_SCR);
    myLCD.PrintString(LCD_UPPER_SCR, "ECE315 Lab #2 Fall 2013");
    OSTimeDly(TICKS_PER_SECOND*1);


    /* Initialize your queue and interrupt here */

    while (1) {
    	/* Insert your queue usage stuff, parsing and scrolling control here */
    	/* You may also choose to do a quick poll of the Data Avail line
    	 * of the encoder to convince yourself that the keypad encoder works.
    	 */
    	OSTimeDly(TICKS_PER_SECOND*100);
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
	sim.eport.eppar = 1<<3; //b1000
	sim.eport.epddr = 0; //b0000
	sim.eport.epier = 1<<1; //b0010

	SetIntc(0, INTERRUPT, 0, 1, 0); // don't think zero is right. We'll try it anyways
}


