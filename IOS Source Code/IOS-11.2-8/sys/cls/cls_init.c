/* $Id: cls_init.c,v 3.2 1995/11/17 08:55:00 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/cls/cls_init.c,v $
 *------------------------------------------------------------------
 * cls_init.c  -- Initialize the CLS subsystem
 *
 * March 1994, Steve Berl
 *
 * Copyright (c) 1994-1995 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Longer description here.
 *------------------------------------------------------------------
 * $Log: cls_init.c,v $
 * Revision 3.2  1995/11/17  08:55:00  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:10:55  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/07/02  05:15:15  sberl
 * CSCdi36581:  cls.h should be cls_private.h
 * Remove references to cls.h from everything that is really a DLU. Change
 * name of cls.h to cls_private.h.
 *
 * Revision 2.1  1995/06/07  20:19:43  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "cls_private.h"
#include "subsys.h"
#include "../cls/dlc_registry.h"

/*
 * cls_init
 * This is the initialization point for the CLS subsystem.
 *
 */

static void cls_init (subsystype *subsys)
{
    CLSInit();
    cls_parser_init();
    /*
     * register our functions
     */
    reg_add_cls_rput(cls_rput, "cls_rput");
}

/*
 * CLS subsystem header
 */

#define CLS_MAJVERSION 1
#define CLS_MINVERSION 0
#define CLS_EDITVERSION  1

SUBSYS_HEADER(cls, CLS_MAJVERSION, CLS_MINVERSION, CLS_EDITVERSION,
	      cls_init, SUBSYS_CLASS_PROTOCOL,
	      "seq: srb, lanmgr, sdlc, llc2",
	      "req: srb, llc2");

