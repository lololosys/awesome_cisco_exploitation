/* $Id: forward.c,v 3.1.10.2 1996/06/28 23:20:13 hampton Exp $
 * $Source: /release/112/cvs/Xsys/kerberos/csfhost/cisco/forward.c,v $
 *------------------------------------------------------------------
 * forward.c - Code to handle credentials forwarding for Kerberos
 *
 * March 1996, Che-lin Ho
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: forward.c,v $
 * Revision 3.1.10.2  1996/06/28  23:20:13  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.1.10.1  1996/04/19  15:12:09  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commmit
 *
 * Revision 3.1.2.4  1996/04/16  03:54:25  che
 * Branch: IosSec_branch
 * Code changes mandated by code review comments.
 *
 * Revision 3.1.2.3  1996/03/27  23:31:30  che
 * Branch: IosSec_branch
 * More Kerberos authentication cleanup
 *
 * Revision 3.1.2.2  1996/03/12  03:12:27  che
 * Branch: IosSec_branch
 * Phase 2 Kerberos commit Part 2
 *
 * Revision 3.1.2.1  1996/03/05  04:46:40  che
 * Branch: IosSec_branch
 * Kerberos Phase 2 commit.
 *
 * Revision 3.1  1996/03/05  01:50:40  che
 * Placeholder file for new Kerberos sourcefiles.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#define CSF_HDRI_NET
#include <csfc5/header.h>
#include "types.h"
#include "ttysrv.h"
#include "aaa.h"
#include "../ui/debug.h"
#include "../os/os_debug_flags.h"
#include "kparse.h"
#include "../kerberos/include/csfc5/libz.h"

#define MALLOC_GETCRED_NAMED "Kerberos:get_forwarding_creds()"
#define MALLOC_READCRED_NAMED "Kerberos:read_forwarded_creds()"

/*
 * get_forwarding_creds()
 *	Retrieves new TGT from the TGS and forwards it to the remote server.
 */

csf_sts get_forwarding_creds(tcbtype *tcb, tt_soc *tty, char *foreignhost, 
			     csfC5_creds *serv_cred, csfC5_data *outbuf)
{
    char **hrealms = NULL;
    csf_sts rsts = CSFC5_STS_EINVAL;
    csfC5_creds *cred = NULL, *tgt = NULL, *outcreds = NULL;
    csfC5_cc *cred_cache = NULL;
    csfC5_prnNC *foreign_realm = NULL;
    csfC5_credsReqCtx reqc;
    csfC5_tStamp lifetime = CSFC5_KINIT_DEFAULT_LIFE;
    csfC5_msgCred *for_cred = NULL;
    csfC5_msgCredInfo *cred_info = NULL;
    csfC5_msgCredEPart *cred_EPart = NULL;
    csfC5_crsEBlock *EBlock = NULL;
    csfC5_data scratch;
    csfC5_usTStamp now;
    char *newptr = NULL;


    /* Initialize global variables */
    /* Allocate and initialize replay cache if it doesn't exist. */
    if (replay_cache == NULL) {
	rsts = csfC5resolve_rc(&replay_cache, "host");
	    if (rsts) {
		printf("Kerberos:\tCannot allocate replay cache!\n");
		goto krb5_mk_cred_fail;
	    }

	    rsts = csfC5initialize_rc(replay_cache, 0);
	    if (rsts) {
		printf("Kerberos:\tCannot initialize replay cache!\n");
		goto krb5_mk_cred_fail;
	    }
    }

    /* Initialize local veriables */
    csfC5clear_credsReqCtx(&reqc);
    csfC5clear_data(&scratch);

    rsts = csfC5get_usTStamp(&now);
    if (rsts) {
	if (kerberos_debug) {
	    buginf("\nKerberos:\tauthentication failed while "
		    "getting time of day");
	}
	goto krb5_mk_cred_fail;
    }

    /* Malloc the space we need to work */
    foreign_realm = malloc_named(sizeof(csfC5_prnNC), MALLOC_GETCRED_NAMED);
    if (foreign_realm == NULL) {
        rsts = CSFC5_STS_ENOMEM;
        goto krb5_mk_cred_fail;
    }

    cred = malloc_named(sizeof(csfC5_creds), MALLOC_GETCRED_NAMED);
    if (cred == NULL) {
        rsts = CSFC5_STS_ENOMEM;
        goto krb5_mk_cred_fail;
    }

    tgt = malloc_named(sizeof(csfC5_creds), MALLOC_GETCRED_NAMED);
    if (tgt == NULL) {
        rsts = CSFC5_STS_ENOMEM;
        goto krb5_mk_cred_fail;
    }

    outcreds = malloc_named(sizeof(csfC5_creds), MALLOC_GETCRED_NAMED);
    if (outcreds == NULL) {
        rsts = CSFC5_STS_ENOMEM;
        goto krb5_mk_cred_fail;
    }

    for_cred = malloc_named(sizeof(csfC5_msgCred), MALLOC_GETCRED_NAMED);
    if (for_cred == NULL) {
	rsts = CSFC5_STS_ENOMEM;
	goto krb5_mk_cred_fail;
    }
    csfC5clear_crsEData(&for_cred->ec_epart);
    for_cred->tickets = NULL;

    cred_info = malloc_named(sizeof(csfC5_msgCredInfo), MALLOC_GETCRED_NAMED);
    if (cred_info == NULL) {
	rsts = CSFC5_STS_ENOMEM;
	goto krb5_mk_cred_fail;
    }

    cred_EPart = malloc_named(sizeof(csfC5_msgCredEPart),MALLOC_GETCRED_NAMED);
    if (cred_EPart == NULL) {
	rsts = CSFC5_STS_ENOMEM;
	goto krb5_mk_cred_fail;
    }
    csfC5clear_msgCredEPart(cred_EPart);

    EBlock = malloc_named(sizeof(csfC5_crsEBlock), MALLOC_GETCRED_NAMED);
    if (EBlock == NULL) {
	rsts = CSFC5_STS_ENOMEM;
	goto krb5_mk_cred_fail;
    }

    /* Get realm name for remote host */
    rsts = csfC5get_HostRealmS(foreignhost, &hrealms);
    if ((rsts) || (!hrealms)) {
	printf("Kerberos:\tCannot determine foreign host's realm name!\n");
	goto krb5_mk_cred_fail;
    }
    rsts = csfC5import_data((void *) hrealms[0], strlen(hrealms[0]),
			    &(foreign_realm->namec));
    if (rsts) {
	printf("Kerberos:\tFailed to convert foreign host's realm name!\n");
	goto krb5_mk_cred_fail;
    }
    
    /* Copy principal into client prnID in cred structure */
    rsts = csfC5copy_prnId(&(serv_cred->client), &(cred->client));
    if (rsts) {
	printf("Kerberos:\tFailed to copy principal into cred structure!\n");
	goto krb5_mk_cred_fail;
    }

    /* Build server prnId in cred structure */
    rsts = csfC5tgsName_prnId(foreign_realm, NULL, NULL, &(cred->server));
    if (rsts) {
	printf("Kerberos:\tFailed to create server principal in cred "
		"structure!\n");
	goto krb5_mk_cred_fail;
    }

    /* Fill in other parts of the cred structure */
    cred->times.tstart = 0;
    cred->times.trenew = 0;
    cred->times.tend = 0;

    cred->tktflags = (csfC5_tktFlags) csfC5getcfgnum( CSFC5_CFG_KDCREQ_DEFOPT );
    reqc.req = cred;

    cred_cache = (csfC5_cc *) tty->ustruct->cred->cred;
    rsts = csfC5get_creds((csf_flags) CSFC5_GC_CACHED, &reqc, cred_cache, tgt);
    if (rsts) {
	printf("Kerberos:\tFailed to retrieve TGT!\n");
	goto krb5_mk_cred_fail;
    }

    rsts = csfC5getNow_tStamp(&cred->times.tend);
    if (rsts) {
	if (kerberos_debug) {
	    buginf("\nKerberos:\tauthentication failed while "
		    "getting time of day");
	}
	goto krb5_mk_cred_fail;
    }

    cred->times.tend += lifetime;

    if (!(tgt->tktflags & CSFC5c_KDCOPT_FORWARDABLE_m)) {
	printf("Kerberos:\tTGT is not forwardable!\n");
	goto krb5_mk_cred_fail;
    }

    csfC5clear_credsReqCtx(&reqc);
    reqc.req = cred;
    reqc.use = tgt;
    rsts = csfC5getViaTgt_creds(&reqc, outcreds);
    if (rsts) {
	printf("Kerberos:\tFailed to get forwarding creds from TGS!\n");
	if (rsts == CSFC5_STS_RC_REPLAY) {
	    printf("Kerberos:\treplay found and re-transmitted\n");
	}
	goto krb5_mk_cred_fail;
    }

    /* Check replay cache for replay */
    if (replay_cache) {
	rsts = csfC5putEntry_rc(replay_cache, outcreds->times.tend, 
				    &outcreds->e_tkt);
	if (rsts) {
	    printf("Kerberos:\tReplay Cache check failed!\n");
	    if (rsts == CSFC5_STS_RC_REPLAY) {
		printf("Kerberos:\treplay found and re-transmitted\n");
	    }
	}
    }

    /* Fill in Cred EPart stuff */
    cred_EPart->nonce = 0;
    cred_EPart->ctime = now;
    rsts = csfC5netinetMakeFaddr(tcb->localhost.addr.ip_address,
				 tcb->localport, &(cred_EPart->s_haddr));
    if (rsts) {
	goto krb5_mk_cred_fail;
    }

    cred_EPart->tktinfo = (csfC5_msgCredInfo **) 
	malloc_named(sizeof(csfC5_msgCredInfo *)*2, MALLOC_GETCRED_NAMED);
    cred_EPart->tktinfo[0] = cred_info;
    cred_EPart->tktinfo[1] = NULL;
   
    /* Fill in Cred Info stuff */
    for_cred->pvno = CSFC5c_PVNO;
    for_cred->msg_type = CSFC5c_AMT_CRED;
    cred_info->flags = outcreds->tktflags;
    cred_info->times = outcreds->times;

    for_cred->tickets = malloc_named(sizeof(csfC5_tkt *)*2,
						   MALLOC_GETCRED_NAMED);
    if (for_cred->tickets == NULL) {
	goto krb5_mk_cred_fail;
    }
    for_cred->tickets[0] = malloc_named(sizeof(csfC5_tkt),
						     MALLOC_GETCRED_NAMED);
    if (for_cred->tickets[0] == NULL) {
	goto krb5_mk_cred_fail;
    }
    rsts = csfC5asnD_tkt(&outcreds->e_tkt, for_cred->tickets[0]);
    if (rsts) {
	goto krb5_mk_cred_fail;
    }
    for_cred->tickets[1] = NULL;

    rsts = csfC5copy_crsKBlock(&outcreds->seskey, &cred_info->seskey);
    if (rsts) {
	goto krb5_mk_cred_fail;
    }
    rsts = csfC5copy_prnId(&outcreds->client, &cred_info->client);
    if (rsts) {
	goto krb5_mk_cred_fail;
    }
    rsts = csfC5copy_prnId(&outcreds->server, &cred_info->server);
    if (rsts) {
	goto krb5_mk_cred_fail;
    }
    rsts = csfC5copyC_netHAddrS(outcreds->haddrs, &cred_info->caddrs);
    if (rsts) {
	goto krb5_mk_cred_fail;
    }

    /* Encode to-be-encrypted part of message */
    rsts = csfC5asnE_msgCredEPart(cred_EPart, &scratch);
    if (rsts) {
	goto krb5_mk_cred_fail;
    }

    /* Put together an EBlock */
    for_cred->ec_epart.kvno = 0;
    rsts = csfC5getEType_crsKType(serv_cred->seskey.key_type, 
				  &for_cred->ec_epart.etype);
    if (rsts) {
	goto krb5_mk_cred_fail;
    }

    /* Set up EBlock */
    rsts = csfC5setEType_crsEBlock(EBlock, for_cred->ec_epart.etype);
    if (rsts) {
	goto krb5_mk_cred_fail;
    }

    for_cred->ec_epart.ciphertext.dl = csfC5crsEncryptSize(scratch.dl, EBlock);

    /* add padding area and zero it */
    scratch.dp = realloc(scratch.dp, for_cred->ec_epart.ciphertext.dl);
    if (scratch.dp == NULL) {
	rsts = CSFC5_STS_ENOMEM;
	goto krb5_mk_cred_fail;
    }
    newptr = (char *)scratch.dp;
    newptr += scratch.dl;
    memset(newptr, 0, for_cred->ec_epart.ciphertext.dl-scratch.dl);

    for_cred->ec_epart.ciphertext.dp = 
	malloc_named(for_cred->ec_epart.ciphertext.dl, MALLOC_GETCRED_NAMED);
    if (for_cred->ec_epart.ciphertext.dp == NULL) {
	rsts = CSFC5_STS_ENOMEM;
	goto krb5_mk_cred_fail;
    }

    /* Process Key */
    rsts = csfC5crsProcessKey(EBlock, &serv_cred->seskey);
    if (rsts) {
	goto krb5_mk_cred_fail;
    }

    /* Encrypt */
    rsts = csfC5crsEncrypt(scratch.dp, for_cred->ec_epart.ciphertext.dp, 
			   scratch.dl, EBlock, 0);
    if (rsts) {
	goto krb5_mk_cred_fail;
    }
    rsts = csfC5crsFinishKey(EBlock);

    /* Encode Cred structure */
    rsts = csfC5asnE_msgCred(for_cred, outbuf);
    if (rsts) {
	goto krb5_mk_cred_fail;
    }
    
krb5_mk_cred_fail:
    if (rsts) {
	printf("Failed to send forwarded credentials!\n");
    }
    csfC5free_crsEData(&for_cred->ec_epart);
    csfC5free_data(&scratch);
    csfC5freeC_HostRealmS(&hrealms);
    csfC5free_creds(cred);
    csfC5free_creds(tgt);
    csfC5free_creds(outcreds);
    csfC5free_prnNC(foreign_realm);
    csfC5freeC_tktS(&for_cred->tickets);
    csfC5free_msgCredEPart(cred_EPart);
    free(foreign_realm);
    free(cred);
    free(tgt);
    free(outcreds);
    free(for_cred);
    free(cred_EPart);
    free(EBlock);
    return(rsts);
}

/*
 * kerberos_read_forwarded_creds()
 *	Reads forwarded credentials from remote client and stores them in
 *	the user's credentials cache.
 */
boolean kerberos_read_forwarded_creds (tcbtype *tcb, char *data, int datalen,
			      void *cs_seskey)
{
    csf_sts rsts = CSFC5_STS_EINVAL;
    csfC5_msgCred *for_cred = NULL;
    csfC5_msgCredEPart *cred_EPart = NULL;
    csfC5_crsEBlock *EBlock = NULL;
    csfC5_msgCredInfo *cred_info = NULL;
    csfC5_data scratch, inbuf;
    csfC5_netHAddr localaddr, remoteaddr;
    int cnt = 0, ncreds = 0;
    csfC5_creds **cred_list = NULL;

    /* Initialize global variables */
    /* Allocate and initialize replay cache if it doesn't exist. */
    if (replay_cache == NULL) {
	rsts = csfC5resolve_rc(&replay_cache, "host");
	    if (rsts) {
		printf("Kerberos:\tCannot allocate replay cache!\n");
		goto krb5_rd_cred_fail;
	    }

	    rsts = csfC5initialize_rc(replay_cache, 0);
	    if (rsts) {
		printf("Kerberos:\tCannot initialize replay cache!\n");
		goto krb5_rd_cred_fail;
	    }
    }

    /* Initialize local veriables */
    csfC5clear_data(&scratch);
    csfC5clear_data(&inbuf);
    csfC5clear_netHAddr(&localaddr);
    csfC5clear_netHAddr(&remoteaddr);

    /* Read data from network if we don't have it already */
    if ((data == NULL) && (datalen == 0)) {
	rsts = csfC5netReadMsg(tcb, &inbuf);
	if (rsts) {
	    goto krb5_rd_cred_fail;
	}
	if (inbuf.dl == 0) {
	    rsts = CSF_STS_OK;
	    goto krb5_rd_cred_fail;
	}
    } else {
	inbuf.dl = datalen;
	inbuf.dp = (void *)data;
    }

    /* Malloc the space we need to work */
    for_cred = malloc_named(sizeof(csfC5_msgCred), MALLOC_READCRED_NAMED);
    if (for_cred == NULL) {
	rsts = CSFC5_STS_ENOMEM;
	goto krb5_rd_cred_fail;
    }
    csfC5clear_crsEData(&for_cred->ec_epart);
    for_cred->tickets = NULL;

    cred_EPart = malloc_named(sizeof(csfC5_msgCredEPart),
						    MALLOC_READCRED_NAMED);
    if (cred_EPart == NULL) {
	rsts = CSFC5_STS_ENOMEM;
	goto krb5_rd_cred_fail;
    }
    csfC5clear_msgCredEPart(cred_EPart);
    EBlock = malloc_named(sizeof(csfC5_crsEBlock), MALLOC_READCRED_NAMED);
    if (EBlock == NULL) {
	rsts = CSFC5_STS_ENOMEM;
	goto krb5_rd_cred_fail;
    }


    /* Decode cred structure */
    rsts = csfC5asnD_msgCred(&inbuf, for_cred);
    if (rsts) {
	goto krb5_rd_cred_fail;
    }

    /* Decrypt encrypted cred message */

    /* Set up EBlock */
    rsts = csfC5setEType_crsEBlock(EBlock, for_cred->ec_epart.etype);
    if (rsts) {
	goto krb5_rd_cred_fail;
    }
    scratch.dl = for_cred->ec_epart.ciphertext.dl;
    scratch.dp = malloc_named(scratch.dl, MALLOC_READCRED_NAMED);
    if (scratch.dp == NULL) {
	goto krb5_rd_cred_fail;
    }

    /* Do Key processing */
    rsts = csfC5crsProcessKey(EBlock, (csfC5_crsKBlock *) cs_seskey);
    if (rsts) {
	goto krb5_rd_cred_fail;
    }

    /* Decrypt */
    rsts = csfC5crsDecrypt(for_cred->ec_epart.ciphertext.dp, scratch.dp, 
			   scratch.dl, EBlock, NULL);

    if (rsts) {
	csfC5crsFinishKey(EBlock);
	goto krb5_rd_cred_fail;
    }
    rsts = csfC5crsFinishKey(EBlock);
    if (rsts) {
	goto krb5_rd_cred_fail;
    }

    /* Decode newly decrypted part */
    rsts = csfC5asnD_msgCredEPart(&scratch, cred_EPart);
    if (rsts) {
	goto krb5_rd_cred_fail;
    }

    rsts = csfC5netinetMakeFaddr(tcb->localhost.addr.ip_address,
				 tcb->localport, &localaddr);
    if (rsts) {
	goto krb5_rd_cred_fail;
    }
    rsts = csfC5netinetMakeFaddr(tcb->foreignhost.addr.ip_address,
				 tcb->foreignport, &remoteaddr);
    if (rsts) {
	goto krb5_rd_cred_fail;
    }
    if (!csfC5isEq_netHAddr(&remoteaddr, &(cred_EPart->s_haddr)))
	goto krb5_rd_cred_fail;
    if (cred_EPart->r_haddr.haddr_data.dl != 0) {
	if (localaddr.haddr_data.dl != 0) {
	    if (!csfC5isEq_netHAddr(&localaddr, &(cred_EPart->r_haddr))) {
		goto krb5_rd_cred_fail;
	    }
	}
    }

    /* Count number of tickets */
    for (ncreds=0;for_cred->tickets[ncreds];ncreds++);

    /* Generate enough pointers for number of tickets */
    cred_list = malloc_named((sizeof(csfC5_creds *) * ncreds)+1,
					     MALLOC_READCRED_NAMED);
    if (cred_list == NULL) {
	goto krb5_rd_cred_fail;
    }

    /* Create and fill in linked list of creds */
    for (cnt = 0; cnt < ncreds; cnt++) {
	cred_info = cred_EPart->tktinfo[cnt];

	cred_list[cnt] = malloc_named(sizeof(csfC5_creds),
						     MALLOC_READCRED_NAMED);
	if (cred_list[cnt] == NULL) {
	    goto krb5_rd_cred_fail;
	}

	rsts = csfC5copy_prnId(&cred_info->client, &((cred_list[cnt])->client));
	if (rsts) {
	    goto krb5_rd_cred_fail;
	}
	rsts = csfC5copy_prnId(&cred_info->server, &((cred_list[cnt])->server));
	if (rsts) {
	    goto krb5_rd_cred_fail;
	}
	rsts = csfC5copy_crsKBlock(&cred_info->seskey, &((cred_list[cnt])->seskey));
	if (rsts) {
	    goto krb5_rd_cred_fail;
	}
	rsts = csfC5copyC_netHAddrS(cred_info->caddrs, &((cred_list[cnt])->haddrs));
	if (rsts) {
	    goto krb5_rd_cred_fail;
	}
	rsts = csfC5asnE_tkt(for_cred->tickets[cnt], &((cred_list[cnt])->e_tkt));
	if (rsts) {
	    goto krb5_rd_cred_fail;
	}
	
	(cred_list[cnt])->times = cred_info->times;
	(cred_list[cnt])->is_skey = FALSE;
	(cred_list[cnt])->tktflags = cred_info->flags;
	(cred_list[cnt])->authz = NULL;
	csfC5clear_data(&((cred_list[cnt])->e_tkt2));
    }
    cred_list[cnt] = NULL;

    /* Check for replay */
    if (replay_cache) {
	for (cnt = 0; cnt < ncreds; cnt++) {
	    rsts = csfC5putEntry_rc(replay_cache, cred_list[cnt]->times.tend, 
				    &(cred_list[cnt]->e_tkt));
	    if (rsts) {
		printf("Kerberos:\tReplay Cache check failed!\n");
		if (rsts == CSFC5_STS_RC_REPLAY) {
		    printf("Kerberos:\treplay found and re-transmitted\n");
		}
	    }
	}
    }

    /* Create and Initialize cred cache */
    if (tcb && tcb->tty && tcb->tty->ustruct) {
	tcb->tty->ustruct->cred = (struct credentials *) 
				  malloc_named(sizeof(struct credentials),
					       MALLOC_READCRED_NAMED);
	if (tcb->tty->ustruct->cred == NULL) {
	    rsts = CSFC5_STS_ENOMEM;
	    goto krb5_rd_cred_fail;
	}
	tcb->tty->ustruct->cred->ref_cnt = 1;
	tcb->tty->ustruct->cred->free_funct = (void *) csfC5close_cc;
	tcb->tty->ustruct->cred->cred_type = KERBEROS;

	rsts = csfC5resolve_cc((csfC5_cc **)&(tcb->tty->ustruct->cred->cred), 
				"MEM:");
	if (rsts) {
	    goto krb5_rd_cred_fail;
	}
	rsts = csfC5copy_prnId(&((cred_list[0])->client), 
		&((csfC5_cc *) tcb->tty->ustruct->cred->cred)->prnid);
	if (rsts) {
	    goto krb5_rd_cred_fail;
	}
	rsts = csfC5initialize_cc(tcb->tty->ustruct->cred->cred, 
				  &((cred_list[0])->client));
	if (rsts) {
	    goto krb5_rd_cred_fail;
	}
	for (cnt = 0; cnt < ncreds; cnt++) {
	    if (kerberos_validate_TGT(cred_list[cnt])) {
		rsts = csfC5addCreds_cc(
				    (csfC5_cc *)tcb->tty->ustruct->cred->cred, 
				    cred_list[cnt]);
		if (rsts) {
		    goto krb5_rd_cred_fail;
		}
	    } else {
		rsts = -1;
		printf("Kerberos:\tCould not validate forwarded credential.  "
		       "Discarding...\n");
		goto krb5_rd_cred_fail;
	    }
	}
    } else {
	rsts = CSFC5_STS_CC_NOCC;
    }

krb5_rd_cred_fail:
    if (rsts) {
	printf("Failed to read forwarded credentials!\n");
    }
    for (cnt = 0; cnt < ncreds; cnt++)
	csfC5free_creds(cred_list[cnt]);
    free(cred_list);
    csfC5free_crsEData(&for_cred->ec_epart);
    csfC5free_data(&scratch);
    if ((data == NULL) && (datalen == 0)) {
	csfC5free_data(&inbuf);
    }
    csfC5free_netHAddr(&localaddr);
    csfC5free_netHAddr(&remoteaddr);
    if (for_cred && for_cred->tickets) {
	csfC5freeC_tktS(&for_cred->tickets);
    }
    csfC5free_msgCredEPart(cred_EPart);
    free(for_cred);
    free(cred_EPart);
    free(EBlock);
    if (rsts) {
	return(FALSE);
    } else {
	return(TRUE);
    }
}
