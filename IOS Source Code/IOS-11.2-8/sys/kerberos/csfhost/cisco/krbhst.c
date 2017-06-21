/* $Id: krbhst.c,v 3.3.28.2 1996/06/28 23:20:19 hampton Exp $
 * $Source: /release/112/cvs/Xsys/kerberos/csfhost/cisco/krbhst.c,v $
 *------------------------------------------------------------------
 * krbhst.c - Defines host running KDC for a given realm
 *
 * September 1995, Che-lin Ho
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: krbhst.c,v $
 * Revision 3.3.28.2  1996/06/28  23:20:19  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.3.28.1  1996/04/19  15:12:17  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commmit
 *
 * Revision 3.1.2.4  1996/04/16  03:54:37  che
 * Branch: IosSec_branch
 * Code changes mandated by code review comments.
 *
 * Revision 3.1.2.3  1996/04/02  08:56:52  carrel
 * cvs fixes
 *
 * Revision 3.1.2.2  1996/03/05  04:46:54  che
 * Branch: IosSec_branch
 * Kerberos Phase 2 commit.
 *
 * Revision 3.1.2.1  1996/01/23  05:57:29  che
 * Branch: IosSec_branch
 * Sync with 11.1 mainline. (960122)
 *
 * Revision 3.3  1996/01/13  01:34:01  che
 * CSCdi46854:  Kerberos KDC ports should be definable for each KDC server
 *
 * Revision 3.2  1995/11/17  17:45:54  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:19:49  shaker
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
#include "kparse.h"

/*
 * All realm/host pairs known, and count
 */
static struct krbhst *krbhst = NULL;
static int nkrbhst = 0;

/*
 * peek_krbhost_entry()
 *	Index into list of realm->host mappings
 *
 * Used during nvgen.  We do not copy, so returned values may only be used
 * until another krbhost entry point is called, at which time we may
 * shuffle memory.
 */
csf_sts
peek_krbhost_entry (int index, char **realmp, char **hostp, int *port)
{
    if (index >= nkrbhst) {
	return(CSFC5_STS_EOF);
    }
    *realmp = krbhst[index].k_realm;
    *hostp = krbhst[index].k_host;
    *port = krbhst[index].k_port;
    return(CSF_STS_OK);
}

/*
 * add_krbhost_entry()
 *	Add a realm/host entry
 *
 * A copy of the given realm/host strings is made within this routine.
 */
csf_sts
add_krbhost_entry (char *realm, char *host, int port)
{
    int x = 0;
    struct krbhst *newkrbhst = NULL, *k = NULL;

    /*
     * If dup, just return successfully
     */
    for (x = 0; x < nkrbhst; ++x) {
	k = &krbhst[x];

	if (!strcmp(realm, k->k_realm) && !strcmp(host, k->k_host) 
	    && (k->k_port == port)) {
	    return(CSF_STS_OK);
	}
    }

    /*
     * Get a new entry
     */
    /*
     * Try to grow the array
     */
    newkrbhst = realloc(krbhst, (nkrbhst+1) * sizeof(struct krbhst));
    if (newkrbhst == NULL) {
	return(CSFC5_STS_ENOMEM);
    }
    krbhst = newkrbhst;

    /*
     * Try to duplicate the strings.  Note that on failure we leave the
     * krbhst array grown, but this is harmless.
     */
    k = &krbhst[nkrbhst];
    k->k_host = strdup(host);
    if (k->k_host == NULL) {
	return(CSFC5_STS_ENOMEM);
    }
    k->k_realm = strdup(realm);
    if (k->k_realm == NULL) {
	free(k->k_host);
	return(CSFC5_STS_ENOMEM);
    }

    k->k_port = port;

    /*
     * Success.  Bump count, and return success.
     */
    nkrbhst += 1;
    return(CSF_STS_OK);
}

/*
 * del_krbhost_entry()
 *	Remove a realm/host entry
 */
csf_sts
del_krbhost_entry (char *realm, char *host, int port)
{
    int x = 0, y = 0;

    /*
     * Look for the pair which matches this
     */
    for (x = 0; x < nkrbhst; ++x) {
	struct krbhst *k;

	k = &krbhst[x];
	if (!strcmp(realm, k->k_realm) && !strcmp(host, k->k_host) 
	    && (k->k_port == port)) {
	    break;
	}
    }

    /*
     * Not present, just return success
     */
    if (x >= nkrbhst) {
	return(CSF_STS_OK);
    }

    /*
     * Shuffle everybody down one.  Never mind trying to srink the array
     */
    for (y = x; y < (nkrbhst-1); ++y) {
	krbhst[y] = krbhst[y+1];
    }

    /*
     * One fewer, return success
     */
    nkrbhst -= 1;
    return(CSF_STS_OK);
}

/*
 * csfC5_get_krbhst()
 *	Return hosts for the named realm
 *
 * The return space is malloc()'ed, as are the string which the array
 * points to.
 */
csf_sts
csfC5_get_krbhst (const csfC5_data *realm, struct krbhst ***hostlist)
{
    csf_sts retval = 0;
    struct krbhst **rethlist = NULL, **rethtmp;
    int x = 0, hlindex = 0;

    /*
     * Has to be at least one, otherwise no realms have been configured
     * yet.
     */
    if (nkrbhst == 0) {
	return(CSFC5_STS_CONFIG_BADFORMAT);
    }

    /*
     * Walk list, adding matches to our return array
     */
    retval = CSF_STS_OK;
    for (x = 0; x < nkrbhst; ++x) {
	    struct krbhst *k;

	    /*
	     * See if realm matches
	     */
	    k = &krbhst[x];
	    if (csf_strncmp(k->k_realm, realm->dp, realm->dl)) {
		continue;               /* no match */
	    }

	    /*
	     * Reallocate with a slot for this new pointer.  Always
	     * leave room for the NULL we will put on the end
	     */
	    rethtmp = csf_realloc(rethlist, 
				  (hlindex+2) * sizeof(struct krbhst *));
	    if (rethtmp == NULL) {
		retval = CSFC5_STS_ENOMEM;
		break;
	    }
	    rethlist = rethtmp;

	    rethlist[hlindex] = malloc_named(sizeof(struct krbhst),
					"Kerberos:csfC5_get_krbhst()");
	    if (rethlist[hlindex] == NULL) {
		retval = CSFC5_STS_ENOMEM;
		break;
	    }

	    /*
	     * Put an entry in the new slot, update the count
	     */
	    rethlist[hlindex]->k_host = csf_strdup(krbhst[x].k_host);
	    if (rethlist[hlindex]->k_host == NULL) {
		    retval = CSFC5_STS_ENOMEM;
		    break;
	    }
	    rethlist[hlindex]->k_port = krbhst[x].k_port;
	    hlindex += 1;
    }

    /*
     * On error (nomem is the only possibility), clean up and
     * return this error with no accompanying data.
     */
    if (retval != CSF_STS_OK) {
	if (rethlist) {
	    for (x = 0; x < hlindex; ++x) {
		csf_free(rethlist[x]->k_host);
		csf_free(rethlist[x]);
	    }
	    free(rethlist);
	    rethlist = NULL;
	}
    /*
     * Otherwise if no hosts found, return unknown realm
     */
    } else if (hlindex == 0) {
	retval = CSFC5_STS_REALM_UNKNOWN;
	rethlist = NULL;
    /*
     * Otherwise put the trailing NULL.  Space for it has always
     * been kept as we realloc()'ed the list larger.
     */
    } else {
	rethlist[hlindex] = NULL;
    }

    *hostlist = rethlist;
    return(retval);
}

/*
 * csf_free_krbhst()
 *	Frees the storage taken by a host list returned by csfC5_get_krbhst
 */
csf_sts
csf_free_krbhst (char **hostlist)
{
    char **cp;

    if (hostlist) {
	for (cp = hostlist; *cp; cp++) {
	    csf_free(*cp);
	}
	csf_free(hostlist);
    }
    return(CSF_STS_OK);
}
