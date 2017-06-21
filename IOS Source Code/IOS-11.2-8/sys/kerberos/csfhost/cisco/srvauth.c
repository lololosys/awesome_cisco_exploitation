/* $Id: srvauth.c,v 3.1.10.7 1996/09/13 23:30:58 che Exp $
 * $Source: /release/112/cvs/Xsys/kerberos/csfhost/cisco/srvauth.c,v $
 *------------------------------------------------------------------
 * srvauth.c - Client and server authentication functions for 
 *	       Kerberos
 *
 * March 1996, Che-lin Ho
 *
 * Copyright (c) 1996-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: srvauth.c,v $
 * Revision 3.1.10.7  1996/09/13  23:30:58  che
 * CSCdi69159:  Minor Kerberos code fixes
 * Branch: California_branch
 * Free some unneeded memory and clear a Kerberos structure before use
 *
 * Revision 3.1.10.6  1996/07/17  00:42:28  che
 * CSCdi61914:  kerberos mandatory broken
 * Branch: California_branch
 * Force telnet client to fail and exit if "kerberos clients mandatory"
 * is configured and we can't successfully authenticate via Kerberized
 * Telnet.
 *
 * Revision 3.1.10.5  1996/06/28  23:20:21  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.1.10.4  1996/06/11  06:21:08  che
 * CSCdi58486:  router cant do kerberos mutual authentication with other
 * router
 * Branch: California_branch
 * Lots of minor fixes.
 *
 * Revision 3.1.10.3  1996/06/04  08:35:52  che
 * CSCdi57424:  router crashes attempting kerberos telnet
 * Branch: California_branch
 * Added kerberos_stackcheck() registry to ensure correct stacksize of
 * process running kerberos clients.
 *
 * Revision 3.1.10.2  1996/06/01  12:27:38  che
 * CSCdi54857:  krb5-instance problem with none auth list
 * Branch: California_branch
 * Add code to set ustruct->user to Kerberos principal and only
 * map instances if authenticated via Kerberos.
 *
 * Revision 3.1.10.1  1996/04/19  15:12:25  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commmit
 *
 * Revision 3.1.2.7  1996/04/16  03:54:40  che
 * Branch: IosSec_branch
 * Code changes mandated by code review comments.
 *
 * Revision 3.1.2.6  1996/03/28  08:44:43  che
 * Branch: IosSec_branch
 * Introduce function to release memory in conn->proto.tn.kerberos_info
 *
 * Revision 3.1.2.5  1996/03/27  23:31:48  che
 * Branch: IosSec_branch
 * More Kerberos authentication cleanup
 *
 * Revision 3.1.2.4  1996/03/26  04:08:01  che
 * Branch: IosSec_branch
 * Cosmetic changes to srvauth.c
 *
 * Revision 3.1.2.3  1996/03/25  20:55:36  che
 * Branch: IosSec_branch
 * Clean up how authentication negotiation works
 *
 * Revision 3.1.2.2  1996/03/12  03:12:32  che
 * Branch: IosSec_branch
 * Phase 2 Kerberos commit Part 2
 *
 * Revision 3.1.2.1  1996/03/05  04:47:00  che
 * Branch: IosSec_branch
 * Kerberos Phase 2 commit.
 *
 * Revision 3.1  1996/03/05  01:50:42  che
 * Placeholder file for new Kerberos sourcefiles.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#define CSF_HDRI_NET
#include <csfc5/header.h>

#include <name.h>
#include "master.h"
#include "sys_registry.h"
#include "aaa.h"
#include "../ui/debug.h"
#include "../os/os_debug_flags.h"
#include "ttysrv.h"
#include "../tcp/tcp.h"
#include "kparse.h"
#include "config.h"
#include "connect.h"
#include "../ip/ip.h"
#include "../tcp/telnet.h"
#include "../kerberos/include/csfc5/libz.h"

#define SUBOPT_PAD 3
#define MALLOC_CLIENT_NAME "Kerberos:kerberos_client_auth()"
#define MALLOC_SERVER_NAME "Kerberos:kerberos_server_auth()"
#define MALLOC_TELNET_NAME "Kerberos:kerberos_telnet_auth()"
#define MALLOC_VALIDATE_NAME "Kerberos:kerberos_validate_TGT()"

/*
 * kerberos_client_auth()
 *	Retrieves appropriate service credentials from TGS and attempts
 *	to Kerberos authenticate to the remote server.  If using rlogin
 *	or rsh, this function will also handle the mutual authentication
 *	and credentials forwarding stuff.
 */
boolean kerberos_client_auth (tcbtype *tcb, tt_soc *tty, addrtype *rhost,
			      long apreq_opts, char *luser, 
			      char *ruser, char *cmd, 
			      void *do_telnet, int auth_type)
{
    csfC5_credsReqCtx reqc;
    csfC5_creds *req_creds = NULL;
    csfC5_creds *ret_creds = NULL;
    csfC5_creds *tmp_creds = NULL;
    csf_sts rsts = CSFC5_STS_EINVAL;
    csfC5_cksBlock checksum;
    char *foreignhost = NULL, *checksumstring = NULL;
    csfC5_msgErr *error = NULL;
    boolean retval = FALSE;
    csfC5_data *outbuf = NULL;
    int len = 0;
    char *buf = NULL;
    int buflen = 0;
    long sec = 0, usec = 0;
    /*
     * do_telnet is a void pointer in the conn_telnet struct that points
     * to memory containing Kerberos-specific data.  We need to map the
     * pointer to the appropriate Kerberos structure here so we can access
     * this data.
     */
    kerberos_info *do_telnet1 = (kerberos_info *)do_telnet;

    ret_creds = malloc_named(sizeof(csfC5_creds), MALLOC_CLIENT_NAME);
    if (ret_creds == NULL) {
	goto krb5_client_cleanup;
    }

    req_creds = malloc_named(sizeof(csfC5_creds), MALLOC_CLIENT_NAME);
    if (req_creds == NULL) {
	goto krb5_client_cleanup;
    }

    error = malloc_named(sizeof(csfC5_msgErr), MALLOC_CLIENT_NAME);
    if (error == NULL) {
	goto krb5_client_cleanup;
    }

    outbuf = malloc_named(sizeof(csfC5_data), MALLOC_CLIENT_NAME);
    if (outbuf == NULL) {
	goto krb5_client_cleanup;
    }

    checksumstring =
	malloc_named(strlen(ruser)+strlen(cmd)+sizeof(long)
		     +sizeof(long), MALLOC_CLIENT_NAME);
    if (checksumstring == NULL) {
	goto krb5_client_cleanup;
    }


    /* Clear Kerberos structures */
    csfC5clear_credsReqCtx(&reqc);
    csfC5clear_creds(ret_creds);
    csfC5clear_creds(req_creds);
    csfC5clear_msgErr(error);
    csfC5clear_data(outbuf);
    csfC5clear_cksBlock(&checksum);

    /* Fill in server principal ID. */
    foreignhost = strdup(name_addr2string(rhost));
    if (foreignhost == NULL) {
	goto krb5_client_cleanup;
    }

    rsts = csfC5netHostTo_prnId(foreignhost, CSFC5c_NAME_SRVHST,
				  CSFC5c_NT_SRV_HST, &req_creds->server);
    if (rsts) {
	printf("Kerberos:\tkerberos_client_auth:  csfC5netHostTo_prnId() "
		"failed!\n");
	goto krb5_client_cleanup;
    }

    /* Check that our checksum type is supported */
    rsts = csfC5check_cksType(CSFC5c_CKS_T_CRC32);
    if (rsts) {
	printf("Kerberos:\tChecksum type not supported!\n");
	goto krb5_client_cleanup;
    }

    /* Compute checksum using CRC-32 */
    rsts = csfC5setSumType_cksBlock(CSFC5c_CKS_T_CRC32, &checksum);
    if (rsts) {
	goto krb5_client_cleanup;
    }
    
    if (do_telnet) {
	checksumstring[0] = KERBEROS_V5;
	checksumstring[1] = auth_type;
	checksumstring[2] = '\0';
    } else {
	sprintf(checksumstring, "%d:%s%s\0", tcb->foreignport, cmd, ruser);
    }
    rsts = csfC5cksCalculate(CSFC5c_CKS_T_CRC32, checksumstring,
			     strlen(checksumstring), 0, 0, &checksum);
    if (rsts) {
	printf("Kerberos:\tFailed to calculate checksum!\n");
	goto krb5_client_cleanup;
    }

    /* Fill in client principal ID. */
    rsts = csfC5strTo_prnId(tty->ustruct->user, &req_creds->client);
    if (rsts) {
        printf("Kerberos:\tkerberos_client_auth:\tcsfC5netHostTo_prnId() "
		"failed!\n");
        goto krb5_client_cleanup;
    }

    reqc.req = req_creds;
    rsts = csfC5get_creds((csf_flags) 0, &reqc, 
			    (csfC5_cc *) tty->ustruct->cred->cred, ret_creds);
    if (rsts) {
	printf("Kerberos:\tkerberos_client_auth:\tcsfC5get_creds() failed!\n");
	goto krb5_client_cleanup;
    }

    /* Notes for csfC5sendAppXAuth():
     *  We're not using sequence numbers.
     *  We're not using subsession keys.
     *  "rep_result" reports current local time, subsession keys, and
     *  sequence numbers.  We use none of these, so we're not interested.
     */
    rsts = csfC5sendAppXAuth(tcb, "KCMDV0.1", &(ret_creds->client), 
			     (csfC5_tktFlags) apreq_opts, &checksum, ret_creds,
			     NULL, NULL, error, NULL, (int) do_telnet, &buf, 
			     &buflen, &sec, &usec);
    if (rsts) {
	printf("Kerberos:\tkerberos_client_auth:\tCouldn't authenticate to "
		"server %s\n.", foreignhost);
	if (kerberos_debug) {
	    if (error->text.dl) {
		buginf("Kerberos:\tkerberos_client_auth:\tError text sent from "
			"server:  %s\n",error->text.dp);
	    }
	}
	goto krb5_client_cleanup;
    }

    if (do_telnet) {
	do_telnet1->telnet_buf = buf;
	do_telnet1->telnet_buflen = buflen;
	do_telnet1->autht_sec = sec;
	do_telnet1->autht_usec = usec;
	
	tmp_creds = malloc_named(sizeof(csfC5_creds), MALLOC_CLIENT_NAME);
	if (tmp_creds == NULL) {
	    retval = FALSE;
	    goto krb5_client_cleanup;
	}

	rsts = csfC5copy_creds(ret_creds, tmp_creds);
	if (rsts) {
	    retval = FALSE;
	} else {
	    csfC5free_creds(do_telnet1->creds);
	    free(do_telnet1->creds);
	    do_telnet1->creds = tmp_creds;
	    retval = TRUE;
	}
	goto krb5_client_cleanup;
    }

    len = tcp_putstring(tcb, luser, strlen(luser)+1);
    len = tcp_putstring(tcb, cmd, strlen(cmd)+1);
    len = tcp_putstring(tcb, ruser, strlen(ruser)+1);
    (void) tcp_putpush(tcb, TRUE, FALSE);       /* write data to net */

    /* Forward credentials if we need to */
    if (do_forwarding) {
	rsts = get_forwarding_creds(tcb, tty, foreignhost, ret_creds, outbuf);
	if (rsts) {
	    printf("Kerberos:\tFailed to get forwarding creds!\n");
	    goto krb5_client_cleanup;
	}
	
	rsts = csfC5netWriteMsg(tcb, outbuf);
	if (rsts) {
	    printf("Kerberos:\tkerberos_client_auth:\tKerberos failed to write "
		"to socket!\n");
	    goto krb5_client_cleanup;
	}
	
    } else {
	outbuf->dl = 0;
	rsts = csfC5netWriteMsg(tcb, outbuf);
	if (rsts) {
	    printf("Kerberos:\tkerberos_client_auth:\tKerberos failed to write "
		"to socket!\n");
	    goto krb5_client_cleanup;
	}
    }

    retval = TRUE;

krb5_client_cleanup:
    free(foreignhost);
    free(checksumstring);
    csfC5free_creds(req_creds);
    csfC5free_creds(ret_creds);
    csfC5free_msgErr(error);
    csfC5free_data(outbuf);
    csfC5free_cksBlock(&checksum);
    free(ret_creds);
    free(req_creds);
    free(error);
    free(outbuf);
    return(retval);
}


/*
 * kerberos_server_auth()
 *	Reads in Kerberos authentication data sent by remote client.
 *	If using rlogin or rsh, this function will also handle the mutual 
 *	authentication stuff.
 */
boolean kerberos_server_auth (tcbtype *tcb, char *authdata, int authdata_len, 
			      void *kerberos_info1)
{
    csfC5_prnId *server = NULL, *client = NULL;
    csfC5_tkt *tkt = NULL;
    csfC5_autht *autht = NULL;
    csfC5_netHAddr *s_haddr = NULL;
    struct in_addr *inp = NULL;
    boolean retval = FALSE;
    csf_sts rsts = CSFC5_STS_EINVAL;
    char *foreignhost = NULL, *clientstring = NULL;
    int counter = 0;
    /*
     * kerberos_info1 is a void pointer in the conn_telnet struct that points
     * to memory containing Kerberos-specific data.  We need to map the
     * pointer to the appropriate Kerberos structure here so we can access
     * this data.
     */
    kerberos_info *kerberos_data = (kerberos_info *) kerberos_info1;

    /* Allocate and initialize replay cache if it doesn't exist. */
    if (replay_cache == NULL) {
	rsts = csfC5resolve_rc(&replay_cache, "host");
	if (rsts) {
	    printf("Kerberos:\tCannot allocate replay cache!\n");
	    goto krb5_server_cleanup;
	}

	rsts = csfC5initialize_rc(replay_cache, 0);
	if (rsts) {
	    printf("Kerberos:\tCannot initialize replay cache!\n");
	    goto krb5_server_cleanup;
	}
    }

    server = malloc_named(sizeof(csfC5_prnId), MALLOC_SERVER_NAME);
    if (server == NULL) {
	goto krb5_server_cleanup;
    }

    client = malloc_named(sizeof(csfC5_prnId), MALLOC_SERVER_NAME);
    if (client == NULL) {
	goto krb5_server_cleanup;
    }

    tkt = malloc_named(sizeof(csfC5_tkt), MALLOC_SERVER_NAME);
    if (tkt == NULL) {
	goto krb5_server_cleanup;
    }

    autht = malloc_named(sizeof(csfC5_autht), MALLOC_SERVER_NAME);
    if (autht == NULL) {
	goto krb5_server_cleanup;
    }

    s_haddr = malloc_named(sizeof(csfC5_netHAddr), MALLOC_SERVER_NAME);
    if (s_haddr == NULL) {
	goto krb5_server_cleanup;
    }

    inp = malloc_named(sizeof(struct in_addr), MALLOC_SERVER_NAME);
    if (inp == NULL) {
	goto krb5_server_cleanup;
    }

    foreignhost = strdup(name_addr2string(&tcb->foreignhost));
    if (foreignhost == NULL) {
	goto krb5_server_cleanup;
    }

    /* Clear Kerberos structures */
    csfC5clear_prnId(server);
    csfC5clear_prnId(client);
    csfC5clear_tkt(tkt);
    csfC5clear_autht(autht);
    csfC5clear_netHAddr(s_haddr);

    /* Create a principal of us */
    rsts = csfC5netHostTo_prnId(NULL, CSFC5c_NAME_SRVHST,
				CSFC5c_NT_SRV_HST, server);

    /* Fill in net address of foreign host */
    s_haddr->haddr_type = CSFC5c_HADDR_T_INET;
    s_haddr->haddr_data.dl = sizeof(struct in_addr);
    s_haddr->haddr_data.dp = (void *)inp;
    inp->s_addr = tcb->foreignhost.addr.ip_address;

    /* Notes for csfC5recvAppXAuth():
     *  We're not using sequence numbers.
     *  We're using csfC5readSvcKey_msgApReq to get the SVCKEY (KEYPROC == NULL)
     *  We're storing the KEYTAB in memory (KEYPROCARGS == NULL);
     */
    rsts = csfC5recvAppXAuth(tcb, "KCMDV0.1", server, s_haddr, NULL, 
			     NULL, replay_cache, NULL, client, tkt, autht,
			     authdata, authdata_len, 
			     &(kerberos_data->telnet_buf),
			     &(kerberos_data->telnet_buflen));
    if (rsts) {
	printf("kerberos_server_auth:\tCouldn't authenticate client from %s.\n",
		foreignhost);
	if (rsts == CSFC5_STS_RC_REPLAY) {
	    printf("Kerberos:\treplay found and re-transmitted\n");
	}
	goto krb5_server_cleanup;
    }

    /* Save the client principal(without the realm) in utruct's user field */
    if (tcb && tcb->tty && tcb->tty->ustruct && tcb->tty->ustruct->user) {
	free(tcb->tty->ustruct->user);
	tcb->tty->ustruct->user = NULL;
	if (csfC5toStr_prnId(client, &clientstring) != CSF_STS_OK) {
	    free(clientstring);
	    clientstring = NULL;
	    printf("Cannot save your username!  This may cause you problems "
		    "later.\n");
	} else {
	    counter = 0;
	    while (clientstring[counter] != '\0') {
		if ((clientstring[counter] == '@')
		    && (clientstring[counter-1] != '\\')) {
		    clientstring[counter] = '\0';
		    break;
		}
		counter++;
	    }
	    tcb->tty->ustruct->user = clientstring;
	}
    }

    /* Save the session key.  We may need it later */
    if (kerberos_data) {
	if (kerberos_data->seskey) {
	    csfC5free_crsKBlock(kerberos_data->seskey);
	    free(kerberos_data->seskey);
	}

	kerberos_data->seskey = malloc_named(sizeof(csfC5_crsKBlock),
						     MALLOC_SERVER_NAME);
	if (kerberos_data->seskey == NULL) {
	    goto krb5_server_cleanup;
	}
        rsts = csfC5copy_crsKBlock(&(tkt->epart.seskey), 
				   (csfC5_crsKBlock *)kerberos_data->seskey);
        if (rsts) {
	    goto krb5_server_cleanup;
	}

	if (autht->subkey.key_data.dp) {
	    if (kerberos_data->subseskey) {
		csfC5free_crsKBlock(kerberos_data->subseskey);
		free(kerberos_data->subseskey);
	    }

	    kerberos_data->subseskey = malloc_named(sizeof(csfC5_crsKBlock),
							MALLOC_SERVER_NAME);
	    if (kerberos_data->subseskey == NULL) {
		goto krb5_server_cleanup;
	    }
	    rsts = csfC5copy_crsKBlock(&(autht->subkey), 
				   (csfC5_crsKBlock *)kerberos_data->subseskey);
	    if (rsts) {
		goto krb5_server_cleanup;
	    }
	} else {
	    kerberos_data->subseskey = NULL;
	}
    }

    retval = TRUE;
    
krb5_server_cleanup:
    free(foreignhost);
    csfC5free_prnId(server);
    csfC5free_prnId(client);
    csfC5free_tkt(tkt);
    csfC5free_autht(autht);
    csfC5free_netHAddr(s_haddr);
    free(server);
    free(client);
    free(tkt);
    free(autht);
    free(s_haddr);
    return(retval);
}


/*
 * kerberos_mutual_auth()
 *	Handle mutual authentication stuff for Telnet clients.
 */
boolean kerberos_mutual_auth (tt_soc *tty, addrtype *rhost, char *buf, 
			      int length, void *kerberos_info1)
{
    csfC5_msgApRep aprep;
    csfC5_data inbuf;
    csf_sts rsts = CSFC5_STS_EINVAL;
    kerberos_info *kerberos_data = (kerberos_info *)kerberos_info1;
    csfC5_creds *ret_creds = (csfC5_creds *) kerberos_data->creds;

    /* Clear Kerberos structures */
    csfC5clear_msgApRep(&aprep);
    csfC5clear_data(&inbuf);

    inbuf.dp = (void *) buf;
    inbuf.dl = length;
    rsts = csfC5rdEc_msgApRep(&inbuf,&(ret_creds->seskey),&aprep);

    if (rsts || (aprep.epart.ctime.secs != kerberos_data->autht_sec) || 
       (aprep.epart.ctime.usecs != kerberos_data->autht_usec)) {
	csfC5free_msgApRep(&aprep);
	return(FALSE);
    }
    csfC5free_msgApRep(&aprep);
    return(TRUE);
}


/*
 * kerberos_telnet_auth
 * Process RFC1416 Authentication suboptions for Kerberos 5
 */

void kerberos_telnet_auth (boolean *result, conntype *conn, int length, 
			   int auth_type)
{
    char *buf = NULL, *nbuf = NULL;
    char *foreignhost = NULL;
    int buflen = 0;
    long authopts = (long) 0;
    csfC5_data outbuf;
    char reject_reply[3];
    csf_sts rsts = CSFC5_STS_EINVAL;
    void *seskey = NULL;
    kerberos_info *kerberos_data = (kerberos_info *)conn->proto.tn.kerberos_info;

    extern boolean telnet_debug;

    *result = FALSE;

    csfC5clear_data(&outbuf);

    if (conn->proto.tn.kerberos_info == NULL) {
	kerberos_data = malloc_named(sizeof(kerberos_info),MALLOC_TELNET_NAME);
	if (kerberos_data == NULL) {
	    goto auth_krb5_failed;
	}
	kerberos_data->telnet_buf = NULL;
	kerberos_data->telnet_buflen = 0;
	kerberos_data->autht_sec = 0;
	kerberos_data->autht_usec = 0;
	kerberos_data->seskey = NULL;
	kerberos_data->subseskey = NULL;
	kerberos_data->creds = NULL;
	conn->proto.tn.kerberos_info = (void *)kerberos_data;
    }

    if (auth_type & AUTH_HOW_MUTUAL) {
	auth_type = AUTH_HOW_MUTUAL;
    } else {
	auth_type = 0;
    }

    switch(conn->proto.tn.subbuffer[1] & 0xff) {
	case TNQ_SEND:
	    /* We are the client. */
	    if (auth_type | AUTH_HOW_MUTUAL) {
		authopts = CSFC5c_APOPTS_MUTUAL_REQUIRED_m;
	    }
	    tn_sendsub(conn, TNO_AUTH, TNQ_NAME, conn->tty->ustruct->user);
	    if (kerberos_client_auth(NULL, conn->tty, &conn->address, authopts,
		NULL, NULL, NULL, (void *)kerberos_data, auth_type)) {
		if (kerberos_data->telnet_buf) {
		    kerberos_data->telnet_buf[0] = KERBEROS_V5;
		    kerberos_data->telnet_buf[1] = auth_type;
		    kerberos_data->telnet_buf[2] = KRB5_AUTH;
		    tn_sendsubcnt(conn, TNO_AUTH, TNQ_IS, 
				  kerberos_data->telnet_buf, 
				  kerberos_data->telnet_buflen);
		    free(kerberos_data->telnet_buf);
		    kerberos_data->telnet_buf = NULL;
		    kerberos_data->telnet_buflen = 0;
		    *result = TRUE;
		    break;
		}
	    } 
	    if (use_kerb_clients) {
		((tcbtype *) conn->tcb)->state = ABORTED;
		if (telnet_debug || kerberos_debug) {
		    printf("Kerberos:\tKerberized telnet failed!\n");
		}
	    }
	    if (telnet_debug || kerberos_debug) {
		printf("Failed to generate authentication data!\n");
	    }
	    break;
	case TNQ_REPLY:
	    /* We are the client. */
	    switch (conn->proto.tn.subbuffer[4]) {
		case AUTH_REJECT:
		    if (conn->proto.tn.subbuffer[5]) {
			printf("Kerberos:\t%s\n",&conn->proto.tn.subbuffer[5]);
		    }
		    if (use_kerb_clients) {
			((tcbtype *) conn->tcb)->state = ABORTED;
			if (telnet_debug || kerberos_debug) {
			    printf("Kerberos:\tKerberized telnet failed!\n");
			}
		    }
		    break;
		case AUTH_ACCEPT:
		    if (telnet_debug || kerberos_debug) {
			printf("Kerberos:\tAuthentication succeeded\n");
		    }
		    *result = TRUE;
		    if (do_forwarding) {
			foreignhost = strdup(name_addr2string(&conn->address));
			if (foreignhost == NULL) {
			    if (kerberos_debug) {
				buginf("\nKerberos:\tCannot forward "
					"credentials!");
			    }
			    free(foreignhost);
			    foreignhost = NULL;
			    break;
			}
			rsts = get_forwarding_creds((tcbtype *)conn->tcb,
						    conn->tty, foreignhost, 
						    kerberos_data->creds,
						    &outbuf);
			if (rsts) {
			    csfC5free_data(&outbuf);
			    if (kerberos_debug) {
				printf("Kerberos:\tFailed to forward "
					"credentials!\n");
			    }
			} else {
			    telnet_suboption_pad_iac(outbuf.dp, outbuf.dl,
						     &nbuf, &buflen);

			    buf = malloc_named(buflen+SUBOPT_PAD,
							MALLOC_TELNET_NAME);
			    if (buf == NULL) {
				if (kerberos_debug) {
				    free(nbuf);
				    nbuf = NULL;
				    printf("Kerberos:\tFailed to forward "
					   "credentials!\n");
				}
			    } else {
				buf[0] = KERBEROS_V5;
				buf[1] = auth_type;
				buf[2] = AUTH_FORWARD;
				memcpy(&(buf[3]), nbuf, buflen);
				buflen+=SUBOPT_PAD;

				if (outbuf.dl != 0) {
				    tn_sendsubcnt(conn, TNO_AUTH, TNQ_IS, buf, 
						  buflen);
				}
				free(buf);
				free(nbuf);
				buf = NULL;
				nbuf = NULL;
			    }
		        }
			csfC5free_data(&outbuf);
			free(foreignhost);
			foreignhost = NULL;
		    }
		    csfC5free_creds((csfC5_creds *)kerberos_data->creds);
		    kerberos_data->creds = NULL;
		    break;
		case AUTH_RESPONSE:
		    if (kerberos_mutual_auth(conn->tty, &conn->address, 
			&conn->proto.tn.subbuffer[5], length-5, 
			kerberos_data)) {
			if (telnet_debug || kerberos_debug) {
			    printf("Kerberos:\tMutual authentication "
				   "succeeded!\n");
			}
			*result = TRUE;
		    } else {
			printf("Kerberos:\tMutual authentication failed!\n");
		    }
		    kerberos_data->autht_sec = 0;
		    kerberos_data->autht_usec = 0;
		    break;
		case AUTH_FORWARD_ACCEPT:
		    printf("Kerberos:\tSuccessfully forwarded credentials\n");
		    *result = TRUE;
		    break;
		case AUTH_FORWARD_REJECT:
		    printf("Kerberos:\tForwarded credentials rejected\n");
		    printf("Kerberos:\t%s\n",&conn->proto.tn.subbuffer[5]);
		    *result = TRUE;
		    break;
		default:
		    if (telnet_debug || kerberos_debug) {
			buginf("\nKerberos:\tReceived bad authentication "
				"suboption response!");
		    }
	    }
	    break;
	case TNQ_IS:
	    /* We are the server. */
	    conn->proto.tn.subbuffer[length] = '\0';
	    switch (conn->proto.tn.subbuffer[4]) {
		case KRB5_AUTH:
		    if (kerberos_server_auth((tcbtype *)conn->tcb, 
			&conn->proto.tn.subbuffer[5], length-4, 
			kerberos_data)) {

			/* send mutual auth stuff if necessary */
			if (auth_type == AUTH_HOW_MUTUAL) {
			    if (kerberos_data->telnet_buf) {
				kerberos_data->telnet_buf[0] = KERBEROS_V5;
				kerberos_data->telnet_buf[1] = auth_type;
				kerberos_data->telnet_buf[2] = AUTH_RESPONSE;
			        tn_sendsubcnt(conn, TNO_AUTH, TNQ_REPLY,
					      kerberos_data->telnet_buf, 
					      kerberos_data->telnet_buflen);
				free(kerberos_data->telnet_buf);
				kerberos_data->telnet_buf = NULL;
				kerberos_data->telnet_buflen = 0;
				/* Fall through and send "accept" message. */
			    } else {
				if (telnet_debug || kerberos_debug) {
				    printf("Failed to generate mutual "
					   "authentication data!\n");
				}
				goto auth_krb5_failed;
			    }
			}
			conn->proto.tn.do_kerb_auth = FALSE;
			reject_reply[0] = KERBEROS_V5;
			reject_reply[1] = auth_type;
			reject_reply[2] = AUTH_ACCEPT;
			tn_sendsubcnt(conn, TNO_AUTH, TNQ_REPLY, 
				      reject_reply, 3);
			conn->tty->ustruct->acc_method = 
					AAA_ACC_METH_KRB5_TELNET;
			*result = TRUE;
			break;
		    } else {
			if (telnet_debug || kerberos_debug) {
			    printf("Failed to generate authentication data!\n");
			}
			goto auth_krb5_failed;
		    }
		case AUTH_FORWARD:
		    if (kerberos_data->subseskey) {
			seskey = kerberos_data->subseskey;
		    } else {
			seskey = kerberos_data->seskey;
		    }

		    /* Do forwarding stuff */
		    if (!kerberos_read_forwarded_creds((tcbtype *)conn->tcb,
			&conn->proto.tn.subbuffer[5], length-5, seskey)) {
			if (telnet_debug || kerberos_debug) {
			    printf("Kerberos:\tFailed to store forwarded "
				   "credentials!\n");
			}
			reject_reply[0] = KERBEROS_V5;
			reject_reply[1] = auth_type;
			reject_reply[2] = AUTH_FORWARD_REJECT;
		        tn_sendsubcnt(conn, TNO_AUTH, TNQ_REPLY, reject_reply, 
				      3);
		    } else {
			reject_reply[0] = KERBEROS_V5;
			reject_reply[1] = auth_type;
			reject_reply[2] = AUTH_FORWARD_ACCEPT;
		        tn_sendsubcnt(conn, TNO_AUTH, TNQ_REPLY, reject_reply, 
				      3);
		    }
		    conn->proto.tn.do_kerb_auth = FALSE;
		    csfC5free_crsKBlock((csfC5_crsKBlock *)
					kerberos_data->seskey);
		    free(kerberos_data->seskey);
		    kerberos_data->seskey = NULL;
		    *result = TRUE;
		    break;
		default:
		    if (telnet_debug || kerberos_debug) {
			buginf("\nKerberos:\tReceived bad authentication "
				"suboption response!");
		    }
		    goto auth_krb5_failed;
	    }
	    break;
	default:
	    if (conn->debug) {
		printf("[Unrecognized]\n");
	    }
	    if (telnet_debug) {
		buginf("[Unrecognized]\n");
	    }
    }
    return;

auth_krb5_failed:
    send_option(conn, STH_AUTH, STH_AUTH, TN_DONT, TNO_AUTH);
    conn->proto.tn.do_kerb_auth = FALSE;
    reject_reply[0] = KERBEROS_V5;
    reject_reply[1] = auth_type;
    reject_reply[2] = AUTH_REJECT;
    tn_sendsubcnt(conn, TNO_AUTH, TNQ_REPLY, reject_reply, 3);
    return;
}


/*
 * kerberos_free_kerberos_info()
 *      Frees Kerberos structure in conn
 */
void
kerberos_free_kerberos_info(void *kerberos_info1)
{
    kerberos_info *kerberos_data = (kerberos_info *)kerberos_info1;

    if (kerberos_data->telnet_buf) {
	free(kerberos_data->telnet_buf);
    }
    if (kerberos_data->seskey) {
	csfC5free_crsKBlock(kerberos_data->seskey);
    }
    if (kerberos_data->subseskey) {
	csfC5free_crsKBlock(kerberos_data->subseskey);
    }
    if (kerberos_data->creds) {
	csfC5free_creds(kerberos_data->creds);
    }
    free(kerberos_data);
}

boolean kerberos_validate_TGT (csfC5_creds *tmpcreds)
{
    int rsts = 0;
    boolean retval = FALSE;
    csfC5_cc *tmpcache = NULL;
    csfC5_credsReqCtx reqc;
    csfC5_creds *req_creds = NULL;
    csfC5_creds *ret_creds = NULL;
    csfC5_crsKBlock key;
    csfC5_tkt tkt;
    char myhostname[100];

    req_creds = malloc_named(sizeof(csfC5_creds), MALLOC_VALIDATE_NAME);
    if (req_creds == NULL) {
	goto krb5_validate_cleanup;
    }

    ret_creds = malloc_named(sizeof(csfC5_creds), MALLOC_VALIDATE_NAME);
    if (ret_creds == NULL) {
	goto krb5_validate_cleanup;
    }

    /* Clear Kerberos structures */
    csfC5clear_credsReqCtx(&reqc);
    csfC5clear_crsKBlock(&key);
    csfC5clear_tkt(&tkt);

    /* First create a temporary cred cache */
    rsts = csfC5resolve_cc((csfC5_cc **) &tmpcache, "MEM:");
    if (rsts) {
	if (kerberos_debug) {
	    buginf("\nKerberos:\tauthentication failed while "
		   "allocating temporary credentials cache");
	}
	goto krb5_validate_cleanup;
    }

    /* Next, initialize temporary cred cache */
    rsts = csfC5initialize_cc(tmpcache, &tmpcreds->client);
    if (rsts) {
	if (kerberos_debug) {
	    buginf("\nKerberos:\tauthentication failed while "
		   "initializing temporary credentials cache");
	}
	goto krb5_validate_cleanup;
    }

    /* Now save our cred into temporary cred cache */
    rsts = csfC5addCreds_cc(tmpcache, tmpcreds);
    if (rsts) {
	if (kerberos_debug) {
	    buginf("\nKerberos:\tauthentication failed while "
		   "adding credentials to temporary credentials cache");
	}
	goto krb5_validate_cleanup;
    }

    /* Fill in server principal ID. */
    if (!reg_invoke_name_lookup_self(myhostname, sizeof(myhostname))) {
	goto krb5_validate_cleanup;
    }

    rsts = csfC5netHostTo_prnId(myhostname, CSFC5c_NAME_SRVHST,
				CSFC5c_NT_SRV_HST, &req_creds->server);
    if (rsts) {
	printf("Kerberos:\tFailed to convert hostname to server principal!\n");
	goto krb5_validate_cleanup;
    }

    /* Fill in client principal ID. */
    rsts = csfC5copy_prnId(&tmpcreds->client, &req_creds->client);
    if (rsts) {
	printf("Kerberos:\tFailed to fill in client principal!\n");
	goto krb5_validate_cleanup;
    }

    reqc.req = req_creds;
    rsts = csfC5get_creds((csf_flags) 0, &reqc, tmpcache, ret_creds);
    if (rsts) {
	printf("Kerberos:\tFailed to retrieve temporary service credentials!\n");
	goto krb5_validate_cleanup;
    }

    /* Now decrypt our service credential with our srvtab */
    rsts = csfC5asnD_tkt(&ret_creds->e_tkt, &tkt);
    if (rsts) {
	printf("Kerberos:\tFailed to decode temporary service credential!\n");
	goto krb5_validate_cleanup;
    }

    rsts = csfC5readSvcKey_msgApReq(NULL, &ret_creds->server, tkt.ec_epart.kvno,
				    &key);
    if (rsts) {
	printf("Failed to retrieve SRVTAB key!\n");
	goto krb5_validate_cleanup;
    }

    rsts = csfC5rdEc_tktEPart(&tkt.ec_epart, &key, &tkt.epart);
    if (rsts) {
	printf("Failed to decrypt service credential ticket!\n");
	goto krb5_validate_cleanup;
    }

    retval = TRUE;

krb5_validate_cleanup:
    csfC5close_cc(&tmpcache);
    csfC5free_crsKBlock(&key);
    csfC5free_tkt(&tkt);
    csfC5free_creds(req_creds);
    free(req_creds);
    csfC5free_creds(ret_creds);
    free(ret_creds);
    return(retval);
}
