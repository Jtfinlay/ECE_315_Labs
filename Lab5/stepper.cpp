#include "predef.h"
#include <stdio.h>
#include <ctype.h>
#include <startnet.h>
#include <autoupdate.h>
#include <dhcpclient.h>
#include <smarttrap.h>
#include <taskmon.h>
#include <NetworkDebug.h>

#include <ucos.h>
#include <cfinter.h>

#include <eTPU.h>
#include <ETPUInit.h>
#include <eTPU_sm.h>

#include <pins.h>
#include <math.h>

#include <sim5234.h>
#include "stepper.h"
#include "motorconstants.h"


/* Name:
 * Description:
 * Inputs:
 * Outputs:
 */
Stepper::Stepper(int mstr_channel, int accel_size)
{
	table_size = accel_size;
	master_channel = mstr_channel;
};

/* Name:
 * Description:
 * Inputs:
 * Outputs:
 */
void Stepper::Init(int mode, unsigned long start, unsigned long slew)
{
	//Ensure Start and Slew Periods are in the correct range
	PutInRange(slew,  SM_MIN_PERIOD, SM_MAX_PERIOD);
	PutInRange(start, SM_MIN_PERIOD, SM_MAX_PERIOD);

	my_output_mode = mode;

	//Build the Acceleration table
	my_accel_tbl = new unsigned short [table_size];
	build_table(start, slew);

	/************** Full-Step Mode ***************/
	if (my_output_mode == ECE315_ETPU_SM_FULL_STEP_MODE)
	{
		//Define steps per revolution for FULL STEP
		steps_per_rev = STEPS_PER_REV_FULL_STEP;

		//Initialize the two drive signals
		if (fs_etpu_sm_init(
			master_channel,				// Stepper Stepper master channel
		    ECE315_ETPU_SM_FULL_STEP, // Use two phase full step configuration
		    0,						// Motor Start Position
		    start,					// Start Period
		    slew,					// Slew Period
			my_accel_tbl, 			// Pointer to acceleration table
			table_size				// Size of Acceleration table
		) == FS_ETPU_ERROR_MALLOC) iprintf ("MALLOC FAIL FULL!!!");

		/* Pull enable lines high permanently */
		/* J2 pins for the two enables are 19 and 20
		 * consult the lab 5 schematic to verify*/
		J2[master_channel+7] = HIGH;
		J2[master_channel+8] = HIGH;


		//Enable the Drive Lines
		fs_etpu_sm_enable(master_channel, FS_ETPU_PRIORITY_LOW);
	}
	/************** Half-Step Mode ***************/
	else {
		steps_per_rev = STEPS_PER_REV_HALF_STEP;

		if(fs_etpu_sm_init(
				master_channel +2,
				ECE315_ETPU_SM_2PHASE_HALF_STEP_ENA,
				0,
				start,
				slew,
				my_accel_tbl,
				table_size) == FS_ETPU_ERROR_MALLOC) iprintf("MALLOC FAIL FULL!!!");

		fs_etpu_sm_enable(master_channel+2,FS_ETPU_PRIORITY_LOW);

		if(fs_etpu_sm_init(
						master_channel,
						ECE315_ETPU_SM_2PHASE_HALF_STEP_DRIVE,
						0,
						start,
						slew,
						my_accel_tbl,
						table_size) == FS_ETPU_ERROR_MALLOC) iprintf("MALLOC FAIL FULL!!!");

		fs_etpu_sm_enable(master_channel,FS_ETPU_PRIORITY_LOW);
	}
	delete 	my_accel_tbl;
}

//Remake the acceleration table, without having to use more p ram
/* Name:
 * Description:
 * Inputs:
 * Outputs:
 */
void Stepper::NewAccelTable()
{
	my_accel_tbl = new unsigned short [table_size];
	build_table(GetStartPeriod(), GetSlewPeriod());

	fs_etpu_sm_table(master_channel, my_accel_tbl);

	if(my_output_mode != ECE315_ETPU_SM_FULL_STEP_MODE)
		fs_etpu_sm_table(master_channel+2, my_accel_tbl);

	delete 	my_accel_tbl;
}

/* Name:
 * Description:
 * Inputs:
 * Outputs:
 */
void Stepper::Step(int steps)
{
	// Step a certain number of steps
	fs_etpu_sm_set_dp(master_channel, fs_etpu_sm_get_cp(master_channel) + steps);

	if(my_output_mode != ECE315_ETPU_SM_FULL_STEP_MODE)
		fs_etpu_sm_set_dp(master_channel+2, fs_etpu_sm_get_cp(master_channel+1) + steps);
}

/* Name:
 * Description:
 * Inputs:
 * Outputs:
 */
unsigned int Stepper::SetSlewPeriod(unsigned int slew)
{
	//ensure period is within range
	iprintf("slew: %i\n",slew);
	PutInRange(slew, SM_MIN_PERIOD, SM_MAX_PERIOD);
	iprintf("slew: %i\n",slew);

	//set slew period, with 24 bits max
	fs_etpu_sm_set_sp(master_channel, slew);
	if(my_output_mode != ECE315_ETPU_SM_FULL_STEP_MODE)
		fs_etpu_sm_set_sp(master_channel+2, slew);

	NewAccelTable();

	return slew;

}
/* Name:
 * Description:
 * Inputs:
 * Outputs:
 */
unsigned int Stepper::SetStartPeriod(unsigned int start)
{
	//ensure period is within range
	iprintf("start: %i\n",start);
	PutInRange(start, SM_MIN_PERIOD, SM_MAX_PERIOD);
	iprintf("start: %i\n",start);

	//set start period, with 24 bits max
	fs_etpu_sm_set_st(master_channel, start);
	if(my_output_mode != ECE315_ETPU_SM_FULL_STEP_MODE)
		fs_etpu_sm_set_st(master_channel + 2, start);

	NewAccelTable();

	return start;
}

/* Create a new acceleration table.
 * The period is calculated to be the great of:
 * 		Start_Period * my_accel_tbl[i] / 0xFFFF
 * 		and
 * 		Slew_Period
 *
 * This function will calculate so the the frequency linearly increased
 * from Tstart to Tslew, to create a constant acceleration.
 * my_accel_tbl[i] = (0xFFFF * Tslew)
 * 					-------------------------
 *			(Tslew + (Tstart - Tslew)* i / table_size)
 */
/* Name:
 * Description:
 * Inputs:
 * Outputs:
 */
void Stepper::build_table(unsigned long start, unsigned long slew)
{
	double val;

	for (int i = 0; i < table_size; i++){
		val = D_Div( D_Mul(0xFFFF,slew) ,slew+ D_Mul( (start-slew),D_Div(i+1,table_size) ) );
		my_accel_tbl[i] = (unsigned short)(val);
		//iprintf("%i) %#hx \n",i, my_accel_tbl[i]);
	}
}

/****** Get Value Functions *****/
//get current position
/* Name:
 * Description:
 * Inputs:
 * Outputs:
 */
signed int Stepper::GetCurrentPosition(void)
{
	return fs_etpu_sm_get_cp(master_channel);
}

//get desired position
/* Name:
 * Description:
 * Inputs:
 * Outputs:
 */
signed int Stepper::GetDesiredPosition(void)
{
	return fs_etpu_sm_get_dp(master_channel);
}

//Get Slew Period
/* Name:
 * Description:
 * Inputs:
 * Outputs:
 */
unsigned int Stepper::GetSlewPeriod(void)
{
	return fs_etpu_sm_get_sp(master_channel);
}


/* Name:
 * Description:
 * Inputs:
 * Outputs:
 */
unsigned int Stepper::GetStartPeriod(void)
{
	return fs_etpu_sm_get_st(master_channel);
}

/* Name:
 * Description:
 * Inputs:
 * Outputs:
 */
unsigned int Stepper::ConvertRPMToPeriodTicks(unsigned int rpm) {
	unsigned int ticks = 0;

	ticks = 60 * ( etpu_a_tcr1_freq / ( rpm * steps_per_rev));
	iprintf( "RPM: %d\n", rpm);
	iprintf ("Ticks after conversion: %d\n", ticks);
	return ticks;
}

/* Name:
 * Description:
 * Inputs:
 * Outputs:
 */
unsigned int Stepper::SetStartPeriodUsingRPM(unsigned int rpm) {
	unsigned int ticks = 0;
	ticks = ConvertRPMToPeriodTicks(rpm);
	SetStartPeriod(ticks);
	return ticks;
}

/* Name:
 * Description:
 * Inputs:
 * Outputs:
 */
unsigned int Stepper::SetSlewPeriodUsingRPM(unsigned int rpm){
	unsigned int ticks = 0;
	ticks = ConvertRPMToPeriodTicks(rpm);
	SetSlewPeriod(ticks);
	return ticks;
}

/* Name:
 * Description:
 * Inputs:
 * Outputs:
 */
void Stepper::Stop(void ) {
	iprintf("Stop!\n");
	fs_etpu_sm_disable( master_channel, FS_ETPU_SM_DISABLE_LOW);
	fs_etpu_sm_enable( master_channel, FS_ETPU_PRIORITY_LOW);
	if(my_output_mode != ECE315_ETPU_SM_FULL_STEP_MODE) {
		fs_etpu_sm_disable( master_channel +2, FS_ETPU_SM_DISABLE_LOW);
		fs_etpu_sm_enable( master_channel +2, FS_ETPU_PRIORITY_LOW);
	}

}

