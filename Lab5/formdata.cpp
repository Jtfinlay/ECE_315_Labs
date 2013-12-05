/*
 * FormData.cpp
 *
 *  Created on: Oct 29, 2009
 *      Author: Nancy Minderman
 *
 *  Modified on: Nov 19, 2013
 *      Author: James Finlay, Andrew Fontaine
 */

#include "formdata.h"
#include <stdlib.h>
#include <string.h>
#include <ucos.h>
#include <stdio.h>
#include <ctype.h>
#include "motorconstants.h"


/* Name: FormData Constructor
 * Description: Empty Constructor for the class
 * Inputs: 	none
 * Outputs: none
 */
FormData::FormData() {
	// TODO Auto-generated constructor stub


}

/* Name: FormData Destructor
 * Description: Empty Destructor for the class
 * Inputs:	none
 * Outputs: none
 */
FormData::~FormData() {
	// TODO Auto-generated destructor stub
}

/* Name: Set Maximum RPM
 * Description: Set the maximum rpm of the motor
 * Inputs: RPM as string
 * Outputs: OK or ERROR
 */
BYTE FormData::SetMaxRPM(char * rpm) {

	this->pend();
	char *err;
	long iRPM = strtol(rpm, &err, BASE_10);
	if(*err != '\0') {
		this->post();
		int_maxrpm = SM_MIN_RPM_PHYSICAL;
		return FORM_ERROR;
	}
	if (iRPM <= SM_MAX_RPM_PHYSICAL &&
			iRPM >= SM_MIN_RPM_PHYSICAL) {
		int_maxrpm = iRPM;
		this->post();
		return FORM_OK;
	}
	this->post();
	return FORM_ERROR;
}
/* Name: Set Maximum RPM
 * Description: Set the maximum rpm of the motor
 * Inputs: RPM as integer
 * Outputs: OK or ERROR
 */
BYTE FormData::SetMaxRPM(int iRPM) {

	this->pend();
	if (iRPM <= SM_MAX_RPM_PHYSICAL &&
			iRPM >= SM_MIN_RPM_PHYSICAL) {
		int_maxrpm = iRPM;
		this->post();
		return FORM_OK;
	}
	this->post();
	return FORM_ERROR;
}
/* Name: Get Maximum RPM
 * Description: Get the maximum rpm of the motor
 * Inputs:
 * Outputs: The Maximum RPM
 */
int  FormData::GetMaxRPM(void){
	return int_maxrpm;
}

/* Name: Set Minimum RPM
 * Description: Set the minimum rpm of the motor
 * Inputs: RPM as string
 * Outputs: OK or ERROR
 */
BYTE FormData::SetMinRPM(char * rpm) {

	this->pend();
	char *err;
	long iRPM = strtol(rpm, &err, BASE_10);
	if(*err != '\0') {
		this->post();
		int_minrpm = SM_MAX_RPM_PHYSICAL;
		return FORM_ERROR;
	}
	if (iRPM <= SM_MAX_RPM_PHYSICAL &&
			iRPM >= SM_MIN_RPM_PHYSICAL) {
		int_minrpm = iRPM;
		this->post();

		return FORM_OK;
	}
	this->post();
	return FORM_ERROR;
}
/* Name: Set Minimum RPM
 * Description: Set the minimum rpm of the motor
 * Inputs: RPM as integer
 * Outputs: OK or ERROR
 */
BYTE FormData::SetMinRPM(int iRPM) {
	this->pend();
	if (iRPM <= SM_MAX_RPM_PHYSICAL &&
			iRPM >= SM_MIN_RPM_PHYSICAL) {
		int_minrpm = iRPM;
		this->post();

		return FORM_OK;
	}
	this->post();
	return FORM_ERROR;
}

/* Name: Get Minimum RPM
 * Description: Get the minimum rpm of the motor
 * Inputs:
 * Outputs: Minimum RPM as integer
 */
int  FormData::GetMinRPM(void) {
	return int_minrpm;
}

/* Name: Set Steps
 * Description: Set the step-size of the motor
 * Inputs: Steps as string
 * Outputs: OK or ERROR
 */
BYTE FormData::SetSteps(char * steps) {
	this->pend();
	char *err;
	long iSteps = strtol(steps,&err,BASE_10);
	if(*err != '\0') {
		this->post();
		return FORM_ERROR;
	}
	if(iSteps < 0 || iSteps > INT_MAX) {
		this->post();
		return FORM_ERROR;
	}this->int_steps = iSteps;
	this->post();
	return FORM_OK;

}
/* Name: Set Rotations
 * Description: Set the rotations of the motor
 * Inputs: Rotations as string
 * Outputs: OK or ERROR
 */
BYTE FormData::SetRotations(char * rot) {
	this->pend();
	char *err;
	long iRot = strtol(rot, &err, BASE_10);
	if(*err != '\0') {
		this->post();
		return FORM_ERROR;
	}
	if(iRot < 1 || iRot > MAX_ROTATIONS) {
		this->post();
		return FORM_ERROR;
	}
	this->int_rotations = iRot;
	this->post();
	return FORM_OK;
}

/* Name: Get Steps size
 * Description: Get the size of the step
 * Inputs:
 * Outputs: Steps as integer
 */
int FormData::GetSteps (void) {
	return int_steps;
}

/* Name: Set Direction
 * Description: Set the direction of rotation
 * Inputs: Direction as string
 * Outputs: OK or ERROR
 */
BYTE FormData::SetDirection(char * dir){
	this->pend();
	if(strcmp("CW", dir) == 0) {
		this->direction = CW;

		this->post();

		return FORM_OK;
	}
	else if(strcmp("CCW", dir) == 0) {
		this->direction = CCW;

		this->post();

		return FORM_OK;
	}
	else if(strcmp("STOP", dir)) {
		this->direction = STOP;

		this->post();

		return FORM_OK;
	}
	else if(strcmp("UNSET", dir)) {
		this->direction = UNSET;

		this->post();

		return FORM_OK;
	}
	this->post();
	return FORM_ERROR;
}

/* Name: Get Rotations
 * Description: Get the rotations of the motor
 * Inputs:
 * Outputs: Rotations as integer
 */
int FormData::GetRotations(void) {
	return int_rotations;
}
/* Name: Get Direction
 * Description: Get the direction of rotation
 * Inputs:
 * Outputs: Direction as BYTE
 */
BYTE FormData::GetDirection(void){
	return direction;
}
/* Name: Get Mode
 * Description: Get the mode
 * Inputs:
 * Outputs: Mode as a byte
 */
BYTE FormData::GetMode(void){
	return mode;
}
/* Name: Get Error max RPM
 * Description: Get whether max rpm has errored
 * Inputs:
 * Outputs: Error as byte
 */
BYTE FormData::GetErrorMaxRPM() {
	return error_maxrpm;
}
/* Name: Set Error max RPM
 * Description: Set whether max rpm has errored
 * Inputs: Error as byte
 * Outputs:
 */
void FormData::SetErrorMaxRPM(BYTE error) {
	error_maxrpm = error;
}
/* Name: Get Error min RPM
 * Description: Get whether min rpm has errored
 * Inputs:
 * Outputs: Error as byte
 */
BYTE FormData::GetErrorMinRPM() {
	return error_minrpm;
}
/* Name: Set Error min RPM
 * Description: set whether max rpm has errored
 * Inputs: Error as byte
 * Outputs:
 */
void FormData::SetErrorMinRPM(BYTE error) {
	error_minrpm = error;
}
/* Name: Get Error Rotations
 * Description: Get whether rotations has errored
 * Inputs:
 * Outputs: Error as byte
 */
BYTE FormData::GetErrorRotations() {
	return error_rotations;
}
/* Name: Set Error max RPM
 * Description: Set whether rotations has errored
 * Inputs:  Error as byte
 * Outputs:
 */
void FormData::SetErrorRotations(BYTE error) {
	error_rotations = error;
}

/* Name: FormData initialization
 * Description: initializes FormData to appropriate values
 * Inputs: The mode of the motor as BYTE
 * Outputs: FORM_OK
 */
BYTE FormData::Init(BYTE motor_mode){
	direction = UNSET;
	mode = motor_mode;
	int_maxrpm = 0;
	int_minrpm = 0;
	int_steps = 0;
	error_maxrpm = FORM_NO_DATA;
	error_minrpm = FORM_NO_DATA;
	error_rotations = FORM_NO_DATA;
	if (OSSemInit(&form_sem,1) == OS_SEM_ERR)
		iprintf("Error in initializing semaphore.\n");
	return FORM_OK;
}
/* Name: Pend
 * Description: Sets binary semaphore to pend
 * Inputs:
 * Outputs:
 */
void FormData::pend() {

	if (OSSemPend(&form_sem, 0) == OS_TIMEOUT)
		iprintf("Timeout waiting for Semaphore\n");
}
/* Name: Post
 * Description: Sets binary semaphore to post
 * Inputs:
 * Outputs:
 */
void FormData::post() {

	if (OSSemPost(&form_sem) == OS_SEM_OVF)
		iprintf("Error posting to Semaphore\n");
}
