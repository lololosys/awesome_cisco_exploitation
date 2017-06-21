/* $Id: nbfcp.c,v 3.2.62.8 1996/09/12 13:32:10 fox Exp $
 * $Source: /release/112/cvs/Xsys/wan/nbfcp.c,v $
 *------------------------------------------------------------------
 * nbfcp.c - NCP routines for NBF (NetBios Frames) over PPP
 *
 * August 1995, Shoou Yiu
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: nbfcp.c,v $
 * Revision 3.2.62.8  1996/09/12  13:32:10  fox
 * CSCdi68759:  AAA per-user routes are not removed when connection drops
 * Branch: California_branch
 * Separate parallel connection support for per-user routes from the
 * support for neighbor routes and dynamic dialer maps.  Transfer
 * ownership of the static route to the last idb to add it (applies only
 * to per-user routes).
 *
 * Revision 3.2.62.7  1996/08/18  09:08:17  fox
 * CSCdi58100:  PPP option types displayed inconsistently
 * Branch: California_branch
 * Consistently display PPP codes and types in decimal.  Convert several
 * occurrences of '0x%x' to '%#x'.
 *
 * Revision 3.2.62.6  1996/07/23  13:31:44  thille
 * CSCdi63329: Centralize logging for getbuffer() and pak_duplicate()
 * failures
 * Branch: California_branch
 * Shamelessly rip off Mr. Katz' code from CSCdi36037.  Provide
 * centralized logging for getbuffer() and pak_duplicate() failures and
 * then rip out the 9 bazillion different messages that where in the
 * various callers to report the failure.
 *
 * Revision 3.2.62.5  1996/06/24  09:27:40  fox
 * CSCdi59550:  PPP causes router to crash
 * Branch: California_branch
 * Slim down and speed up linktype to NCP index to PPP protocol lookups.
 * Extend NCP protocheck functions to pass original IDB as well as config
 * IDB.  Change NCP names to upper case.  Display NCP name in trace of
 * PPP negotiation.
 *
 * Revision 3.2.62.4  1996/05/17  12:16:40  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.2.48.3  1996/05/06  00:09:23  ppearce
 * Sync to IbuMod_Calif_baseline_960504
 *
 * Revision 3.2.48.2  1996/04/03  22:13:04  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.2.48.1  1996/03/16  12:10:20  ppearce
 * CSCdi50498:  IBU modularity - Implement NetBios subblock
 * Branch: IbuMod_Calif_branch
 *   Move NetBios fields from hwidbtype/idbtype structs to NetBios subblock
 *
 * Revision 3.2.62.3  1996/05/10  01:45:08  wmay
 * CPP and PPP half-bridge commit
 * Branch: California_branch
 *
 * Revision 3.2.62.2  1996/04/29  09:45:53  fox
 * CSCdi55358:  PPP subsystem needs to be more modular
 * Branch: California_branch
 * Move Network Control Protocol (NCP) support from PPP subsystem to
 * individual subsystems.  Attach NCP subsystems to respective protocol
 * subsystems.  Add registry calls to network protocol init code to
 * register default NCP support for NCPs not requiring option negotiation.
 *
 * Revision 3.2.62.1  1996/03/18  22:48:07  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.2  1996/03/07  11:07:47  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.2.26.1  1996/02/20  21:52:19  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.2  1995/11/17  18:03:30  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:50:08  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:36:05  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/09/20  23:12:32  syiu
 * Initial placeholder for NBFCP/PPP
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
#include "master.h"
#include <string.h>
#include <ciscolib.h>
#include "sys_registry.h"
#include "media_registry.h"
#include "subsys.h"
#include "interface_private.h"
#include "packet.h"
#include "config.h"
#include "serial.h"
#include "ppp.h"
#include "ieee.h"
#include "ttysrv.h"
#include "nbfcp.h"
#include "ppp_public.h"
#include "ppp_registry.h"
#include "logger.h"
#include "../if/network.h"
#include "../llc2/llc2.h"
#include "../ibm/netbios_sb.h"

static void nbfcp_resetci(fsm *);	/* Reset our Configuration Information */
static int nbfcp_cilen(fsm *);		/* Return length of our CI */
static void nbfcp_addci(fsm *, uchar *);	/* Add our CIs */
static int nbfcp_ackci(fsm *, uchar *, int);	/* Ack some CIs */
static void nbfcp_nakci(fsm *, uchar *, int);	/* Nak some CIs */
static void nbfcp_rejci(fsm *, uchar *, int);	/* Reject some CIs */
static uchar nbfcp_reqci(fsm *, uchar *, short *);	/* Check the requested CIs */
static void nbfcp_up(fsm *);		/* Link came up */
static boolean nbfcpBLOCK(hwidbtype *);

fsm_callbacks nbfcp_callbacks = { /* NBFCP callback routines */
    nbfcp_resetci,		/* Reset our Configuration Information */
    nbfcp_cilen,		/* Length of our Configuration Information */
    nbfcp_addci,		/* Add our Configuration Information */
    nbfcp_ackci,		/* ACK our Configuration Information */
    nbfcp_nakci,		/* NAK our Configuration Information */
    nbfcp_rejci,		/* Reject our Configuration Information */
    nbfcp_reqci,		/* Request peer's Configuration Information */
    nbfcp_up,			/* Called when fsm reaches OPEN state */
    NULL,			/* Called when fsm leaves OPEN state */
    NULL,			/* Called when fsm reaches CLOSED state */
    NULL,			/* Called when Protocol-Reject received */
    NULL,			/* Retransmission is necessary */
	NULL,			/* Reset Request */
	NULL,			/* Rest Acknowledge */
	NULL			/* Code Rejected */
};

sys_timestamp nbfcp_timeout;

/*
 * nbfcp_init - Initialize NBFCP.
 */
void nbfcp_init (fsm *nbfcp_fsm)
{
    nbfcp_options *wo;
    nbfcp_options *ao;

    if (!nbfcp_fsm->wantoptions)
    	nbfcp_fsm->wantoptions = malloc(sizeof(nbfcp_options));
    else
	memset(nbfcp_fsm->wantoptions, 0, sizeof(nbfcp_options));
    if (!nbfcp_fsm->wantoptions)
	return;
    if (!nbfcp_fsm->allowoptions)
    	nbfcp_fsm->allowoptions = malloc(sizeof(nbfcp_options));
    else
	memset(nbfcp_fsm->allowoptions, 0, sizeof(nbfcp_options));
    if (!nbfcp_fsm->allowoptions)
	return;
    if (!nbfcp_fsm->heroptions)
    	nbfcp_fsm->heroptions = malloc(sizeof(nbfcp_options));
    else
	memset(nbfcp_fsm->heroptions, 0, sizeof(nbfcp_options));
    if (!nbfcp_fsm->heroptions)
	return;
    nbfcp_fsm->ouroptions = NULL;

    wo = nbfcp_fsm->wantoptions;
    ao = nbfcp_fsm->allowoptions;

    nbfcp_fsm->retransmits = 0;
    TIMER_STOP(nbfcp_fsm->timeouttime);
    nbfcp_fsm->failures = 0;
    nbfcp_fsm->callbacks = &nbfcp_callbacks;
    nbfcp_fsm->flags |= AOPENDING;           /* Active Open */

}



/*
 * nbfcp_resetci - Reset our CI.
 */

static void nbfcp_resetci (fsm *f)
{
    /* should use reg_invoke_ppp_setoptions... */
    nbfcp_options *wo, *ao;

    wo = f->wantoptions;
    ao = f->allowoptions;

    /*
     * Use defaults
     */
    wo->neg_name_projection = 0;
    wo->neg_peer_info = 0;
    wo->neg_multicast_filter = 0;
    wo->neg_ieee_mac_addr = 0;

    /* 
     * Set up the initial default values
     */
    wo->mcast_fwd_period = NBF_MCAST_FWD_PERIOD_DEF;
    wo->mcast_priority = NBF_MCAST_PRIO_DIRECTED;

    /*
     * The Peername is the hostname, with truncation if necessary
     */
    bcopy(hostname, wo->peername, (strlen(hostname) < MAX_CI_NBFARRAY) ? 
          strlen(hostname) : MAX_CI_NBFARRAY);
   
    /*
     * Allow negotiation of Name projection, Peer info, 
     * Multicast filtering and IEEE MAC header encapsulation
     */
    ao->neg_name_projection = 1;
    ao->neg_peer_info = 1;
    ao->neg_multicast_filter = 1;
    ao->neg_ieee_mac_addr = 1;
    f->gotoptions = f->wantoptions;
}


/*
 * nbfcp_cilen - Return length of our CI.
 */
static int nbfcp_cilen (fsm *f)
{
    nbfcp_options *go;

    go = f->gotoptions;

    return(LENCINAMEPROJ(go->neg_name_projection) +
           LENCIPEERINFO(go->neg_peer_info, strlen(go->peername)) +
           LENCIMCASTFLTR(go->neg_multicast_filter) +
           LENCIIEEEMAC(go->neg_ieee_mac_addr));
}


/*
 * nbfcp_addci - Add our desired CIs to a packet.
 */
static void nbfcp_addci (fsm *f, uchar *ucp)
{

    nbfcp_options *go;

    go = f->gotoptions;

    ADDCIMCASTFLTR(CI_NBF_MCAST_FILTER, go->neg_multicast_filter, 
                   go->mcast_fwd_period, go->mcast_priority); 
    ADDCIIEEEMAC(CI_NBF_IEEE_MAC_HDR, go->neg_ieee_mac_addr); 
}


/*
 * nbfcp_ackci - Ack our CIs.
 *
 * Returns:
 *	0 - Ack was bad.
 *	1 - Ack was good.
 *
 * This routine ensures that a received ACK message is valid. An ACK
 * message must contain all of the options that we specified, in the order
 * that we specified.
 */
static int nbfcp_ackci (fsm *f, uchar *p, int len)
{
    uchar cilen, citype;
    ushort cishort;
    uchar cichar;

    nbfcp_options *go;

    go = f->gotoptions;

    /*
     * CIs must be in exactly the same order that we sent...
     * Check packet length and CI length at each step.
     * If we find any deviations, then this packet is bad.
     */
    ACKCIMCASTFLTR(CI_NBF_MCAST_FILTER, go->neg_multicast_filter, 
                   go->mcast_fwd_period, go->mcast_priority); 
    ACKCIIEEEMAC(CI_NBF_IEEE_MAC_HDR, go->neg_ieee_mac_addr); 

    /*
     * If there are any remaining CIs, then this packet is bad.
     */
    if (len != 0)
	goto bad;
    return (1);
bad:
    if (ppp_error_debug)
	buginf("\nppp: nbfcp_ackci: received bad Ack!");
    return (0);
}


/*
 * nbfcp_nakci - NAK some of our CIs.
 *
 * This routine handles a received NAK message. The NAK message will
 * list all of the options that we specified, the peer supports, but
 * the peer would prefer a different value.
 */
static void nbfcp_nakci (fsm *f, uchar *p, int len)
{
    ushort cishort = 0;
    uchar  cichar = 0;
    nbfcp_options *go;

    go = f->gotoptions;

    /*
     * Any Nak'd CIs must be in exactly the same order that we sent.
     * Check packet length and CI length at each step.
     * If we find any deviations, then this packet is bad.
     */
    NAKCIMCASTFLTR(CI_NBF_MCAST_FILTER, go->neg_multicast_filter, 
           if ((cishort <= NBF_MCAST_FWD_PERIOD_MAX) && 
               (cishort != NBF_MCAST_FWD_PERIOD_NEG) && 
               ((cichar == NBF_MCAST_PRIO_DIRECTED) || 
                (cichar == NBF_MCAST_PRIO_MCAST))) {             
               go->mcast_fwd_period = cishort;
               go->mcast_priority = cichar;
           } else {
               if (ppp_negotiation_debug) {
		 buginf("\nppp: NBFCP mcast_filter period %#x, peer wants %#x", 
                        go->mcast_fwd_period, cishort);
                 buginf("\nppp: NBFCP mcast_priority %#x, peer wants %#x", 
                        go->mcast_priority, cichar);
               }                
                      
               /*
                * Don't try again
                */
               go->neg_multicast_filter = 0;
	   }
           );


    NAKCIIEEEMAC(CI_NBF_IEEE_MAC_HDR, go->neg_ieee_mac_addr,
                if (ppp_negotiation_debug)
		 buginf("\nppp: NBFCP IEEE MAC hdr %#x, peer wants %#x", 
                        go->neg_ieee_mac_addr, cichar);

                /*
                 * Don't try again
                 */
                 go->neg_ieee_mac_addr = 0;
                 
                );
                    
    /*
     * If there are any remaining CIs, then this packet is bad.
     * At this point, we could check for any appended options.
     */
    if (len == 0)
	return;

    if (ppp_error_debug)
	buginf("\nppp: nbfcp_nakci: ignoring unrequested options!");
   return;
}


/*
 * nbfcp_rejci - Reject some of our CIs.
 */
static void nbfcp_rejci (fsm *f, uchar *p, int len)
{
    ushort cishort;
    uchar cichar;
    nbfcp_options *go;

    go = f->gotoptions;

    /*
     * Any Rejected CIs must be in exactly the same order that we sent.
     * Check packet length and CI length at each step.
     * If we find any deviations, then this packet is bad.
     */
    REJCIMCASTFLTR(CI_NBF_MCAST_FILTER, go->neg_multicast_filter, 
                   go->mcast_fwd_period, go->mcast_priority); 
    REJCIIEEEMAC(CI_NBF_IEEE_MAC_HDR, go->neg_ieee_mac_addr); 


    /*
     * If there are any remaining CIs, then this packet is bad.
     */

    if (len == 0)
	return;
bad:
    if (ppp_error_debug)
	buginf("\nppp: nbfcp_rejci: received bad Reject!");
}


/*
 * nbfcp_reqci - Check the peer's requested CIs and send appropriate response.
 *
 * Returns: CONFACK, CONFNAK or CONFREJ and input packet modified
 * appropriately.
 */
static uchar nbfcp_reqci (fsm *f, uchar *inp, short *len)
{
    nbfcp_options *wo;
    nbfcp_options *ho;
    nbfcp_options *ao;
    nbfcp_options *go;
    paktype *pak;
    uchar *cip;		/* Pointer to Current CI */
    ushort cilen, citype;	/* Parsed len, type */
    int rc = CONFACK;		/* Final packet return code */
    int orc;			/* Individual option return code */
    uchar *p = inp;		/* Pointer to next char to parse */
    uchar *ucp = inp;		/* Pointer to current output char */
    int l = *len;		/* Length left */
    netbios_sb_t *netb_sb;      /* Ptr to the NetBios subblock */
 
    wo = f->wantoptions;
    ao = f->allowoptions;
    ho = f->heroptions;
    go = f->gotoptions;

    /*
     * Reset all her options.
     */

    /*
     * Process all her options.
     */
    while (l) {
	orc = CONFACK;			/* Assume success */
	cip = p;			/* Remember begining of CI */
	if (l < 2 ||			/* Not enough data for CI header or */
	    p[1] < 2 ||			/*  CI length too small or */
	    p[1] > l) {			/*  CI length too big? */
	    if (ppp_error_debug)
		buginf("\nppp: nbfcp_reqci: bad CI length!");
	    orc = CONFREJ;		/* Reject bad CI */
	    cilen = l;			/* Reject till end of packet */
	    l = 0;			/* Don't loop again */
	    goto endswitch;
	}
	GETCHAR(citype, p);		/* Parse CI type */
	GETCHAR(cilen, p);		/* Parse CI length */
	l -= cilen;			/* Adjust remaining length */
	cilen -= 2;			/* Adjust cilen to just data */

	switch (citype) {		/* Check CI type */
          case CI_NBF_NAME_PROJECTION:
	    if (!ao->neg_name_projection) {
		INCPTR(cilen, p); 	/* Skip rest of CI */
		orc = CONFREJ;		/* Reject CI */
		if (ppp_negotiation_debug)
		    buginf(" (rejected)");
		break;
	    }

            if (ppp_negotiation_debug) {
              int i, j;
              uchar *q;
              q = p;
              j = 0;
	      buginf("\nppp %s Negotiate NBFCP Name Projection\n",
                       f->idb->hw_namestring);
              for (i=0;i<cilen;i++) {
                if (j==16) {
                  buginf("%d\n", *q);
                  j = 0;
                } else {
                  buginf("%c", *q);
                  j++;
                }
                q++;
              }
            }

            /*
             * Copy the Name projection Names to a special pak
             * and send it to the the NBF process. 
             */
            pak = getbuffer(SMALLDATA);
            if (!pak) {
		INCPTR(cilen, p); 	/* Skip rest of CI */
		orc = CONFREJ;		/* Reject CI */
		break;
            } 

            /*
             * Make the pak a "NetBIOS SAP UI" packet to get pass 
             * scrutiny of initial nbf input processing
             */
            *pak->datagramstart = TYPE_PPP_NBF;
            pak->network_start = pak->datagramstart + 1;
            *(pak->datagramstart+1) = SAP_NETBIOS;
            *(pak->datagramstart+2) = SAP_NETBIOS;
            *(pak->datagramstart+3) = UI; 
            bcopy(p, (pak->datagramstart+4), cilen);
            pak->datagramsize = cilen + 3;   
            pak->flags |= PAK_CONTROL;
            pak->if_input = f->idb->firstsw;

            /*
             * Attach the pak to the interface and send the packet to the 
             * nbf process
             */
            netb_sb = netbios_get_or_create_sb(f->idb->firstsw);
            if (netb_sb) {
                netb_sb->netbios_nbfcp_pak = pak;
                netb_sb->nbfcp_pak_modified = FALSE;
            } else {
                printf(nomemory);
                return(0);
            }
            reg_invoke_raw_enqueue(LINK_NBF, pak);

            /*
             * Start a timer 
             */
            TIMER_START(nbfcp_timeout, ONESEC);
            edisms((blockproc *)nbfcpBLOCK, (long) f->idb);

            /*
             * If it woke up and the pak wasn't modified, it could be a 
             * timeout or an interface down etc. Consider the Name 
             * Projection to be succcessful then. 
             * Otherwise, the Name projection came back with some 
             * modifications. Copy the contents back wholesale, only now, 
             * they contain updated info. 
             */ 
            if (!netb_sb->nbfcp_pak_modified) {
              pak = netb_sb->netbios_nbfcp_pak;
              netb_sb->netbios_nbfcp_pak = NULL;
              retbuffer(pak);
              INCPTR(cilen, p);
              break;
            } else {
              netb_sb->nbfcp_pak_modified = FALSE;
              pak = netb_sb->netbios_nbfcp_pak;
              netb_sb->netbios_nbfcp_pak = NULL;
              if (pak) {
                bcopy(pak->datagramstart+4, p, cilen);
                retbuffer(pak);
              }

              if (ppp_negotiation_debug) {
                int i, j;
                uchar *q;
                q = p;
                j = 0;
                buginf("\nnbfcp %s: NAKing Name Projection Names:\n",
                       f->idb->hw_namestring);

                for (i=0;i<cilen;i++) {
                  if (j==16) {
                    buginf("%d\n", *q);
                    j = 0;
                  } else {
                    buginf("%c", *q);
                    j++;
                  }
                  q++;
                }
              }
              INCPTR(cilen, p);
              orc = CONFNAK;
            } 	
            break;
            
          case CI_NBF_PEER_INFO:
            if (ppp_negotiation_debug)
	        buginf("\nppp %s Negotiate NBFCP Peer Information",
                       f->idb->hw_namestring);
	    if (!ao->neg_peer_info) {
		INCPTR(cilen, p); 	/* Skip rest of CI */
		orc = CONFREJ;		/* Reject CI */
		if (ppp_negotiation_debug)
		    buginf(" (rejected)");
		break;
	    }
            /*
             * Extract values
             */
            PPP_GETSHORT(ho->remotepeerclass, p);
            PPP_GETLONG(ho->remotepeerversion, p);
            memset(ho->remotepeername, 0, MAX_CI_NBFARRAY );
            bcopy(p, ho->remotepeername, 
                  ((cilen-6) < MAX_CI_NBFARRAY) ? 
                   (cilen-6) : MAX_CI_NBFARRAY);
            INCPTR((cilen-6), p);
            if (ppp_negotiation_debug) {  
              buginf("\nppp %s NBFCP Peer Info: class=%#x version=%#x Name=%s\n", 
                     f->idb->hw_namestring, ho->remotepeerclass, 
                     ho->remotepeerversion, ho->remotepeername);
            }        
            break;
          case CI_NBF_MCAST_FILTER:
            if (ppp_negotiation_debug)
	        buginf("\nppp %s Negotiate NBFCP Multicast Filter",
                       f->idb->hw_namestring);
	    if (!ao->neg_multicast_filter) { /* Check CI length */
		INCPTR(cilen, p); 	/* Skip rest of CI */
		orc = CONFREJ;		/* Reject CI */
		if (ppp_negotiation_debug)
		    buginf(" (rejected)");
		break;
	    }
            /*
             * Just advance the pointer
             */
            if (ppp_negotiation_debug) {  
              int i;
              uchar *q;
              q = p;
              buginf("\n \nppp %s NBFCP Mcast filter: \n", 
                     f->idb->hw_namestring);
              for (i=0;i<cilen;i++) {
                buginf("%c", *q);
                q++;
              }
            }    
            INCPTR(cilen, p);
            break;
          case CI_NBF_IEEE_MAC_HDR:
            if (ppp_negotiation_debug)
	        buginf("\nppp %s Negotiate NBFCP IEEE MAC header",
                       f->idb->hw_namestring);
	    if (!ao->neg_ieee_mac_addr) { /* Check CI length */
		INCPTR(cilen, p); 	/* Skip rest of CI */
		orc = CONFREJ;		/* Reject CI */
		if (ppp_negotiation_debug)
		    buginf(" (rejected)");
		break;
	    }
            /*
             * Just advance the pointer
             */
            if (ppp_negotiation_debug) {  
              int i;
              uchar *q;
              q = p;
              buginf("\n \nppp %s NBFCP IEEE MAC header: \n", 
                     f->idb->hw_namestring);
              for (i=0;i<cilen;i++) {
                buginf("%c", *q);
                q++;
              }
            }   
            INCPTR(cilen, p);
            break;
 	  default:
	    if (ppp_error_debug)
	      buginf("\nppp %s: Rejecting unknown NBFCP option %d",
		     f->idb->hw_namestring, citype);
	    INCPTR(cilen, p);
	    orc = CONFREJ;
            break;
        }
	cilen += 2;			/* Adjust cilen whole CI */

endswitch:
    if (ppp_negotiation_debug)
	buginf(" (%s)",
		   orc == CONFACK ? "ACK" : (orc == CONFNAK ? "NAK" : "REJ"));
	if (orc == CONFACK &&		/* Good CI */
	    rc != CONFACK)		/*  but prior CI wasnt? */
	    continue;			/* Don't send ther one */

	if (orc == CONFNAK) {		/* Nak this CI? */
	    if (rc == CONFREJ)		/* Rejecting prior CI? */
		continue;		/* Don't send this one */
	    if (rc == CONFACK) {	/* Ack'd all prior CIs? */
		rc = CONFNAK;		/* Not anymore... */
		ucp = inp;		/* Backup */
	    }
	}
	if (orc == CONFREJ &&		/* Reject this CI */
	    rc != CONFREJ) {		/*  but no prior ones? */
	    rc = CONFREJ;
	    ucp = inp;			/* Backup */
	}
	if (ucp != cip)			/* Need to move CI? */
	    bcopy(cip, ucp, cilen);	/* Move it */
	INCPTR(cilen, ucp);		/* Update output pointer */
    }

    /*
     * XXX If we wanted to send additional NAKs (for unsent CIs), the
     * code would go here.  This must be done with care since it might
     * require a longer packet than we received.
     */

    *len = ucp - inp;			/* Compute output length */
    if (ppp_negotiation_debug)
	buginf("\nppp: nbfcp_reqci: returning %s.",
	      rc == CONFACK ? "CONFACK" :
	      rc == CONFNAK ? "CONFNAK" : "CONFREJ");
    return (rc);			/* Return final code */
}


/*
 * FSM has reached up state. Invoke the NBF protocol specific
 * for this media.
 */
static void nbfcp_up (fsm *f)
{
    reg_invoke_media_protocol_up(f->idb, LINK_NBF);
}


/*
 * Block waiting for a non-empty queue or timeout
 */
static boolean nbfcpBLOCK (hwidbtype *hwidb)
{
  if (AWAKE(nbfcp_timeout) || !interface_up_simple(hwidb->firstsw) ||
      (hwidb->idb_tty && modemcheck(hwidb->idb_tty)))
     return(FALSE);
  return(TRUE);
}

/*
 * nbfcp_init_subsys
 * Initialization of NBFCP subsystem
 */

static void nbfcp_init_subsys (subsystype *subsys)
{
    reg_invoke_ncp_register(LINK_NBF, TYPE_PPP_NBFCP,
			    nbfcp_init, NULL, "NBFCP");
}

/*
 * The NBFCP subsystem header
 */

#define NBFCP_MAJVERSION 1
#define NBFCP_MINVERSION 0
#define NBFCP_EDITVERSION  1

SUBSYS_HEADER(nbfcp, NBFCP_MAJVERSION, NBFCP_MINVERSION, NBFCP_EDITVERSION,
	      nbfcp_init_subsys, SUBSYS_CLASS_PROTOCOL,
	      "seq: ppp",
	      "req: netbios_as_name, ppp");
