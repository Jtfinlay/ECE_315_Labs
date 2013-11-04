/*
 * Modified on 27th May 2013
 * nancy.minderman@ualberta.ca
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <startnet.h>
#include <udp.h>
#include <string.h>
#include "services_udp.h"

// Task Stacks for our UDP tasks
asm( " .align 4 " );
DWORD UdpNumberStack[USER_TASK_STK_SIZE] __attribute__( ( aligned( 4 ) ) );
DWORD UdpCaesarStack[USER_TASK_STK_SIZE] __attribute__( ( aligned( 4 ) ) );

/* Name: StartNumberServer
 * Description: Creates the task that implements the Number server.
 * Actual task code is in UdpNumberTask
 * Inputs: none
 * Outputs: none
 */
void StartNumberServer(void) {

	BYTE err = OS_NO_ERR;


	err = OSTaskCreatewName( UdpNumberTask,
					(void *) NUMBER_PORT,
					&UdpNumberStack[USER_TASK_STK_SIZE],
					UdpNumberStack,
					UDP_NUMBER_PRIO, "Number Task");

	if (err != OS_NO_ERR) {
		iprintf("StartNumberServer: Wow Number Server Failed\n");
	}

}

/* Name: UdpNumberTask
 * Description: Implements an Number server that returns all text sent to it.
 * Inputs: void * pd contains the port to listen on
 * Outputs: none
 */
void UdpNumberTask( void *pd )
{
   int port = ( int ) pd;
   const char number_str[] = { "0123456789"};
   iprintf( "UdpNumberTask monitoring port %d\r\n", port );

   // Create a UDP socket for receiving
   int UdpFd = CreateRxUdpSocket( port );
   if ( UdpFd <= 0 )
   {
      iprintf("UdpNumberTask: Error Creating UDP Listen Socket: %d\r\n", UdpFd);
      while (1)
         OSTimeDly(TICKS_PER_SECOND);
   }
   else
   {
      iprintf( "Listening for UDP packets on port %d\r\n", port );
   }

   while (1)
   {
      IPADDR SrcIpAddr;  // UDP packet source IP address
      WORD   LocalPort;  // Port number UDP packet was sent to
      WORD   SrcPort;    // UDP packet source port number
      char   buffer[MAX_UDP_BUF_LEN+1];

      int len = recvfrom( UdpFd, (BYTE *)buffer, MAX_UDP_BUF_LEN, &SrcIpAddr, &LocalPort, &SrcPort );
      buffer[len] = '\0';

      iprintf( "\r\nReceived a UDP packet with %d bytes from :", len );
      ShowIP( SrcIpAddr );
      iprintf( "\r\n%s\r\n", buffer );

      // Now send short number string back
      {
      	UDPPacket pkt;
      	pkt.SetSourcePort(LocalPort);
      	pkt.SetDestinationPort(SrcPort);
      	pkt.AddData((BYTE *) number_str, (WORD) strlen(number_str));
      	pkt.AddDataByte(0);
      	pkt.Send( SrcIpAddr);
      }
   }
}

/* Name: StartCaesarServer
 * Description: Creates the task that implements the Caesar server
 * Actual task code is in UdpCaesarTask
 * Inputs: none
 * Outputs: none
 */
void StartCaesarServer(void) {

	BYTE err = OS_NO_ERR;


	err = OSTaskCreatewName(  UdpCaesarTask,
					(void *) CAESAR_PORT,
					&UdpCaesarStack[USER_TASK_STK_SIZE],
					UdpCaesarStack,
					UDP_CAESAR_PRIO, "Caesar Task");

	if (err != OS_NO_ERR) {
		iprintf("StartCaesarServer:Wow Creating Caesar Failed\n");
	}

}

/* Name: UdpCaesarTask
 * Description: Implements a Caesar server that returns the Caesar encrypted version.
 * A caesar code is a simple rotational encryption algorithm that supposedly originated
 * in ancient Rome. This implementation uses the ROT13 method. A becomes N, B become O, etc
 *
 * Inputs: void *pd contains the port to listen on.
 * Outputs: none
 */
void UdpCaesarTask( void *pd )
{
   int port = ( int ) pd;

   char 	buffer[MAX_UDP_BUF_LEN+1];
   char 	Caesar_str[MAX_UDP_BUF_LEN+1];

   // Create a UDP socket for receiving
   int UdpFd = CreateRxUdpSocket( port );
   if ( UdpFd <= 0 )
   {
      iprintf("UdpCaesarTask: Error Creating UDP Listen Socket: %d\r\n", UdpFd);
      while (1)
         OSTimeDly(TICKS_PER_SECOND);
   }
   else
   {
      iprintf( "Listening for UDP packets on port %d\r\n", port );
   }

   while (1)
   {
      IPADDR SrcIpAddr;  // UDP packet source IP address
      WORD   LocalPort;  // Port number UDP packet was sent to
      WORD   SrcPort;    // UDP packet source port number


      int len = recvfrom( UdpFd, (BYTE *)buffer, MAX_UDP_BUF_LEN, &SrcIpAddr, &LocalPort, &SrcPort );

      // set both strings to be the same length
      buffer[len] = '\0';
      Caesar_str[len] = '\0';

      {
      	UDPPacket pkt;
      	pkt.SetSourcePort(LocalPort);
      	pkt.SetDestinationPort(SrcPort);
      	pkt.AddData((BYTE *)"ROT13 Conversion: ", (WORD) strlen("ROT13 Conversion: "));
      	CalculateROT13Value(buffer, Caesar_str, MAX_UDP_BUF_LEN );
      	pkt.AddData((BYTE *) Caesar_str, (WORD) strlen(Caesar_str));
      	pkt.AddData((BYTE *) "\r\n", (WORD) strlen("\r\n") );
      	pkt.AddDataByte(0);
      	pkt.Send( SrcIpAddr);
      }
   }
}
/* Name:CalculateROT13Value
 * Description: This routine calculates the ROT13 value for a string with a defined length.
 * Please note that only letters are translated. Non-letters are left as is. A terminating null
 * is not appended to the new string.
 * Inputs: 	char * orig is the original character string.
 * 			char * caesar is the allocated string that will hold the translated
 * 				string
 * 			length is the length of the original string excluding the terminating null.
 *
 * Outputs: caesar string passed in by reference is filled in. non-letters are copied and no
 * 			terminating null is appended.
 */
void CalculateROT13Value(char * orig, char * caesar, char length){
		int i = 0;
		const char caes_lookup[]={"NOPQRSTUVWXYZABCDEFGHIJKLMnopqrstuvwxyzabcdefghijklm"};

		while ( (orig[i] != '\0') && (i <= length) ) {

			switch(orig[i]) {
				case 'A' ... 'Z':
					caesar[i] = caes_lookup[(orig[i] - 'A')];
				break;
				case 'a' ... 'z':
					/* the lookup index needs to be indexed to the lower case letters
					 * hence the 26, 'A' is at index 0 and caes_lookup[0] is 'N'
					 * 'a' is at index 26 and caes_lookup[26] is 'n'
					 */
					caesar[i] = caes_lookup[(orig[i] - 'a' + 26)];
				break;
				default:
					caesar[i] = orig[i];
			};
			i++;
	}
}
