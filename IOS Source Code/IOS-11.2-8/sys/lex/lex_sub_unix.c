/* $Id: lex_sub_unix.c,v 3.2 1995/11/17 17:43:27 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/lex/lex_sub_unix.c,v $
 *------------------------------------------------------------------
 * lex_sub_unix.c. This file is used to support lex related unix 
 * system specific features.
 *
 * Nov 1994, Gannu Satish Kumar
 *
 * Copyright (c) 1994-1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: lex_sub_unix.c,v $
 * Revision 3.2  1995/11/17  17:43:27  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:35:16  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:41:19  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "subsys.h"
#include "interface_private.h"
#include "lex_rcmd.h"
#include "lex.h"

/*
 * LEX UNIX subsystem header
 */

#define LEX_SUB_UNIX_MAJVERSION 1
#define LEX_SUB_UNIX_MINVERSION 0
#define LEX_SUB_UNIX_EDITVERSION 0

void lex_sub_unix_init(subsystype *);

SUBSYS_HEADER(lex_sub_unix, LEX_SUB_UNIX_MAJVERSION, LEX_SUB_UNIX_MINVERSION,
	      LEX_SUB_UNIX_EDITVERSION, lex_sub_unix_init,
	      SUBSYS_CLASS_PROTOCOL, NULL, "req: lex");

/*
 * lex_sub_unix_init
 * Define the subsystem main function.
 * Nothing to be done currently in this function.
 */
void lex_sub_unix_init(subsystype *subsys)
{
}

/*
 * Currently used only in hes, so in unix set it to 0.
 */
void lex_set_span_mac_offset (hwidbtype *hwidb)
{
    hwidb->span_macoffset = 0;    
}
