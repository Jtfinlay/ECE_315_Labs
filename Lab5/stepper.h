#ifndef STEPPER_H_
#define STEPPER_H_

/********************** GUIDE TO USING Stepper CLASS *****************************
 *
 * !!! NOTE ETPU MUST BE SET UP FOR SM FUNCTIONS !!!
 * check <FS_ETPU_SM_FUNCTION_NUMBER> in <etpu_sm_auto.h> has the proper
 * function number as shown in the function set.
 * The function set is chosen in <ETPUinit.cpp>. Only <etpu_set1.h> has
 * SM function in it, at location 8.
 * Run NBEclipse->Rebuild system files if you change anything
 *
 * 1) Setup hardware as seen in the schematics (or equivalent)
 * 2) Ensure SM_MASTER_CHANNEL is defined for the pin you are using in pinmap.h
 * 3) Create a global object of the Stepper (for use in interrupts)
 * 	i.e. Stepper myStepper(SM_MASTER_CHANNEL, SM_ACCEL_TABLE_SIZE);
 * 4) Initialize etpu: eTPUInit();
 * 5) run Init();
 * 	i.e. 	myStepper.Init(
		ECE315_ETPU_SM_FULL_STEP_MODE,
		SM_MAX_PERIOD,
		SM_MIN_PERIOD);
	6) use Step() functions to turn Stepper.
	*/

#define SM_ACCEL_TABLE_SIZE     (50)
#define SM_MASTER_CHANNEL		(12)

#define STEPS_PER_REV_FULL_STEP	(100)
#define STEPS_PER_REV_HALF_STEP	(200)
#define SM_MIN_PERIOD			(1)
#define SM_MAX_PERIOD			(0x007fffff) //23 bits
#define HIGH 					1
#define LOW						0

#define ECE315_ETPU_SM_FULL_STEP	FS_ETPU_SM_2PHASE_FULL_STEP

//Force x to be in the range of: a < x < b
#define PutInRange(x,a,b) \
	if (x < (a))		x = (a); \
	else if (x > (b))	x = (b);

//convert a and b to doubles, then multiply or divide them, and return a double
#define D_Div(a,b) ( (double)( ((double)(a)) / ((double)(b)) ) )
#define D_Mul(a,b) ( (double)( ((double)(a)) * ((double)(b)) ) )

class Stepper {

protected:
	int master_channel;
	unsigned short *my_accel_tbl;
	int table_size;
	int my_output_mode;
	int steps_per_rev;

	//calculate my_accel_tbl based on start and slew period
	void build_table(unsigned long start, unsigned long slew);

public:

	//***** Initialization Functions *****/
	Stepper(int mstr_channel, int accel_size);

	void Init(int mode, unsigned long start, unsigned long slew);

	/****** Change Value Functions *****/

	//new desired position = steps + current position
	void Step(int steps);

	//Create a new acceleration table
	void NewAccelTable();

	// Use this function to convert rpm to period ticks that
	// the tpu functions require
	unsigned int ConvertRPMToPeriodTicks(unsigned int rpm);

	unsigned int SetStartPeriodUsingRPM(unsigned int rpm);

	unsigned int SetSlewPeriodUsingRPM(unsigned int rpm);

	void Stop(void);

	/****** Get Motor Value Functions *****/
	signed int GetCurrentPosition(void);

	signed int GetDesiredPosition(void);

	unsigned int GetSlewPeriod(void);

	unsigned int GetStartPeriod(void);

	unsigned int SetStartPeriod(unsigned int start);

	unsigned int SetSlewPeriod(unsigned int slew);

};


#endif /*STEPPER_H_*/
