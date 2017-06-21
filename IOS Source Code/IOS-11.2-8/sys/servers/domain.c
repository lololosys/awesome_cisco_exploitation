/* $Id: domain.c,v 3.9.4.5 1996/07/23 13:28:03 thille Exp $
 * $Source: /release/112/cvs/Xsys/servers/domain.c,v $
 *------------------------------------------------------------------
 * domain.c -- domain service support.  RFC 882, 883, etc.
 *
 * 27-October-1985, Kirk Lougheed
 *
 * Copyright (c) 1985-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: domain.c,v $
 * Revision 3.9.4.5  1996/07/23  13:28:03  thille
 * CSCdi63329: Centralize logging for getbuffer() and pak_duplicate()
 * failures
 * Branch: California_branch
 * Shamelessly rip off Mr. Katz' code from CSCdi36037.  Provide
 * centralized logging for getbuffer() and pak_duplicate() failures and
 * then rip out the 9 bazillion different messages that where in the
 * various callers to report the failure.
 *
 * Revision 3.9.4.4  1996/05/21  10:01:29  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.9.4.3  1996/04/15  21:22:37  bcole
 * CSCdi54649:  IP Host Modularity, IP Automatic Stub Routing feature
 * commit
 * Branch: California_branch
 *
 * Revision 3.9.4.2  1996/04/12  22:48:43  raj
 * CSCdi50947:  very large DNS TTLs can cause TIMERNEG error message
 * A perfect use for TIMER_START64 instead of TIMER_START
 * Branch: California_branch
 *
 * Revision 3.9.4.1  1996/03/18  21:46:58  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.4.2.3  1996/03/16  07:28:34  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.4.2.2  1996/03/07  10:42:02  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.4.2.1  1996/02/20  17:09:37  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.9  1996/03/09  22:57:22  widmer
 * CSCdi47890:  HTTP server doesnt work
 * Add HTTP server
 *
 * Revision 3.8  1996/02/14  19:47:19  asb
 * CSCdi47788:  servers/bootp.c does not return second async-bootp
 * dns-server entry
 *
 * Don't return the -1 entry.
 *
 * Revision 3.7  1996/02/14  19:36:49  asb
 * CSCdi48113:  Should deliver NetBios NS [implement rest of RFC1877]
 *
 * Revision 3.6  1996/01/26  17:25:17  asb
 * CSCdi46322:  Should return async-bootp dns-server instead of routers
 * DNS server
 *
 * Revision 3.5  1996/01/24  20:19:51  jenny
 * CSCdi47046:  TCP/IP need code clean up (part 3)
 * . removed ip/tuba.c, ip/tuba_default.c and ip/tuba.h
 * . removed all includes for ip/tuba.h.
 * . removed more TUBA related code.
 *
 * Revision 3.4  1996/01/22  06:59:10  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.3  1995/11/29  22:09:53  pst
 * CSCdi44659:  clean up internals of domain code (make an interface API)
 *
 * Revision 3.2  1995/11/17  18:51:14  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:11:43  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.6  1995/09/13  07:01:50  hampton
 * All code should use the routine GETSHORT()/PUTSHORT() instead of using
 * the routines getshort()/putshort().  [CSCdi40249]
 *
 * Revision 2.5  1995/08/30 07:41:06  pitargue
 * CSCdi30543:  dns invalid  with domain-name and domain-list specified
 * create a name lookup path for FQDN's only.  domain-list will not affect
 * the lookup, i.e., domains will not be appended to FQDN's.
 *
 * Revision 2.4  1995/08/08  16:48:34  hampton
 * Miscellaneous changes to eliminate direct references to the system
 * clock and to convert to always using the timer macros to access timers.
 * [CSCdi38391]
 *
 * Revision 2.3  1995/07/10 21:39:42  ahh
 * CSCdi36874:  RSH: reverse name-lookup fails
 * Restore accidentally deleted "else".
 *
 * Revision 2.2  1995/06/21  09:00:35  smackie
 * Rename malloc memory pool derivatives so that they're prefixed with
 * malloc_ for consistency. (CSCdi36222)
 *
 * Revision 2.1  1995/06/07  22:44:07  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include <string.h>
#include <ciscolib.h>
#include "interface_private.h"
#include "packet.h"
#include "../ui/debug.h"
#include "../os/os_debug_flags.h"
#include "../ip/ip.h"
#include "../ip/udp_debug.h"
#include "domain.h"
#include "address.h"
#include "name.h"
#include "config.h"
#include "registry.h"
#include "../ip/ip_registry.h"
#include "../h/sys_registry.h"
#include "../ip/ip_debug_flags.h"
#include "../util/random.h"
#include "../h/cs_registry.regh"

#include "domain_private.h"
#include "subsys.h"
#include "parser.h"
#include "../parser/parser_actions.h"
#include "../parser/macros.h"

/*
 * Forward declarations of local functions.
 * This is ugly, the file should be reorganized so routines self-declare.
 */

static char	*eofield(char *);
static boolean	 domained(char *);
static nametype *domain_query(char *, ipaddrtype, short);
static nametype *domain_FQDN_query(char *, ipaddrtype);
static nametype *domain_query1(char *, char *, short, ipsocktype *);
static nametype *dom2cache(domaintype *, int);
static int	 comp_expand(char *, char *, char *, char *, int);
static int	 comp_skip(char *);

#ifdef	DEBUG
static void	 printdomain(domaintype *, int);
#endif


/*
 * domain_match
 * Compare "canonical" string to "target" string.
 *
 * Return TRUE if they match as is, or if they match when target has the
 * first default domain string appended.  Only the first default domains
 * is considered.  If any other default domains were considered, then we
 * would run the risk of providing the wrong host.  For example: default
 * domains ENG.CISCO.COM, CISCO.COM, STANFORD.EDU.  I open a connection
 * to 'castor.stanford.edu' (a mythical host, I admit), and then open a
 * connection to 'castor'.  If I search all of the default domains for a
 * match first, I get castor.stanford.edu instead of castor.cisco.com.
 */

#define MAXMATCHSIZE 50

boolean domain_match (char *canon, char *target)
{
    char newtarget[MAXMATCHSIZE];
    char *cp, *nt, *dname;
    char c;
    
    if (!strcasecmp(canon, target))	/* if exact, then it's easy */
	return(TRUE);
    if (QUEUEEMPTY(&domain_listQ)) {
	if (domained(target) || (default_domain == NULL))
	    return(FALSE);
	dname = default_domain;
    } else {
	if (target[strlen(target)-1] == '.')
	    return(FALSE);
	dname = ((domainlisttype *)domain_listQ.qhead)->name;
    }

    if ((strlen(target) + strlen(dname) + 2) >= MAXMATCHSIZE)
	return(FALSE);
    nt = (char *) newtarget;		/* build new target string */
    cp = target;			/* start with old target string */
    while ((c = *cp++)) {		/* add it */
	*nt++ = c;
    }
    *nt++ = '.';			/* add a dot */
    cp = dname;				/* end with default domain */
    while ((c = *cp++)) {		/* add it */
	*nt++ = c;
    }
    *nt++ = '\0';			/* add a NUL */
    return(!strcasecmp(canon, newtarget));	/* retry the comparison */
}

/*
 * eofield
 * Return pointer to end of a field.  Fields are terminated by a dot
 * or by the end of string (a NULL).
 */

static
char *eofield (char *ptr)
{
    while ((*ptr != '.') && (*ptr != '\0'))
	ptr++;
    return(ptr);
}

/*
 * domained
 * Returns TRUE if string has a domain specification, FALSE otherwise.
 */

static
boolean domained (char *ptr)
{
    while (*ptr != '\0')
	if (*ptr++ == '.')
	   return(TRUE);
    return(FALSE);
}

/* 
 * comp_copy
 * Copy a string into a buffer, converting to compressed format.
 * Returns length of compressed string in bytes.
 * Note that we don't use internal pointers in the compressed string.
 */

int comp_copy (char *ptr, char *name, int bytes, char *domdefault)
{
    char *optr = ptr;
    int len, hasdomain;

    hasdomain = domained(name);
    len = eofield(name) - name;
    while (len) {
        if (--bytes < 0)
	    return(0);
        *ptr++ = len;
	while ((*name != '.') && (*name != '\0')) {
            if (--bytes < 0)
	        return(0);
	    *ptr++ = *name++;
	}
	if (*name == '.')
	    name++;
	len = eofield(name) - name;		
    }
    if (!hasdomain && domdefault) {
	 len = comp_copy(ptr,domdefault,bytes,NULL);
	 if (len != 0)
	     ptr += len;
	 else
	    return(0);
    }
    else {
        if (--bytes < 0)
	    return(0);
	*ptr++ = '\0';
    }
    return(ptr-optr);
}

/*
 * domain_name_lookup
 * Use domain server to translate a name into an address
 * Returns cache pointer or NULL
 */

nametype *domain_name_lookup (char *namestring, int kind)
{
    int i;
    ipaddrtype server;
    nametype *ptr;

    for (i = 0; i < MAXDOMAINSERVERS; i++) {
	if (!(server = domain_server[i]))
	    return(NULL);

	printf(" (%i)", server);
	flush();

	if (kind & NAM_IP) {
	    ptr = domain_query(namestring,server,TYPE_A);
	    if (ptr)			/* found the host */
		return(ptr);
	}

	if (kind & NAM_CLNS) {
	    ptr = domain_query(namestring,server,TYPE_NSAP);
	    if (ptr)			/* found the host */
		return(ptr);
	}

	if (psipending())	/* interrupted */
	    return(NULL);
				/* no reply from server; try next one */
    }
    return(NULL);
}

/*
 * domain_address_lookup
 * Use domain server to translate an address into a name.
 * Returns cache pointer or NULL
 */

nametype *domain_address_lookup (ipaddrtype address)
{
    char namestring[MAX_DNS_PTR_LEN];
    char *cp;
    uchar *addr;
    nametype *ptr;
    int i;
    ipaddrtype server;
    
    cp = namestring;
    addr = (uchar *) &address;
    cp += numdotcat(addr[3],cp);
    cp += numdotcat(addr[2],cp);
    cp += numdotcat(addr[1],cp);
    cp += numdotcat(addr[0],cp);
    strcpy(cp, "in-addr.arpa");	/* add inverse domain name */
    for (i = 0; i < MAXDOMAINSERVERS; i++) {
	if (!(server = domain_server[i]))
	    return(NULL);
	ptr = domain_query(namestring,server,TYPE_PTR);
	if (ptr) {			/* found the host */
	    ptr->v.ip.adr[0].type = ADDR_IP;
	    ptr->v.ip.adr[0].length = ADDRLEN_IP;
	    ptr->v.ip.adr[0].addr.ip_address = address;
	    return(ptr);
	}
	if (psipending())	/* interrupted */
	    return(NULL);
				/* server did not respond; try next one */
    }
    return(NULL);
}

/*
 * domain_query
 * Query a domain server, return cache ptr or NULL
 */

static
nametype *domain_query (char *namestring, ipaddrtype server, 
			 short qtype)
{
    domainlisttype *dom;
    ipsocktype *soc;
    nametype *ptr = NULL;
    addrtype faddr;

    /*
     * Open UDP socket to the name server.
     */
    faddr.type = ADDR_IP;
    faddr.ip_addr = server;
    soc = open_ipsocket(UDP_PROT, NULL, &faddr, 0, DOMAIN_PORT, 
			SOCK_INTERR | SOCK_NETWORK_ERROR);
    if (!soc)
	return(FALSE);
    
    /*
     * If the name we are seeking is fully qualified, ask the server.
     *
     * If the name is not domained and we are using a single
     * 'domain-name', then ask the server.
     *
     * If the name is not domained, then iterate through all possible
     * names in the domain name list.  Check each one (in turn) to see if
     * it exists in the domain cache before attempting to query for it.
     * We do this just before each query to insure that we have already
     * queried using all of the previous domain names, and do not have an
     * earlier match does not exist.
     */
    if (QUEUEEMPTY(&domain_listQ) ||
	(qtype == TYPE_PTR) || (qtype == TYPE_NSAP_PTR)) {
	if (domained(namestring))
	    ptr = domain_query1(namestring, NULL, qtype, soc);
	else
	    ptr = domain_query1(namestring, default_domain, qtype, soc);
    } else {
	if (namestring[strlen(namestring)-1] == '.')
	    ptr = domain_query1(namestring, NULL, qtype, soc);
	else
	    for (dom = (domainlisttype *)domain_listQ.qhead;
		 !ptr && dom;
		 dom = dom->next)
		ptr = domain_query1(namestring, dom->name, qtype, soc);
    }
    reg_invoke_ip_close_socket(soc);
    return(ptr);
}
/*
 * domain_FQDN_query
 * Query a domain server, return cache ptr or NULL
 * used for FQDN's only
 */

static
nametype *domain_FQDN_query (char *namestring, ipaddrtype server)
{
    ipsocktype *soc;
    nametype *ptr = NULL;
    addrtype faddr;

    /*
     * Open UDP socket to the name server.
     */
    faddr.type = ADDR_IP;
    faddr.ip_addr = server;
    soc = open_ipsocket(UDP_PROT, NULL, &faddr, 0, DOMAIN_PORT, 
			SOCK_INTERR | SOCK_NETWORK_ERROR);
    if (!soc)
	return(FALSE);
    
    ptr = domain_query1(namestring, NULL, TYPE_A, soc);

    reg_invoke_ip_close_socket(soc);
    return(ptr);
}


/*
 * domain_query1
 * Helper routine for domain_query()
 */

static
nametype *domain_query1 (char *namestring, char *domainstring,
			  short qtype, ipsocktype *soc)
{
    paktype *pak, *requestpak;
    domaintype *name;
    uchar *cp;
    nametype *ptr, *newptr;
    nametype *expired_ptr = NULL;
    int bytes, tempbytes;
    uint offset;
    domaintype *namereply;
    udptype *udp;
    char tempname[MAXMATCHSIZE];
    ushort id;
    uchar tries;
    int i, namelen;
    char *nameptr;

    /*
     * Check cache for an entry for THIS name and domain pair.
     */
    if (domainstring) {
	if ((strlen(namestring) + strlen(domainstring) + 2) >= MAXMATCHSIZE)
	    return(NULL);
	sprintf(tempname, "%s.%s", namestring, domainstring);
    } else {
	if ((strlen(namestring)+1) >= MAXMATCHSIZE)
	    return(NULL);
	strcpy(tempname, namestring);
    }

    if (qtype == TYPE_A) {
	if ((ptr = name_cache_lookup(tempname, NULL, NAM_IP)))
	    /*
	     * Only return entry if not expired, else save in expired_ptr.
	     */
	    if ((ptr->flags & NAM_REVALIDATE) == 0) {
		return(ptr);
	    } else {
		expired_ptr = ptr;
	    }
    }
    else if (qtype == TYPE_NSAP) {
	if ((ptr = name_cache_lookup(tempname, NULL, NAM_CLNS)))
	    /*
	     * Only return entry if not expired, else save in expired_ptr.
	     */
	    if ((ptr->flags & NAM_REVALIDATE) == 0) {
		return(ptr);
	    } else {
		expired_ptr = ptr;
	    }
    }

    /*
     * Allocate storage for a datagram.  Assume maximum size.
     */
    pak = setup_ipsocket(soc,MAX_DNS_LEN,&offset);
    if (!pak) {
	return(NULL);
    }

    /*
     * Set up datagram fields.
     */
    if (domain_debug || aaa_authen_debug || aaa_author_debug
	|| aaa_acct_debug) {
	buginf("\nDomain: query for %s to %s ", tempname, 
		address_string(&soc->faddress));
    }
    name = (domaintype *)(pak->data_area + offset);
    memset(name, 0, sizeof(domaintype));	/* zero all header fields */
    name->id = id = random_gen() & 0xffff;	/* pick a random ID number */
    name->opcode = OPCODE_QUERY;		/* query name -> address */
    name->rd = 1;				/* recursion desired */
    cp = ((uchar *)name) + sizeof(domaintype);	/* byte ptr into q area */
    bytes = MAX_DNS_LEN - sizeof(domaintype) - 4; /* max bytes for name */
    name->qdcount = h2n(1);			/* just one question */
    tempbytes = comp_copy((char *)cp,tempname,bytes,NULL); /* write name */
    if (tempbytes == 0) {
	retbuffer(pak);
	return(NULL);
    }
    cp += tempbytes;
    PUTSHORT(cp,qtype);				/* qtype is host address */
    cp += 2;
    PUTSHORT(cp,CLASS_IN);			/* qclass is internet */
    cp += 2;
    bytes = cp - (uchar *)name;			/* datagram length */
#ifdef DEBUG
    printdomain(name, bytes);
#endif
    requestpak = pak_duplicate(pak);
    write_ipsocket(soc,pak,bytes);
    
    /*
     * Wait for a reply and interpret any results.
     */
    ptr = NULL;			/* assume no answer */

    tries = 3;
    while (TRUE) {		/* find response to THIS query */
  	if (psipending()) {
            /*
             * We are leaving the loop early, return the 
             * packet buffer if it is still around.
             */
             if (requestpak)      
                retbuffer(requestpak);
             return(NULL); 
        } 
	pak = read_ipsocket(soc,TRUE,3*ONESEC,&udp,&namereply);
	if (!pak) {		/* No reply received. */
	    if (!soc->error && requestpak && --tries) {
		if (tries) {
		    pak = pak_duplicate(requestpak);
		} else {
		    pak = requestpak;
		    requestpak = NULL;
		}
		if (pak) {
		    write_ipsocket(soc,pak,bytes);
		    continue;
		}
	    }
	    break;
	}
	if (namereply->id == id)
	    break;		/* The packet we want ! */
	else {
	    if (udp_debug)
		buginf("\nDomain: reply to wrong Id %x (wanted %x)",
		       namereply->id, id);
	    retbuffer(pak);
	}
    }  /* end while */

    if (pak) {
	bytes = udp->length - UDPHEADERBYTES;
	ptr = (nametype *) dom2cache(namereply, bytes);
	if (ptr && qtype != TYPE_PTR) {
	    nameptr = ptr->name;
            /*
             * namecount is a field of nametype to resolve CSCdi66910.
             * To not break the stability of 11.0 and 11.1,
             * the following intelligence is added.
             * The namecount field must always be greater than or
             * equal to one, if it is 0, it must be uninitialized,
             * so initialize it to one here".
             */
            if (ptr->namecount == 0)
                ptr->namecount = 1;

	    for (i = 0; i < ptr->namecount; i++) {
		if(!strcasecmp(nameptr, tempname))
		    break;
		nameptr += strlen(nameptr) + 1;
	    }
	    /*
	     * If queried name does not match, create a new cache entry.
	     * Copy over old cache entry and new queried name.
	     * Release old cache entry.
	     */
	    if (i == ptr->namecount) {
                namelen = nameptr - ptr->name;
		newptr = malloc_named(sizeof(nametype) + namelen +
				      strlen(tempname), "DNS Host");
		if (newptr) {
		    bcopy(ptr, newptr, (sizeof(nametype) + namelen - 1));
		    strcpy(newptr->name+namelen, tempname);
		    newptr->namecount++;
		    newptr->next = NULL;
		    newptr->sharecount = 0;
                    ptr->busystring = NULL;
                    ptr->loginstring = NULL;
                    ptr->resumestring = NULL;
                    /*
                     * Clear flags of old cache entry so that
                     * those still in used fields will not be
                     * freed by name_free()
                     */
                    ptr->flags = 0;
                    unqueue(&nameQ, ptr);
                    name_free(ptr);
		    enqueue(&nameQ, newptr);
		    ptr = newptr;
		}
	    }
	}
	if (domain_debug || aaa_authen_debug || aaa_author_debug
	    || aaa_acct_debug)
	    buginf("Reply received %s ", ptr ? "ok" : "empty");
	retbuffer(pak);
    } else {
	if ((domain_debug || aaa_authen_debug || aaa_author_debug 
		|| aaa_acct_debug)) {
	    if (soc->error) {
		buginf("%s unreachable error", unreach2string(soc->error));
	    } else {
		buginf("timed out");
	    }
	}
	soc->error = 0;
    }
    if (requestpak)
	retbuffer(requestpak);

    if (ptr != NULL)
	return(ptr);
    if (expired_ptr != NULL) { 	
 	if (domain_debug || aaa_authen_debug || aaa_author_debug
	    || aaa_acct_debug)
 	    buginf("\nDNS revalidation failed - returning expired entry."); 
 	return(expired_ptr);
    }
    return(ptr);
}

/*
 * dom2cache
 * Given a reply from a domain server, produce a cache entry
 */

#define HOSTSIZE 50

static
nametype *dom2cache (domaintype *reply, int replybytes)
{
    int n, i;
    uchar *cp;
    nametype *ptr;
    short clas, typ, flags;
    long ttl;
    short qdcount, ancount;
    uchar *replyend;
    char host[HOSTSIZE];
    boolean got_expired_entry = FALSE;
    addrtype addresses[MAXIPADDRESSES];

#if DEBUG
    printdomain(reply, replybytes);
#endif
    host[0] = '\0';			/* no host name yet */
    i = 0; 				/* no addresses yet */
    flags = 0;
    cp = ((uchar *) reply) + sizeof(domaintype);
    replyend = ((uchar *)reply) + replybytes;
    qdcount = n2h(reply->qdcount);
    ancount = n2h(reply->ancount);
    if ((reply->rcode != RCODE_OKAY) || (ancount == 0))
	return(NULL);
    while (--qdcount >= 0) {
	cp += comp_skip((char *)cp) + QUERYFIXEDBYTES;
   }
    while ((--ancount >= 0) && (cp < replyend) && (i < MAXIPADDRESSES)) {
	n = comp_expand((char *)reply, (char *)replyend, (char *)cp, host, HOSTSIZE);
	if (n < 0)
	    return(NULL);
	cp += n;			/* skip past name */
	typ = GETSHORT(cp);		/* get RR type */
	cp += sizeof(ushort);		/* skip type */
	clas = GETSHORT(cp);		/* get RR class */
	cp += sizeof(ushort);		/* skip class */
	bcopy(cp, (char *) &ttl, 4);	/* get ttl */
	ttl = n2hl(ttl);
	cp += sizeof(long);		/* skip time to live (ttl) */
	n = GETSHORT(cp);		/* get data length */
	cp += sizeof(ushort);		/* skip length field */
#if DEBUG
	buginf("\nDOM: hostname is %s, RR type=%d, class=%d, ttl=%ld, n=%d",
			 host,typ, clas, ttl, n);
#endif
	switch (typ) {
	    case TYPE_A:		/* host address */
		if (clas == CLASS_IN) {
		    flags |= NAM_IP;
		    addresses[i].type = ADDR_IP;
		    addresses[i].length = ADDRLEN_IP;
		    bcopy(cp, (char *)&addresses[i].ip_addr, ADDRLEN_IP);
		    /* for little endians */
		    addresses[i].ip_addr = n2hl(addresses[i].ip_addr);
		    i++;
#if DEBUG
		    buginf("\nDOM: address is %i", addresses[i-1].ip_addr);
#endif
		}
		
		cp += n;
		break;

	    case TYPE_NSAP:
		if (clas == CLASS_IN) {
		    flags |= NAM_CLNS;
		    addresses[i].type = ADDR_CLNS;
		    addresses[i].length = n + 1;
		    addresses[i].clns_addr[0] = n;
		    bcopy(cp, &addresses[i].clns_addr[1], n);
		    i++;
#if DEBUG
		    buginf("\nDOM: address is %n", addresses[i-1].clns_addr);
#endif
		}
		cp += n;
		break;

	    case TYPE_CNAME:		/* use canonical name always */
		n = comp_expand((char *)reply, (char *)replyend, (char *)cp, host, HOSTSIZE);
		if (n < 0)
		    return(NULL);
#if DEBUG
		buginf("\nDOM: canonical name is %s", host);
#endif
		cp += n;
		break;

	    case TYPE_PTR:		/* "domain" name */
		n = comp_expand((char *)reply, (char *)replyend, (char *)cp, host, HOSTSIZE);
		if (n < 0)
		    return(NULL);
#if DEBUG
		buginf("\nDOM: domain name is %s", host);
#endif
                flags |= NAM_IP;
		i = -1;			/* expect no addresses */
		cp += n;
		break;

	    default:
		cp += n;
		break;
	}
    }

    /*
     * If we got here, we presumably have a host entry.  Verify assumption.
     * Check if the canonical host name we got back is already in our cache.
     * This can happen when, for example, "EE" is a nickname for "SIERRA"
     * which in turn is a nickname for "SIERRA.STANFORD.EDU".
     */
    if ((i == 0) || (host[0] == '\0'))	/* make sure assumption is correct */
	return(NULL);

    ptr = (nametype *) name_cache_lookup(host, NULL, flags);
/*
 * If exact hit and not expired, return it.  otherwise keep looking.
 */
    if (ptr && ((ptr->flags & NAM_ANY) == flags))
	if ((ptr->flags & NAM_REVALIDATE) == 0) {
	    return(ptr);
	} else {
	    got_expired_entry = TRUE;
	}

    /*
     * Create a new cache entry, or update existing one.  If pointer not 
     * NULL, then update the expired entry.
     */
    if (ptr == NULL)
	ptr = malloc_named(sizeof(nametype) + strlen(host), "DNS Host");
    if (ptr == NULL)			/* get storage, quit if no-go */
	return(NULL);
    GET_TIMESTAMP(ptr->lasttime);	/* set last reference time */
    TIMER_START64(ptr->expiretime, ttl*ONESEC); /* set expiration time */
    ptr->flags = NAM_ADDRVALID | flags;
    strcpy(ptr->name, host);		/* copy canonical name */
    ptr->namecount = 1;
    if (i > 0) {
	for (n = 0; n < i; n++)	{	/* copy addresses, if any */
	    switch (addresses[n].type) {
	    case ADDR_IP:
		ptr->v.ip.adr[n].type    = addresses[n].type;
		ptr->v.ip.adr[n].length  = addresses[n].length;
		ptr->v.ip.adr[n].ip_addr = addresses[n].ip_addr;
		break;
	    case ADDR_CLNS:
		ptr->v.clns.adr[n].type   = addresses[n].type;
		ptr->v.clns.adr[n].length = addresses[n].length;
		bcopy(addresses[n].clns_addr, ptr->v.clns.adr[n].clns_addr,
		      addresses[n].length);
		break;
	    }
	}
    }
    if (!got_expired_entry)               /* If new entry, then */
	requeue(&nameQ, ptr);		 /* add it to the cache */
    return(ptr);			/* return pointer to the entry */
}

/*
 * comp_expand
 * Expand compressed domain name 'comp_dn' to full domain name.
 * 'msg' is a pointer to the begining of the message,
 * 'eomorig' points to the first location after the message,
 * 'exp_dn' is a pointer to a buffer of size 'length' for the result.
 * Return size of compressed name or -1 if there was an error.
 */

static
int comp_expand (char *msg, char *eomorig, char *comp_dn,
		 char *exp_dn, int size)
{
    char *cp, *dn;
    int n, c;
    char *eom;
    int len = -1;

    dn = exp_dn;
    cp = comp_dn;
    eom = exp_dn + size - 1;
    /*
     * fetch next label in domain name
     */
    while ((n = *cp++)) {
	/*
	 * Check for indirection
	 */
	switch (n & INDIRECT_MASK) {
	    case 0:
		if (dn != exp_dn) {
		    if (dn >= eom)
			return (-1);
		    *dn++ = '.';
		}
		if (dn+n >= eom)
		    return (-1);
		while (--n >= 0) {
		    c = *cp++;
		    if (c == '.') {
			if (dn+n+1 >= eom)
			    return (-1);
			*dn++ = '\\';
		    }
		    *dn++ = c;
		    if (cp >= eomorig)	/* out of range */
			return(-1);
		}
		break;

	    case INDIRECT_MASK:
		if (len < 0)
		    len = cp - comp_dn + 1;
		cp = msg + (((n & 0x3f) << 8) | (*cp & 0xff));
		if (cp < msg || cp >= eomorig)	/* out of range */
		    return(-1);
		break;

	    default:
		return (-1);			/* flag error */
	}
    }
    *dn = '\0';
    if (len < 0)
	len = cp - comp_dn;
    return (len);
}

/*
 * comp_skip
 * Skip over a compressed domain name.
 * Returns the byte count or -1.
 */

static
int comp_skip (char *name)
{
    char *cp;
    int n;

    cp = name;
    while ((n = *cp++)) {		/* get count */
	switch (n & INDIRECT_MASK) {
	    case 0:			/* no indirection */
		cp += n;
		continue;
	    default:			/* illegal type */
		return (-1);
	    case INDIRECT_MASK:		/* indirection */
		cp++;
	}
	break;
    }
   return (cp - name);
}


#if DEBUG
/*
 * printdomain
 * Given a domaintype pointer, print out a reply packet header
 */

static
void printdomain (domaintype *ptr, int len)
{
    char *cp, name[50];
    int i;
    long ttl;

    buginf("\nDOM: id=%d, %s, opcode=%d, aa=%d, tc=%d, rd=%d, ra=%d",
	ptr->id, ptr->qr ? "response" : "query", ptr->opcode, ptr->aa,
	ptr->tc, ptr->rd, ptr->ra);
    buginf("\n     rcode=%d, qdcount=%d, ancount=%d, nscount=%d, arcount=%d",
      ptr->rcode, ptr->qdcount, ptr->ancount, ptr->nscount, ptr->arcount);
    cp = (char *)ptr + sizeof(domaintype);
    for (i=0; i<ptr->qdcount; i++) {
	cp += comp_expand((char *)ptr, (char *)ptr+len, cp, &name, 50);
	buginf("\n     query name is %s, qtype=%d, class=%d",
	       name, GETSHORT(cp), GETSHORT(cp+2));
	cp += 4;
    }
    for (i=0; i<ptr->ancount; i++) {
	cp += comp_expand((char *)ptr, (char *)ptr+len, cp, &name, 50);
	bcopy(cp+4, (char *) &ttl, 4);	/* get ttl */
	ttl = n2hl(ttl);
	buginf("\n     answer name is %s, RR type=%d, class=%d, ttl=%ld",
	       name, GETSHORT(cp), GETSHORT(cp+2), ttl);
	cp += 10 + GETSHORT(cp+8);
    }
    for (i=0; i<ptr->nscount; i++) {
	cp += comp_expand((char *)ptr, (char *)ptr+len, cp, &name, 50);
	bcopy(cp+4, (char *) &ttl, 4);	/* get ttl */
	ttl = n2hl(ttl);
	buginf("\n     nameserver for %s, RR type=%d, class=%d, ttl=%ld",
	       strlen(name)?name:"(root)", GETSHORT(cp), GETSHORT(cp+2), ttl);
	cp += 10 + GETSHORT(cp+8);
    }
    for (i=0; i<ptr->arcount; i++) {
	cp += comp_expand((char *)ptr, (char *)ptr+len, cp, &name, 50);
	bcopy(cp+4, (char *) &ttl, 4);	/* get ttl */
	ttl = n2hl(ttl);
	buginf("\n     addl info is %s, RR type=%d, class=%d, ttl=%ld",
	       name, GETSHORT(cp), GETSHORT(cp+2), ttl);
	cp += 10 + GETSHORT(cp+8);
    }
}
#endif


/*
 * numdotcat
 * Given a decimal number, concatenate it and a dot to a string
 * Return character count.
 */

int numdotcat (uint n, char *cp)
{
    char digits[20];
    char *q;
    int count = 0;

    if (n == 0) {
	*cp++ = '0';
	count++;
    }
    else {
	for (q = &digits[0]; n != 0; n = n / 10) {
	    *q++ = (n % 10) + '0';
	    count++;
	}
	q--;
	while (q >= &digits[0])
	    *cp++ = *q--;
    }
    *cp++ = '.'; count++;
    return(count);
}

/*
 * domainlist_command
 * Set our list of domain name to append to a hostname when trying to
 * resolve that hostname.
 */

void domainlist_command (parseinfo *csb)
{
    domainlisttype *ptr;

    if (csb->nvgen) {
	for (ptr=(domainlisttype *)domain_listQ.qhead; ptr; ptr=ptr->next)
	    nv_write(TRUE, "%s %s", csb->nv_command, ptr->name);
	return;
    }
    if (GETOBJ(int,1) == FALSE) {	/* No name was given */
	/*
	 * Check to see if it's "no domain-list".
	 */
	if (!csb->sense)
	    while ((ptr = dequeue(&domain_listQ))) {
		free(ptr);
	    }
	return;
    }

    /*
     * See if this domain name already exists
     */
    for (ptr=(domainlisttype *)domain_listQ.qhead; ptr; ptr=ptr->next) {
	if (strcmp(GETOBJ(string,1), ptr->name) == 0)
	    break;
    }

    /*
     * Delete it
     */
    if (!csb->sense) {
	if (ptr) {
	    unqueue(&domain_listQ, ptr);
	    free(ptr);
	}
	return;
    }

    /*
     * If not already present, add the name to the end of the list.
     */
    if (ptr)
	return;
    ptr = malloc(sizeof(domainlisttype) + strlen(GETOBJ(string,1)));
    if (!ptr) {
	printf(nomemory);
	return;
    }
    strcpy(ptr->name, GETOBJ(string,1));
    enqueue(&domain_listQ, ptr);
}

/*
 * domain_FQDN_lookup
 * Use domain server to translate a name into an address
 * Returns cache pointer or NULL
 * used for FQDN's only...
 */

nametype *domain_FQDN_lookup (char *namestring, int kind)
{
    int i;
    ipaddrtype server;
    nametype *ptr;

    for (i = 0; i < MAXDOMAINSERVERS; i++) {
	if (!(server = domain_server[i]))
	    return(NULL);

	if (kind & NAM_IP) {
	    ptr = domain_FQDN_query(namestring,server);
	    if (ptr)			/* found the host */
		return(ptr);
	}

	if (psipending())	/* interrupted */
	    return(NULL);
				/* no reply from server; try next one */
    }
    return(NULL);
}

/*
 * domain_validate_addr
 *       use the address that we have from the tcp connection and
 *       ask the name cache/dns for a router name.  then clear that
 *       entry from the name cache and force a dns query for a address
 *       for the name.  now see if the ip address that we originally
 *       have is valid for that string.
 *
 *       this was not named rcmd_validate_addr() because someday,
 *       someone might need this functionality and move it somewhere...
 */

boolean domain_validate_addr (addrtype *addr)
{
    nametype *ptr, *rtn_ptr;
    ipaddrtype server;
    int i, name_kind;

    if (!domain_enable)
	return(TRUE);

    server = 0;

    /*
     * make sure domain servers exist.  if not, just let them in.
     */
    for (i = 0; i < MAXDOMAINSERVERS; i++) {
	server = domain_server[i];
	if (server)
	    break;
    }

    if (!server)
	return(FALSE);

    rtn_ptr = NULL;
    i = name_kind = 0;

    /*
     * Go do a name lookup.  We may use the cache or we may use
     * a name service of some sort (such as DNS)
     */
    ptr = (nametype *) reg_invoke_name_lookup_number(addr->type, addr);

    if (ptr) {
	char host[HOSTSIZE];

	strcpy(host, ptr->name);

	/*
	 * currently, it only looks for ip and clns addrs.  what a
	 * grody hack...
	 */
	if (addr->type == ADDR_IP)
	    name_kind = NAM_IP;
	else
	    return(FALSE);

	/*
	 * flush the name cache of this entry so a dns name lookup is forced
	 */
	if (!(ptr->flags & NAM_PERMANENT))
	    name_delete(host, name_kind);

	/*
	 * now look up the string via the name cache or name service.
	 * it should give us the same ptr to the name cache...
	 */
	if (reg_invoke_FQDN_lookup_string(host, name_kind,
					  &rtn_ptr)) {
	    if (name_kind == NAM_IP) {
		for (i = 0; i < MAXIPADDRESSES; i++) {
		    if (addr->ip_addr == rtn_ptr->v.ip.adr[i].ip_addr)
			return(TRUE);
		}
	    }
	}
    }

    /* we're sunk... */
    return(FALSE);
}

/*
 * bootp_ns_addr_dns_domain - fills in the system's DNS ip address
 * This is the backup method of providing this information for RFC1877
 * clients.  They should get their primary information from the 
 * async-bootp configuration.
 */
static	
boolean
bootp_ns_addr_dns_domain (boolean primary, ipaddrtype *addr)
{
    if (primary) {
	*addr = domain_server[0];
    } else {
	if (domain_server[1] != -1) {
	    *addr = domain_server[1];
	} else {
	    /*
	     * Return primary DNS as secondary bootp info
	     */
	    *addr = domain_server[0];
	}
    }

    /*
     * -1 means that it wasn't set.
     * For some reason there isn't a #define in this
     * file to indicate it.
     */
    if (*addr == -1) {
	*addr = 0;
	return FALSE;
    }

    return TRUE;
}

/*
 * Initialize the registry for the exported function in this file
 * Must be called after the bootp_dns_addr_ip function for the
 * async-bootp server is called.
 */
static void	
domain_init_registry (void)
{
    reg_add_bootp_ns_addr_dns(bootp_ns_addr_dns_domain,
			      "bootp_ns_addr_dns_domain");
}

/*
 * Parse chain registration array for DNS
 */

#define ALTERNATE       NONE
#include "cfg_ip_domain.h"
LINK_POINT(domain_ip_commands, ALTERNATE);
#undef	ALTERNATE

static parser_extension_request domain_chain_init_table[] = {
    { PARSE_ADD_CFG_TOP_IP_CMD, &pname(domain_ip_commands) },
    { PARSE_LIST_END, NULL }
};

/*
 * domain_init
 * Initialize the domain support
 */

static void domain_init (subsystype *dummy)
{
    parser_add_command_list(domain_chain_init_table, "domain");
    domain_init_registry();
}

/*
 * DNS subsystem header
 */

#define DNS_MAJVERSION 1
#define DNS_MINVERSION 0
#define DNS_EDITVERSION  1

SUBSYS_HEADER(domain, DNS_MAJVERSION, DNS_MINVERSION, DNS_EDITVERSION,
	      domain_init, SUBSYS_CLASS_PROTOCOL,
	      "seq: iphost, bootp",
	      "req: iphost, ipname");
