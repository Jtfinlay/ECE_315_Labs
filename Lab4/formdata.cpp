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


/* Name: FormData Constructor
 * Description: Empty Constructor for the class
 * Inputs: 	none
 * Outputs: none
 */
FormData::FormData() {
	// TODO Auto-generated constructor stub

	if (OSSemInit(&form_sem,1) == OS_SEM_ERR)
		iprintf("Error in initializing semaphore.\n");

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

	this->pend();
	long iRPM = strtol(rpm,NULL,10);
	if (iRPM <= SM_MAX_RPM_PHYSICAL &&
			iRPM >= SM_MIN_RPM_PHYSICAL) {
		int_maxrpm = iRPM;
		this->post();
		return FORM_OK;
	}
	this->post();
	return FORM_ERROR;
}

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

	this->pend();
	long iRPM = strtol(rpm,NULL,10);
	if (iRPM <= SM_MAX_RPM_PHYSICAL &&
			iRPM >= SM_MIN_RPM_PHYSICAL) {
		int_minrpm = iRPM;
		this->post();

		return FORM_OK;
	}
	this->post();
	return FORM_ERROR;
}

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
	this->pend();
	long iSteps = strtol(steps,NULL,10);
	if(iSteps < 0 || iSteps > INT_MAX) {
		this->post();
		return FORM_ERROR;
	}this->int_steps = iSteps;
	this->post();
	return FORM_OK;

}

BYTE FormData::SetRotations(char * rot) {
	this->pend();
	long iRot = strtol(rot, NULL, 10);
	if(iRot <= 1 || iRot >= MAX_ROTATIONS) {
		this->post();
		return FORM_ERROR;
	}
	this->int_rotations = iRot;
	this->post();
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
	this->pend();
	if(strcmp("CW", dir)) {
		this->direction = CW;

		this->post();

		return FORM_OK;
	}
	else if(strcmp("CCW", dir)) {
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

void FormData::pend() {

	if (OSSemPend(&form_sem, 0) == OS_TIMEOUT)
			iprintf("Timeout waiting for Semaphore\n");
}

void FormData::post() {

	if (OSSemPost(&form_sem) == OS_SEM_OVF)
			iprintf("Error posting to Semaphore\n");
}
