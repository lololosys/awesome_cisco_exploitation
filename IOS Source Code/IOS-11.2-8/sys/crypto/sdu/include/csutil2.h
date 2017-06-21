/* $Id: csutil2.h,v 1.1.68.1 1996/04/19 14:57:33 che Exp $
 * $Source: /release/112/cvs/Xsys/crypto/sdu/include/csutil2.h,v $
 *------------------------------------------------------------------
 * Cylink Source code: csutil2.h
 *
 * August 1995, Brian Weis
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: csutil2.h,v $
 * Revision 1.1.68.1  1996/04/19  14:57:33  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commits
 *
 * Revision 1.1.2.1  1995/11/28  01:49:33  bew
 * Branch: IosSec_branch
 * Port of newest Cylink SDU code, plus CS cleanup. The SDU code
 * is moved to its original Cylink tree structure.
 *
 * Revision 1.1  1995/11/22  23:22:07  bew
 * Placeholders for new SDU file locations.
 *
 * Revision 1.7  1995/06/07 12:03:05  smackie
 * Fold back Arizona_branch into mainline.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************************
* $Header: /SecureLAN/SDU/Include/CSUTIL2.H 13    7/19/95 5:47p Tchi 
*
* DESCRIPTION:
*    This is the header file for csutil2.c where all utilities are used
*    by the Connection Setup task.
*
* Copyright(c)  Cylink Corporation 1994.  All rights reserved.
*******************************************************************************
* $History: CSUTIL2.H 
* 
* *****************  Version 13  *****************
* User: Tchi         Date: 7/19/95    Time: 5:47p
* Updated in $/SecureLAN/SDU/Include
* alter the challenge number after signing the message
* 
* *****************  Version 12  *****************
* User: Tchi         Date: 6/30/95    Time: 5:02p
* Updated in $/SecureLAN/SDU/Include
* removed ERR_REL_TWO_KEYS
* 
* *****************  Version 11  *****************
* User: Tchi         Date: 6/14/95    Time: 11:47a
* Updated in $/SecureLAN/SDU/Include
* one extra parameter in create_cib
* 
* *****************  Version 10  *****************
* User: Sduhost      Date: 5/14/95    Time: 3:00p
* Updated in $/SecureLAN/SDU/Include
* fix so get_nnc_size is called only once on the received NNC.
* 
* *****************  Version 9  *****************
* User: Sduhost      Date: 5/09/95    Time: 4:31p
* Updated in $/SecureLAN/SDU/Include
* added pconn_retransmit
* 
* *****************  Version 8  *****************
* User: Tchi         Date: 4/28/95    Time: 10:10a
* Updated in $/SecureLAN/SDU/Include
* added constants
* 
* *****************  Version 7  *****************
* User: Tchi         Date: 2/16/95    Time: 6:56p
* Updated in $/SecureLAN/SDU/Include
* change the addresses of the last two NNC msg
* 
* *****************  Version 6  *****************
* User: Tchi         Date: 2/04/95    Time: 9:00a
* Updated in $/SecureLAN/SDU/Include
* CS and Authentication interface
* 
* *****************  Version 5  *****************
* User: Tchi         Date: 1/20/95    Time: 5:39p
* Updated in $/SecureLAN/SDU/Include
* intgra 2a
* 
* *****************  Version 4  *****************
* User: Tchi         Date: 12/12/94   Time: 4:57p
* Updated in $/SecureLAN/SDU/Include
* added GET_PDU_ULONG
* 
* *****************  Version 3  *****************
* User: Tchi         Date: 11/11/94   Time: 4:23p
* Updated in $/SecureLAN/SDU/Include
* miscellaneous
* 
* *****************  Version 2  *****************
* User: Tchi         Date: 11/08/94   Time: 4:08p
* Updated in $/SecureLAN/SDU/Include
* added DSS
* 
* *****************  Version 1  *****************
* User: Tchi         Date: 10/25/94   Time: 8:47p
* Created in $/SecureLAN/SDU/Include
* created
*******************************************************************************/

#ifndef CSUTIL2_H
#define CSUTIL2_H

/*--------------------------------- INCLUDES ---------------------------------- */
#include "../crypto/sdu/include/cylink.h"
#include "../crypto/sdu/include/pa/pactl.h"

/*------------------------------- CONSTANTS ---------------------------------- */
#define CY_NOT_COMPUTED		0
#define CY_SUCCESS			1
#define CY_FAILED			-1

#define	ANALYZE_PV			0
#define ANALYZE_SV			1

/*------------------------------- MACROS ------------------------------------- */
#ifdef CISCO
#include "target_cisco.h"
#define GET_PDU_SHORT(val, ptr)     \
{                                   \
    val = (short) GETSHORT(ptr); \
}

#define GET_PDU_USHORT(val, ptr)    \
{                                   \
    val = GETSHORT(ptr); \
}

#define GET_PDU_LONG(val, ptr)      \
{                                   \
    val = (long) GETLONG(ptr); 	\
}

#define GET_PDU_ULONG(val, ptr)     \
{                                   \
    val = GETLONG(ptr);	\
}

#else /* !CISCO */
#define GET_PDU_SHORT(val, ptr)     \
{                                   \
    val = (((short)*(ptr)) << 8) | ((short)*(ptr + 1));     \
}


#define GET_PDU_USHORT(val, ptr)    \
{                                   \
    val = (((ushort)*(ptr)) << 8) | ((ushort)*(ptr + 1));   \
}


#define GET_PDU_LONG(val, ptr)      \
{                                   \
    val = (((long)*(ptr)) << 24) | (((long)*(ptr + 1)) << 16) | (((long)*(ptr + 2)) << 8) | ((long)*(ptr + 3)); \
}


#define GET_PDU_ULONG(val, ptr)     \
{                                   \
	val = (((ulong)*(ptr)) << 24) | (((ulong)*(ptr + 1)) << 16) | (((ulong)*(ptr + 2)) << 8) | ((ulong)*(ptr + 3)); \
}
#endif /* !CISCO */

#define GET_GV_BLK(blk_ptr, grp_member, root_leaf, meshiness)   \
{                                                               \
    GET_PDU_LONG(grp_member, (blk_ptr));                        \
    GET_PDU_LONG(root_leaf, (blk_ptr + 4));                     \
    GET_PDU_LONG(meshiness, (blk_ptr + 8));                     \
}

/*The error flag is set at the end of the function */
/*
#define ERR_REL_TWO_KEYS(hi_key1, hi_key2)   			\
{                                                       \
	hMemFree(hi_key1);                       			\
	hMemFree(hi_key2);                       			\
	hi_key1 = 0;										\
	hi_key2 = 0;										\
}
*/

/*******************************************************************************
* EXTERNAL FUNCTION DECLARATIONS
*******************************************************************************/

#ifndef CSUTIL2_C

extern void    create_cib(short, uchar *, uchar, uchar, uchar);
extern uchar   check_nnc_sig(uchar *, short, int, ushort, ulong);
extern uchar   check_frame_sig(short, uchar *, uchar *, ushort);
extern int     good_challenge_odd(uchar *, short);
extern int     good_challenge_cmp(uchar *, short);
extern uchar  *get_nnc(Pa2EpFrameControl *, uchar *, int *);
extern void    gen_DH_pub_secret(uchar *, short, uchar *, Pa2EpFrameControl *);
extern void    gen_challenge_num(uchar *, uchar *, uchar *, Pa2EpFrameControl *);
extern void    gen_signature(uchar *, uchar *, EntityID *, EntityID *,
			     uchar *, uchar, uchar *, ushort);
extern int     pconn_exist(short, ulong, ushort);
extern void    pconn_mgmt(void);
extern void    pconn_retransmit(short);
extern void    time_conn_fast(CIBptr);
extern void    release_pend_cib(CIBptr);
extern uchar  *getInterEnterpriseSMdssPublic(uchar *, uchar *);

#endif

#endif
/*-------------------------------- END OF FILE ------------------------------- */
