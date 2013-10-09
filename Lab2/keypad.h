#ifndef KEYPAD_H_
#define KEYPAD_H_

/*  Nancy Minderman
October 2, 2008
Original code by Justin Smalley.

See mods in keypad.cpp
*/


/* Defines for the data available line */
#define HIGH 			1
#define LOW 			0

/* Array storage maximums for the keypad mapping */
#define BUTTONS			16
#define MAX_BUTTON_NAME  10

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

class Keypad {

protected:

	/* Most recently read button from the Data Out lines on the encoder */
	unsigned char last_encoded_data;

	/* Read the Data Out lines on the encoder */
	void read_data(void);

public:

	/* Constructor initializes only ivar */
	Keypad();

	/* Initialization of pins occurs here */
	void Init(void);

	/* Returns a pointer to a string corresponding to the last read button.
	 * Does not read the Data Out lines.
	 */
	const char * GetLastButtonString(void);

	/* Returns a pointer to a string corresponding to a new button press.
	 * The string mappings are in keypad.cpp
	 * This method does read the Data Out lines.
	 */
	const char* GetNewButtonString(void);

	/* Returns the state of the Data Available line. TRUE (high)
	 * means a button is being pressed and FALSE (low)
	 * means that no button is being pressed.
	 * The data available line goes low
	 * once the user takes their finger off a button.
	 */
	unsigned char ButtonPressed(void);

	/* Returns the value on the encoder DO A, DO B, DO C, and DO D lines.
	 * It varies from 0-15.
	 * This method does read the Data out lines.
	*/
	unsigned char GetNewButtonNumber(void);

	/* Returns the value on the encoder DO A, DO B, DO C,and DO D lines.
	 * It varies from 0-15.
	 * This method does not read the Data out lines.
	 */
	unsigned char GetLastButtonNumber(void);
};

#endif
