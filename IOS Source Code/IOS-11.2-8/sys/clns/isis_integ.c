/* $Id: isis_integ.c,v 3.2.60.1 1996/08/08 14:50:49 dkatz Exp $
 * $Source: /release/112/cvs/Xsys/clns/isis_integ.c,v $
 *------------------------------------------------------------------
 * isis_integ.c -- The Integrated IS-IS general support functions.
 *
 * Copyright (c) 1988-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: isis_integ.c,v $
 * Revision 3.2.60.1  1996/08/08  14:50:49  dkatz
 * CSCdi65076:  ISIS/NLSP uses old scheduler primitives
 * Branch: California_branch
 * Use new scheduler primitives.  Get rid of gratuitous type coercion.
 *
 * Revision 3.2  1995/11/17  08:53:46  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:09:25  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:18:42  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
 
#include "master.h"
#include "interface_private.h"
#include "packet.h"
#include "../clns/clns_externs.h"
#include "../clns/isis_externs.h"


/*
 * isis_insert_nlpid
 */
isis_info_type *isis_insert_nlpid (clns_pdbtype *pdb, isis_info_type *item,
				   int bytes_left)
{

    uchar *nlpid;

    item->length = 0;

    /*
     * Don't insert NLPID if OSI-only mode.
     */
    if (!pdb || pdb->isis_mode == ISIS_OSI_MODE) return(item);

    item->code = ISIS_NLPID;
    nlpid = item->value;

    if (pdb->isis_mode & ISIS_OSI_MODE) {
	if (bytes_left <= 0)
	    goto bail;
	*nlpid++ = NLP_ISO8473;
	item->length++;
	bytes_left--;
    }
    if (pdb->isis_mode & ISIS_IP_MODE) {
	if (bytes_left <= 0)
	    goto bail;
	*nlpid++ = NLP_IP;
	item->length++;
	bytes_left--;
    }
 bail:
    ADVANCE_ITEM(item, FALSE);
    return(item);
}

/*
 * isis_insert_lsp_nlpid
 * 
 * Insert the NLPID option into an LSP, if appropriate.  Assumes
 * there's enough room.
 */
void isis_insert_lsp_nlpid(isis_lspcontext *context)
{
    isis_info_type *item;
    int bytes_left;

    item = isis_new_item(context, ISIS_NLPID, 0, NULL, &bytes_left, FALSE);
    if (item == NULL)
	return;
    item = isis_insert_nlpid(context->pdb, item, bytes_left);
    isis_item_done(context, item);
}

/*
 * isis_print_idr_info
 *
 * Print the Inter-domain routing info included in an LSP. The information
 * will be printed in hexadecimal.
 */
void isis_print_idr_info (isis_info_type *item)
{

    ulong *lptr;
    uchar *cptr;
    int   i, len1, len2;

    if (!item->length) return;

    cptr = (uchar *) item->value;
    lptr = (ulong *) (cptr + 1);
    printf ("\n  IDR Info Type: 0x%02x", *cptr);

    len1 = item->length - 1;
    len2 = len1 >> 2;

    for (i = 0; i < len2; i += 4) {
	if (!(i & 0x03)) printf ("\n    IDR Info Data: ");
	printf ("0x%08x  ", *lptr++);
    }

    if (!(i & 0x03)) printf ("\n    IDR Info Data: ");
    printf ("0x");

    cptr = (uchar *) lptr;
    len2 = len1 & 0x03;

    for (i = 0; i < len2; i++) {
	printf ("%02x", *cptr++);
    }
}


/*
 * isis_checkqueue
 *
 * Look for specific idb in IS-IS link list queue.
 */
idbqueuetype *isis_checkqueue (queuetype *queue, idbtype *idb)
{

    idbqueuetype *idbq;

    for (idbq = queue->qhead; idbq; idbq = idbq->next) {
	if (idbq->idb == idb) break;
    }
    return (idbq);
}
