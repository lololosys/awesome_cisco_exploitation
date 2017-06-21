/* $Id: defrlm.c,v 3.2.60.1 1996/04/19 15:12:08 che Exp $
 * $Source: /release/112/cvs/Xsys/kerberos/csfhost/cisco/defrlm.c,v $
 *------------------------------------------------------------------
 * defrlm.c - Defines a Kerberos Realm
 *
 * September 1995, Che-lin Ho
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: defrlm.c,v $
 * Revision 3.2.60.1  1996/04/19  15:12:08  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commmit
 *
 * Revision 3.1.2.2  1996/04/02  08:56:41  carrel
 * cvs fixes
 *
 * Revision 3.1.2.1  1996/03/05  04:46:39  che
 * Branch: IosSec_branch
 * Kerberos Phase 2 commit.
 *
 * Revision 3.2  1995/11/17  17:45:44  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:19:41  shaker
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

static char *default_realm = NULL;

/*
 * csfC5_set_default_realm()
 *	Set default realm
 *
 * Frees any previous value.  May be passed NULL to clear any
 * local realm.
 */
csf_sts
csfC5_set_default_realm(const char *realm)
{
    char *cp = NULL;

    if (realm) {
	cp = strdup(realm);
	if (cp == NULL) {
	    return(CSFC5_STS_ENOMEM);
	}
    } else {
	cp = NULL;
    }
    if (default_realm) {
	free(default_realm);
    }
    default_realm = cp;
    return(CSF_STS_OK);
}

/*
 * csfC5_get_default_realm()
 *	Return default realm
 *
 * Retrieves the default realm to be used if no user-specified realm is
 * available.  [e.g. to interpret a user-typed principal name with the
 * realm omitted for convenience]
 *
 * Implementation:  the default realm is stored in RAM.  A strdup()'ed copy
 * of it is returned.
 */
csf_sts
csfC5default_HostRealm(char **lrealm)
{
    char *cp = NULL;

    /*
     * If none configured yet, return error
     */
    if (default_realm == NULL) {
	return(CSFC5_STS_CONFIG_BADFORMAT);
    }

    /*
     * Get copy
     */
    cp = strdup(default_realm);
    if (cp == NULL) {
	return(CSFC5_STS_ENOMEM);
    }

    /*
     * Hand it back as success
     */
    *lrealm = cp;
    return(CSF_STS_OK);
}
