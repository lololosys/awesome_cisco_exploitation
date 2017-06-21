/* $Id: csdata.h,v 1.1.68.1 1996/04/19 14:57:30 che Exp $
 * $Source: /release/112/cvs/Xsys/crypto/sdu/include/csdata.h,v $
 *------------------------------------------------------------------
 * Cylink Source code: csdata.h
 *
 * August 1995, Brian Weis
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: csdata.h,v $
 * Revision 1.1.68.1  1996/04/19  14:57:30  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commits
 *
 * Revision 1.1.2.2  1996/03/13  22:45:28  bew
 * Branch: IosSec_branch
 * The RSP/VIP environment (multiple crypto engines) are now supported
 * in crypto parser routines and crypto session mgmt code.
 *
 * Revision 1.1.2.1  1995/11/28  01:49:31  bew
 * Branch: IosSec_branch
 * Port of newest Cylink SDU code, plus CS cleanup. The SDU code
 * is moved to its original Cylink tree structure.
 *
 * Revision 1.1  1995/11/22  23:22:05  bew
 * Placeholders for new SDU file locations.
 *
 * Revision 1.7  1995/06/07 12:03:05  smackie
 * Fold back Arizona_branch into mainline.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************************
* $Header: /SecureLAN/SDU/Include/CSDATA.H 20    7/28/95 1:31p Peacock 
*
* DESCRIPTION:
*    This is the header file for csdata.c where all common globals are stored.
*
* Copyright(c)  Cylink Corporation 1994.  All rights reserved.
*******************************************************************************
* $History: CSDATA.H 
* 
* *****************  Version 20  *****************
* User: Peacock      Date: 7/28/95    Time: 1:31p
* Updated in $/SecureLAN/SDU/Include
* removed the bit in csAvail for socket table entries.
* 
* *****************  Version 19  *****************
* User: Tchi         Date: 7/19/95    Time: 5:47p
* Updated in $/SecureLAN/SDU/Include
* alter the challenge number after signing the message
* 
* *****************  Version 18  *****************
* User: Tchi         Date: 6/30/95    Time: 5:01p
* Updated in $/SecureLAN/SDU/Include
* added hi_dh_shared
* 
* *****************  Version 17  *****************
* User: Tchi         Date: 6/09/95    Time: 10:26a
* Updated in $/SecureLAN/SDU/Include
* added two more globals
* 
* *****************  Version 16  *****************
* User: Tchi         Date: 6/06/95    Time: 5:17p
* Updated in $/SecureLAN/SDU/Include
* added max_pconn_period
* 
* *****************  Version 15  *****************
* User: Tchi         Date: 6/01/95    Time: 6:11p
* Updated in $/SecureLAN/SDU/Include
* changed to use NO_PEDEF_NNC_TABLE and NO_SR_TABLE
* 
* *****************  Version 14  *****************
* User: Sduhost      Date: 5/09/95    Time: 7:10p
* Updated in $/SecureLAN/SDU/Include
* removed unused variable
* 
* *****************  Version 13  *****************
* User: Sduhost      Date: 5/01/95    Time: 7:08p
* Updated in $/SecureLAN/SDU/Include
* added more debugging information
* 
* *****************  Version 12  *****************
* User: Tchi         Date: 4/20/95    Time: 7:42p
* Updated in $/SecureLAN/SDU/Include
* CS config data resouce locking; csAvail to check if CS can start
* 
* *****************  Version 11  *****************
* User: Tchi         Date: 4/18/95    Time: 11:45a
* Updated in $/SecureLAN/SDU/Include
* SSNMP and EP/CS TABLE changes
* 
* *****************  Version 10  *****************
* User: Tchi         Date: 2/16/95    Time: 6:56p
* Updated in $/SecureLAN/SDU/Include
* change the addresses of the last two NNC msg
* 
* *****************  Version 9  *****************
* User: Bald_eagle   Date: 2/13/95    Time: 1:13p
* Updated in $/SecureLAN/SDU/Include
* Modified HMA index type from ushort to short
* 
* *****************  Version 8  *****************
* User: Tchi         Date: 2/04/95    Time: 8:59a
* Updated in $/SecureLAN/SDU/Include
* CS and Authentication interface
* 
* *****************  Version 7  *****************
* User: Tchi         Date: 1/12/95    Time: 6:01p
* Updated in $/SecureLAN/SDU/Include
* changed to ep_frm.h
* 
* *****************  Version 6  *****************
* User: Tchi         Date: 12/19/94   Time: 7:23p
* Updated in $/SecureLAN/SDU/Include
* MIB changes
* 
* *****************  Version 5  *****************
* User: Tchi         Date: 11/11/94   Time: 4:22p
* Updated in $/SecureLAN/SDU/Include
* added a few new global error variables
* 
* *****************  Version 4  *****************
* User: Tchi         Date: 11/08/94   Time: 4:08p
* Updated in $/SecureLAN/SDU/Include
* added DSS
* 
* *****************  Version 3  *****************
* User: Tchi         Date: 11/04/94   Time: 3:54p
* Updated in $/SecureLAN/SDU/Include
* modifed sdu_srng_tb; added sdu_srng_tb_entries
* 
* *****************  Version 2  *****************
* User: Tchi         Date: 10/28/94   Time: 5:30p
* Updated in $/SecureLAN/SDU/Include
* Integration 1.a bug fix
* 
* *****************  Version 1  *****************
* User: Tchi         Date: 10/25/94   Time: 8:47p
* Created in $/SecureLAN/SDU/Include
* created
*******************************************************************************/

#ifndef CSDATA_H
#define CSDATA_H

/*--------------------------------- includes ---------------------------------- */
#include "../crypto/sdu/include/cylink.h"
#include "../crypto/sdu/include/connstup.h"
#include "../crypto/sdu/include/ep_frm.h"

/****************************************************************************
* STRUCTURES
*****************************************************************************/
typedef struct
{
	ulong	mask;		/*set by current CS config parameter in SNMP SetReq */
	ulong	bitMap;		/*each bit represents if the associated cofig param is set */
}
csAvailDef;

/****************************************************************************
* CONSTANTS
*****************************************************************************/
#define CS_PV_TB_AVAIL				0x00000001L		/*bit map representation */
#define CS_PACT1_TB_AVAIL           0x00000002L
#define CS_PACT2_TB_AVAIL			0x00000004L
#define CS_SK_MASK_TB_AVAIL			0x00000008L
#define CS_SACT1_TB_AVAIL			0x00000010L
#define CS_SACT2_TB_AVAIL			0x00000020L
#define CS_SRNG_TB_AVAIL			0x00000040L
/*#define CS_SRNG_TB_ENTRIES_AVAIL	0x00000080L */

#define CS_CONFIG_TB_AVAIL			0x0000007FL		/*sum of all the above */



#ifdef INTGR_DEBUG	/*TEST ONLY */
	#define CS_TEST_SFU_SNIC			0x00000001L
	#define CS_TEST_RETX				0x00000002L
	#define CS_TEST_RACE_DROP_FRAME 	0x00000004L
	#define CS_TEST_PCONN_NOT_EXIST		0x00000008L
	#define CS_TEST_PCONN_FREE_NULL		0x00000010L
	#define CS_TEST_BAD_NNC_FRM_TYPE	0x00000020L
	#define CS_TEST_NO_NW_ADDR			0x00000040L
#endif

/*******************************************************************************
* EXTERNAL DATA DECLARATIONS
*******************************************************************************/
#ifndef CSDATA_C

extern pconn_entry pconn_list[PCONN_ENTRIES];
extern short       pconn_used;
extern short       pconn_free;

#ifdef INTGR_DEBUG
extern long  		ep_post_cs_count;
extern long  		cs_transmit_count;
extern long  		pa_post_cs_count;
extern long  		race_count;
extern ulong 		cs_debug;
#endif

extern uchar       	nnc_new_tb[MAX_NNC_BUF_SIZE];
extern uchar       	nnc_old_tb[MAX_NNC_BUF_SIZE];
extern uchar	   	nnc_sys_oc[MAX_NNC_SIZE];
extern csAvailDef  	csAvail;

#ifndef CISCO
extern ulong	   	cs_unique_conn_id;
#endif
/*extern short      hi_challenge_num; */

extern ushort	   	nncIndexSetGet;	/*NNC index in nnc_new_tb and nnc_old_tb */
									/*to Set or Get. */

extern uchar		syndrome[CHALLENGE_SIZE];	/*scratch pad for DH share syndrome */

/*-------------------------- MIB variables ----------------------------------- */
extern ulong       cs_err;
extern entity_pair cs_err_pconn_timeout;
extern entity_pair cs_err_no_tx_buf;
extern entity_pair cs_err_no_pconn_entry;
extern entity_pair cs_err_nnc_exp;
extern entity_pair cs_err_nnc_sig;
extern entity_pair cs_err_su_sig;
/*extern entity_pair cs_err_auth_api; */
extern entity_pair cs_err_challenge_odd;
extern entity_pair cs_err_challenge_cmp;
extern entity_pair cs_err_ncc_grps;
extern entity_pair cs_err_gen_chl;
extern entity_pair cs_err_gen_dh;
extern entity_pair cs_err_gen_sig;
extern entity_pair cs_err_alloc_hm;
extern entity_pair cs_err_release_hm;
extern entity_pair cs_err_cy_keys;
extern entity_pair cs_err_sok_mask_rng;
extern entity_pair cs_err_verify_sig;
extern entity_pair cs_err_nnc_frm_size;
extern entity_pair cs_err_transmit;

extern uchar	   sdu_nnc[MAX_NNC_SIZE];
#ifdef NO_PEDEF_NNC_TABLE
extern int  	   sdu_nnc_index;
extern int         sdu_nnc_rev;
#endif

extern ulong       sdu_pv_tb[MAX_GRPS];
extern ulong       sdu_pact1_tb[MAX_GRPS];
extern ulong       sdu_pact2_tb[MAX_GRPS];
extern long        sdu_sk_mask_tb[MAX_GRPS];
extern ulong       sdu_sact1_tb[MAX_GRPS];
extern ulong       sdu_sact2_tb[MAX_GRPS];
extern srng_tb_def sdu_srng_tb[MAX_PRTCLS];
extern int         sdu_srng_tb_entries;

extern ulong       sdu_conn_built;
extern ulong       sdu_pconns;
extern ushort	   sdu_failed_conn_retry;
extern ulong	   max_pconn_period;

extern long 	   nncTbRowStatus; 	/*current NNC table and epPEdef[] row status */
extern ushort	   cs_max_retransmissions;

#endif

#endif
/*-------------------------------- END OF FILE ------------------------------- */
