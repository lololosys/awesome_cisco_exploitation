/* $Id: at_static.h,v 3.2.60.1 1996/03/23 01:27:42 slin Exp $
 * $Source: /release/111/cvs/Xsys/atalk/at_static.h,v $
 *------------------------------------------------------------------
 * Header file for AppleTalk static routing.
 *
 * November 1993, Grisha Kotlyar
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 * 
 * This file contains all typedefs , globals and externs 
 * for AppleTalk static routing module.
 *------------------------------------------------------------------
 * $Log: at_static.h,v $
 * Revision 3.2.60.1  1996/03/23  01:27:42  slin
 * Branch: California_branch
 *
 * AppleTalk makes its exodus from the idb.
 *
 * Revision 3.2  1995/11/17  08:43:56  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  10:57:55  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:09:32  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

boolean    atalk_static;          /* Global. When TRUE then
                                   * initialization of static module
				   * has been completed
				   */

/* The following struct describes 
 * a static zone list 
 */
typedef struct a_static_zonelist_ {
    struct a_static_zonelist_ *next;    /* Points to next */
    char   zonename[MAX_ATZONE_NAME];   /* Zone name */
    ushort zonelen;                     /* Actual zone length */
} a_static_zonelist;

/* The following struct describes 
 * main AppleTalk static list 
 */
typedef struct a_static_ {
  struct  a_static_    *next;  /* Points to next */
  struct  a_static_    *prev;  /* Points to previous */
  ushort  start;       /* Start of the range for Phase  2 or net for Phase 1 */
  ushort  end;         /* End of the range; 0 for phase 1 */
  ataddress *nextaddr; /* AT address of the next router */
  ushort  hops;         /* Hop count */
  ushort  test_entry;   /* Entry is for testing purpose */
  atalkidbtype *idb;        /* Interface */
  atalk_rdb_t *r;      /* Routing entry */
  ushort  flags;       /* See flags below */
  a_static_zonelist *z_hdr;    /* Header for the static zone list */
  boolean floating;    /* the entry is floating one */
}a_static;

/* static flags definitions */
#define AT_STATIC_NONACTIVE   0
#define AT_STATIC_ACTIVE 0x0001

extern a_static  *a_static_head;  /* Head of the static list */

/*
 *************** Internals ********************
 */

void    atalk_static_command (parseinfo *csb);
boolean atalk_static_assign(parseinfo *csb);
boolean at_static_insert(ushort, ushort, ataddress *, ushort, uchar *, ushort, ushort, boolean);
boolean at_insert_static_route( a_static *as);
a_static *at_static_new(ushort , ushort, ataddress *, ushort, atalkidbtype *,
			ushort, ushort, boolean);
boolean at_static_put_zone(a_static *as, char *zname);
void    atstatic_Initialize(void);
void    nv_atalk_static(parseinfo *);
atalkidbtype *at_get_nextaddr_idb(ataddress *);
a_static *at_find_static(ushort net);
boolean  at_static_noprefix(ushort start, ushort end, ataddress *nextaddr);
void     at_static_delete (a_static *as);
boolean  at_static_match(ushort start, ushort end, ataddress *nextaddr, a_static *as);
void     at_static_PathAndRoute_Delete (a_static *);
boolean  at_static_FindZone(a_static *, char *);
void     at_static_collect_routes (atalkidbtype *);
void     at_static_AddAllZones (a_static *);
int      at_static_neighbors_match (ataddress *);
void     at_static_DeleteNeighbor(a_static *);
void     at_static_nonactive(atalkidbtype *);
void     at_static_validate_route(ushort Start, ushort End);



