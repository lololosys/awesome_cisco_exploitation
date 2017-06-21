/* $Id: crypto_exchange.c,v 3.2.66.4 1996/07/15 10:05:01 xliu Exp $
 * $Source: /release/112/cvs/Xsys/crypto/crypto_exchange.c,v $
 *------------------------------------------------------------------
 * crypto_exchange.c - Public key exchange using a TCP connection.
 *
 * October 1995, Brian Weis
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: crypto_exchange.c,v $
 * Revision 3.2.66.4  1996/07/15  10:05:01  xliu
 * CSCdi62312:  IP packets without layer 4 headers dont encrypt properly
 * Branch: California_branch
 *
 * Revision 3.2.66.3  1996/06/07  00:24:01  bew
 * CSCdi59169:  Crypto cleanup work
 * Branch: California_branch
 * Various cleanups & addition of "crypto ping".
 *
 * Revision 3.2.66.2  1996/05/12  23:12:14  xliu
 * CSCdi56023:  Fixes made after integrating with turbo IPC, and cached
 * packet mem.
 * Branch: California_branch
 *
 * Revision 3.2.66.1  1996/04/19  14:55:41  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commits
 *
 * Revision 2.1.2.10  1996/04/16  21:51:58  bew
 * Branch: IosSec_branch
 * General code cleanup. Implemented checking of crypto map peers. CIM now
 * addressed to the src/dst rather than pe/upe.
 *
 * Revision 2.1.2.9  1996/03/14  00:57:51  bew
 * Branch: IosSec_branch
 * Changed the crypto key-exchange protocol to be more intuitive.
 *
 * Revision 2.1.2.8  1996/03/13  22:45:00  bew
 * Branch: IosSec_branch
 * The RSP/VIP environment (multiple crypto engines) are now supported
 * in crypto parser routines and crypto session mgmt code.
 *
 * Revision 2.1.2.7  1996/01/25  18:19:49  bew
 * Branch: IosSec_branch
 * Implemented a config option for automatic pre-genereration of
 * Diffie-Hellman key pairs. Also changed the use of timers so
 * that the the STUCKTMR message doesn't show up any more.
 *
 * Revision 2.1.2.6  1996/01/24  22:55:48  bew
 * Branch: IosSec_branch
 * Sync-related changes to sys/crypto.
 *
 * Revision 2.1.2.5  1996/01/13  05:46:38  bew
 * Branch: IosSec_branch
 * Cleanup. Lots of cleanup in the IosSec_branch crypto directory.
 *
 * Revision 2.1.2.4  1995/12/21  07:06:06  bew
 * Branch: IosSec_branch
 * Replace the direct call to SHA with a crypto engine call.
 *
 * Revision 2.1.2.3  1995/12/20  23:30:42  bew
 * Branch: IosSec_branch
 * Public key exchange can now be two-way as well as one-way. Context is
 * now freed after a sign operation. Wrap-around of connection ids now
 * supported.  General cleanup.
 *
 * Revision 2.1.2.2  1995/11/28  01:46:26  bew
 * Port of newest Cylink SDU code, plus CS cleanup. Cylink files
 * are moved to match their original tree structure.
 * Branch: IosSec_branch
 *
 * Revision 2.1.2.1  1995/10/17  01:19:33  bew
 * Branch: IosSec_branch
 * Add the simple unauthenticated public key exchange protocal. Cleaned
 * up the printing of pulic keys. Modified some show commands syntax to
 * match the functional spec. General cleanup.
 *
 * Revision 2.1  1995/10/13  21:27:18  bew
 * Crypto template.
 *
 * Revision 1.7  1995/06/07 12:03:03  smackie
 * Fold back Arizona_branch into mainline.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <ciscolib.h>
#include "globs.h"
#include "ttysrv.h"
#include "interface_private.h"
#include "../ui/debug.h"
#include "../ip/ip.h"
#include "../tcp/tcp.h"
#include "crypto.h"
#include "../crypto/crypto_pubkey.h"
#include "../crypto/crypto_debug.h"
#include "interface_private.h" /* for crypto_engine_api.h */
#include "../crypto/crypto_engine_api.h"
#include "../crypto/crypto_util.h"

#define FINGERPRINT_LENGTH 20

/*
 * Display a fingerprint based on an SHA hash of the key. Get the hash & 
 * display the odd digits in the SDU specified way.
 */
boolean crypto_exchange_get_fp (uint serial, uchar *name, crypto_key *pk)
{
    uchar hash[FINGERPRINT_LENGTH];
    int i;
    epa_status status;
    epa_alg_obj alg;
   
    alg.algorithm_type = SHA_DIGEST;
    status = crypto_create_digest(&alg, pk->key, pk->keylen, hash);
    if (status != EPA_OP_OK) {
	printf("Error in getting fingerprint:%s Aborting.\n",
		crypto_xlate_status(status));
	return(FALSE);
    }

    printf("Public key for %s:\n", name);
    printf("   Serial Number %08x\n", serial);
    printf("   Fingerprint   ");
    for (i=0; i<FINGERPRINT_LENGTH; i++) {
	printf("%x", (hash[i]>>4));
	if (!((i+1) % 4)) {
	    printf(" ");
	}
    }
    printf("\n");
    return(TRUE);
}

/*
 * Send some data from some address of some length.
 */
void crypto_exchange_send (tcbtype *tcb, void *addr, int bytes)
{
    int status;

    if ((status = tcp_putstring(tcb, (uchar *)addr, bytes)) != bytes) {
	printf("Warning: Only %d of %d bytes sent.\n", status, bytes );
    }
    if (crypto_ke_debug) {
	buginf("\nCRYPTO-KE: Sent %d bytes.", status);
    }
    return;
}

/*
 * Receive some data from some address of some length.
 */
int crypto_exchange_receive (tcbtype *tcb, void *location, int total_exp_bytes)
{
    uchar *addr, *copy_addr;
    int actual_bytes, expected_bytes;
    int status;
    int total_bytes = 0;

    /*
     * Iniatialize working variables.
     */
    copy_addr = location;
    expected_bytes = total_exp_bytes;

    do {
	/*
	 * Get some bytes
	 */
    	status = tcp_getstringblock(tcb, &addr, &actual_bytes, expected_bytes);
    	if (crypto_ke_debug) {
	    buginf("\nCRYPTO-KE: Received %d bytes.", actual_bytes);
    	}
	/*
	 * Copy whatever we got into place.
	 */
    	memcpy(copy_addr, addr, actual_bytes);
	/*
	 * Adjust some status counters
	 */
	copy_addr += actual_bytes;      /* Set next addr where to write */
	total_bytes += actual_bytes;    /* Adjust how many bytes received */
	expected_bytes -= actual_bytes; /* Adjust how many left expect to get */
	/*
	 * We're done either when we got how many bytes we wanted, or TCP
	 * says we're done.
	 */
    } while ((expected_bytes > 0) && 
	     (status == OK || status == URGENT_DATA || status == NO_DATA));

    if (status == END_OF_FILE) {
	if (crypto_ke_debug) {
	    buginf("\nCRYPTO-KE: TCP reports end-of-file reached.");
 	}
    }
  
    if (total_bytes != total_exp_bytes) {
	if (crypto_ke_debug) {
	    buginf("\nCRYPTO-KE: Expected %d bytes, got %d bytes.",
		   total_exp_bytes, total_bytes);
	}
	return(1);
    }

    return(0);
}

/* 
 * Send a public key along with relevant info in this order:
 *
 * Send the following information:
 * 1. serial number (4 bytes)
 * 2. length of the name (2 bytes)
 * 3. name (n bytes)
 * 4. public key length (2 bytes)
 * 5. public key (m bytes)
 * 6. the length of a named public key to return to us, or zero if none.
 * 6. a named public key to return to us, or NULL if none.
 */
boolean crypto_exchange_sendkey (tcbtype *tcb, uchar *epa_name)
{
    ushort name_length;
    crypto_mykey *config_key;
    crypto_mykey *key = malloc(sizeof(crypto_mykey));

    if (key == NULL) {
	return(FALSE);
    }

    /*
     * Get a pointer to the public key & copy it to ensure we don't get
     * stuck with a dangling pointer later.
     */
    if ((config_key=crypto_pubkey_get_keypair_by_name(epa_name)) == NULL) {
	printf("Can't find crypto engine named '%s'\n", epa_name);
	return(FALSE);
    }
    memcpy(key, config_key, sizeof(crypto_mykey));

    crypto_exchange_send(tcb, &key->serial, sizeof(uint));

    name_length = strlen(key->name);
    crypto_exchange_send(tcb, &name_length, sizeof(ushort));
    crypto_exchange_send(tcb, key->name, name_length);

    crypto_exchange_send(tcb, &key->public_key.keylen, sizeof(ushort));
    crypto_exchange_send(tcb, &key->public_key.key, key->public_key.keylen);

    tcp_putpush(tcb, TRUE, FALSE);

    /* 
     * Print the DSS fingerprint. Get the hash & display the odd digits.
     */
     (void) crypto_exchange_get_fp(key->serial, key->name, &key->public_key);

    return(TRUE);
}

boolean crypto_exchange_receivekey (tcbtype *tcb)
{
    ushort name_length;
    int status=0;

    crypto_pubkey *key = malloc(sizeof(crypto_pubkey));

    if (key == NULL) {
	return(FALSE);
    }

    status|=crypto_exchange_receive(tcb, &key->serial, sizeof(uint));

    status|=crypto_exchange_receive(tcb, &name_length, sizeof(ushort));
    if ((key->name = malloc(name_length+1)) == NULL) { /* abort */
	free(key);
	return(FALSE);
    }
    status|=crypto_exchange_receive(tcb, key->name, name_length);

    status|=crypto_exchange_receive(tcb, &key->pubkey.keylen, sizeof(ushort));
    status|=crypto_exchange_receive(tcb, &key->pubkey.key, key->pubkey.keylen);

    /*
     * Don't continue if there was an error.
     */
    if (status) {
	printf("\nAborting.\n");
	free(key);
	return(FALSE);
    }

    /* 
     * Print the DSS fingerprint. 
     */
    if (!crypto_exchange_get_fp(key->serial, key->name, &key->pubkey)) {
	free(key);
	return(FALSE); 
    }

    /* 
     * If the fingerprint is correct, link the key structure into the 
     * public key list.
     */
    if (yes_or_no("Add this public key to the configuration",
		   FALSE, FALSE)) {
    	if (!crypto_pubkey_add(key)) {
	    printf("Error in adding public key. Aborted.\n");
	    crypto_pubkey_freekey(key);
    	}
    } else {
	free(key); /* Didn't want the key after all */
    }
    return(TRUE);
}

void crypto_exchange_ip (addrtype *addr, char *epa_name, ushort port)
{
    tcbtype *tcb;
    int status;
    boolean ok;

    /*
     * If the user did not specify a port use the default.
     */
    if (!port) {
	port = CRYPTO_KEY_XCHG;
    }
    
    /*
     * Setup a tcp connection 
     */
    if ((tcb = tcp_open(stdio, port, addr, 0, NULL, &status, NULL)) 
	    == NULL) {
        printf("Error! TCP open failed: %s\n", tcperror2string(status));
	return;
    }

    /*
     * Send the key info
     */
    ok = crypto_exchange_sendkey(tcb, epa_name);

    /*
     * If a peer name was specified, try to receive it now.
     */
    if (ok) {
    	if (confirm("\nWait for peer to send a key")) {
	    printf("\nWaiting ....\n");
	    flush();
            (void) crypto_exchange_receivekey(tcb);
	}
    }

    /* 
     * Close the connection now
     */
    tcp_close(tcb); 
}

void crypto_exchange_passive_ip (ushort port)
{
    tcbtype *tcb, *newtcb;
    int status;
    boolean ok;
    crypto_mykey *k;

    /*
     * If the user did not specify a port use the default.
     */
    if (!port) {
	port = CRYPTO_KEY_XCHG;
    }

    /* 
     * Do the TCP connection stuff.
     */
    if ((tcb = tcp_create(stdio, &status, NULL)) == NULL) {
	printf("TCP create connection failed: %s\n", tcperror2string(status));
	return;
    }

    /* 
     * This should allow the passive side to be interrupted at any time
     * with a ^x interrupt.
     */
    psion(stdio);

    if ((status = tcp_bind(tcb, NULL, port)) != OK) {
	printf("TCP bind failed: %s\n", tcperror2string(status));
	tcp_abort(tcb);
	return;
    }

    /*
     * Warn the user of what is going on.
     */
    printf("\nEnter escape character to abort if connection does not complete.");
    if (!confirm("\nWait for connection from peer")) {
	tcp_abort(tcb);
	return;
    }

    /*
     * Tell user that we accepted the confirmation.
     */
    printf("\nWaiting ....\n");
    flush();

    if ((status=tcp_listen(tcb, 1, NULL)) != OK) {
	printf("TCB listen failed: %s\n", tcperror2string(status));
	tcp_abort(tcb);
	return;
    }
    if ((newtcb=tcp_accept(tcb, &status)) == NULL) {
	printf("TCB accept failed: %s\n", tcperror2string(status));
	tcp_abort(tcb);
	return;
    }
    
    /*
     * Receive the key & add it to the config.
     */
    ok = crypto_exchange_receivekey(newtcb);

    /*
     * Send the key info
     */
    if (ok && confirm("\nSend peer a key in return")) {
	printf("Which one?\n");
	k = crypto_my_key_list_head;
	while (k != NULL) {
	    if (yes_or_no(k->name, TRUE, TRUE)) {
                ok = crypto_exchange_sendkey(newtcb, k->name);
	    	if (ok) {
	       	    tcp_putpush(newtcb, TRUE, FALSE);
	        } 
	        break;
	    }
	    k = k->next;
	}
	if (k == NULL) {
	    printf("Key not found!\n");
	}
    }

    /* 
     * Close the new and old connections
     */
    tcp_close(newtcb);
    tcp_close(tcb);

    return;
}

/*
 * The following functions crypto_exchange_sessionkey() and
 * crypto_exchange_sessionkey_passive() are used for Cisco testing only.
 */
boolean crypto_exchange_sessionkey_ip (addrtype *addr, uchar *my_public_number,
				      uchar *their_public_number)
{
    tcbtype *tcb;
    int status;
    ushort port;

    port = CRYPTO_KEY_XCHG;
    
    /*
     * Setup a tcp connection 
     */
    if ((tcb = tcp_open(stdio, port, addr, 0, NULL, &status, NULL)) 
	    == NULL) {
        printf("Error! TCP open failed: %s\n", tcperror2string(status));
	return(FALSE);
    }
    if (status != OK) {
	printf("TCP open error: %d\n", tcperror2string(status));
	tcp_abort(tcb);
	return(FALSE);
    }

    /*
     * Send our public number & get theirs
     */
    crypto_exchange_send(tcb, my_public_number, 64);
    tcp_putpush(tcb, TRUE, FALSE);
    status = crypto_exchange_receive(tcb, their_public_number, 64);

    if (!status) {
        printf("Keys exchanged.\n");
    }

    /* 
     * Close the connection now
     */
    tcp_close(tcb); 

    return(status == 0);
}

boolean crypto_exchange_sessionkey_passive_ip (uchar *my_public_number,
					      uchar *their_public_number)
{
    tcbtype *tcb, *newtcb;
    int status;
    ushort port;

    port = CRYPTO_KEY_XCHG;

    /* 
     * Do the TCP connection stuff.
     */
    if ((tcb = tcp_create(stdio, &status, NULL)) == NULL) {
	printf("TCP create connection failed: %s\n", tcperror2string(status));
	return(FALSE);
    }
    if (status != OK) {
	printf("TCP create error: %d\n", tcperror2string(status));
	tcp_abort(tcb);
	return(FALSE);
    }

    /* 
     * This should allow the passive side to be interrupted at any time
     * with a ^x interrupt.
     */
    psion(stdio);

    if ((status = tcp_bind(tcb, NULL, port)) != OK) {
	printf("TCP bind failed: %s\n", tcperror2string(status));
	tcp_abort(tcb);
	return(FALSE);
    }

    /*
     * Warn the user of what is going on.
     */
    printf("\nEnter escape character to abort.");
    if (!confirm("\nWait for connection from peer")) {
	tcp_abort(tcb);
	return(FALSE);
    }

    /*
     * Tell user that we accepted the confirmation.
     */
    printf("\nWaiting ....\n");
    flush();

    if ((status=tcp_listen(tcb, 1, NULL)) != OK) {
	printf("TCB listen failed: %s\n", tcperror2string(status));
	tcp_abort(tcb);
	return(FALSE);
    }
    if ((newtcb=tcp_accept(tcb, &status)) == NULL) {
	printf("TCB accept failed: %s\n", tcperror2string(status));
	tcp_abort(tcb);
	return(FALSE);
    }
    if (status != OK) {
	printf("TCP accept error: %d\n", tcperror2string(status));
	tcp_abort(tcb);
	return(FALSE);
    }

    /*
     * Get their public number & send ours
     */
    status = crypto_exchange_receive(newtcb, their_public_number, 64);
    if (!status) {
    	crypto_exchange_send(newtcb, my_public_number, 64);
    	tcp_putpush(newtcb, TRUE, FALSE);
    }

    if (!status) {
        printf("Keys exchanged.\n");
    }

    /* 
     * Close the new and old connections
     */
    tcp_close(newtcb);
    tcp_close(tcb);

    return(status == 0);
}
