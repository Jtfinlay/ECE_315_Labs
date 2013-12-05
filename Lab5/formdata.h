/*
 * FormData.h
 *
 *  Created on: Oct 29, 2009
 *      Author: Nancy Minderman
 */

#ifndef FORMDATA_H_
#define FORMDATA_H_

#include <basictypes.h>
#include <ucos.h>

#define FORM_NO_DATA 	0xFF
#define FORM_ERROR 		0xFE
#define FORM_OK			0

#define BASE_10	10

enum dir { 	UNSET = 0,
		CCW = 1,
		CW = 2,
		STOP = 3
};


class FormData {
public:
	FormData();
	virtual ~FormData();
	BYTE SetMaxRPM(char * rpm);
	BYTE SetMaxRPM(int iRPM);
	int  GetMaxRPM(void);
	BYTE SetMinRPM(char * rpm);
	BYTE SetMinRPM(int iRPM);
	int  GetMinRPM(void);
	BYTE SetSteps(char * st);
	int  GetSteps (void);
	BYTE SetRotations(char * rot);
	int  GetRotations(void);
	BYTE SetDirection(char * dir);
	BYTE GetDirection(void);
	BYTE GetMode(void);
	BYTE GetErrorMaxRPM();
	void SetErrorMaxRPM(BYTE error);
	BYTE GetErrorMinRPM();
	void SetErrorMinRPM(BYTE error);
	BYTE GetErrorRotations();
	void SetErrorRotations(BYTE error);
	BYTE Init(BYTE motor_mode);
	void pend();
	void post();
private:
	int  int_maxrpm;
	int  int_minrpm;
	int  int_steps;
	int	 int_rotations;
	enum dir direction;
	BYTE mode;
	BYTE error_maxrpm;
	BYTE error_minrpm;
	BYTE error_rotations;
	OS_SEM form_sem;
};

#endif /* FORMDATA_H_ */
