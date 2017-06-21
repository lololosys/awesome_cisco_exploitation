/* $Id: manu_les.c,v 3.2.62.1 1996/03/18 20:43:02 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/les/manu_les.c,v $
 *------------------------------------------------------------------
 * manu_les.c - Special code for a manufacturing only release.
 *
 * Feb 1993, Greg Stovall
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: manu_les.c,v $
 * Revision 3.2.62.1  1996/03/18  20:43:02  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.3  1996/03/09  05:07:49  hampton
 * Slim down the kernel of the router.
 * Branch: DeadKingOnAThrone_branch
 *
 * Revision 3.2.26.2  1996/03/07  10:05:53  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.2.26.1  1996/01/24  22:24:46  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.2  1995/11/17  17:41:56  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:33:50  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:38:32  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <ciscolib.h>
#include "../../boot/cpu.h"
#include "../ts/nonvol_x2444.h"

extern int cpu_type;


/*
 * readcookie_command()
 * Special magic command to read the serial NVRAM cookie
 */
void readcookie_command (char *args)
{
    int i;
    uchar nvram_address;
    ushort nvram_value;
    boolean *ok = FALSE;

    /*
     * Only works on cristal/cancun, puke on other stuff
     */
    if (cpu_type == CPU_CRISTAL || cpu_type == CPU_CANCUN ) {
	(void)x2444_io(X2444_CMD_RCL, 8, 0); /* Recall data into RAM */

	/* 
	 * See if there's an optional parameter
	 */
	nvram_address = parse_unsigned(args, ok);
	if (nvram_address > 31) {
	    /* 
	     * There isn't a parameter, display the whole cookie
	     */
	    for (i = 0; i < 16; i++) {
		nvram_value = x2444_io(X2444_CMD_READ + (i * 2), 8, 16); /* Read 16 bits */
		if ((i % 4) == 0)
		    printf("\n");
		printf("0x%04x ", nvram_value);
	    }
	} else {
	    /*
	     * Display the address asked for
	     */
	    nvram_value = x2444_io(X2444_CMD_READ + nvram_address, 8, 16);
	    printf("0x%04x ", nvram_value);
	}
    } else {
	printf("\nNo Serial Cookie present on this system");
	printf("\nThis command is not supported on this platform\n");
    }
}


/*
 * setcookie_command()
 */
void setcookie_command (char *args)
{
    uchar nvram_address;
    ushort nvram_value;
    boolean *ok = FALSE;

    /*
     * Only works on cristal/cancun, puke on other stuff
     */
    if (cpu_type == CPU_CRISTAL || cpu_type == CPU_CANCUN) {
	nvram_address = parse_unsigned(args, ok);
	if (nvram_address > 31) {
	    printf("Bad address value\n");
	    return;
	}
	
	args = deblank(args);
	args += 4;
	nvram_value = parse_unsigned(args, ok);

	(void)x2444_io(X2444_CMD_RCL, 8, 0); /* Recall data into RAM */
	(void)x2444_io(X2444_CMD_WREN, 8, 0); /* Enable writing */
	(void)x2444_io(X2444_CMD_WRITE | nvram_address | nvram_value << 8, 24, 0);
	(void)x2444_io(X2444_CMD_STO, 8, 0); /* Store data in EEPROM */
	(void)x2444_io(X2444_CMD_WRDS, 8, 0); /* Disable writing */
    } else
	printf("\nNo Serial Cookie present on this system");
	printf("\nThis command is not supported on this platform\n");
}

