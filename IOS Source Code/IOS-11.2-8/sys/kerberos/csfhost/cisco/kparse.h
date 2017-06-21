/* $Id: kparse.h,v 3.6.20.2 1996/06/04 08:35:49 che Exp $
 * $Source: /release/112/cvs/Xsys/kerberos/csfhost/cisco/kparse.h,v $
 *------------------------------------------------------------------
 * kparse.h - Definitions for Kerberos parser results
 *
 * September 1995, Che-lin Ho
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: kparse.h,v $
 * Revision 3.6.20.2  1996/06/04  08:35:49  che
 * CSCdi57424:  router crashes attempting kerberos telnet
 * Branch: California_branch
 * Added kerberos_stackcheck() registry to ensure correct stacksize of
 * process running kerberos clients.
 *
 * Revision 3.6.20.1  1996/04/19  15:12:16  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commmit
 *
 * Revision 3.1.2.7  1996/04/16  03:54:36  che
 * Branch: IosSec_branch
 * Code changes mandated by code review comments.
 *
 * Revision 3.1.2.6  1996/04/02  08:56:50  carrel
 * cvs fixes
 *
 * Revision 3.1.2.5  1996/03/27  23:31:42  che
 * Branch: IosSec_branch
 * More Kerberos authentication cleanup
 *
 * Revision 3.1.2.4  1996/03/25  20:55:35  che
 * Branch: IosSec_branch
 * Clean up how authentication negotiation works
 *
 * Revision 3.1.2.3  1996/03/05  04:46:52  che
 * Branch: IosSec_branch
 * Kerberos Phase 2 commit.
 *
 * Revision 3.2  1995/11/17  17:45:53  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:19:48  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 3.1.2.2  1996/02/21  03:38:01  che
 * Branch: IosSec_branch
 * Sync to 11.1 Mainline (02/20/96)
 *
 * Revision 3.1.2.1  1996/01/23  05:57:29  che
 * Branch: IosSec_branch
 * Sync with 11.1 mainline. (960122)
 *
 * Revision 3.6  1996/02/06  23:34:16  che
 * CSCdi48098:  Need to be able to configure pre-auth type for Kerberos
 * messages
 * Added "[no] kerberos preauth [none|encrypted-unix-timestamp]"
 * config command.
 *
 * Revision 3.5  1996/01/13  01:34:00  che
 * CSCdi46854:  Kerberos KDC ports should be definable for each KDC server
 *
 * Revision 3.4  1996/01/04  22:51:23  che
 * CSCdi46133:  Kerberos KDC port should be configurable
 * Added "kerberos kdc-port <port number>" command.
 *
 * In send2kdc.c:
 *
 * Will now wait a minimum of 15 seconds for KDC reply instead of 5 ms.
 *
 * Revision 3.3  1995/12/24  09:35:21  che
 * CSCdi46019:  Store Kerberos credentials in Kerberos library cred cache
 * Use Kerberos library credentials cache structures and functions to
 * manipulate user credentials cache.
 *
 * Revision 3.2  1995/11/17  17:45:53  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:19:48  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef KPARSE_H
#define KPARSE_H
/*
 * kparse.h
 *	Definitions for Kerberos->Cisco parser interface
 */

/*
 * Values for kerberos_command()
 */
#define KERB_LOCALREALM 1	/* Set local realm name */
#define KERB_SRVTAB 2		/* Add an srvtab entry */
#define KERB_REALMS 3		/* Add a realms file entry */
#define KERB_CONF 4		/* Add a krb.conf entry */
#define SHOW_KERBEROS 5		/* Show Kerberos Credentials */
#define CLEAR_KERBEROS 6	/* Clear Kerberos Credentials */
#define KERB_INST 7		/* Define instance name for enable */
#define KERB_PREAUTH 8		/* Define Kerberos Preauth type */
#define KERB_CLIENTS 9		/* Define whether to use Kerberized clients */
#define KERB_DO_FOR 10		/* Define whether or not to forward creds */

#define TIMESTRINGSIZE 24

#define CSFC5_KINIT_DEFAULT_OPTIONS	CSFC5c_KDCOPT_FORWARDABLE_m
#define CSFC5_KINIT_DEFAULT_LIFE	60*60*8 /* 8 hours */
#define OPTS_FORWARD_CREDS		0x00000002
#define OPTS_FORWARDABLE_CREDS		0x00000001

typedef struct instance_map_ {
	char *instance_name;
	uint priv_level;
	struct instance_map_ *next;
} instance_map;

extern instance_map *krb_instance_map;
extern int kerb_preauth_method;
extern int use_kerb_clients;
extern boolean do_forwarding;
extern csfC5_rc *replay_cache;

/*
 * This describes an entry from what would have been the krb.conf
 * file.
 */
struct krbhst {
    char *k_host;	/* The host serving this realm */
    char *k_realm;	/* Realm name */
    int  k_port;	/* port that the KDC is listening on */
};

typedef struct kerberos_info_ {
	char *telnet_buf;
	int telnet_buflen;
	ulong autht_sec;
	ulong autht_usec;
	void *seskey;
	void *subseskey;
	void *creds;
} kerberos_info;

extern csf_sts csfC5get_HostRealmS(const char *host, char ***realmsp);
extern csf_sts csfC5netinetMakeFaddr(csfC5_netINetAddr adr,
				     csfC5_netINetport port, 
				     csfC5_netHAddr *outaddr);
extern csf_sts get_forwarding_creds(tcbtype *, tt_soc *, char *, csfC5_creds *,
				    csfC5_data *);
extern boolean kerberos_read_forwarded_creds(tcbtype *, char *, int, void *);
extern void kerberos_start_login(boolean *result, userstruct *u,
				 authenstruct *a, int action);
extern void kerberos_cont_login(boolean *result, userstruct *u,
				authenstruct *a);
extern int csfC5_set_default_realm(char *);
extern csf_sts add_krbhost_entry(char *, char *, int);
extern csf_sts del_krbhost_entry(char *, char *, int);
extern csf_sts peek_krbhost_entry(int, char **, char **, int *);
extern csf_sts add_host_realm(char *, char *);
extern csf_sts del_host_realm(char *, char *);
extern int peek_host_realm(int, char **, char **);
extern void kerberos_subsys_init(subsystype *);
extern void kerberos_do_author(boolean *, userstruct *, authorstruct *);
extern boolean kerberos_client_auth(tcbtype *, tt_soc *, addrtype *, long, 
				    char *, char *, char *, void *, int);
extern boolean kerberos_server_auth(tcbtype *, char *, int, void *);
extern void kerberos_telnet_auth(boolean *, conntype *, int, int);
extern boolean kerberos_is_running(void);
extern boolean kerberos_stackcheck(void);
extern boolean kerberos_use_kerb_clients(void);
extern void kerberos_free_kerberos_info(void *);
extern boolean kerberos_validate_TGT(csfC5_creds *);

#endif /* KPARSE_H */
