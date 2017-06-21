/* $Id: les_driver_inline.h,v 3.3.62.1 1996/03/18 20:42:59 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/les/les_driver_inline.h,v $
 *------------------------------------------------------------------
 * les_driver_inline.h -- Common inline driver stuff for les drivers
 * 
 * November 1994, Ashwin Baindur
 *
 * Copyright (c) 1988-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: les_driver_inline.h,v $
 * Revision 3.3.62.1  1996/03/18  20:42:59  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.26.2  1996/03/13  01:50:14  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.3.26.1  1996/02/20  14:42:15  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.3  1995/11/17  17:41:46  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:00:24  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:33:41  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:15:08  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/06/07  21:38:13  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __LES_DRIVER_INLINE_H__
#define __LES_DRIVER_INLINE_H__


/*
 * Return a packet buffer initialized for use as a receive buffer.
 * This routine is in the interrupt path and is coded for speed.
 * The calling routine must use pak->datagramstart to set the
 * buffer descriptor.
 */
inline static paktype *les_driver_getbuf_inline (hwidbtype  *idb,
				                 boolean    cache_ok)
{
    paktype *pak = NULL;

    /*
     * Check for input congestion:
     */
    if (idb->input_qcount > idb->input_defqcount)
	return(NULL);

    /*
     * If cache_ok is set to be TRUE when the inline is used, this
     * code will be compiled in. *NO* conditional will be generated.
     */
    if (cache_ok) {
	/*
	 * Try to get a cached buffer. Else get a new buffer.
	 */
	pak = pool_dequeue_cache(idb->pool);
    }
    if (!pak)
	pak = pool_getbuffer(idb->pool);
    if (pak) {

#ifdef DEBUG
	if (!validmem(pak)) {
	    buginf("\n%s(%d) getbuf: Invalid p=%x\n", 
                   idb->hw_namestring, idb->unit, pak);
	    return(NULL);
	}
	if (pak->next != NULL) {
	    buginf("\n%s(%d) getbuf: buffer in list, p=%x\n",
                   idb->hw_namestring, idb->unit, pak);
	    return(NULL);
	}
	if (pak->refcount != 1) {
	    buginf("\n%s(%d) getbuf: refcount not one, p=%x\n",
                   idb->hw_namestring, idb->unit, pak);
	    return(NULL);
	}
#endif

	/*
	 * Initialize datagramstart:
	 */
	pak->datagramstart = pak->data_area + idb->start_offset;
    	return(pak);
    } else {
	idb->counters.output_nobuffers++;
	return(NULL);
    }
}



#endif  /* __LES_DRIVER_INLINE_H__ */
