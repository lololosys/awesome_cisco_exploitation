/* $Id: platform_sierra.c,v 3.8.10.5 1996/09/06 01:11:58 snyder Exp $
 * $Source: /release/112/cvs/Xsys/src-4k-c4000/platform_sierra.c,v $
 *------------------------------------------------------------------
 * platform_sierra.c - Platform specific support for the R4600 C4500
 *
 * February 1995, Scott Mackie (moved from init_sierra.c)
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: platform_sierra.c,v $
 * Revision 3.8.10.5  1996/09/06  01:11:58  snyder
 * CSCdi68277:  make shr_core, shr_ukernel, sub_cxxxx sub_core_platform
 *              things const, save 712 out of data, 208 from image
 * Branch: California_branch
 *
 * Revision 3.8.10.4  1996/06/19  04:44:10  smackie
 * Buffered logging support gets a hot wax and shine. (CSCdi53462)
 *
 *   o Restructure buffered logging to be a independant device
 *   o Add support for "clear logging"
 *   o Enable buffered logging by default on most platforms
 *   o Start hiding logger structures in private include files
 *
 * Branch: California_branch
 *
 * Revision 3.8.10.3  1996/05/17  11:53:14  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.8.2.2  1996/04/26  07:57:20  dstine
 * Branch: IbuMod_Calif_branch
 * - commit of IBU modularity work, phase 1.
 *
 * Revision 3.8.2.1  1996/04/03  21:37:37  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.8.10.2  1996/03/21  23:47:09  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.8.10.1  1996/03/18  21:58:12  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.2.4  1996/03/18  23:33:03  mbeesley
 * Branch: ELC_branch
 * Sync to V111_1_3
 *
 * Revision 3.2.2.3  1996/02/16  02:23:48  mbeesley
 * CSCdi48910:  Add storage of fault history for crashes
 * Branch: ELC_branch
 *
 * Revision 3.2.2.2  1996/01/16  02:16:27  dcarroll
 * Branch: ELC_branch
 * Sync ELC_branch between ELC_baseline_120995 and ELC_baseline_960110.
 *
 * Revision 3.2.2.1  1995/12/11  19:16:39  mbeesley
 * Branch: ELC_branch
 * Sync ELC_branch to ELC_baseline_120995.
 *
 * Revision 3.8  1996/02/23  06:24:37  vdukki
 * CSCdi41895:  Hardware version number is displayed in hexdecimal format
 * Updated the code to display hardware version number for sierra, as
 * an alphabet if the version number is between 1-26 and as a hexadecimal
 * number otherwise.
 *
 * Revision 3.7  1996/02/18  23:00:53  vdukki
 * CSCdi47769:  need to distinguish 4700m model using snmp
 * Modified the code to recognize the 4700m platform using snmp.
 *
 * Revision 3.6  1996/02/18  22:04:08  vdukki
 * CSCdi41637:  need to distinguish between 4700 models via snmp
 * Modified code to return new values for cardType and chassisType mib
 * attributes for 4700s platform.
 *
 * Revision 3.5  1996/02/13  20:39:15  hampton
 * Move buffer initialization to the platform_xxx.c files to eliminate
 * platform dependencies from common code.  [CSCdi48844]
 *
 * Revision 3.4  1996/01/05  04:56:41  hampton
 * Remove extraneous includes of h/timer.h.  [CSCdi46482]
 *
 * Revision 3.3  1995/12/03  18:00:13  thille
 * CSCdi44905:  ciscopro targets not in 11.1
 * Put them in.
 *
 * Revision 3.2  1995/11/17  18:40:52  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:22:37  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.7  1995/10/15  18:54:54  eschaffe
 * CSCdi40294:  Increase the number of Protocol Translation sessions
 *
 * Revision 2.6  1995/10/13  00:54:23  smackie
 * Add missing vendor names. (CSCdi42122)
 *
 * Revision 2.5  1995/10/04  22:59:44  hampton
 * The platform_get_value() routine should return CPU and FAMILY values
 * for the hardware it is currently executing upon, not some compiled in
 * value.  This is required to validate that the correct software image
 * has been booted onto a hardware platform.  [CSCdi41098]
 *
 * Revision 2.4  1995/07/25  17:45:10  mleelani
 * CSCdi37536:  Fix the C4700 Millisecond Ticks.
 * Add support for the 133.33 mhz pipe.
 *
 * Revision 2.3  1995/06/28  23:14:41  tkolar
 * CSCdi35465:  I/O memory needs to be included when dumping core
 * Add flag for regions to determine if they are dumped or not.
 *
 * Revision 2.2  1995/06/15  06:39:56  smackie
 * Tidy up R4600/R4700 revision string handler. (CSCdi35883)
 *
 * Revision 2.1  1995/06/07  22:53:13  hampton
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
#include "packet.h"
#include "exception.h"
#include "sys_registry.h"
#include "config.h"
#include "stacks.h"
#include "interface.h"
#include "../src-4k/util_4k.h"
#include "../os/gdb.h"
#include "../os/buffers.h"
#include "../os/free.h"
#include "../os/pool.h"
#include "../os/region.h"
#include "../les/init_les.h"
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

static regiontype k0_main_region;
static regiontype k0_iomem_region;
static regiontype k1_main_region;
static regiontype k1_iomem_region;

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

static char proc_revision[80];
static char hw_revision[20];
static char hw_serial[20];

/*
 * Local miscellaneous variables
 */
cookie_t        *cookie;        /* Address of cookie */
static cookie_t  cookie_buffer;	/* cookie storage */

/*
 * Forward references
 */
static void shmem_parity_error (r4k_context_t *ptr);


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
     * Set the address of the cookie PROM
     */
    mon_get_cookie_copy((void *)&cookie_buffer);
    cookie = &cookie_buffer;

    /*
     * Build the platform strings
     */

    /*
     * If the hardware version number is within the range 1 to 26,
     * convert it to an alphabetic version number character (rev 1 becomes
     * rev A, rev 2 becomes rev B, etc). Otherwise, simply print the rev
     * number as a 2 digit hex number.
     */
    hw_version = mon_get_rev();
    if (hw_version && (hw_version <= ('Z' - 'A')))
	sprintf(hw_revision, "%c", (hw_version - 1 + 'A'));
    else
	sprintf(hw_revision, "0x%02x", hw_version);

    serial = GETLONG(cookie->serial);
    sprintf(hw_serial, "%08x", serial);

    if ((cookie->magic[0] == COOKIE_MAGIC_0) &&
        (cookie->magic[1] == COOKIE_MAGIC_1) &&
        (cookie->capabilities[1] & CISCOPRO_PLATFORM))
            ciscopro_platform = TRUE;

    r4k_get_revision_string(proc_revision);

    /*
     * Turn the run light on
     */
    IO_ASIC->sys_ctl &= (~CONTROL_RUNLAMP);
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
    createlevel(SHMEM_PARLEVEL,
		(void (*)(void))shmem_parity_error, 
		"Parity interrupt");
}    

/*
 * platform_memory_init
 *
 * Define and declare all the memory regions and mempools for this platform
 */

void platform_memory_init (void)
{
    ulong memsize, iosize;

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
     * Declare a region from the end of BSS to the end of main dram
     * and create a "local" mempool based on it.
     */
    region_create(&pmem_region, "mainheap",
		  _END, (RAMBASE + memsize - (ulong)_END), 
		  REGION_CLASS_LOCAL, REGION_FLAGS_DEFAULT);

    mempool_create(&pmem_mempool, "Processor", &pmem_region, 0,
		   NULL, 0, MEMPOOL_CLASS_LOCAL);

    /*
     * Find out how much shared memory is installed
     */
    iosize  = mon_get_iosize();

    /*
     * Declare a region and mempool for shared memory
     */
    region_create(&iomem_region, "iomem",
		  (void *)ADRSPC_SHAREDMEM, iosize,
		  REGION_CLASS_IOMEM,
		  REGION_FLAGS_DEFAULT | REGION_FLAGS_EXCEPTION_DUMP);
    mempool_create(&iomem_mempool, "I/O", &iomem_region, 0,
		   freelist_iomem_block_sizes, nfreelist_iomem_block_sizes,
		   MEMPOOL_CLASS_IOMEM);

    /*
     * Finally, create the regions for K0 and K1 segments
     */
    region_create(&k0_main_region, "main_k0",
		  (void *)R4K_K0BASE, memsize,
		  REGION_CLASS_LOCAL, REGION_FLAGS_DEFAULT);
    region_create(&k0_iomem_region, "iomem_k0",
		  (void *)(R4K_K0BASE + PHY_ADRSPC_SHAREDMEM),
		  iosize, REGION_CLASS_IOMEM, REGION_FLAGS_DEFAULT);
    region_create(&k1_main_region, "main_k1",
		  (void *)R4K_K1BASE, memsize,
		  REGION_CLASS_LOCAL, REGION_FLAGS_DEFAULT);
    region_create(&k1_iomem_region, "iomem_k1",
		  (void *)(R4K_K1BASE + PHY_ADRSPC_SHAREDMEM),
		  iosize, REGION_CLASS_IOMEM, REGION_FLAGS_DEFAULT);

    region_add_alias(&main_region,  &k0_main_region);
    region_add_alias(&iomem_region, &k0_iomem_region);
    region_add_alias(&main_region,  &k1_main_region);
    region_add_alias(&iomem_region, &k1_iomem_region);

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
	pool_adjust(big, BIG_MIN, C4500_BIG_MAX1, BIG_INIT1, TRUE);
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
 * Returns a variety of strings unique to the 4500
 */

char *platform_get_string (platform_string_type type)
{
    char  *value;
    uchar  vendor = cookie->vendor;

    switch (type) {
      case PLATFORM_STRING_NOM_DU_JOUR:
	if (platform_is_c4500()) {
            if (ciscopro_platform)
                    value = "CPA4500";
            else
                    value = "4500";
	    break;
	}
	if (platform_is_c4700()) {
	    value = "4700";
	    break;
	}
	value = NULL;
	break;

      case PLATFORM_STRING_DEFAULT_HOSTNAME:
	value = "Router";
	break;

      case PLATFORM_STRING_PROCESSOR:
	value = "R4K";
	break;

      case PLATFORM_STRING_PROCESSOR_REVISION:
	value = proc_revision;
	break;

      case PLATFORM_STRING_VENDOR:
	switch (vendor) {
	  case VENDOR_CISCO:
	    value = "cisco";
	    break;
	  case VENDOR_CABLETRON:
	    value = "Cabletron";
	    break;
	  case VENDOR_CHIPCOM:
	    value = "Chipcom";
	    break;
	  case VENDOR_DSC:
	    value = "DSC";
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

      case PLATFORM_STRING_BOOTROM_OR_BOOTFLASH:
        value = "BOOTFLASH:";
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
 * Returns a variety of integer values unique to the 4500
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
	value = (cpu_type == CPU_SIERRA) ? FAMILY_SIERRA : FAMILY_UNKNOWN;
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
 * platform_print_features
 *
 */

static void platform_print_features (void)
{
    /*
     * Print out SW supported ie. enterprise, desktop, etc.
     * Print out cookie value.
     */
    if (ciscopro_platform) {
        printf("Authorized for CiscoPro software set only. \n");
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
 * Initialize platform interface specific features
 */

void platform_interface_init (void)
{
    int               i, j;
    lev2_jumptable_t *ptr;

    /*
     * Reset the vector table to a known state
     */
    for (i = 0; i < C4000_NUM_SLOTS; i++) {
	ptr = &lev2_jumptable[i];
	for (j = 0; j<MAX_IDB_PER_NIM; j++) {
	    ptr->idb[j] = (hwidbtype *)BAD_ADDRESS;
	}
    }

    /*
     * Set the Nevada registers here
     */
    set_nevada_regs();

    /*
     * Create the network interface interrupt stack:
     */
    createlevel(ETHER_INTLEVEL, NULL, "Network interfaces");

    /*
     * We must now flick the switch in Nevada to enable
     * reception of Hi and Lo interrupts. Note, set_nevada_regs()
     * has been called already so the Nevada register is configured
     * as a control register.
     */
    IO_ASIC->sys_stat2 &= ~(SS2_LO_ENABLE|SS2_HI_ENABLE) & 0xff;

    /*
     * Create and initialize the High IRQ interrupt dispatcher
     */
    nim_init_hi_irq_handler();

    /*
     * Add platform registry services
     */
    reg_add_print_memory(platform_print_memory, "platform_print_memory");
    reg_add_print_features(platform_print_features, "platform_print_features");
    reg_add_net_cstate(nim_health_sierra_light, "nim_health_sierra_light");
    reg_add_onemin(check_for_sierra_overtemp, "check_for_sierra_overtemp");
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
    if (ciscopro_platform && !ciscopro_image) {
	printf(msgtxt_ciscopro);
	DELAY(2*ONESEC);
	mon_reload();
    }
}

/*
 * shmem_parity_error :
 *
 * Handle a shared memory parity error interrupt
 */

static void shmem_parity_error (r4k_context_t *ptr)
{
    ushort shmemstat = IO_ASIC->shmem_stat;

    ttyprintf(CONTTY, "%%PAR-1-FATAL: Shared memory parity error\n");
    ttyprintf(CONTTY, " shared memory status register= %#x\n", shmemstat);
    ttyprintf(CONTTY, " error(s) reported for: ");
    if ( !(shmemstat & SHMEM_MPU_PERR) )
	ttyprintf(CONTTY, "CPU ");
    if ( !(shmemstat & SHMEM_SLOT1_PERR) )
	ttyprintf(CONTTY, "NIM1 ");
    if ( !(shmemstat & SHMEM_SLOT2_PERR) )
	ttyprintf(CONTTY, "NIM2 ");
    if ( !(shmemstat & SHMEM_SLOT3_PERR) )
	ttyprintf(CONTTY, "NIM3 ");
    ttyprintf(CONTTY, "on byte(s): ");
    if (shmemstat & (SHMEM_BYTE0_PERR|SHMEM_BYTE1_PERR))
	ttyprintf(CONTTY, "0/1 ");
    if (shmemstat & (SHMEM_BYTE2_PERR|SHMEM_BYTE3_PERR))
	ttyprintf(CONTTY, "2/3 ");
    ttyprintf(CONTTY, "\n");

    /*
     * Reset the parity error
     */
    IO_ASIC->shmem_ctl &= ~SHMEM_PERR_CLEAR;
    IO_ASIC->shmem_ctl |= SHMEM_PERR_CLEAR;
    flush_io_wbuffer();    

    /*
     * Return to the monitor
     */
    r4k_return_to_monitor(SIGERROR, 0, ptr);
}


/*
 * Analyze the config register and setup r4k_1ms_pipe_ticks
 * and r4k_4ms_pipe_ticks. Support divide by 2 (100Mhz), divide
 * by 3 (150Mhz) and special case for (133mhz), divide by 4 (200Mhz).
 * Note, assuming that the system bus is always 50 Mhz.
 */
void platform_r4k_set_clock_params (ulong config_reg)
{
    ulong current_ec_config;

    /*
     * Find the EC bits which tell us the processor speed.
     */
    current_ec_config = config_reg & CONFIG_EC_MASK;

    switch(current_ec_config) {

      case CONFIG_EC_DIV4:
	/*
	 * If it is a 200mhz mipe (divide by 2), load the
	 * appropriate values for pipe_speed and period in nanosecs.
	 */
	r4k_1ms_pipe_ticks = R4K_1MS_AT_200MHZ;
	r4k_4ms_pipe_ticks = R4K_4MS_AT_200MHZ;
	r4k_pipe_speed = 200;
	r4k_count_period_ns = R4K_NS_PERIOD_AT_200MHZ;
	break;

      case CONFIG_EC_DIV3:
	/*
	 * If it is a 150mhz mipe (divide by 3), load the
	 * appropriate values for pipe_speed and period in nanosecs.
	 */
	r4k_1ms_pipe_ticks = R4K_1MS_AT_133MHZ;
	r4k_4ms_pipe_ticks = R4K_4MS_AT_133MHZ;
	r4k_pipe_speed = 133;
	r4k_count_period_ns = R4K_NS_PERIOD_AT_133MHZ;
	break;

      case CONFIG_EC_DIV2:
	/*
	 * If it is a 100mhz mipe (divide by 2), load the
	 * appropriate values for pipe_speed and period in nanosecs.
	 */
	r4k_1ms_pipe_ticks = R4K_1MS_AT_100MHZ;
	r4k_4ms_pipe_ticks = R4K_4MS_AT_100MHZ;
	r4k_pipe_speed = 100;
	r4k_count_period_ns = R4K_NS_PERIOD_AT_100MHZ;
	break;
    }
}

/* end of file */
