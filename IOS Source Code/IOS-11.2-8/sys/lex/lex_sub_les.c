/* $Id: lex_sub_les.c,v 3.2 1995/11/17 17:43:25 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/lex/lex_sub_les.c,v $
 *------------------------------------------------------------------
 * lex_sub_les.c. This file is used to support lex related low end 
 * system specific features.
 *
 * Nov 1994, Gannu Satish Kumar
 *
 * Copyright (c) 1994-1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: lex_sub_les.c,v $
 * Revision 3.2  1995/11/17  17:43:25  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:35:15  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:41:16  hampton
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
 * LEX LES subsystem header
 */

#define LEX_SUB_LES_MAJVERSION 1
#define LEX_SUB_LES_MINVERSION 0
#define LEX_SUB_LES_EDITVERSION 0

void lex_sub_les_init(subsystype *);

SUBSYS_HEADER(lex_sub_les, LEX_SUB_LES_MAJVERSION, LEX_SUB_LES_MINVERSION,
	      LEX_SUB_LES_EDITVERSION, lex_sub_les_init,
	      SUBSYS_CLASS_PROTOCOL, NULL, "req: lex");


/*
 * lex_sub_les_init
 * Define the subsystem main function.
 * Nothing to be done currently in this function.
 */
void lex_sub_les_init(subsystype *subsys)
{
}

/*
 * Currently used only in hes, so in les set it to 0.
 */
void lex_set_span_mac_offset (hwidbtype *hwidb)
{
    hwidb->span_macoffset = 0;    
}
