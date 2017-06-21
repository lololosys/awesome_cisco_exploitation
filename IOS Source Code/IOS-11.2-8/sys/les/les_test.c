/* $Id: les_test.c,v 3.2.62.1 1996/03/18 20:43:00 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/les/les_test.c,v $
 *------------------------------------------------------------------
 * les_test.c -- subsystem support for a Low End System test image
 *
 * 7-June-1993, Kirk Lougheed
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: les_test.c,v $
 * Revision 3.2.62.1  1996/03/18  20:43:00  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.1  1996/01/24  22:24:43  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.2  1995/11/17  17:41:47  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:33:42  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:38:18  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "subsys.h"

/*
 * Externals
 */
extern void transmit_test_add_commands(void);
extern void diag_exec_parser_init(void);

/*
 * test_init
 * Initialize functions specific to a test image
 */

static void test_init (subsystype *subsys)
{
    transmit_test_add_commands();
    diag_exec_parser_init();
}

/*
 * TEST subsystem header
 */

#define TEST_MAJVERSION 1
#define TEST_MINVERSION 0
#define TEST_EDITVERSION  1

SUBSYS_HEADER(test, TEST_MAJVERSION, TEST_MINVERSION, TEST_EDITVERSION,
	      test_init, SUBSYS_CLASS_PROTOCOL,
	      NULL,
	      NULL);
