/* $Id: if_vidb.h,v 3.3 1995/11/17 09:28:06 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/if/if_vidb.h,v $
 *------------------------------------------------------------------
 * if_vidb.c -- Virtual IDBs
 *
 * 15-March-1991, Joel Bion and Tony Li
 *
 * Copyright (c) 1991-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: if_vidb.h,v $
 * Revision 3.3  1995/11/17  09:28:06  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:38:01  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:50:26  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:53:00  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "../srt/srt_registry.h"

#ifndef _IF_VIDB_H_
#define _IF_VIDB_H_

/* Bandwidth for display and IGRP. */

#define VISIBLE_VIDB_BANDWIDTH 56	/* 56 Kbps */
#define VIDB_DELAY 500			/* 5 msec */

typedef struct vidb_list_ {
    struct vidb_list_ *next;
    idbtype *idb;
} vidb_list;


void    vidb_init(void);
idbtype *vidb_malloc(void);
void    vidb_free(idbtype *idb);
idbtype *find_vidb(ushort portNumber); /* CLS support... */
int     get_current_vidb_unit(void);
idbtype *get_next_vidb(idbtype *idb);
void    span_port_disable_onVidb(spantype *span, int);
extern void span_port_disable(spantype *, idbtype *);
#endif
