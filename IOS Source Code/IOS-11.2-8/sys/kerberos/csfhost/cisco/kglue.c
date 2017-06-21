/* $Id: kglue.c,v 3.3.60.1 1996/04/19 15:12:14 che Exp $
 * $Source: /release/112/cvs/Xsys/kerberos/csfhost/cisco/kglue.c,v $
 *------------------------------------------------------------------
 * kglue.c - Glue between CyberSAFE Kerberos and Cisco router environment
 *
 * September 1995, Che-lin Ho
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: kglue.c,v $
 * Revision 3.3.60.1  1996/04/19  15:12:14  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commmit
 *
 * Revision 3.1.2.5  1996/04/16  03:54:31  che
 * Branch: IosSec_branch
 * Code changes mandated by code review comments.
 *
 * Revision 3.1.2.4  1996/04/09  20:51:20  che
 * Branch: IosSec_branch
 * Sync to California_branch (960405)
 *
 * Revision 3.1.2.3  1996/04/02  08:56:46  carrel
 * cvs fixes
 *
 * Revision 3.1.2.2  1996/03/05  04:46:48  che
 * Branch: IosSec_branch
 * Kerberos Phase 2 commit.
 *
 * Revision 3.1.2.1  1995/12/17  01:49:41  che
 * Branch: IosSec_branch
 * Sync to Arkansas
 *
 * Revision 3.3  1995/11/17  17:45:50  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:50:23  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:19:46  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * This file has routines which do relatively simple mappings of Cisco's
 * code to functions the Kerberos subsystem needs.
 */
#define CSF_HDRI_NET
#include <csfc5/header.h>
#include "interface.h"
#include "../util/random.h"
#include <interface_private.h>

extern char *default_domain;			/* default domain name */

/*
 * csfC5_net_getmyhostname()
 *	Get our (unqualified) host name
 */
csf_sts
csfNETgetMyHostName(char *fname, csf_size_t fnsize)
{
    strncpy(fname, hostname, fnsize);
    return(CSF_STS_OK);
}

/*
 * csfC5_net_getfullhostname()
 *	Get fully qualified hostname
 */
csf_sts
csfNETgetFullHostName(const char *hname, char *fname, csf_size_t fnsize)
{
    int hostlen, domlen;
    char *domain = NULL;

    domain = strchr(hname, '.');
    if (domain != NULL) {
	if (*(domain+1) != '\0') {
	    /*
	     * hname is already a fully qualified hostname.
	     * Copy into fname and return. 
	     */

	    hostlen = strlen(hname);
	    memcpy(fname, hname, hostlen+1);
	    return(CSF_STS_OK);
	} else {
	    /*
	     * If we're in here, then hname is a hostname and a dot.
	     * No domain name present.  Get rid of dot and treat
	     * as normal case.
	     */

	     *(domain+1) = '\0';
	}
    }

    if (default_domain == NULL) {
	/* 
	 * We don't have a default domain.  Return what we were sent.
	 */

	hostlen = strlen(hname);
	memcpy(fname, hname, hostlen+1);
	return(CSF_STS_OK);
    }

    /*
     * Calculate length of host and domain names.  Make sure there's
     * room for them, plus a "." and terminating '\0'.
     */
    hostlen = strlen(hostname);
    domlen = strlen(default_domain);
    if ((hostlen + domlen + 2) > fnsize) {
	return(CSFC5_STS_BAD_HOSTNAME);
    }

    /*
     * Put in host, advance pointer
     */
    memcpy(fname, hostname, hostlen);
    fname += hostlen;

    /*
     * Put in dot
     */
    *fname++ = '.';

    /*
     * Put in domain.  Null terminate.  Return success.
     */
    memcpy(fname, default_domain, domlen);
    fname[domlen] = '\0';
    return(CSF_STS_OK);
}

/*
 * freeup()
 *	Free up an array of strings
 *
 * Frees the strings the array points to, and then the array itself.
 * Returns the appropriate Kerberos error code.
 */
static csf_sts
freeup(csfC5_netHAddr **addrs, int naddr)
{
    int x = 0;

    for (x = 0; x < naddr; ++x) {
	free(addrs[x]);
    }
    free(addrs);
    return(CSFC5_STS_ENOMEM);
}

/*
 * csfC5_net_localaddr()
 *	Return all of our IP addresses
 *
 * Severely complicated by the fact that there's umpteen little bits
 * to malloc(), and we need to clean it all up if one fails.
 */
csf_sts
csfC5netLocaladdr(csfC5_netHAddr ***addr)
{
    idbtype *idb;
    csfC5_netHAddr *h, **tmpaddrs = NULL, **tmphold;
    int naddr = 1;
    struct in_addr *inp = NULL;

    FOR_ALL_SWIDBS(idb) {
	/*
	 * Non-IP interface, skip
	 */
	if (!idb->ip_enabled || !idb->ip_address) {
	    continue;
	}

	/*
	 * Grow the array of pointers
	 */
	tmphold = realloc(tmpaddrs, (naddr+1) * sizeof(csfC5_netHAddr *));
	if (tmphold == NULL) {
	    return(freeup(tmpaddrs, naddr-1));
	}
	tmpaddrs = tmphold;

	/*
	 * Allocate space for the next entry in the array
	 */
	h = tmpaddrs[naddr-1] = malloc_named(sizeof(csfC5_netHAddr),
					     "Kerberos:csfC5netLocaladdr()");
	if (h == NULL) {
	    return(freeup(tmpaddrs, naddr-1));
	}

	/* This list of pointers needs to be null-terminated. */
	tmpaddrs[naddr] = NULL;
	naddr += 1;

	/*
	 * Allocate space for the IP address
	 */
	inp = malloc_named(sizeof(struct in_addr),
			   "Kerberos:csfC5netLocaladdr()");
	if (inp == NULL) {
	    free(h);
	    return(freeup(tmpaddrs, naddr-1));
	}

	/*
	 * Fill in the host addr slot with the IP address
	 */
	h->haddr_type = CSFC5c_HADDR_T_INET;
	h->haddr_data.dl = sizeof(struct in_addr);
	h->haddr_data.dp = (void *)inp;
	inp->s_addr = idb->ip_address;
    }

    /*
     * Put a pointer to the array back into the argument.  Return
     * success.
     */
    *addr = tmpaddrs;
    return(CSF_STS_OK);
}

/*
 * csf_fatal()
 *	Die horribly
 */
void
csf_fatal(char *msg)
{
    if (msg) {
	buginf("Kerberos fatal error: %s\n", msg);
    }
    crashdump(10);
}

/*
 * cisco_kerb_srand()
 *	Initialize random number generator
 */
void
cisco_kerb_srand(uint seed)
{
    extern uint textsum;
    ulong time;

    GET_TIMESTAMP32(time);
    random_init(seed ^ time, textsum ^ seed);
}

/*
 * cisco_kerb_rand()
 *	Return next random number
 */
uint
cisco_kerb_rand(void)
{
    return(random_gen());
}

/*
 * csfC5_read_password()
 *	Read password--should never be called, we read it for Kerberos
 */
csf_sts
csfC5_read_password(char *prompt, char *prompt2, char *return_pwd,
	csf_size_t *size_return)
{
    csf_fatal("csfC5_read_password: unsupported");
    return(CSFC5_STS_EOF);
}
