/* $Id: kparse.c,v 3.11.10.12 1996/08/28 12:54:22 thille Exp $
 * $Source: /release/112/cvs/Xsys/kerberos/csfhost/cisco/kparse.c,v $
 *------------------------------------------------------------------
 * kparse.c - All the goo to hook Kerberos to the Cisco parser
 *
 * September 1995, Che-lin Ho
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: kparse.c,v $
 * Revision 3.11.10.12  1996/08/28  12:54:22  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.11.10.11  1996/08/03  04:05:33  che
 * CSCdi65047:  Kerberos SRVTAB keys not encrypted
 * Branch: California_branch
 * Add support for encrypting SRVTAB keys if private DES key exists in
 * Private NVRAM.
 *
 * Revision 3.11.10.10  1996/06/28  23:20:17  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.11.10.9  1996/06/27  16:36:06  hampton
 * Keep your mitts off the scheduler private data structures.  [CSCdi61518]
 * Branch: California_branch
 *
 * Revision 3.11.10.8  1996/06/21  00:07:25  che
 * CSCdi60925:  exec cannot utilize kerberos
 * Branch: California_branch
 * Fix kerberos_stackcheck() to work on platforms other than 68k.
 *
 * Revision 3.11.10.7  1996/06/18  10:41:19  che
 * CSCdi60680:  bogus kerberos credentials forward config line appears
 * Branch: California_branch
 * Fix bonehead Kerberos bug.
 *
 * Revision 3.11.10.6  1996/06/04  08:35:46  che
 * CSCdi57424:  router crashes attempting kerberos telnet
 * Branch: California_branch
 * Added kerberos_stackcheck() registry to ensure correct stacksize of
 * process running kerberos clients.
 *
 * Revision 3.11.10.5  1996/06/01  12:21:15  che
 * CSCdi59251:  kerberos credential forward config problem
 * Branch: California_branch
 * Must not check csb->sense and set values during NVGEN case.
 *
 * Revision 3.11.10.4  1996/05/31  19:21:07  che
 * CSCdi55812:  kerberos srvtab remote gives multiple entries
 * Branch: California_branch
 * Add code to check for duplicate SRVTAB entries.
 *
 * Revision 3.11.10.3  1996/05/21  09:54:02  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.11.10.2  1996/04/19  15:12:15  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commmit
 *
 * Revision 3.1.2.11  1996/04/16  03:54:33  che
 * Branch: IosSec_branch
 * Code changes mandated by code review comments.
 *
 * Revision 3.1.2.10  1996/04/09  20:51:22  che
 * Branch: IosSec_branch
 * Sync to California_branch (960405)
 *
 * Revision 3.1.2.9  1996/04/02  08:56:48  carrel
 * cvs fixes
 *
 * Revision 3.1.2.8  1996/03/28  08:44:41  che
 * Branch: IosSec_branch
 * Introduce function to release memory in conn->proto.tn.kerberos_info
 *
 * Revision 3.1.2.7  1996/03/27  23:31:37  che
 * Branch: IosSec_branch
 * More Kerberos authentication cleanup
 *
 * Revision 3.1.2.6  1996/03/19  08:15:53  che
 * Branch: IosSec_branch
 * Sync to V111_1_3
 *
 * Revision 3.1.2.5  1996/03/12  03:12:31  che
 * Branch: IosSec_branch
 * Phase 2 Kerberos commit Part 2
 *
 * Revision 3.1.2.4  1996/03/05  04:46:50  che
 * Branch: IosSec_branch
 * Kerberos Phase 2 commit.
 *
 * Revision 3.1.2.3  1996/02/21  03:38:00  che
 * Branch: IosSec_branch
 * Sync to 11.1 Mainline (02/20/96)
 *
 * Revision 3.1.2.2  1996/01/23  05:57:28  che
 * Branch: IosSec_branch
 * Sync with 11.1 mainline. (960122)
 *
 * Revision 3.1.2.1  1995/12/19  04:00:18  che
 * Branch: IosSec_branch
 * Re-sync to Arkansas to fix things that broke.
 *
 * Revision 3.11.10.1  1996/03/18  20:31:43  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.7.4.3  1996/03/16  07:01:14  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.7.4.2  1996/03/07  10:02:30  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.7.4.1  1996/02/20  01:05:57  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.11  1996/02/22  14:35:34  widmer
 * CSCdi49281:  make all parser_extension_request static const
 * make parser_extension_request typedef const
 * make all parser_extension_request static
 *
 * Revision 3.10  1996/02/16  00:48:35  hampton
 * Migrate files out of the parser directory.  [CSCdi49139]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.9  1996/02/07  16:13:34  widmer
 * CSCdi48285:  Remove function name from bad_parser_subcommand
 *
 * Revision 3.8  1996/02/06  23:34:15  che
 * CSCdi48098:  Need to be able to configure pre-auth type for Kerberos
 * messages
 * Added "[no] kerberos preauth [none|encrypted-unix-timestamp]"
 * config command.
 *
 * Revision 3.7  1996/01/13  01:33:58  che
 * CSCdi46854:  Kerberos KDC ports should be definable for each KDC server
 *
 * Revision 3.6  1996/01/06  00:39:04  che
 * CSCdi46102:  Clearing non-existent Kerberos creds crashes router
 * Make sure tty->ustruct is a valid pointer to the ustruct pointer
 * before attempting to clear the creds in it.
 *
 * Revision 3.5  1996/01/04  22:51:22  che
 * CSCdi46133:  Kerberos KDC port should be configurable
 * Added "kerberos kdc-port <port number>" command.
 *
 * In send2kdc.c:
 *
 * Will now wait a minimum of 15 seconds for KDC reply instead of 5 ms.
 *
 * Revision 3.4  1995/12/24  09:35:21  che
 * CSCdi46019:  Store Kerberos credentials in Kerberos library cred cache
 * Use Kerberos library credentials cache structures and functions to
 * manipulate user credentials cache.
 *
 * Revision 3.3  1995/12/17  18:31:26  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.2  1995/11/17  17:45:51  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:19:47  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include <csfc5/header.h>
#include <csf/header.h>

#include "master.h"
#include "types.h"
#include "../ui/common_strings.h"
#include "sys_registry.h"
#include "../h/auth_registry.regh"
#include "../h/cs_registry.regh"
#include "subsys.h"
#include "address.h"
#include "config.h"
#include "parser.h"
#include "ttysrv.h"
#include "logger.h"
#include "../ui/debug.h"
#include "../os/os_debug.h"
#include "../os/clock.h"
#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "../parser/msg_parser.c"	/* Not a typo, see logger.h */
#include "kparse.h"
#include "aaa.h"
#include "file.h"
#include "../ip/ip.h"
#include "../tcp/telnet.h"
#include "stacks.h"
#include "sched.h"
#include "../os/nv.h"

/* Fix damage caused by ip.h */
#ifdef tstamp
#undef tstamp
#endif

/* 
 * GLOBAL:  krb_instance_map:  table to map kerberos instances to priv levels
 *	    srvtabsize:  max size of srvtab workspace buffer.
 *	    keytab:  loacation of router's keytab.
 *	    use_kerb_clients:  Must we use Kerberized clients? 1 = yes, 0 = no
 *	    replay_cache:  replay cache for host/router@REALM
 *	    do_forwarding:  Should we forward credentials? 1 = yes, 0 = no
 */
instance_map *krb_instance_map = NULL;
static const ulong srvtabsize = 1000L;
csfC5_kt *keytab = NULL;
int use_kerb_clients = 0;
csfC5_rc *replay_cache = NULL;
boolean do_forwarding = FALSE;
int kerb_preauth_method = CSFC5c_PADATA_T_NONE;

static void kerberos_command (parseinfo *);

/*
 * Subsystem header
 */
#define KERBEROS_MAJVERSION   1
#define KERBEROS_MINVERSION   0
#define KERBEROS_EDITVERSION  1
SUBSYS_HEADER(kerberos, KERBEROS_MAJVERSION, KERBEROS_MINVERSION,
KERBEROS_EDITVERSION,
        kerberos_subsys_init, SUBSYS_CLASS_PROTOCOL,
	NULL,
	"req: ntp, iphost");

/*
 * The Kerberos parse chain
 */
#define ALTERNATE NONE
#include "../../include/cfg_kerberos.h"
LINK_POINT(kerberos_commands, ALTERNATE);
#undef ALTERNATE

#define ALTERNATE NONE
#include "../../include/exec_show_kerberos.h"
LINK_POINT(kerberos_show_commands, ALTERNATE);
#undef ALTERNATE

#define ALTERNATE NONE
#include "../../include/exec_clear_kerberos.h"
LINK_POINT(kerberos_clear_commands, ALTERNATE);
#undef ALTERNATE

#define ALTERNATE NONE
#include "../../include/exec_debug_kerberos.h"
LINK_POINT(kerberos_debug_commands, ALTERNATE);
#undef ALTERNATE

static parser_extension_request kerberos_chain_init_table[] = {
    { PARSE_ADD_CFG_TOP_CMD, &pname(kerberos_commands) },
    { PARSE_ADD_SHOW_CMD, &pname(kerberos_show_commands) },
    { PARSE_ADD_CLEAR_CMD, &pname(kerberos_clear_commands) },
    { PARSE_ADD_DEBUG_CMD, &pname(kerberos_debug_commands) },
    { PARSE_LIST_END, NULL }
};

/*
 * kerberos_is_running()
 *	Returns TRUE if Kerberos is installed and configured.  
 * 	Returns false otherwise.
 */
boolean
kerberos_is_running()
{
    char *p = NULL;

    if (csfC5default_HostRealm(&p) == CSF_STS_OK) {
	free(p);
	return(TRUE);
    }
    free(p);
    return(FALSE);
}

/*
 * kerberos_stackcheck()
 *	Returns TRUE if current processs has KERBEROS_STACK_SIZE amout of stack.
 * 	Returns false otherwise.
 */
boolean
kerberos_stackcheck()
{
    ulong stacksize;

    if (!process_get_stacksize(THIS_PROCESS, &stacksize)) {
	return(FALSE);
    }
    if (stacksize < (wtob(KERBEROS_STACK_SIZE) * STACK_SIZE_SCALE)) {
	return(FALSE);
    }
    return(TRUE);
}

/*
 * kerberos_use_kerb_clients()
 *	Returns TRUE if use_kerb_clients is set to 1(mandatory).
 * 	Returns false otherwise.
 */
boolean
kerberos_use_kerb_clients()
{
    if (use_kerb_clients) {
	return(TRUE);
    } else {
	return(FALSE);
    }
}

/*
 * cfg_conf()
 *	Add realm -> host entries, ala the krb.conf file
 */
static void
cfg_conf (parseinfo *csb, char *realm, ipaddrtype host, int port)
{
    char hostaddr[ADDRLEN_LAT];

    if (csb->nvgen) {
	int x;

	for (x = 0; ; x += 1) {
	    char *realmp, *hostp;
	    int portp;

	    if (peek_krbhost_entry(x, &realmp, &hostp, &portp)) {
		break;
	    }
	    nv_write(TRUE, "%s %s %s", csb->nv_command, realmp, hostp);
	    if (portp != KERBEROS_KDC_PORT) {
		nv_add(TRUE, " %d", portp);
	    }
	}
	return;
    }
    sprintf(hostaddr, "%i\0", host);
    if (csb->sense) {
	if (add_krbhost_entry(realm, hostaddr, port)) {
	    printf("Could not add Kerberos conf entry\n");
	}
    } else {
	(void)del_krbhost_entry(realm, hostaddr, port);
    }
}

/*
 * cfg_realms()
 *	Configure hostname/domain -> realm mapping
 */
static void
cfg_realms (parseinfo *csb, char *dns, char *realm)
{
    if (csb->nvgen) {
	int x;

	for (x = 0; ; x += 1) {
	    char *dnsp, *realmp;

	    if (peek_host_realm(x, &dnsp, &realmp)) {
		break;
	    }
	    nv_write(TRUE, "%s %s %s", csb->nv_command, dnsp, realmp);
	}
	return;
    }
    if (csb->sense) {
	if (add_host_realm(dns, realm)) {
	    printf("Could not add Kerberos realms entry\n");
	}
    } else {
	(void)del_host_realm(dns, realm);
    }
}

/* 
 * Parses a binary srvtab file.  srvtab file structure is:
 *	Keytab version number		2 byte
 *	Keytab entry size		4 bytes
 *	Number of principal components	2 bytes
 *	Size of realm name		2 bytes
 *	Realm name			Size of realm name
 *	Size of prinicipal component #n	2 bytes
 *	Principal component #n		Size of principal component #n
 *	Version number			4 bytes
 *	Timestamp			4 bytes
 *	Entry version number		1 byte
 *	Key type			2 bytes
 *	Key length			2 bytes
 *	Key data			Key length
 */
static boolean
parse_srvtab(char *buffer, ulong buffsize)
{
    csfC5_ktEntry kte, found_kte;
    char *workspace = NULL;
    char *realm = NULL, *princ_string = NULL;
    csfC5_crsEType ktvno = 0;
    ulong entry_size = 0L;
    csfC5_crsEType size = 0;
    csfC5_crsEType realm_size = 0;
    csfC5_crsEType count = 0;
    csfC5_crsEType counter = 0;
    short key_length = 0;

    csfC5clear_ktEntry(&kte);
    csfC5clear_ktEntry(&found_kte);

    workspace = buffer;
    memcpy(&ktvno, workspace, sizeof(short));
    workspace+=sizeof(short);
    if (workspace > buffer+buffsize) {
	printf("Truncated srvtab!  Discarding...\n");
	return(FALSE);
    }

    if ((ktvno != 0x0501) && (ktvno != 0x0502)) {
	printf("Srvtab retrieved is not a Kerberos V5 srvtab!  
		Discarding...\n");
	return(FALSE);
    }
    
    /* workspace now pointing to `entry size'. */
    do {
	memcpy(&entry_size, workspace, sizeof(int));
	workspace+=sizeof(int);
	if ((workspace > buffer+buffsize) || 
	    (workspace+entry_size > buffer+buffsize)) {
	    printf("Truncated srvtab!  Discarding...\n");
	    return(FALSE);
	}
	
	memcpy(&count, workspace, sizeof(short));
	workspace+=sizeof(short);
	if (workspace > buffer+buffsize) {
	    printf("Truncated srvtab!  Discarding...\n");
	    return(FALSE);
	}
	if (ktvno == 0x501) {
	    count--;	/* V1 includes the realm in the count. */
	}
	if (count <= 0) {
	    printf("No principals in srvtab!  Discarding...\n");
	    return(FALSE);
	}

	/* Get the realm name. */
	memcpy(&realm_size, workspace, sizeof(short));
	workspace+=sizeof(short);
	if (workspace > buffer+buffsize) {
	    printf("Truncated srvtab!  Discarding...\n");
	    return(FALSE);
	}
	
	if (realm_size <= 0) {
	    printf("No realm name in srvtab!  Discarding...\n");
	    return(FALSE);
	}

	realm = malloc_named(realm_size+1, "Kerberos:parse_srvtab()");
	if (realm == NULL) {
	    printf("Cannot allocate space for realm name!  Discarding...\n");
	    return(FALSE);
	}
	memcpy(realm, workspace, realm_size);
	workspace+=realm_size;
	if (workspace > buffer+buffsize) {
	    printf("Truncated srvtab!  Discarding...\n");
	    return(FALSE);
	}
	realm[realm_size]='\0';

	/* Got realm name.  Get components of principal and mash them 
	   together. */
	princ_string = malloc_named(entry_size, "Kerberos:parse_srvtab()");
	if (princ_string == NULL) {
	    printf("Cannot allocate space for principal name!  
		    Discarding...\n");
	    return(FALSE);
	}

	for (counter=0;counter<count;counter++) {
	    memcpy(&size, workspace, sizeof(short));
	    workspace+=sizeof(short);
	    if (workspace > buffer+buffsize) {
		printf("Truncated srvtab!  Discarding...\n");
		return(FALSE);
	    }

	    if (size <= 0) {
		printf("Missing principal component in srvtab entry!  
			Discarding...\n");
		return(FALSE);
	    }

	    if (counter != 0) {
		strncat(princ_string, "/\0", sizeof(short));
	    }

	    strncat(princ_string, workspace, size);
	    workspace+=size;
	    if (workspace > buffer+buffsize) {
		printf("Truncated srvtab!  Discarding...\n");
		return(FALSE);
	    }
	}
	strncat(princ_string, "@\0", sizeof(short));
	strncat(princ_string, realm, realm_size);
	free(realm);

	/* Okay.  We got a principal string.  Convert it into a principal ID. */
	csfC5strTo_prnId(princ_string, &kte.prnid);
	free(princ_string);

	/* Get principal type, if it exists. */
	if (ktvno != 0x501) {
	    memcpy(&(kte.prnid.name.prn_type), workspace, sizeof(int));
	    workspace+=sizeof(int);
	    if (workspace > buffer+buffsize) {
		printf("Truncated srvtab!  Discarding...\n");
		return(FALSE);
	    }
	}

	/* Get the timestamp. */
	memcpy(&(kte.tstamp), workspace, sizeof(int));
	workspace+=sizeof(int);
	if (workspace > buffer+buffsize) {
	    printf("Truncated srvtab!  Discarding...\n");
	    return(FALSE);
	}

	/* Get Key version number. */
	memcpy(&(kte.kvno), workspace, 1);
	workspace+=1;
	if (workspace > buffer+buffsize) {
	    printf("Truncated srvtab!  Discarding...\n");
	    return(FALSE);
	}

	/* Get Key type. */
	memcpy(&(kte.key.key_type), workspace, sizeof(short));
	workspace+=sizeof(short);
	if (workspace > buffer+buffsize) {
	    printf("Truncated srvtab!  Discarding...\n");
	    return(FALSE);
	}

	/* Get Key Length. */
	memcpy(&key_length, workspace, sizeof(short));
	workspace+=sizeof(short);
	if (workspace > buffer+buffsize) {
	    printf("Truncated srvtab!  Discarding...\n");
	    return(FALSE);
	}
	kte.key.key_data.dl = (csf_size_t) key_length;

	/* Get Key data. */
	kte.key.key_data.dp = malloc_named(kte.key.key_data.dl,
						   "Kerberos:parse_srvtab()");
	if (kte.key.key_data.dp == NULL) {
	    printf("Cannot allocate space for key data!  Discarding...\n");
	    return(FALSE);
	}

	memcpy((char *) kte.key.key_data.dp, workspace, kte.key.key_data.dl);
	workspace+=kte.key.key_data.dl;
	if (workspace > buffer+buffsize) {
	    printf("Truncated srvtab!  Discarding...\n");
	    return(FALSE);
	}

	/* Check to see if we already have this entry in our SRVTAB */
	if (csfC5getEntry_kt(keytab, &kte.prnid, kte.kvno, &found_kte) 
	    == CSF_STS_OK) {
	    csfC5free_ktEntry(&found_kte);
	    printf("Duplicate SRVTAB entry!  Discarding...\n");
	} else {
	    /* Add keytab entry into list. */
	    if (csfC5addEntry_kt(keytab, &kte) != CSF_STS_OK) {
		printf("Failed to add srvtab entry!!!\n");
		return(FALSE);
	    }
	}
    } while (workspace < buffer+buffsize);

    return(TRUE);
}

static boolean
cfg_srvtab_remote(ipaddrtype host, char *fname)
{
    ulong buffsize = 0L;
    uchar *buffer = NULL;
    filetype dst;
    boolean verbose = TRUE;
    int success = 0;

    /* Initialize the file descriptor of the remote end */
    dst.next = NULL;
    dst.filename = fname;
    dst.resetsubr = NULL;
    dst.responder = FALSE;
    dst.proto = FILE_ACCESS_TFTP;
    dst.ps.tuba.boothost.type = ADDR_IP;
    dst.ps.tuba.boothost.ip_addr = host;
    dst.ps.tuba.display = TRUE;

    if (old_access_control) {
	dst.ps.tuba.username = stdio->user ? stdio->user : hostname;
    } else {
	if (!aaa_get_user_p(stdio->ustruct, &(dst.ps.tuba.username))) {
	    dst.ps.tuba.username = hostname;
	}
    }

    /* Initialize all other stuff. */
    buffsize = srvtabsize;
    buffer = malloc_named(buffsize, "Kerberos:cfg_srvtab_remote()");
    if (buffer == NULL) {
	printf(nomemory);
	return(FALSE);
    }

    /* Yank remote file into local buffer. */
    success = reg_invoke_file_read(dst.proto, &dst, buffer, &buffsize, 
				   verbose, NULL, NULL);
    if (success == ERROR_OK) {
	if (parse_srvtab(buffer, buffsize)) {
	    free(buffer);
	    return(TRUE);
	} else {
	    free(buffer);
	    return(FALSE);
	}
    } else {
	free(buffer);
	return(FALSE);
    }
}

/* SEE cfg_kerberos.h for definitions of OBJ(). */

static boolean
kt_decode(parseinfo *csb, csfC5_ktEntry *kte, char *encoded_key)
{
    boolean ok = FALSE;
    uchar *keydata = NULL;
    uint keylen = 0;

    /* Make sure timestamp field is valid. */
    if (!t_number(GETOBJ(string,3))) {
	printf("Invalid timestamp field!\n");
	return(FALSE);
    }

    /* Encode and save principal. */
    if (csfC5strTo_prnId((char *) GETOBJ(string,2), &(kte->prnid)) 
	!= CSF_STS_OK) {
	printf("Failed to encode principal!\n");
	return(FALSE);
    }

    /* Save principal type. */
    kte->prnid.name.prn_type = (csfC5_prnNType) GETOBJ(int,2);

    /* Save timestamp. */
    kte->tstamp = parse_decimal_long((char *) GETOBJ(string,3), &ok);
    if (kte->tstamp == (long) -1) {
	printf("Invalid timestamp field!\n");
	return(FALSE);
    }

    /* Save key version number. */
    kte->kvno = (csfC5_crsKVNo) GETOBJ(int,3);

    /* Save keytype. */
    kte->key.key_type = (csfC5_crsKType) GETOBJ(int,4);

    /* Save keylen. */
    kte->key.key_data.dl = (csf_size_t) GETOBJ(int,5);

    /* Save encoded key. */
    keydata = malloc_named(kte->key.key_data.dl, "Kerberos:kt_decode()");
    if (keydata == NULL) {
	return(FALSE);
    }
    kte->key.key_data.dp = (void *) keydata;

    /*
     * Kludge ALERT!!!  The first byte in 'encoded_key' defines whether or
     * not we used the privately stored DES key to encrypt the keytab key
     * in the config.  1=DES+BCD, 0=BCD only.
     */
    if (encoded_key[0] == '1') {
	/* 
	 * Encrypted with the private DES key.  Decrypt the sucker and
	 * store it in our keytab structure.
	 */
	keylen = kte->key.key_data.dl * 2;
	if (!nv_decrypt(&(encoded_key[1]), &keylen)) {
	    printf("Cannot decrypt SRVTAB key!\n");
	    return(FALSE);
	}

	if (keylen != kte->key.key_data.dl) {
	    printf("Failed to decrypt SRVTAB key correctly!\n");
	    return(FALSE);
	}

	memcpy(keydata, &(encoded_key[1]), keylen);
    } else {
	/*
	 * No private DES key available.  Hash encode the keytab key and
	 * write it out.
	 */
	chartobcd(&(encoded_key[1]), keydata, kte->key.key_data.dl*2);
    }

    return(TRUE);
}

/* SEE cfg_kerberos.h for definitions of OBJ(). */

static void
cfg_srvtab (parseinfo *csb)
{
    csfC5_ktCursor curs;
    csfC5_ktEntry kte, found_kte;
    char buffer[100];
    csfC5_prnId prnid;
    char *principal = NULL;
    char *hexdata = NULL;
    char *enc_data = NULL;

    buffer[0]='\0';
    csfC5clear_ktEntry(&kte);
    csfC5clear_ktEntry(&found_kte);
    csfC5clear_prnId(&prnid);

    if (csb->nvgen) {
	csfC5startGetEntry_kt(keytab, &curs);
	while (csfC5nextGetEntry_kt(keytab, &curs, &kte) == CSF_STS_OK) {

	    /* Convert prnid to principal string. */
	    if (csfC5toStr_prnId(&kte.prnid, &principal) != CSF_STS_OK) {
		printf("Failed to unparse prnid!\n");
		free(principal);
		return;
	    }

	    nv_write(TRUE, "%s entry %s %ld %ld %ld %d %d ", csb->nv_command, 
	    	     principal, kte.prnid.name.prn_type, kte.tstamp, kte.kvno, 
		     kte.key.key_type, kte.key.key_data.dl);
	    free(principal);

	    hexdata = malloc_named((kte.key.key_data.dl*2)+2,
				    "Kerberos:cfg_srvtab()");
	    if (!hexdata) {
		if (kerberos_debug)
		    buginf("\nKerberos:\tFailed to write SRVTAB key");
		return;
	    }

	    enc_data = malloc_named(kte.key.key_data.dl,
				    "Kerberos:cfg_srvtab()");
	    if (!enc_data) {
		if (kerberos_debug)
		    buginf("\nKerberos:\tFailed to write SRVTAB key");
		free(hexdata);
		return;
	    }

	    if (nv_there_are_keys()) {
		/* Encrypt key. */
		/* first byte has to be '1'. */

		hexdata[0] = '1';
		memcpy(enc_data, kte.key.key_data.dp, kte.key.key_data.dl);
		if (!nv_encrypt(enc_data, kte.key.key_data.dl)) {
		    printf("Failed to encrypt SRVTAB key!  Encoding it!\n");
		    hexdata[0] = '0';
		    memcpy(enc_data, kte.key.key_data.dp, kte.key.key_data.dl);
		}
	    } else {
		/* Encode key. */
		/* first byte has to be '0'. */

		hexdata[0] = '0';
		memcpy(enc_data, kte.key.key_data.dp, kte.key.key_data.dl);
	    }

	    bcdtochar(enc_data, &(hexdata[1]), kte.key.key_data.dl*2);
	    hexdata[(kte.key.key_data.dl*2)+1] = '\0';
	    nv_add(TRUE, "%s", hexdata);
	    free(hexdata);
	    free(enc_data);
	}
	csfC5endGetEntry_kt(keytab, &curs);
	return;
    }

    if (csb->sense) {
	if (keytab == NULL) {
	    if (csfC5resolve_kt(&keytab, "MEM:") != CSF_STS_OK) {
		printf("Cannot allocate memory for srvtab!!!\n");
		return;
	    }
	}

	if (GETOBJ(int,1)) {
	    if (!kt_decode(csb, &kte, GETOBJ(string,4))) {
		printf("Failed to decode srvtab entry!\n");
		return;
	    }

	    /* Check to see if we already have this entry in our SRVTAB */
	    if (csfC5getEntry_kt(keytab, &kte.prnid, kte.kvno, &found_kte) 
		== CSF_STS_OK) {
		csfC5free_ktEntry(&found_kte);
		printf("Duplicate SRVTAB entry!  Discarding...\n");
		return;
	    }

	    if (csfC5addEntry_kt(keytab, &kte) != CSF_STS_OK) {
		printf("Failed to add srvtab entry!!!\n");
		return;
	    }
	} else {
	    /* Get keytabs from remote host */
	    if (!cfg_srvtab_remote((ipaddrtype) GETOBJ(paddr,1)->ip_addr, 
				   (char *) GETOBJ(string,1))) {
		printf("Failed to retrieve srvtab from remote host %i\n",
			(ipaddrtype) GETOBJ(paddr,1)->ip_addr);
	    }
	}
    } else {
	
	/* 
	 * If true, then we got the "no kerberos srvtab remote" command.
	 * Ignore it.
	 */
	if (GETOBJ(int,1) == 0)
	    return;

	/* Encode and save principal. */
	if (csfC5strTo_prnId((char *) GETOBJ(string,2),&prnid) != CSF_STS_OK) {
	    printf("Failed to encode principal!\n");
	    csfC5free_prnId(&prnid);
	    return;
	}

	if (csfC5getEntry_kt(keytab, &prnid, (int) GETOBJ(int,2), &kte) 
	    != CSF_STS_OK) {
	    printf("Failed to remove srvtab entry!!!\n");
	    csfC5free_prnId(&prnid);
	    csfC5free_ktEntry(&kte);
	    return;
	}
	if (csfC5removeEntry_kt(keytab, &kte) != CSF_STS_OK) {
	    printf("Failed to remove srvtab entry!!!\n");
	    csfC5free_prnId(&prnid);
	    csfC5free_ktEntry(&kte);
	    return;
	}
	csfC5free_prnId(&prnid);
	csfC5free_ktEntry(&kte);
    }
}

/*
 * cfg_loc_realm()
 *	[un]Configure local realm name
 */
static void
cfg_loc_realm (parseinfo *csb, char *realm)
{
    char *p = NULL;

    if (csb->nvgen) {
	if (csfC5default_HostRealm(&p) == 0) {
	    nv_write(p != NULL, "%s %s", csb->nv_command, p);
	}
	free(p);
	return;
    }
    if (csfC5_set_default_realm(csb->sense ? realm : NULL) != 0) {
	printf("\n%%Unable to %s local realm name",
	    csb->sense ? "set" : "clear");
    }
}

/*
 * print_time()
 * 	Prints out date in string format
 */
void
print_time (csfC5_tStamp tv)
{
    static  char *Month_names[] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun",
				    "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
    };

    struct timeval_ stime;
    char *tz_name = NULL;

    unix_time_to_timeval(tv, &stime, &tz_name);
    printf("%2d-%s-%4d %02d:%02d:%02d",
	   stime.day,
	   Month_names[stime.month-1],
	   stime.year,
	   stime.hour,
	   stime.minute,
	   stime.second);
}

/*
 * kerb_show_creds()
 *	Show user credentials
 */
static void
kerb_show_creds (parseinfo *csb)
{
    tt_soc *tty = NULL;
    char *sname = NULL, *pname = NULL;
    csf_sts rsts = 0;
    struct timeval_ stime;
    char *tz_name = NULL;
    char timestring[TIMESTRINGSIZE];
    csfC5_ccCursor curs;
    csfC5_creds kerb_creds;
    boolean first = TRUE;

    csfC5clear_creds(&kerb_creds);

    tty = MODEMS[csb->line1];

    if ((tty->ustruct) && (tty->ustruct->cred)) {
	rsts = csfC5toStr_prnId((csfC5_prnId *)
				&((csfC5_cc *)tty->ustruct->cred->cred)->prnid,
				&pname);

	if (rsts) {
	    printf("Cannot unparse server name!\n");
	    return;
	}

	csfC5startGetCreds_cc((csfC5_cc *)tty->ustruct->cred->cred, &curs);
	while (csfC5nextGetCreds_cc((csfC5_cc *)tty->ustruct->cred->cred,
				    &curs, &kerb_creds) == CSF_STS_OK) {
	    rsts = csfC5toStr_prnId((csfC5_prnId *) &(kerb_creds.server),
				    &sname);
	    if (rsts) {
		printf("Cannot unparse server name!\n");
		return;
	    }

	    /* Print out relevant information */
	    if (first) {
		printf("Default Principal:  %s\n", pname);
		printf("Valid Starting\t\tExpires\t\t\tService Principal\n");
		first = FALSE;
	    }

    	    unix_time_to_timeval(kerb_creds.times.tstart,
				 &stime, &tz_name);
	    format_time(timestring,TIMESTRINGSIZE, "%d-%tn-%Y %H:%M:%S",
			&stime, tz_name);
	    printf("%s\t",timestring);
    	    unix_time_to_timeval(kerb_creds.times.tend,
				 &stime, &tz_name);
	    format_time(timestring,TIMESTRINGSIZE, "%d-%tn-%Y %H:%M:%S",
			&stime, tz_name);
	    printf("%s\t",timestring);
	    printf("%s\n",sname);
	}
	csfC5endGetCreds_cc((csfC5_cc *)tty->ustruct->cred->cred, &curs);
    } else
	printf("No Kerberos credentials.\n");
}

/*
 * kerb_clear_creds()
 *	Clear user credentials
 *	Called from command line.
 *	Calls csfC5close_cc() to do most of the dirty work.
 */
static void
kerb_clear_creds (parseinfo *csb)
{
    tt_soc *tty = NULL;

    tty = MODEMS[csb->line1];
    if ((tty->ustruct) && (tty->ustruct->cred)) {
	if (tty->ustruct->cred->ref_cnt > 1)
	    tty->ustruct->cred->ref_cnt--;
	else {
	    csfC5close_cc((csfC5_cc **) &tty->ustruct->cred->cred);
	    free(tty->ustruct->cred);
	    tty->ustruct->cred = NULL;
	}
    } else {
	printf("No Kerberos credentials to free!\n");
    }
}

void
cfg_use_kerb(parseinfo *csb, int mandatory)
{
    if (csb->nvgen) {
	if (use_kerb_clients) {
	    nv_write(TRUE,"%s mandatory", csb->nv_command);
	}
	return;
    }
    if (csb->sense) {
	use_kerb_clients = mandatory;
    } else {
	use_kerb_clients = 0;
    }
}

void
cfg_do_forwarding(parseinfo *csb)
{
    if (csb->nvgen) {
	if (do_forwarding) {
	    nv_write(TRUE,"%s", csb->nv_command);
	}
	return;
    }
    if (csb->sense) {
	do_forwarding = TRUE;
    } else {
	do_forwarding = FALSE;
    }
}

void
cfg_kerberos_instance(parseinfo *csb, char *instance_name, int priv_level)
{
    instance_map *temp_map = krb_instance_map, *temp_map2 = NULL;

    if (csb->nvgen) {
	while (temp_map != NULL) {
	    nv_write((int) temp_map, "%s %s %d", csb->nv_command,
		 temp_map->instance_name, temp_map->priv_level);
	    temp_map = temp_map->next;
	}
	return;
    }
    if (csb->sense) {
	/* Add an instance_name map entry. */
	
	while (temp_map != NULL) {
	    if (!strncmp(temp_map->instance_name, instance_name, 
			 strlen(temp_map->instance_name))) {
		/* Entry already exists.  Just need to update priv level. */
		temp_map->priv_level = priv_level;
		return;
	    }
	    temp_map2 = temp_map;
	    temp_map = temp_map->next;
	}

	/* Entry does not exist. */
	temp_map = malloc_named(sizeof(instance_map),
					"Kerberos:cfg_kerberos_instance()");
	temp_map->instance_name = strdup(instance_name);
	temp_map->priv_level = priv_level;
	temp_map->next = NULL;
	
	if (krb_instance_map == NULL) {

	    /* This is the first entry. */
	    krb_instance_map = temp_map;
	    return;
	} else {

	    /* Add new entry to end of list. */
	    temp_map2->next = temp_map;
	    return;
	}
    } else {
	/* Match the instance_name and remove it from the list. */

	if (temp_map == NULL) {
	    /* List is empty.  No work to do. */
	    printf("No entries in instance map!\n");
	    return;
	}

	/* List is not NULL. */
	while (temp_map != NULL) {
	    if (!strncmp(temp_map->instance_name, instance_name, 
			 strlen(temp_map->instance_name))) {
		if (temp_map2 == NULL) {
		    /* Match is first element. */
		    krb_instance_map = temp_map->next;
		    free(temp_map);
		    return;
		} else {
		    /* Match is not first element. */
		    temp_map2->next = temp_map->next;
		    free(temp_map);
		    return;
		}
	    }
	    temp_map2 = temp_map;
	    temp_map = temp_map->next;
	}
    }
}


/*
 * cfg_preauth(int patype)
 *
 *	Defines preauth method to use.
 */
static void
cfg_preauth(parseinfo *csb, int patype)
{
    if (csb->nvgen) {
	if (kerb_preauth_method == CSFC5c_PADATA_T_NONE)
	    return;

	nv_write(TRUE, "%s ", csb->nv_command); 
	switch(kerb_preauth_method) {
	    case CSFC5cx_PADATA_T_ENC_UNIX_TIMESTAMP:
			nv_add(TRUE, "encrypted-unix-timestamp");
			break;
	    default:	printf("Invalid preauth type.  Ignoring...\n");
	}
	return;
    }
    if (csb->sense) {
	kerb_preauth_method = patype;
    } else {
	kerb_preauth_method = CSFC5c_PADATA_T_NONE;
    }
}


/*
 * kerberos_command()
 *	Process configuration commands for Kerberos
 */
static void
kerberos_command (parseinfo *csb)
{
    switch (csb->which) {
    case SHOW_KERBEROS:
	kerb_show_creds(csb);
	break;
    case CLEAR_KERBEROS:
	kerb_clear_creds(csb);
	break;
    case KERB_CONF:
	cfg_conf(csb, GETOBJ(string,1), GETOBJ(paddr,1)->ip_addr, 
		 GETOBJ(int, 1));
	break;
    case KERB_REALMS:
	cfg_realms(csb, GETOBJ(string,1), GETOBJ(string,2));
	break;
    case KERB_SRVTAB:
	cfg_srvtab(csb);
	break;
    case KERB_INST:
	cfg_kerberos_instance(csb, GETOBJ(string,1), GETOBJ(int,1));
	break;
    case KERB_LOCALREALM:
	cfg_loc_realm(csb, GETOBJ(string,1));
	break;
    case KERB_CLIENTS:
	cfg_use_kerb(csb, GETOBJ(int,1));
	break;
    case KERB_DO_FOR:
	cfg_do_forwarding(csb);
	break;
    case KERB_PREAUTH:
	cfg_preauth(csb, GETOBJ(int,1));
	break;
    default:
	bad_parser_subcommand(csb, csb->which);
	break;
    }
}


/*
 * kerberos_subsys_init()
 *	Wire up parser stuff for kerberos, register functions
 */
void
kerberos_subsys_init (subsystype *subsys)
{
    kerberos5_is_installed = TRUE;
    reg_add_start_login(AAA_ACC_METH_KRB5, kerberos_start_login,
			"kerberos_start_login");
    reg_add_cont_login(AAA_ACC_METH_KRB5, kerberos_cont_login,
			"kerberos_cont_login");
    reg_add_kerberos_is_running(kerberos_is_running, "kerberos_is_running");
    reg_add_kerberos_stackcheck(kerberos_stackcheck, "kerberos_stackcheck");
    reg_add_kerberos_client_auth(kerberos_client_auth, "kerberos_client_auth");
    reg_add_kerberos_server_auth(kerberos_server_auth, "kerberos_server_auth");
    reg_add_do_author(AAA_AUTHOR_METH_KRB5_INSTANCE, kerberos_do_author,
		      "kerberos_do_author");
    reg_add_kerberos_use_kerb_clients(kerberos_use_kerb_clients, 
			"kerberos_use_kerb_clients");
    reg_add_telnet_auth(KERBEROS_V5, kerberos_telnet_auth, 
			"kerberos_telnet_auth");
    reg_add_kerberos_free_kerberos_info(kerberos_free_kerberos_info,
			"kerberos_free_kerberos_info");
    parser_add_command_list(kerberos_chain_init_table, "kerberos");
}

