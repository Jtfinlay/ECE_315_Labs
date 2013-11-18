/*
 * lab3.cpp
 * Created:	May 27th, 2013
 * Author: 	Nancy Minderman
 * 			nancy.minderman@ualberta.ca
 *
 */

#include "predef.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <startnet.h>
#include <autoupdate.h>
#include <dhcpclient.h>
#include <smarttrap.h>
#include <taskmon.h>
#include <NetworkDebug.h>
#include <utils.h>
#include <ucos.h>
#include <cfinter.h>
#include <ip.h>
#include <tcp.h>
#include <udp.h>
#include <multihome.h>
#include <nbtime.h>
#include <rtc.h>
#include <buffers.h>
#include "sim5234.h"
#include "services_udp.h"
#include "lcd.h"
#include "ethervars.h"

#define	WAIT_FOREVER 0

#define DIP_ON 0xFF
#define TCR_OFF 0xFFFFFFFE
#define LED_OFF 0x00
#define TCR_ON 0x1

extern "C" {
	void UserMain(void * pd);
}

extern void QueryIntsFEC(void);
extern void InitializeIntsFEC(void);

/* User task priorities always based on MAIN_PRIO */
/* The priorities between MAIN_PRIO and the IDLE_PRIO are available
   Note that the following PRIOs are taken by the number and caesar servers
   number server prio = MAIN_PRIO + 2
   caesar server prio = MAIN_PRIO + 3
   See services_udp.h for more info
*/


const char * AppName="James and Andrew 1337 crew";

Lcd myLCD;

/****************************************/
void UserMain(void * pd) {
	BYTE err = OS_NO_ERR;


    InitializeStack();
    OSChangePrio(MAIN_PRIO);
    EnableAutoUpdate();
    StartHTTP();
    StartNumberServer();
    StartCaesarServer();
    EnableTaskMonitor();


    #ifndef _DEBUG
    EnableSmartTraps();
    #endif

    #ifdef _DEBUG
    InitializeNetworkGDB_and_Wait();
    #endif

    iprintf("Application started\n");

    myLCD.Init(LCD_BOTH_SCR);
    myLCD.PrintString(LCD_UPPER_SCR, "Welcome to ECE315 - Fall 2013\n");
    OSTimeDly(TICKS_PER_SECOND*3);


    QueryIntsFEC();
    OSTimeDly(TICKS_PER_SECOND*5);

    InitializeIntsFEC();
    OSTimeDly(1);

    QueryIntsFEC();
    OSTimeDly(TICKS_PER_SECOND*5);


    while (1) {
 
    OSTimeDly(TICKS_PER_SECOND*1);

		if (getdipsw() == DIP_ON)
			sim.fec.tcr |= TCR_ON;
		else {
			sim.fec.tcr &= TCR_OFF;
			putleds(LED_OFF);
		}
		iprintf("Total Transmitted Packets: %d\n",
				sim.fec_rmon_t.packets);
		iprintf("Total Transmitted Broadcast Packets: %d\n",
				sim.fec_rmon_t.bc_pkt);
		iprintf("Total Transmitted Multicast Packets: %d\n",
				sim.fec_rmon_t.mc_pkt);
		iprintf("Total transmitted Unicast Packets: %d\n",
				sim.fec_rmon_t.packets - sim.fec_rmon_t.bc_pkt - sim.fec_rmon_t.mc_pkt);
	}
}
