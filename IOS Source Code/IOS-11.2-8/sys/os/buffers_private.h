/* $Id: buffers_private.h,v 3.4.28.3 1996/08/28 13:02:57 thille Exp $
 * $Source: /release/112/cvs/Xsys/os/buffers_private.h,v $
 *------------------------------------------------------------------
 * buffers_private.h - Internal defines and prototypes for buffer code
 *
 * May 1995, Scott Mackie
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: buffers_private.h,v $
 * Revision 3.4.28.3  1996/08/28  13:02:57  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.4.28.2  1996/07/23  13:27:02  thille
 * CSCdi63329: Centralize logging for getbuffer() and pak_duplicate()
 * failures
 * Branch: California_branch
 * Shamelessly rip off Mr. Katz' code from CSCdi36037.  Provide
 * centralized logging for getbuffer() and pak_duplicate() failures and
 * then rip out the 9 bazillion different messages that where in the
 * various callers to report the failure.
 *
 * Revision 3.4.28.1  1996/04/19  15:24:27  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commit
 *
 * Revision 2.1.6.4  1996/04/02  10:31:46  carrel
 * cvs fixes
 *
 * Revision 2.1.6.3  1996/02/21  03:48:12  che
 * Branch: IosSec_branch
 * Sync to 11.1 Mainline (02/20/96)
 *
 * Revision 2.1.6.2  1995/12/19  04:09:03  che
 * Branch: IosSec_branch
 * Re-sync to Arkansas to fix things that broke.
 *
 * Revision 2.1.6.1  1995/11/17  17:21:35  carrel
 * Branch: IosSec_branch
 * Sync to Arkansas
 * Revision 3.4  1996/01/22  06:56:04  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.3  1995/12/16  22:50:46  sberl
 * CSCdi45227:  FRAS/BAN and BNN dont work together
 * Initialize pak->llc2_enctype here to keep recycled packets from using
 * an old value. I hope to take this out later after I figure out the
 * correct places to make sure it is always set for all llc2 packets.
 *
 * Revision 3.2  1995/11/17  18:44:59  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:44:03  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/11/08  21:18:26  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.2  1995/09/18  07:27:28  smackie
 * Prune buffer code into discrete and manageable sections to restore some
 * sanity and focus to buffers.c (CSCdi40529).
 *
 *   o buffers.c - buffer management code
 *   o element.c - queue element code
 *   o queue.c   - singly-linked queue support
 *
 * Revision 2.1  1995/06/07  21:53:50  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/*
 * pak_reset
 *
 * Initialize paktype fields to a known state
 */

static inline void pak_reset (paktype *pak)
{
    pak->flags = 0;		/* clear temporary flags */
    pak->desthost = 0;		/* no dest host */ 
    pak->if_input = NULL;	/* no input interface yet */ 
    pak->if_output = NULL;      /* no output interface yet */ 
    pak->enctype = 0;		/* no encapsulation type yet */ 
    pak->enc_flags = 0;		/* no encapsulation failures yet */ 
    pak->llc2_cb = NULL;	/* clear pointer to llctype */
    pak->tcpdriver_flags = 0;	/* no TCP Driver flags */ 
    pak->tcp_flags = 0;		/* no TCP flags */ 
    pak->linktype = LINK_ILLEGAL;	/* no real link type */ 
    pak->acct_proto = ACCT_PROTO_OTHER; /* no protocol accounting */ 
    pak->cacheptr = NULL;		/* no routing cache pointer */ 
    pak->classification = 0;	/* no security classification */ 
    pak->authority = 0;		/* no security authorities */ 
    pak->lat_groupmask = NULL;	/* No LAT group code mask */ 
    GET_TIMESTAMP(pak->gp_timer);	/* time stamp the buffer */ 
    pak->nif_input = NULL;	/* no IPX netw desc */ 
    pak->nif_output = NULL;	/* no IPX netw desc */ 
    pak->snapshot_flags = 0; 	/* no Snapshot flags */ 
    pak->dialer_input = NULL; 	/* no dialer input IDB */ 
    pak->ip_flags = 0;		/* no IP flags */ 
    pak->x25_flags = 0;		/* no X.25 information flags */ 
    pak->dialer_flags = 0;      /* no DDR information flags */ 
    pak->cryptoflags = 0;	/* no crypto flags */
    pak->llc2_enctype = ET_NULL;/* no llc2 encaps yet either */
    pak->mlp_next_frag = NULL;  /* no multilink fragments */
    pak->mlp_frag_size = 0;     /* no multilink encapsulation */
}


/*
 * Externs
 */

extern list_header publicpoolQ;
extern list_header privatepoolQ;
extern list_header headerpoolQ;
extern char *packet_header;

/*
 * Structure for recording getbuffer() failures
 */
typedef struct getbuffer_failures_ {
    sys_timestamp failtime;		/* Milliseconds since boot */
    pooltype *pool;			/* Pool we tried to get from */
    ulong size;				/* Requested size in bytes */
    ulong calling_pc;			/* Caller's address */
} getbuffer_failures;

#define GETBUFFER_FAILURE_RECORD_COUNT 10 /* Record the last 10 failures */
extern getbuffer_failures getbuffer_failure_trace[GETBUFFER_FAILURE_RECORD_COUNT];
extern int getbuffer_failure_trace_number;	/* Next element to fill in */

/*
 * Private routines
 */

extern void report_getbuffer_failure(pooltype *pool, ulong size, 
				     ulong caller_pc); 
