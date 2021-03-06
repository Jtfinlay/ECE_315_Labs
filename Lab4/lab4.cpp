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
#include "FormData.h"
#include <eTPU.h>
#include <ETPUInit.h>
#include <eTPU_sm.h>
#include <sim5234.h>
#include <cfinter.h>
#include "motorconstants.h"
#include <stdlib.h>

const char *AppName = "James and Andrew";


extern "C"
{
	void UserMain( void *pd );
	void IRQIntInit(void);
	void SetIntc(int intc, long func, int vector, int level, int prio);
	void DisplayLameCounter( int sock, PCSTR url );
	void DisplayMaxRPM(int sock, PCSTR url);
	void DisplayMinRPM(int sock, PCSTR url);
	void DisplayRotations(int sock, PCSTR url);
	void DisplayDirection(int sock, PCSTR url);
	void writeImage(BYTE error, char * buffer);
}

extern void RegisterPost();

#define MAX_COUNTER_BUFFER_LENGTH 200
#define DIP_ON 0xFF


FormData myData;

void UserMain( void *pd )
{
	InitializeStack();
	OSChangePrio( MAIN_PRIO );
	EnableAutoUpdate();
	StartHTTP();
	EnableTaskMonitor();

	//Call a registration function for our Form code
	// so POST requests are handled properly.
	RegisterPost();

	// Insert your code that queries the DIP switches and
	// initialises the motor mode accordingly here.
	if (getdipsw() == DIP_ON)
		myData.Init(ECE315_ETPU_SM_HALF_STEP_MODE);
	else
		myData.Init(ECE315_ETPU_SM_FULL_STEP_MODE);


	while ( 1 )
	{
		OSTimeDly(TICKS_PER_SECOND*100);
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
		snprintf(buffer,MAX_COUNTER_BUFFER_LENGTH,
				"<H1>The page has been reloaded %d times. </H1>", form_counter );
		form_counter++;
		writestring(sock,(const char *) buffer);

	}
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
				"<SELECT NAME='DIRECTION' VALUE='%s'>", "CW");
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
