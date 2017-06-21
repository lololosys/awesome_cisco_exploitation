/* $Id: kauth.c,v 3.7.20.3 1996/06/28 23:20:15 hampton Exp $
 * $Source: /release/112/cvs/Xsys/kerberos/csfhost/cisco/kauth.c,v $
 *------------------------------------------------------------------
 * kauth.c - AAA authentication hooks for Kerberos
 *
 * September 1995, Che-lin Ho
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: kauth.c,v $
 * Revision 3.7.20.3  1996/06/28  23:20:15  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.7.20.2  1996/04/24  02:59:41  lol
 * CSCdi55359:  TACACS events debug output displays handle (0X0) ????
 * Branch: California_branch
 * Cosmetic changes to T+ output formats.
 *
 * Revision 3.7.20.1  1996/04/19  15:12:11  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commmit
 *
 * Revision 3.1.2.7  1996/04/16  03:54:27  che
 * Branch: IosSec_branch
 * Code changes mandated by code review comments.
 *
 * Revision 3.1.2.6  1996/03/12  03:12:29  che
 * Branch: IosSec_branch
 * Phase 2 Kerberos commit Part 2
 *
 * Revision 3.1.2.5  1996/03/05  04:46:43  che
 * Branch: IosSec_branch
 * Kerberos Phase 2 commit.
 *
 * Revision 3.1.2.4  1996/02/28  06:14:35  lol
 * Branch: IosSec_branch
 * TACACS+ processing.
 *
 * Revision 3.1.2.3  1996/02/21  03:37:59  che
 * Branch: IosSec_branch
 * Sync to 11.1 Mainline (02/20/96)
 *
 * Revision 3.1.2.2  1996/01/23  05:57:26  che
 * Branch: IosSec_branch
 * Sync with 11.1 mainline. (960122)
 *
 * Revision 3.1.2.1  1995/12/17  01:49:40  che
 * Branch: IosSec_branch
 * Sync to Arkansas
 *
 * Revision 3.7  1996/02/06  23:34:14  che
 * CSCdi48098:  Need to be able to configure pre-auth type for Kerberos
 * messages
 * Added "[no] kerberos preauth [none|encrypted-unix-timestamp]"
 * config command.
 *
 * Revision 3.6  1996/01/13  01:33:57  che
 * CSCdi46854:  Kerberos KDC ports should be definable for each KDC server
 *
 * Revision 3.5  1996/01/06  20:55:51  che
 * CSCdi46135:  Need more Kerberos debugging messages
 * Added more debugging messages to track Kerberos ticket requests.
 *
 * Revision 3.4  1995/12/24  09:35:19  che
 * CSCdi46019:  Store Kerberos credentials in Kerberos library cred cache
 * Use Kerberos library credentials cache structures and functions to
 * manipulate user credentials cache.
 *
 * Revision 3.3  1995/12/08  00:04:09  che
 * CSCdi45057:  kerberos_user_auth() returns wrong error code
 * Needed to return CSFC5_STS_ENOMEM if the malloc() fails.
 *
 * Revision 3.2  1995/11/17  17:45:46  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:19:43  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include <csfc5/header.h>

#include "../ui/debug.h"
#include "sys_registry.h"
#include "../os/os_debug_flags.h"
#include "kparse.h"
#include "address.h"
#include "aaa.h"
#include "../h/auth_registry.regh"

static void
kerberos_aaa_do_login(userstruct *ustruct, authenstruct *astruct, char *pass);

/*
 * kerberos_user_auth()
 * Authenticates current user via Kerberos
 */
static csf_sts
kerberos_user_auth (userstruct *ustruct, authenstruct *astruct,
		   csfC5_creds *my_creds, csfC5_prnId *prnid, char *pass)
{

    csfC5_creds	*req_creds = NULL;
    char *clientname = NULL;
    csfC5_tStamp lifetime = CSFC5_KINIT_DEFAULT_LIFE;
    csfC5_tktFlags kdcopts = CSFC5_KINIT_DEFAULT_OPTIONS;
    csf_sts rsts = CSFC5_STS_EINVAL;

    req_creds = malloc_named(sizeof(csfC5_creds),
					    "Kerberos:kerberos_user_auth()");
    if (req_creds == NULL) {
	rsts = CSFC5_STS_ENOMEM;
	goto cleanup;
    }

    csfC5clear_creds(req_creds);

    rsts = csfC5copy_prnId(prnid, &req_creds->client);
    if (rsts) {
	if (kerberos_debug) {
	    buginf("\nKerberos:\tauthentication failed when parsing name");
	}
	goto cleanup;
    }

    rsts = csfC5toStr_prnId(&req_creds->client, &clientname);
    if (rsts) {
	if (kerberos_debug) {
	    buginf("\nKerberos:\tauthentication failed when unparsing name");
	}
	goto cleanup;
    }

    rsts = csfC5tgsName_prnId(
	&(req_creds->client.realm),
	NULL,
	NULL,
	&(req_creds->server)
    );

    if (rsts) {
	if (kerberos_debug) {
	    buginf("\nKerberos:\tauthentication failed while "
		    "building server name");
	}
	goto cleanup;
    } 

    if ((rsts = csfC5netLocaladdr(&req_creds->haddrs))) {
	if (kerberos_debug) {
	    buginf("\nKerberos:\tauthentication failed while "
		    "getting my address");
	}
	goto cleanup;
    }

    req_creds->times.tstart = 0;
    req_creds->times.trenew = 0;

    if ((rsts = csfC5getNow_tStamp(&req_creds->times.tend))) {
	if (kerberos_debug) {
	    buginf("\nKerberos:\tauthentication failed while "
		    "getting time of day");
	}
	goto cleanup;
    }

    req_creds->times.tend += lifetime;

    if (kerberos_debug) {
	buginf("\nKerberos:\tRequesting TGT with expiration date of %ld",
		req_creds->times.tend);
    }

    req_creds->tktflags = kdcopts;
    req_creds->seskey.key_type = CSFC5c_CRS_KT_DES;

    csfC5clear_creds(my_creds);

   if (kerb_preauth_method == CSFC5c_PADATA_T_NONE) {
	if (kerberos_debug) {
	    buginf("\nKerberos:\tSending TGT request with no pre-authorization"
			" data.");
	}
    } else {
	if (kerberos_debug) {
	    buginf("\nKerberos:\tSending TGT request with encrypted unix "
			"timestamp preauth data.");
	}
    }
    rsts = csfC5getITktPwd(
	req_creds,
	kerb_preauth_method,
	CSFC5c_CRS_ET_DES_CBC_CRC,
	pass,
	my_creds,
	NULL
    );

    csfC5freeC_authzS(&my_creds->authz);

    if (!rsts) {
	if (kerberos_validate_TGT(my_creds)) {
	    if (kerberos_debug) {
		buginf("\nKerberos:\tReceived valid credential with endtime of "
			"%ld", my_creds->times.tend);
	    }
	} else {
	    rsts = -1;
	    csfC5free_creds(my_creds);
	    printf("Kerberos:\tFailed to validate TGT!\n");
	}
    } else {
	if (kerberos_debug) {
	    buginf("\nKerberos:\tReceived invalid credential.");
	}
    }

cleanup:
    csfCfree(clientname);
    csfC5free_creds(req_creds);
    csfCfree(req_creds);
    return(rsts);
}


/*
 * kerberos_start_login()
 *	AAA start_login for Kerberos
 */
void
kerberos_start_login (boolean *result, userstruct *ustruct, 
		      authenstruct *astruct, int action)
{

    *result = FALSE;            /* Default is failure */

    if (!reg_invoke_kerberos_is_running()) {
	astruct->status = AAA_AUTHEN_STATUS_ERROR;
	return;
    }

    switch (action) {
    case AAA_AUTHEN_ACT_CHPASS:
	/* Not Supported at this time. */
    case AAA_AUTHEN_ACT_SENDPASS:
	/* Deprecated */
    case AAA_AUTHEN_ACT_SENDAUTH:
	/* Not available for kerberos */
	astruct->status = AAA_AUTHEN_STATUS_ERROR;
	break;
    case AAA_AUTHEN_ACT_LOGIN:
	switch (ustruct->authen_type) {
	case AAA_AUTHEN_TYPE_NONE:
	case AAA_AUTHEN_TYPE_CHAP:
	case AAA_AUTHEN_TYPE_ARAP:
	    /* Kerberos Can't handle these. */
	    astruct->status = AAA_AUTHEN_STATUS_ERROR;
	    break;
	case AAA_AUTHEN_TYPE_PAP:
	    /* All the info we need should be in the start packet. */
	    if (ustruct->user &&
		astruct->user_data_len && astruct->user_data) {
		/* astruct->status is set in this function. */
		kerberos_aaa_do_login(ustruct, astruct, astruct->user_data);
	    } else {
		astruct->status = AAA_AUTHEN_STATUS_ERROR;
	    }
	    break;
	case AAA_AUTHEN_TYPE_ASCII:
	    if (!ustruct->user) {
		/* If we don't have a username, then request it. */
		astruct->status = AAA_AUTHEN_STATUS_GETUSER;
		setstring(&astruct->server_msg, username_prompt);
		if (!astruct->server_msg) {
		    astruct->status = AAA_AUTHEN_STATUS_ERROR;
		    return;
		}
	    } else {
		/* Already know user so get password */
		astruct->status = AAA_AUTHEN_STATUS_GETPASS;
		setstring(&astruct->server_msg, password_prompt);
		if (!astruct->server_msg) {
		    astruct->status = AAA_AUTHEN_STATUS_ERROR;
		    return;
		}
		astruct->flags = AAA_RESP_FLAG_NOECHO;
	    }
	    break;
	}
	break;
    }

    if (astruct->status != AAA_AUTHEN_STATUS_ERROR) {
	astruct->seq_num++;
	*result = TRUE;
    }
    return;
}


/*
 * kerberos_cont_login()
 *	AAA cont_login for Kerberos
 */
void
kerberos_cont_login (boolean *result, userstruct *ustruct, 
		     authenstruct *astruct)
{
    *result = FALSE;            /* Default is failure */

    free(astruct->server_msg);
    astruct->server_msg = NULL;	/* reset it */

    switch (astruct->action) {
    case AAA_AUTHEN_ACT_LOGIN:
	switch (astruct->status) {
	case AAA_AUTHEN_STATUS_GETUSER:
	    /* Already know user so get password */
	    astruct->status = AAA_AUTHEN_STATUS_GETPASS;
	    setstring(&astruct->server_msg, password_prompt);
	    if (!astruct->server_msg) {
		astruct->status = AAA_AUTHEN_STATUS_ERROR;
	    } else {
		astruct->flags = AAA_RESP_FLAG_NOECHO;
	    }
	    break;

	case AAA_AUTHEN_STATUS_GETPASS:
	    /* astruct->status is set in this function. */
	    kerberos_aaa_do_login(ustruct, astruct, astruct->user_reply);
	    break;
	default:
	    astruct->status = AAA_AUTHEN_STATUS_ERROR;
	    break;
	}
	break;
    case AAA_AUTHEN_ACT_SENDPASS:
    case AAA_AUTHEN_ACT_SENDAUTH:
    case AAA_AUTHEN_ACT_CHPASS:
    default:
	astruct->status = AAA_AUTHEN_STATUS_ERROR;
	break;
    }

    if (astruct->status != AAA_AUTHEN_STATUS_ERROR) {
	astruct->seq_num++;		/* once on way out */
	*result = TRUE;
    }
    return;
}


/*
 * kerberos_do_author():
 *	Kerberos authorization using Kerberos instances.
 *	    This routine will return the following attributes if successful:
 *		service=shell
 *		cmd=null
 *		priv_lvl=priv_lvl of instance
 */
void
kerberos_do_author (boolean *result, userstruct *ustruct, authorstruct *astruct)
{
    instance_map *local_map = krb_instance_map;
    char *username = NULL;
    char *instance = NULL;
    char *service = NULL;
    char *cmd = NULL;
    char priv_lvl[3];
    int avail = 0;

    extern const char author_failed_msg[];

    /* This is the default return status unless we re-set it */
    astruct->status = AAA_AUTHOR_STATUS_ERROR;
    *result = FALSE;

    /* Check if this is a valid user login. */
    if (ustruct) {
	if ((*result = aaa_get_user_p(ustruct, &username)) == FALSE) {
	    setstring(&astruct->server_msg, author_failed_msg);
	    return;
	}
    }
    
    /* 
     * Isolate the instance of this user.
     * If instance is NULL after this, the user logged in with no kerb instance.
     */
    while ((*(username - 1) != '/') && (*username != '\0')) {
	username++;
    }

    if (*username != '\0') {
	instance = strdup(username);
	username = instance;

	/* Get rid of realm name if it's there. */
	while ((*username != '@') && (*username != '\0'))
	    username++;

	if (*username == '@')
	    *username = '\0';
    }

    service = aaa_get_value(astruct->out_args, AAA_ATTR_service);
    cmd = aaa_get_value(astruct->out_args, AAA_ATTR_cmd);

    /*
     * If service=shell and cmd=NULL, then this is authorization for starting
     * a shell.  Set the following attributes.
     */
    if (STREQ(service, AAA_VAL_SVC_shell) && STREQ(cmd, AAA_VAL_null)) {

	/* 
	 * user is valid, service is valid and cmd is valid.
	 * we've succeeded.  Set outgoing service and cmd attributes
	 * and try to find a matching instance.  If no instance match,
	 * don't try to set priv_lvl.  We'll use the default.
	 */
	astruct->status = AAA_AUTHOR_STATUS_PASS_REPL;

	/* Set service and cmd attributes. */
	*result = aaa_add_AV(&astruct->in_args, &avail, AAA_ATTR_service,
			    AAA_VAL_SVC_shell, AAA_AV_MAND);
	if (*result == FALSE) {
	    setstring(&astruct->server_msg, author_failed_msg);
	    free(instance);
	    return;
	}

	/* Try to match instance. */
	while (local_map != NULL) {
	    if (!strncmp(instance, local_map->instance_name, 
		strlen(local_map->instance_name))) {
		sprintf(priv_lvl, "%d",local_map->priv_level);
		*result = aaa_add_AV(&astruct->in_args, &avail, 
				    AAA_ATTR_priv_lvl, priv_lvl, AAA_AV_MAND);
		if (*result == FALSE) {
		    setstring(&astruct->server_msg, author_failed_msg);
		    free(instance);
		    return;
		}
		break;
	    }
	    local_map = local_map->next;
	}
    } else {
	setstring(&astruct->server_msg, author_failed_msg);
    }
}


static void
kerberos_aaa_do_login (userstruct *ustruct, authenstruct *astruct, char *pass)
{
    csfC5_prnId  prnid;
    csf_sts rsts;

    csfC5_creds	 my_creds;

    csfC5clear_creds(&my_creds);
    csfC5clear_prnId(&prnid);

    rsts = csfC5strTo_prnId(ustruct->user, &prnid);
    if (rsts) {
	if (kerberos_debug) {
	    buginf("\nKerberos:\tauthentication failed when parsing name");
	}
	astruct->status = AAA_AUTHEN_STATUS_ERROR;
	setstring(&astruct->server_msg, auth_failed);
	csfCmemzero(pass, strlen(pass));
	csfC5free_prnId(&prnid);
	return;
    }

    rsts = kerberos_user_auth(ustruct, astruct, &my_creds, &prnid, pass);
    csfCmemzero(pass, strlen(pass));
    if (!rsts) {
	/* Save creds into user_struct here */

	free(ustruct->cred);

	ustruct->cred = (struct credentials *)
	    malloc_named(sizeof(struct credentials), 
			 "Kerberos:kerberos_aaa_do_login()");
	if (!ustruct->cred) {
	    astruct->status = AAA_AUTHEN_STATUS_ERROR;
	    setstring(&astruct->server_msg, auth_failed);
	    csfC5free_creds(&my_creds);
	    csfC5free_prnId(&prnid);
	    return;
	}

	/* Ticket start time is the time we were authenticated */
	my_creds.times.tstart = my_creds.times.tauth;

	ustruct->cred->ref_cnt = 1;
	ustruct->cred->free_funct = (void *) csfC5close_cc;
	ustruct->cred->cred_type = KERBEROS;
	rsts = csfC5resolve_cc((csfC5_cc **) &(ustruct->cred->cred), "MEM:");
	if (rsts) {
	    if (kerberos_debug) {
		buginf("\nKerberos:\tauthentication failed while "
		       "allocating credentials cache");
	    }
	    astruct->status = AAA_AUTHEN_STATUS_ERROR;
	    setstring(&astruct->server_msg, auth_failed);
	    csfC5free_creds(&my_creds);
	    csfC5free_prnId(&prnid);
	    return;
	}

	rsts = csfC5copy_prnId(&prnid, &((csfC5_cc *)
					 ustruct->cred->cred)->prnid);
	if (rsts) {
	    if (kerberos_debug) {
		buginf("\nKerberos:\tauthentication failed while "
		       "copying principal ID");
	    }
	    astruct->status = AAA_AUTHEN_STATUS_ERROR;
	    setstring(&astruct->server_msg, auth_failed);
	    csfC5free_creds(&my_creds);
	    csfC5free_prnId(&prnid);
	    return;
	}

	rsts = csfC5initialize_cc(ustruct->cred->cred, &prnid);
	if (rsts) {
	    if (kerberos_debug) {
		buginf("\nKerberos:\tauthentication failed while "
		       "initializing credentials cache");
	    }
	    astruct->status = AAA_AUTHEN_STATUS_ERROR;
	    setstring(&astruct->server_msg, auth_failed);
	    csfC5free_creds(&my_creds);
	    csfC5free_prnId(&prnid);
	    return;
	}

	rsts = csfC5addCreds_cc(ustruct->cred->cred, &my_creds);
	if (rsts) {
	    if (kerberos_debug) {
		buginf("\nKerberos:\tauthentication failed while "
		       "adding credentials");
	    }
	    astruct->status = AAA_AUTHEN_STATUS_ERROR;
	    setstring(&astruct->server_msg, auth_failed);
	    csfC5free_creds(&my_creds);
	    csfC5free_prnId(&prnid);
	    return;
	}

	astruct->status = AAA_AUTHEN_STATUS_PASS;
	/* server_msg is already set to NULL */

    } else {
	if (rsts == CSFC5_STS_KDC_UNREACH) {
	    if (aaa_authen_debug || kerberos_debug) {
		buginf("\nAAA/AUTHEN (%u): KDC Unreachable",
		       astruct->session_id);
	    }
	    astruct->status = AAA_AUTHEN_STATUS_ERROR;
	} else {
	    if (aaa_authen_debug || kerberos_debug) {
		buginf("\nAAA/AUTHEN (%u): password incorrect",
		       astruct->session_id);
	    }
	    astruct->status = AAA_AUTHEN_STATUS_FAIL;
	    setstring(&astruct->server_msg, auth_failed);
	}
    }
    csfC5free_creds(&my_creds);
    csfC5free_prnId(&prnid);

    return;
}
