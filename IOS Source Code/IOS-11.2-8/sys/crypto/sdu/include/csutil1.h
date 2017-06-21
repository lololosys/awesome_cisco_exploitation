/* $Id: csutil1.h,v 1.1.68.2 1996/06/17 22:24:45 bew Exp $
 * $Source: /release/112/cvs/Xsys/crypto/sdu/include/csutil1.h,v $
 *------------------------------------------------------------------
 * Cylink Source code: csutil1.h
 *
 * August 1995, Brian Weis
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: csutil1.h,v $
 * Revision 1.1.68.2  1996/06/17  22:24:45  bew
 * CSCdi60484:  error_proc multiply defined in APPN and Cylink crypto code
 * Branch: California_branch
 * Change error_proc() to Error_proc() in Cylink code.
 *
 * Revision 1.1.68.1  1996/04/19  14:57:32  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commits
 *
 * Revision 1.1.2.3  1996/04/16  21:53:54  bew
 * Branch: IosSec_branch
 * General code cleanup. Implemented checking of crypto map peers. CIM now
 * addressed to the src/dst rather than pe/upe.
 *
 * Revision 1.1.2.2  1996/03/13  22:45:29  bew
 * Branch: IosSec_branch
 * The RSP/VIP environment (multiple crypto engines) are now supported
 * in crypto parser routines and crypto session mgmt code.
 *
 * Revision 1.1.2.1  1995/11/28  01:49:32  bew
 * Branch: IosSec_branch
 * Port of newest Cylink SDU code, plus CS cleanup. The SDU code
 * is moved to its original Cylink tree structure.
 *
 * Revision 1.1  1995/11/22  23:22:06  bew
 * Placeholders for new SDU file locations.
 *
 * Revision 1.7  1995/06/07 12:03:05  smackie
 * Fold back Arizona_branch into mainline.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************************
* $Header: /SecureLAN/SDU/Include/CSUTIL1.H 20    7/19/95 5:47p Tchi 
*
* DESCRIPTION:
*    This is the header file for csuti1l.c where all utilities are used
*    by the Connection Setup task.
*
* Copyright(c)  Cylink Corporation 1994.  All rights reserved.
*******************************************************************************
* $History: CSUTIL1.H 
* 
* *****************  Version 20  *****************
* User: Tchi         Date: 7/19/95    Time: 5:47p
* Updated in $/SecureLAN/SDU/Include
* alter the challenge number after signing the message
* 
* *****************  Version 19  *****************
* User: Tchi         Date: 6/12/95    Time: 12:08p
* Updated in $/SecureLAN/SDU/Include
* added umSduGetDssPublic
* 
* *****************  Version 18  *****************
* User: Sduhost      Date: 5/14/95    Time: 3:00p
* Updated in $/SecureLAN/SDU/Include
* fix so get_nnc_size is called only once on the received NNC.
* 
* *****************  Version 17  *****************
* User: Tchi         Date: 5/04/95    Time: 4:12p
* Updated in $/SecureLAN/SDU/Include
* type cast pointer arithmatics
* 
* *****************  Version 16  *****************
* User: Tchi         Date: 5/04/95    Time: 2:39p
* Updated in $/SecureLAN/SDU/Include
* added ADJ_LLC_LENGTH
* 
* *****************  Version 15  *****************
* User: Tchi         Date: 4/25/95    Time: 7:26p
* Updated in $/SecureLAN/SDU/Include
* add one extra parameter to function "handle_nnc_req_revisions"
* 
* *****************  Version 14  *****************
* User: Sduhost      Date: 4/17/95    Time: 6:23p
* Updated in $/SecureLAN/SDU/Include
* added "adj_active_conn_count"
* 
* *****************  Version 13  *****************
* User: Tchi         Date: 2/16/95    Time: 6:56p
* Updated in $/SecureLAN/SDU/Include
* change the addresses of the last two NNC msg
* 
* *****************  Version 12  *****************
* User: Tchi         Date: 2/13/95    Time: 6:57p
* Updated in $/SecureLAN/SDU/Include
* error handling for IO MGR
* 
* *****************  Version 11  *****************
* User: Bald_eagle   Date: 2/09/95    Time: 5:05p
* Updated in $/SecureLAN/SDU/Include
* intgr2a
* 
* *****************  Version 10  *****************
* User: Tchi         Date: 2/07/95    Time: 6:55p
* Updated in $/SecureLAN/SDU/Include
* added build_ip_header
* 
* *****************  Version 9  *****************
* User: Tchi         Date: 2/04/95    Time: 9:00a
* Updated in $/SecureLAN/SDU/Include
* CS and Authentication interface
* 
* *****************  Version 8  *****************
* User: Tchi         Date: 1/19/95    Time: 6:37p
* Updated in $/SecureLAN/SDU/Include
* fixed csTransmit
* 
* *****************  Version 7  *****************
* User: Tchi         Date: 1/12/95    Time: 6:01p
* Updated in $/SecureLAN/SDU/Include
* changed to ep_frm.h
* 
* *****************  Version 6  *****************
* User: Tchi         Date: 1/07/95    Time: 12:05p
* Updated in $/SecureLAN/SDU/Include
* can't recall
* 
* *****************  Version 5  *****************
* User: Tchi         Date: 12/12/94   Time: 6:56p
* Updated in $/SecureLAN/SDU/Include
* added SET_PDU_ULONG
* 
* *****************  Version 4  *****************
* User: Tchi         Date: 11/11/94   Time: 4:23p
* Updated in $/SecureLAN/SDU/Include
* miscellaneous
* 
* *****************  Version 3  *****************
* User: Tchi         Date: 11/08/94   Time: 4:08p
* Updated in $/SecureLAN/SDU/Include
* added DSS
* 
* *****************  Version 2  *****************
* User: Tchi         Date: 10/31/94   Time: 9:34a
* Updated in $/SecureLAN/SDU/Include
* use macro SET_PDU_USHORT
* 
* *****************  Version 1  *****************
* User: Tchi         Date: 10/25/94   Time: 8:47p
* Created in $/SecureLAN/SDU/Include
* created
*******************************************************************************/

#ifndef CSUTIL1_H
#define CSUTIL1_H

/*--------------------------------- INCLUDES ---------------------------------- */
#include "../crypto/sdu/include/cylink.h"
#include "../crypto/sdu/include/connstup.h"
#include "../crypto/sdu/include/pa/pactl.h"
#include "../crypto/sdu/include/ep_frm.h"

/*------------------------------ TYPE DEFINITIONS ----------------------------- */
typedef   void (* functype2)  (uchar *, uchar *, uchar *, uchar);


/*------------------------------ MACROS --------------------------------------- */
#ifdef CISCO
#include "target_cisco.h"
#define SET_PDU_USHORT(val, ptr)    \
{									\
	PUTSHORT(ptr, val);				\
}

#define SET_PDU_ULONG(val, ptr)		\
{                                   \
	PUTLONG(ptr, val);				\
}
#else /* !CISCO */
#define SET_PDU_USHORT(val, ptr)    \
{                                   \
    uchar *pval;                    \
									\
    pval = (uchar *) (&val);        \
    *(ptr) = *(pval + 1);           \
    *(ptr + 1) = *pval;             \
}

#define SET_PDU_ULONG(val, ptr)     \
{                                   \
    uchar *pval;                    \
                                    \
	pval = (uchar *) (&val);        \
    *(ptr) = *(pval + 3);           \
    *(ptr + 1) = *(pval + 2);       \
    *(ptr + 2) = *(pval + 1);       \
    *(ptr + 3) = *pval;             \
}
#endif /* !CISCO */

#define ADJ_LLC_LENGTH(L2, endL5)			\
{											\
	ushort	size;							\
											\
	size = (ushort)((endL5) - (L2 + 14));	\
	SET_PDU_USHORT(size, L2 + 12);			\
}

/*******************************************************************************
* EXTERNAL FUNCTION DECLARATIONS
*******************************************************************************/

#ifndef CSUTIL1_C

extern void    build_ip_header(uchar *, uchar *, ushort, uchar);
extern void    swap_l2_da_sa(uchar *, ulong);
extern void    swap_l3_da_sa(uchar *, ushort);
extern short   alloc_pconn(Pa2EpFrameControl *);
extern void    free_pconn(short);
extern void    Error_proc(CIBptr, uchar *, ulong, entity_pair *, EntityID *,
						  EntityID *);
extern short   srch_pconn_index(ulong, ushort);
extern ushort  get_ip_checksum(ushort *, int);
#ifdef CISCO
extern void    build_cs_msg_req(Pa2EpFrameControl *, uchar *, short, uchar **,
								uchar *, uchar, ulong);
extern void    build_cs_msg_rpy(Pa2EpFrameControl *, uchar *, uchar *,
						 uchar *, uchar *, uchar **, short, ushort, ulong);
extern void    build_cs_msg_cfm(uchar *, uchar *, uchar *, uchar **,
				short, uchar *, Pa2EpFrameControl *);
#else
extern void    build_cs_msg_req(Pa2EpFrameControl *, uchar *, short, uchar **,
								uchar *, uchar);
extern void    build_cs_msg_rpy(Pa2EpFrameControl *, uchar *, uchar *,
						 uchar *, uchar *, uchar **, short, ushort);
extern void    build_cs_msg_cfm(uchar *, uchar *, uchar *, uchar **,
								short, uchar *);
#endif
extern uchar   handle_nnc_req_revisions(uchar *, Pa2EpFrameControl *,
										int *, uchar **, uchar);
extern void    init_l3_l4_ip(uchar *, uchar *, uchar *, uchar);
extern void    init_l3_l4_idp_ipx(uchar *, uchar *, uchar *, uchar);
extern void    init_l3_l4_ddp(uchar *, uchar *, uchar *, uchar);
extern int     bad_nnc_frm_size(Pa2EpFrameControl *, uchar *, int, ushort *);
extern void    cs_transmit_frm(Cy2TxFrameControl *, ushort, EntityID *, EntityID *, void *, int);
extern ushort  get_nnc_size(uchar *);
extern void    post_err_iomgr(ulong,ulong *,ushort);
extern void    build_cs_frm_l2_addr(uchar *, ulong);
extern void    build_cs_frm_l3_addr(uchar *, ushort, uchar *);
extern void    genDHshareXorChallenge(Pa2EpFrameControl *,	uchar *, uchar *,
									  short, uchar, uchar);
extern void    xor_challenge(uchar *);
#ifdef CISCO
extern uchar   *umSduGetDssPublic(uchar *, uchar *, CIBptr);
#else
extern uchar   *umSduGetDssPublic(uchar *, uchar *);
#endif

/*******************************************************************************
* EXTERNAL VARIABLE DECLARATIONS
*******************************************************************************/
extern ushort      l4_header_size[];
extern functype2   init_l3_l4_header[];

#endif

#endif
/*-------------------------------- END OF FILE ------------------------------- */
