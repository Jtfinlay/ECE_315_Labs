/*
 * FormData.cpp
 *
 *  Created on: Oct 29, 2009
 *      Author: Nancy Minderman
 */

#include "formdata.h"
#include <stdlib.h>
#include <string.h>
#include <ucos.h>
#include <stdio.h>
#include <ctype.h>
#include "motorconstants.h"

extern OS_SEM form_sem;

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

/* Name:
 * Description:
 * Inputs:
 * Outputs:
 */
BYTE FormData::SetMaxRPM(char * rpm) {

	if (OSSemPend(&form_sem, 0) == OS_TIMEOUT)
		iprintf("Timeout waiting for Semaphore\n");

	long iRPM = strtol(rpm,NULL,10);
	if (iRPM <= SM_MAX_RPM_PHYSICAL &&
			iRPM >= SM_MIN_RPM_PHYSICAL) {
		int_maxrpm = iRPM;
		if (OSSemPost(&form_sem) == OS_SEM_OVF)
			iprintf("Error posting to Semaphore\n");
		return FORM_OK;
	}
	if (OSSemPost(&form_sem) == OS_SEM_OVF)
		iprintf("Error posting to Semaphore\n");
	return FORM_ERROR;
}

BYTE FormData::SetMaxRPM(int iRPM) {

	if (OSSemPend(&form_sem, 0) == OS_TIMEOUT)
		iprintf("Timeout waiting for Semaphore\n");

	if (iRPM <= SM_MAX_RPM_PHYSICAL &&
			iRPM >= SM_MIN_RPM_PHYSICAL) {
		int_maxrpm = iRPM;
		if (OSSemPost(&form_sem) == OS_SEM_OVF)
			iprintf("Error posting to Semaphore\n");
		return FORM_OK;
	}
	if (OSSemPost(&form_sem) == OS_SEM_OVF)
		iprintf("Error posting to Semaphore\n");
	return FORM_ERROR;
}
/* Name:
 * Description:
 * Inputs:
 * Outputs:
 */
int  FormData::GetMaxRPM(void){
	return int_maxrpm;
}

/* Name:
 * Description:
 * Inputs:
 * Outputs:
 */
BYTE FormData::SetMinRPM(char * rpm) {

	if (OSSemPend(&form_sem, 0) == OS_TIMEOUT)
		iprintf("Timeout waiting for Semaphore\n");

	long iRPM = strtol(rpm,NULL,10);
	if (iRPM <= SM_MAX_RPM_PHYSICAL &&
			iRPM >= SM_MIN_RPM_PHYSICAL) {
		int_minrpm = iRPM;
		if (OSSemPost(&form_sem) == OS_SEM_OVF)
			iprintf("Error posting to Semaphore\n");

		return FORM_OK;
	}
	if (OSSemPost(&form_sem) == OS_SEM_OVF)
		iprintf("Error posting to Semaphore\n");
	return FORM_ERROR;
}

BYTE FormData::SetMinRPM(int iRPM) {
	if (OSSemPend(&form_sem, 0) == OS_TIMEOUT)
		iprintf("Timeout waiting for Semaphore\n");

	if (iRPM <= SM_MAX_RPM_PHYSICAL &&
			iRPM >= SM_MIN_RPM_PHYSICAL) {
		int_minrpm = iRPM;
		if (OSSemPost(&form_sem) == OS_SEM_OVF)
			iprintf("Error posting to Semaphore\n");

		return FORM_OK;
	}
	if (OSSemPost(&form_sem) == OS_SEM_OVF)
		iprintf("Error posting to Semaphore\n");
	return FORM_ERROR;
}

/* Name:
 * Description:
 * Inputs:
 * Outputs:
 */
int  FormData::GetMinRPM(void) {
	return int_minrpm;
}

/* Name:
 * Description:
 * Inputs:
 * Outputs:
 */
BYTE FormData::SetSteps(char * steps) {
	if (OSSemPend(&form_sem, 0) == OS_TIMEOUT)
		iprintf("Timeout waiting for Semaphore\n");

	long iSteps = strtol(steps,NULL,10);
	if(iSteps < 0 || iSteps > INT_MAX) {
		if (OSSemPost(&form_sem) == OS_SEM_OVF)
			iprintf("Error posting to Semaphore\n");
		return FORM_ERROR;
	}this->int_steps = iSteps;
	if (OSSemPost(&form_sem) == OS_SEM_OVF)
		iprintf("Error posting to Semaphore\n");
	return FORM_OK;

}

BYTE FormData::SetRotations(char * rot) {
	if (OSSemPend(&form_sem, 0) == OS_TIMEOUT)
		iprintf("Timeout waiting for Semaphore\n");

	long iRot = strtol(rot, NULL, 10);
	if(iRot <= 1 || iRot >= MAX_ROTATIONS) {
		if (OSSemPost(&form_sem) == OS_SEM_OVF)
			iprintf("Error posting to Semaphore\n");
		return FORM_ERROR;
	}
	this->int_rotations = iRot;
	if (OSSemPost(&form_sem) == OS_SEM_OVF)
		iprintf("Error posting to Semaphore\n");
	return FORM_OK;
}

/* Name:
 * Description:
 * Inputs:
 * Outputs:
 */
int FormData::GetSteps (void) {
	return int_steps;
}

/* Name:
 * Description:
 * Inputs:
 * Outputs:
 */
BYTE FormData::SetDirection(char * dir){
	if (OSSemPend(&form_sem, 0) == OS_TIMEOUT)
		iprintf("Timeout waiting for Semaphore\n");

	if(strcmp("CW", dir)) {
		this->direction = CW;

		if (OSSemPost(&form_sem) == OS_SEM_OVF)
			iprintf("Error posting to Semaphore\n");

		return FORM_OK;
	}
	else if(strcmp("CCW", dir)) {
		this->direction = CCW;

		if (OSSemPost(&form_sem) == OS_SEM_OVF)
			iprintf("Error posting to Semaphore\n");

		return FORM_OK;
	}
	else if(strcmp("STOP", dir)) {
		this->direction = STOP;

		if (OSSemPost(&form_sem) == OS_SEM_OVF)
			iprintf("Error posting to Semaphore\n");

		return FORM_OK;
	}
	else if(strcmp("UNSET", dir)) {
		this->direction = UNSET;

		if (OSSemPost(&form_sem) == OS_SEM_OVF)
			iprintf("Error posting to Semaphore\n");

		return FORM_OK;
	}
	if (OSSemPost(&form_sem) == OS_SEM_OVF)
		iprintf("Error posting to Semaphore\n");
	return FORM_ERROR;
}

int FormData::GetRotations(void) {
	return int_rotations;
}
/* Name:
 * Description:
 * Inputs:
 * Outputs:
 */
BYTE FormData::GetDirection(void){
	return direction;
}

/* Name:
 * Description:
 * Inputs:
 * Outputs:
 */
BYTE FormData::GetMode(void){
	return mode;
}

BYTE FormData::GetErrorMaxRPM() {
	return error_maxrpm;
}

void FormData::SetErrorMaxRPM(BYTE error) {
	error_maxrpm = error;
}

BYTE FormData::GetErrorMinRPM() {
	return error_minrpm;
}

void FormData::SetErrorMinRPM(BYTE error) {
	error_minrpm = error;
}

BYTE FormData::GetErrorRotations() {
	return error_rotations;
}

void FormData::SetErrorRotations(BYTE error) {
	error_rotations = error;
}

/* Name:
 * Description:
 * Inputs:
 * Outputs:
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
	return FORM_OK;
}
