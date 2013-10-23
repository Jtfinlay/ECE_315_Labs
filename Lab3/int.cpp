/*
 * int.cpp
 *
 *	Created on: May 27th, 2013
 *	Author: 	Nancy Minderman
 *				nancy.minderman@ualberta.ca
 *
 */
#include <stdio.h>
#include <string.h>
#include <startnet.h>
#include <cfinter.h>
#include "sim5234.h"
#include "lcd.h"

#define WAIT_FOREVER			0


extern "C" {
	// This routine helps you to initialise the interrupt controller properly
	void SetIntc( int intc, long func, int vector, int level, int prio );
}

extern Lcd myLCD;

// Place your INTERRUPT ISR code here for exercise 2
// Remember to signal to the processor to return to normal processing
// at the end of the interrupt
INTERRUPT(gra_isr, 0x2400) /* IRQ 4 Mask */
{
	putleds(0x2331);
	sim.fec.eir |= 1<<28; // set GRA
}

// Place your Interrupt querying code here for exercise 2
void QueryIntsFEC(void){
	/* TODO: Write to LCD */
	iprintf("--------Sim.fec.eimr--------\n");
	iprintf("UN:%d\n", sim.fec.eimr>>19 & 1);
	iprintf("RL:%d\n", sim.fec.eimr>>20 & 1);
	iprintf("LC:%d\n", sim.fec.eimr>>21 & 1);
	iprintf("EBERR:%d\n", sim.fec.eimr>>22 & 1);
	iprintf("MII:%d\n", sim.fec.eimr>>23 & 1);
	iprintf("RXB:%d\n", sim.fec.eimr>>24 & 1);
	iprintf("RXF:%d\n", sim.fec.eimr>>25 & 1);
	iprintf("TXB:%d\n", sim.fec.eimr>>26 & 1);
	iprintf("TXF:%d\n", sim.fec.eimr>>27 & 1);
	iprintf("GRA:%d\n", sim.fec.eimr>>28 & 1);
	iprintf("BABT:%d\n", sim.fec.eimr>>29 & 1);
	iprintf("BABR:%d\n", sim.fec.eimr>>30 & 1);
	iprintf("HBERR:%d\n", sim.fec.eimr>>31 & 1);
}

// Initialize the FEC interrupt mask register and interrupt controller
// for exercise 2 here.
// Use SetIntc to do this with the following values
// use level = 1 and prio = 1

void InitializeIntsFEC(void){
	putleds(0xff);
	sim.fec.eimr |= 1<<28; // set GRA

	SetIntc(0, (long) gra_isr, 0, 1, 1);
}
