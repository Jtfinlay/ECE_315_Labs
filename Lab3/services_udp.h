/*
 * services_udp.h
 *
 *  Created on: Oct 18, 2012
 *      Author: Nancy Minderman
 *      		nancy.minderman@ualberta.ca
 *
 *      Modified for the fall 2013 term.
 */

#ifndef SERVICES_UDP_H_
#define SERVICES_UDP_H_

#define UDP_NUMBER_PRIO 			MAIN_PRIO + 2
#define UDP_CAESAR_PRIO				MAIN_PRIO + 3
#define NUMBER_PORT					4
#define CAESAR_PORT					10
#define	MAX_UDP_BUF_LEN 			20

extern "C"
{
	void UdpNumberTask( void *pd );
	void StartNumberServer(void);
	void UdpCaesarTask(void *pd);
	void StartCaesarServer(void);
	void CalculateROT13Value(char * orig, char * caesar, char length);
}

#endif /* SERVICES_UDP_H_ */
