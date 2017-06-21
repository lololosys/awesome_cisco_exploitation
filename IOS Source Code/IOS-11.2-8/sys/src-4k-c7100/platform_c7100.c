/* $Id: platform_c7100.c,v 3.2.58.21 1996/09/06 01:12:01 snyder Exp $
 * $Source: /release/112/cvs/Xsys/src-4k-c7100/platform_c7100.c,v $
 *------------------------------------------------------------------
 * platform_c7100.c - Platform specific support for Predator
 *
 * July 1995, Michael Beesley
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: platform_c7100.c,v $
 * Revision 3.2.58.21  1996/09/06  01:12:01  snyder
 * CSCdi68277:  make shr_core, shr_ukernel, sub_cxxxx sub_core_platform
 *              things const, save 712 out of data, 208 from image
 * Branch: California_branch
 *
 * Revision 3.2.58.20  1996/08/30  22:14:53  gshen
 * CSCdi67503:  Serial interface PA cause configuration violation on C7200
 * platform.
 * Branch: California_branch
 *
 * Revision 3.2.58.19  1996/08/28  13:11:45  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.2.58.18  1996/08/26  17:45:39  mbeesley
 * CSCdi66652:  IO DRAM partitioning needs to be tuned
 * Branch: California_branch
 *
 * Revision 3.2.58.17  1996/08/20  00:10:33  mbeesley
 * CSCdi66438:  show version should display amount of packet sram
 * Branch: California_branch
 *
 * Revision 3.2.58.16  1996/07/30  01:09:18  mbeesley
 * CSCdi55762:  Need to support fast IP multicasting on platforms
 * capable of scatter/gather IE c7200
 * Branch: California_branch
 *
 * Revision 3.2.58.15  1996/07/24  18:02:30  mbeesley
 * CSCdi63873:  Top two port adaptor slots are optional
 * Branch: California_branch
 *
 * Revision 3.2.58.14  1996/07/22  23:32:52  amcrae
 * CSCdi59922:  SNMP return 0 for serial# on Predator.
 * Branch: California_branch
 * When activating a PA, place the EEPROM serial number
 * in the slot structure before adding the card to SNMP.
 *
 * Revision 3.2.58.13  1996/06/19  04:44:13  smackie
 * Buffered logging support gets a hot wax and shine. (CSCdi53462)
 *
 *   o Restructure buffered logging to be a independant device
 *   o Add support for "clear logging"
 *   o Enable buffered logging by default on most platforms
 *   o Start hiding logger structures in private include files
 *
 * Branch: California_branch
 *
 * Revision 3.2.58.12  1996/05/31  23:44:36  mbeesley
 * CSCdi59046:  Implement config guidelines
 * Branch: California_branch
 *
 * Revision 3.2.58.11  1996/05/22  18:55:09  dcarroll
 * CSCdi57774:  Indefinite retries if init fails.
 * Branch: California_branch
 * Add support to limit init retries.
 *
 * Revision 3.2.58.10  1996/05/17  11:53:20  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.2.44.3  1996/05/05  23:52:17  ppearce
 * Sync to IbuMod_Calif_baseline_960504
 *
 * Revision 3.2.44.2  1996/04/26  07:57:23  dstine
 * Branch: IbuMod_Calif_branch
 * - commit of IBU modularity work, phase 1.
 *
 * Revision 3.2.44.1  1996/04/03  21:39:32  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.2.58.9  1996/05/07  23:22:02  gstovall
 * CSCdi56424:  OIR-insertion of 4T wedges the console
 * Branch: California_branch
 * Protect against newly inserted port adaptors (the 4T) asserting their
 * interrupt line before they are initialized.
 *
 * Revision 3.2.58.8  1996/05/01  13:56:19  dcarroll
 * CSCdi56037:  provide oir active check routine
 * Branch: California_branch
 * Need 'oir active check' routine.
 *
 * Revision 3.2.58.7  1996/05/01  13:28:29  dcarroll
 * CSCdi54895:  oir fix for ethernet port adapter
 * Branch: California_branch
 * OIR support for pas ethernets.
 *
 * Revision 3.2.58.6  1996/04/29  17:59:14  ssangiah
 * CSCdi54472: Inadequate information in the output of "sh controllers"
 *             command
 *   o Add a "show diag" command for the c7200 platform.
 *   o Make the output from "show controllers" consistent.
 * Branch: California_branch
 *
 * Revision 3.2.58.5  1996/04/23  19:21:56  mbeesley
 * CSCdi55360:  Enable cached, write thru view of packet memory
 * Branch: California_branch
 *
 * Revision 3.2.58.4  1996/04/06  02:05:49  ssangiah
 * CSCdi53822:  Need to deal with product name change.
 * Make changes to deal with the change in the product name for Predator
 * from c7100 to c7200.
 * Branch: California_branch
 *
 * Revision 3.2.58.3  1996/04/05  06:30:37  dcarroll
 * CSCdi53603:  avoid hogging cpu in analyze path
 * Branch: California_branch
 * Add platform dependant delay routines.
 *
 * Revision 3.2.58.2  1996/04/05  05:33:01  dcarroll
 * CSCdi53595:  allow for multiple passes through pas idb init code
 * Branch: California_branch
 * Break up the init idb routines and keep track of idb init state.
 *
 * Revision 3.2.58.1  1996/03/21  23:48:27  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.2.2.17  1996/03/18  23:33:16  mbeesley
 * Branch: ELC_branch
 * Sync to V111_1_3
 *
 * Revision 3.2.2.16  1996/03/08  21:36:30  mbeesley
 * CSCdi51090:  Need to support rev 2 CPU card
 * Branch: ELC_branch
 *
 * Revision 3.2.2.15  1996/02/20  01:27:46  mbeesley
 * CSCdi49347:  show hard should not print out revision
 * Branch: ELC_branch
 *
 * Revision 3.2.2.14  1996/02/19  23:24:21  mbeesley
 * CSCdi49335:  Predator needs support for DMA driven coalesce
 * Branch: ELC_branch
 *
 * Revision 3.2.2.13  1996/02/15  00:06:48  mbeesley
 * CSCdi49037:  IO dram mapping incorrect with more than 24 meg
 *         o Fix mapping of IO dram with more than 24 meg of dram
 *         o Disable cached view of IO dram
 *         o Issue MB0 reset at init time (helps PCMCIA master abort)
 * Branch: ELC_branch
 *
 * Revision 3.2.2.12  1996/02/13  01:43:24  mbeesley
 * CSCdi48778:  IO DRAM gets exhausted
 *         o Reduce AMDp2 and FDDI rx particle pool
 *         o Allocate more DRAM for IO on small images
 *         o Add cached view of packet DRAM
 *         o Use packet sram as a fast heap on rev 1 boxes
 * Branch: ELC_branch
 *
 * Revision 3.2.2.11  1996/02/10  00:18:25  mbeesley
 * CSCdi48581:  Add VIP2 support to ELC_branch
 * Branch: ELC_branch
 *
 * Revision 3.2.2.10  1996/02/08  09:26:55  gstovall
 * Branch: ELC_branch
 * Sync to ELC_baseline_960205.
 *
 * Revision 3.2.2.9  1996/01/23  02:18:18  mbeesley
 * Branch: ELC_branch
 * Add support for the CPU ID EEPROM. Postpone PCI SRAM initialization
 * until the rev of the CPU card has been read and verified.
 *
 * Revision 3.2.2.8  1996/01/16  02:16:57  dcarroll
 * Branch: ELC_branch
 * Sync ELC_branch between ELC_baseline_120995 and ELC_baseline_960110.
 *
 * Revision 3.2.2.7  1996/01/09  21:27:52  mbeesley
 * Branch: ELC_branch
 * Enhance/fix Midplane EEPROM utility. Protect against left over
 * interrupts from boot helper image. Fix some gt64010 register naming
 * mistakes.
 *
 * Revision 3.2.2.6  1995/12/13  14:32:20  dcarroll
 * Branch: ELC_branch
 * Move pas_add_card_entry. Fix some debug messages.
 *
 * Revision 3.2.2.5  1995/12/09  22:52:20  mbeesley
 * Branch: ELC_branch
 * Defeat OIR, ENV processes until hw is verified. Fix initial bugs:
 *         o NVRAM access must be 32 bit cycle, only using bits 7:0
 *         o Always create DRAM particle pool
 *         o Read 32 bytes, not 64 bytes from MP eeprom
 *         o Correct some misdefined hw addresses
 *         o Make packet SRAM usage on Rev 1 optional, with default as
 *           no until hw is verified (define C7100_CPU_SRAM to use it)
 *
 * Revision 3.2.2.4  1995/12/08  19:21:35  dcarroll
 * Branch: ELC_branch
 * OIR and ENVMON improvements.
 *
 * Revision 3.2.2.3  1995/12/05  20:00:10  mbeesley
 * Branch: ELC_branch
 * Correct some register initialization. Add support for dynamic
 * parallel bridge config. Do not use a small piece of SRAM as
 * fast memory on Rev 2 cards.
 *
 * Revision 3.2.2.2  1995/11/30  02:14:56  dcarroll
 * Branch: ELC_branch
 *
 * Revision 3.2.2.1  1995/11/22  21:43:39  mbeesley
 * Initial commit of c7100 support.
 * Branch: ELC_branch
 *
 * Revision 3.2  1995/11/17  18:41:53  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:23:25  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Include files
 */
#include "master.h"
#include "../../boot/cpu.h"
#include "interface_private.h"
#include "packet.h"
#include "registry.h"
#include "ttysrv.h"
#include "logger.h"
#include "sched.h"
#include "exception.h"
#include "sys_registry.h"
#include "config.h"
#include "stacks.h"

#include "../os/pool.h"
#include "../os/particle.h"
#include "../os/buffers.h"
#include "../os/buffers_inline.h"
#include "../os/gdb.h"
#include "../os/free.h"
#include "../os/region.h"

#include "../src-4k/util_4k.h"
#include "../hes/parser_defs_ucode.h"

#include "../dev/monitor1.h"
#include "../dev/flash_dvr_spec.h"

#include "../if/if_controller.h"

#include "../pas/pas_controller.h"
#include "../pas/if_pas.h"
#include "../pas/msg_pas.c"		/* Not a typo, see logger.h */
#include "../pas/pas_id.h"

#include "../hes/if_fci.h"
#include "../hes/ucode.h"
#include "../hes/slot.h"
#include "../hes/dgbus.h"

#include "c7100_macaddr.h"
#include "c7100_fpga.h"
#include "msg_c7100.c"			/* Not a typo, see logger.h */

#include "../filesys/clpd6729.h"

/*
 * Bandwidth / port adaptor limits
 */
#define C7100_HIGH_BANDWIDTH_THRESH	85	/* 8e is medium */
#define C7100_MED_BANDWIDTH_THRESH	35	/* m8t is low */
/*
 * NPE-150
 */
#define C7100_NPE150_MAX_FAST_PAS	    3	/* 3 fast PAs */
#define C7100_NPE150_MAX_MED_PAS	    5   /* 5 fast+med PAs */
#define C7100_NPE150_MAX_TOTAL_BANDWIDTH    800	/* MB0+MB1+MB2 */
/*
 * NPE-100
 */
#define C7100_NPE100_MAX_FAST_PAS	    2	/* 2 fast PAs */
#define C7100_NPE100_MAX_MED_PAS	    4   /* 4 fast+med PAs */

/*
 * Number of particle clones to grow
 */
#define C7100_PARTICLE_CLONES		1024	/* Should be lots */

/*
 * Bytes in a fastswitching rewrite particle
 */
#define C7100_FS_PARTICLE_BYTES		128

/*
 * How much SRAM that must be ramaining to allocate a pool for
 * an interface from SRAM. Presently, we'll set it at 256k.
 */
#define SRAM_LOW_THRESHOLD	     	(256*1024)

/*
 * Freelists for IO and PCI memory pools
 */
static const ulong freelist_block_sizes[] = {
    FREESIZE(0), FREESIZE(1), FREESIZE(2), FREESIZE(3), FREESIZE(4),
    FREESIZE(5), FREESIZE(6), FREESIZE(7), 65536
};

static const ulong nfreelist_block_sizes =
    sizeof(freelist_block_sizes)/sizeof(ulong);

/*
 * Memory pools
 */
static mempool pmem_mempool;
static mempool iomem_mempool;
static mempool pcimem_mempool;

/*
 * Memory regions
 */
static regiontype main_region;
static regiontype text_region;
static regiontype data_region;
static regiontype bss_region;
static regiontype pmem_region;
static regiontype iomem_region;
static regiontype pcimem_region;

static regiontype k0_main_region;
static regiontype k1_main_region;
static regiontype pcimem_cwt_region;
static regiontype iomem_cwt_region;

/*
 * Buffer pools
 */
static pooltype *ioparticle_pool;

/*
 * Miscellaneous local storage
 */
static int cpucard_revision;		/* Revison of CPU card */
static int cpucard_type;		/* Type of CPU card (NPE100/150) */
static int sramsize;			/* Amount of packet SRAM */
static uchar reset_reason;		/* Last hardware reset reason */
static char proc_revision[80];		/* Processor revision */
static tinybool poweron_config_err;	/* Config warning once on poweron */

boolean c7100_has_packet_sram;		/* Packet SRAM usable by PA's */

/*
 * c7100_iomem_size:
 * Return the required size of IO memory.
 */
static ulong c7100_iomem_size (ulong memsize)
{
    /*
     * On any box that has 16 Meg or less, allocate 4 meg for IO memory.
     */
    if (memsize <= (16 * ONE_MEG)) {
	return(4 * ONE_MEG);
    }

    /*
     * On any box with 24 to 48 Meg of DRAM, allocate 6 meg for IO memory.
     */
    if (memsize <= (48 * ONE_MEG)) {
	return(6 * ONE_MEG);
    }

    /*
     * On any box that has more then 48 meg, allocate 8 meg for IO memory
     */
    return(8 * ONE_MEG);
}

/*
 * platform_main:
 * Called from main() to allows platforms to initialize basic functions.
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
    kernel_set_sched_loop_hook(c7100_sched_loop_end);
}

/*
 * platform_exception_init:
 * Initialize basic interrupt handling for the platform
 */
void platform_exception_init (void)
{
    /*
     * Initialize the NMI handler
     */
    init_clocktick_handler();

    /*
     * Initialize Predator exception handlers
     */
    c7100_exception_init();
}

/*
 * platform_memory_init:
 * Define and declare all the memory regions and mempools for this platform
 */
void platform_memory_init (void)
{
    ulong memsize, iomemsize;

    /*
     * Find out how much memory the rom monitor thinks we have
     */
    memsize = mon_getmemsize();

    /*
     * Most of this main DRAM will be dedicated to code, data
     * and the global heap. But part of it will be used for IO memory.
     * It has already been mapped once into the virtual address
     * space. We need to map the IO memory part once more to get the
     * IO memory view.
     */
    iomemsize = c7100_iomem_size(memsize);
    memsize -= iomemsize;

    /*
     * Map the IO DRAM memory into the address space.
     */
    if (!c7100_map_iomemory(memsize, iomemsize)) {
	crashdump(0);
    }

    /*
     * Add a region to describe all of main DRAM minus IO memory
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
     * Declare a region from the end of bss to the end of local DRAM
     * and create a "local" mempool based on it.
     */
    region_create(&pmem_region, "heap",
		  bss_region.end,
		  ((ulong)main_region.end - (ulong)bss_region.end),
		  REGION_CLASS_LOCAL, REGION_FLAGS_DEFAULT);
    mempool_create(&pmem_mempool, "Processor", &pmem_region, 0,
		   NULL, 0, MEMPOOL_CLASS_LOCAL);

    /*
     * Create an IO memory pool in DRAM
     */
    region_create(&iomem_region, "iomem",
		  (void *)memsize, iomemsize,
		  REGION_CLASS_IOMEM, REGION_FLAGS_DEFAULT);
    mempool_create(&iomem_mempool, "I/O", &iomem_region,
		   C7100_DCACHE_LINE_SIZE,
		   freelist_block_sizes, nfreelist_block_sizes,
		   MEMPOOL_CLASS_IOMEM);

    /*
     * Add alias for fast memory pool
     */
    mempool_add_alias_pool(MEMPOOL_CLASS_FAST, &pmem_mempool);

    /*
     * Add regions to describe cached write-thru view of packet DRAM
     */
    region_create(&iomem_cwt_region, "iomem_cwt",
		  (void *)(ADRSPC_CACHED_IODRAM + memsize), iomemsize,
		  REGION_CLASS_IOMEM, REGION_FLAGS_DEFAULT);
    region_add_alias(&iomem_region, &iomem_cwt_region);

    /*
     * Finally, create the regions for K0 and K1 segments
     */
    region_create(&k0_main_region, "main_k0",
		  (void *)R4K_K0BASE, memsize,
		  REGION_CLASS_LOCAL, REGION_FLAGS_DEFAULT);
    region_create(&k1_main_region, "main_k1",
		  (void *)R4K_K1BASE, memsize,
		  REGION_CLASS_LOCAL, REGION_FLAGS_DEFAULT);

    region_add_alias(&main_region, &k0_main_region);
    region_add_alias(&main_region, &k1_main_region);

    /*
     * Add aliases for mandatory mempools
     */
    mempool_add_alias_pool(MEMPOOL_CLASS_ISTACK, &pmem_mempool);
    mempool_add_alias_pool(MEMPOOL_CLASS_PSTACK, &pmem_mempool);
}

/*
 * platform_buffer_init:
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

    if (!particle_clones_init(C7100_PARTICLE_CLONES)) {
	crashdump(0);
    }
}

/*
 * platform_get_string:
 * Returns a variety of strings unique to the c7100
 */
char *platform_get_string (platform_string_type type)
{
    char  *value;

    switch (type) {

    case PLATFORM_STRING_NOM_DU_JOUR:
	if (c7100_chassis_model == C7100_FOUR_SLOT) {
	    value = "7204";
	} else if (c7100_chassis_model == C7100_SIX_SLOT) {
	    value = "7206";
	} else {
	    value = "7200";
	}
	break;

    case PLATFORM_STRING_DEFAULT_HOSTNAME:
	value = "Router";
	break;

    case PLATFORM_STRING_PROCESSOR:
	if (cpucard_type == C7100_CPU_NPE100) {
	    value = "NPE100";
	} else {
	    value = "NPE150";
	}
	break;

    case PLATFORM_STRING_PROCESSOR_REVISION:
	value = proc_revision;
	break;

    case PLATFORM_STRING_VENDOR:
	value = "cisco";
	break;

    case PLATFORM_STRING_LAST_RESET:

	switch (reset_reason) {

	case 0:
	    value = "power-on"; break;
	case REASON_PERIPH: 
	    value = "s/w peripheral"; break;
	case REASON_SWNMI:
	    value = "s/w nmi"; break;
	case REASON_PBNMI:
	    value = "push-button"; break;
	case REASON_WATCHDOG:
	    value = "watchdog"; break;
	default:
	    value = "unexpected value"; break;
	}
	break;

    case PLATFORM_STRING_BOOTROM_OR_BOOTFLASH:
        value = "BOOTFLASH:";
        break;

    case PLATFORM_STRING_PCMCIA_CONTROLLER:
	value = "CLPD6729";
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
 * platform_get_value:
 * Returns a variety of integer values unique to the 7100
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
	value = c7100_chassis_serial;
	break;

    case PLATFORM_VALUE_VENDOR:
	value = VENDOR_CISCO;
	break;

    case PLATFORM_VALUE_CPU_TYPE:
	value = CPU_C7100;
	break;

    case PLATFORM_VALUE_FAMILY_TYPE:
	value = FAMILY_C7100;
	break;

    case PLATFORM_VALUE_REFRESH_TIME:
	value = REFRESHTIME;
	break;

    case PLATFORM_VALUE_LOG_BUFFER_SIZE:
	if (system_loading)
	    value = 0;
	else
	    value = EIGHT_K;
	break;

    default:
	value = 0;
	break;
    }
    return(value);
}

/*
 * platform_print_memory:
 * Print out NVRAM memory
 */
static void platform_print_memory (void)
{
    if (nvsize) {
	printf("%dK bytes of non-volatile configuration memory.\n",
               nvsize/1024);
    }
    if (sramsize) {
	printf("%dK bytes of packet SRAM memory.\n", sramsize/1024);
    }
}

/*
 * platform_print_controllers:
 * Print any controllers, or other pieces of hardware
 */
static void platform_print_controllers (void)
{
    /*
     * May want to print something out about midplane cookie
     * and/or envm monitoring module
     */
}

/*
 * c7100_registry_init:
 * Init any c7100 registries
 */
static void c7100_registry_init (void)
{
    /*
     * Setup any hardware registers
     */
    c7100_setup_hardware_registers();

    /*
     * Add in subsys init class registry
     */
    reg_add_subsys_init_class(pas_subsys_init_class, "pas_subsys_init_class");
}

/*
 * clpd6729_pcmcia_supported:
 * Return TRUE if the PCMCIA flash cards are supported on this version
 * of the Predator hardware
 */
boolean clpd6729_pcmcia_supported (void)
{
    /*
     * The rev1 NPE-150 Predator CPU cards are incapable
     * of supporting the PCMCIA devices
     */
    if ( (cpucard_type == C7100_CPU_NPE150) &&
         (cpucard_revision <= C7100_VERSION_REV1) ) {
	return(FALSE);				/* not supported */
    }
    return(TRUE);				/* supported */
}

/* 
 * platform_interface_init :
 * Initialize platform interface specific features
 */
void platform_interface_init (void)
{
    volatile ulong 	*ptr;
    struct r4k_tlb_t 	tlb_e;
    mempool 		*sram_mempool = &iomem_mempool;
    int			temp;

    /*
     * Find out if we are on a rev 2 CPU card or not
     */
    cpucard_revision = c7100_cpucard_version(); 
    cpucard_type = c7100_cpucard_type();
    if (cpucard_revision == C7100_VERSION_UNKNOWN) {
	errmsg(&msgsym(NOCPUVER, C7200));
	cpucard_revision = C7100_VERSION_REV1;
    }
    if ((cpucard_type == C7100_CPU_NPE150) && 
	(cpucard_revision > C7100_VERSION_REV1) &&
	c7100_pci_enable_packet_sram()) {

	c7100_has_packet_sram = TRUE;
	sramsize = ONE_MEG;

	/*
	 * Zero out the packet SRAM
	 */
	ptr = (volatile ulong *)ADRSPC_PCISRAM;
	for (temp=0; temp < sramsize; temp += 4) {
	    *ptr++ = 0;
	}

    } else {

	/*
	 * Either a rev1 NPE150 or an NPE100
	 */
	c7100_has_packet_sram = FALSE;

	/*
	 * Nuke the TLB entries setup by the c7100_vspace table (these
	 * are the first two entries in the tlb)
	 */
	for (temp=0; temp<2; temp++) {
	    r4k_gettlb(&tlb_e, temp);
	    tlb_e.lo0.v = tlb_e.lo1.v = 0; /* mark invalid */
	    r4k_settlb(&tlb_e, temp, FALSE /* don't clear cache */);
	}
    }

    /*
     * If this rev of the CPU supports packet SRAM, setup the regions
     */
    if (c7100_has_packet_sram) {

	region_create(&pcimem_region, "pcimem",
		      (void *)(ADRSPC_PCISRAM), sramsize,
		      REGION_CLASS_PCIMEM, REGION_FLAGS_DEFAULT);
	mempool_create(&pcimem_mempool, "PCI", &pcimem_region,
		       C7100_DCACHE_LINE_SIZE,
		       freelist_block_sizes, nfreelist_block_sizes,
		       MEMPOOL_CLASS_PCIMEM);
	sram_mempool = &pcimem_mempool;

	/*
	 * Add region for cached, write thru view of packet SRAM
	 */
	region_create(&pcimem_cwt_region, "pcimem_cwt",
		      (void *)(ADRSPC_PCISRAM+PCIMEM_CWT_MASK), sramsize,
		      REGION_CLASS_PCIMEM, REGION_FLAGS_DEFAULT);
	region_add_alias(&pcimem_region, &pcimem_cwt_region);

	/*
	 * Setup the DRAM based IO pool as an alternate for
	 * the PCI SRAM pool if it runs out
	 */
	mempool_add_alternate_pool(&pcimem_mempool, &iomem_mempool);

    } else {
	/*
	 * Add an alias for PCI memory
	 */
	mempool_add_alias_pool(MEMPOOL_CLASS_PCIMEM, &iomem_mempool);
    }
    
    /*
     * Init the fastswitching particle pool
     */
    fs_particle_pool = particle_pool_create("F/S",
					    POOL_GROUP_PUBLIC,
					    C7100_FS_PARTICLE_BYTES,
					    POOL_SANITY,
					    C7100_DCACHE_LINE_SIZE,
					    sram_mempool);
    if (fs_particle_pool) {
	pool_adjust(fs_particle_pool, 0, 512, 512, TRUE);
	particle_pool_create_cache(fs_particle_pool, 512);
	pool_adjust_cache(fs_particle_pool, 512);
    }

    /*
     * Allocate a global pool of particles in DRAM.
     */
    ioparticle_pool = particle_pool_create("Normal", 
					   POOL_GROUP_PUBLIC,
					   C7100_PARTICLE_BYTES, 
					   POOL_DEFAULT_FLAGS,
					   C7100_DCACHE_LINE_SIZE,
					   &iomem_mempool);
    if (ioparticle_pool) {
	pool_adjust(ioparticle_pool, 128, 1024, 512, TRUE);
    }

    /*
     * Init the pool of static paktypes for fastswitching
     */
    pas_init_fspak_pool();

    /*
     * Init the global header pool
     */
    headerpool = pak_pool_create("Header", POOL_GROUP_PUBLIC, 0,
				 POOL_DEFAULT_FLAGS,
				 NULL);
    if (headerpool) {
	pool_adjust(headerpool, 256, 1024, 256, TRUE);
	pak_pool_create_cache(headerpool, 256);
	pool_adjust_cache(headerpool, 256);
    }

    /*
     * Setup the ioparticle_pool as the fallback for fs_particle_pool
     */
    if (fs_particle_pool) {
	fs_particle_pool->fallback = ioparticle_pool;
    }

    /*
     * Initialize the c7100 subsystems
     */
    pas_init_jumptable();		/* initialize Net IO jumptable 	*/
    pas_init_network_level();		/* initialize network intr level*/
    pas_init_pa_nmi_handler();		/* initialize PA Hi IRQ stack */
    c7100_registry_init();		/* setup c7100 specific registry */

    /*
     * Add a callback for interface final initialization
     */
    reg_add_if_final_init(pas_if_final_init, "pas_if_final_init");

    /*
     * Add platform registry services
     */
    reg_add_print_memory(platform_print_memory, "platform_print_memory");
    reg_add_print_controllers(platform_print_controllers,
			      "platform_print_controllers");

    /*
     * Initialize the platform MAC addresses
     */
    c7100_macaddr_init();
}

boolean (*platform_interface_ordering) (hwidbtype *new_idb, hwidbtype *old_idb)

     = idb_slotted_ordering;

/*
 * platform_line_init:
 * Initialize the console and other attached auxiliary lines
 */
void platform_line_init (void)
{
    /*
     * Some default function registrations
     */
    reg_add_tty_xon(tty_xon_default, "tty_xon_default");

    nconlines = 1;		/* c7100 always has one console port */
    nauxlines = 1;		/* c7100 always has one AUX port */

    maxvtylines = defvtylines = nvtylines = DEFAULT_NVTYS;

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

/*
 * platform_r4k_set_clock_params:
 * Analyze the config register and setup r4k_1ms_pipe_ticks
 * and r4k_4ms_pipe_ticks. Support divide by 3 (150Mhz) only on Predator.
 * In case, different CPU speed has to be supported in the future,
 * analyze the EC bits of the config reg.
 */
void platform_r4k_set_clock_params (ulong config_reg)
{
    /*
     * It is a 150mhz pipe (divide by 3), load the
     * appropriate values for pipe_speed and period in nanosecs.
     */
    r4k_1ms_pipe_ticks = R4K_1MS_AT_150MHZ;
    r4k_4ms_pipe_ticks = R4K_4MS_AT_150MHZ;
    r4k_pipe_speed = 150;
    r4k_count_period_ns = R4K_NS_PERIOD_AT_150MHZ;
}

/*
 * pci_memaddr_convert:
 * Convert a memory address to the correct cached view *FIXME*
 */
void *pci_memaddr_convert (void *addr, int attr)
{
    return(addr);
}

/*
 * pas_interface_fallback_pool:
 * Return a pointer to an interface's fallback pool. Either IO or PCI
 * particle pool.
 */
pooltype *pas_interface_fallback_pool (hwidbtype *idb)
{
    return(ioparticle_pool);
}

/*
 * pas_interface_header_pool:
 * Return a pointer to an interface's header pool
 */
pooltype *pas_interface_header_pool (hwidbtype *idb)
{
    return(headerpool);
}

/*
 * pas_buffer_mempool:
 * Return a pointer to the memory pool that an interface should use
 * for its buffer pool
 */
mempool *pas_buffer_mempool (hwidbtype *idb)
{
    ulong bytes_left;

    /*
     * If we have packet SRAM and there is enough left, try
     * and allocate the pool in SRAM
     */
    bytes_left = mempool_get_free_bytes(MEMPOOL_CLASS_PCIMEM, FALSE);
    if (c7100_has_packet_sram &&
        (bytes_left > SRAM_LOW_THRESHOLD)) {

	/*
	 * If its a 1fe or an fddi interface, try and allocate some
	 * SRAM for the private buffer pool.
	 */
	if ((idb->type == IDBTYPE_FEIP) ||
	    (idb->type == IDBTYPE_FDDIT)) {
	    return(&pcimem_mempool);
	}
    }

    /*
     * Default to DRAM
     */
    return(&iomem_mempool);
}

/*
 * pas_platform_if_final_init: 
 * Gets called off of the if_final_init registry.
 */
void pas_platform_if_final_init (void)
{
    hwidbtype *idb;

    FOR_ALL_HWIDBS(idb) {
	idb->oir_flags &= ~IDB_FINAL_INIT;
    }
}

/*
 * pas_slots_init
 * Reset slots flags
 * C7100 only.
 */
void pas_slots_init (void)
{
    int pa_bay;

    /*
     * Initialize the slots data structure
     */
    for (pa_bay = 0; pa_bay < MAX_SLOTS; pa_bay++) {
	slots[pa_bay].interface_count = 0;
	slots[pa_bay].slot_num = pa_bay;
	slots[pa_bay].flags |= BOARD_NONEXIST;
	slots[pa_bay].flags &= ~BOARD_ANALYZED;
        TIMER_STOP(slots[pa_bay].insert_time);
	slots[pa_bay].retry_ok = FALSE;
	slots[pa_bay].retry_count = 0;
    }
}

/*
 * pas_get_pa_type
 * Return pa controller type.
 */
ushort pas_get_pa_type (uint pa_bay)
{
    ushort type;

    type = get_pa_type(pa_bay);
    slots[pa_bay].pa_type = type;
    slots[pa_bay].hw_version = get_pa_version(pa_bay);
    return(type);
}

/*
 * pas_pa_exists
 * Update slots that pa exists and enable OIR interrupts
 * for this slot.
 */
void pas_pa_exists (uint pa_bay)
{
    /*
     * Update slots flags to indicate a PA is present.
     * Enable oir interrupts for this PA if it is not on
     * the IO card (slot 0).
     */
    slots[pa_bay].flags |= BOARD_DISABLED;
    slots[pa_bay].flags &= ~BOARD_NONEXIST;
    slots[pa_bay].retry_ok = TRUE;
}

/*
 * pas_pa_analyzed
 * Determine if this PA is already analyzed.
 * C7100 only
 */
boolean pas_pa_analyzed (uint pa_bay)
{
    return(slots[pa_bay].flags & BOARD_ANALYZED);
}

/*
 * pas_activate_bay :
 * Update the slots structure
 */
void pas_activate_bay (uint pa_bay)
{
    dgbus_eeprom_t eeprom;

    slots[pa_bay].flags |= BOARD_ANALYZED;
    slots[pa_bay].flags &= ~BOARD_DISABLED;
    if (!TIMER_RUNNING(slots[pa_bay].insert_time))
        GET_TIMESTAMP(slots[pa_bay].insert_time);
    pas_get_eeprom_data(pa_bay, &eeprom, 16);
    slots[pa_bay].serial_number = eeprom.serial_number_31_24 << 24 |
                    eeprom.serial_number_23_16 << 16 |
                    eeprom.serial_number_15_8 << 8   |
                    eeprom.serial_number_7_0;
    pas_add_card_entry(pa_bay);
}

/*
 * pas_set_fci_type
 * Set the idb fci type.
 */
void pas_set_fci_type (hwidbtype *idb, uint slot)
{
    idb->fci_type = slots[slot].pa_type;
}

/*
 * pas_install_interrupt_status_reg:
 * Stub routine. Not used on Predator.
 */
void pas_install_status_reg (hwidbtype *idb, void *addr, 
			     boolean memory_mapped_io)
{
}

/*
 * pas_delay:
 * platform dependant delay routine.
 */
void pas_delay (uint time)
{
    uint pa_bay;

    if (pas_oir_active()) {
	/*
	 * Before resetting the interrupt level, mask the
	 * individual interrupts in the external hardware for all
	 * PA bays that have not been analyzed. This is done so that the PA's
	 * that can accidentally assert their interrupt signals while in the
	 * reset state will not cause the system to run a yet-to-be installed
	 * interrupt handler, thus locking up the box. Some versions of
	 * the 4T PA do require this.
	 */
	for (pa_bay=0; pa_bay<MAX_PA_BAYS; pa_bay++) {
	    /*
	     * If this slot has been analyzed, continue
	     */
	    if (slots[pa_bay].flags & BOARD_ANALYZED) {
		continue;
	    }
	    
	    /*
	     * Turn off netio and management interrupts.
	     */
	    pas_netio_interrupts(pa_bay, FALSE);
	    pas_mngmt_interrupts(pa_bay, FALSE);
	}
	
	/*
	 * Lower the CPU interrupt mask and dismiss the process
	 */
	reset_interrupt_level(oir_level);
        process_sleep_for(time);

	/*
	 * Raise the CPU interrupt mask to where it was before the dismiss.
	 * Note, the individual PA interrupt mask registers do not need
	 * to be set back to their previous state as the remainder of
	 * the PA init sequence for non-analyzed PA's will re-enable all
	 * appropriate interrupts for that PA.
	 */
	oir_level = raise_interrupt_level(NETS_DISABLE);

    } else {
        DELAY(time);
    }
}

/*
 * oir_active:
 * Test if OIR is active
 */ 
boolean pas_oir_active (void) 
{
    if (oir_init_mode) {
	return (TRUE);
    } else {
	return (FALSE);
    }
}

/*
 * c7100_verify_sysconfig:
 * Verify that the system configuration is sane and supported.
 * Return FALSE if this port adaptor's bay is not to be initialized
 */
boolean c7100_verify_sysconfig (uint pa_bay)
{
    ulong points, tot_points;
    int i, hs, ms;
    boolean exc_cnfg_guide;
    ushort io_card_type;

    if ((pa_bay == 0) || (pa_bay >= MAX_PA_BAYS)) {
	return(FALSE);
    }

    /*
     * Scan thru all the bays, adding up the accumulated points so far.
     */
    hs = ms = tot_points = 0;
    for (i=1; i<MAX_PA_BAYS; i++) {
	if (!pas_pa_present(i)) {
	    continue;
	}
	points = pas_bandwidth_points(get_pa_type(i));
	if (points >= C7100_HIGH_BANDWIDTH_THRESH) {
	    hs += 1;
	} else if (points >= C7100_MED_BANDWIDTH_THRESH) {
	    ms += 1;
	}
	tot_points += points;
    }

    /*
     * Account for the possibility of an IO Card with a fast ether.
     */
    io_card_type = get_pa_type(0);

    if (io_card_type == PA_HARDWARE_C7100_IO1FE) {
        tot_points += pas_bandwidth_points(PA_HARDWARE_1FE);
        hs += 1;
    }

    /*
     * Check to see if we are exceeding the configuration guidelines
     */
    switch (cpucard_type) {

        case C7100_CPU_NPE150:
            exc_cnfg_guide = (hs > C7100_NPE150_MAX_FAST_PAS) ||
                             ((hs + ms) > C7100_NPE150_MAX_MED_PAS) ||
                             (tot_points >= C7100_NPE150_MAX_TOTAL_BANDWIDTH);
            break;

        case C7100_CPU_NPE100:
            exc_cnfg_guide = (hs > C7100_NPE100_MAX_FAST_PAS) ||
                             ((hs + ms) > C7100_NPE100_MAX_MED_PAS);
            break;

        default:
            /*
             * Can't verify the configuration with an unknown CPU card.
             */
            exc_cnfg_guide = FALSE;

    }   /* switch (cpucard_type) */

    if (exc_cnfg_guide) {
	if (pas_oir_active() || !poweron_config_err) {
	    errmsg(&msgsym(CONFIG, C7200));
	}
	poweron_config_err = TRUE;
    }

    return(TRUE);
}

/* end of file */
