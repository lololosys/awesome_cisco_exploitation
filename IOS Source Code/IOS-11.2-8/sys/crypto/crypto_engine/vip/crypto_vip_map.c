/* $Id: crypto_vip_map.c,v 1.1.6.6 1996/08/07 09:22:14 xliu Exp $
 * $Source: /release/112/cvs/Xsys/crypto/crypto_engine/vip/crypto_vip_map.c,v $
 *------------------------------------------------------------------
 * crypto_vip_map.c   crypto map on VIP. 
 *
 * Mar, 1996, xliu 
 *
 * Copyright (c) 1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: crypto_vip_map.c,v $
 * Revision 1.1.6.6  1996/08/07  09:22:14  xliu
 * CSCdi63274:  Wrong crypto engine name a show cr eng conf
 * Branch: California_branch
 *
 * Revision 1.1.6.5  1996/07/03  05:55:11  snijsure
 * CSCdi60711:  sh cr conn eng act does not show anything in vip2 crypto
 * router
 * Branch: California_branch
 *
 * Revision 1.1.6.4  1996/05/28  16:00:16  xliu
 * CSCdi58309:  4500 has problem with encryption/decryption traffic
 * Modified crypto engine parser commands.
 * Branch: California_branch
 *
 * Revision 1.1.6.3  1996/05/12  23:13:41  xliu
 * CSCdi56023:  Fixes made after integrating with turbo IPC, and cached
 * packet mem.
 * Branch: California_branch
 *
 * Revision 1.1.6.2  1996/05/07  23:52:06  carrel
 * CSCdi56978:  Crypto Maps wont compile with named access lists
 * Branch: California_branch
 * Updated to use named access lists
 *
 * Revision 1.1.6.1  1996/04/19  14:57:22  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commits
 *
 * Revision 1.1.4.3  1996/04/17  23:48:36  xliu
 * Branch: IosSec_branch
 * Incorporate code review comments and removing wrong slot assumption.
 *
 * Revision 1.1.4.2  1996/04/09  20:03:33  che
 * Branch: IosSec_branch
 * Sync to California_branch (960405)
 *
 * Revision 1.1.4.1  1996/03/30  06:35:59  xliu
 * Branch: IosSec_branch
 * Commit before CA split.
 *
 * Revision 1.1  1996/03/30  06:03:50  xliu
 * Placeholder files.
 *
 * Revision 1.7  1995/06/07 12:03:03  smackie
 * Fold back Arizona_branch into mainline.
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
#include "ttysrv.h"
#include "../ip/ip.h"
#include "../os/chunk.h"
#include "../ipmulticast/igmp.h"
#include "../crypto/crypto_public.h"
#include "../crypto/crypto_registry.h"
#include "../os/chunk.h"
#include "access.h"
#include "../parser/parser_defs_access.h"

#include "../src-vip/dfs_vip_idb.h"

#include "../ip/ipaccess.h"
#include "../ip/ipaccess_private.h"
#include "../ip/ipaccess_inline.h"
#include "../ip/ip_registry.h"
#include "../ip/ipfast_ipc.h"
#include "../ip/ipfast_index.h"
#include "../ip/ipfast_vip.h"

#include "../crypto/crypto_map.h"
#include "../crypto/parser_defs_crypto.h"
#include "../crypto/crypto_public.h"
#include "../crypto/crypto.h"
#include "../crypto/crypto_engine_api.h"
#include "../crypto/crypto_engine/core/crypto_engine.h"
#include "crypto_engine_vip.h"

#define CRYPTO_MAP_TAG_MAX_SIZE  10

/*
 * Globals
 */
crypto_map_headtype *crypto_maps = NULL;
int crypto_bad_conn_timeout = 10 * ONEMIN;


void crypto_map_free(crypto_maptype *map)
{
    if (!map)
	return;
    chunk_free(crypto_map_chunk,map);
}

void crypto_map_head_free(crypto_map_headtype *head)
{
    crypto_maptype *map1, *map2;

    if (!head)
	return;

    chunk_free(crypto_map_tag_chunk,head->map_tag);
    for (map1 = head->top; map1; map1 = map2) {
	map2 = map1->next;
	crypto_map_free(map1);
    }
    chunk_free(crypto_map_head_chunk, head);
}


/* 
 * crypto_map_head_add
 *
 * Add a crypto map head entry. This is the first entry for the crypto map
 * set.
 */
crypto_map_headtype *crypto_map_head_add (char *map_tag, ushort tag_size)
{
    crypto_map_headtype *head_new, **new_slot;

    head_new = chunk_malloc(crypto_map_head_chunk);
    if (!head_new)
	return (NULL);
    head_new->map_tag = chunk_malloc(crypto_map_tag_chunk);
    if (!head_new->map_tag) {
	chunk_free(crypto_map_head_chunk,head_new);
        return NULL;
    }

    bcopy(map_tag, head_new->map_tag, tag_size);
    head_new->top = NULL;

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
crypto_maptype *crypto_map_add (crypto_map_headtype *head, int seq_no)
{
    crypto_maptype *map_new, **map_slot;
    int slot_seq_no = 0;

    map_new = chunk_malloc(crypto_map_chunk);
    if (!map_new)
	return(NULL);
    map_new->seq_no = seq_no;

    /*
     * If the map head has been assgined to a given interface, set the map
     * hwidb.
     */
    if (head->top) {
        map_new->hwidb = head->top->hwidb;
    }

    for (map_slot = &head->top; *map_slot; map_slot = &((*map_slot)->next)) {
	slot_seq_no = (*map_slot)->seq_no;
	if (seq_no < slot_seq_no) {
	    break;
	}
    }

    map_new->next = *map_slot;
    *map_slot = map_new;

    return(map_new);
}

/*
 * crypto_map_head_delete
 *
 * Delete entire crypto map entry.
 */
void crypto_map_head_delete (char *tag, ushort tag_size)
{
    crypto_map_headtype *head, **prevp;

    prevp = &crypto_maps;
    for (head = crypto_maps; head; head = head->next) {
	if (!strncmp(head->map_tag, tag, tag_size)) {
	    *prevp = head->next;
	    crypto_map_head_free(head);
	    break;
	}
	prevp = &head->next;
    }

    return;
}

/*
 * crypto_map_delete
 *
 * Delete specific crypto map entry or entire set.
 * Return TRUE if the whole crypto map is removed
 */
boolean crypto_map_delete (crypto_map_headtype *head, crypto_maptype *map)
{
    crypto_map_headtype **head_slot, *tmp_head = NULL;
    crypto_maptype **map_slot, *tmp_map = NULL;

    for (map_slot = &head->top; *map_slot; map_slot = &((*map_slot)->next)) {
	if ((*map_slot) == map) {
	    tmp_map = (*map_slot);
	    (*map_slot) = tmp_map->next;
	    break;
	}
    }

    if (tmp_map) {
	crypto_map_free(tmp_map);
    }

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
crypto_map_headtype *vip_find_crypto_map_head (uchar *name, ushort size)
{
    crypto_map_headtype *head;

    for (head = crypto_maps;
	 head && strncmp(name, head->map_tag, size);
	 head = head->next);

    return(head);
}

/*
 * find_crypto_map
 *
 * Find specific crypto map entry using supplied sequence number.
 */
crypto_maptype *find_crypto_map (crypto_map_headtype* head, int seq)
{
    crypto_maptype *map = NULL;

    for (map = head->top; map && (map->seq_no != seq); map = map->next);

    return(map);
}

/*
 * crypto_map_init
 */
void crypto_map_init (void)
{
#if 0
    reg_add_crypto_map_check(crypto_map_check, "crypto_map_check");
    reg_add_ip_access_list(crypto_map_acl_changed, "crypto_map_acl_changed");
#endif
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
    boolean ret;

    pak->cryptoflags = 0;
    if (action == CRYPTO_DECRYPT) {
    	pak->crypto_dec_conn_id = 0;
    } else {
    	pak->crypto_enc_conn_id = 0;
    }

    for (head = crypto_maps; head; head = head->next) {
	if (!strncmp(crypto_map, head->map_tag, strlen(crypto_map)))
	    break;
    }

    if (!head) {	/* Couldn't find map head */
	pak->cryptoflags |= CRYPTO_DENY;
        if (crypto_engine_vip_debug) {
            buginf("\nCRYPTO_MAP: empty map head ->deny: %x", pak);
        }
	return(TRUE);
    }

    for (map = head->top; map; map = map->next) {
	if (!map->acl_list || !map->acl_list->list) {
            if (!map->acl_list_number || !acl_array[map->acl_list_number]) {
	        /* An incomplete Crypto Map means NO TRAFFIC should pass */
                if (crypto_engine_vip_debug) {
                    buginf("\nCRYPTO_MAP:incomplete map ->deny:" 
                       "map name: %s, map acl ptr: %x, map acl list %x",
                       head->map_tag, map->acl_list, map->acl_list->list);
                }
	        pak->cryptoflags |= CRYPTO_DENY;
	        return(TRUE);
            } else {
                map->acl_list = acl_array[map->acl_list_number];
            }
	}
	pak->cryptoflags &= ~CRYPTO_ACL_LOG;
	if (action == CRYPTO_DECRYPT) {
	    ret = ip_accesscheck_reverse_inline(pak, map->acl_list, NULL,
								TRUE);
	} else {
	    ret = ip_accesscheck_inline(pak, map->acl_list, NULL, TRUE);
	}
	if ((ret == TRUE) ||
            ((ret == FALSE) && (pak->cryptoflags & CRYPTO_ACL_LOG))) { /* we matched */
	    pak->cryptoflags &= ~CRYPTO_ACL_LOG;
	    if (map->conn_id < CRYPTO_CONN_MINIMUM) {
		map->conn_id = CRYPTO_CONN_UNSET;
	    }
	    if (action == CRYPTO_DECRYPT) {
	    	pak->crypto_dec_conn_id = map->conn_id;
	    } else {
	    	pak->crypto_enc_conn_id = map->conn_id;
	    }
	    pak->cryptoflags |= action;
	    break;
	}
    }

    return(TRUE);
}

void crypto_vip_show_map (void)
{
    crypto_map_headtype *head;
    crypto_maptype *map;

    for (head = crypto_maps; head; head = head->next) {
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
            printf("     (%d established,    %d failed)\n",
                   map->num_conn, map->num_conn_err);
            if (!map->acl_list) {
                if (!map->acl_list_number || 
                    !acl_array[map->acl_list_number]) {
                    printf("\tNo matching address list set.\n");
                } else {
                    map->acl_list = acl_array[map->acl_list_number];
                }
            } else {
                printf("\tPE = %x\t\tUPE = %x\n", map->pe.ip_addr,
                       map->upe.ip_addr);
            }
	    if (map->mode_flags) {
		printf("Mode: %x\n", map->mode_flags);
	    }
        }
    }
}




