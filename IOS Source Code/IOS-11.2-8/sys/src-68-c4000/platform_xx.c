/* $Id: platform_xx.c,v 3.4.12.4 1996/09/06 01:12:14 snyder Exp $
 * $Source: /release/112/cvs/Xsys/src-68-c4000/platform_xx.c,v $
 *------------------------------------------------------------------
 * platform_xx.c - Platform specific support for the 68030 C4000
 *
 * February 1995, Scott Mackie (moved from init_xx.c)
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: platform_xx.c,v $
 * Revision 3.4.12.4  1996/09/06  01:12:14  snyder
 * CSCdi68277:  make shr_core, shr_ukernel, sub_cxxxx sub_core_platform
 *              things const, save 712 out of data, 208 from image
 * Branch: California_branch
 *
 * Revision 3.4.12.3  1996/06/19  04:44:24  smackie
 * Buffered logging support gets a hot wax and shine. (CSCdi53462)
 *
 *   o Restructure buffered logging to be a independant device
 *   o Add support for "clear logging"
 *   o Enable buffered logging by default on most platforms
 *   o Start hiding logger structures in private include files
 *
 * Branch: California_branch
 *
 * Revision 3.4.12.2  1996/05/17  11:53:36  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.4.2.2  1996/04/26  07:57:33  dstine
 * Branch: IbuMod_Calif_branch
 * - commit of IBU modularity work, phase 1.
 *
 * Revision 3.4.2.1  1996/04/03  21:40:49  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.4.12.1  1996/03/18  21:59:30  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.12.2  1996/03/09  05:12:27  hampton
 * Slim down the kernel of the router.
 * Branch: DeadKingOnAThrone_branch
 *
 * Revision 3.3.12.1  1996/02/20  17:56:19  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.4  1996/02/13  20:39:27  hampton
 * Move buffer initialization to the platform_xxx.c files to eliminate
 * platform dependencies from common code.  [CSCdi48844]
 *
 * Revision 3.3  1996/01/05  04:56:46  hampton
 * Remove extraneous includes of h/timer.h.  [CSCdi46482]
 *
 * Revision 3.2  1995/11/17  18:43:26  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:25:15  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.5  1995/10/15  18:55:03  eschaffe
 * CSCdi40294:  Increase the number of Protocol Translation sessions
 *
 * Revision 2.4  1995/10/13  00:54:39  smackie
 * Add missing vendor names. (CSCdi42122)
 *
 * Revision 2.3  1995/10/04  23:00:02  hampton
 * The platform_get_value() routine should return CPU and FAMILY values
 * for the hardware it is currently executing upon, not some compiled in
 * value.  This is required to validate that the correct software image
 * has been booted onto a hardware platform.  [CSCdi41098]
 *
 * Revision 2.2  1995/06/28  23:14:48  tkolar
 * CSCdi35465:  I/O memory needs to be included when dumping core
 * Add flag for regions to determine if they are dumped or not.
 *
 * Revision 2.1  1995/06/07  22:54:52  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../../boot/cpu.h"
#include "registry.h"
#include "ttysrv.h"
#include "logger.h"
#include "stacks.h"
#include "interface_private.h"
#include "packet.h"
#include "exception.h"
#include "config.h"
#include "sys_registry.h"
#include "../os/buffers.h"
#include "../os/free.h"
#include "../os/pool.h"
#include "../os/region.h"
#include "../les/if_les.h"
#include "../dev/monitor1.h"

/*
 * Memory pools
 */
static mempool pmem_mempool;
static mempool iomem_mempool;
static mempool sram_mempool;

/*
 * Memory regions
 */
static regiontype main_region;
static regiontype text_region;
static regiontype data_region;
static regiontype bss_region;
static regiontype pmem_region;
static regiontype iomem_region;
static regiontype sram_region;
static regiontype pmem32_region;

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

static char hw_revision[20];
static char hw_serial[20];

/*
 * Local miscellaneous variables
 */
cookie_t   *cookie;		/* Address of cookie PROM */
hwidbtype **nim_idb_array;      /* Array of IDBs for interrupt handlers */

/*
 * Defines
 */

#define SRAM_MALLOC_START (uchar *)0x00001000
#define SRAM_MALLOC_SIZE           0x00010000


/*
 * platform_main
 *
 * Called from main() to allows platforms to initialize basic functions
 * such as LEDs.
 */

void platform_main (void)
{
    ushort hw_version;
    uint serial;

    /*
     * Set the address of the cookie PROM and the status register pointer
     */
    cookie = mon_get_cookie();
    if ((ulong)cookie == -1)
	cookie = (cookie_t *)ADRSPC_COOKIE;

    /*
     * Build platform strings
     */
    hw_version = mon_get_rev();
    sprintf(hw_revision, "0x%02x", hw_version);

    serial = GETLONG(cookie->serial);
    serial = (((serial & 0x0f) - 10) << 28) + (serial >> 4);
    sprintf(hw_serial, "%u", serial);

    /*
     * Turn on the RUN lamp to indicate that we're up and running
     */
    *(volatile ulong *)ADRSPC_CONTROL &= (~CONTROL_RUNLAMP);
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
}    


/*
 * platform_memory_init
 *
 * Define and declare all the memory regions and mempools for this platform
 */

void platform_memory_init (void)
{
    ulong memsize, mainsize, iosize;
    uchar *iostart;

    /*
     * Find out how much main dram the Rom Monitor thinks we've got
     */
    memsize = mon_getmemsize();
    
    /*
     * If we've got more than 16Meg of RAM, we've got two banks. Only
     * return 16Meg for pmem as that's the size of the first bank.
     */
    mainsize = (memsize > SIXTEEN_MEG) ? SIXTEEN_MEG : memsize;

    /*
     * Add a region to describe all of main DRAM
     */
    region_create(&main_region, "main",
		  RAMBASE, mainsize,
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
		  _END, (mainsize - (ulong)_END),
		  REGION_CLASS_LOCAL, REGION_FLAGS_DEFAULT);

    mempool_create(&pmem_mempool, "Processor", &pmem_region, 0,
		   NULL, 0, MEMPOOL_CLASS_LOCAL);

    /*
     * If we've got more than 16M of memory, it's in two banks of memory
     * with the second bank mapped below shared memory in cacheable
     * address space.
     *
     * Declare a region and add it to pmem_mempool.
     */
    if (memsize > SIXTEEN_MEG) {
	region_create(&pmem32_region, "heap32",
		      (void *)ADRSPC_EXP_RAM, SIXTEEN_MEG,
		      REGION_CLASS_LOCAL, 
		      REGION_FLAGS_DEFAULT | REGION_FLAGS_EXCEPTION_DUMP);

	mempool_add_region(&pmem_mempool, &pmem32_region);
    }

    /*
     * Find out how much shared memory is installed
     */
    iosize  = mon_get_iosize();
    iostart = mon_get_iobase();

    /*
     * Declare a region and mempool for shared memory
     */
    region_create(&iomem_region, "iomem",
		  iostart, iosize,
		  REGION_CLASS_IOMEM,
		  REGION_FLAGS_DEFAULT | REGION_FLAGS_EXCEPTION_DUMP);
    mempool_create(&iomem_mempool, "I/O", &iomem_region, 0,
		   freelist_iomem_block_sizes, nfreelist_iomem_block_sizes,
		   MEMPOOL_CLASS_IOMEM);

    /*
     * Declare a region and mempool for fast SRAM memory
     */
    region_create(&sram_region, "sram",
		  SRAM_MALLOC_START, SRAM_MALLOC_SIZE,
		  REGION_CLASS_FAST, REGION_FLAGS_DEFAULT);
    mempool_create(&sram_mempool, "SRAM", &sram_region, 0,
		   NULL, 0, MEMPOOL_CLASS_FAST);

    /*
     * If SRAM runs out, give it someplace to go.
     */
    mempool_add_alternate_pool(&sram_mempool, &pmem_mempool);

    /*
     * Add aliases for mandatory mempools
     */
    mempool_add_alias_pool(MEMPOOL_CLASS_ISTACK, &sram_mempool);
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
	pool_adjust(big, BIG_MIN, C4000_BIG_MAX1, BIG_INIT1, TRUE);
	pool_adjust(verybig, VERYBIG_MIN, VERYBIG_MAX1, VERYBIG_INIT1, TRUE);
	pool_adjust(large, LARGE_MIN, LARGE_MAX1, LARGE_INIT1, TRUE);
	pool_adjust(huge, HUGE_MIN, HUGE_MAX1, HUGE_INIT1, TRUE);
	break;
      case PLATFORM_BUFFER_ROUTING:
	pool_adjust(small, SMALL_MIN, SMALL_MAX2, SMALL_INIT2, TRUE);
	pool_adjust(middle, MID_MIN, MID_MAX2, MID_INIT2, TRUE);
	pool_adjust(big, BIG_MIN, BIG_MAX2, BIG_INIT2, TRUE);
	pool_adjust(verybig, VERYBIG_MIN, VERYBIG_MAX2, VERYBIG_INIT2, TRUE);
	pool_adjust(large, LARGE_MIN, LARGE_MAX2, LARGE_INIT2, TRUE);
	pool_adjust(huge, HUGE_MIN, HUGE_MAX2, HUGE_INIT2, TRUE);
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
    uchar  vendor = cookie->vendor;

    switch (type) {
      case PLATFORM_STRING_NOM_DU_JOUR:
	value = "4000";
	break;

      case PLATFORM_STRING_DEFAULT_HOSTNAME:
	value = "Router";
	break;

      case PLATFORM_STRING_PROCESSOR:
	value = "68030";
	break;

      case PLATFORM_STRING_PROCESSOR_REVISION:
	value = NULL;
	break;

      case PLATFORM_STRING_VENDOR:
	switch (vendor) {
	  case VENDOR_CISCO:
	    value = "cisco";
	    break;
	  case VENDOR_CABLETRON:
	    value = "Cabletron";
	    break;
	  case VENDOR_ALCATEL:
	    value = "Alcatel";
	    break;
	  default:
	    value = "Unknown Vendor";
	    break;
	}
	break;

      case PLATFORM_STRING_HARDWARE_REVISION:
	value = hw_revision;
	break;

      case PLATFORM_STRING_HARDWARE_SERIAL:
	value = hw_serial;
	break;

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
	value = (cpu_type == CPU_XX) ? FAMILY_XX : FAMILY_UNKNOWN;
	break;

      case PLATFORM_VALUE_REFRESH_TIME:
	value = REFRESHTIME;
	break;

      case PLATFORM_VALUE_LOG_BUFFER_SIZE:
	if (system_loading)
	    value = 0;
	else
	    value = FOUR_K;
	break;

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
    ulong **nim_scratch;
    int     count;

    /*
     * Find the base of our NIM array
     */
    nim_idb_array = (hwidbtype **)HWIDB_TABLE_BASE;

    /*
     * Reset all the IDB entries to the default bad address
     */
    for (count = 0; count < HWIDB_TABLE_ENTRIES; count++)
	nim_idb_array[count] = (hwidbtype *)BAD_ADDRESS;

    nim_scratch = (ulong **)NIM_SCRATCH_BASE;

    /*
     * Reset all the entries to the default bad address
     */
    for (count = 0; count < NIM_SCRATCH_ENTRIES; count++)
	nim_scratch[count] = (ulong *)BAD_ADDRESS;

    /*
     * Create the network interface interrupt stack:
     */
    createlevel(ETHER_INTLEVEL, NULL, "Network interfaces");

    /*
     * Create a cache buffer pool in static ram for the ichain calls
     */
    init_ichain_cache();

    /*
     * Create and initialize the High IRQ interrupt dispatcher
     */
    nim_init_hi_irq_handler();

    /*
     * Add platform registry services
     */
    reg_add_print_memory(platform_print_memory, "platform_print_memory");
    reg_add_net_cstate(nim_health_xx_light, "nim_health_xx_light");
    reg_add_onemin(check_for_xx_overtemp, "check_for_xx_overtemp");
    reg_add_subsys_init_class(nim_subsys_init_class,
			      "nim_subsys_init_class");
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
    nauxlines = 1;
    maxvtylines = defvtylines = nvtylines = DEFAULT_NVTYS;

    /*
     * Assign base indexes into the MODEMS[] data structure.
     */
    AuxBase = freeLineBase;
    freeLineBase += nauxlines;
    VTYBase = freeLineBase;

    /* 
     * Increase the number of VTY lines
     */
    if (protocolconversion)
	maxvtylines = MAXLINES - VTYBase;

    ALLlines = nconlines + nttylines + nauxlines + nvtylines;

    /*
     * Initialize the MODEMS[] data structure
     */
    auxline_init();
    vty_init();
}

/*
 * platform_verify_config
 *
 * Validate hardware/software compatability.
 */

void platform_verify_config(void)
{
}
