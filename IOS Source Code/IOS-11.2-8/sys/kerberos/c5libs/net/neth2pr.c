/* $Id: neth2pr.c,v 3.2 1995/11/17 17:45:26 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/kerberos/c5libs/net/neth2pr.c,v $
 *------------------------------------------------------------------
 * neth2pr.c - Convert hostname and service name to principal
 *
 * September 1995, Che-lin Ho
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: neth2pr.c,v $
 * Revision 3.2  1995/11/17  17:45:26  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:19:25  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#define csfSRCID "%W% %G%"
/*.original sn2princ.c 3.5 6/5/94*/

/*
 * Copyright (c) 1994 CyberSAFE Corporation.
 * Copyright (c) 1991 by the Massachusetts Institute of Technology.
 * All Rights Reserved.
 */

/*
 * Convert a hostname and service name to a principal in the "standard"
 * form.
 */

/*TBD/JNK--this is most of what use to be base/sn2pr.c (sname_to_principal),
 * which was moved here because of network dependencies.  Some of this should
 * move back to the original module.  In particular, this module should perform
 * only the portion of the principal name construction dependent on the host(s);
 * our caller should do the rest.
 */

#define CSFC5_HDRI_NET
#include <csfc5/header.h>

csf_sts csfC5netHostTo_prnId (
	const char *hostname,
	const char *sname,
	csf_ui32 type,
	csfC5_prnId *ret_princ
){
	char **hrealms = NULL;
	char *realm = NULL;
	char *remote_host = NULL;
	int use_local_realm = FALSE;
	csf_sts rsts = CSF_STS_OK;
	char localname[CSF_CFG_NET_MAXHOSTNLEN];
	char fullname[CSF_CFG_NET_MAXHOSTNLEN];

	if ((type == CSFC5c_NT_UNKNOWN) || (type == CSFC5c_NT_SRV_HST)) {
		if (!hostname) {
			use_local_realm = TRUE;
			/* convenience hack:  if hostname is NULL, use ours */
			rsts = csfNETgetMyHostName( localname, CSF_CFG_NET_MAXHOSTNLEN );
			if (rsts) {
				goto clean_exit;
			}
			hostname = localname;
		}
		/* if sname is NULL, use host service name */
		if (!sname) {
			sname = CSFC5c_NAME_SRVHST;
		}
		/* copy the hostname into non-volatile storage */
		if (type == CSFC5c_NT_SRV_HST) {
			rsts = csfNETgetFullHostName
				( hostname, fullname, CSF_CFG_NET_MAXHOSTNLEN );
			if (rsts) {
				goto clean_exit;
			}
			remote_host = csfCstrdup( fullname );
		} 
		else {
			/* type == CSFC5c_NT_UNKNOWN */
			remote_host = csfCstrdup( hostname );
		}

		if (!remote_host) {
			rsts = CSF_STS_ENOMEM;
			goto clean_exit;
		}

		if (type == CSFC5c_NT_SRV_HST) {
			(void) csfCstrlower( remote_host );
		}

		if (use_local_realm) {
			rsts = csfC5default_HostRealm( &realm );
			if (rsts) {
				goto clean_exit;
			}
		} 
		else {
			rsts = csfC5get_HostRealmS( remote_host, &hrealms );
			if (rsts) {
				goto clean_exit;
			}
			if (!hrealms[0]) {
				rsts = CSFC5_STS_HOST_REALM_UNKNOWN;
				goto clean_exit;
			}
			realm = hrealms[0];
		}

		{
			csfC5_data realm0, sname0, remote_host0;

			rsts = csfC5dataTo_prnId(
				ret_princ,
				csfC5litStr_data( realm, &realm0 ),
				csfC5litStr_data( sname, &sname0 ),
				csfC5litStr_data( remote_host, &remote_host0 ),
				NULL
			);
			if (rsts) {
				goto clean_exit;
			}
		}

		ret_princ->name.prn_type = type;

	} 
	else {
		rsts = CSFC5_STS_SNAME_UNSUPP_NAMETYPE;
		goto clean_exit;
	}


 clean_exit:
	csfCfree( remote_host );
	csfC5freeC_HostRealmS( &hrealms );

	return rsts;
}
