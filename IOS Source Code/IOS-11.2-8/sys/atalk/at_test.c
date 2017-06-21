/* $Id: at_test.c,v 3.7.4.6 1996/07/10 22:19:03 dstine Exp $
 * $Source: /release/112/cvs/Xsys/atalk/at_test.c,v $
 *------------------------------------------------------------------
 * at_test.c -- AppleTalk test exec command entry points
 *
 * June, 1994. David S.A. Stine
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: at_test.c,v $
 * Revision 3.7.4.6  1996/07/10  22:19:03  dstine
 * CSCdi62471:  Superfluous #include files in atalk, atm, os files
 * Branch: California_branch
 *
 * Revision 3.7.4.5  1996/06/29  00:07:07  thille
 * CSCdi61558:  Trim a little fat in the appletalk text strings
 * Clean up some literal strings and get back 580 bytes.
 *
 * Revision 3.7.4.4  1996/05/21  09:42:30  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.7.4.3  1996/03/24  03:07:22  ppearce
 * atalk build failure
 * Branch: California_branch
 *   ../atalk/at_test.c needs to include "interface_private.h" again
 *     to pick up definition for "encapstype"
 *
 * Revision 3.7.4.2  1996/03/23  01:27:44  slin
 * Branch: California_branch
 *
 * AppleTalk makes its exodus from the idb.
 *
 * Revision 3.7.4.1  1996/03/18  18:53:19  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.5.12.3  1996/03/16  06:27:05  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.5.12.2  1996/03/07  08:29:36  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.5.12.1  1996/02/20  00:17:51  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.7  1996/03/09  22:54:24  widmer
 * CSCdi47890:  HTTP server doesnt work
 * Add HTTP server
 *
 * Revision 3.6  1996/02/01  05:59:17  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.5  1995/12/17  18:24:21  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.4  1995/12/01  15:56:01  widmer
 * CSCdi44724:  Remove nv_top argument to parser_add_mode()
 * Clean up "no" handling at beginning of modes
 * Fix appn modes
 * Removed unneded pdecl's
 *
 * Revision 3.3  1995/11/17  08:43:58  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:06:07  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  10:57:57  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/11/08  20:50:35  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.2  1995/06/28  09:21:42  smackie
 * Repair widespread subsystem header braindamage. (CSCdi23568)
 *
 *   o Fix subsystem entry points to be foo(subsystype *) not foo(void)
 *   o Repair nonsensical and redundant subsystem sequences
 *   o Use NULL where no property is required
 *
 * Revision 2.1  1995/06/07  20:09:34  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <ciscolib.h>
#include "registry.h"
#include "subsys.h"
#include "interface_private.h" /* added back in --ppearce*/
#include "packet.h"
#include "address.h"
#include "ttysrv.h"
#include "../atalk/atalk_private.h"
#include "../atalk/at_nbp.h"
#include "../atalk/at_arp.h"
#include "at_registry.h"
#include "config.h"
#include "parser.h"

#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "../parser/parser_defs_parser.h"
#include "../iprouting/parser_defs_igrp2.h"
/*
 * AppleTalk subsystem header
 */

#define ATALKTEST_MAJVERSION	1
#define ATALKTEST_MINVERSION	0
#define ATALKTEST_EDITVERSION	1

SUBSYS_HEADER(atalktest, ATALKTEST_MAJVERSION, ATALKTEST_MINVERSION,
	      ATALKTEST_EDITVERSION,
	      atalk_test_init, SUBSYS_CLASS_PROTOCOL,
	      "seq: atalk", "req: atalk");

/*
 * AppleTalk TEST commands
 */
#define ALTERNATE NONE
#include        "exec_test_apple.h"
LINK_POINT      (atalk_test_commands, ALTERNATE);
#undef  ALTERNATE

static nbptimer timer;
static nbpEntity regnve;
/*
 * Forward declarations.
 */
static void atalk_test_arp(parseinfo *);
static void atalk_test_nbp(parseinfo *);
static void atalk_test_nbp_confirm(parseinfo *);
static void atalk_test_nbp_lookup(parseinfo *);
static void atalk_test_nbp_param(parseinfo *, nbptimer *);
static void atalk_test_nbp_poll(parseinfo *);
static void atalk_test_nbp_register(parseinfo *);
static void atalk_test_nbp_statistics(parseinfo *);
static void atalk_test_nbp_unregister(parseinfo *);



/* * AppleTalk test command */ void atalk_test_command (parseinfo
*csb) {

    if (!atalk_running) {
	printf(atalk_not_running);
	return;
    }
    switch (csb->which) {
      case ATALK_TEST_ARP:
	atalk_test_arp(csb);
	break;

      case ATALK_TEST_NBP:
	atalk_test_nbp(csb);
	break;

      case ATALK_TEST_END:
	csb->mode = exec_mode;
	break;

      default:
	printf("\nat_test: Should be unreachable.  Test case #%d.\n",
	       csb->which);
	break;
    }
}

/*
 * atalk_test
 *
 * Sets the prompt and modifies the parse chain to let the AppleTalk test
 * chain be the top chain.
 *
 * Exit the chain with the keyword "end"
 */
void atalk_test (parseinfo *csb)
{
    if (!atalk_running) {
	printf(atalk_not_running);
	return;
    }
    set_mode_byname(&csb->mode, "atalk-test", MODE_VERBOSE);
}

/*
 * atalk_test_init
 *
 * Initialize the AppleTalk test subsystem.
 */
void atalk_test_init (subsystype *subsys)
{
    timer.maxrequests = DEF_MAXREQUESTS;
    timer.maxreplies = DEF_MAXREPLIES;
    timer.interval = DEF_INTERVAL;
    
    parser_add_commands(PARSE_ADD_TEST_CMD,
			&pname(atalk_test_commands),
			"atalk");
    parser_add_mode("atalk-test", "atalk test", "Appletalk test mode",
		    FALSE, FALSE, NULL, NULL, NULL,
		    &pname(top_test_atalk),
		    NULL);
}

void
atalk_test_arp (parseinfo *csb)
{
    atalkidbtype	*idb;			/* interface */
    ataddrtype	address;		/* DDP address */
    uchar	*mac_address;		/* MAC address */
    ushort	net;
    uchar	node;

    idb  = atalk_getidb(GETOBJ(idb,1));
    if (!atalkif_InterfaceUp(idb)) {
	printf("\n%%AppleTalk not up on interface %s",
	       idb_get_namestring(GETOBJ(idb, 1)));
	return;
    }
    net  = GETOBJ(paddr,1)->apple_addr.net;
    node = GETOBJ(paddr,1)->apple_addr.host;
    address = atalk_address(net,node);
    mac_address = (uchar *)(*GETOBJ(hwaddr,1)).addr;
    (void)aarp_insert(idb, address, mac_address, IEEEBYTES,
		      idb->aarp_enctype, AARP_TEST);
}


void
atalk_test_nbp (parseinfo *csb)
{
    switch (GETOBJ(int,1)) {
      case ATNBP_TEST_ADDCLIENT:
	reg_invoke_atalk_macip_add_client(csb);
	break;

      case ATNBP_TEST_DELCLIENT:
	reg_invoke_atalk_macip_del_client(csb);
	break;

      case ATNBP_TEST_CONFIRM:
	atalk_test_nbp_confirm(csb);
	break;

      case ATNBP_TEST_LOOKUP:
	atalk_test_nbp_lookup(csb);
	break;

      case ATNBP_TEST_PARAMETERS:
	atalk_test_nbp_param(csb, &timer);
	break;

      case ATNBP_TEST_POLL:
	atalk_test_nbp_poll(csb);
	break;

      case ATNBP_TEST_REGISTER:
	atalk_test_nbp_register(csb);
	break;

      case ATNBP_TEST_STATISTICS:
	atalk_test_nbp_statistics(csb);
	break;

      case ATNBP_TEST_UNREGISTER:
	atalk_test_nbp_unregister(csb);
	break;

      default:
	break;
    }

}
/*
 * unregister_cmd -
 *	
 *	unregister an NVE.
 *
 * Syntax is:
 *
 * test appletalk nbp unregister <nbp entity>
 *	
 */
static void
atalk_test_nbp_unregister (parseinfo *csb)
{
    nbp_result err;

    /* get name,type,zone from user: */
    if (!atnbp_parse_tuple(GETOBJ(string,1), &regnve)) {
	printf("\n%%Unable to unregister %s", GETOBJ(string,1));
        return;
    }

    err = atnbp_unregister(&regnve);

    if (err != NBP_OK) {
	printf("\n%%Error while attempting to unregister %s, %s",
	       GETOBJ(string,1), atnbp_resultStr(err));
    }
}

/*
 * atalk_test_nbp_confirm
 *	
 *	confirm a known nbp entity.
 *
 * Syntax is:
 *
 * test appletalk nbp confirm <fqa ddp address> <NBP nve string>
 *	
 */
static void
atalk_test_nbp_confirm (parseinfo *csb)
{
    static nbpEntity	nve;		/* reuse last entry */
    static ataddress	dst;
    ataddress		src;
    watched_queue       *qptr;
    nbp_result		err;
    nbpQelem		*ee;

    /* if first time... */
    if (EMPTYSTR32(&nve.srvr_name)) {
        /* ...set defaults */
        atnbp_mkname(&nve, "=", "AFPserver", "*");
        dst.net  = 0;
        dst.node = 0;
        dst.spec = ATALK_SOCK_NBP;
    }

    src.net  = 0;
    src.node = 0;
    src.spec = ATALK_SOCK_NBP;

    /* get ddp parms: */
    if (!atalk_parse_address(GETOBJ(string, 1), &dst.net, &dst.node, &dst.spec))
        return;

    /* parse name,type,zone from user: */
    if (!atnbp_parse_tuple(GETOBJ(string, 2), &nve))
        return;

    /* malloc a result queue: */
    qptr = create_watched_queue("AT NBP Test Queue", 0, 0);
    
    err = atnbp_confirm(&nve, &dst, &src, (nbptimer *) NULL, qptr);
    if (err != NBP_OK) {
        printf("nbp_confirm() failed: %s\n", atnbp_resultStr(err));
        delete_watched_queue(&qptr);
        return;
    }

    /* block until complete or timeout: */
    process_watch_queue(qptr, ENABLE, ONE_SHOT);
    process_wait_for_event();
    process_watch_queue(qptr, DISABLE, ONE_SHOT);

    /* something on queue: */
    ee = (nbpQelem *) process_dequeue(qptr);

    switch(ee->msg) {
      case NBP_REPLY_LAST:
        printf("confirmed %s at %s\n",
	       atnbp_entityStr(&ee->nve), ddpAddrStr(&ee->srvr_addr));
	
        atnbp_event_free(ee);
        delete_watched_queue(&qptr);
        return;				/* confirmed */
	
      case NBP_TIMEOUT:
        printf("NBP lookup request timed out\n");
        break;
	
      default:
        /* internal error */
        printf("confirm error: no such msg, %d\n",ee->msg);
        atnbp_cancel(qptr);		/* cancel if still pending */
        break;
    }
    
    atnbp_event_free(ee);
    delete_watched_queue(&qptr);
}

/*
 * atalk_test_nbp_lookup
 *	
 *	look for the NVE supplied by user.  use default timing parms.
 *
 * Syntax:
 *	test appletalk nbp <NBP nve string>
 *	
 */
void
atalk_test_nbp_lookup (parseinfo *csb)
{
    static nbpEntity	nve;		/* reuse last entry */
    ataddress 		src;
    watched_queue	*qptr;
    int			nevents;
    int			nreplies;
    nbp_result		err;
    nbpQelem		*ee;

    /* if first call, set defaults: */
    if (EMPTYSTR32(&nve.srvr_name))
        atnbp_mkname(&nve, "=", "=", "Twilight");

    /* get name,type,zone from user: */
    if (!atnbp_parse_tuple(GETOBJ(string,1), &nve))
        return;

    src.net  = 0;
    src.node = 0;
    src.spec = 2;			/* no filter */

    /*
     * malloc a result queue:
     * warning: can't allocate on stack, as address has
     * no meaning once task swapped out.  has to be global
     * or malloc'd.
     */
    qptr = create_watched_queue("AT Test NBP Lookup Queue", 0, 0);

    if ((err = atnbp_find(&nve, &src, &timer, qptr)) != NBP_OK) {
        printf("\n%%nbp_find() reject: %s", atnbp_resultStr(err));
        delete_watched_queue(&qptr);
        return;
    }

    nevents = nreplies = 0;
    psion(stdio);			/* allow user to interrupt us */

    /* block until complete, timeout, or user interrupt: */
    while (!psiset(stdio)) {
	process_watch_queue(qptr, ENABLE, ONE_SHOT);
	process_wait_for_event();
	process_watch_queue(qptr, DISABLE, ONE_SHOT);

        /* something on queue: */
        ee = (nbpQelem *) process_dequeue(qptr);
        ++nevents;
        switch(ee->msg) {
        case NBP_REPLY:
        case NBP_REPLY_LAST:
            ++nreplies;
            printf("(%s)[%d]<-(%d.%d.%d): '%s'\n",
		   ddpAddrStr(&ee->srvr_addr),
		   ee->enumerator,
		   ee->orig_addr.net, ee->orig_addr.node, ee->orig_addr.spec,
		   atnbp_entityStr(&ee->nve));
	    
            if (ee->msg == NBP_REPLY_LAST) {
                printf("NBP lookup request completed.\n");
                printf("Processed %d replies, %d events\n",
		       nreplies, nevents);
                atnbp_event_free(ee);
                delete_watched_queue(&qptr);
		return;
            }

            atnbp_event_free(ee);
            break;

        case NBP_TIMEOUT:
            printf("\n%%NBP lookup request timed out\n");
            printf("Processed %d replies, %d events\n", nreplies, nevents);
            atnbp_event_free(ee);
            delete_watched_queue(&qptr);
            return;

        default:
            /* programmer error */
            printf("nbpfind: no such msg, %d\n",ee->msg);
            atnbp_event_free(ee);
            atnbp_cancel(qptr);
            delete_watched_queue(&qptr);
            return;
        }
    }

    /* interrupt - delete pending request: */
    printf("Interrupt...\n");
    atnbp_cancel(qptr);
    delete_watched_queue(&qptr);
}

/*
 * atalk_test_nbp_param
 *
 * Syntax:
 *	test appletalk nbp [<maxrequests> [<maxreplies> [<interval>]]]
 */
static void
atalk_test_nbp_param (parseinfo *csb, nbptimer *tm)
{
    if (tm == NULL)
	return;
    tm->maxrequests = GETOBJ(int,2);
    tm->maxreplies = GETOBJ(int,3);
    tm->interval = GETOBJ(int,4);
}

/*
 * atalk_test_nbp_poll
 *
 * Poll for all NBP nve's in all zones.
 *
 * Syntax:
 *	test appletalk nbp poll
 */
static void
atalk_test_nbp_poll (parseinfo *csb)
{
    watched_queue       *qptr;
    ziptabletype	*zz;
    int			ii;
    nbpEntity		nve;
    ataddress		src;
    int			startevents = 0;
    int			endevents = 0;
    char 		czone[ZONELEN+1];
    nbpQelem		*ee;
    int			nevents;
    int			nreplies;
    nbp_result		err;
    boolean		interrupt;

    qptr = create_watched_queue("AT NBP Test Poll Queue", 0, 0);

    /*
     * send lookups to all zones, with all replies on same queue
     */

    /* foreach slot in zone hash table... */
    for (ii = 0; ii < ZIPHASHLEN; ii++) {
        zz = zip_table[ii];
        /* foreach entry in slot... */
        while (zz) {
            bcopy(zz->name, czone, zz->length);
            czone[zz->length] = '\0';

            src.net  = 0;
            src.node = 0;
            src.spec = 2;		/* no filter */

            /* do the lookup: */
            atnbp_mkname(&nve,"=","=",czone);
            if (at_debug(atalknbp_debug, ATALK_NULL_MASK))
                buginf("AT: nbp_poll, sending '=:=@%s'\n",czone);
            if ((err = atnbp_find(&nve, &src, &timer, qptr)) != NBP_OK) {
		if (at_debug(atalknbp_debug, ATALK_NULL_MASK))
		    buginf("AT: nbp_poll: nbp_find err=%s\n",
			   atnbp_resultStr(err));
            } else {
                ++startevents;
	    }
            zz = zz->next;
        }
    }

    if (startevents <= 0) {
        delete_watched_queue(&qptr);
        printf("poll:  no lookups pending\n");
        return;
    }

    printf("\npoll:  sent %d lookups",startevents);

    nevents = nreplies = 0;
    psion(stdio);			/* allow user to interrupt us */

    /* block until complete, timeout, or user interrupt: */
    while (!(interrupt = psiset(stdio))) {
        if (at_debug(atalknbp_debug, ATALK_NULL_MASK))
            buginf("\nAT: nbp_poll:  blocking, start=%d, end=%d\n",
		   startevents, endevents);
	process_watch_queue(qptr, ENABLE, ONE_SHOT);
	process_wait_for_event();
	process_watch_queue(qptr, DISABLE, ONE_SHOT);

        /* something on queue: */
        ee = (nbpQelem *) process_dequeue(qptr);
        ++nevents;
        switch(ee->msg) {
	  case NBP_REPLY:
	  case NBP_REPLY_LAST:
            printf("\n(%s)[%d]:   '%s'",
		   ddpAddrStr(&ee->srvr_addr),
		   ee->enumerator,
		   atnbp_entityStr(&ee->nve));
	    
            ++nreplies;
            if (ee->msg == NBP_REPLY_LAST)
                ++endevents;

            /* free the used queue element: */
            atnbp_event_free(ee);
            break;
	  case NBP_TIMEOUT:
            atnbp_event_free(ee);
            ++endevents;
            break;
	  default:
            /* programmer error */
            printf("poll: no such msg, %d\n",ee->msg);
            atnbp_event_free(ee);
            atnbp_cancel(qptr);
            delete_watched_queue(&qptr);
            return;
        }

        if (endevents >= startevents)
            break;
    }

    /* interrupt or all lookups finished: */
    if (interrupt)
        printf("\nInterrupted.");
    else
        printf("\nNBP polling completed.");

    printf("\nProcessed %d replies, %d events", nreplies, nevents);

    atnbp_cancel(qptr);
    delete_watched_queue(&qptr);
}


/*
 * atalk_test_nbp_register-
 *
 * Syntax:
 *
 *	test appletalk nbp register <fqa ddp address> <NBP nve string>
 *	
 */
static void
atalk_test_nbp_register (parseinfo *csb)
{
    static ataddress	src;
    nbp_result		err;

    if (csb == NULL)
	return;

    /* get ddp parms: */
    if (!atalk_parse_address(GETOBJ(string,1), &src.net, &src.node, &src.spec))
        return;

    if (!atnbp_parse_tuple(GETOBJ(string,2), &regnve))
        return;

    /* register: */
    err = atnbp_register(&regnve, &src, FALSE);

    if (err == NBP_OK)
        printf("\nRegistered '%s' @(%s)", atnbp_entityStr(&regnve),
	       ddpAddrStr(&src));
    else
        printf("\nCouldn't register '%s':  %s",
	       atnbp_entityStr(&regnve), atnbp_resultStr(err));
}

static void
atalk_test_nbp_statistics (parseinfo *csb)
{

}
