/* Nancy Minderman
nem@ece.ualberta.ca
October 2, 2008
Original Code by Justin Smalley

Mods:
Oct 2, 2008
- Changed Text array for keymap to reflect schematic given to students
and Nancy's keypad.
- Removed calibration routine because it borks:  rewrite it
- Removed calibration member because calibration fails
- Removed ValueGrid because it's unused
- Removed GetNumber because it's unused
- Removed all flag functions
*/
#include "predef.h"
#include <stdio.h>
#include <ctype.h>

#include <ETPU.h>
#include <ETPUinit.h>
#include <etpu_gpio.h>   // Freescale eTPU GPIO API
#include <pins.h>
#include <system.h>

#include <pinmap.h>
#include "keypad.h"

	const char KeypadButtonMapText[BUTTONS] [MAX_BUTTON_NAME] =
	{
	"*", "*", "*", "*",
	"*", "*", "*", "*",
	"*", "*", "*", "*",
	"*", "*", "*", "*"
	};

/* Name:Keypad Constructor
 * Description: Constructor for the Keypad class. Not much to do beyond
 * initializing the only class ivar
 * Inputs: none
 * Outputs: none
 */
Keypad::Keypad(void)
{
	last_encoded_data = 0;
}

/* Name:Init
 * Description: Initializes all the GPIO pins that interface to the keypad.
 * Inputs: none
 * Outputs: none
 */
void Keypad::Init(void)
{
	// Set the DA, DB, DC, DD , and DAvail pins to be general purpose IO pins
		KEYPAD_DO_A.function(PIN_GPIO);	// Keypad Data out A
		KEYPAD_DO_B.function(PIN_GPIO);	// Keypad Data Out B
		KEYPAD_DO_C.function(PIN_GPIO);	// Keypad Data Out C
		KEYPAD_DO_D.function(PIN_GPIO);	// Keypad Data out D
		KEYPAD_D_AVAIL.function(PIN_GPIO);	// Keypad Data Available

		// Set the DO_A, DO_B, DO_C, DO_D and DO_Avail pins to be
		// high impedance input
		KEYPAD_DO_A.hiz();	//Set KEYPAD Data out A
		KEYPAD_DO_B.hiz();	//Set KEYPAD Data out B
		KEYPAD_DO_C.hiz();	//Set KEYPAD Data out C
		KEYPAD_DO_D.hiz();	//Set KEYPAD Data out D
		KEYPAD_D_AVAIL.hiz();	// Set KEYPAD Data Available
}

/* Name:read_data
 * Description: Reads the data on all the 4 Data Out lines and stores
 * it in last_encoded_data
 * Inputs: none
 * Outputs: none
 */
void Keypad::read_data(void)
{
	//Record values on DA, DB, DC, DD from 922 Encoder
		last_encoded_data = 0;
		if (KEYPAD_DO_A) last_encoded_data += 0x01;
		if (KEYPAD_DO_B) last_encoded_data += 0x02;
		if (KEYPAD_DO_C) last_encoded_data += 0x04;
		if (KEYPAD_DO_D) last_encoded_data += 0x08;
}

/* Name:GetNewButtonNumber
 * Description: reads the data out lines of the encoder and returns that
 * newly read value
 * Inputs: none
 * Outputs: unsigned char from 0-15
 */
unsigned char Keypad::GetNewButtonNumber(void)
{

	read_data();
	return last_encoded_data;
}


/* Name:GetLastButtonNumber
 * Description: Returns the previously read value on the Data out lines from
 * the encoder
 * Inputs:
 * Outputs:
 */
unsigned char Keypad::GetLastButtonNumber(void)
{
	return last_encoded_data;
}

/* Name:GetNewButtonString
 * Description: Returns a pointer to a string that corresponds to the newly
 * read value on the Data Out lines. Calling this method will read the new
 * value on Data out lines on the encoder.
 * Inputs: none
 * Outputs: returns a pointer to a string in the string array that maps
 * strings from the button number.
 */
const char * Keypad::GetNewButtonString(void)
{
	read_data();
	return(KeypadButtonMapText[last_encoded_data]);
}

/* Name: GetLastButtonString
 * Description: Returns a pointer to a string that corresponds to the last
 * value read on the Data Out lines. Calling this method will not read a new
 * value on the Data Out lines on the encoder.
 * Inputs: none
 * Outputs: returns a pointer to a string in the string array that maps
 * strings from the button number.
 */
const char * Keypad::GetLastButtonString(void)
{
	return(KeypadButtonMapText[last_encoded_data]);
}

/* Name: ButtonPressed
 * Description: The method reads the current value of the data available line
 * on the encoder.
 * Inputs: none
 * Outputs: It returns TRUE is the Data available lines is high and FALSE
 * if low. The data available line is high is any button is being pressed and
 * low if no button is being pressed.
 */
unsigned char Keypad::ButtonPressed(void)
{
	if (KEYPAD_D_AVAIL == HIGH) {
		return TRUE;
	} else {
		return FALSE;
	}
}
