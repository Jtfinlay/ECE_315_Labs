/* Nancy Minderman
 * May 08, 2013
 * nancy.minderman@ualberta.ca
 * Solution Code for Lab #1
 * ECE315
 */

#include "predef.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <startnet.h>
#include <autoupdate.h>
#include <smarttrap.h>
#include <taskmon.h>
#include <NetworkDebug.h>
#include "lcd.h"

/* User task priorities always based on MAIN_PRIO */
/* The priorities between MAIN_PRIO and the IDLE_PRIO are available */
#define TASK1_PRIO 	MAIN_PRIO + 1
#define TASK2_PRIO 	MAIN_PRIO + 2
#define TASK3_PRIO	MAIN_PRIO + 3
#define TASK4_PRIO	MAIN_PRIO + 4

/* LCD cursor positions for edge of screen and beginning of second screen. */
#define LCD_TOP_END		0x27
#define LCD_BOTTOM_BEGIN 	0x28
#define LCD_BOTTOM_END 		0x4F

/* DIP Switches defined as on/off */
#define DIP_SWITCH_ON	0xFF

/* If you create new C subroutines you'll need to add them inside
 * the braces
 */

extern "C" {
	void UserMain(void * pd);
	void StartTask1(void);
	void StartTask2(void);
	void StartTask3(void);
	void StartTask4(void);
	void Task1Main(void * pd);
	void Task2Main(void * pd);
	void Task3Main(void * pd);
	void Task4Main(void * pd);
}

/* Remember to add in a personal identifier next to the Lab 1 string
 * I'll be looking for it in the demo
 */
const char * AppName= "James and Andrew in the MORRRNNIING";
const char * Welcome="Welcome to ECE315-Fall2013";

/* Task stacks for all the user tasks */
/* If you add a new task you'll need to add a new stack for that task */
DWORD Task1Stk[USER_TASK_STK_SIZE] __attribute__( ( aligned( 4 ) ) );
DWORD Task2Stk[USER_TASK_STK_SIZE] __attribute__( ( aligned( 4 ) ) );
DWORD Task3Stk[USER_TASK_STK_SIZE] __attribute__( ( aligned( 4 ) ) );
DWORD Task4Stk[USER_TASK_STK_SIZE] __attribute__( ( aligned( 4 ) ) );

/* This really needs to be in the heap so it's here and allocated
 * as a global class due to its size */
Lcd myLCD;
OS_SEM theSemaphore1, theSemaphore2, theSemaphore3, theSemaphore4;

/* Everything in userland starts with this task */
void UserMain(void * pd) {

	BYTE err = OS_NO_ERR;
	InitializeStack();
	OSChangePrio(MAIN_PRIO);
	EnableAutoUpdate();
	StartHTTP();
	EnableTaskMonitor();
	/* The LCD must be initialised before any task that may use the
	 * class is created.
	 */
	myLCD.Init(LCD_BOTH_SCR);
	myLCD.PrintString(LCD_UPPER_SCR, Welcome);
	//myLCD.ModifyScreenFeatures(LCD_BOTH_SCR, CMD_DISPLAY|DISPLAY_ON|DISPLAY_NOCURSOR|DISPLAY_NOBLINK);
	OSTimeDly(TICKS_PER_SECOND*1);

	/* For exercise 3 and 4 put your semaphore and/or queue initialisations
	 * here.
	 */
	if (OSSemInit(&theSemaphore1,1) == OS_SEM_ERR)
		iprintf("Error in initializing semaphore1.\n");
	if (OSSemInit(&theSemaphore2,0) == OS_SEM_ERR)
		iprintf("Error in initializing semaphore2.\n");
	if (OSSemInit(&theSemaphore3,0) == OS_SEM_ERR)
		iprintf("Error in initializing semaphore3.\n");
	if (OSSemInit(&theSemaphore4,0) == OS_SEM_ERR)
		iprintf("Error in initializing semaphore4.\n");

#ifndef _DEBUG
	EnableSmartTraps();
#endif

#ifdef _DEBUG
	InitializeNetworkGDB_and_Wait();
#endif

	iprintf("Application started: %s\n", AppName);

	/* Start all of our user tasks here. If you add a new task
	 * add it in here.
	 */
	StartTask1();
	StartTask2();
	StartTask3();
	StartTask4();


	while (1) {
		OSTimeDly(TICKS_PER_SECOND*0.5);

	}
}

/* Name: StartTask1
 * Description:
 * Inputs: none
 * Outputs: none
 */
void StartTask1(void) {
	BYTE err = OS_NO_ERR;

	err = OSTaskCreatewName(	Task1Main,
			(void *)NULL,
			(void *) &Task1Stk[USER_TASK_STK_SIZE],
			(void *) &Task1Stk[0],
			TASK1_PRIO, "Task 1" );
	if(err) {
		iprintf("Task Creation Error in StartTask1\n");
	}
}

/* Name: Task1Main
 * Description:
 * Inputs:  void * pd -- pointer to generic data . Currently unused.
 * Outputs: none
 */
void	Task1Main( void * pd) {
	/* place semaphore usage code inside the loop */
	while (1) {
		if (OSSemPend(&theSemaphore1, 0) == OS_TIMEOUT)
			iprintf("Timeout waiting for Semaphore 1");

		myLCD.Clear(LCD_BOTH_SCR);

		if (getdipsw() == DIP_SWITCH_ON) {
			myLCD.Home(LCD_BOTH_SCR);
		} else {
			myLCD.MoveCursor(LCD_UPPER_SCR, LCD_TOP_END);
		}

		myLCD.PrintChar(LCD_UPPER_SCR, 'a');
		OSTimeDly(TICKS_PER_SECOND*.5);


		if (getdipsw() == DIP_SWITCH_ON) {
			if (OSSemPost(&theSemaphore2) == OS_SEM_OVF)
				iprintf("Error posting to Semaphore 2");
		} else {
			if (OSSemPost(&theSemaphore4) == OS_SEM_OVF)
				iprintf("Error posting to Semaphore 4");
		}
	}

}

void StartTask2(void) {
	BYTE err = OS_NO_ERR;

	err = OSTaskCreatewName(	Task2Main,
			(void *)NULL,
			(void *) &Task2Stk[USER_TASK_STK_SIZE],
			(void *) &Task2Stk[0],
			TASK2_PRIO, "Task 2" );
	if(err) {
		iprintf("Task Creation Error in StartTask2\n");
	}
}
void	Task2Main( void * pd) {
	while (1) {
		if (OSSemPend(&theSemaphore2, 0) == OS_TIMEOUT)
			iprintf("Timeout waiting for Semaphore 2");

		myLCD.Clear(LCD_BOTH_SCR);

		if (getdipsw() == DIP_SWITCH_ON) {
			myLCD.MoveCursor(LCD_BOTH_SCR, LCD_BOTTOM_BEGIN);
		} else {
			myLCD.MoveCursor(LCD_BOTH_SCR, LCD_BOTTOM_END);
		}


		myLCD.PrintString(LCD_UPPER_SCR, "b");
		OSTimeDly(TICKS_PER_SECOND*.5);

		if (getdipsw() == DIP_SWITCH_ON) {
			if (OSSemPost(&theSemaphore3) == OS_SEM_OVF)
				iprintf("Error posting to Semaphore 3");
		} else {
			if (OSSemPost(&theSemaphore1) == OS_SEM_OVF)
				iprintf("Error posting to Semaphore 1");
		}
	}
}
void StartTask3(void) {
	BYTE err = OS_NO_ERR;

	err = OSTaskCreatewName(	Task3Main,
			(void *)NULL,
			(void *) &Task3Stk[USER_TASK_STK_SIZE],
			(void *) &Task3Stk[0],
			TASK3_PRIO, "Task 3" );
	if(err) {
		iprintf("Task Creation Error in StartTask3\n");
	}
}
void	Task3Main( void * pd) {
	while (1) {
		if (OSSemPend(&theSemaphore3, 0) == OS_TIMEOUT)
			iprintf("Timeout waiting for Semaphore 3");

		myLCD.Clear(LCD_BOTH_SCR);

		if (getdipsw() == DIP_SWITCH_ON) {
			myLCD.Home(LCD_BOTH_SCR);
		} else {
			myLCD.MoveCursor(LCD_BOTH_SCR, LCD_TOP_END);
		}


		myLCD.PrintString(LCD_LOWER_SCR, "c");
		OSTimeDly(TICKS_PER_SECOND*.5);

		if (getdipsw() == DIP_SWITCH_ON) {
			if (OSSemPost(&theSemaphore4) == OS_SEM_OVF)
				iprintf("Error posting to Semaphore 4");
		} else {
			if (OSSemPost(&theSemaphore2) == OS_SEM_OVF)
				iprintf("Error posting to Semaphore 2");
		}
	}
}
void StartTask4(void) {
	BYTE err = OS_NO_ERR;

	err = OSTaskCreatewName(	Task4Main,
			(void *)NULL,
			(void *) &Task4Stk[USER_TASK_STK_SIZE],
			(void *) &Task4Stk[0],
			TASK4_PRIO, "Task 4" );
	if(err) {
		iprintf("Task Creation Error in StartTask4\n");
	}
}
void	Task4Main( void * pd) {
	while (1) {
		if (OSSemPend(&theSemaphore4, 0) == OS_TIMEOUT)
			iprintf("Timeour waiting for Semaphore 4");

		myLCD.Home(LCD_BOTH_SCR);
		myLCD.Clear(LCD_BOTH_SCR);

		if (getdipsw() == DIP_SWITCH_ON) {
			myLCD.MoveCursor(LCD_BOTH_SCR, LCD_BOTTOM_BEGIN);
		} else {
			myLCD.MoveCursor(LCD_BOTH_SCR, LCD_BOTTOM_END);
		}


		myLCD.PrintString(LCD_LOWER_SCR, "d");
		OSTimeDly(TICKS_PER_SECOND*0.5);

		if (getdipsw() == DIP_SWITCH_ON) {
			if (OSSemPost(&theSemaphore1) == OS_SEM_OVF)
				iprintf("Error posting to Semaphore 1");
		} else {
			if (OSSemPost(&theSemaphore3) == OS_SEM_OVF)
				iprintf("Error posting to Semaphore 3");
		}
	}
}


