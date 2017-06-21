/* $Id: filesys_test_cmd.c,v 3.2 1995/11/17 09:07:33 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/filesys/filesys_test_cmd.c,v $
 *------------------------------------------------------------------
 * TEST FILESYS commands
 *
 * January 1995, Jack Jenney
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: filesys_test_cmd.c,v $
 * Revision 3.2  1995/11/17  09:07:33  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:31:42  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/06/28  09:23:47  smackie
 * Repair widespread subsystem header braindamage. (CSCdi23568)
 *
 *   o Fix subsystem entry points to be foo(subsystype *) not foo(void)
 *   o Repair nonsensical and redundant subsystem sequences
 *   o Use NULL where no property is required
 *
 * Revision 2.1  1995/06/07  20:32:00  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "ttysrv.h"
#include "config.h"
#include "subsys.h"

extern void fslib_test_main(void);
extern void filesys_test_parser_init(void);

/*
 * filesys_command
 * flash file system debug console
 */

void filesys_command (parseinfo *csb)
{
    printf("File System Test Program\n");
    fslib_test_main();
    
}


/*
 *
 */
static void filesys_test_init (subsystype *subsys)
{
    filesys_test_parser_init();
}


/*
 * cBus test subsystem header
 */
#define	FILESYSTEST_MAJVERSION	1
#define	FILESYSTEST_MINVERSION	0
#define	FILESYSTEST_EDITVERSION	1

SUBSYS_HEADER(filesys_test, FILESYSTEST_MAJVERSION,
	      FILESYSTEST_MINVERSION, FILESYSTEST_EDITVERSION,
	      filesys_test_init, SUBSYS_CLASS_DRIVER,
	      NULL,
	      NULL);
