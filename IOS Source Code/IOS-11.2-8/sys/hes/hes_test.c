/* $Id: hes_test.c,v 3.2 1995/11/17 09:16:38 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/hes/hes_test.c,v $
 *------------------------------------------------------------------
 * hes_test.c -- subsystem support for a High End System test image
 *
 * 7-June-1993, Kirk Lougheed
 *
 * Copyright (c) 1993-1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: hes_test.c,v $
 * Revision 3.2  1995/11/17  09:16:38  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:39:14  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:40:06  hampton
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

void test_init (subsystype *subsys)
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
