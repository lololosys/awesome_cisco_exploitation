/* $Id: csutil2.c,v 1.1.68.7 1996/09/11 23:23:12 bew Exp $
 * $Source: /release/112/cvs/Xsys/crypto/sdu/task/cs/csutil2.c,v $
 *------------------------------------------------------------------
 * Cylink Source code: csutil2.c
 *
 * August 1995, Brian Weis
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: csutil2.c,v $
 * Revision 1.1.68.7  1996/09/11  23:23:12  bew
 * CSCdi68438:  crypto determination of network address should be done
 * earlier
 * Branch: California_branch
 * Run the routing tables during connection message setup to determine
 * which interface the packet will go out.
 *
 * Revision 1.1.68.6  1996/09/10  23:25:06  bew
 * CSCdi64360:  Bad and pending connection does not timeout
 * Branch: California_branch
 * When a pending connecttion times out, clear out the connection table
 * as well as pending connection table.
 *
 * Revision 1.1.68.5  1996/07/01  09:07:14  xliu
 * CSCdi61744:  fail to select correct crypto maps when multiple are
 * created
 * Branch: California_branch
 *
 * Revision 1.1.68.4  1996/06/28  23:40:54  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 1.1.68.3  1996/06/17  22:25:09  bew
 * CSCdi60484:  error_proc multiply defined in APPN and Cylink crypto code
 * Branch: California_branch
 * Change error_proc() to Error_proc() in Cylink code.
 *
 * Revision 1.1.68.2  1996/06/07  00:27:44  bew
 * CSCdi59169:  Crypto cleanup work
 * Branch: California_branch
 * Cleanup to let compile with -Wall.
 *
 * Revision 1.1.68.1  1996/04/19  14:58:00  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commits
 *
 * Revision 1.1.2.7  1996/04/16  21:54:14  bew
 * Branch: IosSec_branch
 * General code cleanup. Implemented checking of crypto map peers. CIM now
 * addressed to the src/dst rather than pe/upe.
 *
 * Revision 1.1.2.6  1996/03/25  18:44:05  bew
 * Branch: IosSec_branch
 * Added 40-bit DES and DES 8-bit CFB support. Implemented partial
 * challenge processing support, not yet compatible with SDU.
 * Updated the crypto methods vector per Cylink's new definitions.
 *
 * Revision 1.1.2.5  1996/03/13  22:45:37  bew
 * Branch: IosSec_branch
 * The RSP/VIP environment (multiple crypto engines) are now supported
 * in crypto parser routines and crypto session mgmt code.
 *
 * Revision 1.1.2.4  1996/01/18  23:23:44  bew
 * Branch: IosSec_branch
 * The crypto session manager no longer forks off connection messages but
 * handles them itself serially. This is due to SDU connection setup code
 * not handling the multitasking gracefully.
 *
 * Revision 1.1.2.3  1995/12/20  23:32:08  bew
 * Branch: IosSec_branch
 * Context is now freed after a sign operation. Before the callback is
 * called a pointer to the pak is taken so it can be used after the
 * callback returns.
 *
 * Revision 1.1.2.2  1995/12/12  23:12:30  bew
 * Branch: IosSec_branch
 * Better formatting of debug messages.
 *
 * Revision 1.1.2.1  1995/11/28  01:50:03  bew
 * Branch: IosSec_branch
 * Port of newest Cylink SDU code, plus CS cleanup. The SDU code
 * is moved to its original Cylink tree structure.
 *
 * Revision 1.1  1995/11/22  23:22:27  bew
 * Placeholders for new SDU file locations.
 *
 * Revision 1.7  1995/06/07 12:03:05  smackie
 * Fold back Arizona_branch into mainline.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************************
* $Header: /SecureLAN/SDU/Task/CS/CSUTIL2.C 41    7/21/95 5:08p Sduhost 
*
* DESCRIPTION:  This file has all utility functions used by the Connection
*               setup module.
*
* Copyright(c)  Cylink Corporation 1994.  All rights reserved.
*******************************************************************************
* $History: CSUTIL2.C 
* 
* *****************  Version 41  *****************
* User: Sduhost      Date: 7/21/95    Time: 5:08p
* Updated in $/SecureLAN/SDU/Task/CS
* Added a include file from snmp directory.
* 
* *****************  Version 40  *****************
* User: Tchi         Date: 7/19/95    Time: 5:49p
* Updated in $/SecureLAN/SDU/Task/CS
* alter the challenge number after signing the message
* 
* *****************  Version 39  *****************
* User: Tchi         Date: 6/30/95    Time: 5:05p
* Updated in $/SecureLAN/SDU/Task/CS
* fixed the bug that EP Custodian never releases the hidden memory area
* used by two keys in cib->crypto; but CS keeps allocating hidden memory.
* 
* *****************  Version 38  *****************
* User: Tchi         Date: 6/14/95    Time: 11:47a
* Updated in $/SecureLAN/SDU/Task/CS
* fixed create_cib for unmanaged SDU
* 
* *****************  Version 37  *****************
* User: Tchi         Date: 6/06/95    Time: 5:20p
* Updated in $/SecureLAN/SDU/Task/CS
* use MIB variable max_pconn_period instead of a fixed constant
* MAX_PCONN_TIME
* 
* *****************  Version 36  *****************
* User: Tchi         Date: 6/06/95    Time: 2:15p
* Updated in $/SecureLAN/SDU/Task/CS
* fixed bug #23: for NNCs with the same customer IDs, scan all 32 bits
* (start from MSB) in crypto method.
* 
* *****************  Version 35  *****************
* User: Tchi         Date: 6/01/95    Time: 6:10p
* Updated in $/SecureLAN/SDU/Task/CS
* changed to use NO_PEDEF_NNC_TABLE and NO_SR_TABLE
* 
* *****************  Version 34  *****************
* User: Tchi         Date: 5/26/95    Time: 9:59a
* Updated in $/SecureLAN/SDU/Task/CS
* changed BLK_READ | BLK_WRITE to BLK_NOACCESS
* 
* *****************  Version 33  *****************
* User: Tchi         Date: 5/24/95    Time: 2:56p
* Updated in $/SecureLAN/SDU/Task/CS
* (1) Socket Range Table changes; (2) check for bridged traffic 
* 
* *****************  Version 32  *****************
* User: Tchi         Date: 5/17/95    Time: 7:57p
* Updated in $/SecureLAN/SDU/Task/CS
* get rid of compiler warning about hReadRandom
* 
* *****************  Version 31  *****************
* User: Sduhost      Date: 5/14/95    Time: 3:02p
* Updated in $/SecureLAN/SDU/Task/CS
* fixed so that get_nnc_size is called only once on the received NNC
* 
* *****************  Version 30  *****************
* User: Sduhost      Date: 5/09/95    Time: 7:14p
* Updated in $/SecureLAN/SDU/Task/CS
* changed to use hReadRandom
* 
* *****************  Version 29  *****************
* User: Sduhost      Date: 5/09/95    Time: 4:30p
* Updated in $/SecureLAN/SDU/Task/CS
* added pconn_retransmit
* 
* *****************  Version 28  *****************
* User: Sduhost      Date: 5/01/95    Time: 7:10p
* Updated in $/SecureLAN/SDU/Task/CS
* added more debugging information
* 
* *****************  Version 27  *****************
* User: Tchi         Date: 4/28/95    Time: 10:11a
* Updated in $/SecureLAN/SDU/Task/CS
* fix the part that fills crypto in CIB
* 
* *****************  Version 26  *****************
* User: Tchi         Date: 4/20/95    Time: 7:49p
* Updated in $/SecureLAN/SDU/Task/CS
* config data resouce locking; csAvail to check if CS can start
* 
* *****************  Version 25  *****************
* User: Sduhost      Date: 4/17/95    Time: 6:23p
* Updated in $/SecureLAN/SDU/Task/CS
* added "adj_active_conn_count"
* 
* *****************  Version 24  *****************
* User: Tchi         Date: 4/17/95    Time: 5:22p
* Updated in $/SecureLAN/SDU/Task/CS
* CS_ERR_AUTH_API is not used anymore
* 
* *****************  Version 23  *****************
* User: Sduhost      Date: 4/07/95    Time: 3:57p
* Updated in $/SecureLAN/SDU/Task/CS
* clear XCHG_KEY bit when returning a CIB
* 
* *****************  Version 22  *****************
* User: Tchi         Date: 3/27/95    Time: 4:54p
* Updated in $/SecureLAN/SDU/Task/CS
* replace KS_inqtime with GetRTClock; the logic to check error code
* returned by the latter will be done later
*
* *****************  Version 21  *****************
* User: Tchi         Date: 3/24/95    Time: 11:48a
* Updated in $/SecureLAN/SDU/Task/CS
* fix the bug of posting signature generation failure message to event
* mgr.
* 
* *****************  Version 20  *****************
* User: Tchi         Date: 2/23/95    Time: 2:03p
* Updated in $/SecureLAN/SDU/Task/CS
* fix the NNC frame signature signing and verification
* 
* *****************  Version 19  *****************
* User: Sduhost      Date: 2/23/95    Time: 11:11a
* Updated in $/SecureLAN/SDU/Task/CS
* bug fix
* 
* *****************  Version 18  *****************
* User: Tchi         Date: 2/16/95    Time: 6:58p
* Updated in $/SecureLAN/SDU/Task/CS
* changed the addresses of the last two NNC msg
* 
* *****************  Version 17  *****************
* User: Sduhost      Date: 2/15/95    Time: 3:00p
* Updated in $/SecureLAN/SDU/Task/CS
* Changes in Crypto API - Elimination of Random Indexes.
* 
* *****************  Version 16  *****************
* User: Bald_eagle   Date: 2/13/95    Time: 1:38p
* Updated in $/SecureLAN/SDU/Task/CS
* Modified spelling of VerDSSSig...
* 
* *****************  Version 15  *****************
* User: Bald_eagle   Date: 2/13/95    Time: 1:31p
* Updated in $/SecureLAN/SDU/Task/CS
* Modified HMA index type from ushort to short
*
* *****************  Version 14  *****************
* User: Tchi         Date: 2/12/95    Time: 5:50p
* Updated in $/SecureLAN/SDU/Task/CS
* error handling for IO Mgr
* 
* *****************  Version 13  *****************
* User: Tchi         Date: 2/07/95    Time: 7:01p
* Updated in $/SecureLAN/SDU/Task/CS
* fixed the problem that open-close bit in CIB status is not cleared to
* zero when this CIB is reused.
* 
* *****************  Version 12  *****************
* User: Tchi         Date: 2/04/95    Time: 9:16a
* Updated in $/SecureLAN/SDU/Task/CS
* CS to Authentication interface
* 
* *****************  Version 11  *****************
* User: Tchi         Date: 1/20/95    Time: 5:40p
* Updated in $/SecureLAN/SDU/Task/CS
* intgra 2a
* 
* *****************  Version 10  *****************
* User: Tchi         Date: 1/19/95    Time: 6:39p
* Updated in $/SecureLAN/SDU/Task/CS
* fixed csTransmit
* 
* *****************  Version 9  *****************
* User: Tchi         Date: 1/13/95    Time: 10:47a
* Updated in $/SecureLAN/SDU/Task/CS
* changed to cy_frm.h
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
* User: Tchi         Date: 3/16/94    Time: 11:17a
* Updated in $/SecureLAN/SDU/Task/CS
* fixed crypto method table bug
* 
* *****************  Version 5  *****************
* User: Tchi         Date: 11/11/94   Time: 4:25p
* Updated in $/SecureLAN/SDU/Task/CS
* retransmission, bypass NNC frame from different product,...
*
* *****************  Version 4  *****************
* User: Tchi         Date: 11/08/94   Time: 4:09p
* Updated in $/SecureLAN/SDU/Task/CS
* added DSS
*
* *****************  Version 3  *****************
* User: Tchi         Date: 11/04/94   Time: 3:57p
* Updated in $/SecureLAN/SDU/Task/CS
* modified sdu_srng_tb; added sdu_srng_tb_entries
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

#define CSUTIL2_C

/*******************************************************************************
* INCLUDE FILES
*******************************************************************************/
#include "../crypto/sdu/include/cylink.h"
#ifndef CISCO
#include <mem.h>
#include <stdlib.h>
#endif
#include "../crypto/sdu/include/connstup.h"
#include "../crypto/sdu/include/csutil1.h"
#include "../crypto/sdu/include/csutil2.h"
#include "../crypto/sdu/include/ep_frm.h"
#include "../crypto/sdu/include/pa/pactl.h"
#ifndef CISCO
#include "../crypto/sdu/include/rtxcapi.h"
#endif
#include "../crypto/sdu/include/csdata.h"
#include "../crypto/sdu/include/cy_frm.h"
#ifndef CISCO
#include "../crypto/sdu/include/phybufm.h"
#endif
#include "../crypto/sdu/include/mgdata.h"
#include "../crypto/sdu/include/cy_api.h"
#include "../crypto/sdu/include/debug.h"
#include "../crypto/sdu/include/auth/authext.h"
#ifndef CISCO
#include "../crypto/sdu/include/snmp_imp.h"
#endif

#ifdef CISCO
#include "../crypto/crypto_glue_structs.h"
#include "../crypto/crypto_glue_sdu.h"
#include "../util/random.h"
#include "address.h"
#include "../crypto/crypto_map.h"
#endif

/*******************************************************************************
* PUBLIC DECLARATIONS (prototypes)
*******************************************************************************/
void    create_cib(short, uchar *, uchar, uchar, uchar);
uchar   check_nnc_sig(uchar *, short, int, ushort, ulong);
uchar   check_frame_sig(short, uchar *, uchar *, ushort);
int     good_challenge_odd(uchar *, short);
int     good_challenge_cmp(uchar *, short);
uchar * get_nnc(Pa2EpFrameControl *, uchar *, int *);
void    gen_DH_pub_secret(uchar *, short, uchar *, Pa2EpFrameControl *);
void    gen_challenge_num(uchar *, uchar *, uchar *, Pa2EpFrameControl *);
void    gen_signature(uchar *, uchar *, EntityID *, EntityID *,
					  uchar *, uchar, uchar *, ushort);
int     pconn_exist(short, ulong, ushort);
void    pconn_mgmt(void);
void 	pconn_retransmit(short);
void 	time_conn_fast(CIBptr);
void 	release_pend_cib(CIBptr);
uchar	*getInterEnterpriseSMdssPublic(uchar *, uchar *);

/*******************************************************************************
* STATIC DECLARATIONS (prototypes)
*******************************************************************************/
static void analyze_cib_crypto( int, CIBptr, ushort, long *, short,
								uchar *, int *);
static int get_cy_method_keys(CIBptr, short, uchar *);
static int get_cy_method(short, uchar *);
static int get_cy_keys(CIBptr, short);
static void proc_failed_pconn(short, long, entity_pair *);
static uchar good_signature(uchar *, uchar *, short, uchar *);
static void handle_pv_cy_fail(int, CIBptr, ushort);
static void handle_sk_cy_fail(int, CIBptr, ushort);

/**************************************************************************
* EXTERNALS
***************************************************************************/
extern ushort hDssKSeedHolder;

/*******************************************************************************
* NAME:             create_cib
*
* DESCRIPTION:      This function creates the CIB block for EP. NOTICE THAT
*                   this function does not clear the pending bit in CIB status.
*
* INPUTS:           index - pending connection list entry. non-NULL_INDEX.
*                   pdu - point to where the received NNC msg
*
* OUTPUT:           CIB is created
*
* NOTE:             An example is:
*
*                  <cmmgrp>          long (32 groups)
*                           ----------------------------------
*                      [0]  |    block 1                     |
*                           ---------------------------------- num_blks = 2
*                      [1]  |    block 2             |       | num_grps = 50
*                           ----------------------------------
*                      [2]  |                                |
*                           ----------------------------------
*
*******************************************************************************/
void create_cib(	short index,
					uchar *pdu,
					uchar auth,
					uchar far_end_auth,
					uchar um_other_sdu)
{
	CIBptr          cib;
	uchar *         far_nnc;
	uchar *         far_nnc_gv;
	uchar *         local_nnc;
	uchar *         local_nnc_gv;
	uchar *         local_blk_ptr;
	uchar *         far_blk_ptr;
	pconn_entry *	pconn;
	long            cmmgrp[MAX_GRP_BLKS];    /*common group for two PEs */
	long            local_grp_member;
	long            local_root_leaf;
	long            local_meshiness;
	long            far_grp_member;
	long            far_root_leaf;
	long            far_meshiness;
	long            cs_sk_mask;     /*socket mask */
	long            cur_mask;
	ulong           cs_pv;          /*protocol vector */
	ulong           cs_pa1;         /*protocol action1 */
	ulong           cs_pa2;         /*protocol action2 */
	ulong           cs_sa1;         /*socket action1 */
	ulong           cs_sa2;         /*socket action1 */
	ushort          num_grps;     /*the number of security groups in local NNC */
	ushort          far_grps;     /*the number of security groups in far NNC */
	ushort          num_blks;     /*the number of 32-group blocks */
	ushort          cs_status;      /*CIB status */
	ushort          i;
	ushort          j;
	ushort          k;
	ushort          group;
	ushort          prtcl;
	int             found;
	int 			cy_info;
	uchar			um_this_sdu;
	uchar           cmmgrp_exists;

	/*******************************************************
	* Initialization
	********************************************************/
	pconn = pconn_list + index;
	cib =  pconn->cib;
	local_nnc = pconn->nnc;		/*non-null */
	far_nnc = pdu + PDU_FIX_NNC_OFFSET + dhPublicLen;
	local_nnc_gv =local_nnc + NNC_FIX_GRP_OFFSET + dssPkeyLen;
	far_nnc_gv = far_nnc + NNC_FIX_GRP_OFFSET + dssPkeyLen;

	cs_pv = 0;
	cs_status = (((ushort)local_nnc_gv[NNC_GRP_OC_OFFSET]) &
				 ((ushort)far_nnc_gv[NNC_GRP_OC_OFFSET])) << 8;
	/*cs_status is non-zero if both PEs are open */

	/***********************************************************
	* Check if one SDU is using the system flag
	************************************************************/
	if (!(auth & STAT_SYS_OC_FLAG) &&
		!(far_end_auth & STAT_SYS_OC_FLAG))
	{
		/********************************************************
		* Check the number of security groups in two NNCs
		*********************************************************/
		GET_PDU_USHORT(num_grps, local_nnc_gv);
		GET_PDU_USHORT(far_grps, far_nnc_gv);
		if (num_grps == far_grps)   /*the number of security groups should be same */
		{
			/****************************************************
			* Check for cryto failure
			*****************************************************/
			if (!(auth & STAT_CYOC_MASK) &&        /*my cryto status */
				!(far_end_auth & STAT_CYOC_MASK))  /*far end's crypto status */
			{
				/************************************************
				* Check for unmanaged SDU
				************************************************/
				cy_info = CY_NOT_COMPUTED;	/*initialization */
				um_this_sdu = memcmp(pdu + PDU_FIX_NNC_OFFSET + dhPublicLen + NNC_FIX_GRP_OFFSET,
									 nnc_sys_oc + NNC_FIX_GRP_OFFSET, dssPkeyLen)
							  ? 0 : 1;

				if ((!um_other_sdu && !um_this_sdu) ||
					((um_other_sdu || um_this_sdu) && cs_status))
				{	/*cs_status is both_open */

					num_blks = (num_grps + 31) >> 5;
					/*the next two point to first block */
					local_blk_ptr = local_nnc_gv + NNC_GRP_VEC_OFFSET;
					far_blk_ptr = far_nnc_gv + NNC_GRP_VEC_OFFSET;

					/**************************************************************
					* resolve group membership, root-leaf, and meshiness
					**************************************************************/
					cmmgrp_exists = 0;                              /*init */
					for (i = 0; i < num_blks; i++)
					{
						GET_GV_BLK(local_blk_ptr, local_grp_member,   /*macro to get */
								   local_root_leaf, local_meshiness); /*group vector */
						GET_GV_BLK(far_blk_ptr, far_grp_member,       /*components */
								   far_root_leaf, far_meshiness);     /*for two NNCs */

						cmmgrp[i] = ((local_root_leaf ^ far_root_leaf) |
									 (local_meshiness & far_meshiness)) &
									(local_grp_member & far_grp_member);

						if (cmmgrp[i])
						{
							cmmgrp_exists = 1;
						}

						local_blk_ptr += NNC_GV_BLK_SIZE;
						far_blk_ptr += NNC_GV_BLK_SIZE;
					}

        			/*****************************************************************
        			* resolve protocol vector, action, socket mask, range and action
					*****************************************************************/
    				if (cmmgrp_exists)
					{
						/*init */
						cs_pa1 = 0;
						cs_pa2 = 0;
						cs_sk_mask = 0;
    					cs_sa1 = 0;
						cs_sa2 = 0;

						for (i = 0; i < num_blks; i++)
						{
							group = i << 5;                         /*i * 32 */
							j = min(((i + 1) << 5), num_grps);      /*(i + 1) * 32 */
							while ((group < j) && (cmmgrp[i]))
        					{
								if (cmmgrp[i] < 0) /*a common group found */
								{
									cs_pv  |= sdu_pv_tb[group];
									cs_pa1 |= sdu_pact1_tb[group];
									cs_pa2 |= sdu_pact2_tb[group];
        
									if (sdu_sk_mask_tb[group])
    								{
										cur_mask = sdu_sk_mask_tb[group];
										cs_sk_mask |= cur_mask;
										cs_sa1 |= sdu_sact1_tb[group];
										cs_sa2 |= sdu_sact2_tb[group];

										for (prtcl = 31;
											 cur_mask != 0;
											 prtcl--, cur_mask <<= 1)
										{
											if (cur_mask < 0)
											{
    
											#ifdef NO_SR_TABLE
    											for (k = 0, found = 0;
													(k < sdu_srng_tb_entries) &&
													(!found);
													k++)
												{
    												if ((sdu_srng_tb[k].group ==
    													 group) &&
														(sdu_srng_tb[k].prtcl ==
														 prtcl))
											#else
												for (k = 0, found = 0;
													(k < MAX_PRTCLS) && (!found);
													k++)
												{
    												if ((sdu_srng_tb[k].rowstatus ==
														 ACTIVE) &&
														(sdu_srng_tb[k].group ==
														 group) &&
														(sdu_srng_tb[k].prtcl ==
														 prtcl))
    										#endif

    												{
#ifdef CISCO
                            /* Validity check! */
                            if (cib->socket) {
                              cib->socket->range[prtcl] =
                                sdu_srng_tb[k].skrng;
                            }
                            /*
                             * Unconditionally mark
                             * as found - that's the
                             * effect of the orig 
                             * code too.
                             */
                                found = 1;
#else
                             cib->socket->range[prtcl] =
                             sdu_srng_tb[k].skrng;
                                 found = 1;
#endif

													}
        										}

												/**************************************
												* prtcl = 31: IP/TCP,
												*         30: IP/UDP, ...., 13: NTBS
												***************************************/

												/**************************************
												* The socket range table wastes too
        										* much memomry to do it as the following:
												*   cib->socket->range[prtcl] =
												* sdu_srng_tb[group][prtcl];
												***************************************/

												if (!found)
        										{
													Error_proc(0,
															   0,
															   CS_ERR_SOK_MASK_RNG,
															   &cs_err_sok_mask_rng,
        													   &(pconn->pe),
															   &(pconn->upe));
        										}
        									}
										}
    								}
								}
								/*else: no common group found. continue to search. */

								/*continue while-loop */
								(cmmgrp[i]) <<= 1;
    							group++;

							}   /*end of while loop */
						}   /*end of for loop */

						if (cs_pv != 0)
						{
							/**********************************
							* analyze and fill CIB crypto
							***********************************/
							cib->act_msb = cs_pa1;
							cib->act_lsb = cs_pa2;
#ifdef CISCO
              /* Validity Check! */
              if (cib->socket) {
                  cib->socket->mask = cs_sk_mask;
                cib->socket->act_msb = cs_sa1;
                  cib->socket->act_lsb = cs_sa2;
              }
#else
                cib->socket->mask = cs_sk_mask;
              cib->socket->act_msb = cs_sa1;
                cib->socket->act_lsb = cs_sa2;
#endif

							/*analyze prtcl table: FIRST */
							analyze_cib_crypto( ANALYZE_PV,
												cib,
												cs_status,
												(long *)&cs_pv,
    											index,
												far_nnc,
												&cy_info);

							/*analyze socket mask table: SECOND */
							analyze_cib_crypto( ANALYZE_SV,
												cib,
												cs_status,
												(long *)&cs_pv,
												index,
												far_nnc,
												&cy_info);
						}
					}
					/*else: no common groups - create a CIB to either block or */
					/*      bypass depending on two NNC's open-close flags. */
				}

				if ((um_other_sdu || um_this_sdu) &&
					((!cs_status) ||   			/*not both open, */
					 (cy_info != CY_SUCCESS)))  /*or both open but cannot */
				{                               /*build crypto connection */
					cs_pv = 0L;
					cs_status &= (~OPEN_NODE);  /*create a blocking CIB. */
				}
			}
			/*else: cryto failure- create a CIB to either block or bypass */
			/*      depending on two NNC's open-close flags. Function */
			/*      Error_proc has already been called to log the error. */
		}
		else  /*the far end should dectect the same inconsistency */
		{
			Error_proc(0,
					   0,
					   CS_ERR_NNC_GRPS,
					   &cs_err_ncc_grps,
					   &(pconn->pe),
					   &(pconn->upe));
		}
	}

	/*****************************************************************
	* fill CIB - protocol vector and status
	******************************************************************/
	cib->pvector = cs_pv;
	/*clear the open-close bit in the status flag of CIB */
	(cib->status) &= (~OPEN_NODE);
	(cib->status) |= cs_status;		/*do bit or, don't do assignment */
}

/*******************************************************************************
*
* CIB protocol actions, socket mask table, and socket action
* table have to be set before this function is called.
*
*******************************************************************************/
static void analyze_cib_crypto( int				type,
								CIBptr			cib,
								ushort			status,
								long *			p_pv,
								short			index,
								uchar *			far_nnc,
								int *			cy_info)
{
	long	vector;
	long    action1;
	long	action2;
	int 	bit;

	/************************************************************************
	* Initialization 
	************************************************************************/
	if (type == ANALYZE_PV)
	{
		vector = *p_pv;
		action1 = cib->act_msb;
		action2 = cib->act_lsb;
	}
	else
	{
#ifdef CISCO
    /* Validicy Check! */
    if (!cib->socket) {
      return;
      }
#endif
		vector = cib->socket->mask;
		action1 = cib->socket->act_msb;
		action2 = cib->socket->act_lsb;
	}

	/************************************************************************
	* Analyze protocol table or socket mask table by scanning all 32 bits
	************************************************************************/
	for (bit = 0;
		 (vector != 0L) && (bit < 32);
		 vector <<= 1, action1 <<= 1, action2<<= 1, bit++)
	{
		if ((vector < 0L) && ((action1 & 0x80000000L) == 0) && (action2 < 0L))
		{
			/*We need to encrypt this protocol. Check if Cy info has been set. */
			if (*cy_info == CY_NOT_COMPUTED)
			{
				*cy_info = get_cy_method_keys(cib, index, far_nnc);
			}

			if (*cy_info == CY_FAILED) {
				if (type == ANALYZE_PV)
				{
					handle_pv_cy_fail(bit, cib, status);
				}
				else
				{
					handle_sk_cy_fail(bit, cib, status);
				}
			}
		}
	}
}

/****************************************************************************
*
* Change protocol action bits to be the same as open-close flag in CIB status
*
****************************************************************************/
static void handle_pv_cy_fail(	int 	bit,
								CIBptr	cib,
								ushort 	status)
{
	ulong 	action;
	int		i;

	/*prepare the mask */
	if (status & OPEN_NODE)
	{
		/*to pass */
		action = 0x7FFFFFFFL;
		for (i = 0; i < bit; i++) action >>= 1;  /*find the bit position */
		(cib->act_msb) &= action;
		(cib->act_lsb) &= action;
	}
	else
	{
		/*to block */
		action = 0x80000000L;
		for (i = 0; i < bit; i++) action >>= 1;  /*find the bit position */
		(cib->act_msb) |= action;
		(cib->act_lsb) |= action;
	}
#ifdef CISCO
	/*
	 * It is reasonable then to clear the data out of the cib on which the 
	 * Cisco implementation depends. This make is very obvious that this 
	 * was a failed connection & any pre-existing connection is now invalid.
	 * crypto_glue_timeout_keys is called first to clear the connection_id
	 * from the policy & EPA.
	 */
    DEBUG("Connection failed due to incompatible policy");
	crypto_glue_timeout_keys(cib);
	crypto_glue_cleanup_cib(cib);
#endif
}

/***************************************************************************
*
* Compare open-close flag in status with the corresponding protocol action
* and choose the most restrictive one. That is, if any of those two is
* blocking, then create a blocking socket action. Otherwise create a pass
* socket action.
*
****************************************************************************/
static void handle_sk_cy_fail(	int 	bit,
								CIBptr	cib,
								ushort 	status )
{
	int	 	i;
	long 	mask = 0x80000000L;
	long 	action;

#ifdef CISCO
  /* Validity Check! */
  if (!cib->socket) {
    return;
  }
#endif

	for (i = 0; i < bit; i++) mask >>= 1;   /*find the bit position */

	if ((!(status & OPEN_NODE)) ||	/*close PE or */
		(cib->act_msb & mask))	   	/*prtcl action is to block */
	{
		/*to block */
		action = 0x80000000L;
		for (i = 0; i < bit; i++) action >>= 1;   /*find the bit position */
		(cib->socket->act_msb) |= action;
		(cib->socket->act_lsb) |= action;
	}
	else
	{
		/*to pass */
		action = 0x7FFFFFFFL;
		for (i = 0; i < bit; i++) action >>= 1;   /*find the bit position */
		(cib->socket->act_msb) &= action;
		(cib->socket->act_lsb) &= action;
	}
}

/***************************************************************************
*
*
****************************************************************************/
static int get_cy_method_keys(CIBptr cib, short index, uchar *far_nnc)
{
	int	ix;

	if ((ix = get_cy_method(index, far_nnc)) >= 0)           /*crypto method */
	{
		if (!get_cy_keys(cib, index)) /*two keys */
		{
			cib->crypto.method = cyMethodTbl[ix];
#ifdef CISCO
			if (crypto_glue_set_crypto_method(cib)) {
				return CY_SUCCESS;
			}
#endif
		}
	}

	return CY_FAILED;
}

/****************************************************************************
*
*
*
* RETURN:	the index to cyMethodTbl; -1 if not found.
*
****************************************************************************/
static int get_cy_method(short index, uchar *far_nnc)
{
	uchar *         local_nnc;
	long            far_cy_md;
	long            local_cy_md;
	long            cy_md;
	int             i;
#ifdef CISCO
	long local_cus_id, far_cus_id;
#endif

	/****************************************
	* fill CIB - crypto method
	****************************************/
	local_nnc = pconn_list[index].nnc;
	GET_PDU_LONG(far_cy_md,   far_nnc + NNC_CY_VEC_OFFSET);
	GET_PDU_LONG(local_cy_md, local_nnc + NNC_CY_VEC_OFFSET);
	cy_md = far_cy_md & local_cy_md;

#ifdef CISCO
	/* The original code gives alignment problems in an R4000 */
	GET_PDU_LONG(local_cus_id, local_nnc + NNC_CUS_ID_OFFSET);
	GET_PDU_LONG(far_cus_id, far_nnc + NNC_CUS_ID_OFFSET);
    if (local_cus_id == far_cus_id)
#else
	if ((*((ulong *)(local_nnc + NNC_CUS_ID_OFFSET))) ==
		(*((ulong *)(far_nnc + NNC_CUS_ID_OFFSET))))
#endif 
	{
		/*same customer ID, check MSB 16 bits of Net Sepcific Method first, */
		/*then check LSB 16 bits of Global Method */
		/*cy_md &= 0xFFFF0000L; */
		i = 31;
	}
	else
	{
		/*diff customer ID, check LSB 16 bits of Global Method only */
		cy_md &= 0x0000FFFFL;
		cy_md <<= 16;
		i = 15;
	}

	if (cy_md)
	{
		while (cy_md > 0)
		{
			cy_md <<= 1;
			i--;
		}
#ifdef CISCO 
		/* 
		 * Compensate for endian-ness, apparently.
		 * Look at DES for example. It is in the method array
		 * as 0x00010000, or the first algorithm of the top 16 bits.
		 * But we need to translate this into an index. The index
		 * counts from 0, where DES is the first bit in the array, or
		 * 0x80000000. See the problem? Sigh. So by the time we get done
		 * finding the bit above, if we are in the NSM set (1st 16 bits) we want
		 * to subtract 16 to get to the right bit in the first half of the 
		 * cyMethodTbl[]. If we were in the GM set (2nd 16 bits) then we need
		 * to place it in the second 16 bits. I think adding 16 does it.
		 */
		if (i>=16) {
	     	i -= 16;
		} else {
		    i += 16;
		}
#endif
		return i;	/*index into cyMethodTbl[] */
	}
	else
	{
		return -1;
	}
}

/****************************************************************************
* NAME:             get_cy_keys
*
* DESCRIPTION:      This function fills CIB with session key index, IV key
*                   index, and time.
*
* INPUTS:           cib - pointer to the block passed from EP
*                   index - pending connection list index
*                   cs_pv - pointer to the protocol vector
*
* OUTPUTS:          CIB is filled with two key indexes and time.
*
****************************************************************************/
static int get_cy_keys(	CIBptr cib,
						short index )
{
	ulong	tm = 0;
#ifndef CISCO
	int		error = 0;
	/*********************************************************************
	* Allocate Hidden Memory if necessary.
	* NOTE: EPC never frees hidden memory. (no matter rekey or recycle)
	*********************************************************************/
	if (!(cib->crypto.skey_i))				/*HM has never been allocated */
	{
		if (hMemAlloc(&(cib->crypto.skey_i),
						 CY_KEY_SIZE,
						 BLK_NOACCESS)) 	/*allocate HM for session key */
		{
			cib->crypto.skey_i = 0;
			error = -1;
		}
	}

	if (!(cib->crypto.ikey_i))				/*HM has never been allocated */
	{
		if (hMemAlloc(&(cib->crypto.ikey_i),
						 CY_KEY_SIZE,
						 LK_NOACCESS)) 	/*allocate HM for IV key */
		{
        	cib->crypto.ikey_i = 0;
			error = -1;
		}
	}
	if (error)
	{
		Error_proc(	0,
					0,
					CS_ERR_ALLOC_HM,
					&cs_err_alloc_hm,
					&pconn_list[index].pe,
					&pconn_list[index].upe);
		return error;
	}

	/*****************************************************************
	* Generate Cy keys
	******************************************************************/
	/*DH shared is already computed before this function is called */
	/*If the function to compute DH share number has failed, this function */
	/*will not be called. */
	if (!hSetDESKAPPAkey(dhPublicLen,
						 pconn_list[index].hi_dh_shared,
						 SKEY_OFFSET,
						 cib->crypto.skey_i)) /*make session key */
	{
		if (!hSetDESKAPPAkey(dhPublicLen,
							 pconn_list[index].hi_dh_shared,
							 IKEY_OFFSET,
							 cib->crypto.ikey_i)) /*make IV key */
		{
#endif /* CISCO */
			/***********************************************
			* All cryto API calls are succesful.
			************************************************/
			GetRTClock(&tm);
			cib->crypto.time = (ushort)(tm / 60); /*minutes */
			(cib->status) |= TIME_KEYS;
			return 0;
#ifndef CISCO
		}
	}

	/*crypto failure. generate Trap here. */
	Error_proc(0,
			   0,
			   CS_ERR_CY_KEYS,
			   &cs_err_cy_keys,
			   &pconn_list[index].pe,
			   &pconn_list[index].upe);
	return -1;
#endif /* !CISCO */
}
/*******************************************************************************
* NAME:             good_challenge_odd
*
* DESCRIPTION:      This function checks if the odd bits of the challenge
*                   number is bad
*
* INPUTS:           pdu - point to where the received connection message starts
*                   index - pending connection list index
*
* OUTPUT:           return 1 if the challenge number is good; 0 if it is bad
*
*******************************************************************************/
int good_challenge_odd(uchar * pdu, short index)
{
	int     ok;
    int     i;
	uchar   *frm;           /* pointer to the challenge number in received frame */
    uchar   *challenge;     /* pointer to the challenge number sent */

    frm = pdu + PDU_CHAL_OFFSET;
    challenge = (pconn_list[index].retx_buf)
                    + (pconn_list[index].l5_offset) + PDU_CHAL_OFFSET;

	for (i = 0, ok = 1; (i < CHALLENGE_SIZE) && ok; i++)
	{
        if ((frm[i] & CHAL_ODD_MASK) != challenge[i])
        {
            ok = 0;
            Error_proc(0,
                       0,
                       CS_ERR_CHALLENGE_ODD,
					   &cs_err_challenge_odd,
					   &pconn_list[index].pe,
                       &pconn_list[index].upe);
        }
    }

    return(ok);
}


/*******************************************************************************
* NAME:             good_challenge_cmp
*
* DESCRIPTION:      This function checks if the entire challenge number is bad
*
* INPUTS:           pdu - point to where the received connection message starts
*                   index - pending connection list index
*
* OUTPUT:           return 1 if the challenge number is good; 0 if it is bad
*
*******************************************************************************/
int good_challenge_cmp(uchar * pdu, short index)
{

    uchar   *challenge;     /* pointer to the challenge number sent */

    challenge = (pconn_list[index].retx_buf)
					+ (pconn_list[index].l5_offset) + PDU_CHAL_OFFSET;

	if (!memcmp(pdu + PDU_CFM_CHAL_OFFSET,
                challenge,
				CHALLENGE_SIZE))
    {
		return(1);      /*ok */
        
    }
    else
	{
        Error_proc(0,
                   0,
                   CS_ERR_CHALLENGE_CMP,
                   &cs_err_challenge_cmp,
                   &pconn_list[index].pe,
                   &pconn_list[index].upe);
        return(0);      /*not ok */

    }
}

/*******************************************************************************
* NAME:             check_nnc_sig
*
* DESCRIPTION:      This function checks if the NNC signature is bad
*
* INPUTS:           fc  - the frame control associated with the received NNC
*                         Request frame
*                   pdu - point to where the received connection message starts
*
* OUTPUT:           return 1 if the signature is bad; 0 if it is good
*
*******************************************************************************/
uchar check_nnc_sig( uchar 		*pdu,
					 short 		index,
					 int 		use_new_old,
					 ushort 	recv_nnc_size,
					 ulong		my_customer_id)
{
#ifndef CISCO
	uchar 	*recv_nnc;
	uchar	*sm_dss_public_num;
	uchar	*recv_customer_id;
#endif

#ifdef CISCO
	/*
	 * The NNC is unsigned, so for now just return 0. Eventually, should
	 * be handled gracefully.
	 */
	return(0);
#else /* !CISCO */
	recv_nnc = pdu + PDU_FIX_NNC_OFFSET + dhPublicLen;
	recv_customer_id = recv_nnc + NNC_CUS_ID_OFFSET;
	if (my_customer_id == (*((ulong *)recv_customer_id)))
	{
		sm_dss_public_num = getSMdssPublicKey(use_new_old);
	}
	else
	{
		sm_dss_public_num = getInterEnterpriseSMdssPublic(recv_customer_id,
												  recv_nnc + NNC_REV_OFFSET);
	}

	if (sm_dss_public_num != NULL)
	{
		if (!good_signature(recv_nnc,
							recv_nnc + recv_nnc_size - DEF_SIG_SIZE,
							index,
							sm_dss_public_num))
		{
			Error_proc(	0,
						0,
						CS_ERR_NNC_SIG,
						&cs_err_nnc_sig,
						&(pconn_list[index].pe),
						&(pconn_list[index].upe));
			return (AUTH_NNC_SIG_FAILED);     	/*not OK */
		}
		else
		{
			return(0);		/*OK */
		}
	}
	else
	{
		/*Error_proc(0, */
		/*		   0, */
		/*		   CS_ERR_AUTH_API, */
		/*		   &cs_err_auth_api, */
		/*		   &(pconn_list[index].pe), */
		/*		   &(pconn_list[index].upe)); */
		return(AUTH_NNC_SIG_FAILED);
	}
#endif /* !CISCO */
}

/*******************************************************************************
* NAME:             check_frame_sig
*
* DESCRIPTION:      This function checks if sending SU's signature is bad
*
* INPUTS:           sig - where the signature starts
*
* OUTPUT:           return 1 if the signature is bad; 0 if it is good
*
*******************************************************************************/
uchar check_frame_sig(short 			index,
					  uchar 			*pdu,
					  uchar 			*sdu_dss_public_num,
					  ushort			recv_nnc_size)
{
	uchar	*sig_start;

	switch (pdu[PDU_OP_OFFSET])
	{
		case ECHO_CONFIRM:		/*Confirm */
			sig_start = pdu + PDU_CFM_SIG_OFFSET;
			break;

		default :				/*Req, Reply */
			sig_start = pdu + PDU_FIX_NNC_OFFSET + dhPublicLen + recv_nnc_size;
			break;
	}

/* NOTE: Router is not happy with the following. It adds one extra byte to
		 Novell SAP frame so the following statement gives wrong pointer to
         the NNC frame signature.

	sig_start = ((uchar *)fc->frameLink) + fc->frameLength
				- MAC_CRC32_SIZE - DEF_SIG_SIZE;
*/
	if (!good_signature(pdu, sig_start, index, sdu_dss_public_num))
	{
		Error_proc(0,
				   0,
				   CS_ERR_SU_SIG,
				   &cs_err_su_sig,
				   &(pconn_list[index].pe),
				   &(pconn_list[index].upe));
		return(AUTH_SDU_SIG_FAILED);
	}
	else
	{
		return 0;
	}
}

/*******************************************************************************
* NAME:             get_nnc
*
* DESCRIPTION:      This function returns a pointer to the NNC whose open-
*					close flag is used to create a CIB to either block or
*					pass traffic due to an error condition. It is also used
*					by "send_nnc_req()" to choose an old or new NNC to be
*					embedded in frame.
*
* INPUTS:           frame control (to get index to the NNC table)
*
* RETURN:           pointer to the NNC. always non-null.
*
*******************************************************************************/
uchar * get_nnc(Pa2EpFrameControl *fc, uchar *auth, int *use_new_old)
{
	ulong   		current_time;
	ulong			base_time;
	AuthNetControl	*p_tm;
	ushort 			nnc_index;
	uchar			*pNnc;
#ifdef CISCO
	uchar			*cisco_nnc;
	uchar			*methodbuf;
	int				nnc_size;
#endif

	pNnc = NULL;	/*intialization */
	/*config tables have been downloaded, and there is NC in system. */
	if ((csAvail.bitMap == CS_CONFIG_TB_AVAIL) &&
		((p_tm = getAuthTimers()) != NULL) &&
		((base_time = getNCtimeStamp()) != 0))
	{
		nnc_index = fc->epEntry.info.cs.nnc;
		GetRTClock(&current_time);
		if (current_time < ((p_tm->aTime) + base_time))	/*now < A_time */
		{
			*use_new_old = SM_DSS_PUBLIC_OLD;
			pNnc = nnc_old_tb + nnc_index;
		}
		else if (current_time < ((p_tm->eTime) + base_time)) /* A_time <= now < E_time */
		{
			*use_new_old = SM_DSS_PUBLIC_NEW;
			pNnc = nnc_new_tb + nnc_index;
		}
		else                             		/*now >= E_time */
		{
			Error_proc(	0,               		/*log error */
						0,
						CS_ERR_NNC_EXP,
						&cs_err_nnc_exp,
						&(fc->epEntry.pe),
						&(fc->epEntry.upe));
		}
	}

	if (pNnc && ((*(ushort *)(pNnc + NNC_REV_OFFSET)) != 0))
	{
		return pNnc;
	}
	else
	{
		/*There is no NC, NNC in system. Or the NNC has expired. */
		/*Or the NNC revision is zero (when epPEdef has been updated but not NNC table). */
		/*Or config tables (protcol table, ...) have not been downloaded yet. */
#ifndef CISCO
		(*auth) |= STAT_SYS_OC_FLAG;	/**use_new_old is undefined */
		return nnc_sys_oc;              /*Use the dummy system NNC. */
#else /* CISCO */
		/* 
		 * Use a copy of the dummy system NNC which we have overloaded to
		 * be the Cisco NNC. The copy must be returned because we have to
		 * dynamically put the network address, protocol vector, etc. in it.
		 *
		 * In the event that malloc can't return memory, we just use the
		 * dummy system NNC directly. Although it affects the nnc_sys_oc
		 * template, this is more reliable than trying to change all of the
		 * SDU code to check for a NULL nnc pointer ....
		 */
		nnc_size = get_nnc_size(nnc_sys_oc);
		cisco_nnc = malloc(nnc_size);
		if (cisco_nnc) {
			memcpy(cisco_nnc, nnc_sys_oc, nnc_size);
		} else {
			cisco_nnc = nnc_sys_oc;
		}
		/*
		 * Fill in the method vector for this connection.
		 */
		methodbuf = cisco_nnc + NNC_GRP_VEC_OFFSET*4; /* 4 bytes/word */
		SET_PDU_ULONG(fc->epEntry.info.cs.cib->crypto.cisco.methods_allowed, 
					  methodbuf);
		return cisco_nnc;
#endif /* CISCO */
	}
}

/*******************************************************************************
* NAME:             gen_DH_pub_secret
*
* DESCRIPTION:      this function generates the DH public number and secret
*                   number.
*
* INPUTS:           dh_secret - hidden memory index for DH secret number.
*                   buf - pointer to the DH public number field in NNC frame
*                   p_status - pointer to the status field in NNC frame
*
* OUTPUT:           public number is generated. secret number index is stored
*                   in the pending connection entry.
*
*******************************************************************************/
void gen_DH_pub_secret(uchar *buf, short dh_secret, uchar *p_status,
					   Pa2EpFrameControl *fc)
{
        
#ifdef CISCO
	if (fc->epEntry.info.cs.cib) {
		if (crypto_glue_generate_DH_public_number(buf, 
				fc->epEntry.info.cs.cib->crypto.cisco.new_connection_id,
				fc->epEntry.info.cs.cib->crypto.cisco.slot)) {
			return;
		}
	} else {
		DEBUG("gen_DH_pub_secret() found NULL cib");
	}
#else /* !CISCO */
	if(!hGenDHpair(	dhPublicLen,
					dh_secret, 	/*secret number generated */
					buf,        /*public number generated */
					dhBase,
					dhModulus))
	{
		return;
	}
#endif /* !CISCO */

	/*crypto failure. if everything works fine, this function has exit already. */
	/*generate Trap here. */
	(*p_status) |= STAT_GEN_DH_FAILED;
	memset(buf, 0, dhPublicLen);
	Error_proc(0,
			   0,
			   CS_ERR_GEN_DH,
			   &cs_err_gen_dh,
			   &(fc->epEntry.pe),
			   &(fc->epEntry.upe));
	return;
}

/*******************************************************************************
* NAME:             gen_challenge_num
*
* DESCRIPTION:      this function generates the challenge number.
*
* INPUTS:           buf - destination of the generated challenge number in
*                         NNC frame
*                   chal_recv - pointer to the received challenge number in
*                               NNC frame
*                   p_status - pointer to the status field in NNC frame
*                   fc - frame control assiciated with current frame.
*
* OUTPUT:           challenge number is generated.
*                   If the crypto library failed to generate the challenge
*                   number, function "Error_proc" will be called and the
*                   challenge number will set to zeros. Also the status field
*                   in NNC frame will have a bit set to indicate error.
*
*******************************************************************************/
void gen_challenge_num(uchar *buf, uchar *chal_recv, uchar *p_status,
					   Pa2EpFrameControl *fc)
{
	ushort  i;

	/*"hi_challenge_num" has been pre-allocated at initialization */
	/*and it never gets released */
#ifndef CISCO
/*    if (!hGenRandom(hi_challenge_num, CHALLENGE_SIZE)) */
/*    { */
/*		if (!hMemRead(hi_challenge_num, CHALLENGE_SIZE, buf)) */
		if (!hReadRandom((char *)buf, CHALLENGE_SIZE))
		{
#else /* CISCO */
			random_fill(buf, CHALLENGE_SIZE, TRUE);
#endif /* CISCO */
			if (chal_recv == NULL)
			{
				/*building NNC Echo Request message; generate ODD bits */
				for (i = 0; i < CHALLENGE_SIZE; i++)
				{
					buf[i] &= CHAL_ODD_MASK;
				}
			}
			else
			{
				/*building NNC Echo Reply message; generate EVEN bits and */
				/*combine received ODD bits */
				for (i = 0; i < CHALLENGE_SIZE; i++)
				{
					buf[i] = (buf[i] & CHAL_EVEN_MASK) | chal_recv[i];
				}
			}

			return;
#ifndef CISCO
		}
/*	} */

	/*crypto failure. if everything works fine, this function has exit already. */
	/*generate Trap here. */
	memset(buf, 0, CHALLENGE_SIZE);
	(*p_status) |= STAT_GEN_CHL_FAILED;
	Error_proc(0,
			   0,
			   CS_ERR_GEN_CHL,
			   &cs_err_gen_chl,
			   &(fc->epEntry.pe),	/*OK, since this function is called */
               &(fc->epEntry.upe));	/*at sending and receiving the first msg */
	return;
#endif
}
/*******************************************************************************
* NAME:             gen_signature
*
* DESCRIPTION:      This function generates the DSS signature for each
*                   connection setup message.
*
* INPUTS:           p_status: pointer to the status field in the frame
*                   buf:      pointer to where the signature starts
*                   fc:       frame control assiciated with current frame.
*                   l5:       pointer to the beginning of the CS message
*
* OUTPUT:           signature is attached to the message
*
*******************************************************************************/
void gen_signature(uchar *buf,
					uchar *p_status,
					EntityID *pe_id,		/*OK, defined */
					EntityID *upe_id,	/*OK, defined */
					uchar *l5,
					uchar auth,
					uchar *sdu_dss_public_num,
					ushort slot)
{
#ifndef CISCO
	uchar   hash_result[DSS_FIX_SIZE];
	short	hi_dss_x;
#endif

	/*if a real NNC exists (therefore SDU DSS public number exists) */
	if (!(auth & STAT_SYS_OC_FLAG))
	{
#ifndef CISCO
		/*compute hDssKSeedHolder random num */
/*		if (!hGenDSSnumber(dssQ, hi_kkey)) */
/*		{ */
			/*compute hash result */
			if ((hi_dss_x = getSDUdssSecretKeyIndex(sdu_dss_public_num))
					!= 0)
			{
				if (!SHA(l5, (ushort)(buf - l5), hash_result))
				{
					if (!hGenDSSsignature(dssPkeyLen,
										  dssP,
										  dssQ,
										  dssG,
										  hi_dss_x,	/*DSS secret number */
										  buf,
										  buf + DSS_FIX_SIZE,
										  hash_result))
					{
					  return;
					}
				}
#else /* !CISCO */
		if (crypto_glue_crypto_sign(slot, l5, (ushort)(buf - l5), buf)) {
			return;
		}	
#endif /* !CISCO */
				Error_proc(	0,
							0,
							CS_ERR_GEN_SIG,
							&cs_err_gen_sig,
							pe_id,
							upe_id );
#ifndef CISCO
			}
/*		} */
#endif /* !CISCO */
	}

	/*a dummy NNC is used so there is no SDU DSS public number to generate */
	/*signature. OR it is a crypto failure. */

	/*if everything works fine, this function has exit already. */
	/*generate Trap here. */
	memset(buf, 0, DEF_SIG_SIZE);
	(*p_status) |= STAT_GEN_SIG_FAILED;
	return;
}


/*******************************************************************************
* NAME:             good_signature
*
* DESCRIPTION:      This function generates the DSS signature for each
*                   connection setup message.
*
* INPUTS:           start: pointer to the start of the signed message
*                   sig: poointer ot the signature
*                   fc: frame control assiciated with current frame.
*                   dss_pub_num: either SU's or SM's DSS public number.
*                                the former is used to verify CS messages while
*                                the latter is used to veriy NNCs.
*
* OUTPUT:           1: if signature is good.     0: otherwise
*
*******************************************************************************/
static uchar good_signature(uchar *start, uchar *sig, short index, uchar *dss_pub_num)
{
#ifndef CISCO
	uchar   hash_result[DSS_FIX_SIZE];
#endif
	short   ok;

#ifndef CISCO
	if (!SHA(start, (ushort)(sig - start), hash_result)) /*compute hash result */
	{
		if (!VerDSSSignature(dssPkeyLen,
							 dssP,
							 dssQ,
							 dssG,
							 dss_pub_num,
							 sig,
							 sig + DSS_FIX_SIZE,
							 hash_result))
		{
			ok = 1;
		}
		else
		{
			ok = 0;	/*the calling functin generates a error flag of invalid signature */
		}
	}
	else
#else /* CISCO */
	if ((ok = crypto_glue_good_signature(start, sig, index, dss_pub_num)) == 0) 
#endif /* CISCO */
	{
		/*crypto failure. */
		/*generate Trap here and treat it as bad signature. */
		ok = 0;
		Error_proc(0,
				   0,
				   CS_ERR_VERIFY_SIG,
				   &cs_err_verify_sig,
				   &(pconn_list[index].pe),
				   &(pconn_list[index].upe));
	}

	return(ok);
}


/*******************************************************************************
* NAME:             pconn_exist
*
* DESCRIPTION:      This function checks if the pending connection entry exists
*                   for a NNC frame. It is a safe guard due to the fact that
*                   retransmitted frame could occur after the connection is
*                   established and entry is released.
*
* INPUTS:           index - the index of the pending entry to verify
*                   recv_conn_id - unique connection ID received in frame
*					conn_id_ofs - the CS LAYER offset to the connection ID
*
* OUTPUT:           1 if the entry is verified to exist; 0 otherwise.
*
*******************************************************************************/
int pconn_exist(short index, ulong recv_conn_id, ushort conn_id_ofs)
{
	pconn_entry     *pconn;
	ulong           saved_conn_id;

	pconn = pconn_list + index;
    saved_conn_id = *((ulong *)(pconn->retx_buf + pconn->l5_offset + conn_id_ofs));

	if ((pconn->timestamp != 0L) && (saved_conn_id == recv_conn_id))
	{
		return 1;
	}
	else
	{
#ifdef INTGR_DEBUG	/*TEST ONLY */
		cs_debug |= CS_TEST_PCONN_NOT_EXIST;
#endif
		return 0;
	}

/*	return (((pconn->timestamp != 0L) && 	   	//this entry is in use */
/*			 (saved_conn_id == recv_conn_id)) ?	//this entry has the same conn ID */
/*			1 : 0 ); */
}

/***************************************************************************
* NAME:         pconn_mgmt
*
* DESCRIPTION:  This function goes through the pending connection list
*               to check any timed out entries. If it times out for the
*               first time, a retransmission starts. If it is the second
*               time, it is processed as failed connection. See next function.
*
* INPUTS:       pending connection list
*
* OUTPUTS:      a retransmission could occur, or a CIB is created for the
*               failed connections.
*
****************************************************************************/
void pconn_mgmt(void)
{
	short               i;
	short               nxt;
	ulong              	current_time;
	ulong              	base_time;
	pconn_entry         *pconn;

#ifndef CISCO
	GetRTClock(&current_time);
#endif
	i = pconn_used;
	while (i != NULL_INDEX)
    {
		pconn = pconn_list + i;
		nxt = pconn->next;
        base_time = pconn->timestamp;

#ifdef CISCO
        /*
         * Check to make sure that the process hasn't taken too much time.
         */
        if (process_time_exceeded()) {
            if (crypto_sm_debug) {
                buginf("\nCRYPTO: Process Time Exceeded - am suspending now\n");
            }
            process_safe_suspend();
        }
        /*
         * Read the clock here since we may have yielded the CPU
         */
        GetRTClock(&current_time);
		/*
		 * If base_time is 0, the process building the connection message 
		 * hasn't completed building the mesage yet. This can happen
		 * in the Cisco implementation due to the process creating the entry
		 * early on, but then blocking several times waiting for the EPA to
		 * service a request.
		 */
        if ((base_time != 0) && (current_time >= base_time))
#else
        if (current_time >= base_time)
#endif /* !CISCO */
		{
			if ((current_time - base_time) > max_pconn_period)
			{
				if (pconn->tx_count >= cs_max_retransmissions)
				{
					/**********************************************
					* Process timed out pending connectin entry
					**********************************************/
					proc_failed_pconn(i,
									  CS_ERR_PCONN_TIMEOUT,
									  &cs_err_pconn_timeout);
				}
				else
				{
					/********************************************
					* Retransmission
					********************************************/
					pconn_retransmit(i);
				}
			}
			/*else: this entry does not time out, examine next one */
		}
		else /*system time counter wraps - 68.1 years after system boots up */
		{
			pconn->timestamp = current_time;     /*reset base time */
		}

		i = nxt;
	}
}


/***************************************************************************
* NAME:         proc_failed_pconn
*
* DESCRIPTION:  This function processes those pending connection entries
*               that get timed out twice.
*
* INPUTS:       i -          the index of the entry
*               err_code -   the error code to be recorded
*               p_entities - pointer ot the PE/UPE pair involved in this conn
*
* OUTPUTS:      a CIB is created for this entry. This entry is released.
*
****************************************************************************/
static void proc_failed_pconn(short         i,
							  long          err_code,
							  entity_pair   *p_entities)
{
	CIBptr          cib;
	bits16          oc_flag;
	uchar			post_err = 1;
	uchar           *oc_ptr;
	pconn_entry     *pconn;
#ifdef CISCO
	boolean			already_freed_pconn = FALSE;
#endif

	pconn = pconn_list + i;
	cib = pconn->cib;

	if (cib)
	{
		if (!memcmp(&(pconn->pe), &(pconn->upe), 6))
		{
			/*If PE and UPE has the same Network Addr, this is a "bridged" case */
			post_err = 0;
			if (ep_ctrl & PASS_INTRANET)
			{
				(cib->status) |= OPEN_NODE;		/*bypass */
			}
			else
			{
				(cib->status) &= (~OPEN_NODE);   /*block */
			}
		}
		else if ((*(pconn->retx_buf + pconn->l5_offset + PDU_OP_OFFSET)) ==
				  ECHO_REQUEST)
		{
			/***********************************************************
			* Create CIB according to PE's openness/closeness.
			* Far end could be no SDU, or no valid NNC to send back.
			************************************************************/
			oc_ptr = pconn->nnc + NNC_FIX_GRP_OFFSET +
						dssPkeyLen + NNC_GRP_OC_OFFSET;
			GET_PDU_USHORT(oc_flag, oc_ptr);

			/*clear the open-close bit in the status flag of CIB */
			(cib->status) &= (~OPEN_NODE);
			(cib->status) |= oc_flag;
		}
		else    /*frame sent is ECHO_REPLY */
		{
			/**********************************************************
			* Always create a blocking CIB in case that far end SDU
			* sends back a confirm message and it gets lost. Far end
			* SDU already creates CIB to encrypt this connection, but
			* this SDU could have use PE's open flag to bypass traffic.
			***********************************************************/
			(cib->status) &= (~OPEN_NODE);      /*block traffic */
		}

		cib->pvector = 0;
		time_conn_fast(cib);
		release_pend_cib(cib);
#ifdef CISCO
		/* 
		 * crypto_glue_timeout_keys() does a free_pconn(i), so mark pconn
		 * as alrady freed below.
		 */
		crypto_glue_timeout_keys(cib); 
		already_freed_pconn = TRUE;
#endif

	}

	if (post_err)
	{
		Error_proc(0, 0, err_code, p_entities, &(pconn->pe), &(pconn->upe));
	}

#ifdef CISCO
	if (!already_freed_pconn) {
	    free_pconn(i);  /*return the entry to the free pool */
    }
#else /* !CISCO */
	free_pconn(i);  /*return the entry to the free pool */
#endif /* !CISCO */
	DEBUG(cs100);
}

/*****************************************************************************
*
*
* INPUTS:		i - pending connection list index
*
*****************************************************************************/
void pconn_retransmit(short i)
{
	Cy2TxFrameControl   *fch;       /*frame control header */
	ushort              nnc_retx_offset;
	uchar				*pdu;		/*CS message PDU */
	pconn_entry         *pconn;
#ifdef CISCO
	cisco_FrameControlHeader *cisco;
#endif

	fch = taskAcquireBuffer();
	if (fch)
	{
		/*copy over entire frame except NNC and signature */
		pconn = pconn_list + i;
		nnc_retx_offset = pconn->l5_offset +
						  PDU_FIX_NNC_OFFSET + dhPublicLen;
		memcpy(fch->frameP, pconn->retx_buf, nnc_retx_offset);

		/*re-build NNC */
		memcpy(((uchar *)fch->frameP) + nnc_retx_offset,
			   pconn->nnc,
			   get_nnc_size(pconn->nnc));

		/*re-generate signature */
		pdu = ((uchar *)fch->frameP) + pconn->l5_offset;
#ifdef CISCO
		/*
		 * Indicate that this is a retransmitted frame and stuff away 
		 * the network-address. Also load up the epEntry structure.
		 */
		 cisco = (cisco_FrameControlHeader *)fch->otherP;
		 cisco->retransmit = TRUE;
		 cisco->epEntry.pe = pconn->pe;
		 cisco->epEntry.upe = pconn->upe;
		 cisco->epEntry.info.cs.cib = pconn->cib;
		 cisco->epEntry.cisco.src = pconn->cisco.src;
		 cisco->epEntry.cisco.dst = pconn->cisco.dst;
		gen_signature(((uchar *)fch->frameP) + pconn->retx_fsize - DEF_SIG_SIZE,
					  pdu + PDU_STAT_OFFSET,
					  &(pconn->pe),
					  &(pconn->upe),
					  pdu,
					  pdu[PDU_STAT_OFFSET],
					  /*SDU DSS public number */
					  pdu + PDU_FIX_NNC_OFFSET + dhPublicLen + 
							NNC_FIX_GRP_OFFSET,
					  pconn->cib->crypto.cisco.slot);
#else /* !CISCO */
		gen_signature(((uchar *)fch->frameP) + pconn->retx_fsize - DEF_SIG_SIZE,
					  pdu + PDU_STAT_OFFSET,
					  &(pconn->pe),
					  &(pconn->upe),
					  pdu,
					  pdu[PDU_STAT_OFFSET],
					  /*SDU DSS public number */
					  pdu + PDU_FIX_NNC_OFFSET + dhPublicLen + NNC_FIX_GRP_OFFSET);
#endif /* CISCO */
		/*Alter the challenge number if retransmits Reply. The Confirm msg */
		/*won't be retransmitted since once the frame is sent, pending */
		/*connection entry is released. */
		if ((pdu[PDU_OP_OFFSET] == ECHO_REPLY) &&
			!(pdu[PDU_STAT_OFFSET] & STAT_GEN_DH_FAILED))
		{
#ifndef CISCO
			if (!hMemSyndrome(pconn->hi_dh_shared,
							  SYNDROME_HMA_OFFSET,
							  dhPublicLen,
							  CHALLENGE_SIZE,
							  syndrome))
#else /* CISCO */
			if (crypto_glue_generate_syndrome(
				pconn->cib->crypto.cisco.new_connection_id,
				pconn->cib->crypto.cisco.slot,
				SYNDROME_HMA_OFFSET,
				CHALLENGE_SIZE,
				syndrome))
#endif /* CISCO */
			{
				xor_challenge(pdu + PDU_CHAL_OFFSET);
			}
		}

		/*retransmit the frame. no error checking necessary. */
		cs_transmit_frm(fch,
						pconn->retx_fsize,
						&(pconn->pe),
						&(pconn->upe),
						pconn->retx_port,
						pconn->retx_action);

		/*reset the timer, increment the transmission counter */
		GetRTClock((ulong *)(&pconn->timestamp));
		(pconn->tx_count)++;
	}
	else
	{
		proc_failed_pconn(i,
						  CS_ERR_NO_TX_BUF,
						  &cs_err_no_tx_buf);
	}
}

/****************************************************************************
*
*
*
*
****************************************************************************/
void time_conn_fast(CIBptr cib)
{
	ulong			tm = 0;

	GetRTClock(&tm);
	cib->crypto.time = ((ushort) (tm / 60)) +  	/*minutes */
						sdu_failed_conn_retry -
						epMaxKeyAge;
	cib->status |= TIME_KEYS;
}

/**********************************************************************
*
*
*
*
***********************************************************************/
void release_pend_cib(CIBptr cib_ptr)
{
	/*do EP a favor. always clear this bit. */
	(cib_ptr->status) &= (~XCHG_KEYS);

	/*adj_active_conn_count(cib_ptr) is the same as follows: */
	/*update statistics. */
	if ((cib_ptr->status) & BAD_CONN) 	 	/* build new connection */
	{
		sdu_conn_built++;
		(cib_ptr->status) &= (~BAD_CONN);	/* clear the bit */
	}

	/*release CIB to EP */
	(cib_ptr->status) &= (~PEND_CONN);

#ifdef INTGR_DEBUG
	display_cib(cib_ptr);						/* debugging purpose */
#endif
}

/**********************************************************************
* NAME:			getInterEnterpriseSMdssPublic
*
* DESCRYPTION:	This function searches a SNMP managable table for the
*				Inter-Enterprise SM DSS Public number by using the
*				"customer ID" and "NNC revision" as the search key.
*
* INPUTS:		customerID - pointer to the customer ID in NNC
*				nncRev - pointer to the revision in NNC
*
* RETURN:       A pointer to the Inter-Enterprise SM DSS Public number.
*				NULL if not found.
*
***********************************************************************/
#pragma argsused
uchar *getInterEnterpriseSMdssPublic(uchar *customerID, uchar *nncRev)
{
	return NULL;
}

/*---------------------------- END OF FILE -------------------------------- */
