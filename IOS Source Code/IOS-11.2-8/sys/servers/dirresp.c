/* $Id: dirresp.c,v 3.1.18.2 1996/08/28 13:08:46 thille Exp $
 * $Source: /release/112/cvs/Xsys/servers/dirresp.c,v $
 *------------------------------------------------------------------
 * director_responder.c -- Response agent for remote director queries
 *
 * Feburary 1996, Paul Traina
 *
 * Copyright (c) 1996-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: dirresp.c,v $
 * Revision 3.1.18.2  1996/08/28  13:08:46  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.1.18.1  1996/08/15  06:49:44  raj
 * CSCdi61197:  Part of  CSCdi49395 patches missing from 11.2
 * Branch: California_branch
 * Update Director Responder Agent to the latest version.
 *
 * Revision 3.1  1996/02/20  22:45:59  pst
 * Initial incorporation of distributed directory agent support
 *
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "subsys.h"
#include "interface_private.h"
#include "packet.h"
#include "config.h"
#include "parser.h"
#include "address.h"
#include "registry.h"
#include "sys_registry.h"
#include "logger.h"
#include "../servers/msg_dirresp.c"	/* Not a typo, see logger.h */
#include "login_public.h"
#include "access.h"
#include "../ui/debug.h"
#include "../ip/ip.h"
#include "../ip/ip_registry.h"
#include "../ip/ipaccess.h"
#include "../if/network.h"
#include "../iprouting/route.h"
#include "../crypto/keyman.h"
#include "../util/md5_rfc1828.h"
#include "dirresp.h"
#include "dirresp_debug.h"
#include "dirresp_private.h"


int	dirresp_acl;		/* access control */

/*
 * Local storage
 */
static ulong dirresp_requests;		/* input DRP packets */
static ulong dirresp_failures;		/* bad messages */
static ulong dirresp_successes;		/* successful route lookups */
static ulong dirresp_noroute;		/* failed route lookups */

static boolean	dirresp_enable;		/* is DRP agent enabled? */

char  *dirresp_keychain;		/* key chain authentication */

boolean	Delay_responses=FALSE;
pid_t	dirresp_delay_process=0;

#include "../util/random.h"
static queuetype	Delay_Q;
ulong			Maxdelaytime=2*ONESEC;

#ifdef	DEBUG
static void
dirresp_print_rtquery (drquerytype *entry) 
{
    dripv4qtype	*p;
    int		length;

    length = entry->len - sizeof(drquerytype);

    switch (entry->family) {
    case DRP_FAM_IPV4:
	p = (dripv4qtype *)entry->data;
	while (length >= sizeof(dripv4qtype)) {
	    buginf("\n\taddress=%i", p->address);
	    buginf("\n\tint_metric=%d, int_type=%d", p->int_metric,
		   p->int_type);
	    buginf("\n\text_metric=%d, ext_type=%d", p->ext_metric,
		   p->ext_type);
	    length -= sizeof(dripv4qtype);
	    p++;
	}
	break;
	
    default:
	buginf("\n\tUnknown family = %d", entry->family);
	break;
    }
}


/*
 * dirresp_print_drp
 *
 * Print out the contents of a DRP packet
 * (For debugging)
 */
void
dirresp_print_drp (drtype *msg) 
{
    drentrytype	*entry;
    int		length;

    buginf("\nmsg header:");
    buginf("\n\tlength=%d, version=%d, flags=0x%x", msg->length, msg->version,
	   msg->flags);

    length = msg->length - sizeof(drtype);
    entry = msg->entry;
    while (length > 0) {
	if (entry->len > length || entry->len < sizeof(drentrytype)) {
	    buginf("\n[error] entry->len=%d, remaining length=%d",
		   entry->len, length);
	    return;
	}
	switch (entry->op) {
	case DRP_OP_FAILURE:
	    buginf("\nType DRP_OP_FAILURE");
	    buginf("\n\tlen=%d, reason=%d", ((drfailtype *)entry)->len,
		   ((drfailtype *)entry)->reason);
	    break;

	case DRP_OP_AUTH:
	    buginf("\nType DRP_OP_AUTH");
	    buginf("\n\tlen=%d, mode=%d, key_id=%d", ((drauthtype *)entry)->len,
		   ((drauthtype *)entry)->mode,
		   ((drauthtype *)entry)->key_id);
	    break;

	case DRP_OP_RTQUERY:
	    buginf("\nType DRP_OP_RTQUERY");
	    goto rtqueryprnt;		/* below */
	case DRP_OP_RTRESP:
	    buginf("\nType DRP_OP_RTRESP");
	rtqueryprnt:
	    buginf("\n\tlen=%d, family=%d", ((drquerytype *)entry)->len,
		   ((drquerytype *)entry)->family);
	    dirresp_print_rtquery((drquerytype *)entry);
	    break;
	}
	length -= entry->len;
	entry   = (drentrytype *) ((uchar *)entry + entry->len);
    }
}


#endif

/*
 * dirresp_ipv4_rtlookup
 * Query the IPv4 routing table for metric information
 * This should move into ip routing and have a protocol-inspecific
 * argument format.
 */
static void
dirresp_ipv4_lookup (dripv4qtype *p)
{
    ipaddrtype nexthop;
    ndbtype *ndb;
    
    dirresp_requests++;
    
    PUTLONG(&p->int_metric, DRP_UNREACHABLE);
    PUTLONG(&p->ext_metric, 0);
    p->int_type = p->ext_type = DRP_METTYPE_UNKNOWN;
    
    ndb = ip_rtlookup(p->address, 0);
    if (!ndb) {
	errmsg(&msgsym(NOROUTE, DRP), p->address);
	dirresp_noroute++;
	return;
    }	
    
    if (NDB2PROCTYPE(ndb) & PROC_BGP) {
	PUTLONG(&p->ext_metric, ndb->rdb[0]->hopcount);
	p->ext_type = DRP_METTYPE_BGP;
    }	
    
    nexthop = p->address;
    ndb = NULL;
    
    if (!iprouting_first_hop(&nexthop, nexthop, 0L, ROUTINGDEPTH,
			     IP_PATH_EXACT_MATCH, &ndb)) {
	errmsg(&msgsym(NOROUTE, DRP), p->address);
	dirresp_noroute++;
	return;
    }
    
    if (!ndb) {
	errmsg(&msgsym(NOROUTE, DRP), p->address);
	dirresp_noroute++;
	return;
    }
    
    if (ndb->attrflags & NET_SUBNETTED) {
	errmsg(&msgsym(SUBNETTED, DRP), p->address, ndb->netnumber);
	return;
    }
    
    PUTLONG(&p->int_metric, ndb->metric);
    
    switch (NDB2PROCTYPE(ndb)) {
    case PROC_ISIS:
        p->int_type = DRP_METTYPE_ISIS;
        break;
    case PROC_OSPF:
        p->int_type = DRP_METTYPE_OSPF;
        break;
    case PROC_RIP:
        p->int_type = DRP_METTYPE_RIP;
        break;
    case PROC_IGRP:
        p->int_type = DRP_METTYPE_IGRP;
        break;
    case PROC_IGRP2:
        p->int_type = DRP_METTYPE_IGRP2;
        break;
    case PROC_STATIC:
        p->int_type = DRP_METTYPE_STATIC;
        break;
    case PROC_CONNECTED:
        p->int_type = DRP_METTYPE_CONNECTED;
	break;
    case PROC_MOBILE:
        p->int_type = DRP_METTYPE_MOBILE;
        break;
    }

    dirresp_successes++;

    return;
}

/*
 * dirresp_op_rtquery
 * Fill in metric information for all entries
 */
static int
dirresp_op_rtquery (drquerytype *entry)
{
    dripv4qtype *p;
    int length;

    if (entry->family != DRP_FAM_IPV4)
	return DRP_FAIL_FAMILY;

    length = entry->len - sizeof(drquerytype);
    p      = (dripv4qtype *) entry->data;

    while (length >= sizeof(dripv4qtype)) {
	dirresp_ipv4_lookup(p);

	if (dirresp_debug)
	    buginf("\nDRP: RTQUERY for %i returned internal=%u, "
		   "external=%u %s",
		   GETLONG(&p->address), GETLONG(&p->int_metric),
		   GETLONG(&p->ext_metric),
		   GETLONG(&p->int_metric) >= DRP_UNREACHABLE ?
		   "(inaccessible)" : "");

	length   -= sizeof(dripv4qtype);
	p++;
    }

    if (length != 0)
	return DRP_FAIL_FORMAT;

    entry->op = DRP_OP_RTRESP;	/* we can just reuse message */

    return DRP_FAIL_NOFAIL;	/* success */
}

int
dirresp_op_authenticate (drauthtype *entry, char *data, int length,
			 boolean *authenticated)
{
    uchar	computed_digest[DRP_AUTH_DIGEST_LEN];
    char	storage[DRP_AUTH_DIGEST_LEN+1];	/* max useful key is 16bytes */
    int		id;
    char	*key;
    int		msglength;
    drtype	*msg;

    if (!dirresp_keychain)		/* this isn't a real failure */
	return DRP_FAIL_NOFAIL;

    if (entry->mode != DRP_AUTH_KMD5)	/* we only understand keyed MD5 */
	return DRP_FAIL_AUTH;

    /* save away the msg->length field */
    msg = (drtype *)data;
    msglength = msg->length;
    msg->length = 0;

    *authenticated = FALSE;		/* not valid until proven so */

    /* Only try the same key id# which was specified in the packet */
    id = entry->key_id;
    key = reg_invoke_key_get_next_live_key(dirresp_keychain, &id,
					   NULL, KEY_ACCEPT_LIFETIME,
					   storage, sizeof(storage));
    if (key && id == entry->key_id) {
	md5_hmac(data, length, key, strlen(key),
		 computed_digest, sizeof(computed_digest));
	
	if (!memcmp(computed_digest, entry->info,
		    DRP_AUTH_DIGEST_LEN)) {
	    *authenticated = TRUE;
	    msg->length = msglength;
	    return DRP_FAIL_NOFAIL;
	}
    }

    msg->length = msglength;
    return DRP_FAIL_NOFAIL;
}

/*
 * dirresp_fillin_auth
 *
 * Fill in an authentication entry at the location passed.
 * (There is an assumption here that the pak already contains enough space
 * for this authentication entry to be put here.)
 */
static void
dirresp_fillin_auth (drauthtype *entry, drtype *msg, char *key, int keyid,
		     int querylength)
{
    int		msglength;
    
    entry->mandatory = TRUE;
    entry->op	     = DRP_OP_AUTH;
    entry->len = sizeof(drauthtype);

    entry->mode	     = DRP_AUTH_KMD5;
    entry->key_id    = keyid;
    memset(entry->info, 0, sizeof(entry->info));	/* zero out info */

    msglength = msg->length;
    msg->length = 0;
    msg->flags |= DRP_FLAG_AUTH;
    
    /*
     * MD5 is calculated over the packet minus the auth field(s) at the end
     * after setting the msg->length field to zero.
     */
    md5_hmac((uchar *) msg, querylength, key, strlen(key),
	     entry->info, sizeof(entry->info));

    msg->length	= msglength;		/* restore it */
    msg->length	+= sizeof(drauthtype);	/* account for entry */
}

/*
 * dirresp_append_auth
 *
 * If we have a drp authentication keychain defined, then append an
 * authentication record to the end of the DRP packet passed.
 */
boolean
dirresp_append_auth (drtype *msg, int querylength, int id)
{
    char	*key;
    char	storage[DRP_AUTH_DIGEST_LEN+1];	/* max useful key is 16bytes */
    drauthtype	*entry;

    if (! dirresp_keychain)
	return (FALSE);

    while ((key = reg_invoke_key_get_next_live_key(dirresp_keychain, &id,
					   NULL, KEY_ACCEPT_LIFETIME,
					   storage, sizeof(storage)))
	   != NULL) {

	entry = (drauthtype *)((uchar *) msg + msg->length);
	dirresp_fillin_auth(entry, msg, key, id, querylength);
	id++;
    }

    return (msg->flags);
}


/*
 * dirresp_update_auth
 *
 * Update all of the authentication field(s) in the packet passed to make
 * sure they agree with the authentication key we're supposed to be using.
 * How this is done:
 * Foreach (key <- valid_authentication_key) {
 *   look for next authentication record in this packet;
 *   if (found) {
 *      overwrite it with the new key information;
 *   } else {
 *	count the number of keys which we still need to put onto this packet;
 *      reallocate a new packet large enough to hold everything plus
 *       the new auth information.
 *      copy the packet to the new one;
 *	continue with overwriting keys;
 *   }
 * }
 * Most of the time we should be able to simply overwrite the auth fields
 * which are there with the new information.  I wouldn't think we'd need to
 * copy the packet to a new one very often at all.
 */
static void
dirresp_update_auth (paktype *pak, drtype *msg) 
{
    char	*key;
    int		id;
    char	storage[DRP_AUTH_DIGEST_LEN+1];	/* max useful key is 16bytes */
    int		paksize, nextid, length, querylength, numkeys;
    drentrytype	*entry;
    paktype	*newpak;

    /* Find first authentication record */
    paksize = msg->length + UDPHEADERBYTES + IPHEADERBYTES(NOPT);
    length = msg->length - sizeof(drtype);
    entry = msg->entry;
    querylength = sizeof(drtype);	/* size of DRP header */
    id = 0;				/* starting key id */
    msg->length = sizeof(drtype);	/* init. */
    while (length > 0) {
	if (entry->len > length || entry->len < sizeof(drentrytype)) {
	    buginf("\nError in re-writing MD5 authentication fields");
	    return;			/* error in packet */
	}
	if (entry->op == DRP_OP_AUTH) {
	    /* find next valid key */
	    if ((key = reg_invoke_key_get_next_live_key(dirresp_keychain, &id,
							NULL,
							KEY_ACCEPT_LIFETIME,
							storage,
							sizeof(storage)))
		!= NULL) {
		/* fill it into this auth record */
		dirresp_fillin_auth((drauthtype *)entry, msg, key, id,
				    querylength);
		id++;
	    } else
		return;			/* no more valid keys */
	} else {
	    querylength += entry->len;	/* add in this record length */
	    msg->length += entry->len;
	}
	length -= entry->len;
	entry = (drentrytype *)((uchar *)entry + entry->len);
    }

    /* Look for more keys */
    numkeys = 0;
    nextid = id;
    while ((key = reg_invoke_key_get_next_live_key(dirresp_keychain, &nextid,
						   NULL,
						   KEY_ACCEPT_LIFETIME,
						   storage,
						   sizeof(storage)))
	   != NULL) {
	numkeys++;
	nextid++;
    }

    if (numkeys) {
	/* and allocate space for this many more auth fields */
	if ((newpak = pak_grow(pak, paksize,
			       paksize + (numkeys * sizeof(drauthtype))))
	    != NULL) {
	    /* continue with rest of keys */
	    dirresp_append_auth(msg, querylength, id);
	}
    }

    return;
}



/*
 * dirresp_send_failure
 * Generate a failure message back to the query system.
 *
 * Thank gcc for register utilization, this doesn't generate disgusting
 * code.
 */
void
dirresp_send_failure (paktype *old_pak, void *data, int reason)
{
    paktype     *new_pak;
    iphdrtype   *old_ip;
    udptype     *old_udp;
    drtype      *new_msg;
    drfailtype  *failure;
    
    old_ip  = (iphdrtype *) ipheadstart(old_pak);
    old_udp = (udptype *)   ipdatastart(old_pak);

    if (dirresp_debug)
	buginf("\nDRP: sending code %d failure message to %i:%u",
	       reason, old_ip->srcadr, old_udp->sport);

    new_pak = getbuffer(IPHEADERBYTES(NOPT)+UDPHEADERBYTES+
			sizeof(drtype)+sizeof(drfailtype));
    if (new_pak) {
	new_msg = (drtype *) (ipheadstart(new_pak) +
			      IPHEADERBYTES(NOPT) + UDPHEADERBYTES);

	new_msg->length  = sizeof(drtype) + sizeof(drfailtype);
	new_msg->version = DRP_VERSION;

	failure = (drfailtype *) new_msg->entry;
	failure->mandatory = 1;
	failure->op        = DRP_OP_FAILURE;
	failure->len       = sizeof(drfailtype);

	failure->reason    = reason;
	if (data)
	    memcpy(failure->info, data, sizeof(failure->info));
	else
	    memset(failure->info, 0, sizeof(failure->info));

	if (!ip_udp_send(new_pak, old_ip->dstadr, old_ip->srcadr,
			 old_udp->dport, old_udp->sport))
	    errmsg(&msgsym(NOWRITE, DRP), old_ip->srcadr, old_udp->sport);
    }
    datagram_done(old_pak);

    dirresp_failures++;
}

/*
 * This will have to change in the future, but for now, we can
 * get away with reusing info in the packet to respond to the
 * query-originator.  (We filled in the queries with responses)
 *
 */
static void
dirresp_respond (paktype *pak, drtype *msg)
{
    iphdrtype *ip  = (iphdrtype *) ipheadstart(pak);
    udptype   *udp = (udptype *)   ipdatastart(pak);

    msg->flags = 0;

    /* Take care of authentication field(s) */
    dirresp_update_auth(pak, msg);

#ifdef	DEBUG
    if (dirresp_debug) {
	buginf("\nReply packet:");
	dirresp_print_drp(msg);
    }
#endif

    udp->dport  = udp->sport;
    udp->sport  = DIRECTOR_PORT;
    udp->checksum = 0;
    udp->length = msg->length + UDPHEADERBYTES;
    udp->checksum = CheckSum((uchar *)udp, udp->length,\
			     ip->srcadr, ip->dstadr, UDP_PROT);

    pak->flags &= ~PAK_SENDSELF; /* turn off looping packet detection XXX */

    if (Delay_responses) {
	TIMER_START(pak->gp_timer, random_gen() % Maxdelaytime);
	if (dirresp_debug)
	    buginf("\nDelaying DRP response by %lTf sec's",
		   pak->gp_timer);
	enqueue(&Delay_Q, pak);
    } else
	ipwrite(pak, udp->length + IPHEADERBYTES(NOPT), UDP_PROT,
		ip->dstadr, ip->srcadr,
		NULL, NULL, &ipdefaultparams);
}

static boolean
dirresp_input (paktype *pak, udptype *udp)
{
    iphdrtype   *ip  = (iphdrtype *) ipheadstart(pak);
    drtype 	*msg = (drtype *)    udp->udpdata;
    drentrytype	*entry;
    int		 length, querylength, error;
    boolean	 authenticated;

    if (udp->dport != DIRECTOR_PORT)
	return(FALSE);

    if (!dirresp_enable)
	return(FALSE);

    if (dirresp_acl && !fast_check(dirresp_acl, ip->srcadr)) {
	send_unreachable(pak, ICMP_UNRPORT);
	datagram_done(pak);
	return(TRUE);
    }

    if (dirresp_debug)
	buginf("\nDRP: received v%d packet from %i, via %s",
	    msg->version, ip->srcadr, pak->if_input->namestring);

#ifdef	DEBUG
    if (dirresp_debug) {
	buginf("\nIncoming packet:");
	dirresp_print_drp(msg);
    }
#endif

    if (msg->version < DRP_MIN_VER || msg->version > DRP_MAX_VER) {
	error = DRP_FAIL_VERSION;
	entry = NULL;
	goto punt;
    }

    length = msg->length - sizeof(drtype);
    entry  = msg->entry;
    error  = DRP_FAIL_NOFAIL;
    authenticated = FALSE;
    querylength = sizeof(drtype);	/* size of DRP header */

    /*
     * If this is authenticated, we need to verify that first before
     * we start filling in the rest of the packet.
     *
     * Look through the entire packet for the authentication struct(s)
     * and see if any of them are valid.
     */
    if (msg->flags & DRP_FLAG_AUTH) {
	while (length > 0) {
	    if (entry->len > length || entry->len < sizeof(drentrytype)) {
		error = DRP_FAIL_FORMAT;
		break;
	    } else {
		if (entry->op == DRP_OP_AUTH) {
		    error = dirresp_op_authenticate((drauthtype *) entry,
						    (char *) msg,
						    querylength,
						    &authenticated);
		    if (authenticated)
			break;	/* that's all we were after */
		} else
		    querylength += entry->len;
	    }
	    length -= entry->len;
	    entry   = (drentrytype *) ((uchar *)entry + entry->len);
	}
    }

    /*
     * Now search through the rest of the packet and perform
     * operations.
     */
    if (!authenticated && dirresp_keychain) {
	dirresp_send_failure(pak, msg->entry, DRP_FAIL_AUTH);
	return (TRUE);
    }
    
    length = msg->length - sizeof(drtype);
    entry  = msg->entry;
    error  = DRP_FAIL_NOFAIL;

    while (length > 0) {
	if (entry->len > length || entry->len < sizeof(drentrytype))
	    error = DRP_FAIL_FORMAT;

	else {
	    switch (entry->op) {
	    case DRP_OP_FAILURE:
	    case DRP_OP_RTRESP:
		error = DRP_FAIL_COMMAND;	/* we never see these */
		break;

	    case DRP_OP_AUTH:
		/* done above */
		break;

	    case DRP_OP_RTQUERY:
		error = dirresp_op_rtquery((drquerytype *) entry);
		break;

	    default:
		if (entry->mandatory)
		    error = DRP_FAIL_COMMAND;
	    }
	}

	if (error != DRP_FAIL_NOFAIL)
	    break;

	length -= entry->len;
	entry   = (drentrytype *) ((uchar *)entry + entry->len);
    }

 punt:
    if (error != DRP_FAIL_NOFAIL) {
	dirresp_send_failure(pak, entry, error);
	return(TRUE);
    }

    dirresp_respond(pak, msg);
    return(TRUE);
}

/*
 * dirresp_background
 *
 * Background process for the director responder server.  This process
 * simply removes a "pak" structure from the "Delay_Q" if it has been
 * there long enough, and sends it out.
 */
process
dirresp_background (void)
{
    paktype	*pak, *nextpak;
    iphdrtype	*ip;
    udptype	*udp;

    for (;;) {
	for ((pak = (paktype *)Delay_Q.qhead) ; pak ; pak = nextpak) {
	    nextpak = pak->next;
	    
	    if (TIMER_RUNNING_AND_SLEEPING(pak->gp_timer)) continue;

	    unqueue(&Delay_Q, pak);
	    ip  = (iphdrtype *) ipheadstart(pak);
	    udp = (udptype *)   ipdatastart(pak);
	    ipwrite(pak, ip->tl, UDP_PROT, ip->dstadr, ip->srcadr,
		    NULL, NULL, &ipdefaultparams);
	}
	
	process_wait_for_event_timed(10); /* 10 millisec accuracy */
    }
}

void
dirresp_delay(parseinfo *csb)
{

    if (csb->nvgen) return;

    Delay_responses = csb->sense;

    if (csb->sense) {
	if (dirresp_enable && !dirresp_delay_process)
	    dirresp_delay_process = process_create(dirresp_background,
						   "Dirresp delayer",
						   NORMAL_STACK,
						   PRIO_NORMAL);
    } else if (dirresp_delay_process) {
	process_kill(dirresp_delay_process);
	dirresp_delay_process = 0;
    }

    printf("Delayed responsed are now %s\n", Delay_responses ? "on" : "off");
}

/*
 * dirresp_command
 * process director responder commands
 */
void
dirresp_command (parseinfo *csb)
{
    if (csb->nvgen) {
	switch (csb->which) {
	case DRP_CMD_AUTH_KEYCHAIN:
	    nv_write(dirresp_keychain != NULL, "%s %s", csb->nv_command,
		     dirresp_keychain);
	    break;

	case DRP_CMD_ACCESS:
	    nv_write(dirresp_acl != 0, "%s %d", csb->nv_command, dirresp_acl);
	    break;

	case DRP_CMD_ENABLE:
	    nv_write(dirresp_enable, "%s", csb->nv_command);
	    break;

	default:
	    bad_parser_subcommand(csb, csb->which);
	    break;
	}
	return;
    }

    switch (csb->which) {
    case DRP_CMD_AUTH_KEYCHAIN:
	setstring(&dirresp_keychain, 
		  csb->sense ? GETOBJ(string, 1) : NULL);
	break;

    case DRP_CMD_ACCESS:
	dirresp_acl = csb->sense ? GETOBJ(int, 1) : 0;
	break;

    case DRP_CMD_ENABLE:
	dirresp_enable = csb->sense;
	if (csb->sense) {
	    if (Delay_responses && !dirresp_delay_process)
		dirresp_delay_process = process_create(dirresp_background,
						       "Dirresp delayer",
						       NORMAL_STACK,
						       PRIO_NORMAL);
	    reg_add_ip_udp_input(dirresp_input, "dirresp_input");
	} else {
/*
   this call should be:
            reg_delete_ip_udp_input(dirresp_input);
   but there's a bug in the registry compliation code currently
   so that this registery call doesn't exist.  Instead, we do what it
   would do until the registery compilation is fixed.
   (As per Gordon Cox - Nov 7, '96)
 */
	    registry_delete_list(REG_IP, SERVICE_IP_UDP_INPUT, dirresp_input);
	    if (dirresp_delay_process) {
		process_kill(dirresp_delay_process);
		dirresp_delay_process = 0;
	    }
	}
	break;
	
    default:
	bad_parser_subcommand(csb, csb->which);
	break;
    }
}

/*
 * dirresp_show
 * show director responder statistics
 */
void
dirresp_show (parseinfo *csb)
{
    printf("Director Responder Protocol Agent is %s\n",
	   dirresp_enable ? "enabled" : "disabled");
    if (!dirresp_enable)
	return;
    
    printf("%d director requests, %d successful lookups, %d failures, %d no route\n",
	   dirresp_requests, dirresp_successes,
	   dirresp_failures, dirresp_noroute);

    if (dirresp_keychain)
	printf("Authentication is enabled, using \"%s\" key-chain\n",
	       dirresp_keychain);

    if (dirresp_acl)
	printf("Director requests filtered by access-list %d\n", dirresp_acl);

    if (Delay_responses)
	printf("Director Responder responses are being delayed\n");
}

/*
 * dirresp_clear
 *
 * Clear director responder counters
 */
void
dirresp_clear (parseinfo *csb)
{

    dirresp_requests = 0;
    dirresp_successes = 0;
    dirresp_failures = 0;
    dirresp_noroute = 0;
}


/*
 * subsystem initialization
 */
static void
dirresp_init (subsystype *subsys)
{
    dirresp_parser_init();
    dirresp_debug_init();

    dirresp_enable = FALSE;	/* off by default */

#ifdef	DELAY_RESPONSES
    queue_init(&Delay_Q, 0);
#endif
}

/*
 * DR subsystem header
 */

#define DR_MAJVERSION 1
#define DR_MINVERSION 1
#define DR_EDITVERSION 1

SUBSYS_HEADER(director_responder, DR_MAJVERSION, DR_MINVERSION,
	      DR_EDITVERSION, dirresp_init, SUBSYS_CLASS_PROTOCOL,
	      NULL, "req: iprouting");
