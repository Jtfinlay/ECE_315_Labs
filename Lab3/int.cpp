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
	sim.fec.eir |= 1<<28; // reset GRA
	putleds(0xFF);
}

// Place your Interrupt querying code here for exercise 2
void QueryIntsFEC(void){
	/* TODO: Write to LCD */
	myLCD.Clear(LCD_BOTH_SCR);
	myLCD.Home(LCD_BOTH_SCR);

	char output[400];
	sprintf(output, "UN:%d, ", sim.fec.eimr>>19 & 1);
	sprintf(output, "%sRL:%d, ", output, sim.fec.eimr>>20 & 1);
	sprintf(output, "%sLC:%d, ", output, sim.fec.eimr>>21 & 1);
	sprintf(output, "%sEBERR:%d, ", output, sim.fec.eimr>>22 & 1);
	sprintf(output, "%sMII:%d, ", output, sim.fec.eimr>>23 & 1);
	sprintf(output, "%sRXB:%d, ", output, sim.fec.eimr>>24 & 1);
	sprintf(output, "%sTXB:%d, ", output, sim.fec.eimr>>26 & 1);
	sprintf(output, "%sTXF:%d, ", output, sim.fec.eimr>>27 & 1);
	sprintf(output, "%sGRA:%d, ", output, sim.fec.eimr>>28 & 1);
	myLCD.PrintString(LCD_UPPER_SCR, output);


	sprintf(output, "BABT:%d, ", sim.fec.eimr>>29 & 1);
	sprintf(output, "%sBABR:%d, ", output, sim.fec.eimr>>30 & 1);
	sprintf(output, "%sBABR:%d, ", output, sim.fec.eimr>>30 & 1);
	sprintf(output, "%sHBERR:%d", output, sim.fec.eimr>>31 & 1);
	myLCD.PrintString(LCD_LOWER_SCR, output);



/*	iprintf("--------Sim.fec.eimr--------\n");
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
	iprintf("HBERR:%d\n", sim.fec.eimr>>31 & 1);*/
}

// Initialize the FEC interrupt mask register and interrupt controller
// for exercise 2 here.
// Use SetIntc to do this with the following values
// use level = 1 and prio = 1

void InitializeIntsFEC(void){
	//putleds(0xff);
	sim.fec.eimr |= 1<<28; // set GRA

	SetIntc(0, (long) gra_isr, 32, 1, 1);
}
