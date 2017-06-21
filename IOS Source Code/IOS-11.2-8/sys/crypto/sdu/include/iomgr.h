/* $Id: iomgr.h,v 1.1.68.1 1996/04/19 14:57:40 che Exp $
 * $Source: /release/112/cvs/Xsys/crypto/sdu/include/iomgr.h,v $
 *------------------------------------------------------------------
 * Cylink Source code: iomgr.h
 *
 * August 1995, Brian Weis
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: iomgr.h,v $
 * Revision 1.1.68.1  1996/04/19  14:57:40  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commits
 *
 * Revision 1.1.2.1  1995/11/28  01:49:39  bew
 * Branch: IosSec_branch
 * Port of newest Cylink SDU code, plus CS cleanup. The SDU code
 * is moved to its original Cylink tree structure.
 *
 * Revision 1.1  1995/11/22  23:22:13  bew
 * Placeholders for new SDU file locations.
 *
 * Revision 1.7  1995/06/07 12:03:05  smackie
 * Fold back Arizona_branch into mainline.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************************
* $Header: /SecureLAN/SDU/Include/IOMGR.H 37    8/14/95 9:43a Tchi 
* DESCRIPTION:  Event Manager header file
*
* Copyright(c)  Cylink Corporation 1994.  All rights reserved.
*******************************************************************************
* $ History: 
*******************************************************************************/

#ifndef IOMGR_H
#define IOMGR_H

/* -------------------------------------------------------------------------
*   Tickles Watchdog so that it does n't reset the system; Should be executed
*   within 5 secs from the previous tickle
* -------------------------------------------------------------------------- */
# include "uc.h"

# ifndef WDOG_DISABLE
# define PokeWdog()  outportb( WDOG_DEVICE, 0) ; /* select the watchdog device */
# else
# define PokeWdog()
# endif

typedef struct {
			uchar   source;
			uchar   event_id;
			ushort  data;
		  } IOMGR_Q_STRUCT;

enum { SNMP_ACTIVE=1, SNMP_DELETE=6  } ;

/* An alarm entry */
typedef struct alarmEntry {
				ulong timeTicks ;
				short eventId ;
				short snmpTaskId ;
				short snmpDelete ;
				char  eventData[34] ;
		  } AlarmEntry ;

enum { MED_LOCKED, MED_UNLOCKED } ;
/* ---------------------- Global vars --------------------------------------- */
# define SYS_STATES_LOW		 2	/* snmp */
# define SYS_STATES_HIGH	 4 
/* careful in changing value - hard coded elsewhere! */
enum { NO_SNMP_ADMIN_OP=0, SDU_INITIALIZATION=1, SDU_OFFLINE=2, SDU_LOCKED=3, SDU_OPERATION=4 } ;
extern int   	sduState ;

# define MAX_ALARMS                 32
struct alarmDbase {
			AlarmEntry alarmTable[MAX_ALARMS] ;
			int alarmBottom ;
			int alarmTop ;
		 } ;

extern struct alarmDbase alarmInfo;

extern IQueue  IOmgr_IQ;   /* IO Manager inter-module queue structure */

extern int medLocked_g ;   /* Referred to by RS 232 Monitor */

extern short unique_event_id [15] [35];

/*-----------------------External Functions-----------------------------------                  */
extern void  iomgr_task( void ); 
extern void  post2IOmanager( uchar source, uchar event, ushort data);
extern void init_unique_event_id ( void );

/* ---------------------------------------------------------------------------
*   Event IDs -- Additions/Deletions need an update in evntmgr.c table; Also
*		any change in the total number of events for each task require another
*		change in evntmgr.c
* --------------------------------------------------------------------------- */
/* ---------------------------------CONNSTUP--------------------------------- */
/*#define CS_PCONN_INDEX_LOST         7 -- ignored(Chi) */
/* Total CS events = 25, keep updating this whenever there is a change */
#define CS_NO_TX_BUF                1
#define CS_NO_PCONN_ENTRY           2
#define CS_PCONN_TIMEOUT            3
#define CS_NNC_EXP                  4
#define CS_NNC_SIG                  5
#define CS_SU_SIG                   6
#define CS_CHALLENGE_ODD            8
#define CS_CHALLENGE_CMP            9
#define CS_NNC_GRPS                10
#define CS_GEN_CHL                 11
#define CS_GEN_DH                  12
#define CS_GEN_SIG                 13
#define CS_ALLOC_HM                14
#define CS_RELEASE_HM              15
#define CS_CY_KEYS                 16
#define CS_SOK_MASK_RNG            17
#define CS_VERIFY_SIG              18
#define CS_NNC_FRM_SIZE            19
#define CS_TRANSMIT                20
#define CS_AUTH_API                21
#define CS_NC_NNC_OUT_SYNC         22
#define CS_CASCADED_CONFIG         23
#define CS_NNC_SIZE                24
#define CS_NVM_PUTREC              25
#define CS_EVT_CONFIG_TB_MISSING   33

/* ----------------------------------------------------------------------------- */
/* EVENT IDs assignments for AUTH_TASK */
/* ----------------------------------------------------------------------------- */
# define NO_CERT_STATE          1  /* NO Certificate State */
# define MAN_CERT_STATE        	2  /* Manufacturing Certificate State */
# define NET_CERT_STATE			3  /* Certificate State */
# define CERT_RENEW_TIME        4  /* Certificate Renewal */
# define CERT_EXPIRE_TIME       5  /* Certificate Expired */
# define MAN_CERT_ERROR    		6  /* Manuf. Cert Data Error */
# define NET_CERT_ERROR    		7  /* Network Cert Data Error */
# define AUTH_FLASH_READ_TIMEOUT 8  /* read cy flash fail */

/* data for Authentication task */
# define AUTH_NO_CERT           0
# define AUTH_NET_CERT          1
# define AUTH_MAN_CERT          2

/* EVENT IDs assignments for MIBMGR */
/* ----------------------------------------------------------------------------- */
#define TF_NO_TX_BUFF        	 1
#define TF_TRANSMIT              2
#define TF_BLK_SYNCH         	 3
#define TF_CODE_OVERSIZE         4
#define TF_FRM_OVERSIZE          5
#define TF_CY_FAIL               6
#define TF_OPCODE                7
#define TF_SHA                   8
#define TF_TIME_ZEROED           9
#define TF_CHIP_FAIL            10
#define TF_AUTH_API_FAIL    	11
#define MB_NO_TX_BUFF           12
#define MB_TANSMIT              13
#define MB_PK_ALLOC             14
#define MB_SDU_IP_ADDR          15
#define MB_NETEXIT              16
#define MB_TEXT                 17
#define MB_ASN_1             	19
#define MB_DE_CY            	20
#define MB_SHA              	21
#define MB_SSNMP_TIME       	22
#define MB_EN_CY            	23
#define MB_ARP_TX           	24
#define MB_NVM_PUT          	25
#define SDU_STATE_CHANGE		26	/* sdu state change via snmp */
#define MB_READ_RAND			27	/* read randome fail */
	
/* ----------------------------------------------------------------------------- */
/* EVENT IDs assignments for Enitity Protection (+custodian) */
/* ----------------------------------------------------------------------------- */
# define EPC_BADLIST_A		     1
# define EPC_BADLIST_B 	         2
# define EP_COTFULL				 3

/* ----------------------------------------------------------------------------- */
/* EVENT IDs assignments for NET_DRIVER */
/* ----------------------------------------------------------------------------- */
# define PORT0_SYS_ERROR         1
# define PORT1_SYS_ERROR     	 2
# define PORT0_LINK_UP           3
# define PORT0_LINK_DOWN         4
# define PORT1_LINK_UP           5
# define PORT1_LINK_DOWN         6

/* Data --- Don't ever change net system error values; they are hard coded */
# define NET_PARITY_ERROR     0
# define NET_MASTER_ABORT     1
# define NET_TARGET_ABORT     2

/* net link data - redundant, could be ignored */
# define NET_LINK_UP          3
# define NET_LINK_DOWN        4

/* --------------------------------SDU_init----------------------------------- */
# define INIT_IN_PROGRESS         1    /* init in progress */
# define NV_INITFAIL              2    /* nvram init fail */
# define NV_GETFAIL               3    /* nvram getall fail */
# define NV_PUTALL                4    /* nvram putall fail */
# define NV_INITNEW			      5    /* nvram - initialize new parms */
# define EP_INIT_FAIL             6    /* EP init */
# define CS_INIT_FAIL             7    /* CS init */
# define MIBMGR_INIT_ERROR        8    /* MIBMGR */
# define NET_CNFG_ERROR           9    /* PCI/ Card error (see data) */
# define NET_PORT1_LPBK_FAIL     10    /* Net Port 0 Loopback fail (see data) */
# define NET_PORT0_LPBK_FAIL     11    /* Net Port 1 Loopback fail (see data) */
# define CCC_FAILURE			 12	   /* Crypto Card Failed init */
# define INIT_COMPLETE           13    /* Initialization Complete */
# define UTILITY_NOT_FOUND		 14    /* utility card cannot be located */
# define RTC_RESET				 15	   /* Real-time clock has been reset */
# define NET_P0_PHYS_CONN_10B5   16    /* Port 0 - 10B5 connection problem */
# define NET_P1_PHYS_CONN_10B5   17    /* Port 1 - 10B5 connection problem */
# define NET_P0_PHYS_CONN_NC  	 18    /* Port 0 - No Cable connection */
# define NET_P1_PHYS_CONN_NC  	 19    /* Port 1 - No Cable connection */

/* -----------------------CCC module---------------------------------- */

# define CCCEVT_NORESPONSE	 	1 	 	/* no response */
# define CCCEVT_TAMPER			2		/* CCC was tampered since last power-on. */
# define CCCEVT_DES1FAIL		3
# define CCCEVT_DES2FAIL		4
# define CCCEVT_EXP1FAIL		5
# define CCCEVT_EXP2FAIL		6
# define CCCEVT_RTCFAIL			7		/* RTC has stopped, it's frozen */
# define CCCEVT_RTCSTART		8		/* RTC has resumed after stopping */
# define CCCEVT_INIT			9		/* CCC starting up (doing INIT process) */
# define CCCEVT_BADSTATCHG		10		/* status bit changed in invalid way */
# define CCCEVT_UNKSTATUS		11		/* unknown status bit set */
# define CCC_NUM_EVTS			11		/* number of events for CCC module */

/* -----------------------Event Manager Task-------------------------- */
# define SDU_SYS_STATE			  1  	/* system state change event id  */
# define EVENT_MAC_ADDRESS		  2  	/* register MAC address */
# define EVENT_IP_ADDRESS		  3  	/* register IP address */
# define SDU_VERSION			  4

/* -----------------------BOOTP Client Task-------------------------- */
# define BOOT_IP_ADDRESS		  1
#endif         

