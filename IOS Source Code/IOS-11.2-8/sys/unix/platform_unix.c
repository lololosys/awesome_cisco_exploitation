/* $Id: platform_unix.c,v 3.3.8.4 1996/09/13 04:15:00 smackie Exp $
 * $Source: /release/112/cvs/Xsys/unix/platform_unix.c,v $
 *------------------------------------------------------------------
 * platform_unix.c - Platform specific support for Unix platforms
 *
 * March 1995, Scott Mackie (moved from unix.c and unix_init.c)
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: platform_unix.c,v $
 * Revision 3.3.8.4  1996/09/13  04:15:00  smackie
 * Fix some of the major unix image unhappiness. (CSCdi69048)
 *
 *    o Remove problematic unix NIT support
 *    o Change default memory pool alignment to align 64 bit integers
 *    o Fix message compilation errors
 *
 * Branch: California_branch
 *
 * Revision 3.3.8.3  1996/06/19  05:38:49  smackie
 * Buffered logging support gets a hot wax and shine. (CSCdi53462)
 * Branch: California_branch
 *
 * Revision 3.3.8.2  1996/05/17  12:15:24  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.2.48.3  1996/04/26  07:59:08  dstine
 * Branch: IbuMod_Calif_branch
 * - commit of IBU modularity work, phase 1.
 *
 * Revision 3.2.48.2  1996/04/03  22:07:22  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.2.48.1  1996/03/17  18:53:15  ppearce
 * Sync IbuMod_Calif_branch to V111_1_3
 *
 * Revision 3.3.8.1  1996/03/18  22:29:48  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.2  1996/03/09  05:14:24  hampton
 * Slim down the kernel of the router.
 * Branch: DeadKingOnAThrone_branch
 *
 * Revision 3.2.26.1  1996/02/20  21:27:18  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.2  1995/11/17  17:55:22  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:42:05  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/10/15  18:55:18  eschaffe
 * CSCdi40294:  Increase the number of Protocol Translation sessions
 *
 * Revision 2.2  1995/06/28  23:15:00  tkolar
 * CSCdi35465:  I/O memory needs to be included when dumping core
 * Add flag for regions to determine if they are dumped or not.
 *
 * Revision 2.1  1995/06/07  23:11:53  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../../boot/cpu.h"
#include "interface_private.h"
#include "packet.h"
#include "ttysrv.h"
#include "logger.h"
#include "../os/buffers.h"
#include "../os/free.h"
#include "../os/pool.h"
#include "../os/region.h"
#include "../dev/monitor1.h"
#include "../unix/unix.h"
#include "../srt/span.h"
#include "config.h"
#include "../os/platform_clock.h"
#include "sys_registry.h"

/*
 * Memory pools
 */
static mempool pmem_mempool;

/*
 * Memory regions
 */
static regiontype main_region;
static regiontype text_region;
static regiontype data_region;
static regiontype bss_region;
static regiontype pmem_region;


/*
 * platform_main
 *
 * Called from main() to allows platforms to initialize basic functions
 * such as LEDs.
 */

void platform_main (void)
{
}


/*
 * platform_exception_init
 *
 * Initialize basic interrupt handling for the platform
 */

void platform_exception_init (void)
{
}    


/*
 * platform_memory_init
 *
 * Define and declare all the memory regions and mempools for this platform
 */

void platform_memory_init (void)
{
    ulong memsize;

    /*
     * Allocate memory via sbrk().
     */
    memsize = mon_getmemsize();
    
    /*
     * Add a region to describe all of main DRAM
     */
    region_create(&main_region, "main",
		  RAMBASE, memsize,
		  REGION_CLASS_LOCAL, 
		  REGION_FLAGS_DEFAULT | REGION_FLAGS_EXCEPTION_DUMP);

    /*
     * Add regions to describe the loaded image
     */
    region_create(&text_region, "text",
		  TEXT_START, ((uint)TEXT_END - (uint)TEXT_START),
		  REGION_CLASS_IMAGETEXT, REGION_FLAGS_DEFAULT);
    region_create(&data_region, "data",
		  DATA_START, ((uint)DATA_END - (uint)DATA_START),
		  REGION_CLASS_IMAGEDATA, REGION_FLAGS_DEFAULT);
    region_create(&bss_region, "bss",
		  DATA_END, ((uint)_END - (uint)DATA_END),
		  REGION_CLASS_IMAGEBSS, REGION_FLAGS_DEFAULT);

    /*
     * Declare a region from the end of BSS to the end of main dram
     * and create a "local" mempool based on it.
     */
    region_create(&pmem_region, "heap",
		  _END, (memsize - (ulong)_END),
		  REGION_CLASS_LOCAL, REGION_FLAGS_DEFAULT);

    mempool_create(&pmem_mempool, "Processor", &pmem_region, sizeof(ulonglong),
		   NULL, 0, MEMPOOL_CLASS_LOCAL);

    /*
     * Add aliases for mandatory mempools
     */
    mempool_add_alias_pool(MEMPOOL_CLASS_IOMEM,  &pmem_mempool);
    mempool_add_alias_pool(MEMPOOL_CLASS_FAST,   &pmem_mempool);
    mempool_add_alias_pool(MEMPOOL_CLASS_ISTACK, &pmem_mempool);
    mempool_add_alias_pool(MEMPOOL_CLASS_PSTACK, &pmem_mempool);
}


/*
 * platform_buffer_init
 *
 * Define and declare all the buffer sizes for this platform
 */

void platform_buffer_init (platform_buffer_init_type type)
{
    switch (type) {
      case PLATFORM_BUFFER_BOOTSTRAP:
	pool_adjust(small, 0, SMALL_MAX1, SMALL_INIT_BOOT, TRUE);
	pool_adjust(middle, 0, MID_MAX1, MID_INIT_BOOT, TRUE);
	pool_adjust(big, 0, BIG_MAX1, BIG_INIT_BOOT, TRUE);
	pool_adjust(verybig, 0, VERYBIG_MAX1, VERYBIG_INIT_BOOT, TRUE);
	pool_adjust(large, 0, LARGE_MAX1, LARGE_INIT_BOOT, TRUE);
	pool_adjust(huge, 0, HUGE_MAX1, HUGE_INIT_BOOT, TRUE);
	break;
      case PLATFORM_BUFFER_NORMAL:
	pool_adjust(small, SMALL_MIN, SMALL_MAX1, SMALL_INIT1, TRUE);
	pool_adjust(middle, MID_MIN, MID_MAX1, MID_INIT1, TRUE);
	pool_adjust(big, BIG_MIN, BIG_MAX1, BIG_INIT1, TRUE);
	pool_adjust(verybig, VERYBIG_MIN, VERYBIG_MAX1, VERYBIG_INIT1, TRUE);
	pool_adjust(large, LARGE_MIN, LARGE_MAX1, LARGE_INIT1, TRUE);
	pool_adjust(huge, HUGE_MIN, HUGE_MAX1, HUGE_INIT1, TRUE);
	break;
      case PLATFORM_BUFFER_ROUTING:
	pool_adjust(small, SMALL_MIN, SMALL_MAX3, SMALL_INIT3, TRUE);
	pool_adjust(middle, MID_MIN, MID_MAX3, MID_INIT3, TRUE);
	pool_adjust(big, BIG_MIN, BIG_MAX3, BIG_INIT3, TRUE);
	pool_adjust(verybig, VERYBIG_MIN, VERYBIG_MAX3, VERYBIG_INIT3, TRUE);
	pool_adjust(large, LARGE_MIN, LARGE_MAX3, LARGE_INIT3, TRUE);
	pool_adjust(huge, HUGE_MIN, HUGE_MAX3, HUGE_INIT3, TRUE);
	break;
    }
}

/*
 * platform_get_string
 *
 * Returns a variety of strings unique to the 4000
 */

char *platform_get_string (platform_string_type type)
{
    char  *value;

    switch (type) {
      case PLATFORM_STRING_NOM_DU_JOUR:
	value = "Unix";
	break;

      case PLATFORM_STRING_DEFAULT_HOSTNAME:
	value = "Router";
	break;

      case PLATFORM_STRING_PROCESSOR:
	value = PROCESSOR_NAME;
	break;

      case PLATFORM_STRING_VENDOR:
	value = OS_STRING;
	break;

      case PLATFORM_STRING_PROCESSOR_REVISION:
      case PLATFORM_STRING_HARDWARE_REVISION:
      case PLATFORM_STRING_HARDWARE_SERIAL:
      default:
	value = NULL;
	break;
    }

    return(value);
}

/*
 * platform_get_value
 *
 * Returns a variety of integer values unique to the 4000
 */

uint platform_get_value (platform_value_type type)
{
    uint value;

    switch (type) {
      case PLATFORM_VALUE_SERVICE_CONFIG:
	value = nvsize ? FALSE : TRUE;
	break;

      case PLATFORM_VALUE_FEATURE_SET:
	value = 0x0000;
	break;

      case PLATFORM_VALUE_VENDOR:
	value = VENDOR_CISCO;
	break;

      case PLATFORM_VALUE_CPU_TYPE:
	value = CPU_UNIX;
	break;

      case PLATFORM_VALUE_FAMILY_TYPE:
	value = FAMILY_UNIX;
	break;

      case PLATFORM_VALUE_REFRESH_TIME:
	value = REFRESHTIME;
	break;

      case PLATFORM_VALUE_LOG_BUFFER_SIZE:
	value = FOUR_K;
	break;

      case PLATFORM_VALUE_HARDWARE_SERIAL:
      case PLATFORM_VALUE_HARDWARE_REVISION:
      default:
	value = 0;
	break;
    }

    return(value);
}

/*
 * platform_print_memory
 *
 */

static void platform_print_memory (void)
{
    if (nvsize)
	printf("%dK bytes of non-volatile configuration memory.\n",
               (mon_get_nvram_size()/1024));
}


/* platform_interface_init
 * 
 * Initialize platform specific interface features
 */

void platform_interface_init (void)
{
    /*
     * Add registry callbacks
     */
    reg_add_print_memory(platform_print_memory, "platform_print_memory");
}

boolean (*platform_interface_ordering) (hwidbtype *new_idb, hwidbtype *old_idb)

     = idb_default_ordering;

/*
 * platform_line_init
 * Initialize the terminal line data structures.
 */

void platform_line_init (void)
{
    /*
     * Some default function registrations
     */
    reg_add_tty_xon(tty_xon_default, "tty_xon_default");

    /*
     * "Discover" the console
     */
    nconlines = 1;
    nauxlines = 1;
    maxvtylines = defvtylines = nvtylines = DEFAULT_NVTYS;

    /*
     * Assign base indexes into the MODEMS[] data structure.
     */
    AuxBase = 1;
    VTYBase = 2;
    ALLlines = nconlines + nauxlines + nvtylines;

    /*
     * Initialize the MODEMS[] data structure
     */
    if (nauxlines)
	MODEMS[AuxBase] = auxline_init();
    if (nvtylines)
	vty_init();
}

/*
 * platform_verify_config
 *
 * Validate hardware/software compatability.
 */

void platform_verify_config(void)
{
    extern forktype unix_console(tt_soc *);
    extern tt_soc *console;

    (void) cfork((forkproc *)unix_console, (long) console, 0, "UNIX Console",
		 CONSOLE_LINENUM);
}
