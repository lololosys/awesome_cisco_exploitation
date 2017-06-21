/* $Id: csdata.c,v 1.1.68.1 1996/04/19 14:57:57 che Exp $
 * $Source: /release/112/cvs/Xsys/crypto/sdu/task/cs/csdata.c,v $
 *------------------------------------------------------------------
 * Cylink Source code: csdata.c
 *
 * August 1995, Brian Weis
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: csdata.c,v $
 * Revision 1.1.68.1  1996/04/19  14:57:57  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commits
 *
 * Revision 1.1.2.2  1996/03/13  22:45:34  bew
 * Branch: IosSec_branch
 * The RSP/VIP environment (multiple crypto engines) are now supported
 * in crypto parser routines and crypto session mgmt code.
 *
 * Revision 1.1.2.1  1995/11/28  01:49:59  bew
 * Branch: IosSec_branch
 * Port of newest Cylink SDU code, plus CS cleanup. The SDU code
 * is moved to its original Cylink tree structure.
 *
 * Revision 1.1  1995/11/22  23:22:25  bew
 * Placeholders for new SDU file locations.
 *
 * Revision 1.7  1995/06/07 12:03:05  smackie
 * Fold back Arizona_branch into mainline.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************************
* $Header: /SecureLAN/SDU/Task/CS/CSDATA.C 21    7/19/95 5:49p Tchi 
*
* DESCRIPTION:	This file has all global data used by different Connection
*               Setup files.
*
* Copyright(c)  Cylink Corporation 1994.  All rights reserved.
*******************************************************************************
* $History: CSDATA.C 
* 
* *****************  Version 21  *****************
* User: Tchi         Date: 7/19/95    Time: 5:49p
* Updated in $/SecureLAN/SDU/Task/CS
* alter the challenge number after signing the message
* 
* *****************  Version 20  *****************
* User: Tchi         Date: 6/30/95    Time: 5:04p
* Updated in $/SecureLAN/SDU/Task/CS
* added hi_dh_shared
* 
* *****************  Version 19  *****************
* User: Tchi         Date: 6/09/95    Time: 10:28a
* Updated in $/SecureLAN/SDU/Task/CS
* added two more global vaiables
* 
* *****************  Version 18  *****************
* User: Tchi         Date: 6/06/95    Time: 5:20p
* Updated in $/SecureLAN/SDU/Task/CS
* added max_pconn_period
* 
* *****************  Version 17  *****************
* User: Tchi         Date: 6/01/95    Time: 6:09p
* Updated in $/SecureLAN/SDU/Task/CS
* changed to use NO_PEDEF_NNC_TABLE and NO_SR_TABLE
* 
* *****************  Version 16  *****************
* User: Sduhost      Date: 5/09/95    Time: 7:13p
* Updated in $/SecureLAN/SDU/Task/CS
* removed hi_challenge_num
* 
* *****************  Version 15  *****************
* User: Sduhost      Date: 5/01/95    Time: 7:09p
* Updated in $/SecureLAN/SDU/Task/CS
* added more debugging variables
* 
* *****************  Version 14  *****************
* User: Tchi         Date: 4/20/95    Time: 7:48p
* Updated in $/SecureLAN/SDU/Task/CS
* config data resouce locking; csAvail to check if CS can start
* 
* *****************  Version 13  *****************
* User: Tchi         Date: 4/18/95    Time: 11:47a
* Updated in $/SecureLAN/SDU/Task/CS
* SSNMP and EP/CS TABLE changes
* 
* *****************  Version 12  *****************
* User: Tchi         Date: 2/16/95    Time: 6:58p
* Updated in $/SecureLAN/SDU/Task/CS
* changed the addresses of the last two NNC msg
* 
* *****************  Version 11  *****************
* User: Bald_eagle   Date: 2/13/95    Time: 1:14p
* Updated in $/SecureLAN/SDU/Task/CS
* Modified HMA index type from ushort to short
* 
* *****************  Version 10  *****************
* User: Tchi         Date: 2/04/95    Time: 9:16a
* Updated in $/SecureLAN/SDU/Task/CS
* CS to Authentication interface
* 
* *****************  Version 9  *****************
* User: Sduhost      Date: 1/20/95    Time: 11:29a
* Updated in $/SecureLAN/SDU/Task/CS
* All data is made into FAR type, and different Data Segment is Created.
* 
* *****************  Version 8  *****************
* User: Tchi         Date: 1/13/95    Time: 10:39a
* Updated in $/SecureLAN/SDU/Task/CS
* changed to ep_frm.h
* 
* *****************  Version 7  *****************
* User: Tchi         Date: 12/19/94   Time: 7:24p
* Updated in $/SecureLAN/SDU/Task/CS
* MIB changes
* 
* *****************  Version 6  *****************
* User: Tchi         Date: 11/11/94   Time: 4:25p
* Updated in $/SecureLAN/SDU/Task/CS
* retransmission, bypass NNC frame from different product,...
* 
* *****************  Version 5  *****************
* User: Tchi         Date: 11/08/94   Time: 4:09p
* Updated in $/SecureLAN/SDU/Task/CS
* added DSS
* 
* *****************  Version 4  *****************
* User: Tchi         Date: 11/04/94   Time: 3:55p
* Updated in $/SecureLAN/SDU/Task/CS
* modifed sdu_srng_tb; added sdu_srng_tb_entries
* 
* *****************  Version 3  *****************
* User: Tchi         Date: 11/01/94   Time: 2:11p
* Updated in $/SecureLAN/SDU/Task/CS
* use MAX_NNC_BUF_SIZE
* 
* *****************  Version 2  *****************
* User: Tchi         Date: 10/28/94   Time: 5:32p
* Updated in $/SecureLAN/SDU/Task/CS
* Integration 1.a bug fix
* 
* *****************  Version 1  *****************
* User: Tchi         Date: 10/25/94   Time: 8:50p
* Created in $/SecureLAN/SDU/Task/CS
* created
*******************************************************************************/

#define CSDATA_C

/*******************************************************************************
* INCLUDE FILES
*******************************************************************************/
#include "../crypto/sdu/include/cylink.h"
#include "../crypto/sdu/include/connstup.h"
#include "../crypto/sdu/include/ep_frm.h"
#include "../crypto/sdu/include/csdata.h"

#pragma option -zECS_FAR_DATA -zF* -zH*

#ifdef INTGR_DEBUG	/*TEST ONLY */
long  far ep_post_cs_count = 0L;
long  far cs_transmit_count = 0L;
long  far pa_post_cs_count = 0L;
long  far race_count = 0L;
ulong far cs_debug = 0L;
#endif

/*************************************************************************
* COMMON GLOBAL DATA
**************************************************************************/
uchar   far    	nnc_new_tb[MAX_NNC_BUF_SIZE];	/*first two are special NNCs */
uchar   far    	nnc_old_tb[MAX_NNC_BUF_SIZE];	/*first two are special NNCs */
uchar	far	   	nnc_sys_oc[MAX_NNC_SIZE];	/*fake NNC with system open-close flag */
csAvailDef far	csAvail;	/*determines if all CS config data are available */

#ifndef CISCO
/*increment every time a new pending connection was built. */
ulong	far	   	cs_unique_conn_id;
#endif

/*short   far    hi_challenge_num; //HM index for the challenge number */
ushort	far   	nncIndexSetGet;	 /*NNC index in nnc_new_tb and nnc_old_tb */
uchar	far		syndrome[CHALLENGE_SIZE];	/*scratch pad for DH share syndrome */

/*******************************************************************************
* MIB GLOBAL DATA
*******************************************************************************/
ulong   far    cs_err;                 /* error flag */

/*  The following MIB variables will be written over if more than one pair of */
/*entities are involved in the error condition */
/*  They are meaningful only if the bit in cs_err is set. */

entity_pair far  cs_err_pconn_timeout;   /* failed connection pair of nodes. */
entity_pair far cs_err_no_tx_buf;       /* failed transmission of NNC request */
entity_pair far cs_err_no_pconn_entry;  /* pending conn list full */
entity_pair far cs_err_nnc_exp;         /* NNC revision expired */
entity_pair far cs_err_nnc_sig;         /* failed NNC signature */
entity_pair far cs_err_su_sig;          /* failed SU signature */
/*entity_pair far cs_err_auth_api;		// Authentication (V.S.) failure */
entity_pair far cs_err_challenge_odd;   /* bad odd challenge number */
entity_pair far cs_err_challenge_cmp;   /* bad complete challenge number */
entity_pair far cs_err_ncc_grps;        /* the number of NNC groups is different */
entity_pair far cs_err_gen_chl;         /* cy failed to generate challenge number */
entity_pair far cs_err_gen_dh;          /* cy failed to generate DH number pair */
entity_pair far cs_err_gen_sig;         /* cy failed to generate signature */
entity_pair far cs_err_alloc_hm;        /* cy failed to allocate hidden memory */
entity_pair far cs_err_release_hm;      /* cy failed to release hidden memory */
entity_pair far cs_err_cy_keys;         /* cy failed to get keys or anything related */
entity_pair far cs_err_sok_mask_rng;    /* socket mask table and range table not consistent */
entity_pair far cs_err_verify_sig;      /* cy failed to verify signature */
entity_pair far cs_err_nnc_frm_size;    /* nnc frame size is short */
entity_pair far cs_err_transmit;        /* csTransmit failed */

/*WARNING: NOTICE: */

/*The special UPE's NNC must have a zeroed Group Membership and */
/*a open-close flag equal to the SDU's open-close system flag. */

/*download one NNC at a time */

/*SNMP SetReq: object1=NNC table index; object2=NNC */

/*SNMP GetReq: in order to do this, we need a SetReq beforehand with */
/*			    object1=NNC table index; object2=NNC revision. */
/*				Then SM could send a GetReq with one object=NNC */

#ifdef NO_PEDEF_NNC_TABLE
int		far	   sdu_nnc_index;
int		far	   sdu_nnc_rev;
#endif
uchar	far	   sdu_nnc[MAX_NNC_SIZE];

ulong   far    sdu_pv_tb[MAX_GRPS];
ulong   far    sdu_pact1_tb[MAX_GRPS];
ulong   far    sdu_pact2_tb[MAX_GRPS];
long    far    sdu_sk_mask_tb[MAX_GRPS];
ulong   far    sdu_sact1_tb[MAX_GRPS];
ulong   far    sdu_sact2_tb[MAX_GRPS];

srng_tb_def far sdu_srng_tb[MAX_PRTCLS];
int    far     	sdu_srng_tb_entries;

ulong   far    	sdu_conn_built;
ulong   far    	sdu_pconns;
/*minutes that a failed connection's COT entry should be deleted so */
/*SDU could try to establish the same connection again as a second chance. */
ushort	far    	sdu_failed_conn_retry = 30;				/*minutes */
ulong	far	   	max_pconn_period = MAX_PCONN_TIME;		/*seconds */

long 	far   	nncTbRowStatus; 	/*current NNC table and epPEdef row status */
ushort	far		cs_max_retransmissions = DEFAULT_RETRANSMISSIONS;

/*---------------------------- END OF FILE -------------------------------- */
