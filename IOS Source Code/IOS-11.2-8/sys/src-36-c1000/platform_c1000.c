/* $Id: platform_c1000.c,v 3.6.10.5 1996/09/06 01:11:55 snyder Exp $
 * $Source: /release/112/cvs/Xsys/src-36-c1000/platform_c1000.c,v $
 *------------------------------------------------------------------
 * platform_c1000.c - Platform specific support for cisco 1000 boxes
 *
 * March 1995, Scott Mackie (moved from init_c1000.c)
 *
 * Copyright (c) 1995 - 1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: platform_c1000.c,v $
 * Revision 3.6.10.5  1996/09/06  01:11:55  snyder
 * CSCdi68277:  make shr_core, shr_ukernel, sub_cxxxx sub_core_platform
 *              things const, save 712 out of data, 208 from image
 * Branch: California_branch
 *
 * Revision 3.6.10.4  1996/06/19  04:44:06  smackie
 * Buffered logging support gets a hot wax and shine. (CSCdi53462)
 *
 *   o Restructure buffered logging to be a independant device
 *   o Add support for "clear logging"
 *   o Enable buffered logging by default on most platforms
 *   o Start hiding logger structures in private include files
 *
 * Branch: California_branch
 *
 * Revision 3.6.10.3  1996/06/12  19:57:04  jng
 * CSCdi60260:  Async catwoman commit
 * Branch: California_branch
 * Async Catwoman commit into California
 *
 * Revision 3.6.10.2  1996/05/17  11:53:08  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.6.2.2  1996/04/26  07:57:16  dstine
 * Branch: IbuMod_Calif_branch
 * - commit of IBU modularity work, phase 1.
 *
 * Revision 3.6.2.1  1996/04/03  21:37:05  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.6.10.1  1996/03/18  21:57:32  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.4.12.4  1996/03/16  07:32:14  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.4.12.3  1996/03/09  05:11:30  hampton
 * Slim down the kernel of the router.
 * Branch: DeadKingOnAThrone_branch
 *
 * Revision 3.4.12.2  1996/02/20  17:49:28  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.6  1996/02/23  17:16:11  cshen
 * CSCdi48454:  Need to support on-board flash on c1005 derivative
 *
 * Revision 3.5  1996/02/13  20:39:11  hampton
 * Move buffer initialization to the platform_xxx.c files to eliminate
 * platform dependencies from common code.  [CSCdi48844]
 *
 * Revision 3.4.12.1  1996/01/24  22:50:14  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.4  1996/01/05  04:56:39  hampton
 * Remove extraneous includes of h/timer.h.  [CSCdi46482]
 *
 * Revision 3.3  1995/12/03  18:00:09  thille
 * CSCdi44905:  ciscopro targets not in 11.1
 * Put them in.
 *
 * Revision 3.2  1995/11/17  18:40:19  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:19:59  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.9  1995/11/08  21:26:44  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.8  1995/10/20  00:54:46  ljiang
 * CSCdi42394:  Add vendor codes for cisco partners
 *
 * Revision 2.7  1995/10/15  18:54:50  eschaffe
 * CSCdi40294:  Increase the number of Protocol Translation sessions
 *
 * Revision 2.6  1995/10/04  22:59:37  hampton
 * The platform_get_value() routine should return CPU and FAMILY values
 * for the hardware it is currently executing upon, not some compiled in
 * value.  This is required to validate that the correct software image
 * has been booted onto a hardware platform.  [CSCdi41098]
 *
 * Revision 2.5  1995/09/23  05:58:13  enf
 * CSCdi39189:  100X need insurance against loading incorrect image
 *
 * Revision 2.4  1995/09/22  04:36:47  enf
 * CSCdi40869:  Add frankenstorm to 11.0
 *
 * Revision 2.3  1995/09/05  21:40:00  enf
 * CSCdi39019:  <SYSTEM OK> LED off after booting
 * move LED initialization to after Exception setup
 *
 * Revision 2.2  1995/06/28  23:14:37  tkolar
 * CSCdi35465:  I/O memory needs to be included when dumping core
 * Add flag for regions to determine if they are dumped or not.
 *
 * Revision 2.1  1995/06/07  22:52:42  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../../boot/cpu.h"
#include "ttysrv.h"
#include "logger.h"
#include "stacks.h"
#include "exception.h"
#include "packet.h"
#include "interface.h"
#include "sys_registry.h"
#include "../os/buffers.h"
#include "../os/free.h"
#include "../os/pool.h"
#include "../os/region.h"
#include "../les/if_les.h"
#include "../dev/monitor1.h"
#include "../os/msgtxt_sys.h"

/*
 * Memory pools
 */
static mempool pmem_mempool;
static mempool iomem_mempool;

/*
 * Memory regions
 */
static regiontype main_region;
static regiontype text_region;
static regiontype data_region;
static regiontype bss_region;
static regiontype pmem_region;
static regiontype iomem_region;

/*
 * Freelists
 */
static const ulong freelist_iomem_block_sizes[] = {
    FREESIZE(0), FREESIZE(1), FREESIZE(2), FREESIZE(3), FREESIZE(4),
    FREESIZE(5), FREESIZE(6), FREESIZE(7), 65536
};
static const ulong nfreelist_iomem_block_sizes =
    sizeof(freelist_iomem_block_sizes)/sizeof(ulong);

/*
 * Local strings
 */

static char hw_serial[20];
static char hw_revision[20];

/*
 * Local miscellaneous variables
 */
cookie_t *cookie;		/* Address of cookie PROM */


/*
 * platform_main
 *
 * Called from main() to allows platforms to initialize basic functions
 * such as LEDs.
 */

void platform_main (void)
{
    quicc_dpr_t *dpr;
    ushort hw_version;
    uint serial;
    
    /*
     * Whilst we're in here, set up the cookie register
     */
    cookie = mon_get_cookie();

    if ((cookie->magic[0] == COOKIE_MAGIC_0) &&
        (cookie->magic[1] == COOKIE_MAGIC_1) &&
        (cookie->capabilities[1] & CISCOPRO_PLATFORM))
            ciscopro_platform = TRUE;

    /*
     * Set up our basic strings
     */
    hw_version = mon_get_rev();
    sprintf(hw_revision, "%c", (hw_version - 1 + 'A'));

    serial = GETLONG(cookie->serial);
    sprintf(hw_serial, "%08x", serial);

    /*
     * Turn on the RUN lamp to indicate that we're up and running
     */
    dpr = (quicc_dpr_t *) getmbar();

}


/*
 * platform_exception_init
 *
 * Initialize basic interrupt handling for the platform
 */

void platform_exception_init (void)
{
    /*
     * Initialize the NMI handler
     */
    init_clocktick_handler();

    /*
     * Intialize the basic exception handlers (spurious interrupts, etc)
     */
    stack_hardware_init();

    /*
     * Initialize the pit timer.
     */
    c1000_init_pit_timer();
    c1000_post_exception_init();
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
     * Find out how much main dram the Rom Monitor thinks we've got
     * and subtract off the memory we steal for IO memory
     */
    memsize = mon_getmemsize() - C1000_IOMEM_SIZE;

    /*
     * Add a region to describe all of main DRAM
     */
    region_create(&main_region, "main", (void *)RAMBASE, memsize,
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
		  _END, ((ulong)main_region.end - (ulong)_END),
		  REGION_CLASS_LOCAL, REGION_FLAGS_DEFAULT);

    mempool_create(&pmem_mempool, "Processor", &pmem_region, 0,
		   NULL, 0, MEMPOOL_CLASS_LOCAL);

    /*
     * Declare a region and mempool for shared memory
     */
    region_create(&iomem_region, "iomem",
		  main_region.end, C1000_IOMEM_SIZE,
		  REGION_CLASS_IOMEM, REGION_FLAGS_DEFAULT);

    mempool_create(&iomem_mempool, "I/O", &iomem_region, 0,
		   freelist_iomem_block_sizes, nfreelist_iomem_block_sizes,
		   MEMPOOL_CLASS_IOMEM);

    /*
     * Allow main memory to steal from IO memory under low memory situations
     */
    mempool_add_alternate_pool(&pmem_mempool, &iomem_mempool);

    /*
     * Add aliases for mandatory mempools
     */
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
	pool_adjust(small, SMALL_MIN, SMALL_MAX2, SMALL_INIT2, TRUE);
	pool_adjust(middle, MID_MIN, MID_MAX2, MID_INIT2, TRUE);
	pool_adjust(big, BIG_MIN, BIG_MAX2, BIG_INIT2, TRUE);
	pool_adjust(verybig, VERYBIG_MIN, VERYBIG_MAX2, C1000_VERYBIG_INIT2, TRUE);
	pool_adjust(large, LARGE_MIN, LARGE_MAX2, LARGE_INIT2, TRUE);
	pool_adjust(huge, HUGE_MIN, HUGE_MAX2, HUGE_INIT2, TRUE);
	break;
    }
}

/*
 * platform_get_string
 *
 * Returns a variety of strings unique to the 1000
 */

char *platform_get_string (platform_string_type type)
{
    char  *value;

    switch (type) {
      case PLATFORM_STRING_NOM_DU_JOUR:
	if (ciscopro_platform)
	    value = "CPA1000";
	else if ((cookie->vendor) == VENDOR_NEC)
            value = "98 Router";
	else if ((cookie->vendor) == VENDOR_OKI)
            value = "1005";
	else 
            value = "1000";
	break;

      case PLATFORM_STRING_DEFAULT_HOSTNAME:
	value = "Router";
	break;

      case PLATFORM_STRING_PROCESSOR:
	value = "68360";
	break;

      case PLATFORM_STRING_VENDOR:
	switch (cookie->vendor) {
	  case VENDOR_CISCO:
	    value = "cisco";
	    break;
	
	  case VENDOR_OKI:
	    value = "OKI";
            break;

          case VENDOR_NEC:
            value = "NEC";
            break;

	  default:
	    value = "Unknown Vendor";
	    break;
	}
	break;

    case PLATFORM_STRING_BOOTROM_OR_BOOTFLASH:
        value = "BOOTFLASH:";
        break;

      case PLATFORM_STRING_HARDWARE_REVISION:
	value = hw_revision;
	break;

      case PLATFORM_STRING_HARDWARE_SERIAL:
	value = hw_serial;
	break;

      case PLATFORM_STRING_PROCESSOR_REVISION:
      default:
	value = NULL;
	break;
    }

    return(value);
}

/*
 * platform_get_value
 *
 * Returns a variety of integer values unique to the 3000
 */

uint platform_get_value (platform_value_type type)
{
    uint value;

    switch (type) {
      case PLATFORM_VALUE_SERVICE_CONFIG:
	value = nvsize ? FALSE : TRUE;
	break;
	
      case PLATFORM_VALUE_FEATURE_SET:
	value = 0;
	break;
	
      case PLATFORM_VALUE_HARDWARE_REVISION:
	value = mon_get_rev();
	break;

      case PLATFORM_VALUE_HARDWARE_SERIAL:
	value = GETLONG(cookie->serial);
	break;

      case PLATFORM_VALUE_VENDOR:
	value = cookie->vendor;
	break;

      case PLATFORM_VALUE_CPU_TYPE:
	value = mon_proctype();
	break;

      case PLATFORM_VALUE_FAMILY_TYPE:
	value = (cpu_type == CPU_SAPPHIRE) ? FAMILY_C1000 : FAMILY_UNKNOWN;
	break;

      case PLATFORM_VALUE_REFRESH_TIME:
	value = REFRESHTIME;
	break;

      case PLATFORM_VALUE_LOG_BUFFER_SIZE:
	if (system_loading)
	    value = 0;
	else
	    value = TWO_K;
	break;

      default:
	value = 0;
	break;
    }

    return(value);
}


/*
 * platform_print_features
 *
 */

static void platform_print_features (void)
{
    /*
     * Print out SW supported.
     */
     if (ciscopro_platform)
         printf("Authorized for CiscoPro software set only.\n");
}


/* platform_interface_init
 * 
 * Initialize platform specific interface features
 */

void platform_interface_init (void)
{
    quicc_dpr_t *dpr;

    dpr = (quicc_dpr_t *) getmbar();

    /*
     * Init the SCC interrupt handlers
     */
    quicc_install_cpm_exception(dpr, quicc_scc1_interrupt,
				QUICC_CPIC_INTERRUPT_SCC1);
    quicc_install_cpm_exception(dpr, quicc_scc2_interrupt,
				QUICC_CPIC_INTERRUPT_SCC2);
    quicc_install_cpm_exception(dpr, quicc_scc3_interrupt,
				QUICC_CPIC_INTERRUPT_SCC3);
    quicc_install_cpm_exception(dpr, quicc_scc4_interrupt,
				QUICC_CPIC_INTERRUPT_SCC4);

    /*
     * Add platform registry services
     */
    c1000_add_registries();
    reg_add_print_features(platform_print_features, "platform_print_features");
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
     * First discover the devices and count them.
     */
    nconlines = 1;
    nauxlines = 0;
    maxvtylines = defvtylines = nvtylines = DEFAULT_NVTYS;

    /*
     * Assign base indexes into the MODEMS[] data structure.
     */
    VTYBase = freeLineBase;

    if (protocolconversion)
        maxvtylines = MAXLINES - VTYBase;

    ALLlines = nconlines + nttylines + nauxlines + nvtylines;

    /* Added for Async */
    reg_invoke_line_init();
    
    vty_init();
}

/*
 * platform_verify_config
 *
 * Validate hardware/software compatability.
 */

void platform_verify_config(void)
{
    if (ciscopro_platform && !ciscopro_image) {
	printf(msgtxt_ciscopro);
	DELAY(2*ONESEC);
	mon_reload();
    }
}
