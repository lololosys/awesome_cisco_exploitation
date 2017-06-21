/* $Id: vip_test.c,v 3.4.10.4 1996/05/21 10:05:01 thille Exp $
 * $Source: /release/112/cvs/Xsys/src-vip/vip_test.c,v $
 *------------------------------------------------------------------
 * vip_test.c -- subsystem support for VIP test commands
 *
 * March 1995, Michael Beesley
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: vip_test.c,v $
 * Revision 3.4.10.4  1996/05/21  10:05:01  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.4.10.3  1996/04/06  09:56:28  ondiaye
 * CSCdi52438:  vip_test.c broken for rvip: Removed the cya test subsystem
 *              from vip_test.c. cya test is now in src-svip/cya_test* .
 *                 ondiaye.
 * Branch: California_branch
 *
 * Revision 3.4.10.2  1996/03/21  23:52:35  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.4.10.1  1996/03/18  22:03:50  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.2.4  1996/03/18  23:36:52  mbeesley
 * Branch: ELC_branch
 * Sync to V111_1_3
 *
 * Revision 3.2.2.3  1996/02/10  00:22:40  mbeesley
 * CSCdi48581:  Add VIP2 support to ELC_branch
 * Branch: ELC_branch
 *
 * Revision 3.2.2.2  1996/01/16  02:36:23  dcarroll
 * Branch: ELC_branch
 * Sync ELC_branch between ELC_baseline_120995 and ELC_baseline_960110.
 *
 * Revision 3.2.2.1  1995/12/09  20:52:46  mbeesley
 * Branch: ELC_branch
 * Move port adaptor test support into pas_test.c.
 *
 * Revision 3.4  1996/02/22  14:38:39  widmer
 * CSCdi49281:  make all parser_extension_request static const
 * make parser_extension_request typedef const
 * make all parser_extension_request static
 *
 * Revision 3.3  1995/12/17  18:38:37  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.2  1995/11/17  18:52:14  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:30:02  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:29:49  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/06/07  22:59:54  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include <ctype.h>
#undef   toupper			/* use library function call */
#include <ciscolib.h>
#include "subsys.h"
#include "interface_private.h"
#include "stacks.h"
#include "packet.h"
#include "ttysrv.h"
#include "config.h"
#include "parser.h"
#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "../pas/if_pas.h"
#include "vip_memd.h"
#include "vip_dma.h"

#define BUFLEN 			80


/*
 * =======
 * Bringup Support
 */
void vip_test_bringup (parseinfo *csb)
{
    char buff[BUFLEN];
    boolean parsed;
    char cmdchar = '?';
    uint bytes = 4;
    ulong addr = 0;
    ulong data = 0;
    char rw = 'R';

    /*
     * Loop until the user quits or there is an error
     */
    while (TRUE) {
	
	printf("\n VIPER Bringup Tests: (? for help)[%c]:  ",cmdchar);
	parsed = rdtty(buff, BUFLEN);
	if (!null(buff) && parsed) {
	    cmdchar = toupper(buff[0]);
	}
	
	switch (cmdchar) {

	/* Print out a helpfull menu */
	   
	case 'H':
	case '?':
	  printf("\n  i - I/O tests");
	  printf("\n  q - exit viper bringup test");
	  printf("\n\n  'c' rules of radix type-in and display apply.\n");
	  
	  break;
	  
	case 'Q':
	    return;

	case 'I':
	    printf("\n  Virtual Address, [0x%08x]? :  ", addr);
	    parsed = rdtty(buff, BUFLEN);
	    if (!null(buff) && parsed) {
		addr = parse_unsigned(buff,&parsed);
	    }

	    if (!touch_device((void *)addr)) {
		printf("\nInvalid address 0x%08x", addr);
		break;
	    }

	    printf("\n  Read or Write at location 0x%08x [%c]? :  ",addr, rw);
	    parsed = rdtty(buff, BUFLEN);
	    if (!null(buff) && parsed) 
		rw = toupper(buff[0]);

	    if (rw == 'R') {

		printf("\n  Number of bytes to read [%d]?:  ", bytes);
		parsed = rdtty(buff, BUFLEN);
		if (!null(buff) && parsed) {
		    bytes = parse_unsigned(buff,&parsed);
		    if ((bytes > 4) || (bytes < 1)){
			printf("\n%%Illegal value \n");
			return;
		    }
		}

		switch (bytes) {
		  case 1:
		    data = *((uchar *)addr);
		    break;
		  case 2:
		    data = *((ushort *)addr);
		    break;
		  case 3:
		    printf("\nnot supported");
		    break;
		  case 4:
		    data = *((ulong *)addr);
		    break;
		  default:
		    printf("\nnot supported");
		    break;

		}
		printf("\n  Read 0x%x, from address 0x%08x\n",
		       data, addr);
		
	    } else if (rw == 'W') {
		printf("\n  Number of bytes to write [%d]?:  ", bytes);

		parsed = rdtty(buff, BUFLEN);
		if (!null(buff) && parsed) {
		    bytes = parse_unsigned(buff,&parsed);
		    if ((bytes > 4) || (bytes < 1)){
			printf("\n%%Illegal value \n");
			return;
		    }
		}

		printf("\n  Write data  [%x]?:  ", data);

		parsed = rdtty(buff, BUFLEN);
		if (!null(buff) && parsed) {
		    data = parse_unsigned(buff,&parsed);
		}

		printf("\n  Writing 0x%x, to address 0x%08x\n",
		       data, addr);

		switch (bytes) {
		  case 1:
		    *((uchar *)addr) = data;
		    break;
		  case 2:
		    *((ushort *)addr) = data;
		    break;
		  case 3:
		    printf("\nnot supported");
		    break;
		  case 4:
		    *((ulong *)addr) = data;
		    break;
		  default:
		    printf("\nnot supported");
		    break;

		}
	    } else {
		printf("\n%%Invalid selector\n");
		return;
	    }
	    break;

	default:
	    printf("%%Invalid selection\n");
	    break;
	}
    }
}
 
/* end of file */




