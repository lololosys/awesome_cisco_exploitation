/* $Id: mgdata.h,v 1.1.68.1 1996/04/19 14:57:42 che Exp $
 * $Source: /release/112/cvs/Xsys/crypto/sdu/include/mgdata.h,v $
 *------------------------------------------------------------------
 * Cylink Source code: mgdata.h
 *
 * August 1995, Brian Weis
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: mgdata.h,v $
 * Revision 1.1.68.1  1996/04/19  14:57:42  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commits
 *
 * Revision 1.1.2.1  1995/11/28  01:49:40  bew
 * Branch: IosSec_branch
 * Port of newest Cylink SDU code, plus CS cleanup. The SDU code
 * is moved to its original Cylink tree structure.
 *
 * Revision 1.1  1995/11/22  23:22:18  bew
 * Placeholders for new SDU file locations.
 *
 * Revision 1.7  1995/06/07 12:03:05  smackie
 * Fold back Arizona_branch into mainline.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************************
* $Header: /SecureLAN/SDU/Include/MGDATA.H 29    7/19/95 7:01p Tchi 
*
* DESCRIPTION:
*
* Copyright(c)  Cylink Corporation 1994.  All rights reserved.
*******************************************************************************
* $History: MGDATA.H 
* 
* *****************  Version 29  *****************
* User: Tchi         Date: 7/19/95    Time: 7:01p
* Updated in $/SecureLAN/SDU/Include
* added saveSMinfoToNvram
* 
* *****************  Version 28  *****************
* User: Sduhost      Date: 6/26/95    Time: 6:50p
* Updated in $/SecureLAN/SDU/Include
* added two globals
* 
* *****************  Version 27  *****************
* User: Ramesh       Date: 6/21/95    Time: 11:17a
* Updated in $/SecureLAN/SDU/Include
* Defined mib II system group objects as extern.
* 
* *****************  Version 26  *****************
* User: Tchi         Date: 6/06/95    Time: 5:17p
* Updated in $/SecureLAN/SDU/Include
* added max_pconn_period
* 
* *****************  Version 25  *****************
* User: Sduhost      Date: 6/06/95    Time: 2:57p
* Updated in $/SecureLAN/SDU/Include
* Secure SNMP update.
* 
* *****************  Version 24  *****************
* User: Venkat       Date: 4/22/95    Time: 9:27a
* Updated in $/SecureLAN/SDU/Include
* Cleanup regd. BOOTP variables.
* 
* *****************  Version 23  *****************
* User: Tchi         Date: 4/20/95    Time: 7:44p
* Updated in $/SecureLAN/SDU/Include
* CS config data resouce locking; csAvail to check if CS can start
* 
* *****************  Version 22  *****************
* User: Tchi         Date: 4/18/95    Time: 11:45a
* Updated in $/SecureLAN/SDU/Include
* SSNMP and EP/CS TABLE changes
* 
* *****************  Version 21  *****************
* User: Tchi         Date: 3/22/95    Time: 12:51p
* Updated in $/SecureLAN/SDU/Include
* removed mg_sm_mac_addr
* 
* *****************  Version 20  *****************
* User: Tchi         Date: 2/16/95    Time: 6:56p
* Updated in $/SecureLAN/SDU/Include
* change the addresses of the last two NNC msg
* 
* *****************  Version 19  *****************
* User: Tchi         Date: 2/14/95    Time: 7:21p
* Updated in $/SecureLAN/SDU/Include
* added DDP node ID
* 
* *****************  Version 18  *****************
* User: Tchi         Date: 2/13/95    Time: 6:57p
* Updated in $/SecureLAN/SDU/Include
* error handling for IO MGR
* 
* *****************  Version 17  *****************
* User: Tchi         Date: 2/04/95    Time: 2:57p
* Updated in $/SecureLAN/SDU/Include
* finish NVRAM
* 
* *****************  Version 16  *****************
* User: Tchi         Date: 2/04/95    Time: 9:00a
* Updated in $/SecureLAN/SDU/Include
* CS and Authentication interface
* 
* *****************  Version 15  *****************
* User: Tchi         Date: 1/20/95    Time: 6:16p
* Updated in $/SecureLAN/SDU/Include
* intgra 2a
* 
* *****************  Version 14  *****************
* User: Tchi         Date: 1/12/95    Time: 6:01p
* Updated in $/SecureLAN/SDU/Include
* added cur_sys_frm
* 
* *****************  Version 13  *****************
* User: Tchi         Date: 1/06/95    Time: 10:01a
* Updated in $/SecureLAN/SDU/Include
* added mg_sdu_version
* 
* *****************  Version 12  *****************
* User: Tchi         Date: 12/19/94   Time: 7:23p
* Updated in $/SecureLAN/SDU/Include
* MIB changes
* 
* *****************  Version 11  *****************
* User: Tchi         Date: 12/13/94   Time: 3:25p
* Updated in $/SecureLAN/SDU/Include
* SSNMP changes
* 
* *****************  Version 10  *****************
* User: Tchi         Date: 12/07/94   Time: 5:13p
* Updated in $/SecureLAN/SDU/Include
* added constants
* 
* *****************  Version 9  *****************
* User: Sduhost      Date: 12/03/94   Time: 1:40p
* Updated in $/SecureLAN/SDU/Include
* intgr1D
* 
* *****************  Version 8  *****************
* User: Croyle       Date: 12/01/94   Time: 10:02a
* Updated in $/SecureLAN/SDU/Include
* Added SM IP variable
* 
* *****************  Version 7  *****************
* User: Croyle       Date: 11/30/94   Time: 5:55p
* Updated in $/SecureLAN/SDU/Include
* Added EXTERN statements for some BOOTP variables
* 
* *****************  Version 6  *****************
* User: Sduhost      Date: 11/30/94   Time: 4:12p
* Updated in $/SecureLAN/SDU/Include
* mibmgr.h was misspelled in previous version
* 
* *****************  Version 5  *****************
* User: Croyle       Date: 11/30/94   Time: 4:08p
* Updated in $/SecureLAN/SDU/Include
* Added a few more BOOTP related variables
* 
* *****************  Version 4  *****************
* User: Croyle       Date: 11/30/94   Time: 2:42p
* Updated in $/SecureLAN/SDU/Include
* Added Gateway IP variable
* 
* *****************  Version 3  *****************
* User: Sduhost      Date: 11/30/94   Time: 2:33p
* Updated in $/SecureLAN/SDU/Include
* intgr1D
* 
* *****************  Version 2  *****************
* User: Tchi         Date: 11/29/94   Time: 1:42p
* Updated in $/SecureLAN/SDU/Include
* added sys_descr
* 
* *****************  Version 1  *****************
* User: Tchi         Date: 11/28/94   Time: 4:56p
* Created in $/SecureLAN/SDU/Include
* created
*******************************************************************************/

#ifndef MGDATA_H
#define MGDATA_H

#include "../crypto/sdu/include/cylink.h"
#ifndef CISCO
#include <nptypes.h>
#include <snmpport.h>
#include <snmp_var.h>
#include "../crypto/sdu/include/mib2.h"
#endif

#include "../crypto/sdu/include/mibmgr.h"
#include "../crypto/sdu/include/connstup.h"

/*----------------------------- CONSTANTS ------------------------------------- */
#define MAX_L2_SIZE         24  /*14 byte ieee802.3 plus 9 byte ieee802.2 */
#define SSNMP_ID_VAL_SIZE   58  /*max. value */
										  /*SHA = 20, TIME = 4, ALGO = 2, IV = 32 max. */

typedef struct
{
	uchar   hdr[MAX_L2_SIZE];   /*the MAC header from SM; */
								/*could be Ethernet or IEEE802.3 */
	ushort  hdr_len;            /*length in bytes */
	ushort  type;               /*MAC frame type: IEEE8023 or ETHERNET */
	void	*port;				/*the port where sys frame from SM is received */
}
sm_mac;

typedef struct
{
	sm_mac	mac;
	ulong	ipaddr;
}
trap_info;

/*******************************************************************************
* EXTERNAL FUNCTION DECLARATIONS
*******************************************************************************/
#ifndef MGDATA_C

/*globals */
extern sys_frm_info 	cur_sys_frm;
extern sm_mac       	mg_sm_mac;
extern trap_info		trap_sm_info;
extern int				mg_sm_port_id;
extern int				trap_sm_port_id;

/*NOTE: This variable itself should not be saved to NVRAM */
extern ushort			saveSMinfoToNvram;

/*extern void 			*mg_sm_port;		// Port ptr to use for SM comm */
/*extern ushort       	hi_des_iv; */
extern int				NVMindex;
extern int				snmpAgentRunning;
extern ssnmpObjAttrib	ssnmpObjInfo;

/*MIB variables */
extern ulong		mg_sm_ip;					/* SM IP adr - NO more from BOOTP! */

extern ulong		mg_customer_id;				/* Customer ID */
extern uchar		mg_product_id;				/* Product ID */

/*MB */
extern uchar        mg_ssnmp_id_val[SSNMP_ID_VAL_SIZE]; /*SSNMP ID value part */
extern ulong        mg_err;	/*error bit flag for MG task */
#ifndef CISCO
extern uchar        mg_sdu_serial_num[SERIAL_NUM_SIZE];
#endif
extern char		 	mg_sdu_version[];

extern uchar        sys_sdu_nw_addr_ip[PDU_NW_ADDR_SIZE];  /*4 bytes */
extern uchar        sys_sdu_nw_addr_ipx[PDU_NW_ADDR_SIZE]; /*4 bytes */
extern uchar        sys_sdu_nw_addr_idp[PDU_NW_ADDR_SIZE]; /*4 bytes */
extern uchar        sys_sdu_nw_addr_ddp[DDP_NW_ADDR_SIZE]; /*2 bytes */
extern uchar		sys_sdu_ddp_node_id;

extern char         sys_descr[];                /*SNMP system descriptor */

#ifdef CISCO /* From snmp/snmpport.h */
#define SYS_STRING_MAX 256
#endif
extern char sysContact[SYS_STRING_MAX];
extern char sysLocation[SYS_STRING_MAX];
extern char sysName[SYS_STRING_MAX];

#endif

#endif
