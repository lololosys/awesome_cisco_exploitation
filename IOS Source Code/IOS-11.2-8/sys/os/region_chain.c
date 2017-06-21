/* $Id: region_chain.c,v 3.6.10.2 1996/05/21 09:58:21 thille Exp $
 * $Source: /release/112/cvs/Xsys/os/region_chain.c,v $
 *------------------------------------------------------------------
 * region_chain.c - Parse chains and code for region support
 *
 * March 1995, Scott Mackie
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: region_chain.c,v $
 * Revision 3.6.10.2  1996/05/21  09:58:21  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.6.10.1  1996/03/18  21:31:06  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.14.3  1996/03/16  07:21:07  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.3.14.2  1996/03/07  10:19:30  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.3.14.1  1996/02/20  16:41:21  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.6  1996/02/22  14:36:48  widmer
 * CSCdi49281:  make all parser_extension_request static const
 * make parser_extension_request typedef const
 * make all parser_extension_request static
 *
 * Revision 3.5  1996/01/29  07:29:16  hampton
 * Remove unnecessary includes of parser definition files.  [CSCdi47717]
 *
 * Revision 3.4  1996/01/25  11:20:31  smackie
 * Registry code gets some much-needed hot-oil massaging, free liposuction
 * and a makeover.  (CSCdi47101)
 *
 *    o Compress registry structures. Saves over 120K for all platforms
 *    o Add registry subsystems and remove feature registry initializion
 *      from registry.c to fix woeful initialization scaling problems
 *    o Remove unused and seemingly unloved registry debugging code
 *    o Add registry memory statistics to shame people into action
 *
 * Revision 3.3  1995/12/17  18:34:22  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.2  1995/11/17  18:50:53  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:47:42  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/11/08  21:19:15  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.2  1995/07/02  10:51:17  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize os, hes, les, if, and ibm.
 *
 * Revision 2.1  1995/06/07  21:57:59  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include <stdarg.h>
#include "logger.h"
#include "subsys.h"
#include "registry.h"
#include "interface_private.h"
#include "packet.h"
#include "config.h"
#include "parser.h"
#include "../ui/debug.h"
#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "region.h"


/*
 * Parser chains for "show region"
 */
#define ALTERNATE NONE
#include "exec_show_region.h"
LINK_POINT(region_show_commands, ALTERNATE);
#undef ALTERNATE


/*
 * Region parse chain table
 */
static parser_extension_request region_chain_init_table[] = {
    { PARSE_ADD_SHOW_CMD, &pname(region_show_commands) },
    { PARSE_LIST_END, NULL }
};


/*
 * region_ui_init
 *
 * Initialize region parser support.
 *
 */

static void region_ui_init (subsystype *subsys)
{
    parser_add_command_list(region_chain_init_table, "region");
}


/*
 * region_get_class_string
 * 
 * Get a textual name for a region class
 */

static char *region_get_class_string (region_class class)
{
    switch (class) {
      case REGION_CLASS_LOCAL:     return("Local");
      case REGION_CLASS_IOMEM:     return("Iomem");
      case REGION_CLASS_PCIMEM:    return("PCI");
      case REGION_CLASS_FAST:      return("Fast");
      case REGION_CLASS_MULTIBUS:  return("Multi");
      case REGION_CLASS_CBUS:      return("cBus");
      case REGION_CLASS_FLASH:     return("Flash");
      case REGION_CLASS_NVRAM:     return("NVRam");
      case REGION_CLASS_FILESYSTEM: return("F/S");
      case REGION_CLASS_CONFIG:    return("Confg");
      case REGION_CLASS_IMAGETEXT: return("IText");
      case REGION_CLASS_IMAGEDATA: return("IData");
      case REGION_CLASS_IMAGEBSS:  return("IBss");
      case REGION_CLASS_ANY:
      default:                     return("Bad");
    }
}

/*
 * region_get_media_string
 * 
 * Get a textual name for a region media
 */

static char *region_get_media_string (region_media media)
{
    switch (media) {
      case REGION_MEDIA_READWRITE:     return("R/W");
      case REGION_MEDIA_READONLY:      return("R/O");
      case REGION_MEDIA_WRITEONLY:     return("W/O");
      case REGION_MEDIA_UNKNOWN:       return("  -  ");
      case REGION_MEDIA_ANY:
      default:                   return("Bad");
    }
}

/*
 * region_show_all
 * Show all of the regions declared
 */
void region_show_all (parseinfo *csb)
{
    regiontype   *region;

    /*
     * Turn on the "more" prompting
     */
    automore_enable(NULL);

    printf("\nRegion Manager:\n");
 
    printf("\n      Start         End     Size(b)  Class  Media  Name\n");

    /*
     * Find the first region in the list
     */
    region = region_find_by_attributes(REGION_CLASS_ANY,
				       REGION_STATUS_ANY,
				       REGION_MEDIA_ANY);

    /*
     * Loop through all of our regions and print out info about them
     */
    while (region) {
        printf(" 0x%08x  0x%08x", region->start, region->end - 1);

	if (region->flags & REGION_FLAGS_IGNORE)
	    printf("           -");
	else
	    printf("  %-10u", region->size);

	printf("  %5s  %5s",
	       region_get_class_string(region->class),
	       region_get_media_string(region->media));

	switch (region->status) {
	  case REGION_STATUS_PARENT:
	    printf("  %s\n", region->name);
	    break;
	  case REGION_STATUS_CHILD:
	    printf("  %s:%s\n", region->parent->name, region->name);
	    break;
	  case REGION_STATUS_ALIAS:
	    printf("  %s:(%s)\n", region->alias->name, region->name);
	    break;
	  default:
	    printf("  [%s]\n", region->name);
	    break;
	}
    
	/*
	 * Get the next region
	 */
	region = region_find_next_by_attributes(region,
						REGION_CLASS_ANY,
						REGION_STATUS_ANY,
						REGION_MEDIA_ANY);
    }
    printf("\n");

    automore_disable();
}


/*
 * region_show_address
 * 
 * Given an address, print out the region that it corresponds to.
 */
void region_show_address (parseinfo *csb)
{
    regiontype  *region;
    void        *address;

    /*
     * Find the region for this address
     */
    address = (void *)GETOBJ(int,1);
    region = region_find_by_addr(address);

    /*
     * If we couldn't find a region, print a polite address
     */
    if (!region) {
	printf("Unable to find a region for this address\n");
	return;
    }

    printf("\nAddress 0x%08x is ", address);
    if (region_addr_match(region, address))
	printf("located physically in :\n");
    else
	printf("aliased in :\n");

    printf("\n  Name  : %s", region->name);
    printf("\n  Class : %s", region_get_class_string(region->class));
    printf("\n  Media : %s", region_get_media_string(region->media));
    printf("\n  Start : 0x%08x", region->start);
    printf("\n  End   : 0x%08x", region->end - 1);
    printf("\n  Size  : 0x%08x", region->size);

    printf("\n\n");
}

/*
 * region_ui subsystem header
 */
 
#define REGION_UI_MAJVERSION    1
#define REGION_UI_MINVERSION    0
#define REGION_UI_EDITVERSION   1
 
SUBSYS_HEADER(region_ui, REGION_UI_MAJVERSION, REGION_UI_MINVERSION,
              REGION_UI_EDITVERSION,
              region_ui_init, SUBSYS_CLASS_MANAGEMENT,
              NULL, NULL);

