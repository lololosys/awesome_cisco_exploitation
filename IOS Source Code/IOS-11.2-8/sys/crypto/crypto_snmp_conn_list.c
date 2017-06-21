/* $Id: crypto_snmp_conn_list.c,v 3.1.4.5 1996/08/27 19:45:46 snijsure Exp $
 * $Source: /release/112/cvs/Xsys/crypto/crypto_snmp_conn_list.c,v $
 *------------------------------------------------------------------
 * Cisco IP encryption mib.
 * Code that constructs linked list of connections. Correlating the 
 * information contained within the crypto subsystem. 
 *
 * April 1996, Subodh Nijsure
 *
 * Copyright (c) 1996-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: crypto_snmp_conn_list.c,v $
 * Revision 3.1.4.5  1996/08/27  19:45:46  snijsure
 * CSCdi66755:  Mib object cieEsaMode is not updated to show current
 * status of ESA.
 * Branch: California_branch
 * Extract status of ESA and copy it into cieESaMode.
 *
 * Revision 3.1.4.4  1996/08/06  00:36:18  snijsure
 * CSCdi65133:  Change reference to encryption hardware to ESA from EPA.
 * Branch: California_branch
 * Changed mib objects name prefix from epa* to esa*
 *
 * Revision 3.1.4.3  1996/07/15  10:05:21  xliu
 * CSCdi62312:  IP packets without layer 4 headers dont encrypt properly
 * Branch: California_branch
 *
 * Revision 3.1.4.2  1996/05/12  23:12:28  xliu
 * CSCdi56023:  Fixes made after integrating with turbo IPC, and cached
 * packet mem.
 * Branch: California_branch
 *
 * Revision 3.1.4.1  1996/04/19  14:56:03  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commits
 *
 * Revision 3.1.2.3  1996/04/12  16:07:44  snijsure
 * Branch: IosSec_branch
 * Remove printf and modify code per cisco
 * coding conventions.
 *
 * Revision 3.1.2.2  1996/04/10  23:47:58  snijsure
 * Branch: IosSec_branch
 * Code to set value for newly added cieTestConnCryptoMapName object.
 *
 * Revision 3.1.2.1  1996/04/10  17:40:32  snijsure
 * Branch: IosSec_branch
 * Checking in implementation of Cisco IP encryption MIB.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
#include "master.h"
#include <ciscolib.h>
#include "subsys.h"
#include "../snmp/snmp_api.h"
#include "sr_encryptmib.h"
#include "sr_encryptmib-mib.h"

#include "config.h"
#include "globs.h"
#include "interface_private.h"
#include "sys_registry.h"
#include "../ipc/ipc_util.h"
#include "../ipc/ipc.h"
#include "../ip/ip.h"
#include "../crypto/crypto.h"
#include "../crypto/crypto_conntable.h"   
#include "../crypto/crypto_engine_api.h"
#include "../crypto/crypto_glue_private.h" 
#include "../crypto/crypto_engine/core/crypto_engine.h"
#include "../crypto/crypto_pubkey.h"

#include "../crypto/sdu/include/csdata.h"
#include "../crypto/crypto_debug.h"
#include "../crypto/crypto_engine/crypto_lib/lib_toolkit.h"
#include "../crypto/crypto_engine/crypto_lib/encrypt.h"
#include "../crypto/crypto_engine/core/crypto_swepa.h"
#include "../crypto/crypto_engine/core/crypto_engine.h"
#include "../crypto/sr_encryptmibdefs.h"
#include "../crypto/crypto_snmp_conn_list.h"
#include "../crypto/crypto_debug_flags.h"

#define MIN_cieConnIndex     1
#define MAX_ConnIndex        2147483647 	
extern PETnode *pet;
extern ushort epCOTnodes;
long	global_cieConnIndex = 1;	
crypto_snmp_connection *crypto_snmp_connection_head = NULL;

/* 
 * Searches the epa_swcrypto_conn_tbl for specific connection id
 * and fills up the table_entry with counters 
 */
static void 
fill_counters (crypto_snmp_connection *table_entry,
	long connection_id )
{
    long n;
    epa_swcrypto_connection *conn; 
    if ( connection_id <= 0 ) {
	table_entry->noPktsEncrypted = 0;
        table_entry->noPktsDecrypted = 0;
	table_entry->noPktsDropped = 0;
	table_entry->localTimeEstablished = 0;
     } else {
	for ( n = 0; n < CRYPTO_CONNTBL_SIZE;n++ ) {
            conn = &epa_swcrypto_conn_tbl[n];
	    if ( conn->connection_id == connection_id ) {
	        table_entry->noPktsEncrypted = conn->encrypt_count;
	        table_entry->noPktsDecrypted = conn->decrypt_count; 
	        table_entry->noPktsDropped = conn->drop_count; 
	        table_entry->localTimeEstablished = 0;
	        table_entry->alg_type = conn->alg_type;
		return;
             }
	}
     }
}
/*
 * Walk the crypto_snmp_connection list and free the memory
 */
static void 
free_crypto_snmp_connection_list (void)
{
    crypto_snmp_connection *tmp,*entry = crypto_snmp_connection_head; 
    while ( entry != NULL ) {
	tmp = entry->next;
	free(entry);
	entry = tmp;
    }	
    crypto_snmp_connection_head = NULL;
}

static crypto_snmp_connection * 
check_if_entry_exists (uint engineID,
                       long peAddr,long upeAddr)
{
    crypto_snmp_connection *entry = crypto_snmp_connection_head; 

    while ( entry != NULL ) {
       if ( entry->engineID == engineID && entry->protectedEntity == peAddr
	    && entry->unprotectedEntity == upeAddr ) {
	    return entry;
       }	
       entry = entry->next;
    }
    return NULL;
}

/* 
 * Add a entry of type crypto_snmp_connection to the list, if the list
 * is empty assign new node to global 'head' pointer
 */
static crypto_snmp_connection *
crypto_snmp_connection_list_add (void)
{
    /* Append the entry to end of the list and return it */
    crypto_snmp_connection *tmp,*entry; 
    entry = malloc(sizeof(crypto_snmp_connection));
    if (!entry) {
	return NULL;
    }
    entry->next = NULL;
    if (  crypto_snmp_connection_head == NULL ) {
        crypto_snmp_connection_head = entry; 
        return entry;	
    }
    tmp = crypto_snmp_connection_head;
    while ( tmp != NULL ) {
	if ( tmp->next == NULL ){
		tmp->next = entry;
		break;
	}
	tmp = tmp->next;
    }	

    return entry; 
}

/*
 * Scan through the crypto subsytem's data structure and create 
 * linked list which contains detailed information about each structure.
 * This function returns a number that indicates number of items present
 * on the list.
 * NOTE: crypto subsystem has hard coded limit of 300, would declaring a
 *       global array of 300 be more efficient? Memory v/s performance? 
 *       
 */
long 
crypto_no_of_connections (void)
{

    int status;
    ushort i, j, cur, page;
    long peAddress, upeAddress;

    extern pconn_entry pconn_list[];
    crypto_snmp_connection *entry;
    long no_of_connections = 0;
    uint sw_engine_serial_no = 0; 
    extern crypto_mykey *crypto_my_key_list_head ;
    crypto_mykey  *key_entry = crypto_my_key_list_head;

    /* scan through the list of crypto_mykey search for key that has serial
     * number 0, which should indicate its software engine (?) 
     */
    if ( key_entry == NULL )
	return 0;	/* There are no crypto engines configured */ 
    while ( key_entry != NULL ) {
	if ( key_entry->slot ==  0 ) {
            sw_engine_serial_no = key_entry->serial;	
	    break;
	}
	key_entry = key_entry->next;
    }
    if  (crypto_snmp_connection_head != NULL ) {
        global_cieConnIndex=0;	
	free_crypto_snmp_connection_list();
    }

    /* pet_hash is hashed list of protected entity */
    for (i=0; i<epPEThash; i++) {
        if (pet_hash[i] != 0) { /* found a hash list */
            cur = pet_hash[i];
            while (cur != 0) {
                page = cur;
                for (j=1; j<epCOTnodes; j++)  {
                    if (cot[page].node[j].id.adr != 0) {
			peAddress = pet[page].id.adr;  
			upeAddress = cot[page].node[j].id.adr;

			entry = check_if_entry_exists(sw_engine_serial_no,
				peAddress,upeAddress);	

			/* This is a new entry */
			if ( entry == NULL ) {
			    entry = crypto_snmp_connection_list_add();
			    if (!entry) {
				return no_of_connections;
			    }
			    global_cieConnIndex++;	
			    /* Wrap the counter! */
			    if ( global_cieConnIndex > MAX_ConnIndex )        
				global_cieConnIndex=1;	
		        }	
			entry->engineID = sw_engine_serial_no;	/* sw engine */
			entry->connectionIndex = global_cieConnIndex;	
			entry->protectedEntity = peAddress; 
			entry->unprotectedEntity = upeAddress; 
			entry->localTimeEstablished 
				= cot[page].node[j].cib.crypto.time ;
			status = cot[page].node[j].cib.status ;
		        if ( (status & PEND_CONN) == PEND_CONN )
			   entry->connectionStatus = 
			      D_cieConnStatus_pendingConnection; 
		        else if ( (status & OPEN_NODE) == OPEN_NODE )
			   entry->connectionStatus = 
			      D_cieConnStatus_openConnection; 
		        else if ( (status & XCHG_KEYS) == XCHG_KEYS )
			   entry->connectionStatus = 
			      D_cieConnStatus_exchangeKeys;
		        else if ( (status & BAD_CONN) == BAD_CONN )
			   entry->connectionStatus = 
			      D_cieConnStatus_badConnection;
		        else 
			    entry->connectionStatus = 6; 
			fill_counters (entry,
			  cot[page].node[j].cib.crypto.cisco.connection_id);
       			no_of_connections++; 
        	   }
    		}
	        cur = pet[cur].hash.next;
    	     }
         }
    }

    /* 
     * Need to add code here that would go through connections maintained 
     * by VIP.
     */

    return no_of_connections; 
}

boolean 
crypto_ping_update_entry (
	unsigned long peAddr, 
        unsigned long upeAddr, long *retStatus)
{
    int status;
    ushort i, j, cur, page;

    extern pconn_entry pconn_list[];
    crypto_mykey  *key_entry = crypto_my_key_list_head;

    if ( key_entry == NULL )
	return FALSE;	/* There are no crypto engines configured! */ 

    /* pet_hash is hashed list of protected entity */
    for (i=0; i<epPEThash; i++) {
        if (pet_hash[i] != 0) { /* found a hash list */
            cur = pet_hash[i];
            while (cur != 0) {
                page = cur;
                for (j=1; j<epCOTnodes; j++)  {
                    if (cot[page].node[j].id.adr != 0 && 
			peAddr == pet[page].id.adr &&
                        upeAddr == cot[page].node[j].id.adr ) {

			status = cot[page].node[j].cib.status;
		        if ( (status & PEND_CONN) == PEND_CONN )
			   *retStatus = D_cieConnStatus_pendingConnection; 
		        else if ( (status & OPEN_NODE) == OPEN_NODE )
			   *retStatus = D_cieConnStatus_openConnection; 
		        else if ( (status & XCHG_KEYS) == XCHG_KEYS )
			   *retStatus = D_cieConnStatus_exchangeKeys;
		        else if ( (status & BAD_CONN) == BAD_CONN )
			   *retStatus = D_cieConnStatus_badConnection;
		        else 
			   *retStatus = 6; 
			if(*retStatus != D_cieConnStatus_pendingConnection)
			   return TRUE;
			else
			   return FALSE;
        	   }
    		}
	        cur = pet[cur].hash.next;
    	     }
         }
    }
    return FALSE;
}
      
