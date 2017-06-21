/* $Id: platform_rsp.c,v 3.6.12.6 1996/06/19 04:44:32 smackie Exp $
 * $Source: /release/112/cvs/Xsys/src-rsp/platform_rsp.c,v $
 *------------------------------------------------------------------
 * platform_rsp.c - Platform specific support for the RSP
 *
 * March 1995, Scott Mackie (moved from rsp_init.c)
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: platform_rsp.c,v $
 * Revision 3.6.12.6  1996/06/19  04:44:32  smackie
 * Buffered logging support gets a hot wax and shine. (CSCdi53462)
 *
 *   o Restructure buffered logging to be a independant device
 *   o Add support for "clear logging"
 *   o Enable buffered logging by default on most platforms
 *   o Start hiding logger structures in private include files
 *
 * Branch: California_branch
 *
 * Revision 3.6.12.5  1996/05/17  11:54:16  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.6.2.3  1996/05/05  23:53:15  ppearce
 * Sync to IbuMod_Calif_baseline_960504
 *
 * Revision 3.6.2.2  1996/04/26  07:57:43  dstine
 * Branch: IbuMod_Calif_branch
 * - commit of IBU modularity work, phase 1.
 *
 * Revision 3.6.2.1  1996/04/03  21:42:22  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.6.12.4  1996/04/22  15:56:01  bew
 * CSCdi55263:  restore correct method of getting rsp serial number
 * Branch: California_branch
 * Code was accidentally checked in which short-circuited the RSP serial
 * number being returned properly.
 *
 * Revision 3.6.12.3  1996/04/19  15:31:17  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commit
 *
 * Revision 2.1.6.7  1996/04/09  21:57:34  che
 * Branch: IosSec_branch
 * Sync to California_branch (960405)
 *
 * Revision 2.1.6.6  1996/04/02  20:24:48  carrel
 * cvs fixes
 *
 * Revision 2.1.6.5  1996/02/21  04:07:53  che
 * Branch: IosSec_branch
 * Sync to 11.1 Mainline (02/20/96)
 *
 * Revision 2.1.6.4  1996/01/23  06:49:19  che
 * Branch: IosSec_branch
 * Sync with 11.1 mainline. (960122)
 *
 * Revision 2.1.6.3  1995/12/19  04:16:45  che
 * Branch: IosSec_branch
 * Re-sync to Arkansas to fix things that broke.
 *
 * Revision 2.1.6.2  1995/11/17  18:15:54  carrel
 * Branch: IosSec_branch
 * Sync to Arkansas
 *
 * Revision 2.1.6.1  1995/08/30  21:23:17  bew
 * Branch: IosSec_branch
 * Temporary commit to get the rsp serial number. For an unexplained
 * reason the 11.0 method did not actually return it. This will be
 * investigated further after the next time the IosSec_branch is
 * synched up with the mainline.
 *
 * Revision 3.6.12.2  1996/03/21  23:49:35  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.6.12.1  1996/03/18  22:01:32  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.2.4  1996/03/18  23:34:27  mbeesley
 * Branch: ELC_branch
 * Sync to V111_1_3
 *
 * Revision 3.2.2.3  1996/02/16  02:24:15  mbeesley
 * CSCdi48910:  Add storage of fault history for crashes
 * Branch: ELC_branch
 *
 * Revision 3.2.2.2  1996/02/08  09:29:16  gstovall
 * Branch: ELC_branch
 * Sync to ELC_baseline_960205.
 *
 * Revision 3.2.2.1  1996/01/16  02:35:07  dcarroll
 * Branch: ELC_branch
 * Sync ELC_branch between ELC_baseline_120995 and ELC_baseline_960110.
 *
 * Revision 3.6  1996/02/13  20:39:37  hampton
 * Move buffer initialization to the platform_xxx.c files to eliminate
 * platform dependencies from common code.  [CSCdi48844]
 *
 * Revision 3.5  1996/01/15  18:05:03  schaefer
 * CSCdi45761: support RSP in new chassis
 *
 * Revision 3.4  1996/01/05  04:57:03  hampton
 * Remove extraneous includes of h/timer.h.  [CSCdi46482]
 *
 * Revision 3.3  1995/12/15  08:02:01  rramacha
 * CSCdi42101:  sh ver does not recognize CX-SMIP or CX-SSIP cards
 *
 * Revision 3.2  1995/11/17  18:46:48  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:27:15  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.7  1995/11/08  21:27:51  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.6  1995/10/15  18:55:12  eschaffe
 * CSCdi40294:  Increase the number of Protocol Translation sessions
 *
 * Revision 2.5  1995/10/04  23:00:17  hampton
 * The platform_get_value() routine should return CPU and FAMILY values
 * for the hardware it is currently executing upon, not some compiled in
 * value.  This is required to validate that the correct software image
 * has been booted onto a hardware platform.  [CSCdi41098]
 *
 * Revision 2.4  1995/07/25  17:45:19  mleelani
 * CSCdi37536:  Fix the C4700 Millisecond Ticks.
 * Add support for the 133.33 mhz pipe.
 *
 * Revision 2.3  1995/06/28  23:14:55  tkolar
 * CSCdi35465:  I/O memory needs to be included when dumping core
 * Add flag for regions to determine if they are dumped or not.
 *
 * Revision 2.2  1995/06/15  06:40:00  smackie
 * Tidy up R4600/R4700 revision string handler. (CSCdi35883)
 *
 * Revision 2.1  1995/06/07  22:57:06  hampton
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
#include "packet.h"
#include "interface_private.h"
#include "exception.h"
#include "sys_registry.h"
#include "config.h"
#include "../src-4k/signal_4k.h"
#include "../src-4k/util_4k.h"
#include "rsp_bufhdr.h"
#include "rsp_if.h"
#include "rsp_pcmap.h"
#include "rsp_chasint.h"
#include "rsp_qasic.h"
#include "rsp_macaddr.h"
#include "../os/sum.h"
#include "../os/buffers.h"
#include "../os/free.h"
#include "../os/pool.h"
#include "../os/region.h"
#include "../dev/monitor1.h"
#include "../hes/dbus.h"
#include "types.h"

/*
 * Memory pools
 */
static mempool pmem_mempool;
static mempool fast_mempool;

/*
 * Memory regions
 */
static regiontype main_region;
static regiontype text_region;
static regiontype data_region;
static regiontype bss_region;
static regiontype fast_region;
static regiontype pmem_region;

static regiontype memd_region;
static regiontype memd_bitswap_region;
static regiontype memd_writethru_region;
static regiontype memd_uncached_region;

static regiontype k0_main_region;
static regiontype k0_memd_region;
static regiontype k1_main_region;
static regiontype k1_memd_region;


/*
 * Externs
 */


/*
 * Defines
 */

#define FAST_MEMPOOL_SIZE 0x00020000  /* 128K of fast, aligned memory */


/*
 * Miscellaneous local storage
 */

static uchar reset_reason;      /* Last hardware reset reason */
static char proc_revision[80];  /* Processor revision */
int fsip_ser;                   /* sum of all serials on all FSIP's */
int ssip_ser;                   /* sum of all serials on all SSIP's */


/*
 * platform_main
 *
 * Called from main() to allows platforms to initialize basic functions
 * such as exception interrupt handlers and the like.
 */

void platform_main (void)
{
    /*
     * Remember what caused the last hardware reset
     */
    reset_reason = ACTIVE_LOW(mon_reset_reason()) & REASON_ALL;
    if (reset_reason == REASON_ALL)
	reset_reason = 0;

    /*
     * Build the platform strings
     */
    r4k_get_revision_string(proc_revision);

    /*
     * Hook into the scheduler
     */
    kernel_set_sched_loop_hook(rsp_sched_loop_end);
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
     * Initialize RSP exception handlers
     */
    rsp_exception_init();
}


/*
 * platform_memory_init
 *
 * Define and declare all the memory regions and mempools for this platform
 */

void platform_memory_init (void)
{
    ulong memsize, memdsize;

    /*
     * Find out how much main dram the Rom Monitor thinks we've got
     */
    memsize = mon_getmemsize();
    
    /*
     * Add a region to describe all of main DRAM
     */
    region_create(&main_region, "main",
		  (void *)RAMBASE, memsize,
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
     * Mark the text segment as read only as the R4600 TLB is set up
     * to protect this area
     */
    region_set_media(&text_region, REGION_MEDIA_READONLY);

    /*
     * Declare a region from the end of BSS as "fast" memory and set
     * the default alignment to be 32 bytes
     */
    region_create(&fast_region, "fastheap",
		  _END, FAST_MEMPOOL_SIZE,
		  REGION_CLASS_LOCAL, REGION_FLAGS_DEFAULT);

    mempool_create(&fast_mempool, "Fast", &fast_region,
		   RSP_DCACHE_LINE_SIZE,
		   NULL, 0, MEMPOOL_CLASS_FAST);

    /*
     * Declare a region from the end of the fast memory to the end of
     * main dram and create a "local" mempool based on it.
     */
    region_create(&pmem_region, "heap",
		  fast_region.end,
		  ((ulong)main_region.end - (ulong)fast_region.end),
		  REGION_CLASS_LOCAL, REGION_FLAGS_DEFAULT);

    mempool_create(&pmem_mempool, "Processor", &pmem_region, 0,
		   NULL, 0, MEMPOOL_CLASS_LOCAL);

    memdsize = mon_get_iosize();

    /*
     * Create a region for MEMD memory
     */
    region_create(&memd_region, "memd",
		  (void *)ADRSPC_MEMD, memdsize,
		  REGION_CLASS_CBUS, REGION_FLAGS_DEFAULT);

    /*
     * Create various alias regions for MEMD
     */
    region_create(&memd_bitswap_region, "memd_bitswap",
		  (void *)ADRSPC_MEMD_BITSWAP, memdsize,
		  REGION_CLASS_CBUS, REGION_FLAGS_DEFAULT);
    region_create(&memd_writethru_region, "memd_writethru",
		  (void *)ADRSPC_MEMD_WRITETHRU, memdsize,
		  REGION_CLASS_CBUS, REGION_FLAGS_DEFAULT);
    region_create(&memd_uncached_region, "memd_uncached",
		  (void *)ADRSPC_MEMD_UNCACHED, memdsize,
		  REGION_CLASS_CBUS, REGION_FLAGS_DEFAULT);
    
    region_add_alias(&memd_region, &memd_bitswap_region);
    region_add_alias(&memd_region, &memd_writethru_region);
    region_add_alias(&memd_region, &memd_uncached_region);

    /*
     * Finally, create the regions for K0 and K1 segments
     */
    region_create(&k0_main_region, "main_k0",
		  (void *)R4K_K0BASE, memsize,
		  REGION_CLASS_LOCAL, REGION_FLAGS_DEFAULT);
    region_create(&k0_memd_region, "memd_k0",
		  (void *)(R4K_K0BASE + PHY_MEMD), memdsize,
		  REGION_CLASS_CBUS, REGION_FLAGS_DEFAULT);
    region_create(&k1_main_region, "main_k1",
		  (void *)R4K_K1BASE, memsize,
		  REGION_CLASS_LOCAL, REGION_FLAGS_DEFAULT);
    region_create(&k1_memd_region, "memd_k1",
		  (void *)(R4K_K1BASE + PHY_MEMD), memdsize,
		  REGION_CLASS_CBUS, REGION_FLAGS_DEFAULT);

    region_add_alias(&main_region, &k0_main_region);
    region_add_alias(&memd_region, &k0_memd_region);
    region_add_alias(&main_region, &k1_main_region);
    region_add_alias(&memd_region, &k1_memd_region);

    /*
     * Add alternate pools for those sticky moments
     */
    mempool_add_alternate_pool(&fast_mempool, &pmem_mempool);

    /*
     * Add aliases for mandatory mempools
     */
    mempool_add_alias_pool(MEMPOOL_CLASS_IOMEM, &pmem_mempool);
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
 * Returns a variety of strings unique to the RSP
 */

char *platform_get_string (platform_string_type type)
{
    char  *value;

    switch (type) {
      case PLATFORM_STRING_NOM_DU_JOUR:
	switch (rsp_flavor()) {
	  case RSP_RSP1:	value = "RSP1";		break;
	  case RSP_RSP2:	value = "RSP2";		break;
	  case RSP_RSP7000:	value = "RSP7000";	break;
	  default:		value = "RSP?";		break;
	}
	break;

      case PLATFORM_STRING_DEFAULT_HOSTNAME:
	if (rsp_is_master())
	    value = "Router";
	else
	    value = "Slave";
	break;

      case PLATFORM_STRING_PROCESSOR:
	value = "R4600";
	break;
	
      case PLATFORM_STRING_VENDOR:
	value = "cisco";
	break;

      case PLATFORM_STRING_LAST_RESET:
	switch (reset_reason) {
	  case 0:
	    value = "power-on";
	    break;
	  case REASON_PERIPH:
	    value = "s/w peripheral";
	    break;
	  case REASON_SWNMI:
	    value = "s/w nmi";
	    break;
	  case REASON_PBNMI:
	    value = "push-button";
	    break;
	  case REASON_WATCHDOG:
	    value = "watchdog";
	    break;
	  default:
	    value = "unexpected value";
	    break;
	}
	break;

      case PLATFORM_STRING_PROCESSOR_REVISION:
	value = proc_revision;
	break;
       
      case PLATFORM_STRING_PCMCIA_CONTROLLER:
	value = "CLPD6720";
	break;

      case PLATFORM_STRING_BOOTROM_OR_BOOTFLASH:
	value = "BOOTFLASH:";
	break;
	
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
 * Returns a variety of integer values unique to the 7000
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
	value = chassis_serial;
	break;

      case PLATFORM_VALUE_VENDOR:
	value = VENDOR_CISCO;
	break;

      case PLATFORM_VALUE_CPU_TYPE:
	value = mon_proctype();
	break;

      case PLATFORM_VALUE_FAMILY_TYPE:
	value = (cpu_type == CPU_RSP) ? FAMILY_RSP : FAMILY_UNKNOWN;
	break;

      case PLATFORM_VALUE_REFRESH_TIME:
	value = REFRESHTIME;
	break;

      case PLATFORM_VALUE_LOG_BUFFER_SIZE:
	if (system_loading)
	    value = 0;
	else
	    value = SIXTEEN_K;
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
	     nvsize/1024);
}


/*
 * platform_print_controllers
 *
 */

static void platform_print_controllers (void)
{
    if (ci_present())
	printf("Chassis Interface.\n");
}


/* platform_interface_init
 * 
 * Initialize platform interface specific features
 */

void platform_interface_init (void)
{
    /*
     * Initialize the bus support
     */
    rsp_bus_init();

    /*
     * Add platform registry services
     */
    reg_add_print_memory(platform_print_memory, "platform_print_memory");
    reg_add_print_controllers(platform_print_controllers,
			      "platform_print_controllers");
}

boolean (*platform_interface_ordering) (hwidbtype *new_idb, hwidbtype *old_idb)

     = idb_slotted_ordering;

/*
 * platform_line_init
 *
 * Initialize the console and other attached auxiliary lines
 *
 */

void platform_line_init (void)
{
    /*
     * Some default function registrations
     */
    reg_add_tty_xon(tty_xon_default, "tty_xon_default");

    nconlines = 1;		/* RSP always has one console port */
    nauxlines = 1;		/* RSP always has one AUX port */
    if (system_loading) {
	nvtylines = 0;
	maxvtylines = 0;
	defvtylines = 0;
    } else {
	/* 
	 * default number of VTYs 
	 */
	nvtylines = maxvtylines = defvtylines = DEFAULT_NVTYS;
    }

    /*
     * Assign base indexes into the MODEMS[] data structure.
     */
    AuxBase = freeLineBase;		/* tty driver has added any lines */
    freeLineBase += nauxlines;		/* update first free line */
    VTYBase = freeLineBase;		/* set the vty base */

    /* 
     * Increase the number of VTY lines
     */
    if (protocolconversion)
	maxvtylines = MAXLINES - VTYBase;

    ALLlines = nconlines + nttylines + nauxlines + nvtylines;

    /*
     * Now that we've set up our indices, complete the initialization
     * of the MODEMS[] data structure.
     */
    auxline_init();
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
}

/*
 * Analyze the config register and setup r4k_1ms_pipe_ticks
 * and r4k_4ms_pipe_ticks. Support divide by 2 (100Mhz) only for rsp.
 * In case, different CPU speed has to be supported in the future,
 * analyze the EC bits of the config reg.
 */
void platform_r4k_set_clock_params (ulong config_reg)
{
    /*
     * It is a 100mhz mipe (divide by 2), load the
     * appropriate values for pipe_speed and period in nanosecs.
     */
    r4k_1ms_pipe_ticks = R4K_1MS_AT_100MHZ;
    r4k_4ms_pipe_ticks = R4K_4MS_AT_100MHZ;
    r4k_pipe_speed = 100;
    r4k_count_period_ns = R4K_NS_PERIOD_AT_100MHZ;
}

/* end of file */
