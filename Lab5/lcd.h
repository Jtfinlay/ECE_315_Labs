/* This code was modified and tailored for use on a NetBurner MOD5234
 * for University of Alberta CMPE 401 lab sections
 * Justin Smalley, 2008 */

/*****************************************************************************/
/* GUIDE TO USING LCD CLASS

	1) Connect the LCD hardware as indicated in the lab1 schematic.
	2) pinmap.h contains the PinIO class mappings for the LCD. They
	   correspond directly to the schematic version. Don't mess with
	   the mappings unless you change the hardware to match.
	3) Construct class at compile time in the heap.
		Lcd myLcd();
	4) Call Initialize function: i.e.
		myLcd.Init(screen);

	Useful LCD Functionalities
	a) wraps around at the end of line
	b) '\n' (ASCII 10) will move to the next line

	Semaphores:
	The LCD uses a semaphore to ensure that LCD commands are atomic.
	All public functions manage the semaphore so the user
	does not have to worry about contention for the LCD for each
	character and command write. Groups of characters and commands need
	to be dealt with by the application.

******************************************************************************/


#ifndef _LCD_H_
#define _LCD_H_


#define LCD_COLS		40
#define LCD_ORIGIN		0
#define LCD_DDRAM_LINE1 0
#define LCD_DDRAM_LINE2 0x40

#define LCD_UNSET_SCR	0
#define LCD_UPPER_SCR	1
#define LCD_LOWER_SCR	2
#define LCD_BOTH_SCR	3


#define LSBIT			0x01
#define HIGH 			1
#define LOW				0

/******** Display Debug Information ***********/
//#define LCD_DEBUG(x) (x)
#define LCD_DEBUG(x)

/* List of available commands */
#define CMD_CLEAR		0x01
#define CMD_HOME		0x02
#define CMD_ENTRY_MODE	0x04
#define CMD_DISPLAY		0x08
#define CMD_SHIFT		0x10
#define CMD_FUNCTION	0x20
#define CMD_CGRAM_ADR	0x40
#define CMD_DDRAM_ADR	0x80

// Function constants to initialise LCD hardware
#define FUNCTION_8BIT	0x10 // enable 8 pin mode
#define FUNCTION_4BIT	0x00 // enable 4 pin mode (not supported)
#define FUNCTION_2LINE	0x08 // LCD has two lines, line two starts at addr 0x40
#define FUNCTION_1LINE	0x00 // LCD has one continuous line
#define FUNCTION_5x10	0x04 // use 5x10 custom characters
#define FUNCTION_5x8	0x00 // use 5x8 custom characters

/* CMD_DISPLAY constants,  can be OR'd together to select one feature
 * from each pair
 */
#define DISPLAY_ON			0x04	// display on
#define DISPLAY_OFF			0x00	// display off
#define DISPLAY_CURSOR		0x02	// cursor on
#define DISPLAY_NOCURSOR	0x00	// cursor off
#define DISPLAY_BLINK		0x01	// cursor blink on
#define DISPLAY_NOBLINK		0x00	// cursor blink off

/* CMD_ENTRY_MODE constants, can be OR'd together to select one feature
 * from each pair
 */
#define ENTRY_CURSOR_INC	0x02 // increment cursor position
#define ENTRY_CURSOR_DEC	0x00 // decrement cursor position
#define ENTRY_SHIFT			0x01 // shift entire display
#define ENTRY_NOSHIFT		0x00 // don't shift display

/* CMD_SHIFT constants, can be OR'd together to select one feature
 * from each pair
 */
#define SHIFT_SCREEN	0x08 // shift display
#define SHIFT_CURSOR	0x00 // shift cursor
#define SHIFT_RIGHT		0x04 // to the right
#define SHIFT_LEFT		0x00 // to the left

#define RS_DATA 		1	// to send data set rs line high
#define RS_CMD 			0  	// to send a command set rs line low

#define LCD_ERROR		0xff // To signal general LCD errors

#ifndef WAIT_FOREVER
#define WAIT_FOREVER	0
#endif

class Lcd;

class Lcd {

protected:

	unsigned char  position_upper; // current value of cursor on upper screen
									// value should be from 0-79
	unsigned char  position_lower;  // current value of cursor on lower screen
									// value should be from 0-79

	OS_SEM			sem;			//internal semaphore to guarantee atomicity of each LCD instruction

	unsigned char   sem_inited;		// Calling Init method more than once should not reinitialize the semaphore

	/* pulses the enable pin E1 to write to the upper LCD */
	void enable_upper(void);

	/* pulses the enable pin E2 to write to the lower LCD */
	void enable_lower(void);

	/* print a character to the LCD at the current position
	 * of the cursor on the specified screen */
	void print_char(unsigned char screen, char data);


	/* Move the cursor to a screen and position.
	 * screen can be upper, lower, or both simultaneously
	 * LCD_UPPER_SCR, LCD_LOWER_SCR, LCD_BOTH_SCR
	 * position can be from 0-79
	 */
	void move(unsigned char screen, unsigned char position);

	/* Sets the RS line for sending either data or commands
	 * to the LCD, two possible values RS_DATA or RS_CMD
	 */
	void set_rs_line(unsigned char  rs);

	/* Sends a command to the LCD over data lines*/
	void send_cmd(unsigned char screen, unsigned char cmd);

	/* Sends data to the LCD to display.
	 * This data must be printable ASCII characters
	 * for the data to show up
	 */
	void send_data(unsigned char screen, unsigned char data);

	/* Sets the data lines D0 to D7 */
	void set_data_lines(unsigned char data);

//*********************** PUBLIC *********************************************
public:

	/* Creates the Lcd class. */
	Lcd();

	/* Initialise LCD hardware*/
	void Init(unsigned char screen);

	/* Clear the display of the specified screen. */
	void Clear(unsigned char screen);

	/* Move cursor to home position (top/left) on specified screen. */
	void Home(unsigned char screen);

	/* Moves the cursor to a position on the specified screen */
	void MoveCursor(unsigned char screen, unsigned char position);

	/* Print a single ASCII character to the LCD screen at the current cursor position. */
	void PrintChar(unsigned char screen, char c);

	/*Print a c-style string to the LCD screen at the current cursor position */
	void PrintString(unsigned char screen, const char * str);

	/* Shift the screen to the right by one position */
	void ShiftDisplayRight(unsigned char screen);

	/* Shift the screen to the left by one position */
	void ShiftDisplayLeft(unsigned char screen);

	/* TurnScreenFeatureOn */
	void ModifyScreenFeatures(unsigned char screen, unsigned char feature);
};

#endif

