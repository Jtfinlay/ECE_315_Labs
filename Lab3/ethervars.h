/* $Revision: 1.1 $ */ 
#ifndef _ETHER_VARS_H
#define _EHTER_VARS_H

// PHY ID Register #2 bits with the last 4 revision number bits being dont cares
#define MICREL_ID   (0x1610) 
#define DAVICOM_ID  (0xB880)
#define AMD_ID      (0x5610)
#define NATIONAL_ID (0x5C90)

#if ( defined CB34EX || defined MOD5234 )
#define PHY_MICREL
#define CORE70
#endif

#ifdef MOD5270
#define PHY_DAVICOM
#define CORE70
#endif

#ifdef SB70
#define PHY_DAVICOM
#define CORE70
#endif

#ifdef PK70
#define PHY_MICREL
#define CORE70
#endif

#ifdef MOD5282
#define DAVICOM_AND_MICREL_PHY
#define PHY_DAVICOM
#define PHY_MICREL
#define CORE70
#endif

#ifdef MOD5272
#define DAVICOM_AND_MICREL_PHY
#define PHY_DAVICOM
#define PHY_MICREL
#define CORE72
#endif

#ifdef SB72EX
#define PHY_DAVICOM
#define CORE72
#endif

#ifdef SB72
#define PHY_AMD
#define CORE72
#endif

#ifdef MCF5208
#define PHY_NATIONAL
#define CORE70
#endif

#define IP_20BYTE_ID  (0x4500)
#define ETHER_TYPE_IP (0x0800)
#define ETHER_TYPE_ARP (0x0806)

#define NUM_TX_BD (3)
#define NUM_RX_BD (10)

extern volatile int nTx;
extern volatile int nRx;

extern volatile EtherBD TxBD[NUM_TX_BD] __attribute__( ( aligned( 16 ) ) );
extern volatile EtherBD RxBD[NUM_RX_BD] __attribute__( ( aligned( 16 ) ) );;

extern volatile PoolPtr RxBDpp[NUM_RX_BD];
extern volatile PoolPtr TxBDpp[NUM_TX_BD];

extern volatile BYTE RxBDFlag[NUM_RX_BD];
extern volatile BYTE TxBDFlag[NUM_RX_BD];

extern int last_rxbd;

extern DWORD LastIsrStatus;
extern DWORD RxPosts;
extern DWORD TxcPosts;

extern BOOL bEthLink; /* do we have link? */
extern BOOL bEthDuplex; /* Are we setup for duplex? */
extern BOOL bEth100Mb; /* Are we 100Mb? */
extern BOOL AutoNeg;

#ifndef _DEBUG
extern BYTE PHY_addr;
extern WORD PHY_ID;
#endif

#endif
