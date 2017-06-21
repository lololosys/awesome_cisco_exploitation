/* $Id: arap_v42bis.c,v 3.2.60.1 1996/06/28 23:04:41 hampton Exp $
 * $Source: /release/112/cvs/Xsys/arap/arap_v42bis.c,v $
 *------------------------------------------------------------------
 * V.42bis routines
 *
 * October 1993, Tim Kolar
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: arap_v42bis.c,v $
 * Revision 3.2.60.1  1996/06/28  23:04:41  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.2  1995/11/17  08:38:59  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  10:53:54  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/06/18  19:54:36  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.2  1995/06/09  12:58:20  hampton
 * Rename old scheduler calls to fit the new naming convention.
 * [CSCdi35514]
 *
 * Revision 2.1  1995/06/07 20:05:10  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "interface_private.h"
#include "packet.h"
#include "../h/config.h"
#include "parser.h"
#include "../atalk/macip_util.h"
#include "arap_v42bis.h"
#include "../os/async.h"
#include "../h/ttysrv.h"

extern paktype *arap_getbuffer(int, int);
extern void arap_datagram_done(paktype *, int);
extern paktype *arap_input_getbuffer(int, hwidbtype *, int);
extern paktype *arap_pak_duplicate(paktype *, int);


/*
 * initialize v42bis
 */
v42type *init_v42(void)
{
    v42type *ptr;

    ptr = malloc(sizeof(v42type));
    if (ptr == NULL) 
	return(NULL);

    ptr->comp = fcr_init_v42bis();
    if (ptr->comp == NULL)  {
	free(ptr);
	return(NULL);
    }
    ptr->bufpak = NULL;

    ptr->input_inchar = 1;	/* set these to 1 instead of zero, to avoid
				 * ptr->input_outchar = 1;
				 *  by zero errors */
    ptr->output_inchar = 1;
    ptr->output_outchar = 1;

    return (ptr);
}

void destroy_v42(tt_soc * tty)
{
    v42type *ptr = get_tty_v42ptr(tty);
    int ttynum = tty->ttynum;

    if (ptr) {
        fcr_destroy_v42bis(ptr->comp, ttynum);
        free(ptr);
    }
}

/*
 * v42_input -- undo compression and strip modem tool headers, then enqueue
 */
int v42_input(tt_soc *tty, queuetype *v42InQptr, queuetype *ARAPInQptr)
{
    paktype *pak;
    int len, count=0;
    int ret = 0;
    v42type *ptr = get_tty_v42ptr(tty);
    int ttynum = tty->ttynum;
 
    while ((pak = dequeue(v42InQptr))) {
	count++;
	ptr->input_inchar += pak->datagramsize;

	/*  Under no conditions should v.42bis get data in chunks larger
		than 128 bytes.  Since the maximum sized MNP4 packet is
		256 bytes, the following code is safe. */

	if (pak->datagramsize <= 128) {
	    len = fcr_undo_v42(ptr->comp, pak->datagramstart,
			       pak->datagramsize, ARAPInQptr, ttynum);
	    if (len < 0) {
		ret = -1;
		break;
	    }
	    ptr->input_outchar += len;
	} else {
	    len = fcr_undo_v42(ptr->comp, pak->datagramstart,
			       128, ARAPInQptr, ttynum);
	    if (len < 0) {
		ret = -1;
		break;
	    }
	    ptr->input_outchar += len;
	    len = fcr_undo_v42(ptr->comp, pak->datagramstart +128,
			       pak->datagramsize-128, ARAPInQptr, ttynum);
	    if (len < 0) {
		ret = -1;
		break;
	    }
	    ptr->input_outchar += len;

	}
	/*s_suspend() may never return eg, death()*/ 
	arap_datagram_done(pak, ttynum);
	pak = NULL;
	if (!(count%5)) {
		process_suspend();  /* be a nice citizen */
	}
    }
    if (pak)
	arap_datagram_done(pak, ttynum);
    return(ret);
}

/*
 * v42_output -- compress packets and send them to MNP
 */
int v42_output(tt_soc *tty, queuetype *v42OutQptr, queuetype *MNP4OutQptr)
{
    paktype *pak;
    int len, count = 0;
    int ret = 0;
    v42type *ptr = get_tty_v42ptr(tty);
    int ttynum = tty->ttynum;

    while ((pak = dequeue(v42OutQptr))) {
	ptr->output_inchar += pak->datagramsize;
	len = fcr_do_v42(ptr->comp, pak->datagramstart,
			 pak->datagramsize, MNP4OutQptr, ttynum);
	if (len < 0) {
	    ret = -1;
	    break;
	}
	ptr->output_outchar += len;
	/*s_suspend() may never return eg, death()*/
	arap_datagram_done(pak, ttynum);
	pak = NULL;
	count++;
	if (!(count%5)) 
	{
		process_suspend(); /* be a nice citizen */
	}
    }
    if (pak)
	arap_datagram_done(pak, ttynum);
    return(ret);
}
