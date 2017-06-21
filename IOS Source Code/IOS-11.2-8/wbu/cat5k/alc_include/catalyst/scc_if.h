/* $Id: scc_if.h,v 1.1.4.1 1996/05/09 14:55:46 rbadri Exp $
 * $Source: /release/112/cvs/Xwbu/cat5k/alc_include/catalyst/scc_if.h,v $
 *------------------------------------------------------------------
 * scc_if.h
 *
 * ?
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * Copyright (c) 1993 Xlnt Designs Inc
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: scc_if.h,v $
 * Revision 1.1.4.1  1996/05/09  14:55:46  rbadri
 * Branch: California_branch
 * Synergy features to the mainline
 *
 * Revision 1.1.40.1  1996/04/27  06:15:16  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.28.1  1996/04/08  01:40:15  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.16.1  1996/03/22  09:11:25  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.1  1996/03/13  07:44:06  rlowe
 * Add file content from Synergy repository.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1  1996/03/11  07:40:45  rlowe
 * Placeholders for California.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
@(#)scc_if.h	1.1 10/18/93 10/22/93 Copyright (c) 1993 Xlnt Designs Inc 
*/
#ifndef _SCC_H_
#define _SCC_H_

#define SCC_MAXLEN 4095
#define BUFSIZE 4096

struct buf {
	char free;
	uInt16 b_len;
	char data;
	struct buf *b_act;
};

struct  sccqueue {
	struct  buf *ifq_head;
	struct  buf *ifq_tail;
	int ifq_len;
	int ifq_maxlen;
	int ifq_drops;
} ;

struct  sccinqueue {
	Int16 head;
	Int16 tail;
	Int16 max;
	Int16 mask;
	uInt32 drops;
	unsigned char *buf;
} ;

#define SCC_QFULL(ifq)       ((ifq)->ifq_len >= (ifq)->ifq_maxlen)
#define SCC_DROP(ifq)        ((ifq)->ifq_drops++)
#define SCC_ENQUEUE(ifq, m) { \
	(m)->b_act = 0; \
	if ((ifq)->ifq_tail == 0) \
		(ifq)->ifq_head = m; \
	else \
		(ifq)->ifq_tail->b_act = m; \
	(ifq)->ifq_tail = m; \
	(ifq)->ifq_len++; \
}
#define SCC_PREPEND(ifq, m) { \
	(m)->b_act = (ifq)->ifq_head; \
	if ((ifq)->ifq_tail == 0) \
		(ifq)->ifq_tail = (m); \
	(ifq)->ifq_head = (m); \
	(ifq)->ifq_len++; \
}
#define SCC_DEQUEUE(ifq, m) { \
	(m) = (ifq)->ifq_head; \
	if (m) { \
		if (((ifq)->ifq_head = (m)->b_act) == 0) \
			(ifq)->ifq_tail = 0; \
		(m)->b_act = 0; \
		(ifq)->ifq_len--; \
	} \
}

#define MIN(a,b)	(((a)<(b))?(a):(b))
#define MAX(a,b)	(((a)>(b))?(a):(b))

#endif
