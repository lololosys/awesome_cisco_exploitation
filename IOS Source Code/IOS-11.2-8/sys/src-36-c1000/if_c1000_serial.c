/* $Id: if_c1000_serial.c,v 3.3.46.3 1996/08/28 13:10:47 thille Exp $ 
 * $Source: /release/112/cvs/Xsys/src-36-c1000/if_c1000_serial.c,v $
 *------------------------------------------------------------------
 * if_c1000_serial.c  C1000 specific serial driver
 *
 * March 1995, Dave Coli and John Ng
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: if_c1000_serial.c,v $
 * Revision 3.3.46.3  1996/08/28  13:10:47  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.3.46.2  1996/05/21  10:03:00  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.3.46.1  1996/03/18  21:57:31  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.14.1  1996/01/24  22:50:12  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.3  1995/12/17  18:37:32  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.2  1995/11/17  18:40:14  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:19:53  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:26:40  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/06/07  22:52:35  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include "interface_private.h"
#include "media_registry.h" /* need for reg_invoke_serial_driver_setup proto */
#include "subsys.h"
#include "config.h" /* needed for parser defs */
#include "logger.h"
#include "../src-36/msg_quicc.c"	/* Not a typo, see logger.h */
#include "parser.h" /* needed for parser defs */
#include "../parser/parser_actions.h" /* exec_show_controllers_serial parse chain */
#include "../parser/macros.h" /* for exec_show_controllers_serial parse chain */
#include "../parser/parser_defs_parser.h" /* needed for IFTYPE_SERIAL */
#include "../les/if_les.h" /* needed for INTERFACE_1E1T, cookie */
#include "../src-36/quicc.h"   /* needed for QUICC_PORT_SCC4 */
#include "../les/les_driver.h" /* needed for QUICC_DRIVER */


/*
 * Set up an idb for an serial interface
 * and initialize that interface.
 */
static void 
c1000_serial_init (int driver, int which, long membase, long port)
{
    hwidbtype *idb;
    
    /*
     * Set up storage, add IDB to interface queue
     */
    idb = idb_create();
    if (!idb) {
	errmsg(&msgsym(NOMEMORY, QUICC), which, "idb structure");
	crashdump(0);
    }

    /*
     * Save our unit number
     */
    idb->unit = which;    /* unit number */

    /*
     * Set up the hardware.
     * Allocate the driver instance data structure.
     * Initialize controller specific stuff in the idb.
     * The registry call will CASE on the driver type.
     * This call will initialize the hardware port for the
     * specific interface.  The call to 
     */
    reg_invoke_serial_driver_setup(driver, idb, membase, port);

}

/*
 * Serial show controllers command
 */
#define ALTERNATE       NONE
#include "../les/exec_show_controllers_serial.h"
LINK_POINT(quicc_serial_showcontroller_commands, ALTERNATE);
#undef  ALTERNATE

/*
 * Parser init for serial show controller command
 */
static void
c1000_serial_parser_init (void)
{
    parser_add_commands(PARSE_ADD_SHOW_CONTROLLER_CMD,
                        &pname(quicc_serial_showcontroller_commands), "quicc");
}

/*
 * c1000 serial subsystem initialization entry point
 */
static void c1000_serial_subsys_init (subsystype *subsys)
{
    int driver_type = UNKNOWN_DRIVER;  /* default; keep compiler happy */
    long dpram_base = -1;  /* default; keep compiler real happy */
    long serial_port = 0;  /* default; keep compiler extremely happy */

    /*
     * Discover all the Serial interfaces in the system.
     * If any found, initialize them.
     */
    switch (cookie->interfaces) {
      case INTERFACE_1E1T:
         nserialnets = 1;
         driver_type = QUICC_DRIVER;
         dpram_base = (long)getmbar();
         serial_port = QUICC_PORT_SCC4;
         break;
      default:
         nserialnets = 0;
         break;
    }
    /*
     * If any serials found, set up registry calls (if any),
     * initialize the interface(s), and set up the parser stuff.
     */
    if (nserialnets) {
        switch(nserialnets) {
        default:
        case 1:
	    c1000_serial_init(driver_type, 0, dpram_base, serial_port);
	    break;
        }
        c1000_serial_parser_init();
    }
}

#define C1000_SERIAL_MAJVERSION   1
#define C1000_SERIAL_MINVERSION   0
#define C1000_SERIAL_EDITVERSION  1

SUBSYS_HEADER(c1000_serial, C1000_SERIAL_MAJVERSION, 
              C1000_SERIAL_MINVERSION,
              C1000_SERIAL_EDITVERSION,
              c1000_serial_subsys_init, SUBSYS_CLASS_DRIVER,
              "seq: quicc_serial",
              NULL);






