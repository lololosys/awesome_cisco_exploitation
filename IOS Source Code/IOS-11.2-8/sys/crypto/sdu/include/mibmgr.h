/* $Id: mibmgr.h,v 1.1.68.1 1996/04/19 14:57:43 che Exp $
 * $Source: /release/112/cvs/Xsys/crypto/sdu/include/mibmgr.h,v $
 *------------------------------------------------------------------
 * Cylink Source code: mibmgr.h
 *
 * August 1995, Brian Weis
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: mibmgr.h,v $
 * Revision 1.1.68.1  1996/04/19  14:57:43  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commits
 *
 * Revision 1.1.2.1  1995/11/28  01:49:41  bew
 * Branch: IosSec_branch
 * Port of newest Cylink SDU code, plus CS cleanup. The SDU code
 * is moved to its original Cylink tree structure.
 *
 * Revision 1.1  1995/11/22  23:22:20  bew
 * Placeholders for new SDU file locations.
 *
 * Revision 1.7  1995/06/07 12:03:05  smackie
 * Fold back Arizona_branch into mainline.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************************
* $Header: /SecureLAN/SDU/Include/MIBMGR.H 23    9/26/95 5:41p Sduhost 
*
* DESCRIPTION:
*    Header file for the MIB Manager (SNMP agent).
*
* NOTES:
*
* Copyright(c)  Cylink Corporation 1994.  All rights reserved.
*******************************************************************************
* $History: MIBMGR.H 
* 
* *****************  Version 23  *****************
* User: Sduhost      Date: 9/26/95    Time: 5:41p
* Updated in $/SecureLAN/SDU/Include
* Defined two new defines.
* #define	SNMP_ERR_FAILPASS1_DECRYPT	0x100
* #define SNMP_ERR_FAILFINALSHA		0x101
* 
* 
* *****************  Version 22  *****************
* User: Sduhost      Date: 7/27/95    Time: 4:58p
* Updated in $/SecureLAN/SDU/Include
* Prototype change for pass2ProcOutgoingObj.
* 
* *****************  Version 21  *****************
* User: Sduhost      Date: 7/24/95    Time: 11:28a
* Updated in $/SecureLAN/SDU/Include
* Update to prototype of pass2ProcOutgoingObj.
* 
* *****************  Version 20  *****************
* User: Ramesh       Date: 6/21/95    Time: 11:19a
* Updated in $/SecureLAN/SDU/Include
* No changes made.
* 
* *****************  Version 19  *****************
* User: Ramesh       Date: 6/15/95    Time: 3:51p
* Updated in $/SecureLAN/SDU/Include
* Changed prototype to get rid of unsed argument.
* 
* *****************  Version 18  *****************
* User: Sduhost      Date: 6/10/95    Time: 12:35p
* Updated in $/SecureLAN/SDU/Include
* Update for secure SNMP
* 
* *****************  Version 17  *****************
* User: Sduhost      Date: 6/06/95    Time: 2:57p
* Updated in $/SecureLAN/SDU/Include
* Secure SNMP update
* 
* *****************  Version 16  *****************
* User: Tchi         Date: 5/17/95    Time: 8:49p
* Updated in $/SecureLAN/SDU/Include
* smart boot changes
* 
* *****************  Version 15  *****************
* User: Tchi         Date: 4/18/95    Time: 11:45a
* Updated in $/SecureLAN/SDU/Include
* SSNMP and EP/CS TABLE changes
* 
* *****************  Version 14  *****************
* User: Tchi         Date: 2/13/95    Time: 6:57p
* Updated in $/SecureLAN/SDU/Include
* error handling for IO MGR
* 
* *****************  Version 13  *****************
* User: Tchi         Date: 2/04/95    Time: 2:57p
* Updated in $/SecureLAN/SDU/Include
* finish NVRAM
* 
* *****************  Version 12  *****************
* User: Tchi         Date: 1/27/95    Time: 6:29p
* Updated in $/SecureLAN/SDU/Include
* intgr2a
* 
* *****************  Version 11  *****************
* User: Tchi         Date: 1/27/95    Time: 4:35p
* Updated in $/SecureLAN/SDU/Include
* added MB_ERR_ARP_TX
* 
* *****************  Version 10  *****************
* User: Tchi         Date: 1/27/95    Time: 4:02p
* Updated in $/SecureLAN/SDU/Include
* created ARP
* 
* *****************  Version 9  *****************
* User: Venkat       Date: 1/07/95    Time: 1:25p
* Updated in $/SecureLAN/SDU/Include
* Protocol type manifest constants were moved to pactl.h file
* 
* *****************  Version 8  *****************
* User: Tchi         Date: 1/07/95    Time: 11:23a
* Updated in $/SecureLAN/SDU/Include
* can not recall
* 
* *****************  Version 7  *****************
* User: Tchi         Date: 12/13/94   Time: 3:25p
* Updated in $/SecureLAN/SDU/Include
* SSNMP changes
* 
* *****************  Version 6  *****************
* User: Sduhost      Date: 12/07/94   Time: 11:35a
* Updated in $/SecureLAN/SDU/Include
* add more error code
* 
* *****************  Version 5  *****************
* User: Sduhost      Date: 12/02/94   Time: 10:54a
* Updated in $/SecureLAN/SDU/Include
* intgr1D
* 
* *****************  Version 4  *****************
* User: Sduhost      Date: 12/01/94   Time: 8:46a
* Updated in $/SecureLAN/SDU/Include
* intgr1D
* 
* *****************  Version 3  *****************
* User: Tchi         Date: 11/28/94   Time: 4:56p
* Updated in $/SecureLAN/SDU/Include
* added SNMP
* 
* *****************  Version 2  *****************
* User: Tchi         Date: 11/18/94   Time: 9:46a
* Updated in $/SecureLAN/SDU/Include
* added mac_type, SYS_ICMP
*
* *****************  Version 1  *****************
* User: Tchi         Date: 11/16/94   Time: 5:43p
* Created in $/SecureLAN/SDU/Include
* created
******************************************************************************/

#ifndef MIBMGR_H
#define MIBMGR_H

/*--------------------------------- INCLUDES ---------------------------------- */
#include "../crypto/sdu/include/cylink.h"
#include "../crypto/sdu/include/cy_api.h"

/*--------------------------------- CONSTANTS --------------------------------- */
/* SYS_SNMP, SYS_ICMP, SYS_BOOTP, SYS_TFTP, SYS_ARP are defined in pactl.h */
#define SYS_CS              4               /*response, and confirm */

/*#define SSNMP_PAST_DLT      75              //seconds */
/*#define SSNMP_FUTURE_DLT    45              //seconds */
#define MAC_FCS_SIZE        4

#define IV_RNG_SIZE         8
#define HASH_RES_SIZE       20
#define ALGO_TYPE_SIZE		2
#define	SHA_TIME_ALGO_SIZE  (HASH_RES_SIZE + 4 + ALGO_TYPE_SIZE) /*time has 4 bytes */
#define MIN_CYSTR_LEN		(SHA_TIME_ALGO_SIZE + IV_RNG_SIZE)
#define DES_ALGO_TYPE		0

/*The following TFTP error code has to be type WORD (less than 16 bits) */
/*since the error code is also carried in the TFTP ERROR frame. */

/*Also notice that there is no system state defined for TFTP. So all the */
/*following is using OPERATIONAL state. */
#define MB_NO_ERROR         	0			/*EVENT ID: STATE: DISPLAY MSG: */
#define TF_ERR_NO_TX_BUF    	0x00000001L	/*1:  OPERATIONAL: "No transmission buffer." */
#define TF_ERR_TRANSMIT     	0x00000002L	/*2:  OPERATIONAL: "Driver failed to transmit." */
#define TF_ERR_BLK_SYNCH		0x00000004L	/*3:  OPERATIONAL: "The block number in TFTP frame is out of sync." */
#define TF_ERR_CODE_OVERSIZE	0x00000008L	/*4:  OPERATIONAL: "The code image exceeds the flash size." */
#define TF_ERR_FRM_OVERSIZE		0x00000010L	/*5:  OPERATIONAL: "The TFTP payload is over 512 bytes." */
#define TF_ERR_CY_FAIL			0x00000020L	/*6:  OPERATIONAL: "Crypto failed." */
#define TF_ERR_OPCODE			0x00000040L	/*7:  OPERATIONAL: "Invalid op code in TFTP frame." */
#define TF_ERR_SHA				0x00000080L	/*8:  OPERATIONAL: "TFTP frame has been tampered so SHA failed." */
#define TF_ERR_CODE_HDR			0x00000100L	/*9:  OPERATIONAL: "The code header in first TFTP frame is wrong." */
#define TF_ERR_CHIP_FAIL		0x00000200L	/*10: OPERATIONAL: "Failed to write to Flash." */
#define TF_ERR_AUTH_API_FAIL	0x00000400L	/*11: OPERATIONAL: "Authentication failed." */
/*SNMP error code: continue after TFTP error code */
#define MB_ERR_NO_TX_BUF    	0x00000800L	/*12: ALARM3: "No transmission buffer." */
#define MB_ERR_TRANSMIT     	0x00001000L	/*13: ALARM3: "Driver failed to transmit." */
#define MB_ERR_PK_ALLOC     	0x00002000L	/*14: ALARM3: "SNMP buffer allocation failed." */
#define MB_ERR_SDU_IP_ADDR		0x00004000L	/*15: OPERATIONAL: "Invalid SDU IP addr (does not belong class A, B, or C)." */
#define MB_ERR_NETEXIT      	0x00008000L /*16: ALARM3: "SNMP agent terminated due to fatal error." */
#define MB_ERR_TEXT         	0x00010000L /*17: OPERATIONAL: Please display global variable "snmp_err_text". */
/*#define MB_ERR_LEN_LONG     	0x00020000L //18: OPERATIONAL: "Error in ASN.1 extended length." */
#define MB_ERR_ASN1         	0x00040000L /*19: OPERATIONAL: "Bad ASN.1 syntax." */
#define MB_ERR_DE_CY        	0x00080000L /*20: ALARM2: "Crypto failed to decrypt SSNMP." */
#define MB_ERR_SHA          	0x00100000L /*21: TAMPERED: "SSNMP SHA failed." */
#define MB_ERR_SSNMP_TIME   	0x00200000L /*22: TAMPERED: "A playback is dectected in SSNMP time field." */
#define MB_ERR_EN_CY        	0x00400000L /*23: ALARM2: "Crypto failed to encrypt SSNMP." */
#define MB_ERR_ARP_TX			0x00800000L /*24: OPERATIONAL: "Driver failed to transmit." */
#define MB_ERR_NVM_PUT			0x01000000L /*25: OPERATIONAL: "Failed to write to NV RAM." */
#define MB_ERR_SSNMP_IND_N1		0x02000000L /*SSNMP Indicator is not the first object */
#define MB_ERR_SSNMP_INV_SM		0x04000000L	/*SSNMP frame is not from a registered SM */
#define MB_ERR_SSNMP_IND_LS		0x08000000L	/*SSNMP Indicator is missing before a SSNMP object is parsed. */
#define MB_ERR_INV_SHA			0x10000000L	/*The final SHA is inconsistent with that in frame */
#define MB_ERR_NO_CY_ALGO		0x20000000L	/*Unsupported Crypto algorithem type */
#define MB_ERR_READ_RAND		0x40000000L	/*31:Read Random failed */

/*initialization error code */
/*#define MG_ALLOC_IV_FAILED  1 */

/*ARP related */
#define MAX_HA_PA_LEN           12
#define ARP_OP_OFFSET			7
#define ARP_OP_RESP				2
#define ARP_SENDER_HA_OFFSET	8
#define ARP_SENDER_IA_OFFSET    14
#define ARP_HLEN_OFFSET         4
#define ARP_PLEN_OFFSET         5

#define SSNMP_PASS1		1
#define SSNMP_PASS2		2

#define	SNMP_ERR_FAILPASS1_DECRYPT	0x100
#define SNMP_ERR_FAILFINALSHA		0x101

#define MAX_OIDLEN	24				/* defined a random value for now. */

/*--------------------------------- STRUCTURES -------------------------------- */
typedef struct
{
	uchar   *l2;        /*pointer to MAC layer (L2: first byte of the frame) */
	uchar   *l3;        /*pointer to Network layer (L3: IP) */
	uchar	*l4;		/*pointer to Transport layer (L4: UDP).TFTP uses it. */
	uchar   *l5;        /*pointer to Session layer (L5: SNMP, BOOTP, TFTP) */
	ushort  size;       /*entire frame length in bytes */
	ushort  type;       /*SYS_SNMP, SYS_ICMP, SYS_BOOTP, or SYS_TFTP */
	ushort  mac;        /*MAC frame type: IEEE8023 or ETHERNET */
	ushort  pid;		/*layer 3 protocol id such as INTERNET_IP */
	void    *port;		/*ptr to receive port */
}
sys_frm_info;

/*The first four members are updated for each object in frame; they are set */
/*		in function "snmp_parse_var_op". */
/*Fifth member is intialized to 1 for each Object; reset to 0 by MIB II */
/*		binding functions. */
/*the sixth member is set by the binding function of SSNMP Indicator */
typedef struct
{
	ulong	 name[MAX_OIDLEN];	 /*point to the Name in current object in frame */
	uchar		*val;		/*point to the Value in current object in frame */
	ushort		nameLen;	/*length of Name in bytes in frame */
	ushort		valLen;		/*length of Value in bytes in frame */
	uchar		*cyStr;		/*indicates if SSNMP Indicator has been parsed. */
							/*	it points to the Indicator in frame for Pass1; */
							/*	it points to the local MIB variable for Pass2. */
	int			inSmib;		/*1 if it is in SMIB; 0 if in MIB II */
	int			indFound;	/*1 if the current object in frame is SSNMP Indicator */
	int			pass;		/*indicating pass1 or pass2 */
	int			intrError;	/*error generated by binding function */
	int			ssnmptype;  /* Secure SNMP PDU. */
} ssnmpObjAttrib;

/*------------------------------- MACROS ------------------------------------ */
#define ASN1_ERROR()                			\
{                                   			\
	post_err_iomgr(MB_ERR_ASN1,&mg_err,MIBMGR);	\
	return(0);                      			\
}

#define NEXT_TLV(pdu, len)          \
{                                   \
	pdu += asn_read_len(pdu, &len); \
	pdu += (ushort)len;             \
}

/*----------------------------- DECLARATIONS -------------------------------- */

extern void mibmgr(void);
extern int mibmgr_init(void);
extern void record_trap_SM_info(void);
extern int getTlvValue(uchar *, uchar **, ushort *);
extern ulong pass2ProcOutgoingObj(uchar, uchar *, SHA_context *, int * );
extern ulong pass1ProcCurObject(long, SHA_context *, uchar);
extern ulong pass1CheckFinalSHA(SHA_context *);
extern ulong pass2SetFinalSHA(SHA_context *, int);
extern void assemble_version(char *, int);

#endif

/*-------------------------------- END OF FILE ------------------------------ */
