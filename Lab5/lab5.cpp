/* Rev:$Revision: 1.1 $ */
/******************************************************************************
 * Copyright 1998-2008 NetBurner, Inc.  ALL RIGHTS RESERVED
 *   Permission is hereby granted to purchasers of NetBurner Hardware
 *   to use or modify this computer program for any use as long as the
 *   resultant program is only executed on NetBurner provided hardware.
 *
 *   No other rights to use this program or it's derivitives in part or
 *   in whole are granted.
 *
 *   It may be possible to license this or other NetBurner software for
 *   use on non NetBurner Hardware. Please contact Licensing@Netburner.com
 *   for more information.
 *
 *   NetBurner makes no representation or warranties with respect to the
 *   performance of this computer program, and specifically disclaims any
 *   responsibility for any damages, special or consequential, connected
 *   with the use of this program.
 *
 *   NetBurner, Inc
 *   5405 Morehouse Drive
 *   San Diego Ca, 92121
 *   http://www.netburner.com
 *
 *****************************************************************************/

#include "predef.h"
#include <stdio.h>
#include <ctype.h>
#include <startnet.h>
#include <autoupdate.h>
#include <taskmon.h>
#include <dhcpclient.h>
#include <string.h>
#include "stepper.h"
#include "keypad.h"
#include <eTPU.h>
#include <ETPUInit.h>
#include <eTPU_sm.h>
#include <sim5234.h>
#include <cfinter.h>
#include "lcd.h"
#include "motorconstants.h"
#include "formdata.h"

#define EPPAR_RISING_EDGE 0x4
#define EPDDR_INPUT 0xFFF0
#define EPIER_ENABLE 0x2
#define EPFR_RESET 0X2

#define DIP_SW_ON 0xFF

const char *AppName = "Andrew!";

extern "C"
{
	void UserMain( void *pd );
	void DisplayLameCounter( int sock, PCSTR url );
	char * strtrim(char *str);
	void DisplayMaxRPM(int sock, PCSTR url);
	void DisplayMinRPM(int sock, PCSTR url);
	void DisplayRotations(int sock, PCSTR url);
	void DisplayDirection(int sock, PCSTR url);
	void writeImage(BYTE error, char * buffer);
	void DisplayMotorStep(int sock, PCSTR url);
	void IRQIntInit(void);
	void SetIntc(int intc, long func, int vector, int level, int prio);
}

extern void RegisterPost();

FormData myData;
OS_SEM form_sem;
OS_Q lcdQueue;
Keypad myKeypad;
Lcd myLCD;
Stepper myStepper(SM_MASTER_CHANNEL, SM_ACCEL_TABLE_SIZE);

void *msg[20];

#define MAX_COUNTER_BUFFER_LENGTH 200


void UserMain( void *pd )
{
	BYTE err = OS_NO_ERR;
	InitializeStack();
	OSChangePrio( MAIN_PRIO );
	EnableAutoUpdate();

	eTPUInit();


	myLCD.Init(LCD_BOTH_SCR);
	myKeypad.Init();
	IRQIntInit();

	err = OSQInit(&lcdQueue, msg, 10*sizeof(char)); // Initialize queue
	if(err != OS_NO_ERR) {
		iprintf("Error initializing queue");
	}


	/* Initialise your formdata and stepper class here based on the output
	 * from the DIP switches.
	 * Sample Call for the initialization of the eTPU in the
	 * stepper motor class:
	 * myStepper.Init(ECE315_ETPU_SM_FULL_STEP_MODE,
					SM_MAX_PERIOD,
					SM_INIT_SLEW_PERIOD);
	 */


	if(getdipsw() == DIP_SW_ON) {
		myStepper.Init(ECE315_ETPU_SM_FULL_STEP_MODE,
				SM_MAX_PERIOD,
				SM_INIT_SLEW_PERIOD);
		myData.Init(ECE315_ETPU_SM_FULL_STEP_MODE);
	}
	else {
		myStepper.Init(ECE315_ETPU_SM_HALF_STEP_MODE,
				SM_MAX_PERIOD,
				SM_INIT_SLEW_PERIOD);
		myData.Init(ECE315_ETPU_SM_HALF_STEP_MODE);
	}

	StartHTTP();
	EnableTaskMonitor();

	//Call a registration function for our Form code
	// so POST requests are handled properly.
	RegisterPost();
	iprintf("%s\n", AppName);
	myLCD.Clear(LCD_BOTH_SCR);
	// Display welcome message
	myLCD.PrintString(LCD_UPPER_SCR, "Welcome to Lab 5- ECE315");
	OSTimeDly(TICKS_PER_SECOND*1);

	char* msg;

	while ( (msg = (char *) OSQPend(&lcdQueue, 0, &err)) != NULL )
	{
		switch(*msg) {
		case '*':
			myStepper.Stop();
			myLCD.Clear(LCD_LOWER_SCR);
			myLCD.Home(LCD_LOWER_SCR);
			myLCD.PrintString(LCD_LOWER_SCR, "Emergency Stop Requested");
			break;
		case 'w':
			myLCD.Clear(LCD_LOWER_SCR);
			myLCD.Home(LCD_LOWER_SCR);
			myLCD.PrintString(LCD_LOWER_SCR, "CW Motor Movement Requested");
			break;
		case 'c':
			myLCD.Clear(LCD_LOWER_SCR);
			myLCD.Home(LCD_LOWER_SCR);
			myLCD.PrintString(LCD_LOWER_SCR, "CCW Motor Movement Requested");
			break;
		case 's':
			myLCD.Clear(LCD_LOWER_SCR);
			myLCD.Home(LCD_LOWER_SCR);
			myLCD.PrintString(LCD_LOWER_SCR, "Regular Stop Requested");
			break;
		}
	}
}

/* Name: DisplayLameCounter
 * Description: Displays the number of times the page has been (re)loaded.
 * Meant to be called using <!-- FUNCTIONCALL DisplayLameCounter --> in the
 * index.htm file that contains our forms.
 * Inputs: int sock is a file descriptor for the socket that we are
 * using to communicate with the client.
 * Outputs: None
 */
void DisplayLameCounter( int sock, PCSTR url )
{
	static int form_counter = 0;
	char buffer[MAX_COUNTER_BUFFER_LENGTH+1];

	if((sock > 0) && (url != NULL)) {
		iprintf(url);
		snprintf(buffer,MAX_COUNTER_BUFFER_LENGTH, "<H1>The page has been reloaded %d times. </H1>", form_counter );
		form_counter++;
		writestring(sock,(const char *) buffer);

	}
}

void DisplayMotorStep(int sock, PCSTR url) {
	myData.pend();

	char buffer[MAX_COUNTER_BUFFER_LENGTH+1];
	if ((sock > 0) && (url != NULL)) {

		switch(myData.GetMode()) {
		case ECE315_ETPU_SM_HALF_STEP_MODE:
			snprintf(buffer, MAX_COUNTER_BUFFER_LENGTH, "ECE 315 Half Step Motor Controller");
			break;
		case ECE315_ETPU_SM_FULL_STEP_MODE:
			snprintf(buffer, MAX_COUNTER_BUFFER_LENGTH, "ECE 315 Full Step Motor Controller");
			break;
		}

		iprintf("%s\n", buffer);
		writestring(sock, (const char *) buffer);
	}
	myData.post();
}

void DisplayMaxRPM(int sock, PCSTR url)
{

	myData.pend();

	char buffer[MAX_COUNTER_BUFFER_LENGTH+1];
	if ((sock > 0) && (url != NULL)) {
		iprintf("DisplayMaxRPM: %s\n",url);
		snprintf(buffer, MAX_COUNTER_BUFFER_LENGTH,
				"<input type='text' name='MAX_RPM' value='%d' />",
				myData.GetMaxRPM());
		writeImage(myData.GetErrorMaxRPM(), buffer);
		writestring(sock, (const char *) buffer);
	}
	myData.post();
}

void DisplayMinRPM(int sock, PCSTR url)
{
	myData.pend();

	char buffer[MAX_COUNTER_BUFFER_LENGTH+1];

	if ((sock > 0) && (url != NULL)) {
		iprintf("DisplayMaxRPM: %s\n",url);
		snprintf(buffer, MAX_COUNTER_BUFFER_LENGTH,
				"<input type='text' name='MIN_RPM' value='%d' />",
				myData.GetMinRPM());
		writeImage(myData.GetErrorMinRPM(), buffer);
		writestring(sock, (const char *) buffer);
	}

	myData.post();
}

void DisplayRotations(int sock, PCSTR url)
{
	myData.pend();

	char buffer[MAX_COUNTER_BUFFER_LENGTH+1];

	if ((sock > 0) && (url != NULL)) {
		iprintf("DisplayMaxRPM: %s\n",url);
		snprintf(buffer, MAX_COUNTER_BUFFER_LENGTH,
				"<input type='text' name='ROTATIONS' value='%d' />",
				myData.GetRotations());
		writeImage(myData.GetErrorRotations(), buffer);
		writestring(sock, (const char *) buffer);
	}
	myData.post();
}

void DisplayDirection(int sock, PCSTR url)
{
	myData.pend();

	char buffer[MAX_COUNTER_BUFFER_LENGTH+1];

	if ((sock > 0) && (url != NULL)) {
		iprintf("DisplayMaxRPM: %s\n",url);
		snprintf(buffer, MAX_COUNTER_BUFFER_LENGTH,
				"<SELECT NAME='DIRECTION' VALUE='CCW'>");
		snprintf(buffer, MAX_COUNTER_BUFFER_LENGTH,
				"%s<OPTION VALUE='CCW'>Counter Clockwise</OPTION>", buffer);
		snprintf(buffer, MAX_COUNTER_BUFFER_LENGTH,
				"%s<OPTION VALUE='CW'>Clockwise</OPTION>", buffer);
		snprintf(buffer, MAX_COUNTER_BUFFER_LENGTH,
				"%s</SELECT>", buffer);
		writestring(sock, (const char *) buffer);
	}
	myData.post();
}

/*
 * Trims leading and trailing '+' from string.
 *
 * Adapted from http://stackoverflow.com/a/122721/2152672
 */
char * strtrim(char *str)
{
	char *end;

	// Trim leading space
	while(*str == '+') str++;

	if(*str == 0)  // All spaces?
		return str;

	// Trim trailing space
	end = str + strlen(str) - 1;
	while(end > str && *end == '+') end--;

	// Write new null terminator
	*(end+1) = 0;

	return str;
}

void writeImage(BYTE error, char * buffer) {
	if(error == FORM_ERROR) {
		snprintf(buffer, MAX_COUNTER_BUFFER_LENGTH,
				"%s <img src='http://i.imgur.com/crRhbqn.gif' height='150' />", buffer);
	}
	else if(error == FORM_OK){
		snprintf(buffer, MAX_COUNTER_BUFFER_LENGTH,
				"%s <img src='http://i.imgur.com/Y7Fxh6s.gif' height='150' />", buffer);

	}

	return;
}

INTERRUPT(out_irq_pin_isr, 0x2500){

	sim.eport.epfr |= EPFR_RESET;

	OSQPost(&lcdQueue,(void *)myKeypad.GetNewButtonString());
}


void IRQIntInit(void) {
	sim.eport.eppar |= EPPAR_RISING_EDGE; //b0100
	sim.eport.epddr &= EPDDR_INPUT; //b0000
	sim.eport.epier |= EPIER_ENABLE; //b0010

	SetIntc(0, (long) out_irq_pin_isr, 1, 1, 1);
}


