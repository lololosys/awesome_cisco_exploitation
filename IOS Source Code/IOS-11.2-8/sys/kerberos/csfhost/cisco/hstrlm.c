/* $Id: hstrlm.c,v 3.2.60.1 1996/04/19 15:12:10 che Exp $
 * $Source: /release/112/cvs/Xsys/kerberos/csfhost/cisco/hstrlm.c,v $
 *------------------------------------------------------------------
 * hstrlm.c - Handle mapping of [host|domain]->realm
 *
 * September 1995, Che-lin Ho
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: hstrlm.c,v $
 * Revision 3.2.60.1  1996/04/19  15:12:10  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commmit
 *
 * Revision 3.1.2.2  1996/04/02  08:56:43  carrel
 * cvs fixes
 *
 * Revision 3.1.2.1  1996/03/05  04:46:41  che
 * Branch: IosSec_branch
 * Kerberos Phase 2 commit.
 *
 * Revision 3.2  1995/11/17  17:45:45  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:19:42  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Copyright (C) 1994 CyberSAFE Corporation.
 * Copyright (C) 1993 Open Computing Security Group
 * Copyright 1990,1991 by the Massachusetts Institute of Technology.
 * All Rights Reserved.
 */
#include <csfc5/header.h>

/*
 * These map a host/domain to a realm
 */
struct hstrlm {
    char *h_host;
    char *h_realm;
};
static struct hstrlm *hstrlm = NULL;
static int nhstrlm = 0;

/*
 * peek_host_realm()
 *	Peek at the nth entry in the domain->realm mapping database
 */
csf_sts
peek_host_realm(int index, char **hostp, char **realmp)
{
    if (index >= nhstrlm) {
	return(CSFC5_STS_EOF);
    }
    *hostp = hstrlm[index].h_host;
    *realmp = hstrlm[index].h_realm;
    return(CSF_STS_OK);
}

/*
 * add_host_realm()
 *	Add a host/domain -> realm mapping
 *
 * domain_name should be of the form .XXX.YYY (e.g. .LCS.MIT.EDU)
 * host names should be in the usual form (e.g. FOO.BAR.BAZ)
 *
 * The strings are duplicated within this routine.
 */
csf_sts
add_host_realm(char *host, char *realm)
{
    struct hstrlm *h = NULL;
    int x = 0;

    /*
     * No-op duplicates
     */
    for (x = 0; x < nhstrlm; ++x) {
	h = &hstrlm[x];
	if (!strcmp(h->h_host, host) && !strcmp(h->h_realm, realm)) {
	    return(CSF_STS_OK);
	}
    }

    /*
     * Get room for the new element
     */
    h = realloc(hstrlm, (nhstrlm + 1) * sizeof(struct hstrlm));
    if (h == NULL) {
	return(CSFC5_STS_ENOMEM);
    }
    hstrlm = h;

    /*
     * Duplicate strings
     */
    h = &hstrlm[nhstrlm];
    h->h_host = strdup(host);
    if (h->h_host == NULL) {
	return(CSFC5_STS_ENOMEM);
    }
    h->h_realm = strdup(realm);
    if (h->h_realm == NULL) {
	free(h->h_host);
	return(CSFC5_STS_ENOMEM);
    }

    /*
     * Bump count, return success
     */
    nhstrlm += 1;
    return(CSF_STS_OK);
}

/*
 * del_host_realm()
 *	Remove a host/domain->realm mapping
 */
csf_sts
del_host_realm(char *host, char *realm)
{
    int x = 0, y = 0;

    /*
     * Look for this entry
     */
    for (x = 0; x < nhstrlm; ++x) {
	struct hstrlm *h;

	h = &hstrlm[x];
	if (!strcmp(h->h_host, host) && !strcmp(h->h_realm, realm)) {
	    break;
	}
    }

    /*
     * No match--just return
     */
    if (x >= nhstrlm) {
	return(CSF_STS_OK);
    }

    /*
     * Move everybody else down and decrement count
     */
    for (y = x; y < (nhstrlm - 1); ++y) {
	hstrlm[y] = hstrlm[y+1];
    }
    nhstrlm -= 1;
    return(CSF_STS_OK);
}

/*
 * csf_sts csfC5_get_host_realm()
 *	Map host to a realm
 *
 * Figures out the Kerberos realm names for host, filling in a
 * pointer to an argv[] style list of names, terminated with a null pointer.
 * If host is NULL, the local host's realms are determined.
 * If there are no known realms for the host, the filled-in pointer is set
 * to NULL.
 * The pointer array and strings pointed to are all in allocated storage,
 * and should be freed by the caller when finished.
 *
 * Implementation notes:
 *
 * this implementation only provides one realm per host

 * Given a fully-qualified domain-style primary host name,
 * return the name of the Kerberos realm for the host.
 * If the hostname contains no discernable domain, or an error occurs,
 * return the local realm name, as supplied by csfC5_get_default_realm().
 * If the hostname contains a domain, but no translation is found,
 * the local-realm is returned.
 */
csf_sts
csfC5get_HostRealmS (const char *host, char ***realmsp)
{
    char **retrealms, *domain = NULL;
    char local_host[CSFC5_CFG_NET_MAXHOSTNLEN+1];
    csf_sts retval = 0;
    int x = 0;

    /*
     * Initial space for retrealms
     */
    retrealms = csf_calloc(2, sizeof(char *));
    if (retrealms == NULL) {
	return(CSFC5_STS_ENOMEM);
    }

    /*
     * If no host supplied, use local one
     */
    if (!host) {
	retval = csfNETgetFullHostName(NULL,
	    local_host, sizeof(local_host)-1);
	if (retval != CSF_STS_OK) {
	    return(retval);
	}
	host = local_host;
    }

    /*
     * Point to domain part of hostname
     */
    domain = csf_strchr((char *)host, '.');

    /*
     * Prepare default realm name
     */
    if (domain) {
	retrealms[0] = strdup(domain+1);
	if (retrealms[0] == NULL) {
	    csf_free(retrealms);
	    return(CSFC5_STS_ENOMEM);
	}
    } else {
	retrealms[0] = strdup(host);
	if (retrealms[0] == NULL) {
	    csf_free(retrealms);
	    return(CSFC5_STS_ENOMEM);
	}
    }

    for (x = 0; x < nhstrlm; ++x) {
	struct hstrlm *h;

	h = &hstrlm[x];
	if (!csf_strcmpi(h->h_host, host)) {
	    /*
	     * Exact match of hostname, so return the realm
	     */
	    free(retrealms[0]);
	    retrealms[0] = strdup(h->h_realm);
	    if (retrealms[0] == NULL) {
		csf_free(retrealms);
		return(CSFC5_STS_ENOMEM);
	    }
	    break;
	}

	if ((h->h_host[0] == '.') && domain) {
	    /*
	     * This is a possible domain match
	     */
	    if (!csf_strcmpi(h->h_host, domain)) {
		/*
		 * Domain match, save to return if no exact host match found
		 */
		free(retrealms[0]);
		retrealms[0] = strdup(h->h_realm);
		if (retrealms[0] == NULL) {
		    csf_free(retrealms);
		    return(CSFC5_STS_ENOMEM);
		}
		continue;
	    }
	}
    }

    if (x == nhstrlm) {
	/*
	 * We did not match any in our domain-to-realms table.
	 * Return default realm.
	 */
	free(retrealms[0]);
	retval = csfC5default_HostRealm(&retrealms[0]);
	if (retval) {
	    csf_free(retrealms);
	    return(retval);
	}
    }

    *realmsp = retrealms;	/* set up the pointers for the return */

    return CSF_STS_OK;
}

/*
 * csf_free_host_realm()
 *	Frees a realm list returned by csfC5_get_local_realm
 */
void
csfC5freeC_HostRealmS (char ***realmlist)
{
    /* same format, so why duplicate code? */
    csf_free_krbhst(*realmlist);
}
