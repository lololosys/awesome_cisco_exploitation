/* $Id: c7100_test.c,v 3.1.68.1 1996/03/21 23:48:16 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/src-4k-c7100/c7100_test.c,v $
 *------------------------------------------------------------------
 * c7100_test.c -- C7100 diagnostic routines
 *
 * Novemeber 1995, Michael Beesley
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: c7100_test.c,v $
 * Revision 3.1.68.1  1996/03/21  23:48:16  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.1.2.2  1996/01/23  02:18:14  mbeesley
 * Branch: ELC_branch
 * Add support for the CPU ID EEPROM. Postpone PCI SRAM initialization
 * until the rev of the CPU card has been read and verified.
 *
 * Revision 3.1.2.1  1995/11/22  21:43:28  mbeesley
 * Initial commit of c7100 support.
 * Branch: ELC_branch
 *
 * Revision 3.1  1995/11/21  01:37:50  mbeesley
 * Add placeholder files for new development.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Include files
 */
#include "master.h"
#include "interface_private.h"
#include "config.h"
#include "subsys.h"

#include "../hes/dgbus.h"
#include "../pas/if_pas.h"

/*
 * test_c7100_cpueeprom:
 * read & write the NMC93 CPU eeprom
 */
void test_c7100_cpueeprom (parseinfo *csb)
{
    uchar addr, value, *eeprom_ptr;
    dgbus_eeprom_t eeprom;

    /*
     * Read the complete eeprom
     */
    c7100_read_cpueeprom(&eeprom);
    eeprom_ptr = (uchar *)&eeprom;

    addr = GETOBJ(int,2);

    switch (GETOBJ(int,1)) {

    case TEST_C7100_EEPROM_READ:
	value = eeprom_ptr[addr];
	printf("Value: %02x", value);
	break;

    case TEST_C7100_EEPROM_WRITE:
	eeprom_ptr[addr] = GETOBJ(int,3);
	c7100_write_cpueeprom(&eeprom);
	break;
    }
}

/*
 * c7100_test_init:
 * Init the C7100 test subsystem. Nothing at the moment.
 */
static void c7100_test_init (subsystype *subsys)
{
}

/*
 * C7100 test subsystem header
 */
#define	C7100TEST_MAJVERSION	1
#define	C7100TEST_MINVERSION	0
#define	C7100TEST_EDITVERSION	1

SUBSYS_HEADER(c7100_test,		/* name */
	      C7100TEST_MAJVERSION,	/* major version */
	      C7100TEST_MINVERSION,	/* minor version */
	      C7100TEST_EDITVERSION,	/* edit version */
	      c7100_test_init,		/* init function */
	      SUBSYS_CLASS_DRIVER,	/* subsystem class */
	      NULL, NULL);

/* end of file */
