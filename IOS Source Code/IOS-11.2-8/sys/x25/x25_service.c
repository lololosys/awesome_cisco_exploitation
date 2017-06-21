/* $Id: x25_service.c,v 3.4.20.1 1996/06/28 23:32:56 hampton Exp $
 * $Source: /release/112/cvs/Xsys/x25/x25_service.c,v $
 *------------------------------------------------------------------
 * x25_service.c -- X25 services
 *
 * January 1991, Greg Satz
 *
 * Copyright (c) 1987-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: x25_service.c,v $
 * Revision 3.4.20.1  1996/06/28  23:32:56  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.4  1996/02/01  06:13:14  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.3  1996/01/22  07:46:42  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.2  1995/11/17  18:08:06  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:54:06  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/08/30  19:52:16  rbatz
 * CSCdi30085:  SDLLC over RSRB/FST over X.25 crashes router due to stack
 *              overflow. Made 'fakepak' static instead of automatic.
 *
 * Revision 2.1  1995/06/07  23:22:52  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "interface_private.h"
#include "packet.h"
#include "address.h"
#include "media_registry.h"
#include "config.h"
#include "../os/hostname.h"
#include "parser_defs_x25.h"
#include "x25.h"
#include "x25_address.h"
#include "x25_switch.h"

typedef struct _service {
    struct _service *next;
    lcitype *lci;
    queuetype inq;
    int len;			/* used for banner ptr */
} x25servicetype;

x25servicetype *servicelist;

int x25_sf;			/* service process number */


/*
 * x25_service_init
 * X.25 services initialization
 */

void x25_service_init (void)
{
    x25_sf = 0;
    servicelist = NULL;

    reg_add_media_x25_macaddr(LINK_X25SERVICE, x25_extract_lci,
			      "x25_extract_lci");

}

/*
 * x25serviceBLOCK
 * Scheduler test for X.25 services process
 */

boolean x25serviceBLOCK (void)
{
    x25servicetype *serv;
    lcitype *lci;
    x25routeentrytype *pt;

    for (serv = servicelist; serv; serv = serv->next) {
	if (!QUEUEEMPTY(&serv->inq)) /* something to input */
	    return(FALSE);
	lci = serv->lci;
	pt = lci->lci_rtp;
	if (pt &&
	    ((int)pt->data == X25_FUNC_CHARGEN ||
	     (int)pt->data == X25_FUNC_HELP)) {
	    if (QUEUEEMPTY(&lci->lci_holdQ)) /* something to output */
		return(FALSE);
	}
    }
    return(TRUE);
}

/*
 * x25_service
 * X.25 background services process
 */

forktype x25_service (void)
{
    x25servicetype *serv;
    lcitype *lci;
    x25routeentrytype *pt;
    paktype *pak;
    static paktype fakepak;
    int i;
    register leveltype level;

    for (;;) {
	edisms((blockproc *)x25serviceBLOCK, 0);
	for (serv = servicelist; serv; serv = serv->next) {
	    level = raise_interrupt_level(NETS_DISABLE);
	    lci = serv->lci;
	    pt = lci->lci_rtp;
	    if (pt && !QUEUEEMPTY(&serv->inq)) /* something to input */
		switch ((int)pt->data) {
		case X25_FUNC_CHARGEN:
		case X25_FUNC_DISCARD:
		case X25_FUNC_HELP:
		    while ((pak = p_dequeue(&serv->inq))) {
			datagram_done(pak);
		    }
		    break;
		case X25_FUNC_ECHO:
		    while ((pak = p_dequeue(&serv->inq))) {
			pak->linktype = LINK_X25SERVICE;
			pak->if_output = lci->lci_idb;
		        pak->datagramstart = pak->network_start;
			if (x25_encaps(pak, (long)lci, NULL))
			    pak_enqueue(&lci->lci_holdQ, pak);
			else
			    datagram_done(pak);
		    }
		    break;
		}
	    switch ((int)pt->data) {
	    case X25_FUNC_CHARGEN:
		if (QUEUEEMPTY(&lci->lci_holdQ)) {
		    pak = getbuffer(80);
		    if (pak == NULL)
			break;
		    pak->linktype = LINK_X25SERVICE;
		    pak->if_output = lci->lci_idb;
		    pak->datagramstart = pak->network_start;
		    pak->datagramsize = 80;
		    for (i = 0; i < pak->datagramsize; i++)
			pak->datagramstart[i] = 0x30 + i % 10;
		    if (x25_encaps(pak, (long)lci, NULL))
			pak_enqueue(&lci->lci_holdQ, pak);
		    else
			datagram_done(pak);
		}
		break;
	    case X25_FUNC_DISCARD:
		break;
	    case X25_FUNC_ECHO:
		break;
	    case X25_FUNC_HELP:
		if (serv->len == -1 && (lci->lci_state == X25_P4 ||
		    lci->lci_state == X25_D1))
		    x25_sendcause(lci->lci_idb->hwptr, lci, X25_CLEAR,
			  X25_CLEAR_DTE_ORIGINATED, X25_DIAG_NO_INFO, NULL);
		else if (QUEUEEMPTY(&lci->lci_holdQ)) {
		    pak = getbuffer(80);
		    if (pak == NULL)
			break;
		    pak->linktype = LINK_X25SERVICE;
		    pak->if_output = lci->lci_idb;
		    pak->datagramstart = pak->network_start;
		    if (banner_motd != NULL) {
			for (i = serv->len; i < serv->len + 80; i++) {
			    if (banner_motd[i] == '\0')
				break;
			    *pak->datagramstart++ = banner_motd[i];
			}
			pak->datagramsize = i - serv->len;
			if (x25_encaps(pak, (long)lci, NULL))
			    pak_enqueue(&lci->lci_holdQ, pak);
			else
			    datagram_done(pak);
		    } else
			i = 0;
		    if (i == strlen(banner_motd))
			serv->len = -1;
		    else
			serv->len = i;
		}
		break;
	    }
	    i = lci->lci_ps;	/* update window with data or RR */
	    lci->lci_ack = lci->lci_pr;
	    if (!QUEUEEMPTY(&lci->lci_holdQ)) {
		fakepak.datagramsize = 0;
		fakepak.x25_cb = lci;
		x25_output(lci->lci_idb->hwptr, &fakepak, TAIL);
	    }
	    if (i == lci->lci_ps)
		x25_sendflow(lci->lci_idb->hwptr, lci, X25_RR);

	    reset_interrupt_level(level);
	}
    }
}

/*
 * x25_service_start
 * Start up X.25 service fork
 */

void x25_service_start (void)
{
    if (x25_sf == 0)
	x25_sf = cfork(x25_service, 0L, 700, "X25 Services", 0);
}

/*
 * x25_service_accept
 * Accept a call to an X.25 service and return TRUE if okay.
 */

boolean x25_service_accept (lcitype *lci)
{
    x25servicetype *serv;

    serv = malloc(sizeof(x25servicetype));
    if (serv == NULL)
	return(FALSE);
    serv->lci = lci;
    queue_init(&serv->inq, 0);
    serv->len = 0;
    serv->next = servicelist;
    servicelist = serv;
    return(TRUE);
}

/*
 * x25_service_remove
 * Remove instance of X.25 service when connection closes
 */

void x25_service_remove (lcitype *lci)
{
    x25servicetype *serv, *pserv;
    paktype *pak;

    for (pserv = serv = servicelist; serv; pserv = serv, serv = serv->next)
	if (serv->lci == lci)
	    break;
    if (serv == NULL)
	return;
    if (serv == pserv)
	servicelist = serv->next;
    else
	pserv->next = serv->next;
    while ((pak = p_dequeue(&serv->inq)))
	datagram_done(pak);
    free(serv);
}

/*
 * x25_service_input
 * Perform input to X.25 services
 */

void x25_service_input (lcitype *lci, paktype *pak)
{
    x25servicetype *serv;

    for (serv = servicelist; serv; serv = serv->next)
	if (serv->lci == lci)
	    break;
    if (serv == NULL) {
	datagram_done(pak);
	return;
    }
    p_enqueue(&serv->inq, pak);
}
