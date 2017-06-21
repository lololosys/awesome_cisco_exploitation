/* $Id: lex_sub_hes.c,v 3.2 1995/11/17 17:43:24 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/lex/lex_sub_hes.c,v $
 *------------------------------------------------------------------
 * lex_sub_hes.c. This file is used to support lex related high end 
 * system specific features.
 *
 * Nov 1994, Gannu Satish Kumar
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: lex_sub_hes.c,v $
 * Revision 3.2  1995/11/17  17:43:24  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:35:14  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:41:14  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "subsys.h"
#include "interface_private.h"
#include "../hes/if_mci.h"
#include "lex_rcmd.h"
#include "lex.h"
#include "lex_fs_mci.h"

/*
 * LEX HES subsystem header
 */

#define LEX_SUB_HES_MAJVERSION 1
#define LEX_SUB_HES_MINVERSION 0
#define LEX_SUB_HES_EDITVERSION 0

static void lex_sub_hes_init(subsystype *);

SUBSYS_HEADER(lex_sub_hes, LEX_SUB_HES_MAJVERSION, LEX_SUB_HES_MINVERSION,
	      LEX_SUB_HES_EDITVERSION, lex_sub_hes_init,
	      SUBSYS_CLASS_PROTOCOL, NULL, "req: lex");

/*
 * lex_sub_hes_init
 * Define the subsystem main function. 
 * Nothing to be done currently in this function.
 */
static void lex_sub_hes_init(subsystype *subsys)
{
}

void lex_set_span_mac_offset (hwidbtype *hwidb)
{
    hwidb->span_macoffset = LEX_RECEIVE_OFFSET;    
}
