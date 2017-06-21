/* $Id: confreg.c,v 3.2.58.1 1996/03/21 23:26:43 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/rommon/src/confreg.c,v $
 *------------------------------------------------------------------
 * confreg.c
 *
 * April 1994, Rob Clevenger
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Configuration register routines.
 *------------------------------------------------------------------
 * $Log: confreg.c,v $
 * Revision 3.2.58.1  1996/03/21  23:26:43  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.2.2.2  1995/12/05  23:12:18  mbeesley
 * Branch: ELC_branch
 * Add complete support for discontigous NVRAM.
 *
 * Revision 3.2.2.1  1995/12/01  04:19:54  mbeesley
 * Branch: ELC_branch
 * Reorganize Rom monitor header files to make it easier to port
 * to new platforms.
 *
 * Revision 3.2  1995/11/17  18:41:12  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:04:51  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:36:50  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
** confreg.c
**
** Utility to make editing the Virtual Configuration Register
** more user friendly.
*/
#include "monitor.h"
#include "confreg.h"
#include "mon_boot.h"
#include "bitdef.h"
#include "proto.h"

extern char *platform_string;

struct bitdef confregbits[] = {
    1, "diagnostic mode",
    1, "use net in IP bcast address",
    1, "load rom after netboot fails",
    0, "",   /* console baud rate */
    0, "",   /* console baud rate */
    1, "use all zero broadcast",
    0, "",   /* unused */
    0, "break/abort has effect",
    0, "",   /* oem mode?? */
    1, "ignore system config info",  /* and disable watchdog timer */
    0, "",   /* unused */
    0, "",   /* unused */
    0, "",   /* bootfile number */
    0, "",   /* bootfile number */
    0, "",   /* bootfile number */
    0, "",   /* bootfile number */
};
#define NUM_CR_BITS (sizeof(confregbits) / sizeof(struct bitdef))

int
confreg(int argc, char *argv[])
{

    unsigned short confreg;
    long i, shift;
    long temp;
    char buffer[16];

    if (NV_VALID_CONFREG) {
	confreg = nv_read_config();
    } else {
	confreg = (NVRAM)->confreg;  /* get current confreg */
    }

    if(argc > 2) {
usage:
	printf("usage: %s [<hexval>]\n");
	return(1);
    }
    if(argc == 2) {  /* value specified on command line */
	i = getnum(argv[1], 16, &temp);
	if(i == 0) {
	    printf("illegal argument\n");
	    goto usage;
	}
	confreg = temp;
    } else {
	while(1) {
	    puts("\n\n    Configuration Summary\nenabled are:\n");
	    for(i=0, shift = 15; i<NUM_CR_BITS; i++, shift--) {
		if(*confregbits[i].string == '\0') continue;
		if(!(((confreg >> shift) & 1) == confregbits[i].actstate))
		    continue;
		puts(confregbits[i].string);
		puts("\n");
	    }
	    puts("console baud: ");
	    switch((confreg & CONSOLE_MASK) >> CONSOLE_SHIFT) {
	    case 0:
		puts("9600");
		break;
	    case 1:
		puts("4800");
		break;
	    case 2:
		puts("1200");
		break;
	    case 3:
		puts("2400");
		break;
	    }
	    puts("\nboot: ");
	    switch(confreg & BOOT_FIELD) {
	    case 0:
		puts("the ROM Monitor");
		break;
	    case 1:
		puts("the boot helper image");
		break;
	    default:
		puts("image specified by the boot system commands\n\
      or default to: ");
		printf(CISCO_FORMAT, confreg & BOOT_FIELD, platform_string);
		break;
	    }
	    if(getc_answer("\n\ndo you wish to change the configuration? y/n",
			   "yn", 'n')  == 'n') break;
	    for(i=0, shift = 15; i<NUM_CR_BITS; i++, shift--) {
		if(*confregbits[i].string == '\0') continue;
		if(((confreg >> shift) & 1) == confregbits[i].actstate)
		    puts("disable");
		else puts("enable ");
		printf(" \"%s\"", confregbits[i].string);
		if(getc_answer("? y/n", "yn", 'n')  == 'n') continue;
		confreg ^= (1 << shift);
	    }
	    if(getc_answer("change console baud rate? y/n", "yn", 'n')
	       == 'y') {
		i = getc_answer("\
enter rate: 0 = 9600, 1 = 4800, 2 = 1200, 3 = 2400", "0123",
				((confreg & CONSOLE_MASK) >> CONSOLE_SHIFT) + '0');
		confreg &= ~CONSOLE_MASK;
		confreg |= (i << CONSOLE_SHIFT) & CONSOLE_MASK;
	    }
	    if(getc_answer("change the boot characteristics? y/n", "yn", 'n')
	       == 'y') {
		while(1) {
		    printf("\
enter to boot:\n\
 0 = ROM Monitor\n\
 1 = the boot helper image\n\
 2-15 = boot system\n\
    [%c]:  ", (confreg & BOOT_FIELD) + '0');
		    if(!getline(buffer, sizeof(buffer))) {  /* use default */
			i = confreg & BOOT_FIELD;
			break;
		    }
		    if(!getnum(buffer, 10, &i)) continue;
		    if(i < 0 || i > 15) continue;
		    break;
		}
		confreg &= ~BOOT_FIELD;
		confreg |= i;
	    }
	}
    }
    puts("\n\n");
    if(confreg != (NVRAM)->confreg) {
	NV_WR_CONFREG(confreg);
	puts("You must reset or power cycle for new config to take effect\n");
    }
    return(0);
}
