/* $Id: alignment.h,v 3.2.62.1 1996/03/18 21:57:47 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/src-4k/alignment.h,v $
 *------------------------------------------------------------------
 * alignment.h - include file for the misalignment handling subsystem
 *
 * April 1994, Jim Hayes
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: alignment.h,v $
 * Revision 3.2.62.1  1996/03/18  21:57:47  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.2  1996/03/09  05:11:38  hampton
 * Slim down the kernel of the router.
 * Branch: DeadKingOnAThrone_branch
 *
 * Revision 3.2.26.1  1996/02/20  17:49:39  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.2  1995/11/17  18:40:27  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:20:09  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/07/14  09:36:25  schaefer
 * CSCdi36176: misalign/spurious access traceback broken for leaf functions
 * also, remove unused code
 *
 * Revision 2.1  1995/06/07  22:52:49  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __ALIGNMENT_H__
#define __ALIGNMENT_H__

/*
 * Some defines for the alignment subsystem
 */
#define NULL_TABLE_MAX		50	/* entries in nullade table */
#define ALIGN_TABLE_MAX       	100	/* entries in alignment table */

#define ALIGN_MESSAGE_BACKOFF_LIMIT 6	/* Max backoff is 2^6 minutes; */

#define ALIGN_ADDR_LIMIT   	0x4000	/* flag the first 16k of memory map
					   as special illegal addresses
					   worthy of a special message */

#define ALIGN_LOG_DEFAULT  	TRUE	/* TRUE = Log messages by default
					   FALSE = Don't log messages */

/* What types of misaligned accesses we can read or write */

#define RTYPE_HW_BYTE       0
#define RTYPE_LW_BYTE       1
#define RTYPE_LW_HALF       2
#define RTYPE_DW_BYTE       3
#define RTYPE_DW_HALF       4
#define RTYPE_DW_WORD       5
#define RTYPE_MAX	    RTYPE_DW_WORD

#define WTYPE_HW_BYTE       6
#define WTYPE_LW_BYTE       7
#define WTYPE_LW_HALF       8
#define WTYPE_DW_BYTE       9
#define WTYPE_DW_HALF       10
#define WTYPE_DW_WORD       11

#define ALIGN_FRAMES	4
typedef struct {
    ulong pc[ALIGN_FRAMES];
} align_traceback_t;

typedef struct alignment_access_t_ {
    uint align:5;		/* address alignment */
    uint read:1;		/* boolean - is it a read? */
    uint shift16:2;		/* (log16 - 1) of access size in bits */
} align_access_t;

typedef struct alignment_entry_t_ {
    align_traceback_t trace;		/* access traceback */
    ulong addr;				/* address accessed */
    ushort count;			/* How many times it happened */
    uchar access_type;			/* What kind of error it was */
    tinybool message_printed;		/* TRUE if this has been logged */
} alignment_entry_t;

typedef struct alignment_t_ {
    SIG_PF sigbus_save;			/* Saved signal handlers */
    SIG_PF sigsegv_save;
    int tail;				/* Next avail. entry in table below */
    int reads, writes;
    int total, recorded;
    boolean database_full;		/* True if we printed "full" msg. */
    boolean activated;			/* TRUE if we're correcting errors */
    boolean logging;			/* TRUE if we're logging errors */
    boolean align_msg_needed;		/* TRUE if misalignment occurred and
					   need to have a peek during the 
					   once-a-minute logging task */
    boolean null_msg_needed;		/* TRUE if a Null pointer occurred
					   and we need to peek during the
					   once-a-minute logging task */

    int nulltail;			/* Next avail. entry in nulltable */
    int nulltotal, nullrecorded;

    /* And now, ladies and gentlemen, the tables... */

    alignment_entry_t table[ALIGN_TABLE_MAX];
    alignment_entry_t nulltable[NULL_TABLE_MAX];

} alignment_t;

/*
 * Prototypes
 */

extern int unaligned_handler(int, int, r4k_context_t *);
extern boolean is_valid_pc(void *);
extern void show_alignment(parseinfo *);
extern void alignment_parser_init(void);
extern void alignment_handler_disable(void);
extern void alignment_handler_enable(void);
extern void r4k_pc_trace(ulong* trace, uint nframes, r4k_context_t* sc);

#endif /* __ALIGNMENT_H__ */
