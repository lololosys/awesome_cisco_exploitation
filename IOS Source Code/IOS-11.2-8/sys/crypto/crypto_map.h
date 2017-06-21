/* $Id: crypto_map.h,v 3.1.22.3 1996/05/12 23:12:21 xliu Exp $
 * $Source: /release/112/cvs/Xsys/crypto/crypto_map.h,v $
 *------------------------------------------------------------------
 * Header file for crypto_map.c
 *
 * Sept 1995, David Carrel
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: crypto_map.h,v $
 * Revision 3.1.22.3  1996/05/12  23:12:21  xliu
 * CSCdi56023:  Fixes made after integrating with turbo IPC, and cached
 * packet mem.
 * Branch: California_branch
 *
 * Revision 3.1.22.2  1996/05/07  23:51:32  carrel
 * CSCdi56978:  Crypto Maps wont compile with named access lists
 * Branch: California_branch
 * Updated to use named access lists
 *
 * Revision 3.1.22.1  1996/04/19  14:55:53  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commits
 *
 * Revision 3.1.2.8  1996/04/17  23:47:23  xliu
 * Branch: IosSec_branch
 * Incorporate code review comments and removing wrong slot assumption.
 *
 * Revision 3.1.2.7  1996/04/16  21:52:30  bew
 * Branch: IosSec_branch
 * General code cleanup. Implemented checking of crypto map peers. CIM now
 * addressed to the src/dst rather than pe/upe.
 *
 * Revision 3.1.2.6  1996/03/30  05:11:28  xliu
 * Branch: IosSec_branch
 * Commit before CA repository split.
 *
 * Revision 3.1.2.5  1996/03/22  17:39:20  xliu
 * Branch: IosSec_branch
 * Add ipcache update to crypto map.
 *
 * Revision 3.1.2.4  1996/03/21  21:14:40  xliu
 * Branch: IosSec_branch
 * Add registry call to download crypto map.
 *
 * Revision 3.1.2.3  1996/03/21  10:07:04  xliu
 * Branch: IosSec_branch
 * Migrated to CA branch.
 *
 * Revision 3.1.2.2  1996/03/01  01:39:07  carrel
 * Branch: IosSec_branch
 * Crypto map enhancements, parser clarification and LOTS of code cleanup
 * Fixed a bug in access list processing and one in crypto maps.
 *
 * Revision 3.1.2.1  1996/02/14  22:09:42  carrel
 * Branch: IosSec_branch
 * Add crypto_map.{c,h}
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef CRYPTO_MAP_H_
#define CRYPTO_MAP_H_

#define CRYPTO_MAP_SEQ_MAX	0xffff
#define CRYPTO_MAP_SEQ_STEP	10
#define CRYPTO_MAP_MAX_PEERS	40

/*
 * The mode flags indicate special characteristics of a crypto map.
 * The flags are meant to be low-level characteristics which can be
 * grouped to provide a  crypto map "mode", such as "sdu-compatibility".
 * The grouping allows us to add newly-discovered incompatibilities to
 * the mode, in escence fixing bugs in that mode without affecting 
 * compatibility in the entire customer base.
 */
#define CRYPTO_MAP_MODE_FLAGS_SYNDROME 0x00000001

#define SDU_COMPATIBLITY_FLAGS (CRYPTO_MAP_MODE_FLAGS_SYNDROME)

typedef struct crypto_maptype_ {
    struct crypto_maptype_	*next;
    int				seq_no;
    int				conn_id;
    sys_timestamp		bad_conn_timer;
    addrtype			pe;
    addrtype			upe;

    /* Set parameters */
    char			*peers[CRYPTO_MAP_MAX_PEERS + 1];
    int				algorithms;
    int				key_timeout;
    uint			mode_flags;

    /* Match parameters */
    acl_headertype		*acl_list;
    int                         acl_list_number;

    /* Statistics */
    int				num_conn;	/* # of conns established */
    int				num_conn_err;	/* # of conns that fail */
    hwidbtype                   *hwidb;
    int                         slot;
    void                        *head;
    ushort                      crypto_engine_id;

} crypto_maptype;

typedef struct crypto_map_headtype_ {
    struct crypto_map_headtype_	*next;
    char			*map_tag;
    struct crypto_maptype_	*top;
} crypto_map_headtype;

#define CRYPTO_CONN_UNDEFINE(x) (((x) == CRYPTO_CONN_UNSET) || ((x) == CRYPTO_CONN_MINIMUM)? TRUE:FALSE)

void crypto_map_init(void);
boolean crypto_map_check(paktype *, char *, int, boolean);
void crypto_map_command(parseinfo *csb);
void show_cryptomaps(parseinfo *csb);
void crypto_map_eols(parseinfo *csb);
extern void crypto_map_replace_conn_id(int old, int new, ushort slot);
void crypto_map_build_pe_upe(crypto_maptype *map);
void crypto_map_acl_changed(acl_headertype *acl_list);
boolean find_crypto_map_policy(char *, int, addrtype *, addrtype *, int *, 
			       char ***);
crypto_map_headtype *find_crypto_map_head(uchar *);

extern crypto_map_headtype *crypto_maps;

#endif /* CRYPTO_MAP_H_ */
