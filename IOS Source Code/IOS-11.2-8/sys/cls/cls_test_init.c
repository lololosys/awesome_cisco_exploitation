/* $Id: cls_test_init.c,v 3.2 1995/11/17 08:55:05 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/cls/cls_test_init.c,v $
 *------------------------------------------------------------------
 * cls_test_init.c  -- Initialize the CLS test subsystem
 *
 * Jan 1995, Steve Berl
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: cls_test_init.c,v $
 * Revision 3.2  1995/11/17  08:55:05  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:11:03  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/11/08  20:53:00  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.2  1995/07/02  05:15:19  sberl
 * CSCdi36581:  cls.h should be cls_private.h
 * Remove references to cls.h from everything that is really a DLU. Change
 * name of cls.h to cls_private.h.
 *
 * Revision 2.1  1995/06/07  20:19:47  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "address.h"
#include "../cls/clsi_msg.h"
#include "../cls/clsi_queue.h"
#include "subsys.h"
#include "clsitest.h"
#include "dlutest.h"

/*
 * cls_test_init
 * This is the initialization point for the CLS test subsystem.
 *
 */

static void cls_test_init (subsystype *subsys)
{
    dlutest_parser_init();
/*    clsitest_parser_init(); */
}

/*
 * CLS test subsystem header
 */

#define CLS_TEST_MAJVERSION 1
#define CLS_TEST_MINVERSION 0
#define CLS_TEST_EDITVERSION  1

SUBSYS_HEADER(cls_test, CLS_TEST_MAJVERSION, CLS_TEST_MINVERSION, CLS_TEST_EDITVERSION,
	      cls_test_init, SUBSYS_CLASS_PROTOCOL,
	      "seq: cls",
	      "req: cls");

