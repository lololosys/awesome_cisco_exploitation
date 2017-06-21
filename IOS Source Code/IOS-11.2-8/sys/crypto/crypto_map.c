/* $Id: crypto_map.c,v 3.1.22.19 1996/08/28 19:29:12 bew Exp $
 * $Source: /release/112/cvs/Xsys/crypto/crypto_map.c,v $
 *------------------------------------------------------------------
 * Source file for Crypto Maps
 *
 * Sept 1995, David Carrel
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: crypto_map.c,v $
 * Revision 3.1.22.19  1996/08/28  19:29:12  bew
 * CSCdi66328:  crypto connection set too early on CRM side
 * Branch: California_branch
 * Use the pending connection id on the CRM side to tag crypto maps. The
 * pending conn id is replaced by connection id when connection
 * established as on initiating side.
 *
 * Revision 3.1.22.18  1996/08/28  00:12:54  bew
 * CSCdi66046:  One peer can be set twice in same crypto map statement
 * Branch: California_branch
 * Don't add duplicate peers to the crypto map.
 *
 * Revision 3.1.22.17  1996/08/21  01:04:21  bew
 * CSCdi66372:  crypto map should only use permits to determine pe and upe
 * Branch: California_branch
 * Only include items which grant in the consideration of pe and upe
 *
 * Revision 3.1.22.16  1996/08/16  02:15:25  bew
 * CSCdi66019:  Crypto map output needs cleanup for documenting
 * Branch: California_branch
 * Remove irrelevant slot info in 'show crypto map'.
 *
 * Revision 3.1.22.15  1996/08/09  23:33:30  xliu
 * CSCdi63854:  Traffic is pass in clear text when distributed switching
 * is off
 * Branch: California_branch
 *
 * Revision 3.1.22.14  1996/08/07  21:58:49  bew
 * CSCdi65215:  crypto connection msgs not handled properly if router is
 *              destination
 * Branch: California_branch
 * If the connection message is a CIM, check each crypto map statement
 * until one matches. If a CRM or CCM which is for us, queue it up for
 * session mgr without checking the policy.
 *
 * Revision 3.1.22.13  1996/07/31  02:34:21  bew
 * CSCdi64682:  Crypto map without address causes alignment error on write
 * term
 * Branch: California_branch
 * Need to check if the address pointer is null first.
 *
 * Revision 3.1.22.12  1996/07/18  23:52:16  bew
 * CSCdi63307:  a show crypto map show slot -1 on 1005 router
 * Branch: California_branch
 * map->slot should not be set from the idb except for 7500.
 *
 * Revision 3.1.22.11  1996/07/15  10:05:07  xliu
 * CSCdi62312:  IP packets without layer 4 headers dont encrypt properly
 * Branch: California_branch
 *
 * Revision 3.1.22.10  1996/07/06  19:44:34  xliu
 * CSCdi61999:  decryption packet lost after connection already establish
 * Branch: California_branch
 *
 * Revision 3.1.22.9  1996/06/29  17:06:55  xliu
 * CSCdi59901:  Alignment error on RSP/Crypto map related
 * Branch: California_branch
 *
 * Revision 3.1.22.8  1996/06/17  22:40:48  xliu
 * CSCdi60489:  Crypto RSP crashed upon connection initiation
 * RSP crypto vector should be assinged after RP crypto vector is assigned.
 * Branch: California_branch
 *
 * Revision 3.1.22.7  1996/06/07  00:24:13  bew
 * CSCdi59169:  Crypto cleanup work
 * Branch: California_branch
 * Various cleanups & addition of "crypto ping".
 *
 * Revision 3.1.22.6  1996/05/28  15:58:55  xliu
 * CSCdi58309:  4500 has problem with encryption/decryption traffic
 * Modified crypto engine parser commands.
 * Branch: California_branch
 *
 * Revision 3.1.22.5  1996/05/12  23:12:20  xliu
 * CSCdi56023:  Fixes made after integrating with turbo IPC, and cached
 * packet mem.
 * Branch: California_branch
 *
 * Revision 3.1.22.4  1996/05/08  06:26:49  carrel
 * CSCdi56978:  Crypto Maps wont compile with named access lists
 * Branch: California_branch
 * Missed handling numbered list correctly in crypto maps
 *
 * Revision 3.1.22.3  1996/05/07  23:51:29  carrel
 * CSCdi56978:  Crypto Maps wont compile with named access lists
 * Branch: California_branch
 * Updated to use named access lists
 *
 * Revision 3.1.22.2  1996/04/23  22:04:21  bew
 * CSCdi55458:  fix usage of slot when registering crypto vectors
 * Branch: California_branch
 * Fixed a bad deference of null pointer when applying a badly named
 * crypto map to an interface.
 *
 * Revision 3.1.22.1  1996/04/19  14:55:52  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commits
 *
 * Revision 3.1.2.15  1996/04/17  23:47:22  xliu
 * Branch: IosSec_branch
 * Incorporate code review comments and removing wrong slot assumption.
 *
 * Revision 3.1.2.14  1996/04/17  23:01:13  carrel
 * Branch: IosSec_branch
 * Final commit from IP code review comments
 *
 * Revision 3.1.2.13  1996/04/16  21:52:28  bew
 * Branch: IosSec_branch
 * General code cleanup. Implemented checking of crypto map peers. CIM now
 * addressed to the src/dst rather than pe/upe.
 *
 * Revision 3.1.2.12  1996/04/15  17:27:54  carrel
 * Branch: IosSec_branch
 * Fixed my breakage in connection message processing
 *
 * Revision 3.1.2.11  1996/04/12  23:07:33  carrel
 * Branch: IosSec_branch
 * Fix wildcarding in ACL for crypto map.  Also some cleanup.
 *
 * Revision 3.1.2.10  1996/04/09  20:02:38  che
 * Branch: IosSec_branch
 * Sync to California_branch (960405)
 *
 * Revision 3.1.2.9  1996/03/30  05:11:25  xliu
 * Branch: IosSec_branch
 * Commit before CA repository split.
 *
 * Revision 3.1.2.8  1996/03/25  18:43:41  bew
 * Branch: IosSec_branch
 * Added 40-bit DES and DES 8-bit CFB support. Implemented partial
 * challenge processing support, not yet compatible with SDU.
 * Updated the crypto methods vector per Cylink's new definitions.
 *
 * Revision 3.1.2.7  1996/03/22  17:39:19  xliu
 * Branch: IosSec_branch
 * Add ipcache update to crypto map.
 *
 * Revision 3.1.2.6  1996/03/21  21:14:33  xliu
 * Branch: IosSec_branch
 * Add registry call to download crypto map.
 *
 * Revision 3.1.2.5  1996/03/21  10:07:03  xliu
 * Branch: IosSec_branch
 * Migrated to CA branch.
 *
 * Revision 3.1.2.4  1996/03/13  22:45:09  bew
 * Branch: IosSec_branch
 * The RSP/VIP environment (multiple crypto engines) are now supported
 * in crypto parser routines and crypto session mgmt code.
 *
 * Revision 3.1.2.3  1996/03/01  01:39:05  carrel
 * Branch: IosSec_branch
 * Crypto map enhancements, parser clarification and LOTS of code cleanup
 * Fixed a bug in access list processing and one in crypto maps.
 *
 * Revision 3.1.2.2  1996/02/28  19:29:49  bew
 * Branch: IosSec_branch
 * Changes relating to the Sync.
 *
 * Revision 3.1.2.1  1996/02/14  22:09:41  carrel
 * Branch: IosSec_branch
 * Add crypto_map.{c,h}
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <ciscolib.h>
#include "interface_private.h"
#include "packet.h"
#include "config.h"
#include "parser.h"
#include "../ip/ip.h"
#include "../ip/ipfast.h"
#include "../ipmulticast/igmp.h"
#include "crypto_public.h"
#include "crypto.h"
#include "crypto_pubkey.h"
#include "crypto_map.h"
#include "crypto_sesmgmt.h"
#include "crypto_ip.h"
#include "sys_registry.h"
#include "../crypto/crypto_registry.h"
#include "access.h"
#include "../parser/parser_defs_access.h"
#include "../ip/ipaccess.h"
#include "../ip/ipaccess_private.h"
#include "../ip/ipaccess_inline.h"
#include "../ip/ip_registry.h"
#include "../les/isdn_bri.h"
#include "crypto_map.h"
#include "parser_defs_crypto.h"
#include "crypto_conntable.h"
#include "crypto_engine_api.h"
#include "crypto_glue.h"
#include "sys_registry.h"

/*
 * Specifiying the algorithm in a crypto map is optional. If it is
 * not specified the global set is used. If any algorithms have been
 * set as part of the cyrpto map however, then the set is bounded by the 
 * defined global set.
 */
#define CALCULATE_ALGORITHM_SET(map) (map->algorithms ? \
    map->algorithms & crypto_get_alg_vector() : crypto_get_alg_vector())

/*
 * Globals
 */
crypto_map_headtype *crypto_maps = NULL;
int crypto_bad_conn_timeout = 10 * ONEMIN;
/*
 * Local static functions
 */
static void crypto_map_free(crypto_maptype *map);
static void crypto_map_head_free(crypto_map_headtype *head);
static crypto_map_headtype *crypto_map_head_add (char *map_tag);
static crypto_maptype *crypto_map_add(crypto_map_headtype *head, int seq_no);
static void crypto_map_head_delete(char *tag);
static boolean crypto_map_delete(crypto_map_headtype *head,
				 crypto_maptype *map);
crypto_map_headtype *find_crypto_map_head(uchar *name);
static inline crypto_maptype *find_crypto_map(crypto_map_headtype *head,
					      int seq);
static inline boolean crypto_map_match_ip_addrs_inline(paktype *, iphdrtype *,
						       acl_headertype *,
						       boolean);
void crypto_map_if_statechange_complete(idbtype *, hwidbtype *);

static
void crypto_map_free(crypto_maptype *map)
{
    int i;

    if (!map)
	return;

    for (i = 0; map->peers[i]; i++)
	free(map->peers[i]);
    free(map);
}

static
void crypto_map_head_free(crypto_map_headtype *head)
{
    crypto_maptype *map1, *map2;

    if (!head)
	return;

    free(head->map_tag);
    for (map1 = head->top; map1; map1 = map2) {
	map2 = map1->next;
	crypto_map_free(map1);
    }
    free(head);
}


/* 
 * crypto_map_head_add
 *
 * Add a crypto map head entry. This is the first entry for the crypto map
 * set.
 */
static
crypto_map_headtype *crypto_map_head_add (char *map_tag)
{
    crypto_map_headtype *head_new, **new_slot;

    head_new = malloc(sizeof(crypto_map_headtype));
    if (!head_new)
	return (NULL);
    head_new->map_tag = strdup(map_tag);
    head_new->top = NULL;
    if (!head_new->map_tag) {
	free(head_new);
	return(NULL);
    }

    for (new_slot = &crypto_maps; *new_slot; new_slot = &((*new_slot)->next));

    *new_slot = head_new;

    return(head_new);
}

/*
 * crypto_map_add
 *
 * Add specific crypto map entry to set. This function supports replacing
 * an existing entry. If the the sequence number is changed, the crypto map
 * is reordered by deleting and readding it.
 */
static
crypto_maptype *crypto_map_add (crypto_map_headtype *head, int seq_no)
{
    crypto_maptype *map_new, **map_slot;
    int slot_seq_no = 0;
    leveltype level;

    map_new = malloc(sizeof(crypto_maptype));
    if (!map_new)
	return(NULL);
    map_new->seq_no = seq_no;

    /*
     * If this is not the first statement in the map, set the slot and
     * crypto_engine_id to the same as the first crypto map statement in
     * the map. If the crypto map has been assigned to an interface, the
     * correct values will be assigned to the new statement. Also the hwidb
     * must be updated in order for the vip to get notified.
     */
    if (head->top) {
	map_new->hwidb = head->top->hwidb;
	map_new->slot = head->top->slot;
	map_new->crypto_engine_id = head->top->crypto_engine_id;
    }

    level = raise_interrupt_level(NETS_DISABLE);

    for (map_slot = &head->top; *map_slot; map_slot = &((*map_slot)->next)) {
	slot_seq_no = (*map_slot)->seq_no;
	if (seq_no < slot_seq_no) {
	    break;
	}
    }

    map_new->next = *map_slot;
    map_new->head = head;
    *map_slot = map_new;

    reset_interrupt_level(level);

    return(map_new);
}

/*
 * crypto_map_head_delete
 *
 * Delete entire crypto map entry.
 */
static
void crypto_map_head_delete (char *tag)
{
    crypto_map_headtype *head, **prevp;
    leveltype level;

    level = raise_interrupt_level(NETS_DISABLE);
    prevp = &crypto_maps;
    for (head = crypto_maps; head; head = head->next) {
	if (!strcmp(head->map_tag, tag)) {
	    *prevp = head->next;
	    break;
	}
	prevp = &head->next;
    }
    reset_interrupt_level(level);

    crypto_map_head_free(head);

    return;
}

/*
 * crypto_map_delete
 *
 * Delete specific crypto map entry or entire set.
 * Return TRUE if the whole crypto map is removed
 */
static
boolean crypto_map_delete (crypto_map_headtype *head, crypto_maptype *map)
{
    crypto_map_headtype **head_slot, *tmp_head = NULL;
    crypto_maptype **map_slot, *tmp_map = NULL;
    leveltype level;

    level = raise_interrupt_level(NETS_DISABLE);
    for (map_slot = &head->top; *map_slot; map_slot = &((*map_slot)->next)) {
	if ((*map_slot) == map) {
	    tmp_map = (*map_slot);
	    (*map_slot) = tmp_map->next;
	    break;
	}
    }
    reset_interrupt_level(level);

    if (tmp_map) {
	crypto_map_free(tmp_map);
    }

    /* Remove the head when there is no map assoicated with. */

    level = raise_interrupt_level(level);
    if (!head->top) {
	for (head_slot = &crypto_maps;
	     *head_slot;
	     head_slot = &((*head_slot)->next)) {
	    if ((*head_slot) == head) {
		tmp_head = (*head_slot);
		(*head_slot) = tmp_head->next;
		break;
	    }
	}
    }
    reset_interrupt_level(level);

    if (tmp_head) {
	crypto_map_head_free(tmp_head);
	return(TRUE);
    }

    return(FALSE);
}

/*
 * find_crypto_map_head
 * Locate crypto map entry based on tag name. This locates the first entry
 * in a set of  crypto maps that are sorted by sequence number.
 */
crypto_map_headtype *find_crypto_map_head (uchar *name)
{
    crypto_map_headtype *head;

    for (head = crypto_maps;
	 head && strcmp(name, head->map_tag);
	 head = head->next);

    return(head);
}

/*
 * find_crypto_map
 *
 * Find specific crypto map entry using supplied sequence number.
 */
static inline
crypto_maptype *find_crypto_map (crypto_map_headtype *head, int seq)
{
    crypto_maptype *map;

    for (map = head->top; map && (map->seq_no != seq); map = map->next);

    return(map);
}

/*
 * crypto_map_init
 */
void crypto_map_init (void)
{
    reg_add_crypto_map_check(crypto_map_check, "crypto_map_check");
    reg_add_ip_access_list(crypto_map_acl_changed, "crypto_map_acl_changed");
    reg_add_if_statechange_complete(crypto_map_if_statechange_complete, 
				       "crypto_map_if_statechange_complete");
}

/*
 * Show Crypto Maps
 */
void show_cryptomaps (parseinfo *csb)
{
    crypto_map_headtype *head;
    crypto_maptype *map;
    char *tag = NULL;
    boolean found_map = TRUE;

    switch (GETOBJ(int,2)) {
    case PARSER_CRYPTO_MAP_IF:
	tag = GETOBJ(idb,1)->crypto_map;
	if (!tag)
	    found_map = FALSE;
	break;
    case PARSER_CRYPTO_MAP_TAG:
	tag = GETOBJ(string,1);
	break;
    case PARSER_CRYPTO_MAP_ALL:
	break;
    }

    if (found_map) {
	found_map = FALSE;
	for (head = crypto_maps; head; head = head->next) {
	    if (!tag || !strcmp(head->map_tag, tag)) {
		found_map = TRUE;
		for (map = head->top; map; map = map->next) {
		    printf("Crypto Map \"%s\" %d\n",
			   head->map_tag, map->seq_no);
		    printf("\tConnection Id = ");
		    switch (map->conn_id) {
		    case CRYPTO_CONN_UNSET:
			printf("UNSET   ");
			break;
		    case CRYPTO_CONN_BAD:
			printf("BAD     ");
			break;
		    case CRYPTO_CONN_BLOCKED:
			printf("BLOCKED ");
			break;
		    case CRYPTO_CONN_ERROR:
			printf("ERROR   ");
			break;
		    default:
			printf("%2d      ", map->conn_id);
			break;
		    }
		    if (!NO_EXTENDED_SLOTS_ON_CHASSIS) {
			printf("Crypto Engine = %d", map->crypto_engine_id);
		    }
		    printf(" (%d established,    %d failed)\n",
			   map->num_conn, map->num_conn_err);
		    if (!map->acl_list) {
			printf("\tNo matching address list set.\n");
		    } else {
			printf("\tPE = %i\t\tUPE = %i\n", map->pe.ip_addr,
			       map->upe.ip_addr);
		    }
		}
	    }
	}
    }

    if (!found_map) {
	printf("No crypto maps found.\n");
    }
}

/*
 * crypto_map_command()
 *
 * Handles global and per-interface crypto map configuration
 *
 * (global)
 * [no] crypto map <tag> [<seq-num>]
 *	which =		PARSER_CRYPTO_MAP_GLOBAL
 *	OBJ(string,1) =	tag
 *	OBj(int,2) =	sequence number
 *
 * (interface)
 * [no] crypto map <tag>
 *	which =		PARSER_CRYPTO_MAP_INT
 *	OBJ(string,1) =	tag
 */
void crypto_map_command (parseinfo *csb)
{
    crypto_map_headtype	*head = NULL;
    crypto_maptype	*map = NULL;
    char *nv_command = NULL;
    idbtype *idb = csb->interface, *tmp_idb;
    hwidbtype *tmp_hwidb;

    if (csb->nvgen) {
	switch (csb->which) {
	case PARSER_CRYPTO_MAP_GLOBAL:
	    setstring(&nv_command, csb->nv_command);
	    if (crypto_maps) {
		nv_write(TRUE, "!");
	    }
	    /*
	     * Set up parse chain to NVGEN crypto maps and
	     * associated subcommands.
	     */
	    for (head = crypto_maps; head; head = head->next) {
		for (map = head->top; map; map = map->next) {
		    nv_write(TRUE, "%s %s %d", csb->nv_command,
			     head->map_tag, map->seq_no);
		    if (set_mode_byname(&csb->mode, "crypto-map",
					MODE_VERBOSE)) {
			*csb->nv_command = '\0';
			csb->userDataBlock[0] = map;
			recurse_node(csb, get_mode_nv(csb));
			strcpy(csb->nv_command, nv_command);
		    }
		}
	    }
	    free(nv_command);
	    break;
	case PARSER_CRYPTO_MAP_INT:
	    nv_write((boolean)idb->crypto_map, "%s %s",
		     csb->nv_command, idb->crypto_map);
	    break;
	default:
	    bad_parser_subcommand(csb, csb->which);
            break;
	}
        return;
    }

    switch (csb->which) {
    case PARSER_CRYPTO_MAP_GLOBAL:
	/*
	 * Find head of crypto map queue. If deleting return message.
	 */
	head = find_crypto_map_head(GETOBJ(string,1));
	if (!head && !csb->sense) {
	    printf("\nCould not find crypto map %s", GETOBJ(string,1));
	    return;
	}

	/* If deleting and no sequence provided, then delete entire map. */
	if (!csb->sense && (GETOBJ(int,2) == -1)) {
            if (head->top && head->top->hwidb && is_vip(head->top->hwidb)) {
                reg_invoke_crypto_vip_map_head_delete(GETOBJ(string, 1));
            }
	    crypto_map_head_delete(GETOBJ(string,1));
	    return;
	}

	/*
	 * Get specific crypto map entry based on sequence number supplied.
	 */
	if (head) {
	    map = find_crypto_map(head, GETOBJ(int,2));
	    if (!map && !csb->sense) {
		printf("\nCould not find crypto map entry %s %d", 
		       GETOBJ(string,1), GETOBJ(int,2));
		return;
	    }
	}

	/*
	 * If crypto head or map do not exist, create them. Set csb to point
	 * to crypto map for subsequent subcommands.
	 */
	if (csb->sense) {
	    if (!head) {
		head = crypto_map_head_add(GETOBJ(string,1));
		if (!head)
		    return;
                reg_invoke_crypto_vip_map_head_add((void *)head);
	    }
	    if (!map) {
		map = crypto_map_add(head, GETOBJ(int,2));
		if (!map)
		    return;
                reg_invoke_crypto_vip_map_add((char *)map);
	    }
	    if (set_mode_byname(&csb->mode, "crypto-map", MODE_VERBOSE)) {
		csb->userDataBlock[0] = map;
	    }
	} else {

	    /*
	     * Deletes crypto map. Returns TRUE if a crypto map head is
	     * removed. 
	     * Depending on the returned value inform protocols with
	     * appropriate event.
	     */
            if (map->hwidb && is_vip(map->hwidb)) {
                reg_invoke_crypto_vip_map_delete((char *)map);
            }
	    crypto_map_delete(head, map);
	    csb->userDataBlock[0] = NULL;
	}
	break;
    case PARSER_CRYPTO_MAP_INT:
	if (!csb->sense) {
	    boolean list_set = FALSE;
	    char *tmp_ptr = idb->crypto_map;
            head = find_crypto_map_head(idb->crypto_map);
	    idb->crypto_map = NULL;
	    free(tmp_ptr);

	    idb->hwptr->ip_fast_flags &= ~IP_FAST_CRYPTO_MAP;

	    FOR_ALL_SWIDBS(tmp_idb) {
		if (tmp_idb->crypto_map)
		    list_set = TRUE;
	    }
	    if (!list_set) {
		FOR_ALL_HWIDBS(tmp_hwidb) {
		    tmp_hwidb->ip_fast_flags &= ~IP_FAST_INPUT_CRYPTO_MAP;
		}
	    }
	    reg_invoke_crypto_engine_restore_media_vectors(idb->hwptr, head);
            reg_invoke_crypto_dialer_remove_map(idb->hwptr);
	} else {
            /*
             * If this is vip interface, check if distributed switching is
             * enabled. If not, don't assign the map to the interface.
             */
            if (is_vip(idb->hwptr)) {
               if (crypto_engine_tbl[idb->hwptr->slot].crypto_unit_type ==
                        CRYPTO_TYPE_UNKNOWN) {
                    printf("\nThere is no crypto engine installed on VIP2. ");
                    printf("\nPlease check VIP2 installation, and try again.");
                    return;
                }

                if (!(idb->hwptr->ip_routecache & IP_DISTRIBUTED_ROUTECACHE)){
                    printf("ERROR: The interface on VIP must be configured "
                      "with distributed switching before enabling encryption. "
                           "Please configure this interface with distribute "
                            "switching, then try again.");
                    return;
                }
            }

            head = find_crypto_map_head(GETOBJ(string,1));
	    if (!head) {
		/*
		 * This error case must return. head is dereferenced later
		 * which doesn't work very well when it is NULL ...
		 */
	    	if (csb->resolvemethod == RES_MANUAL) {
		    printf("ERROR: Crypto Map with tag \"%s\" "
                           "does not exist.\n", GETOBJ(string,1));
		}
		return;
	    }
	    setstring(&idb->crypto_map, GETOBJ(string,1));
	    idb->hwptr->ip_fast_flags |= IP_FAST_CRYPTO_MAP;
	    FOR_ALL_HWIDBS(tmp_hwidb) {
		tmp_hwidb->ip_fast_flags |= IP_FAST_INPUT_CRYPTO_MAP;
	    }
           
            map = head->top;
            for (; map;  map = map->next) {
                map->hwidb = idb->hwptr;
		/* 
		 * On an RP, both the slot and the crypto_engine_id should be 
		 * the same as the engine id (e.g., zero). On an RSP, the slot 
		 * should be the real idb slot, and the crypto_engine_id should
		 * be the slot ofthe crypto engine (e.g., idb is EIP6, crypto 
		 * engine is the RSP in slot 4).
		 *
		 * Check to make sure the slot is valid before changing the
		 * map->slot or translating to a crypto_engine_id. This catches
		 * the case where the crypto map is being placed on a Tunnel 
		 * or Dialer which has a slot of -1.
		 *
		 * Note that tha map->slot may not be initialized here before
		 * it is used, if the idb->hwptr->slot was not valid. This 
		 * assumes that the original map->slot is either initialized,
		 * or zero (which may mean uninialized). This is still better
		 * than putting -1 in the map->slot.
		 */
		if (NO_EXTENDED_SLOTS_ON_CHASSIS) {
		    map->slot = crypto_engine_rpsp_slot;
		} else {
		    if (VALID_SLOT(idb->hwptr->slot)) {
		    	map->slot = idb->hwptr->slot;
		    }
		}
		if (VALID_SLOT(map->slot)) {
                    map->crypto_engine_id = crypto_get_engine_id(map->slot);
		}
            }
	    reg_invoke_crypto_engine_register_crypto_vectors(idb->hwptr, head);
            reg_invoke_interface_cfg_modified(idb->hwptr);
	}
	reg_invoke_ipcache_invalidate(idb->hwptr, idb, TRUE);
	break;
    default:
	bad_parser_subcommand(csb, csb->which);
	break;
    }	    
}


/*
 * crypto_map_eols
 *
 * Parser function for commands within crypto map mode.
 *
 * match address
 *	OBJ(int,2) = acl #
 *  or  OBJ(string,1) = acl name
 * set peer
 *	OBJ(string,1) = peer
 * set algorithm 40-bit-des|des
 *	OBJ(int,2) = algorithm
 * set mode
 *	OBJ(int,2) = mode
 */
void crypto_map_eols(parseinfo *csb)
{
    int i;
    char *tmp_ptr;
    crypto_maptype *map = csb->userDataBlock[0];
    boolean found;

    if (csb->nvgen) {
	switch (csb->which) {
	case PARSER_CRYPTO_MAP_SET_ALG:
	    nv_write(map->algorithms & CRYPTO_DES_56, "%s %s", csb->nv_command,
		     CRYPTO_DES_56_STR);
	    nv_write(map->algorithms & CRYPTO_DES_56_8, "%s %s %s", 
		     csb->nv_command,
		     CRYPTO_DES_56_STR,
		     CRYPTO_CFB8_STR);
	    nv_write(map->algorithms & CRYPTO_DES_40, "%s %s", csb->nv_command,
		     CRYPTO_DES_40_STR);
	    nv_write(map->algorithms & CRYPTO_DES_40_8, "%s %s %s", 
		     csb->nv_command,
		     CRYPTO_DES_40_STR,
		     CRYPTO_CFB8_STR);
	    break;
	case PARSER_CRYPTO_MAP_SET_PEER:
	    for (i = 0; map->peers[i]; i++) {
		nv_write((boolean)map->peers[i], "%s %s",
			 csb->nv_command, map->peers[i]);
	    }
	    break;
	case PARSER_CRYPTO_MAP_SET_MODE:
	    if (map->mode_flags & SDU_COMPATIBLITY_FLAGS) {
	    	nv_write(TRUE, "%s sdu-compatibility", csb->nv_command);
	    }
	    break;
	case PARSER_CRYPTO_MAP_MATCH_ADDR:
	    if (map->acl_list && map->acl_list->name) {
	    	nv_write((boolean)map->acl_list, "%s %s", csb->nv_command,
		     	 map->acl_list->name);
	    }
	    break;
	}
	return;
    }

    switch (csb->which) {
    case PARSER_CRYPTO_MAP_SET_ALG:
	if (!csb->sense) {
	    map->algorithms &= ~(GETOBJ(int,2));
	} else {
	    map->algorithms |= GETOBJ(int,2);
	}
	break;
    case PARSER_CRYPTO_MAP_SET_PEER:
	if (!csb->sense) {
	    boolean found = FALSE;
	    for (i = 0; map->peers[i]; i++) {
		if (!strcmp(map->peers[i], GETOBJ(string,1))) {
		    tmp_ptr = map->peers[i];
		    for (; map->peers[i]; i++) {
			map->peers[i] = map->peers[i+1];
		    }
		    free(tmp_ptr);
		    found = TRUE;
		}
	    }
	    if (!found)
		printf("No such peer \"%s\" to remove.\n", GETOBJ(string,1));
	} else {
	    if ((csb->resolvemethod == RES_MANUAL) &&
		!crypto_pubkey_get_by_name(GETOBJ(string,1))) {
		printf("WARNING: No key present for peer \"%s\".\n",
		       GETOBJ(string,1));
	    }
	    for (i = 0, found = FALSE; i < CRYPTO_MAP_MAX_PEERS; i++) {
		if (!map->peers[i])
		    break;
		/*
		 * If this peer is already in the list (i.e., this is a
		 * duplicate), mark it as found.
		 */
		if (!strcmp(map->peers[i], GETOBJ(string,1))) {
		    found = TRUE;
		    break;
		}
	    }
	    if (i == CRYPTO_MAP_MAX_PEERS) {
		printf("Unable to set peer.  "
		       "Maximum number of peers (%d) exceeded.\n",
		       CRYPTO_MAP_MAX_PEERS);
	    } else {
		/*
		 * Only add it if not already found in the list.
		 */
	    	if (!found) {
		    setstring(&map->peers[i], GETOBJ(string,1));
	    	}
	    }
	}
	break;
    case PARSER_CRYPTO_MAP_SET_MODE:
	if (!csb->sense) {
	    map->mode_flags &= (~SDU_COMPATIBLITY_FLAGS);
	} else {
	    map->mode_flags |= SDU_COMPATIBLITY_FLAGS;
	}
        if (map->hwidb && is_vip(map->hwidb)) {
            reg_invoke_crypto_vip_map_set_modeflags((char *)map);
        }
	break;
    case PARSER_CRYPTO_MAP_MATCH_ADDR:
	if (!csb->sense) {
	    map->acl_list = NULL;
	    map->pe.type = map->upe.type = 0;
	} else {
	    int list;
	    char *name;
	    list = GETOBJ(int,2);
	    name = GETOBJ(string,1);

	    if (list) {
		map->acl_list = access_find_or_create_acl(list);
	    } else {
		if (!is_valid_nacl(name)) {
		    printf(nacl_name_error);
		    return;
		}
                map->acl_list =
		    access_find_or_create_nacl(name, ACL_IP_NAMED_EXTENDED);
		if (!map->acl_list) {
		    printf("ERROR: \"%s\" is either an invalid name or the\n"
			   "\tlist already exists but is the wrong type.\n",
			   name);
		    return;
		}
	    }

	    /* Walk list and build PE and UPE values. */
	    crypto_map_build_pe_upe(map);
            if (map->hwidb && is_vip(map->hwidb)) {
                reg_invoke_crypto_vip_map_set_peupe((char *)map);
            }
	}
	break;
    }
}


/*
 * crypto_map_replace_conn_id
 * called by connection manager to walk ALL CMs and replace all with
 * a given connection id.
 */
void crypto_map_replace_conn_id (int old, int new, ushort engine_id)
{
    crypto_map_headtype *head;
    crypto_maptype *map;
    for (head = crypto_maps; head; head = head->next) {
	for (map = head->top; map; map = map->next) {
	    if ((map->conn_id == old) && (map->crypto_engine_id == engine_id)) {
		map->conn_id = new;
                reg_invoke_crypto_vip_upd_map_conn_id((void*)map, old);
		if (map->hwidb && map->hwidb->firstsw) {
	            reg_invoke_ipcache_invalidate(map->hwidb, 
						  map->hwidb->firstsw,
                                              	  TRUE);
		}
                /* Only clear the dh number when a new connection is set */
                if (map->conn_id !=0)
                    crypto_clear_dh_numbers(
                         map->crypto_engine_id, map->conn_id);
            }
	}
    }
}

/*
 * crypto_map_acl_changed
 *
 * Called whenever an ACL changes and we need to update the PE and UPE values
 */
void crypto_map_acl_changed (acl_headertype *acl_list)
{
    crypto_map_headtype *head;
    crypto_maptype *map;

    if (!acl_list || !acl_list->name)
	return;

    for (head = crypto_maps; head; head = head->next) {
	for (map = head->top; map; map = map->next) {
            if (!map->acl_list || !map->acl_list->name) {
                continue;
            }
	    if (!strcmp(map->acl_list->name, acl_list->name)) {
		crypto_map_build_pe_upe(map);
	    }
	}
    }
}

/*
 * crypto_map_build_pe_upe
 *
 * Update the PE and UPE values based on the access list
 * Only use the "permit" entries since only they define the PE and UPE.
 */
void crypto_map_build_pe_upe (crypto_maptype *map)
{
    if ((map->acl_list->type == ACL_IP_NAMED_EXTENDED) ||
	(map->acl_list->type == ACL_IP_EXTENDED)) {
	slowtype *item;

	map->pe.type = map->upe.type = ADDR_IP;
	map->pe.length = map->upe.length = ADDRLEN_IP;
	map->pe.ip_addr = map->upe.ip_addr = MASK_HOST;	/* Max */

	for (item = (slowtype *)map->acl_list->list; item; item = item->next) {
	    if (!item->grant) {
		continue;
	    }
	    if (item->source < map->pe.ip_addr)
		map->pe.ip_addr = item->source;
	    if (item->destination < map->upe.ip_addr)
		map->upe.ip_addr = item->destination;
	}
	if (map->pe.ip_addr == 0) {
	    map->pe.ip_addr = 1;	/* Must not be zero. */
	}
	if (map->upe.ip_addr == 0) {
	    map->upe.ip_addr = 1;	/* Must not be zero. */
	}
    } else {
	map->pe.type = map->upe.type = 0;
    }
}

/*
 * crypto_map_check
 *
 * This is the main crypto map function.  It can be called at either
 * process or interrupt level.
 *
 * Returns TRUE when the calling function should handle packet based
 *	on values in pak->cryptoflags
 * Returns FALSE when the calling function should no longer reference the pak
 *	or in fastswitch to drop to process switch.
 */
boolean crypto_map_check (paktype *pak, char *crypto_map, int action,
			  boolean fastswitch)
{
    crypto_map_headtype *head;
    crypto_maptype *map;
    int algorithms;
    boolean ret;
    ipaddrtype phost, uphost;
    iphdrtype *ip;
    crypto_sesmgmt_message_types cs_msg_type;

    for (head = crypto_maps; head; head = head->next) {
	if (!strcmp(crypto_map, head->map_tag))
	    break;
    }

    ip = (iphdrtype *)ipheadstart(pak);

    /*
     * If a connection message is found, queue it up for session mgr.
     * If a packet is marked at a connection message we've sourced, just
     * return TRUE.
     */
    if (crypto_ip_found_connection_message(ip)) {
	if (pak->cryptoflags & CRYPTO_CM) {
	    return(TRUE);	/* Send it on its merry little way */
        }

	if (fastswitch) {
	    return(FALSE);	/* Drop to process switching. */
	}
    
	/*
	 * Find the connection message type. The CIM is addressed
	 * to the end host so we need to do a crypto map check. Otherwise,
	 * (CRM, CCM) the message should be addressed to us and we accept it. 
	 * If there is an error, we just don't understand the connection
	 * message, so should let it go through untouched. Either some 
	 * Cylink device will understand it, or it will get reflected back to 
	 * the originator who will deal with the fact there is no peer.
	 */
	cs_msg_type = crypto_ip_find_connection_msg_type(pak);
	if (cs_msg_type == Unsuccessful_Message) {
	    return(TRUE);
	}

	if (cs_msg_type == CIM) {

	    if (!head) {	/* Couldn't find map head */
	        return(TRUE);	/* Allow the connection message to pass. */
	    }

	    for (map = head->top; map; map = map->next) {
		if (!map->acl_list || !map->acl_list->list || !map->peers[0]) {
		    continue;
		}
		if (crypto_map_match_ip_addrs_inline(pak, ip, map->acl_list,
						     action == CRYPTO_DECRYPT)){
		    algorithms = CALCULATE_ALGORITHM_SET(map);
		    if (crypto_sm_debug) {
			ipbug(pak, "Found an ICMP connection message.\n");
		    }
		    /*
		     * Pending conn id set in crypto_ip_queue_conn_message().
		     */
		    crypto_ip_queue_conn_message(pak, map, algorithms);
		    return(FALSE);
		}
	    }
	    /* No matching Crypto Map found. Let the connection msg pass.*/
	    return(TRUE);
	} else { /* !CIM so it must be CRM or CCM */
	    if (ip_forus(pak)) {
		/* 
		 * No map passed; connection info should be in pending 
		 * connection table.
		 */
		crypto_ip_queue_conn_message(pak, NULL, 0);
		return(FALSE);
	    } else { /* Not for us */
		return(TRUE);
	    }
	}
    } else {
/* DAVE -- check for protocols not to encrypt */
    }

    if (!head || !head->top) {	/* Couldn't find map head */
	pak->cryptoflags |= CRYPTO_DENY;
	return(TRUE);
    }

    for (map = head->top; map; map = map->next) {
	if (!map->acl_list || !map->acl_list->list || !map->peers[0]) {
	    /* An incomplete Crypto Map means NO TRAFFIC should pass */
	    pak->cryptoflags |= CRYPTO_DENY;
	    return(TRUE);
	}
	pak->cryptoflags &= ~CRYPTO_ACL_LOG;
	if (action == CRYPTO_DECRYPT) {
	    ret = ip_accesscheck_reverse_inline(pak, map->acl_list, NULL,
						fastswitch);
	    uphost = GETLONG(&ip->srcadr);
	    phost  = GETLONG(&ip->dstadr);
	} else {
	    ret = ip_accesscheck_inline(pak, map->acl_list, NULL, fastswitch);
	    phost  = GETLONG(&ip->srcadr);
	    uphost = GETLONG(&ip->dstadr);
	}
	if (ret == TRUE) {	/* we matched */
	    if ((map->conn_id < CRYPTO_CONN_MINIMUM) &&
		TIMER_RUNNING_AND_AWAKE(map->bad_conn_timer)) {
		TIMER_STOP(map->bad_conn_timer);
		map->conn_id = CRYPTO_CONN_UNSET;
	    }
	    if (map->conn_id == CRYPTO_CONN_UNSET) {
		algorithms = CALCULATE_ALGORITHM_SET(map);
		/* Assume IP.  This must change when supporting others. */
		crypto_conn_entry_initiate_ip(&map->conn_id, map->pe.ip_addr,
					      map->upe.ip_addr,
					      phost, uphost,
                                              map->crypto_engine_id,
					      algorithms, map->peers,
					      map->mode_flags);
		if (map->conn_id < CRYPTO_CONN_MINIMUM) {
		    TIMER_START(map->bad_conn_timer, crypto_bad_conn_timeout);
		    map->num_conn_err++;
		} else {
		    map->num_conn++;
		}

		/* Check if we need to update the VIP connection table. */
		if (is_vip(map->hwidb) && (map->conn_id > 0)) {
		    reg_invoke_crypto_vip_upd_map_conn_id((void *)map, 
                                                           map->slot);
		}

                pak->cryptoflags |= CRYPTO_CONN_NON_EXIST;

	    }
	    if (action == CRYPTO_DECRYPT) {
	    	pak->crypto_dec_conn_id = map->conn_id;
	    } else {
		pak->crypto_enc_conn_id = map->conn_id;
	    }
	    pak->cryptoflags |= action;
	    break;
	} else if ((ret == FALSE) && (pak->cryptoflags & CRYPTO_ACL_LOG)) {
	    break; /* we matched but logging is on, go to process level */
        }
    }

    if ((fastswitch == TRUE) &&
	(pak->cryptoflags & (CRYPTO_ENCRYPT | CRYPTO_DECRYPT)) &&
	(get_ip_morefragments(ip) || (getfo(ip) != 0))) 
	return(FALSE);
    else if ((fastswitch == TRUE) && (pak->cryptoflags & CRYPTO_ACL_LOG)) {
	pak->cryptoflags &= ~CRYPTO_ACL_LOG;
	return(FALSE); /* drop to process level if log is on */
    } else
	return(TRUE);
}

static inline
boolean crypto_map_match_ip_addrs_inline (paktype *pak, iphdrtype *ip,
					  acl_headertype *list,
					  boolean reverse)
{
    slowtype *item, *next;
    ipaddrtype source, destination;

    if ((list->type != ACL_IP_NAMED_EXTENDED) &&
	(list->type != ACL_IP_EXTENDED))
	return(FALSE);

    item = (slowtype *)list->list;
    if (item == NULL)
	return(FALSE);

    /* Now set source and dest depending on our encryption direction */
    if (reverse) {
	destination = GETLONG(&ip->srcadr);
	source = GETLONG(&ip->dstadr);
    } else {
	source = GETLONG(&ip->srcadr);
	destination = GETLONG(&ip->dstadr);
    }

    for (; item; item = next) {
	next = item->next;
	/* DAVE -- what about dynamic lists?? */
/* DAVE -- should this inline be in ipaccess_inline.h instead ??*/
	if (item->source != (source & ~item->srcmask))
	    continue;
	if (item->destination != (destination & ~item->dstmask))
	    continue;
	return(item->grant);
    }

    return(FALSE);
}

/*
 * Find the algorithm and peer for a particular crypto map statement.
 */
boolean find_crypto_map_policy (char *name, int statement, addrtype *pe,
				addrtype *upe, int *alg, char ***peers) {
    crypto_map_headtype *head;
    crypto_maptype *map;

    head = find_crypto_map_head(name);
    if (!head) {
	return(FALSE);
    }
    map = find_crypto_map(head, statement);
    if (!map) {
	return(FALSE);
    }

    *alg = CALCULATE_ALGORITHM_SET(map);
    *peers = map->peers;
    *pe = map->pe;
    *upe = map->upe;
    return(TRUE);
}

/*
 * Drop all crypto connections from a crypto map.
 */
void crypto_map_drop_connections (char *crypto_map)
{
    crypto_map_headtype *head;
    crypto_maptype *map;

    head = find_crypto_map_head(crypto_map);
    if (!head) {
	return;
    }
    for (map = head->top; map; map = map->next) {
	if (map->conn_id != CRYPTO_CONN_UNSET) {
	    crypto_glue_clear_connection(map->conn_id, map->crypto_engine_id);
	}
    }
    return;
}

/*
 * Clear the connections from any crypto maps applied to software idbs
 * associated with an interface.
 */
void crypto_map_clear_interface (hwidbtype *hwidb)
{
    idbtype *idb;

    FOR_ALL_SWIDBS_ON_HW(hwidb, idb) {
	if (idb->crypto_map) {
	    crypto_map_drop_connections(idb->crypto_map);
	}
    }
 }

#define PRIMARY_INTERFACE (hwidb->secondary && hwidb->backup_state)
#define BACKUP_INTERFACE  (hwidb->standby)

/*
 * The following function gets called after every interface state change.
 * If the interface is going down due to an interface backup switchover, all
 * crypto connections in any crypto map asociated with the interface are
 * cleared. This is done in case the backup interface leads to a different
 * encryption peer than the primary case.
 *
 * The definition of an interface bacup switchover is one of the following
 * events:
 *   A primary interface is being shutdown.
 *   A backup inteface is being administratively shutdown.
 */
void crypto_map_if_statechange_complete (idbtype *idb, hwidbtype *hwidb)
{
    if (hwidb) {
	if (IS_ENCRYPTION(hwidb) &&
	    ((PRIMARY_INTERFACE && (hwidb->state == IDBS_DOWN)) ||
	    (BACKUP_INTERFACE  && (hwidb->state == IDBS_ADMINDOWN)))) {
	    crypto_map_clear_interface(hwidb);
	}
    }

    if (idb) {
	/*
	 * Usually hwidb is NULL when idb has a value. If so, reset hwidb.
	 */
	if (!hwidb) {
	    hwidb = idb->hwptr;
	}
	if (IS_ENCRYPTION(hwidb)) {
	    if (is_subinterface(idb)) {
		/*
		 * A subinterface cannot be a backup interface as of this 
		 * writing. In fact there isn't yet enough state in an idb to 
		 * determine whether a subinterface is a backup or not. We can 
		 * only check for the primary subinterface case. 
		 *
		 * idb->backup and idb->subif_state are subinterface-only fields
		 */
		if (idb->backup && (idb->subif_state == SUBIF_STATE_DOWN)) {
		    crypto_map_drop_connections(idb->crypto_map);
		}
	    } else {
		if ((PRIMARY_INTERFACE && (hwidb->state == IDBS_DOWN)) ||
		    (BACKUP_INTERFACE  && (hwidb->state == IDBS_ADMINDOWN))) {
		    crypto_map_drop_connections(idb->crypto_map);
		}
	    }
	}
    }
} 
