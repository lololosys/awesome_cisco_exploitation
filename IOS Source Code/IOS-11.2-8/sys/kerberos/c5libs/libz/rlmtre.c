/* $Id: rlmtre.c,v 3.2 1995/11/17 17:45:21 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/kerberos/c5libs/libz/rlmtre.c,v $
 *------------------------------------------------------------------
 * rlmtre.c - Get realm tree
 *
 * September 1995, Che-lin Ho
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: rlmtre.c,v $
 * Revision 3.2  1995/11/17  17:45:21  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:19:20  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#define csfSRCID "%W% %G%"
/*.original walk_rtree.c 3.1 12/29/93*/
/*.original free_rtree.c 3.1 12/29/93*/

/*
 * Copyright (C) 1993-1994 CyberSAFE Corporation.
 * Copyright 1990,1991 by the Massachusetts Institute of Technology.
 * All Rights Reserved.
 */

#include <csfc5/header.h>

#if 0 /*TBD/JNK--waiting inter-realm fix*/
csf_sts csfC5get_realmTree (
	const csfC5_data *client,
	const csfC5_data *server,
	csfC5_prnNCTree *tree,
	char realm_branch_char
){
	csf_sts rsts;
	csfC5_prnNCTree rettree;
	char *ccp, *scp;
	char *prevccp = 0, *prevscp = 0;
	char *com_sdot = 0, *com_cdot = 0;
	int i, links = 0;
	int clen, slen;
	csfC5_data tmpcrealm, tmpsrealm;
	int nocommon = TRUE;

	clen = client->dl;
	slen = server->dl;

	for (
		com_cdot = ccp = (char *) client->dp + clen - 1,
			com_sdot = scp = (char *) server->dp + slen - 1;
		clen && slen && *ccp == *scp;
		ccp--, scp--,  clen--, slen--
	) {
		if (*ccp == realm_branch_char) {
			com_cdot = ccp;
			com_sdot = scp;
			nocommon = FALSE;
		}
	}
	/* ccp, scp point to common root.
	 * com_cdot, com_sdot point to common components.
	 */

	/* handle case of one ran out */
	if (!clen) {
		/* construct path from client to server, down the tree */
		if (!slen )
			/* in the same realm--this means there is no ticket
			 * in this realm.
			 */
			return CSFC5_STS_NO_TKT_IN_RLM;
		if (*scp == realm_branch_char) {
			/* one is a subdomain of the other */
			com_cdot = client->dp;
			com_sdot = scp;
		} /* else normal case of two sharing parents */
	}
	if (!slen) {
		/* construct path from client to server, up the tree */
		if (*ccp == realm_branch_char) {
			/* one is a subdomain of the other */
			com_sdot = server->dp;
			com_cdot = ccp;
		} /* else normal case of two sharing parents */
	}
	/* determine #links to/from common ancestor */
	if (nocommon) {
		links = 1;
	}
	else {
		links = 2;
	}

	/* if no common ancestor, artificially set up common root at the last
	 * component, then join with special code.
	 */
	for (ccp = client->dp; ccp < com_cdot; ccp++) {
		if (*ccp == realm_branch_char) {
			links++;
			if (nocommon) {
				prevccp = ccp;
			}
		}
	}

	for (scp = server->dp; scp < com_sdot; scp++) {
		if (*scp == realm_branch_char) {
			links++;
			if (nocommon) {
				prevscp = scp;
			}
		}
	}

	if (nocommon) {
		if (prevccp) {
			com_cdot = prevccp;
		}
		if (prevscp) {
			com_sdot = prevscp;
		}
		if (com_cdot == (char *) client->dp + client->dl -1) {
			com_cdot = (char *) client->dp - 1 ;
		}
		if (com_sdot == (char *) server->dp + server->dl -1) {
			com_sdot = (char *) server->dp - 1 ;
		}
	}

	rettree.nrealm = links + 2;
	rettree.realms = (csfC5_prnId *)
		csfCcalloc( rettree.nrealm, sizeof(csfC5_prnId) );
	if (!rettree.realms) {
		return CSF_STS_ENOMEM;
	}
	i = 1;
	rsts = csfC5tgtname( client, client, &rettree.realms[0] );
	if (rsts) {
		csfC5free_realmTree( &rettree );
		return rsts;
	}
	for (
		prevccp = ccp = client->dp;
		ccp <= com_cdot;
		ccp++
	) {
		if (*ccp != realm_branch_char) {
			continue;
		}
		++ccp; /* advance past dot */
		tmpcrealm.dp = prevccp;
		tmpcrealm.dl = client->dl - (prevccp - (char *) client->dp);
		tmpsrealm.dp = ccp;
		tmpsrealm.dl = client->dl - (ccp - (char *) client->dp);
		rsts = csfC5tgtname( &tmpsrealm, &tmpcrealm, &rettree.realms[i] );
		if (rsts) {
			csfC5free_realmTree( &rettree );
			return rsts;
		}
		prevccp = ccp;
		i++;
	}
	if (nocommon) {
		tmpcrealm.dp = com_cdot + 1;
		tmpcrealm.dl = client->dl - (com_cdot + 1 - (char *) client->dp);
		tmpsrealm.dp = com_sdot + 1;
		tmpsrealm.dl = server->dl - (com_sdot + 1 - (char *) server->dp);
		rsts = csfC5tgtname( &tmpsrealm, &tmpcrealm, &rettree.realms[i] );
		if (rsts) {
			csfC5free_realmTree( &rettree );
			return rsts;
		}
		i++;
	}

	for (
		prevscp = com_sdot + 1, scp = com_sdot - 1;
		scp > (char *) server->dp;
		scp--
	) {
		if (*scp != realm_branch_char) {
			continue;
		}
		if (scp - 1 < (char *) server->dp) {
			break;                      /* XXX only if . starts realm? */
		}
		tmpcrealm.dp = prevscp;
		tmpcrealm.dl = server->dl -
			(prevscp - (char *) server->dp);
		tmpsrealm.dp = scp + 1;
		tmpsrealm.dl = server->dl -
			(scp + 1 - (char *) server->dp);
		rsts = csfC5tgtname( &tmpsrealm, &tmpcrealm, &rettree.realms[i] );
		if (rsts) {
			csfC5free_realmTree( &rettree );
			return rsts;
		}
		prevscp = scp + 1;
		i++;
	}
	if (slen && com_sdot >= (char *) server->dp) {
		/* only necessary if building down tree from ancestor or client
		 * however, we can get here if we have only one component
		 * in the server realm name, hence we make sure we found a component
		 * separator there...
		 */
		tmpcrealm.dp = prevscp;
		tmpcrealm.dl = server->dl -
			(prevscp - (char *) server->dp);
		rsts = csfC5tgtname( server, &tmpcrealm, &rettree.realms[i] );
		if (rsts) {
			csfC5free_realmTree( &rettree );
			return rsts;
		}
	}

	*tree = rettree;

	return CSF_STS_OK;
}


void csfC5free_realmTree (
	csfC5_prnNCTree *rtree
){

	if (rtree) {
		int i;

		for (i = 0; i < rtree->nrealm; i++) {
			csfC5free_prnId( &rtree->realms[i] );
		}
		csfCfree( rtree->realms );
		rtree->realms = NULL;
	}

}
#endif
