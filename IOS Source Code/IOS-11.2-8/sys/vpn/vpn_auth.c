/* $Id: vpn_auth.c,v 1.1.4.6 1996/07/09 19:57:32 tkolar Exp $
 * $Source: /release/112/cvs/Xsys/vpn/vpn_auth.c,v $
 *------------------------------------------------------------------
 * vpn_auth.c     ---  Authentication code for VPN
 *
 * March 1996, Tim Kolar
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: vpn_auth.c,v $
 * Revision 1.1.4.6  1996/07/09  19:57:32  tkolar
 * CSCdi62202:  PAP broken on VPDN connections
 * Branch: California_branch
 * Make VPDN PAP work correctly with the new AAA no-GETPASS model.
 *
 * Revision 1.1.4.5  1996/06/24  23:10:42  tkolar
 * CSCdi61123:  L2F Tunnel authentication message more helpful
 * Branch: California_branch
 * Make an authentication failure show up under "debug vpdn error", and
 * change the key generation method to match the RFC.
 *
 * Revision 1.1.4.4  1996/05/22  18:11:45  snyder
 * CSCdi58423:  spellink errors
 * Branch: California_branch
 *
 * suceeded -> succeeded
 * non-existant -> non-existent
 *
 * Revision 1.1.4.3  1996/05/21  10:08:50  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 1.1.4.2  1996/05/01  02:06:37  perryl
 * CSCdi56162:  MultiChassis MLP and VPN AAA calls mismatched
 * Branch: California_branch
 * Add wrapper functions aaa_chap_rresponse() and aaa_chap_rchallenge(),
 * for L2F and MLPVT to do CHAP style authenication with the new AAA API.
 *
 * Revision 1.1.4.1  1996/04/27  06:34:26  syiu
 * Branch: California_branch
 * Commit Dial1_branch
 *
 * Revision 1.1.2.4  1996/04/25  23:56:47  syiu
 * Branch: Dial1_branch
 * Dial1_branch to IosSec_postmerge_960419
 *
 * Revision 1.1.2.3  1996/03/25  05:32:16  tkolar
 * Branch: Dial1_branch
 * Enable Tunnel authentication.
 *
 * Revision 1.1.2.2  1996/03/24  01:44:28  tkolar
 * Branch: Dial1_branch
 * Enable AAA forwarding of VPN sessions, and complete
 * authentication of MIDs.  Also update a few parts of the
 * protocol to match changes in the RFC.
 *
 * Revision 1.1.2.1  1996/03/21  23:51:49  tkolar
 * Branch: Dial1_branch
 * Add support for multiple incoming tunnels, fix a crash in the resend
 * code, and add the beginnings of true authentication.
 *
 * Revision 1.1  1996/03/21  18:12:07  tkolar
 * Initial revision
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include "login.h"
#include "../util/md5.h"
#include "../h/address.h"
#include "aaa.h"
#include "../servers/tacacs_plus.h"
#include "../servers/tacacs.h"
#include "logger.h"
#include "config.h"
#include "linktype.h"
#include "../parser/parser_actions.h"
#include "../vpn/vpn_debug_flags.h"
#include <ciscolib.h>

ulong vpn_make_tunnel_key(char *md5) 
{
    ulong response = 0;
    uchar array[4];

    array[0] =  md5[0] ^ md5[4] ^ md5[8] ^ md5[12];
    array[1] =  md5[1] ^ md5[5] ^ md5[9] ^ md5[13];
    array[2] =  md5[2] ^ md5[6] ^ md5[10] ^ md5[14];
    array[3] =  md5[3] ^ md5[7] ^ md5[11] ^ md5[15];

    bcopy(array, &response, sizeof(ulong));
    return(response);
}

boolean vpn_get_md5_digest(char *user, uchar id, 
			uchar *challenge, uint challenge_len, uchar *digest) 
{
    userstruct *ustruct;
    authenstruct astruct;
    boolean ret;

    if (!digest)
	return(FALSE);

    /*
     * Obtain the response for the challenge thru AAA
     */
    ret = aaa_chap_rchallenge (&ustruct, &astruct, user, NULL,
				 id, challenge, challenge_len, "default");

    /*
     * Yes, we have a response, copy it to digest
     */
    if (ret) {
	bcopy(astruct.server_data, digest, MD5_LEN);
    }
    else {
	if (vpn_event_debug) {
	    buginf("\nL2F:  VPN authentication failed, couldn't find user "
			"information for %s.", user);
	}
    }
    aaa_cleanup_login(ustruct, &astruct);
    aaa_free_user(&ustruct);
    return (ret);
}

boolean vpn_chapstyle_auth(char *user, uchar *wesent, int wesentsize,
			uchar *theysent, uchar chap_id)
{
    userstruct *ustruct;
    authenstruct astruct;
    boolean ret;
    
    /* 
     * Check the response thru AAA
     */
    ret = aaa_chap_rresponse (&ustruct, &astruct, user, NULL, 
			      theysent,
			      chap_id, wesent, wesentsize,
			      "default");

    if (vpn_event_debug) {
	if (!ret)
	    buginf("\nL2F:  Chap authentication failed for %s.", user);
	else
	    buginf("\nL2F:  Chap authentication succeeded for %s.", user);
    }

    aaa_cleanup_login(ustruct, &astruct);
    aaa_free_user(&ustruct);
    return (ret);
}


boolean vpn_papstyle_auth(char *user, char *peersecret)
{
    userstruct *ustruct;
    authenstruct astruct;
    boolean ret;
    char *rem = NULL;

    bzero(&astruct, sizeof(astruct));
    ret = aaa_create_user(&ustruct, user, NULL, NULL, rem,
			  AAA_AUTHEN_TYPE_PAP, AAA_SVC_PPP, PRIV_USER);

    astruct.user_data = strdup(peersecret);
    astruct.user_data_len = strlen(peersecret);

    if (ret)
	ret = aaa_start_login(ustruct, &astruct, "default",
			      AAA_AUTHEN_ACT_LOGIN, AAA_SVC_PPP);

    if (!ret || (astruct.status != AAA_AUTHEN_STATUS_PASS)) {
	if (vpn_error_debug) {
	    buginf("\nL2F: Unable to validate PAP response.  "
		   "USERNAME %s: lookup failure.", user);
	}
	aaa_cleanup_login(ustruct, &astruct);
	aaa_free_user(&ustruct);
	return(FALSE);
    }

    aaa_cleanup_login(ustruct, &astruct);
    return(TRUE);
}
