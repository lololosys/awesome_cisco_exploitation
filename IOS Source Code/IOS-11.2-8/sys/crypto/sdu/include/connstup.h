/* $Id: connstup.h,v 1.1.68.2 1996/09/11 23:21:05 bew Exp $
 * $Source: /release/112/cvs/Xsys/crypto/sdu/include/connstup.h,v $
 *------------------------------------------------------------------
 * Cylink Source code: connstup.h
 *
 * August 1995, Brian Weis
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: connstup.h,v $
 * Revision 1.1.68.2  1996/09/11  23:21:05  bew
 * CSCdi68438:  crypto determination of network address should be done
 * earlier
 * Branch: California_branch
 * Run the routing tables during connection message setup to determine
 * which interface the packet will go out.
 *
 * Revision 1.1.68.1  1996/04/19  14:57:29  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commits
 *
 * Revision 1.1.2.3  1996/04/16  21:53:52  bew
 * Branch: IosSec_branch
 * General code cleanup. Implemented checking of crypto map peers. CIM now
 * addressed to the src/dst rather than pe/upe.
 *
 * Revision 1.1.2.2  1996/01/16  20:20:58  bew
 * Branch: IosSec_branch
 * Simplified the session mgmt debug msgs & changed the way that duplicate
 * connection messages are handled.
 *
 * Revision 1.1.2.1  1995/11/28  01:49:30  bew
 * Branch: IosSec_branch
 * Port of newest Cylink SDU code, plus CS cleanup. The SDU code
 * is moved to its original Cylink tree structure.
 *
 * Revision 1.1  1995/11/22  23:22:04  bew
 * Placeholders for new SDU file locations.
 *
 * Revision 1.7  1995/06/07 12:03:05  smackie
 * Fold back Arizona_branch into mainline.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************************
* $Header: /SecureLAN/SDU/Include/CONNSTUP.H 31    7/24/95 10:04a Tchi 
*
* DESCRIPTION:
*    This file defines all constants, macros, and function declarations for
*    the CS module.
*
* NOTES:
*
* Copyright(c)  Cylink Corporation 1994.  All rights reserved.
*******************************************************************************
* $History: CONNSTUP.H 
* 
* *****************  Version 31  *****************
* User: Tchi         Date: 7/24/95    Time: 10:04a
* Updated in $/SecureLAN/SDU/Include
* fixed MAX_NNC_SIZE
* 
* *****************  Version 30  *****************
* User: Tchi         Date: 7/19/95    Time: 5:47p
* Updated in $/SecureLAN/SDU/Include
* alter the challenge number after signing the message
* 
* *****************  Version 29  *****************
* User: Tchi         Date: 6/09/95    Time: 10:25a
* Updated in $/SecureLAN/SDU/Include
* added NNC_ENTITY_ID_OFFSET
* 
* *****************  Version 28  *****************
* User: Tchi         Date: 6/06/95    Time: 5:17p
* Updated in $/SecureLAN/SDU/Include
* changed timeout period to 20 seconds
* 
* *****************  Version 27  *****************
* User: Sduhost      Date: 5/15/95    Time: 6:25p
* Updated in $/SecureLAN/SDU/Include
* Redefine the action entries
* 
* *****************  Version 24  *****************
* User: Sduhost      Date: 5/09/95    Time: 7:10p
* Updated in $/SecureLAN/SDU/Include
* removed unused constants
* 
* *****************  Version 23  *****************
* User: Sduhost      Date: 5/01/95    Time: 7:07p
* Updated in $/SecureLAN/SDU/Include
* added connstup in function declaration
* 
* *****************  Version 22  *****************
* User: Tchi         Date: 4/20/95    Time: 7:41p
* Updated in $/SecureLAN/SDU/Include
* CS config data resouce locking; csAvail to check if CS can start.
* 
* *****************  Version 21  *****************
* User: Tchi         Date: 4/18/95    Time: 11:45a
* Updated in $/SecureLAN/SDU/Include
* SSNMP and EP/CS TABLE changes
* 
* *****************  Version 20  *****************
* User: Sduhost      Date: 2/23/95    Time: 6:06p
* Updated in $/SecureLAN/SDU/Include
* added a compiler directive so we can disable retransmission timeout
* 
* *****************  Version 19  *****************
* User: Tchi         Date: 2/23/95    Time: 2:03p
* Updated in $/SecureLAN/SDU/Include
* included other SDU's DSS public number in the pending connection list
* entry.
* 
* *****************  Version 18  *****************
* User: Sduhost      Date: 2/23/95    Time: 11:10a
* Updated in $/SecureLAN/SDU/Include
* bug fix
* 
* *****************  Version 17  *****************
* User: Tchi         Date: 2/16/95    Time: 6:56p
* Updated in $/SecureLAN/SDU/Include
* change the addresses of the last two NNC msg
* 
* *****************  Version 16  *****************
* User: Bald_eagle   Date: 2/13/95    Time: 1:10p
* Updated in $/SecureLAN/SDU/Include
* Modified HMA index type from ushort to short
* 
* *****************  Version 15  *****************
* User: Tchi         Date: 2/13/95    Time: 9:05a
* Updated in $/SecureLAN/SDU/Include
* changed comments
* 
* *****************  Version 14  *****************
* User: Tchi         Date: 2/12/95    Time: 5:48p
* Updated in $/SecureLAN/SDU/Include
* error handling for IO Mgr
* 
* *****************  Version 13  *****************
* User: Tchi         Date: 2/04/95    Time: 2:57p
* Updated in $/SecureLAN/SDU/Include
* finish NVRAM
* 
* *****************  Version 12  *****************
* User: Tchi         Date: 2/04/95    Time: 8:59a
* Updated in $/SecureLAN/SDU/Include
* CS and Authentication interface
* 
* *****************  Version 11  *****************
* User: Tchi         Date: 1/20/95    Time: 5:39p
* Updated in $/SecureLAN/SDU/Include
* intgra 2a
* 
* *****************  Version 10  *****************
* User: Tchi         Date: 1/19/95    Time: 6:37p
* Updated in $/SecureLAN/SDU/Include
* fixed csTransmit
* 
* *****************  Version 9  *****************
* User: Tchi         Date: 1/19/95    Time: 1:30p
* Updated in $/SecureLAN/SDU/Include
* added RAND_GEN_SIZE
* 
* *****************  Version 8  *****************
* User: Tchi         Date: 1/12/95    Time: 6:01p
* Updated in $/SecureLAN/SDU/Include
* changed to ep_frm.h
* 
* *****************  Version 7  *****************
* User: Tchi         Date: 12/19/94   Time: 7:23p
* Updated in $/SecureLAN/SDU/Include
* MIB changes
* 
* *****************  Version 6  *****************
* User: Tchi         Date: 11/11/94   Time: 4:24p
* Updated in $/SecureLAN/SDU/Include
* miscellaneous
* 
* *****************  Version 5  *****************
* User: Tchi         Date: 11/08/94   Time: 4:08p
* Updated in $/SecureLAN/SDU/Include
* added DSS
* 
* *****************  Version 4  *****************
* User: Tchi         Date: 11/04/94   Time: 3:54p
* Updated in $/SecureLAN/SDU/Include
* modifed sdu_srng_tb; added sdu_srng_tb_entries
* 
* *****************  Version 3  *****************
* User: Tchi         Date: 11/02/94   Time: 8:31a
* Updated in $/SecureLAN/SDU/Include
* MAX_SDU_PE is moved to ep.h
*
* *****************  Version 2  *****************
* User: Tchi         Date: 11/01/94   Time: 2:11p
* Updated in $/SecureLAN/SDU/Include
* added MAX_NNC_BUF_SIZE
* 
* *****************  Version 1  *****************
* User: Tchi         Date: 10/25/94   Time: 8:47p
* Created in $/SecureLAN/SDU/Include
* created
*******************************************************************************/

#ifndef CONNSTUP_H
#define CONNSTUP_H

/*--------------------------------- includes ---------------------------------- */
#include "../crypto/sdu/include/cylink.h"
#ifndef CISCO
#include <time.h>    /* time_t */
#else /* CISCO */
typedef ulong   time_t;
#endif /* CISCO */
#include "../crypto/sdu/include/ep_frm.h"

/*--------------------------------- constants --------------------------------- */
#define SECURE_LAN_PRTCL    0

#define PS_SDU              0   /*protocol subtype for SDU */
#define PS_SFU              1   /*protocol subtype for SFU */
#define PS_SNEIC            2   /*protocol subtype for SNE or SNIC */

#define ECHO_REQUEST            0
#define ECHO_REPLY              1
#define ECHO_CONFIRM            2

#define CS_NO_ERROR             0			/*EVENT ID: STATE: DISPLAY MSG */
#define CS_ERR_NO_TX_BUF        0x00000001L	/*1:  ALRAM1: "No transmission buffer." */
#define CS_ERR_NO_PCONN_ENTRY   0x00000002L	/*2:  ALARM1: "No pending connection entry." */
#define CS_ERR_PCONN_TIMEOUT    0x00000004L	/*3:  ALARM1: "Still no response after retransmission." */
#define CS_ERR_NNC_EXP          0x00000008L	/*4:  ALARM1: "NNC exceeds the expiratin time." */
#define CS_ERR_NNC_SIG          0x00000010L	/*5:  TAMPERED: "Invalid signature in NNC." */
#define CS_ERR_SU_SIG           0x00000020L	/*6:  TAMPERED: "Invalid signature in CS message." */
#define CS_ERR_PCONN_INDEX_LOST 0x00000040L /*7:  OPERATIONAL: none */
#define CS_ERR_CHALLENGE_ODD    0x00000080L	/*8:  TAMPERED: "Invalid challenge number in the received Response message." */
#define CS_ERR_CHALLENGE_CMP    0x00000100L	/*9:  TAMPERED: "Invalid challenge number in the received Confirm message." */
#define CS_ERR_NNC_GRPS         0x00000200L	/*10: ALARM1: "Inconsistent groups in the NNCs compared." */
#define CS_ERR_GEN_CHL          0x00000400L /*11: ALARM1: "Crypto failed to generate challenge number." */
#define CS_ERR_GEN_DH           0x00000800L /*12: ALARM1: "Crypto failed to generate Diffie-Hellman pair." */
#define CS_ERR_GEN_SIG          0x00001000L /*13: ALARM1: "Crypto failed to generate signature." */
#define CS_ERR_ALLOC_HM         0x00002000L /*14: ALARM1: "Crypto failed to allocate Hidden Memory." */
#define CS_ERR_RELEASE_HM       0x00004000L /*15: OPERATINAL: "Crypto failed to release Hidden Memory." */
#define CS_ERR_CY_KEYS          0x00008000L /*16: ALARM1: "Crypto failed to generate key index." */
#define CS_ERR_SOK_MASK_RNG     0x00010000L /*17: OPERATIONAL: "Socket mask table and range table are inconsistent." */
#define CS_ERR_VERIFY_SIG       0x00020000L /*18: ALARM1: "Crypto failed in SHA when verifying signature." */
#define CS_ERR_NNC_FRM_SIZE     0x00040000L /*19: OPERATIONAL: "Invalid CS frame size (frame discarded)." */
#define CS_ERR_TRANSMIT         0x00080000L /*20: ALARM1: "Driver could not transmit." */
#define CS_ERR_AUTH_API			0x00100000L /*21: NC&noMC: "Authentication failed." */
#define CS_ERR_NC_NNC_OUT_SYNC	0x00200000L /*22: OPERATIONAL: "The downloaded NNC's revision is inconsistent with NC's (discared by SNMP)." */
#define CS_ERR_CASCADED_CONFIG	0x00400000L /*23: OPERATIONAL: "Invalid configuration detected (frame discarded)." */
#define CS_ERR_NNC_SIZE			0x00800000L /*24: OPERATIONAL: "Invalid NNC size (use default)." */
#define CS_ERR_NVM_PUTREC		0x01000000L /*25: OPERATIONAL: "Failed to write to NV RAME." */
#define	CS_EVT_CONFIG_TB_MISSING		 33 /*Stay OFFLINE until Config Tables are downloaded */

#ifdef CISCO
#define INIT_CHLG_FAILED        1 
#else
/*#define INIT_CHLG_FAILED        1 */
#endif
/*#define INIT_RAND_HID_FAILED    2 */
/*#define INIT_HI_RVAL_FAILED     3 */
/*#define INIT_KKEY_FAILED        4 */

/*errors or indicators in the status field in the NNC frame */
#define STAT_AUTH_MASK          0x0F    /*The following errors will cause */
#define AUTH_NNC_SIG_FAILED     0x01    /*the connection blocked since it */
#define AUTH_SDU_SIG_FAILED     0x02    /*indicates tampering */
#define AUTH_CHALLENGE_FAILED   0x04
/*#define AUTH_GENERAL_FAILED		0x08 */
#define STAT_CYOC_MASK          0xF0    /*The following internal errors will */
#define STAT_GEN_DH_FAILED      0x10    /*cause the connection to be either */
#define STAT_GEN_CHL_FAILED     0x20    /*blocked or passed depending on the */
#define STAT_GEN_SIG_FAILED     0x40    /*open-close flags of both NNCs or the */
#define STAT_SYS_OC_FLAG        0x80	/*system flags "epOpen". */

#define INIT_ICMP_ECHO_REQ      0
#define INIT_ICMP_ECHO_RPY      1

#define PCONN_ENTRIES           182

#ifndef CS_NO_RETRANSMIT
#define MAX_PCONN_TIME          20L      /*retransmission timeout (seconds) */
#else
#define MAX_PCONN_TIME          0xFFFFFFFFL	/*no retransmission */
#endif

#define ICMP_HEADER_SIZE        8
#define ECHO_HEADER_SIZE        2
#define AEP_HEADER_SIZE         1

#define CHALLENGE_SIZE          20
#define CHAL_ODD_MASK           0xAA
#define CHAL_EVEN_MASK          0x55

#define NULL_INDEX              -1
#define UNDEF_INDEX             -2
#define MAC_ADDR_LEN            6
#define MAC_CRC32_SIZE			4
#define MAC_TO_DSAP_LEN         14
#define MAC_TO_SSAP_LEN         15
#define IP_ADDR_LEN             4
#define RAND_GEN_SIZE			20

/*#define DEF_DSS_SIZE            64 */
#define MAX_DSS_SIZE            96
#define DSS_FIX_SIZE            20
/*#define DEF_DH_SIZE             64 */
/*#define MAX_DH_SIZE             96 */
#define DEF_SIG_SIZE            40
#define CY_KEY_SIZE             8
#define SKEY_OFFSET             0
#define IKEY_OFFSET             32
#define NNC_GRP_BLK_SIZE		12

/*NOTE: If SERIAL_NUM_SIZE changes, sanity check of far PE echoing back needs */
/*		to change accordingly. */
#define SERIAL_NUM_SIZE          4
#define PDU_NW_ADDR_SIZE         4
#define	DDP_NW_ADDR_SIZE		 2
#define PCONN_INDEX_SIZE         2
#define PDU_MAX_NW_ADDR_SIZE	 16

#define NNC_GRP_VEC_OFFSET      4
#define NNC_GRP_OC_OFFSET       2
#define NNC_CY_VEC_OFFSET       16
#define NNC_CUS_ID_OFFSET       2
#define NNC_FIX_GRP_OFFSET      20      /*except DSS public */
#define NNC_ENTITY_ID_OFFSET	8
#define NNC_REV_OFFSET          14
#define NNC_FIX_SIZE			64		/*except DSS public and (blocks * 12) */
#define PDU_FIX_NNC_OFFSET      56      /*except DH public */
#define PDU_CHAL_OFFSET         36
#define PDU_FIX_NNC_REV_OFFSET  70      /*except DH public */
#define PDU_PS_OFFSET           1
#define PDU_OP_OFFSET           2     
#define PDU_STAT_OFFSET         3
#define PDU_SERIAL_NUM_OFFSET   4
#define PDU_NW_ADDR_OFFSET		8
#define PDU_INIT_CONNID_OFFSET 	24
#define PDU_RESP_CONNID_OFFSET	28
#define PDU_INIT_PCONN_OFFSET   32
#define PDU_RESP_PCONN_OFFSET   34
#define PDU_CFM_RP_CID_OFFSET	4
#define PDU_CFM_RPC_OFFSET      10
#define PDU_CFM_CHAL_OFFSET     12
#define PDU_CFM_SIG_OFFSET		32

#define MAX_RETX_HEADER         208  /*L2=23 bytes; L3=32 bytes; */
									 /*L4=8 bytes; L5=144 bytes */
#define DEFAULT_RETRANSMISSIONS   2    /*only retransmit once */

/*TO DO: verify the following later */
#define MAX_PRTCLS              32
#define MAX_GRP_BLKS            3
#define NNC_GV_BLK_SIZE         12
#define MAX_NNC_SIZE            (160 + (NNC_GV_BLK_SIZE * MAX_GRP_BLKS))
#define MAX_GRPS                (MAX_GRP_BLKS << 5)
/*#define MAX_SDU_PE: max number of DEFINED protected entities. see ep_frm.h */
#define SPECIAL_NNCS			2   /*UPE NNC and AUTO NNC */
#define MAX_NNC_BUF_SIZE        ((MAX_SDU_PE + SPECIAL_NNCS) * MAX_NNC_SIZE)

#define SNMP_ROW_ACTIVE			1L
#define SNMP_ROW_CREATE_GO		4L
#define SNMP_ROW_DESTROYED		6L

#define SYNDROME_HMA_OFFSET		20	/*ask Jim about this magic number */

/*--------------------------------- structures -------------------------------- */
/*356 bytes per entry. the entire list has 182 entries. */
typedef struct
{
	EntityID    pe;                         /*PE's ID */
	EntityID    upe;                        /*UPE's ID */
	CIBptr      cib;                        /*CIB pointer */
	time_t      timestamp;                  /*time stamp when the frame */
											/*is sent (4 bytes) */

	short		hi_dh_shared;				/*DH share number HM index */
	short       hi_dh_secret;               /*DH secret number HM index */
	short       retx_fsize;                 /*length of the frame sent */

	void *		retx_port;					/*retransmission port */
	int			retx_action;				/*retransmission frame direction */
	int			sm_dss_pub_new_old;			/*SM_DSS_PUBLIC_NEW or OLD */

	uchar *     nnc;                        /*NNC to be sent to other SU */
											/*    and used to create CIB */

	ushort      l5_offset;                  /*connection msg offset */
	ushort      tx_count;                   /*transmission count */
	uchar       retx_buf[MAX_RETX_HEADER];  /*frame sent except NNC and signature */
	uchar		other_dss_pub[MAX_DSS_SIZE];/*other SDU's DSS public number */

	short       prev;                       /*previous node index */
	short       next;                       /*next node index */
#ifdef CISCO
	struct		cisco_pconn {				/*cisco additions: */
		boolean	i_sent_cim;					/* Record of who sent NNC Req */
		ulong src;							/* Src & Dst of CIM */
		ulong dst;
	} cisco;
#endif
}
pconn_entry;


typedef struct
{
    EntityID    pe;         /*PE's ID */
    EntityID    upe;        /*UPE's ID */
}
entity_pair;


typedef struct              /*socket range table entry */
{
    ushort      group;
    ushort      prtcl;
	 SokRng      skrng;
    ushort		 rowstatus;	
}
srng_tb_def;

/*******************************************************************************
* EXTERNAL FUNCTION DECLARATIONS
*******************************************************************************/
#ifdef INTGR_DEBUG
extern void display_cib(CIBptr);	/*debugging purpose */
#endif

#ifndef CONNSTUP_C
extern void 	connstup(void);
extern uchar    cs_init(void);
#endif

#endif

/* defines used to indicate row status. */
#define NOTUSED			0
#define ACTIVE				1
#define NOTINSERVICE		2
#define NOTREADY			3
#define CREATEANDGO		4
#define CREATEANDWAIT	5
#define DESTROY			6

/* defines for protocol action as defined in the enterprise MIB. */
#define REJECT				1
#define PASS				2
#define ENCRYPT			3

/*------------------------- END OF FILE ------------------------------ */
