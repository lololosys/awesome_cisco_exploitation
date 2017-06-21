/* $Id: cfg_kerberos.h,v 3.5.20.2 1996/08/06 19:45:09 che Exp $
 * $Source: /release/112/cvs/Xsys/kerberos/include/cfg_kerberos.h,v $
 *------------------------------------------------------------------
 * cfg_kerberos.h - Kerberos configuration commands
 *
 * September 1995, Che-lin Ho
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cfg_kerberos.h,v $
 * Revision 3.5.20.2  1996/08/06  19:45:09  che
 * CSCdi65278:  Clean up Kerberos srvtab parser help strings
 * Branch: California_branch
 * Make kerberos srvtab entry parser help match what is in the docs
 *
 * Revision 3.5.20.1  1996/04/19  15:12:28  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commmit
 *
 * Revision 3.1.2.5  1996/04/16  03:54:48  che
 * Branch: IosSec_branch
 * Code changes mandated by code review comments.
 *
 * Revision 3.1.2.4  1996/04/02  07:40:22  carrel
 * cvs fixes
 *
 * Revision 3.1.2.3  1996/03/05  04:47:09  che
 * Branch: IosSec_branch
 * Kerberos Phase 2 commit.
 *
 * Revision 3.2  1995/11/17  17:47:02  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1.2.2  1996/02/21  03:38:07  che
 * Branch: IosSec_branch
 * Sync to 11.1 Mainline (02/20/96)
 *
 * Revision 3.1.2.1  1996/01/23  05:57:41  che
 * Branch: IosSec_branch
 * Sync with 11.1 mainline. (960122)
 *
 * Revision 3.5  1996/02/06  23:34:21  che
 * CSCdi48098:  Need to be able to configure pre-auth type for Kerberos
 * messages
 * Added "[no] kerberos preauth [none|encrypted-unix-timestamp]"
 * config command.
 *
 * Revision 3.4  1996/01/13  01:34:09  che
 * CSCdi46854:  Kerberos KDC ports should be definable for each KDC server
 *
 * Revision 3.3  1996/01/04  22:52:49  che
 * CSCdi46133:  Kerberos KDC port should be configurable
 * Added "kerberos kdc-port <port number>" command.
 *
 * Revision 3.2  1995/11/17  17:47:02  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:20:46  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/**************************************************
 *
 * [no] kerberos credentials forward
 * Defines whether or not to forward Kerberos credentials
 * Variables:
 *	None
 */
EOLS	(kerberos_do_forward_eol, kerberos_command, KERB_DO_FOR);
NOPREFIX(kerberos_do_forward_nopre, kerberos_do_forward_eol, 
	kerberos_do_forward_eol);
NVGENS	(kerberos_do_forward_nvgen, kerberos_do_forward_nopre,
	kerberos_command, KERB_DO_FOR);
KEYWORD	(kerberos_do_forward, kerberos_do_forward_nvgen, no_alt, 
	"forward", "Forward Kerberos credentials", PRIV_CONF);
KEYWORD (kerberos_credentials, kerberos_do_forward, no_alt,
	 "credentials", "Kerberos credentials configuration options",
	 PRIV_CONF);


/**************************************************
 *
 * [no] kerberos instance map INSTANCE PRIVILEGE
 * Defines instance of a user to be used in enable password
 * Variables:
 *	OBJ(string,1) = Kerberos instance name.
 *	OBJ(int,1) = Privilege level.
 */
EOLS    (kerberos_instance_eol, kerberos_command, KERB_INST);
NUMBER  (kerberos_instance_priv_level, kerberos_instance_eol,
	 no_alt, OBJ(int,1), PRIV_MIN, PRIV_MAX, "Privilege level");
NOPREFIX(kerberos_instance_nopre, kerberos_instance_priv_level,
	 kerberos_instance_eol);
STRING  (kerberos_instance_name, kerberos_instance_nopre,
	 no_alt, OBJ(string,1), "Enable instance name");
NVGENS  (kerberos_instance_nvgen, kerberos_instance_name,
	 kerberos_command, KERB_INST);
KEYWORD (kerberos_instance_map, kerberos_instance_nvgen, no_alt, "map",
	 "Map Kerberos instance to a privilege level", PRIV_CONF);
KEYWORD (kerberos_instance, kerberos_instance_map, kerberos_credentials,
	 "instance", "Kerberos instance commands", PRIV_CONF);


/**************************************************
 *
 * [no] kerberos preauth [none|encrypted-unix-timestamp]
 * Defines the preauth method to use during authentication
 * Variables:
 *	OBJ(int,1) = Pre-auth method.
 *		0 = NONE (default)
 *		5 = Encrypted unix timestamp
 */
EOLS	(kerberos_preauth_eol, kerberos_command, KERB_PREAUTH);
KEYWORD_ID(kerberos_preauth_enc_unix_time, kerberos_preauth_eol,
	no_alt, OBJ(int,1), CSFC5cx_PADATA_T_ENC_UNIX_TIMESTAMP,
	"encrypted-unix-timestamp", "Use encrypted unix timestamp", PRIV_CONF);
KEYWORD_ID(kerberos_preauth_none, kerberos_preauth_eol,
	kerberos_preauth_enc_unix_time, OBJ(int,1), CSFC5c_PADATA_T_NONE,
	"none", "Do not use Kerberos pre-auth", PRIV_CONF);
NOPREFIX(kerberos_preauth_nopre, kerberos_preauth_none, kerberos_preauth_eol);
NVGENS	(kerberos_preauth_nvgen, kerberos_preauth_nopre,
	kerberos_command, KERB_PREAUTH);
KEYWORD	(kerberos_preauth, kerberos_preauth_nvgen,
	kerberos_instance, "preauth", "define Kerberos preauth type", 
	PRIV_CONF);


/**************************************************
 *
 * [no] kerberos server REALM HOST PORT
 * Defines the location of the KDC serving the specified Kerberos REALM.
 * Variables:
 *	OBJ(int,1) = Port that the KDC is listening on.
 *	OBJ(string,1) = Name of Kerberos REALM.
 *	OBJ(paddr,1) = IP address of host where KDC is running.
 */

EOLS	(kerberos_server_eol, kerberos_command, KERB_CONF);
NUMBER  (kerberos_server_port, kerberos_server_eol, kerberos_server_eol,
        OBJ(int,1), 0, 65535, "Port number (default is 88)");
SET	(kerberos_server_default_port, kerberos_server_port, OBJ(int,1), 
	    KERBEROS_KDC_PORT);
IPADDR_NAME(kerberos_server_entry_host, kerberos_server_default_port, no_alt,
	    OBJ(paddr,1), "Server for realm");
STRING	(kerberos_server_entry, kerberos_server_entry_host, no_alt,
	OBJ(string,1), "Kerberos Realm name");
NVGENS	(kerberos_server_nvgen, kerberos_server_entry,
	kerberos_command, KERB_CONF);
KEYWORD	(kerberos_server, kerberos_server_nvgen,
	kerberos_preauth, "server", "add realm to host mapping", 
	PRIV_CONF);


/**************************************************
 * [no] kerberos clients [mandatory]
 * If you have Kerberos credentials, defines whether or not to use Kerberized
 * clients.
 * Variables:
 *	OBJ(int,1) = Must use Kerberized clients?
 *		0 = optional (default)
 *		1 = mandatory
 */
EOLS	(kerberos_clients_eol, kerberos_command, KERB_CLIENTS);
KEYWORD_ID(kerberos_clients_mand, kerberos_clients_eol, no_alt, OBJ(int,1), 1, 
	"mandatory", "Must use Kerberized clients", PRIV_CONF);
NOPREFIX(kerberos_clients_nopre, kerberos_clients_mand, kerberos_clients_eol);
NVGENS	(kerberos_clients_nvgen, kerberos_clients_nopre,
	kerberos_command, KERB_CLIENTS);
KEYWORD	(kerberos_clients, kerberos_clients_nvgen,
	kerberos_server, "clients", 
	"Use Kerberized clients if user has Kerberos creds", PRIV_CONF);


/**************************************************
 *
 * [no] kerberos realm DNS-suffix REALM
 * Maps DNS suffixes and hostnames to a Kerberos REALM
 * Variables:
 * 	OBJ(string,1) = DNS suffix.
 *	OBJ(string,2) = Name of a Kerberos REALM.
 */

EOLS	(kerberos_realm_eol, kerberos_command, KERB_REALMS);
STRING	(kerberos_realm_realm, kerberos_realm_eol, no_alt,
	OBJ(string,2), "Kerberos Realm for given DNS suffix");
STRING	(kerberos_realm_entry, kerberos_realm_realm, no_alt,
	OBJ(string,1), "DNS suffix");
NVGENS	(kerberos_realm_nvgen, kerberos_realm_entry,
	kerberos_command, KERB_REALMS);
KEYWORD	(kerberos_realm, kerberos_realm_nvgen,
	kerberos_clients, "realm", "Add domain to realm mapping", PRIV_CONF);


/**************************************************
 *
 * [no] kerberos srvtab remote IP_ADDRESS FILENAME
 * [no] kerberos srvtab entry KEYTAB_VERSION_NUMBER PRINCIPAL VERSION_NUMBER
 *			    TIMESTAMP ENTRY_VERSION_NUMBER KEYTYPE
 *			    ENCODED KEY
 * Define a key for a service.
 * Variables:
 *    OBJ(paddr,1) = name of remote host.
 *    OBJ(string,1) = name of srvtab on remote host.
 *    OBJ(string,2) = principal string.
 *    OBJ(string,3) = Timestamp
 *    OBJ(string,4) = Encoded key.
 *    OBJ(int,1) = 0=get srvtab from remote host;  1=manually entered srvtab
 *    OBJ(int,2) = Principal type
 *    OBJ(int,3) = Key version number
 *    OBJ(int,4) = Keytype
 *    OBJ(int,5) = Keylength
 */

EOLS	(kerberos_srvtab_eol, kerberos_command, KERB_SRVTAB);
STRING	(kerberos_srvtab_entry_key, kerberos_srvtab_eol, no_alt,
	OBJ(string,4), "encrypted-key");
NUMBER	(kerberos_srvtab_entry_keylen, kerberos_srvtab_entry_key, no_alt,
	OBJ(int,5), 1, 65536, "key-length");
NOPREFIX(kerberos_srvtab_entry_nopre, kerberos_srvtab_entry_keylen,
	kerberos_srvtab_eol);
NUMBER	(kerberos_srvtab_entry_keytype, kerberos_srvtab_entry_nopre, no_alt,
	OBJ(int,4), 1, 65536, "key-type");
NUMBER	(kerberos_srvtab_entry_kvno, kerberos_srvtab_entry_keytype, no_alt,
	OBJ(int,3), 1, 65536, "key-version number");
STRING	(kerberos_srvtab_entry_timestamp, kerberos_srvtab_entry_kvno, no_alt,
	OBJ(string,3), "timestamp");
NUMBER	(kerberos_srvtab_entry_verno, kerberos_srvtab_entry_timestamp, no_alt,
	OBJ(int,2), 0, 65536, "principal-type");
STRING	(kerberos_srvtab_entry_principal, kerberos_srvtab_entry_verno, no_alt,
	OBJ(string,2), "srvtab-principal");
SET	(kerberos_srvtab_set_manual, kerberos_srvtab_entry_principal,
	OBJ(int,1), 1);
KEYWORD	(kerberos_srvtab_entry, kerberos_srvtab_set_manual, no_alt,
	"entry", "Encoded srvtab entry", PRIV_CONF);
STRING	(kerberos_srvtab_remote_file, kerberos_srvtab_eol, no_alt,
	OBJ(string,1), "Name of srvtab file on remote host");
IPADDR_NAME(kerberos_srvtab_remote_name, kerberos_srvtab_remote_file, no_alt,
	OBJ(paddr,1), "Remote srvtab server");
NOPREFIX(kerberos_srvtab_remote_nopre, kerberos_srvtab_remote_name,
	kerberos_srvtab_eol);
SET	(kerberos_srvtab_remote_set, kerberos_srvtab_remote_nopre,
	OBJ(int,1), 0);
KEYWORD	(kerberos_srvtab_remote, kerberos_srvtab_remote_set,
	kerberos_srvtab_entry, "remote", "Retrieve srvtab from remote host",
	PRIV_CONF);
NVGENS	(kerberos_srvtab_nvgen, kerberos_srvtab_remote, kerberos_command,
	KERB_SRVTAB);
KEYWORD	(kerberos_srvtab, kerberos_srvtab_nvgen,
	kerberos_realm, "srvtab", "Add principal to key mapping", PRIV_CONF);


/**************************************************
 *
 * [no] kerberos local-realm REALM
 * Define a name of a Kerberos REALM.
 * Variables:
 * 	OBJ(string,1) = ascii name of Kerberos REALM.
 */

EOLS	(kerberos_local_realm_eol, kerberos_command, KERB_LOCALREALM);
STRING	(kerberos_local_realm_name, kerberos_local_realm_eol,
	no_alt, OBJ(string,1), "Local kerberos realm name");
NOPREFIX(no_kerberos_local_realm_name, kerberos_local_realm_name,
	kerberos_local_realm_eol);
NVGENS	(kerberos_local_realm_nvgen, no_kerberos_local_realm_name,
	kerberos_command, KERB_LOCALREALM);
KEYWORD	(kerberos_local_realm, kerberos_local_realm_nvgen,
	kerberos_srvtab, "local-realm", "Set local realm name", PRIV_CONF);

KEYWORD	(sys_kerberos, kerberos_local_realm, ALTERNATE, "kerberos",
	"Configure Kerberos", PRIV_CONF);

#undef ALTERNATE
#define ALTERNATE sys_kerberos
