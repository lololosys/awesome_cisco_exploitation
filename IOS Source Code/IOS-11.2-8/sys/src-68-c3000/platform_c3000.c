/* $Id: platform_c3000.c,v 3.7.10.8 1996/09/06 01:12:11 snyder Exp $
 * $Source: /release/112/cvs/Xsys/src-68-c3000/platform_c3000.c,v $
 *------------------------------------------------------------------
 * platform_c3000.c - Platform specific support for 25XX/3XXX boxes
 *
 * March 1995, Scott Mackie (moved from init_pan.c)
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: platform_c3000.c,v $
 * Revision 3.7.10.8  1996/09/06  01:12:11  snyder
 * CSCdi68277:  make shr_core, shr_ukernel, sub_cxxxx sub_core_platform
 *              things const, save 712 out of data, 208 from image
 * Branch: California_branch
 *
 * Revision 3.7.10.7  1996/06/19  04:44:21  smackie
 * Buffered logging support gets a hot wax and shine. (CSCdi53462)
 *
 *   o Restructure buffered logging to be a independant device
 *   o Add support for "clear logging"
 *   o Enable buffered logging by default on most platforms
 *   o Start hiding logger structures in private include files
 *
 * Branch: California_branch
 *
 * Revision 3.7.10.6  1996/06/17  08:35:38  jhernand
 * Commit Modular Cobra to California_branch
 *
 * Revision 3.7.10.5  1996/06/13  19:09:13  etrehus
 * Commit boa to California Branch
 *
 * Revision 3.7.10.4  1996/05/17  11:53:30  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.7.2.3  1996/05/05  23:52:34  ppearce
 * Sync to IbuMod_Calif_baseline_960504
 *
 * Revision 3.7.2.2  1996/04/26  07:57:29  dstine
 * Branch: IbuMod_Calif_branch
 * - commit of IBU modularity work, phase 1.
 *
 * Revision 3.7.2.1  1996/04/03  21:40:42  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.7.10.3  1996/04/30  18:38:22  etrehus
 * CSCdi51546:  Need warning for non-isdn image on platforms with bri
 * Branch: California_branch Committed.
 *
 * Revision 3.7.10.2  1996/04/19  20:24:10  etrehus
 * CSCdi51928:  3000 restarts after SCHED-2-WATCH error
 * Added temporary level 3 interrupt handler if mk5025 is present during
 * boot sequence to work around hardware shortcommings, cause the interrupt
 * cannot be disabled at the source.
 * Branch: California_branch
 *
 * Revision 3.7.10.1  1996/03/18  21:59:17  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.4.12.3  1996/03/16  07:34:21  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.4.12.2  1996/03/09  05:12:21  hampton
 * Slim down the kernel of the router.
 * Branch: DeadKingOnAThrone_branch
 *
 * Revision 3.4.12.1  1996/02/20  17:56:11  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.7  1996/02/27  06:00:11  afactor
 * CSCdi47194:  Add cookie capability for jalapeno mechanical option
 *
 * Revision 3.6  1996/02/19  17:28:44  abaindur
 * CSCdi30593:  SHOW VERSION underreports processor memory by 4 kilobytes
 *
 * Revision 3.5  1996/02/13  20:39:23  hampton
 * Move buffer initialization to the platform_xxx.c files to eliminate
 * platform dependencies from common code.  [CSCdi48844]
 *
 * Revision 3.4  1996/01/05  04:56:44  hampton
 * Remove extraneous includes of h/timer.h.  [CSCdi46482]
 *
 * Revision 3.3  1995/12/03  18:00:18  thille
 * CSCdi44905:  ciscopro targets not in 11.1
 * Put them in.
 *
 * Revision 3.2  1995/11/17  18:43:20  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:25:11  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.9  1995/10/20  00:54:37  ljiang
 * CSCdi42394:  Add vendor codes for cisco partners
 *
 * Revision 2.8  1995/10/15  18:55:00  eschaffe
 * CSCdi40294:  Increase the number of Protocol Translation sessions
 *
 * Revision 2.7  1995/10/04  22:59:56  hampton
 * The platform_get_value() routine should return CPU and FAMILY values
 * for the hardware it is currently executing upon, not some compiled in
 * value.  This is required to validate that the correct software image
 * has been booted onto a hardware platform.  [CSCdi41098]
 *
 * Revision 2.6  1995/08/14  19:32:14  ebuell
 * CSCdi36998:  Remove the Authorized for portion of the banner
 *
 * The Feature Set feature feature was never used.  Remove it's
 * output from the banner.
 *
 * Revision 2.5  1995/08/13  18:48:59  smackie
 * Add code review comments. (CSCdi38689)
 *
 * Revision 2.4  1995/08/12  23:46:37  smackie
 * Fix wrongly formatted output of platform serial number. (CSCdi38689)
 *
 * Revision 2.3  1995/08/07  00:29:10  ssangiah
 * CSCdi38231:  Reserve cookie value for new development
 * Knowledge of this cookie value for the new platform has also been added
 * to the init code for the appropriate drivers so that the boot ROMs from
 * this release will initialize the corresponding interfaces on the
 * platform properly.
 *
 * Revision 2.2  1995/06/28  23:14:46  tkolar
 * CSCdi35465:  I/O memory needs to be included when dumping core
 * Add flag for regions to determine if they are dumped or not.
 *
 * Revision 2.1  1995/06/07  22:54:45  hampton
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
#include "subsys.h"
#include "../os/buffers.h"
#include "../os/free.h"
#include "../os/pool.h"
#include "../os/region.h"
#include "../les/if_les.h"
#include "../dev/monitor1.h"
#include "../les/if_c3000_bri.h"
#include "../les/flh.h"
#include "../les/if_les_mk5025.h"
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
static regiontype flhlog_region;

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
static char hw_rework[20];

/*
 * Local miscellaneous variables
 */
cookie_t *cookie;		/* Address of cookie PROM */
volatile ushort *status_ptr;	/* Address of status register */

hwidbtype *network_a_idb;	/* IDB addresses for interfaces */
hwidbtype *network_b_idb;
hwidbtype *network_c_idb;
hwidbtype *network_c0_idb;
hwidbtype *network_c1_idb;
hwidbtype *network_d_idb;

/*
 * Forward references
 */
static char *c3000_nom_du_jour(void);
static void c3000_reset_interrupts(int cputype, cookie_t *cookie);

/*
 * Externs
 */
extern void level2_dummy_rte(void);


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
    uint rework;
    int cputype;

    /*
     * Whilst we're in here, set up the cookie and status register
     */
    cookie = mon_get_cookie();
    if ((ulong)cookie == -1)
        cookie = (cookie_t *)ADRSPC_COOKIE;
    status_ptr = (volatile ushort *)ADRSPC_STATUS;
 
    if (cookie->capabilities[1] & CISCOET_PLATFORM) {
        ciscoet_platform = TRUE;
    }
    if (cookie->magic[0] == COOKIE_MAGIC_0 &&
	cookie->magic[1] == COOKIE_MAGIC_1 ) {
	if (cookie->capabilities[1] & CISCOPRO_PLATFORM) {
            ciscopro_platform = TRUE;
	}
    } 

    /*
     * Build platform strings
     */
    hw_version = mon_get_rev();
    if (hw_version && (hw_version <= ('Z' - 'A')))
	sprintf(hw_revision, "%c", (hw_version - 1 + 'A'));
    else
	sprintf(hw_revision, "0x%02x", hw_version);
    
    /*
     * Format the serial number of the cookie. If the serial number is
     * from an older IGS-style platform, format it into an integer using
     * the baroque manipulation below. If it's a later rev (such as a
     * 2500), print it as a simple hex number
     */
    serial = GETLONG(cookie->serial);
    if ((serial & 0x0f) < 10) {
	sprintf(hw_serial, "%08x", serial);
    } else {
	serial = (((serial & 0x0f) - 10) << 28) + (serial >> 4);
	sprintf(hw_serial, "%u", serial);
    }

    /*
     * Format the hardware rework number.  This is set by manufacturing
     * after applying ECOs to a board.
     */
    rework = GETLONG(cookie->hw_rework);
    sprintf(hw_rework, "%08x", rework);

    /*
     * The routine gets called so early, nothing is initialized yet
     */
    cputype = mon_proctype();
 
    /*
     * Reset any oustanding interrupt sources
     */
    c3000_reset_interrupts(cputype, cookie);
    
    /*
     * Turn on the RUN lamp to indicate that we're up and running
     */
    *(volatile ushort *)ADRSPC_CONTROL |= CONTROL_RUNLAMP;
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
 * platform_get_flhlogsize
 *
 * Get size of any main memory reserved for the FLH
 */
static ulong platform_get_flhlogsize (void)
{
    int buff_len, ver;

    /*
     * Get processor memory size usurped by FLH. 
     */
    buff_len = FLH_LOGBUFSIZE;
    if (buff_len >= 0)  /* Valid length from emt call */
	return (buff_len);

    ver = FLH_ROMVERSION;
    if (ver < FLH_VERSION_INITIAL) 
	return (0);  /* No FLH support in boot ROMs */

    return (DEFAULT_FLHLOG_SIZE);
}

/*
 * platform_memory_init
 *
 * Define and declare all the memory regions and mempools for this platform
 */

void platform_memory_init (void)
{
    ulong mainsize, iosize, flhlogsize;
    uchar *iostart;

    /*
     * Find out how much main dram and iomemory the Rom Monitor thinks
     * we've got
     */
    mainsize = mon_getmemsize();
    flhlogsize = platform_get_flhlogsize();
    iosize   = mon_get_iosize();
    iostart  = mon_get_iobase();

    /*
     * If this box is an IGS running old EPROMs, ie 8.2, which
     * don't support the IOSIZE EMT call, assume IO memory is 512 KB:
     */
    if (iosize == -1L)
	iosize = HALF_MEG;

    /*
     * Check if this is an older version of the rom monitor.
     * Older versions require the size of shared I/O mem to be 
     * subtracted off the total if the size of dram < 16 Mbytes
     */
    if ((mon_version() < 0x406) && (mainsize < (16*ONE_MEG)))
	mainsize -= iosize; 

    /*
     * Add a region to describe all of main DRAM
     */
    region_create(&main_region, "main",
		  RAMBASE, mainsize + flhlogsize,
		  REGION_CLASS_LOCAL, 
		  REGION_FLAGS_DEFAULT | REGION_FLAGS_EXCEPTION_DUMP);

    if (flhlogsize) {
	region_create(&flhlog_region, "flhlog",
		      (void *)(RAMBASE+mainsize), flhlogsize,
		      REGION_CLASS_LOCAL,
		      REGION_FLAGS_DEFAULT);
    }
    
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
     * If we've got a Brut or Cancun, allow main memory to steal from
     * shared memory under low memory situations
     */
    if ((cookie->processor == PROCESSOR_BRUT) ||
	(cookie->processor == PROCESSOR_CANCUN))
	mempool_add_alternate_pool(&pmem_mempool, &iomem_mempool);

    /*
     * Add aliases for mandatory mempools
     */
    mempool_add_alias_pool(MEMPOOL_CLASS_FAST, &pmem_mempool);
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
	pool_adjust(verybig, VERYBIG_MIN, VERYBIG_MAX2, VERYBIG_INIT2, TRUE);
	pool_adjust(large, LARGE_MIN, LARGE_MAX2, LARGE_INIT2, TRUE);
	pool_adjust(huge, HUGE_MIN, HUGE_MAX2, HUGE_INIT2, TRUE);
	break;
    }
}

/*
 * platform_get_string
 *
 * Returns a variety of strings unique to the 3000
 */

char *platform_get_string (platform_string_type type)
{
    char  *value;
    uchar  vendor    = cookie->vendor;
    uchar  processor = cookie->processor;

    switch (type) {
      case PLATFORM_STRING_NOM_DU_JOUR:
	value = c3000_nom_du_jour();
	break;

      case PLATFORM_STRING_DEFAULT_HOSTNAME:
	value = "Router";
	break;

      case PLATFORM_STRING_PROCESSOR:
	switch(processor) {
	  case PROCESSOR_PAN:
	    value = "68020";
	    break;
	  case PROCESSOR_MERLOT:
	  case PROCESSOR_BRUT:
	  case PROCESSOR_BASS:
	  case PROCESSOR_CRISTAL:
	  case PROCESSOR_CANCUN:
	    value = "68030";
	    break;
	  default:
	    value = "Unknown";
	    break;
	}
	break;

      case PLATFORM_STRING_PROCESSOR_REVISION:
	value = NULL;
	break;

      case PLATFORM_STRING_VENDOR:
	switch (vendor) {
	  case VENDOR_CISCO:
	    value = "cisco";
	    break;
	  case VENDOR_CHIPCOM:
	    value = "Chipcom";
	    break;
	  case VENDOR_DEC:
	    value = "DECbrouter 90";
	    break;
	  case VENDOR_NCR:
	    value = "NCR";
	    break;
	  case VENDOR_USROBOTICS:
	    value = "USRobotics";
	    break;
	  case VENDOR_ALCATEL:
	    value = "Alcatel";
	    break;
	  case VENDOR_NEC:
	    value = "NEC";
	    break;
	  case VENDOR_DSC:
	    value = "DSC";
	    break;
	  case VENDOR_MICROCOM:
	    value = "Microcom";
	    break;
	  case VENDOR_UBN:
            value = "UB Network";
	    break;
          case VENDOR_HP:
	    value = "Hewlett Packard";
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

      case PLATFORM_STRING_HARDWARE_REWORK:
	value = hw_rework;
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
	switch (cpu_type) {
	  case CPU_MERLOT:
	  case CPU_IGS_BRUT:
	  case CPU_BASS:
	  case CPU_CRISTAL:
	  case CPU_CANCUN:
	    value = FAMILY_PAN;
	    break;
	  default:
	    value = FAMILY_UNKNOWN;
	    break;
	}
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
 * count_bri_hardware()
 * returns 1 if platform has bri hardware, otherwise 0.
 */
int count_bri_hardware (void)
{
   if ((cookie->interfaces == INTERFACE_1E1B)   ||
       (cookie->interfaces == INTERFACE_1R1B)   ||
       (cookie->interfaces == INTERFACE_1R1T1B) ||
       (cookie->interfaces == INTERFACE_1E1T1B) ||
       (cookie->interfaces == INTERFACE_1R2T1B) ||
       (cookie->interfaces == INTERFACE_1E2T1B) ||
       (cookie->interfaces == INTERFACE_1E2T1Bhub) ||
       (cookie->interfaces == INTERFACE_1E2T1B2AS) ||
       (cookie->interfaces == INTERFACE_1R2T1B2AS)) {
       return(1);
   }
   else if ((cookie->interfaces == INTERFACE_1E1B2W) ||
            (cookie->interfaces == INTERFACE_1R1B2W)) {
        if (BOA_BRI_ANY_MODULES_PRESENT)
            return (1);
   }
   else {  /* Fiji case. Check daughter board presence */
       if ((cookie->interfaces == INTERFACE_1E1T1I) ||
           (cookie->interfaces == INTERFACE_1R1T1I))
           if (FIJI_D_CARD_PRESENT) {
               return(1);
           }
   }
   return(0);
}


/*
 * platform_print_features
 *
 */

static void platform_print_features (void)
{
    /*
     * Print out SW supported and mechanical options,
     * ie. ciscopro, jalapeno, etc.
     */
    if (ciscopro_platform) {
        printf("Authorized for CiscoPro software set only.\n");
    }
    if (ciscoet_platform) {
        printf("Cisco-ET Extended Temperature platform.\n");
    }
    if (count_bri_hardware() != 0 && subsys_find_by_name("isdn") == NULL) {
      printf("WARNING: No ISDN software support in this image.\n");
    }
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
    int i;

    /*
     * Initialize IDB values
     */
    network_a_idb  = (hwidbtype *)BAD_ADDRESS;
    network_b_idb  = (hwidbtype *)BAD_ADDRESS;
    network_c_idb  = (hwidbtype *)BAD_ADDRESS;
    network_c0_idb = (hwidbtype *)BAD_ADDRESS;
    network_c1_idb = (hwidbtype *)BAD_ADDRESS;
    network_d_idb  = (hwidbtype *)BAD_ADDRESS;

    /*
     * Create the network interface interrupt stack:
     */
    createlevel(ETHER_INTLEVEL, NULL, "Network interfaces");

    /*
     * Set up interrupt pointers
     */
    switch (cpu_type) {
      case CPU_CRISTAL:
      case CPU_CANCUN:
	/* Level 3 Serial interface change interrupt needs to be disabled here
	 * for IGS subset images that do not include the serial driver.
	 * If the serial driver subsystem is present, this interrupt will be
	 * enabled when the serial subsystem is activated.
	 */
	*((ushort *)ADRSPC_DEVICE_SERIAL_0) &= ~DEVICE_SERIAL_INT_ENB;
	*((ushort *)ADRSPC_DEVICE_SERIAL_1) &= ~DEVICE_SERIAL_INT_ENB;
	exceptionHandler(IRQ3Vector, level3_cristal);
	switch (cookie->interfaces) {
	  case INTERFACE_2R2T:
	    exceptionHandler(IRQ4Vector, level4_campeche_2r2t);
            break;
          case INTERFACE_2E2T:
	    exceptionHandler(IRQ4Vector, level4_campeche_2e2t);
            break;
          case INTERFACE_1R2T1E:
	    exceptionHandler(IRQ4Vector, level4_campeche_1r2t1e);
            break;
          default:
	    exceptionHandler(IRQ4Vector, level4_cristal);
            break;
	}
        break;
      case CPU_IGS_BRUT:
	exceptionHandler(IRQ3Vector, level3_pan);
	exceptionHandler(IRQ4Vector, level4_brut);
        break;
      default:
	exceptionHandler(IRQ3Vector, level3_pan);
	exceptionHandler(IRQ4Vector, level4_pan);
        break;
    }
    
    /*
     * Initialize the handler array
     */
    for (i = 0; i < PAN_MAX_INTER; i++) {
	pan_int_array[i].function = (ulong)level4_nop;
	pan_int_array[i].idb1 = (hwidbtype *)i;
    }

    /*
     * Add platform registry services
     */
    reg_add_print_memory(platform_print_memory, "platform_print_memory");
    reg_add_print_features(platform_print_features, "platform_print_features");
}

boolean (*platform_interface_ordering) (hwidbtype *new_idb, hwidbtype *old_idb)

     = idb_default_ordering;

/*
 * c3000_hardware_shutdown()
 * The hardware shutdown routine for the 2500/3000 platforms.
 */
static void c3000_hardware_shutdown (void)
{
    /*
     * Certain IGS ROM monitors will loop if we execute EMT_RELOAD with
     * the console transmitter disabled. Avoid this by enabling the
     * transmitter right before the system image exits
     */
    tty_portenable(console);  /* Ensure that the console is enabled */

    /*
     * During the FLH operation, we need to reset the Cirrus controllers
     * since the rxboot does not yet have support for the Cirrus interfaces.
     * This reset is needed to ensure that we do not take any interrupt on
     * these controllers in the rxboot where there is no handler to handle
     * these interrupts.
     */
    if ((cookie->interfaces == INTERFACE_1E2T1B2AS) ||
        (cookie->interfaces == INTERFACE_1R2T1B2AS)) {
        (*(ushort *)ADRSPC_LSS_RESET_ASEL) &= DEVICE_LSS_RESET_MASK;
        DELAY(5);
        (*(ushort *)ADRSPC_LSS_RESET_ASEL) |= ~DEVICE_LSS_RESET_MASK;
    }
}

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
    nauxlines = analyzeAUXports();
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
    if (nauxlines)
        auxline_init();

    /*
     * Possibly initialize other line drivers in the system
     */
    reg_invoke_line_init();

    vty_init();

    reg_add_hardware_shutdown(c3000_hardware_shutdown, "c3000_hardware_shutdown");
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

/*
 * c3000_nom_du_jour
 *
 * Return the textual name for the IGS/c2500/c3000 derivative.
 */

static char *c3000_nom_du_jour (void)
{
    uchar vendor = cookie->vendor;
    char *value;
    ushort misc_reg;
    static char ciscopro_value[21];

    switch (cpu_type) {
      case CPU_PAN:        /* IGS, 16 MHz 68020 */
	value = "IGS";
	break;
      case CPU_MERLOT:     /* Merlot , 20 Mhz 68030 */
	value = "3000";
	break;
      case CPU_IGS_BRUT:   /* IGS-BRUT,68030 20MHz, w/ no CPU memory */
	if (vendor == VENDOR_DEC)
	    value = "router";
	else
	    value = "2000";
	break;
      case CPU_CRISTAL:    /* Cristal (3 port merlot) */
	value = "3000";
	break;
      case CPU_CANCUN: /* Cancun similar to Cristal with ASIC and Brut  */
	if (cookie->interfaces == INTERFACE_1R1T1I) {
	    if (FIJI_D_CARD_PRESENT) {
		if (ciscopro_platform)
		    value = "2500PCEI";
		else
		    value = "AP-RBC";
	    } else {
		if (ciscopro_platform)
		    value = "2500PCE";
		else
		    value = "AP-RC";
	    }
	} else if (cookie->interfaces == INTERFACE_1E1T1I) {
	    if (FIJI_D_CARD_PRESENT)
		value = "AP-EBC";
	    else
		value = "AP-EC";
	} else if (cookie->interfaces == INTERFACE_1E2Thub) {
	    if (netherhubports == 8)
		value = "2505";
	    else 
		value = "2507";
	} else if (cookie->interfaces == INTERFACE_1E2T1Bhub) {
	    value = "2516";
        } else if (cookie->interfaces == INTERFACE_1E1B2W) {
            value = "2524";
        } else if (cookie->interfaces == INTERFACE_1R1B2W) {
            value = "2525";
        } else if (touch_device((void *)ADRSPC_LSS_MISC)) {
            /*
             * This is either a Copan or a Cobra. Read the miscellaneous
             * register and determine whether or not the daughter card is
             * installed. This information along with the value of the
             * interfaces field of the cookie is then used to derive the
             * exact model number for the platform.
             * Note that Copans might have the 1E2T or 1R2T value in
             * the interfaces field.
             */
            misc_reg = *(ushort *)ADRSPC_LSS_MISC;
            misc_reg &= LSS_DAUGHTER;
            switch (cookie->interfaces) {
            case INTERFACE_1E2T8A:
            case INTERFACE_1E2T:
                if (!misc_reg)
                    value = "2511"; /* Ethernet Copan with daughter card */
                else
                    value = "2509"; /* Ethernet Copan without daughter card */
                break;
            case INTERFACE_1R2T8A:
            case INTERFACE_1R2T:
                if (!misc_reg)
                    value = "2512"; /* Token Ring Copan with daughter card */
                else
                    value = "2510"; /* Token Ring Copan without daughter card */
                break;
            case INTERFACE_1E2T1B2AS:
                if (!misc_reg)
                    value = "2522"; /* Ethernet Cobra with daughter card */
                else
                    value = "2520"; /* Ethernet Cobra without daughter card */
                break;
            case INTERFACE_1R2T1B2AS:
                if (!misc_reg)
                    value = "2523"; /* Token-ring Cobra with daughter card */
                else
                    value = "2521"; /* Token-ring Cobra without daughter card */
                break;
            default:
                value = "2500"; /* Should never happen */
                break;
            }
        } else if (cookie->interfaces == INTERFACE_1E1B2W) {
            value = "2524";
        } else if (cookie->interfaces == INTERFACE_1R1B2W) {
            value = "2525";
        } else {
	    value = "2500";
	}
	if (ciscopro_platform) {
	    sprintf(ciscopro_value, "CPA%s", value);
	    value = ciscopro_value;
	}
	break;
      case CPU_BASS:       /* Bass and Pinot */
	value = "3000";
	break;
      default:
	value = "Unknown";
	break;
    }

    return(value);
}


/*
 * c3000_spurious_level4
 *
 * Stub handler for 3000 assembly wrappers
 */

void c3000_spurious_level4 (int int_num)
{
    buginf("\nInterrupt (%d) from an interface with no handler\n", int_num);
}


/*
 * reset_fiji_online()
 *
 * This is called early in the main() to prevent unexpected PCbus
 * interrupts. The IOS initialization expects that all devices are
 * in reset state when entering the system image. However, the
 * PCbus interface does not provide a way to mask the interrupts
 * from the device (in addition to mask interrupts from CPU).
 * There are chances that the PC host will generate interrupts to
 * the router while other subsystem initializations enable all
 * interrupts before the PCbus subsystem has chance to install
 * the real ISR. This condition will cause the router to crash.
 *
 */

static void reset_fiji_online (int mycputype, cookie_t *mycookie)
{
    if (mycputype == CPU_CANCUN) {
        if ((mycookie->interfaces == INTERFACE_1E1T1I) ||
            (mycookie->interfaces == INTERFACE_1R1T1I)) {

            /* install the dummy handler */
            exceptionHandler(IRQ2Vector, level2_dummy_rte);
        }
    }
}


/*
 * count_hd64570()
 *
 * Check to see if we've got any Hitachi's.
 */

int count_hd64570 (int cpu_type, cookie_t *cookie)
{
    /*
     * Check to see if we have any Hitachi's on board
     */
    switch (cpu_type) {
      case CPU_CRISTAL:
      case CPU_CANCUN:
	{
	    switch (cookie->interfaces) {
		/*
		 * Check to see if we've got just one Hitachi
		 */
	      case INTERFACE_1R1T:
	      case INTERFACE_1E1T:
	      case INTERFACE_1R1T1B:
	      case INTERFACE_1E1T1B:
		return(1);
		
		/*
		 * Check to see if we have a second serial interface
		 */
	      case INTERFACE_1R2T:
	      case INTERFACE_1E2T:
	      case INTERFACE_1E2Thub:
	      case INTERFACE_1E2T1Bhub:
	      case INTERFACE_1R2T1B:
	      case INTERFACE_1E2T1B:
	      case INTERFACE_1R2T8A:
	      case INTERFACE_1E2T8A:
	      case INTERFACE_1R2T1E:
	      case INTERFACE_2R2T:
	      case INTERFACE_2E2T:
              case INTERFACE_1E2T1B2AS:
              case INTERFACE_1R2T1B2AS:
              case INTERFACE_1E1B2W:
              case INTERFACE_1R1B2W:
		return(2);
		
		/*
		 * Fiji case. Check daughter board presence
		 */
	      case INTERFACE_1E1T1I:
	      case INTERFACE_1R1T1I:
		if (FIJI_D_CARD_PRESENT) {
		    return(2);
		} else {
		    return(1);
		}
	    
	      default:
		/*
		 * Bummer. Unknown derivative
		 */
		return(0);
	    }
	}

      default:
	return(0);
    }
}


/*
 * reset_hd64570_online - Reset HD64570 serial interface
 *
 * The ROM monitor does not reset the HD64570 serial interface interrupts,
 * so we have to do it here. We get the number of devices, and the
 * reset them.
 *
 * This routine really should have a better way of getting the device
 * address, but there isn't one at present. The device drivers jam this
 * data into IDBs, but at this point, the device drivers haven't been
 * initialized yet.
 */

static void reset_hd64570_online (int mycputype, cookie_t *mycookie)
{
    int count;
    ushort *reg;
    int i;
    
    count = count_hd64570(mycputype, mycookie);       /* Count the units */
    
    for (i = 0; i < count; i++) {
	/*
	 * Get the device register address 
	 */
	if (i == 0)
	    reg = (ushort *)ADRSPC_DEVICE_SERIAL_0;
	else
	    reg = (ushort *)ADRSPC_DEVICE_SERIAL_1;
	
	/*
	 * Clear out possible interrupts
	 */
	*reg &= ~(DEVICE_SERIAL_INT_CLR |
		  DEVICE_SERIAL_INT_ENB |
		  DEVICE_SERIAL_LTST    |
		  DEVICE_SERIAL_CTS     |
		  DEVICE_SERIAL_RTS     |
		  DEVICE_SERIAL_DSR     |
		  DEVICE_SERIAL_DTR     |
		  DEVICE_SERIAL_DCD);
    }
}


static void reset_cd2430_online (int cputype, cookie_t *cookie)
{
    if (cputype == CPU_CANCUN) {
        if ((cookie->interfaces == INTERFACE_1E2T1B2AS) ||
            (cookie->interfaces == INTERFACE_1R2T1B2AS)) {
            (*(ushort *)ADRSPC_LSS_RESET_ASEL) &= DEVICE_LSS_RESET_MASK;
            DELAY(5);
            (*(ushort *)ADRSPC_LSS_RESET_ASEL) |= ~DEVICE_LSS_RESET_MASK;
        }
    }
}


/*
 * We stop the mk5025 to prevent level 4 interrupts from occuring.
 * We cannot prevent mk5025 circuitry from asserting level 3 interrupts when
 * modem signal changes occur.  Install a mini interrupt handler 
 * that clears the level 3 interrupt and returns.
 */
static void reset_mk5025_online (int cputype, cookie_t *cookie)
{
  if (cpu_type != CPU_CRISTAL && cpu_type != CPU_CANCUN) {
    if ((cookie->interfaces == INTERFACE_1E2T) ||
	(cookie->interfaces == INTERFACE_1R2T)) {
      mk5025_hw_reg_t *regaddr;

      exceptionHandler(IRQ3Vector,no_mk5025_exception_handler_2);

      regaddr = (mk5025_hw_reg_t *)ADRSPC_NETWORK_C;
      regaddr->rap = MK5025_CSR1;
      regaddr->csr = MK5025_CSR1_UPRIM_STOP | MK5025_CSR1_UAV;
      regaddr->rap = MK5025_CSR0;

      goto below;

    }
    else if ((cookie->interfaces == INTERFACE_1E1T) ||
           (cookie->interfaces == INTERFACE_1R1T)) {
      mk5025_hw_reg_t *regaddr;

      exceptionHandler(IRQ3Vector,no_mk5025_exception_handler);

    below:

      regaddr = (mk5025_hw_reg_t *)ADRSPC_NETWORK_B;
      regaddr->rap = MK5025_CSR1;
      regaddr->csr = MK5025_CSR1_UPRIM_STOP | MK5025_CSR1_UAV;
      regaddr->rap = MK5025_CSR0;

    }
  }
}

/* c3000_reset_interrupts
 *
 * This routine is called at early system startup, before interrupts
 * are enabled, to quiet any devices that may interrupt. While the ROM
 * monitor has the primary responsibility for doing this, some devices
 * are not properly quiesced by the monitor so we need to do them here.
 */

static void c3000_reset_interrupts (int mycputype, cookie_t *mycookie)
{
    reset_fiji_online(mycputype, mycookie);
    reset_hd64570_online(mycputype, mycookie);
    reset_cd2430_online(mycputype, mycookie);
    reset_mk5025_online(mycputype, mycookie);
}

