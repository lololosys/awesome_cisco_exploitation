/* $Id: crypto_pubkey.c,v 3.1.60.8 1996/08/07 09:21:12 xliu Exp $
 * $Source: /release/112/cvs/Xsys/crypto/crypto_pubkey.c,v $
 *------------------------------------------------------------------
 * crypto_pubkey.c - IOS manipulation of public/private keys 
 *
 * June 1995, Brian Weis
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: crypto_pubkey.c,v $
 * Revision 3.1.60.8  1996/08/07  09:21:12  xliu
 * CSCdi63274:  Wrong crypto engine name a show cr eng conf
 * Branch: California_branch
 *
 * Revision 3.1.60.7  1996/07/01  09:05:57  xliu
 * CSCdi61744:  fail to select correct crypto maps when multiple are
 * created
 * Branch: California_branch
 *
 * Revision 3.1.60.6  1996/06/21  22:29:11  xliu
 * CSCdi61014:  Migrate to support vip2.
 * Branch: California_branch
 *
 * Revision 3.1.60.5  1996/06/18  00:21:55  bew
 * CSCdi60362:  Crypto router choose 40-des over 56-des
 * Branch: California_branch
 * The strongest DES possible is now negotiated during connection setup.
 *
 * Revision 3.1.60.4  1996/06/07  00:24:17  bew
 * CSCdi59169:  Crypto cleanup work
 * Branch: California_branch
 * Various cleanups & addition of "crypto ping".
 *
 * Revision 3.1.60.3  1996/06/05  23:11:30  bew
 * CSCdi59169:  Crypto cleanup work
 * Branch: California_branch
 * Fix crypto zeroize & key generation for 7500 with VIPs.
 * Fix crypto connection setup with traffic in both directions.
 * Fix encryption bugs.
 *
 * Revision 3.1.60.2  1996/05/12  23:12:23  xliu
 * CSCdi56023:  Fixes made after integrating with turbo IPC, and cached
 * packet mem.
 * Branch: California_branch
 *
 * Revision 3.1.60.1  1996/04/19  14:55:55  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commits
 *
 * Revision 2.1.2.22  1996/04/17  23:47:27  xliu
 * Branch: IosSec_branch
 * Incorporate code review comments and removing wrong slot assumption.
 *
 * Revision 2.1.2.21  1996/04/16  21:52:36  bew
 * Branch: IosSec_branch
 * General code cleanup. Implemented checking of crypto map peers. CIM now
 * addressed to the src/dst rather than pe/upe.
 *
 * Revision 2.1.2.20  1996/04/09  20:02:43  che
 * Branch: IosSec_branch
 * Sync to California_branch (960405)
 *
 * Revision 2.1.2.19  1996/03/13  22:45:13  bew
 * Branch: IosSec_branch
 * The RSP/VIP environment (multiple crypto engines) are now supported
 * in crypto parser routines and crypto session mgmt code.
 *
 * Revision 2.1.2.18  1996/02/29  19:03:36  bew
 * Branch: IosSec_branch
 * Commit of the private/encrypted NVRAM feature. This is in support
 * of the crypto project and allows portions of NVRAM to be
 * encrypted, and a portion to be private key storage for the IOS.
 *
 * Revision 2.1.2.17  1996/01/13  05:46:46  bew
 * Branch: IosSec_branch
 * Cleanup. Lots of cleanup in the IosSec_branch crypto directory.
 *
 * Revision 2.1.2.16  1995/12/20  23:30:49  bew
 * Branch: IosSec_branch
 * Public key exchange can now be two-way as well as one-way. Context is
 * now freed after a sign operation. Wrap-around of connection ids now
 * supported.  General cleanup.
 *
 * Revision 2.1.2.15  1995/11/30  05:57:24  xliu
 * Branch: IosSec_branch
 *
 * Integrated with the new crypto engine code.
 *
 * Revision 2.1.2.14  1995/11/28  01:46:37  bew
 * Port of newest Cylink SDU code, plus CS cleanup. Cylink files
 * are moved to match their original tree structure.
 * Branch: IosSec_branch
 *
 * Revision 2.1.2.13  1995/11/17  16:33:20  carrel
 * Branch: IosSec_branch
 * Sync to Arkansas
 *
 * Revision 2.1.2.12  1995/10/17  01:19:37  bew
 * Branch: IosSec_branch
 * Add the simple unauthenticated public key exchange protocal. Cleaned
 * up the printing of pulic keys. Modified some show commands syntax to
 * match the functional spec. General cleanup.
 *
 * Revision 2.1.2.11  1995/10/16  04:04:21  xliu
 * Branch: IosSec_branch
 * Add crypto engine parser commands.
 *
 * Revision 2.1.2.9  1995/10/10  18:19:40  bew
 * Branch: IosSec_branch
 * Implementation of connection timeouts with managed timers & the clear
 * crypto connection command.  Introduced the concept of "current
 * connection" and "new connection" for use during connection setup.
 * Also oodles of cleanup work.
 *
 * Revision 2.1.2.8  1995/10/03  18:51:27  xliu
 * Branch: IosSec_branch
 *
 * Moved epa dir under crypto dir. Renamed epa dir to crypto_engine dir.
 * Create crypto_lib under crypto_engine which include Cylink sw crypto
 * libs.
 *
 * Revision 2.1.2.7  1995/09/09  01:27:08  bew
 * Branch: IosSec_branch
 * General cleanup & bug fixing in crypto.
 *
 * Revision 2.1.2.6  1995/09/06  00:55:47  bew
 * Branch: IosSec_branch
 * epa_init() is now called whenever new DSS keys are created or read in
 * from the configuration. Also added a crypto config command to initialize
 * the EPA to replace the exec epa command. The epa command was removed
 * altogether.
 *
 * Revision 2.1.2.5  1995/09/02  05:05:26  xliu
 * Branch: IosSec_branch
 *
 * Add function to retrive dss keys when reinitializaing.
 *
 * Revision 2.1.2.4  1995/08/30  20:41:40  bew
 * Branch: IosSec_branch
 * First commit of Cylink connection setup code, plus session
 * management modifications to call that code.
 *
 * Revision 2.1.2.3  1995/08/02  19:24:33  bew
 * Branch: IosSec_branch
 * Addition of Connection Table manipulation routines for use by ACL code.
 * Cleanup of sign & verify code.
 * Storage of DSS key pair in the configuration (for now).
 *
 * Revision 2.1.2.2  1995/07/08  01:55:42  bew
 * Branch: IosSec_branch
 * Added "show crypto pubkey" commands to show a peer public key by
 * either peer name or peer serial number.
 *
 * Revision 2.1.2.1  1995/07/01  05:31:23  carrel
 * Branch: IosSec_branch
 * Initial merge of the 10.3 IosSec_branch to Arizona.
 *
 * Revision 1.1.2.1  1995/06/23  02:57:02  bew
 * Branch: IosSec_branch
 * Support the storing of public keys in the configuration.
 *
 * Revision 3.1  1995/11/09  11:13:29  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/29  22:59:54  carrel
 * Placeholders for IosSecurity.
 *
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Maintenance of the peer public key list.
 */

#include "master.h"
#include <ciscolib.h>
#include "config.h"
#include "packet.h"
#include "globs.h"
#include "parser.h"
#include "interface_private.h"
#include "../crypto/parser_defs_crypto.h"
#include "crypto_pubkey.h"
#include "../ip/ip.h"
#include "crypto.h"
#include "crypto_engine_api.h"
#include "../os/nv.h"

/*
 * Pointers associated with the public key list.
 */
crypto_pubkey *crypto_public_key_list_head = NULL;

/*
 * Pointers associated with the local public/private key list.
 */
crypto_mykey *crypto_my_key_list_head = NULL;

#define PK(csb) ((crypto_pubkey *)csb->userDataBlock[0])
#define SETPK(csb,ptr) ((crypto_pubkey *)csb->userDataBlock[0] = ptr)

#define MY_KEYS(csb) ((crypto_mykey *)csb->userDataBlock[0])
#define SET_MY_KEYS(csb,ptr) ((crypto_mykey *)csb->userDataBlock[0] = ptr)
#define WHICH_KEY(csb) ((int)csb->userDataBlock[1])

/* 
 * NOTE: Changing CRYPTO_PUB_GR_LEN must be accompanyed with changing the 
 * format in nv_add() & printf() below!
 */
#define CRYPTO_PUB_GR_LEN 	4
#define CRYPTO_PUB_LINE_LEN	80
#define CRYPTO_PUB_NUM_GR	(int) (CRYPTO_PUB_LINE_LEN/(CRYPTO_PUB_GR_LEN*2+1))

void crypto_pubkey_print_key_data (int key_length, uchar *pubkey, 
				  boolean nvgen, boolean private)
{
    int i,j,l;

    /*
     * Print groups of characters. 
     */
    for (i=0,l=0; i+CRYPTO_PUB_GR_LEN<key_length; i+=CRYPTO_PUB_GR_LEN,l++) {
	if (!(l % CRYPTO_PUB_NUM_GR)) {
	    if (nvgen) {
		if (private) {
	            nv_write_private(TRUE, " ");
		} else {
	            nv_write(TRUE, " ");
	    	}
	    } else {
	        printf("\n ");
	    }
	}
	if (nvgen) {
	    if (private) {
	    	nv_add_private(TRUE, "%02x%02x%02x%02x ", 
			pubkey[i],  pubkey[i+1], pubkey[i+2], pubkey[i+3]);
	    } else {
	        nv_add(TRUE, "%02x%02x%02x%02x ", 
		        pubkey[i],  pubkey[i+1], pubkey[i+2], pubkey[i+3]);
	    }
	} else {
	    printf("%02x%02x%02x%02x ", 
		pubkey[i],  pubkey[i+1], pubkey[i+2], pubkey[i+3]);
	}
    }
    /*
     * Cleanup any remaining characters (although real keys shouldn't have 
     * any remainder....)
     */
    for (j=i; j<key_length; j++,l++) {
	if (!(l % CRYPTO_PUB_LINE_LEN)) {
	    if (nvgen) {
		if (private) {
	            nv_write_private(TRUE, " ");
		} else {
	            nv_write(TRUE, " ");
		}
	    } else {
	        printf("\n");
	    }
	}
	if (nvgen) {
	    if (private) {
	    	nv_add_private(TRUE, "%02x", pubkey[j]); 
	    } else {
	        nv_add(TRUE, "%02x", pubkey[j]); 
	    }
	} else {
	    printf("%02x", pubkey[j]); 
	}
    }
}

void crypto_pubkey_print (uint serial, uchar *name, crypto_key *k)
{

    printf("crypto public-key %s %08x", name, serial);
    crypto_pubkey_print_key_data(k->keylen, k->key, FALSE, FALSE);
    printf("\nquit\n");
}

void crypto_pubkey_nvgen_print (int key_length, uchar *pubkey, boolean private)
{
    crypto_pubkey_print_key_data(key_length, pubkey, TRUE, private);
    if (private) {
        nv_write_private(TRUE, " quit"); 
    } else {
        nv_write(TRUE, " quit"); 
    }
}

/*
 * Retrive the name associated with the dss key generated for a 
 * crypto engine for the given slot. This name is used as the crypt 
 * engine name.
 */
uchar* crypto_get_key_name_by_slot (ushort slot)
{
    crypto_mykey *my_key;

    my_key = crypto_mykey_get_by_slot(slot);

    if (!my_key) {
        return NULL;
    } else {
        return (my_key->name);
    }
}

crypto_mykey *crypto_mykey_get_by_slot (ushort slot) 
{
    crypto_mykey *k=crypto_my_key_list_head;

    while (k != NULL) {
       if (k->slot == slot) { /* Found it! */
	    return(k);
       } 
       k = k->next;
    }
    return(NULL);
}

crypto_pubkey *crypto_pubkey_get_by_name (uchar *name)
{
    crypto_pubkey *k=crypto_public_key_list_head;

    while (k != NULL) {
       if (!strcmp(k->name, name)) { /* Found it! */
	    return(k);
       } 
       k = k->next;
    }
    return(0);
}

void crypto_show_pubkey_by_name (parseinfo *csb)
{
    crypto_pubkey *k;

    if ((k=crypto_pubkey_get_by_name(GETOBJ(string,1))) == NULL) {
	printf("No public key found matching name '%s'\n",
		GETOBJ(string,1));
    } else {
	crypto_pubkey_print(k->serial, k->name, &k->pubkey);
    }
}

crypto_pubkey *crypto_pubkey_get_by_serial (uint serial)
{
    crypto_pubkey *k=crypto_public_key_list_head;

    while (k != NULL) {
       if (k->serial == serial) { /* Found it! */
	    return(k);
       }
       k = k->next;
    }
    return(0);
}

void crypto_show_pubkey_by_serial (parseinfo *csb)
{
    crypto_pubkey *k;
    uint serial;

    /*
     * Convert string serial to hex
     */
    serial = parse_hex_long(GETOBJ(string,1));
    if ((k=crypto_pubkey_get_by_serial(serial)) == NULL) {
	printf("No public key found matching serial number '%08x'\n",
		GETOBJ(int,2));
    } else {
	crypto_pubkey_print(k->serial, k->name, &k->pubkey);
    }
}

crypto_mykey *crypto_pubkey_get_keypair_by_name (uchar *name)
{
    crypto_mykey *k=crypto_my_key_list_head;

    while (k != NULL) {
       if (!strcmp(k->name, name)) { /* Found it! */
	    return(k);
       } 
       k = k->next;
    }
    return(0);
}
 
crypto_mykey *crypto_pubkey_get_keypair_by_serial (ulong serial)
{
    crypto_mykey *k=crypto_my_key_list_head;

    while (k != NULL) {
       if (k->serial == serial) {
    	    return(k);
       }
       k = k->next;
    }
    return(NULL);
}

crypto_mykey *crypto_pubkey_get_keypair_by_slot (ushort slot)
{
    crypto_mykey *k=crypto_my_key_list_head;

    while (k != NULL) {
       if (k->slot == slot) {
    	    return(k);
       }
       k = k->next;
    }
    return(NULL);
}

void crypto_show_mypubkey (ulong serial)
{
    crypto_mykey *k;
    if ((k=crypto_pubkey_get_keypair_by_serial(serial)) != NULL) {
	if (!NO_EXTENDED_SLOTS_ON_CHASSIS) {
	    printf("%% Key for slot %d:\n", k->slot);
	}
	crypto_pubkey_print(k->serial, k->name, &k->public_key);
    } else {
    	printf("No public key for serial %d can be found.\n", serial);
    }
    return;
}

void crypto_show_pubkey_all (parseinfo *csb)
{
    crypto_pubkey *k=crypto_public_key_list_head;

    while (k != NULL) {
	crypto_pubkey_print(k->serial, k->name, &k->pubkey);
       	k = k->next;
    }
    return;
}


void crypto_show_mypubkey_all(parseinfo *csb)
{
    crypto_mykey *k=crypto_my_key_list_head;

    while (k != NULL) {
	if (!NO_EXTENDED_SLOTS_ON_CHASSIS) {
	    printf("%% Key for slot %d:\n", k->slot);
	}
	crypto_pubkey_print(k->serial, k->name, &k->public_key);
	k = k->next;
    }
    return;
}

void crypto_pubkey_freekey (crypto_pubkey *key)
{
    free(key->name);
    free(key);
    return;
}

boolean crypto_pubkey_add (crypto_pubkey *new_key)
{
    crypto_pubkey *k, *prev_k;

    if (crypto_public_key_list_head) {
	/*
	 * Insert the public key in the list 
	 */
	prev_k=NULL;
	k=crypto_public_key_list_head;
	while (k != NULL) {
	   if (k->serial == new_key->serial) { 
		/* Update to an existing name. Replace it and quit. */
		new_key->next = k->next;
		crypto_pubkey_freekey(k);
		if (prev_k == NULL) {
		    crypto_public_key_list_head = new_key;
		} else {
		    prev_k->next = new_key;
		}
	    	break;
	   } 
	   prev_k=k;
	   k=k->next;
	}
	if (k == NULL) {
	    prev_k->next=new_key;
	}
    } else {
	/* 
	 * Begin the list.
	 */
	crypto_public_key_list_head=new_key;
    }

    return(TRUE);
}

void crypto_pubkey_delete (parseinfo *csb)
{
    crypto_pubkey *k=crypto_public_key_list_head;
    crypto_pubkey *prev_k=NULL;

    while (k != NULL) {
       if (!strcmp(k->name, PK(csb)->name)) { /* Delete the key. */
    	    if (prev_k == NULL) {
    	    	crypto_public_key_list_head = k->next;
    	    } else {
    	    	prev_k->next = k->next;
    	    }
    	    crypto_pubkey_freekey(k);
    	    break;
       } 
       prev_k=k;
       k=k->next;
    }
    if (k == NULL) {
        buginf("\nCRYPTO: Public key '%s' not found in list.",
		PK(csb)->name);
    }
    crypto_pubkey_freekey(PK(csb));
    PK(csb) = NULL;
}

/*
 * Cleanup space allocated for a public key
 */
void crypto_pubkey_crash_and_burn (parseinfo *csb)
{
    crypto_pubkey_freekey(PK(csb));
    SETPK(csb, NULL);
    exit_config_submode(csb);
}

/*
 * Process crypto-pubkey mode commands
 * 
 * crypto_pubkey_hexmode is the dispatch routine for the hex data parsing
 * mode. It accumulates data onto the end of the hex buffer in the packet
 * generator context until it runs out of space. When the user types "q",
 * it gets out of hex mode.
 */
void crypto_pubkey_hexmode (parseinfo *csb)
{
    ushort cpylen;		/* Number of bytes to be copied */

    switch (csb->which) {

    /* 
     * User typed a line of data. OBJ(int,1) is the length, and OBJ(string,1)
     * contains the actual data.
     */
    case PARSER_CRYPTO_PUBKEY_DATA:
	cpylen = GETOBJ(int,1);	/* Get length of data parsed */
	/*
	 * Copy the hex data into the correct offset into the public key.
	 */
	if ((cpylen + PK(csb)->pubkey.keylen) > CRYPTO_KEYSIZE) {
	    printf ("Key too long for parse buffer.\n");
	    crypto_pubkey_crash_and_burn(csb);
	    return;
	} else {
	    bcopy (GETOBJ(string,1), 
		(PK(csb)->pubkey.key + PK(csb)->pubkey.keylen),
	       	cpylen);
	    PK(csb)->pubkey.keylen += cpylen;
	    break;
	}

    /* User is done with hex mode. */
    case PARSER_CRYPTO_PUBKEY_QUIT:

	/* Move the pubkey into the configuration */
	if (!crypto_pubkey_add(PK(csb))) {
	    crypto_pubkey_crash_and_burn(csb);
	    return;
	}
        PK(csb) = NULL;

	/* Switch back to configuration */
	exit_config_submode(csb);

	break;

    /* This should never happen */
    default:
	printf ("Hex-mode parsing error: bad \"which\" value\n");
	break;
    }
}

void crypto_pubkey_cmd (parseinfo *csb)
{
    crypto_pubkey *k;
    int name_len;

    if (csb->nvgen) {
	if (crypto_public_key_list_head == NULL)
	    return;
	k=crypto_public_key_list_head;
        nv_write(TRUE, "!");
	while (k != NULL) {
	    nv_write(TRUE, "crypto public-key %s %08x", 
			k->name, k->serial);
	    crypto_pubkey_nvgen_print(k->pubkey.keylen, k->pubkey.key, FALSE);
    	    k=k->next;
	}
	return;
    }

    /*
     * Get some space for the public key structure & put a pointer to it in
     * the csb.
     */
    SETPK(csb, malloc(sizeof(crypto_pubkey)));
    if (PK(csb) == (crypto_pubkey *) NULL)
	return;

    /*
     * Add what we know so far.
     */
    name_len = strlen(GETOBJ(string,1))+1;
    PK(csb)->name = malloc(name_len);
    if ((PK(csb)->name == (uchar *) NULL)) {
	free(PK(csb));
	return;
    }
    sstrncpy(PK(csb)->name, GETOBJ(string,1), name_len);
    PK(csb)->serial = GETOBJ(int,2);
    PK(csb)->pubkey.keylen = 0;
    PK(csb)->next = NULL;
    
    /*
     * no crypto public-key <name> 
     */
    if (!csb->sense) {
	crypto_pubkey_delete(csb);
	return;
    }

    /*
     * Adding a public key
     */
    if (csb->resolvemethod == RES_MANUAL) {
	/* Only print the message for a "conf terminal" */
    	printf("Enter a public key as a hexidecimal number ....\n");
    }
    set_mode_byname(&csb->mode, "crypto-pubkey", MODE_VERBOSE);
}

/*
 * Cleanup space allocated for a keypair
 */

void crypto_pubkey_keypair_freekey (crypto_mykey *key)
{
    free(key->name);
    free(key);
    return;
}


void crypto_pubkey_keypair_crash_and_burn (parseinfo *csb)
{
    crypto_pubkey_keypair_freekey(MY_KEYS(csb));
    SET_MY_KEYS(csb, NULL);
    exit_config_submode(csb);
}

boolean crypto_pubkey_keypair_add (crypto_mykey *newkey)
{
    crypto_mykey *k, *prev_k;
    int name_len;

    if (crypto_my_key_list_head) {
	/*
	 * Insert the public key in the list 
	 */
	prev_k=NULL;
	k=crypto_my_key_list_head;
	while (k != NULL) {
	   if (k->slot == newkey->slot) { 
		/* Update to an existing key. Replace it and quit. */
		newkey->next = k->next;
		crypto_pubkey_keypair_freekey(k);
		if (prev_k == NULL) {
		    crypto_my_key_list_head = newkey;
		} else {
		    prev_k->next = newkey;
		}
	    	break;
	   } 
	   prev_k=k;
	   k=k->next;
	}
	if (k == NULL) {
	    prev_k->next=newkey;
            /* Update crypto_engine_tbl state */
            if (newkey->slot < CRYPTO_MAX_SLOTS) {
                crypto_engine_tbl[newkey->slot].state = 
                                   CRYPTO_ENGINE_DSS_KEY_GEN;
                if (!crypto_engine_tbl[newkey->slot].name) {
    		    name_len = strlen(newkey->name)+1;
                    crypto_engine_tbl[newkey->slot].name = 
                                     malloc(name_len);
		    if (crypto_engine_tbl[newkey->slot].name) {
                    	strncpy(crypto_engine_tbl[newkey->slot].name, 
				newkey->name, name_len);
		    }
                }
            }
	}
    } else {
	/* 
	 * Begin the list.
	 */
	crypto_my_key_list_head = newkey;
    }

    return(TRUE);
}

void crypto_pubkey_keypair_delete (crypto_mykey *oldkey)
{
    crypto_mykey *k=crypto_my_key_list_head;
    crypto_mykey *prev_k=NULL;

    while (k != NULL) {
       if (k->slot == oldkey->slot) { /* Delete the key. */
    	    if (prev_k == NULL) {
    	    	crypto_my_key_list_head = k->next;
    	    } else {
    	    	prev_k->next = k->next;
    	    }
    	    crypto_pubkey_keypair_freekey(k);
    	    break;
       } 
       prev_k=k;
       k=k->next;
    }
    if (k == NULL) {
        buginf("\nCRYPTO: Key pair for crypto engine in slot '%d' not found.",
		oldkey->slot);
    }
}

/*
 * Process crypto-keypair mode commands. It would be nice to fold this
 * with crypto_pubkey_hexmode, but that may be wasted effort as the keeping
 * of private keys (and maybe our own public keys) must go away in time.
 */
void crypto_pubkey_keypair_hexmode (parseinfo *csb)
{
    ushort cpylen;		/* Number of bytes to be copied */

    switch (csb->which) {

    /* 
     * User typed a line of data. OBJ(int,1) is the length, and OBJ(string,1)
     * contains the actual data.
     */
    case PARSER_CRYPTO_PUBKEY_DATA:
	cpylen = GETOBJ(int,1);	/* Get length of data parsed */
	/*
	 * Copy the hex data into the correct offset into the key.
	 */
    	if (WHICH_KEY(csb) == 0) { /* private key */

	    if ((cpylen + MY_KEYS(csb)->private_key.keylen) > 
							CRYPTO_KEYSIZE) {
	        printf ("Private Key too long for parse buffer.\n");
	        crypto_pubkey_keypair_crash_and_burn(csb);
	        return;
	    } else {
		bcopy (GETOBJ(string,1), (MY_KEYS(csb)->private_key.key + 
			MY_KEYS(csb)->private_key.keylen), cpylen);
	        MY_KEYS(csb)->private_key.keylen += cpylen;
	        break;
	    }
	} else { /* public key */
	    if ((cpylen + MY_KEYS(csb)->public_key.keylen) > 
							CRYPTO_KEYSIZE) {
	        printf ("Public Key too long for parse buffer.\n");
	        crypto_pubkey_keypair_crash_and_burn(csb);
	        return;
	    } else {
		bcopy (GETOBJ(string,1), (MY_KEYS(csb)->public_key.key + 
			MY_KEYS(csb)->public_key.keylen), cpylen);
	        MY_KEYS(csb)->public_key.keylen += cpylen;
	        break;
	    }
	}

    /* User is done with hex mode. */
    case PARSER_CRYPTO_PUBKEY_QUIT:

	if (WHICH_KEY(csb) == 0) {
	    WHICH_KEY(csb) = 1; /* switch hexdata input to the public key */
        } else {
	    /* 
	     * We've got both keys, so move the keypair into the 
	     * configuration & initialize the EPA.
	     */
	    if (crypto_pubkey_keypair_add(MY_KEYS(csb))) {
		epa_init(crypto_engine_rpsp_slot, NULL, NULL);
	    } else {
	        crypto_pubkey_keypair_crash_and_burn(csb);
	        return;
	    }
    	    MY_KEYS(csb) = NULL;
	    /* Switch back to configuration */
	    exit_config_submode(csb);
        }
        break;

    /* This should never happen */
    default:
	printf ("Hex-mode parsing error: bad \"which\" value\n");
	break;
    }
}


/*
 * Trying to create (in private NVRAM):
 *
 * crypto DSS-key-pair 0 bew.cisco.com 12345678 2
 *  3948 0202 8482 00AD ADAD ADAD DC93 0944 3249 4949 2040 2042 4204 9408 3943 
 *  9348 4583 4383 484A CFAC FACF ACFA CFC3 9939 A948 0808 8488 00AD ADAD ADAD 
 *  DC9A 0944
 *  quit
 *  1234 5678 9012 3456 7890
 *  quit
 *
 */

void crypto_pubkey_keypair_cmd (parseinfo *csb)
{
    crypto_mykey *k;
    int name_len;

    if (csb->nvgen) {
	if (crypto_my_key_list_head == NULL)
	    return;
	k=crypto_my_key_list_head;
        nv_write_private(TRUE, "!");
	while (k != NULL) {
	    nv_write_private(TRUE, "crypto DSS-key-pair %s %08x", 
			       k->name, k->serial);
	    if (!NO_EXTENDED_SLOTS_ON_CHASSIS) {
	    	nv_add_private(TRUE, " %d", k->slot);
	    }
	    crypto_pubkey_nvgen_print(k->private_key.keylen,
				      k->private_key.key, TRUE);
	    crypto_pubkey_nvgen_print(k->public_key.keylen, 
				      k->public_key.key, TRUE);
    	    k=k->next;
	}
	return;
    }
	
    /* 
     * Important! Don't accept keys unless they come from private NVRAM!
     */
    if (csb->resolvemethod != RES_PRIVATE_NONVOL) {
        return;
    }

    /*
     * Get some space for the key pair structure & put a pointer to it in
     * the csb.
     */
    SET_MY_KEYS(csb, malloc(sizeof(crypto_mykey))); 
    if (MY_KEYS(csb) == (crypto_mykey *) NULL)
	return;

    /*
     * Add what we know so far.
     */
    name_len = strlen(GETOBJ(string,1))+1;
    MY_KEYS(csb)->name = malloc(name_len);
    if ((MY_KEYS(csb)->name == (uchar *) NULL)) {
	free(MY_KEYS(csb));
	return;
    }
    sstrncpy(MY_KEYS(csb)->name, GETOBJ(string,1), name_len);
    MY_KEYS(csb)->serial = GETOBJ(int,2);
    if (GETOBJ(int,3) == CRYPTO_INVALID_SLOT) {
    	MY_KEYS(csb)->slot = crypto_engine_rpsp_slot;
    } else {
    	MY_KEYS(csb)->slot = GETOBJ(int,3);
    }
    MY_KEYS(csb)->private_key.keylen = 0;
    MY_KEYS(csb)->public_key.keylen = 0;
    MY_KEYS(csb)->next = NULL;
    WHICH_KEY(csb)=0; /* initialize to 1st key */
    
    /*
     * Adding the key pair
     */
    set_mode_byname(&csb->mode, "crypto-pubkey-keypair", MODE_VERBOSE);
}
