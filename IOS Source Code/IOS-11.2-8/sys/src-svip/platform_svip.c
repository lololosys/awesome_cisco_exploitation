/* $Id: platform_svip.c,v 3.1.62.15 1996/09/06 01:12:27 snyder Exp $
 * $Source: /release/112/cvs/Xsys/src-svip/platform_svip.c,v $
 *------------------------------------------------------------------
 * platform_svip.c - Platform specific support for SVIP
 *
 * Sept 1995, David Getchell
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: platform_svip.c,v $
 * Revision 3.1.62.15  1996/09/06  01:12:27  snyder
 * CSCdi68277:  make shr_core, shr_ukernel, sub_cxxxx sub_core_platform
 *              things const, save 712 out of data, 208 from image
 * Branch: California_branch
 *
 * Revision 3.1.62.14  1996/08/28  13:15:00  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.1.62.13  1996/08/20  00:36:10  cheilek
 * CSCdi58374:  More show diag info needed
 * Branch: California_branch
 * Add CCB_CMD_VIP_GET_CONFIG, vip_ll_sw.
 * Add sw_version_str to slot structure.
 *
 * Revision 3.1.62.12  1996/08/07  17:41:42  drowell
 * CSCdi65226:  IPFast vip send stats during init require preallocated
 * buffer
 * Branch: California_branch
 * Changed the size of the pre-allocated buffer from very-big to huge.
 *
 * Revision 3.1.62.11  1996/06/19  04:44:38  smackie
 * Buffered logging support gets a hot wax and shine. (CSCdi53462)
 *
 *   o Restructure buffered logging to be a independant device
 *   o Add support for "clear logging"
 *   o Enable buffered logging by default on most platforms
 *   o Start hiding logger structures in private include files
 *
 * Branch: California_branch
 *
 * Revision 3.1.62.10  1996/06/03  18:04:42  getchell
 * CSCdi52344:  scheduler allocate needed for debug under load
 * Branch: California_branch
 *
 * Revision 3.1.62.9  1996/05/23  03:58:22  kao
 * CSCdi58288:  inconsistent ip cache in VIP after mic rel with traffic
 * Branch: California_branch
 *
 * Revision 3.1.62.8  1996/05/17  12:12:19  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.1.48.3  1996/05/05  23:54:20  ppearce
 * Sync to IbuMod_Calif_baseline_960504
 *
 * Revision 3.1.48.2  1996/04/26  07:57:54  dstine
 * Branch: IbuMod_Calif_branch
 * - commit of IBU modularity work, phase 1.
 *
 * Revision 3.1.48.1  1996/04/03  21:44:20  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.1.62.7  1996/05/01  13:56:26  dcarroll
 * CSCdi56037:  provide oir active check routine
 * Branch: California_branch
 * Need 'oir active check' routine.
 *
 * Revision 3.1.62.6  1996/04/20  15:13:36  getchell
 * CSCdi52652:  Management interrupts not working
 * Branch: California_branch
 *
 * Revision 3.1.62.5  1996/04/05  06:30:43  dcarroll
 * CSCdi53603:  avoid hogging cpu in analyze path
 * Branch: California_branch
 * Add platform dependant delay routines.
 *
 * Revision 3.1.62.4  1996/04/05  05:33:10  dcarroll
 * CSCdi53595:  allow for multiple passes through pas idb init code
 * Branch: California_branch
 * Break up the init idb routines and keep track of idb init state.
 *
 * Revision 3.1.62.3  1996/04/04  19:45:36  tboyle
 * CSCdi51923:  Dynamic particle sizes for VIP2 based on SRAM size
 * Branch: California_branch
 *
 * Revision 3.1.62.2  1996/04/03  22:49:25  jwelder
 * CSCdi47348:  lingering references to rvip
 * Branch: California_branch
 *
 * Revision 3.1.62.1  1996/03/21  23:51:17  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.1.10.4  1996/03/18  23:36:09  mbeesley
 * Branch: ELC_branch
 * Sync to V111_1_3
 *
 * Revision 3.1.10.3  1996/03/07  17:29:36  getchell
 * CSCdi50953:  viper dfs mods
 * Branch: ELC_branch
 *
 * Revision 3.1.10.2  1996/02/16  02:24:28  mbeesley
 * CSCdi48910:  Add storage of fault history for crashes
 * Branch: ELC_branch
 *
 * Revision 3.1.10.1  1996/02/10  00:21:53  mbeesley
 * CSCdi48581:  Add VIP2 support to ELC_branch
 * Branch: ELC_branch
 *
 * Revision 3.1  1995/12/26  21:18:32  getchell
 * Placeholder files for Viper development
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Include files
 */
#include "master.h"
#include "stdlib.h" /* for atoi */
#include "../../boot/cpu.h"
#include "interface_private.h"
#include "packet.h"
#include "sched.h"
#include "logger.h"
#include "ttysrv.h"
#include "exception.h"
#include "stacks.h"
#include "sys_registry.h"
#include "subsys.h"
#include "string.h"

#include "../os/gdb.h"
#include "../os/free.h"
#include "../os/region.h"
#include "../os/hostname.h"
#include "../os/pool.h"
#include "../os/particle.h"
#include "../os/buffers.h"
#include "../os/buffers_inline.h"
#include "../if/network.h"

#include "../src-4k/util_4k.h"
#include "../dev/monitor1.h"
#include "../hes/dgbus.h"

#include "../pas/if_pas.h"
#include "../pas/msg_pas.c"		/* Not a typo, see logger.h */
#include "../pas/pas_id.h"
#include "../src-vip/vip_dbus.h"
#include "../src-svip/svip_dbus.h"
#include "../src-vip/vip_memd.h"
#include "../src-svip/svip_dma.h"

/*
 * These variables are held in the vip-version.c file which
 * gets dynamically generated and compiled by the VIP makefile. They
 * hold the VIP VPLD version number and VIP software version number.
 */
extern ushort vip_sw_vint;

/*
 * Memory pools
 */
static mempool pmem_mempool;
static mempool pcimem_mempool;

/*
 * Memory regions
 */
static regiontype main_region;
static regiontype text_region;
static regiontype data_region;
static regiontype bss_region;
static regiontype pmem_region;
static regiontype pcimem_region;

static regiontype k0_main_region;
static regiontype k0_pcimem_region;
static regiontype k1_main_region;
static regiontype k1_pcimem_region;

/*
 * Buffer pools
 */
static pooltype *particle_pool;
static pooltype *header_pool;

/*
 * Local strings, pointers
 */
static char proc_revision[80];
static char vip_ios_name[20];
static char hw_revision[20];
static char hw_serial[20];
static char *vip_ios_name_ptr = NULL;

/*
 * Freelists
 */
static const ulong freelist_pcimem_block_sizes[] = {
    FREESIZE(0), FREESIZE(1), FREESIZE(2), FREESIZE(3), FREESIZE(4),
    FREESIZE(5), FREESIZE(6), FREESIZE(7), 65536
};

static const ulong nfreelist_pcimem_block_sizes =
    sizeof(freelist_pcimem_block_sizes)/sizeof(ulong);

/*
 * platform_main:
 * Called from main() to allows platforms to initialize basic functions
 * such as LEDs.
 */

void platform_main (void)
{
    ushort hw_version;

    DAS_DEBUG(0x11110010);
    /*
     * Build the platform strings
     */
    sprintf(hw_serial, "%08x", 0);
    hw_version = mon_get_rev();
    sprintf(hw_revision, "0x%02x", hw_version);
    r4k_get_revision_string(proc_revision);

    /*
     * Hook into the scheduler
     */
    kernel_set_sched_loop_hook(svip_sched_loop_end);
}

/*
 * choose_particle_size
 * Called after interfaces are analyzed and before they are initialized
 */

void choose_particle_size (void)
{
    int pa0 = PA_HARDWARE_UNKNOWN;
    int pa1 = PA_HARDWARE_UNKNOWN;
    ulong memsize;

    if (pas_pa_present(PA0)) pa0 = pas_get_pa_type(PA0);
    if (pas_pa_present(PA1)) pa1 = pas_get_pa_type(PA1);

    if ((pa0 == PA_HARDWARE_FDDI_DAS_MM) ||
        (pa1 == PA_HARDWARE_FDDI_DAS_MM) ||
        (pa0 == PA_HARDWARE_FDDI_DAS_SS) ||
        (pa1 == PA_HARDWARE_FDDI_DAS_SS) ||
        (pa0 == PA_HARDWARE_4R) ||
        (pa1 == PA_HARDWARE_4R)) {

       vsys_vec.vip_particle_size = 512;
       return;
    }

    memsize = mon_get_iosize();

    if (memsize == 0x80000) /* 512k SRAM */ {
       vsys_vec.vip_particle_size = 128;
       return;
    }

    if (memsize == 0x100000) /* 1MB SRAM */ {
        if ((pa0 == PA_HARDWARE_1FE) ||
	    (pa1 == PA_HARDWARE_1FE) ||
	    (pa0 == PA_HARDWARE_1FE_TX_ISL) ||
	    (pa1 == PA_HARDWARE_1FE_TX_ISL) ||
	    (pa0 == PA_HARDWARE_1FE_FX_ISL) ||
	    (pa1 == PA_HARDWARE_1FE_FX_ISL) ||
	    (pa0 == PA_HARDWARE_1FE_TX_NISL) ||
	    (pa1 == PA_HARDWARE_1FE_TX_NISL) ||
	    (pa0 == PA_HARDWARE_1FE_FX_NISL) ||
	    (pa1 == PA_HARDWARE_1FE_FX_NISL)) {
	  
	    vsys_vec.vip_particle_size = 512;
        } else {
	    vsys_vec.vip_particle_size = 256;
	}
	return;
    }

    vsys_vec.vip_particle_size = 512;
    return;
}

/*
 * platform_exception_init:
 * Initialize basic interrupt handling for the platform
 */
void platform_exception_init (void)
{

    DAS_DEBUG(0x11110020);

    /*
     * Initialize the NMI handler
     */
    init_clocktick_handler();

    /*
     * Set up DBUS interrupt handler
     */
    install_svip_dbus_handler();

    /* 
     * Set up CBUS STALL interrupt handler
     * This must happen early as no memd accesses are possible until
     * the CYA is configured, and the possibility of cbus stall is
     * great during initialization
     */
    install_cbus_stall_handler();

    /*
     * determine a particle size based on configuration
     */
    choose_particle_size ();

    /*
     * Set up DMA handler
     */
    install_dma_handler();

    /*
     * Set up MALU Attention handler
     */
    install_cbus_handler();

    /*
     * Set up Error handler
     */
    install_svip_error_handler();

    /*
     * Restart count/compare mechanism due to the possibly 
     * really long time we have spent in here doing initialization
     * with the timer interrupt off.
     */
    restart_count_compare(0);
}


/*
 * platform_memory_init:
 * Define and declare all the memory regions and mempools for this platform
 */
void platform_memory_init (void)
{
    ulong memsize, iosize;

    DAS_DEBUG(0x11110030);

    /*
     * Find out how much main dram the Rom Monitor thinks we've got
     */
    memsize = mon_getmemsize();
    
    /*
     * Add a region to describe all of main DRAM
     */
    region_create(&main_region, "main",
		  (void *)RAMBASE, memsize,
		  REGION_CLASS_LOCAL, REGION_FLAGS_DEFAULT);

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
     * Declare a region from the end of BSS to the end of the processor
     * portion of DRAM and create a "local" mempool based on it.
     */
    region_create(&pmem_region, "mainheap",
		  _END, (RAMBASE + memsize - (ulong)_END), 
		  REGION_CLASS_LOCAL, REGION_FLAGS_DEFAULT);

    mempool_create(&pmem_mempool, "Processor", &pmem_region, 32,
		   NULL, 0, MEMPOOL_CLASS_LOCAL);

    /*
     * Find out how much shared memory is installed
     */
    iosize  = mon_get_iosize();

    /*
     * Declare a region and mempool for shared memory
     */
    region_create(&pcimem_region, "pcimem",
		  (void *)ADRSPC_PCIMEM, iosize,
		  REGION_CLASS_PCIMEM, REGION_FLAGS_DEFAULT);
    mempool_create(&pcimem_mempool, "PCI", &pcimem_region, 32,
		   freelist_pcimem_block_sizes, nfreelist_pcimem_block_sizes,
		   MEMPOOL_CLASS_PCIMEM);

    /*
     * Create the regions for K0 and K1 segments
     */
    region_create(&k0_main_region, "main_k0",
		  (void *)R4K_K0BASE, memsize,
		  REGION_CLASS_LOCAL, REGION_FLAGS_DEFAULT);
    region_create(&k0_pcimem_region, "pcimem_k0",
		  (void *)(R4K_K0BASE + PHY_ADRSPC_PCIMEM),
		  iosize, REGION_CLASS_PCIMEM, REGION_FLAGS_DEFAULT);
    region_create(&k1_main_region, "main_k1",
		  (void *)R4K_K1BASE, memsize,
		  REGION_CLASS_LOCAL, REGION_FLAGS_DEFAULT);
    region_create(&k1_pcimem_region, "pcimem_k1",
		  (void *)(R4K_K1BASE + PHY_ADRSPC_PCIMEM),
		  iosize, REGION_CLASS_PCIMEM, REGION_FLAGS_DEFAULT);

    region_add_alias(&main_region,  &k0_main_region);
    region_add_alias(&pcimem_region, &k0_pcimem_region);
    region_add_alias(&main_region,  &k1_main_region);
    region_add_alias(&pcimem_region, &k1_pcimem_region);

    /*
     * Add aliases for mandatory mempools
     */
    mempool_add_alias_pool(MEMPOOL_CLASS_IOMEM, &pmem_mempool);
    mempool_add_alias_pool(MEMPOOL_CLASS_FAST, &pmem_mempool);
    mempool_add_alias_pool(MEMPOOL_CLASS_ISTACK, &pmem_mempool);
    mempool_add_alias_pool(MEMPOOL_CLASS_PSTACK, &pmem_mempool);

    DAS_DEBUG(0x55552222);
    DAS_DEBUG(&pmem_mempool);
    DAS_DEBUG(&pcimem_mempool);
}

/*
 * platform_buffer_init:
 * Define and declare all the buffer sizes for this platform
 * VIP is only called with PLATFORM_BUFFER_NORMAL
 */
void platform_buffer_init (platform_buffer_init_type type)
{
    pool_adjust(small, SMALL_MIN, SMALL_MAX1, SMALL_INIT1, TRUE);
    pool_adjust(middle, MID_MIN, MID_MAX1, MID_INIT1, TRUE);
    pool_adjust(big, BIG_MIN, BIG_MAX1, BIG_INIT1, TRUE);
    pool_adjust(verybig, VERYBIG_MIN, VERYBIG_MAX1, VERYBIG_INIT1, TRUE);
    pool_adjust(large, LARGE_MIN, LARGE_MAX1, LARGE_INIT1, TRUE);

    /*
     * Need at least one huge at init time for IPC/DFS packet
     */
    pool_adjust(huge, HUGE_MIN, HUGE_MAX1, VIP2_HUGE_INIT1, TRUE);
}

/*
 * platform_get_string:
 * Returns a variety of strings unique to the SVIP
 */
char *platform_get_string (platform_string_type type)
{

    char *value;

    switch (type) {

    case PLATFORM_STRING_NOM_DU_JOUR:
	value = "VIP2";
	break;

    case PLATFORM_STRING_DEFAULT_HOSTNAME:
	if (!vip_ios_name_ptr) {
	    sprintf(vip_ios_name, "VIP-Slot%d", vip_logical_slot());
	    vip_ios_name_ptr = vip_ios_name;
	}
	value = vip_ios_name_ptr;
	break;

    case PLATFORM_STRING_PROCESSOR:
	value = "R4K";
	break;

    case PLATFORM_STRING_PROCESSOR_REVISION:
	value = proc_revision;
	break;

    case PLATFORM_STRING_VENDOR:
	value = "cisco";
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
 * platform_get_value:
 * Returns a variety of integer values unique to the SVIP
 */
uint platform_get_value (platform_value_type type)
{
    uint value;

    switch (type) {
      
    case PLATFORM_VALUE_SERVICE_CONFIG:
	value = FALSE;
	break;

    case PLATFORM_VALUE_FEATURE_SET:
	value = 0x0000;
	break;

    case PLATFORM_VALUE_HARDWARE_REVISION:
	value = mon_get_rev();
	break;

    case PLATFORM_VALUE_HARDWARE_SERIAL:
	value = 0;
	break;

    case PLATFORM_VALUE_VENDOR:
	value = VENDOR_CISCO;
	break;

    case PLATFORM_VALUE_CPU_TYPE:
	value = CPU_VIP;
	break;

    case PLATFORM_VALUE_FAMILY_TYPE:
	value = (cpu_type == CPU_VIP) ? FAMILY_VIP : FAMILY_UNKNOWN;
	break;

    case PLATFORM_VALUE_REFRESH_TIME:
	value = REFRESHTIME;
	break;

    case PLATFORM_VALUE_LOG_BUFFER_SIZE:
    default:
	value = 0;
	break;
    }

    return(value);
}

/*
 * vip_system_configured:
 * VIP post-configuration callback
 */
static void vip_system_configured (void)
{
    /*
     * Disable netconfig_enable to stop spawning off config
     * probe processes. Also call whoami() to setup hostname.
     */
    netconfig_enable = FALSE;
    whoami();
}

/*
 * svip_setup_hardware_registers:
 */
static void svip_setup_hardware_registers (void)
{
    u64  temp64;
    struct {
	ulong upper;
	ulong lower;
    }split64;


    /* Set up the PMA ASIC */
    DAS_DEBUG(0x11110040);


    /* 
     * First:  Initialize configuration register 1
     * Sets bus timeouts, etc. Uses size set by rom monitor
     */
    move64((u64 *)&PMA_ASIC->config_reg1, (u64 *)&split64);
    split64.lower &=  (CR1L_SRAM_SIZE_MASK | CR1L_SRAM_BANK_SIZE_MASK);
    split64.lower |= PMA_CR1L_DEFAULT;
    split64.upper = PMA_CR1U_DEFAULT;
    move64((u64*)&split64, (u64 *)&PMA_ASIC->config_reg1);


    /*
     * Second:  Initialize PCI latency timer, and PCI timeout counters
     */
    PMA_ASIC->pci_latency_time = PCI_LATENCY_TIME_DEFAULT;
    PMA_ASIC->pci_tmout_cntrs = PMA_TIMER_CNTR_DEFAULT;

    /*
     * Initialize Memory sector for PCI target accesses
     * Defines the starting address of PCI memory as seen
     * from memory space of the PCI bus.
     */
    PMA_ASIC->pci_mem_loc = PCI_MEM_MAP_ADRSPC_PCIMEM; 	

    /*
     * Initialize I/O sector for PCI target accesses
     * Defines the starting address of PCI memory as seen
     * from I/O space of the PCI bus.
     */
    PMA_ASIC->pci_io_loc = PCI_IO_MAP_ADRSPC_PCIMEM;

    /* Clear the pma asic pci status command register */
    PMA_ASIC->pci_status_cmd = PMA_PCI_STAT_CMD_DEFAULT;

    /* Clear the pma asic error register */
    temp64 = 0;
    move64(&temp64, (u64 *)&PMA_ASIC->err_reg); 

    /*
     * Initialize Interrupt mask register (Config reg 2)
     * Sets interrupt masks for stall, parity errs, timeout errs, bus 
     * errs, etc.
     */
    PMA_ASIC->intr_mask_reg = PMA_INTR_MASK_REG_DEFAULT;

    /*
     * Init the network IO throttling mechanism
     */
    svip_netint_throttle_init();
}

/*
 * svip_subsys_init_class:
 * Invoke pas_subsys_init_class and then tell RSP we are ready
 */
static void svip_subsys_init_class (ushort class) {

    if (class == SUBSYS_CLASS_DRIVER) {
        pas_subsys_init_class(class);
 
	/*
	 * Let the RP/RSP know we are ready
	 */
	vip_kernel_ready();
    }
}

/*
 * vip_registry_init:
 * Register some VIP specific functions
 */
static void vip_registry_init (void)
{
    /*
     * Setup hardware registers
     */
    svip_setup_hardware_registers();

    reg_add_subsys_init_class(svip_subsys_init_class,
			      "svip_subsys_init_class");
    /*
     * Setup a phony handler for autoaddr_okay() to stop config probes
     */
    reg_add_system_configured(vip_system_configured, "vip_system_configured");

    /*
     * Register the buffer returning
     */
    reg_add_onesec(vip_memd_return_periodic, "vip_memd_return_periodic");
}

/* 
 * platform_interface_init:
 * Initialize platform interface specific features
 */
void platform_interface_init (void)
{

    DAS_DEBUG(0x11110050);

    /*
     * Allocate a global pool of particles
     */
    particle_pool = particle_pool_create("GLOBAL", 
					 POOL_GROUP_PUBLIC,
					 VIP_PARTICLE_BYTES, 
					 POOL_DEFAULT_FLAGS,
					 32,
					 &pcimem_mempool);
    if (particle_pool) {
	pool_adjust(particle_pool, MIN_GLOBAL_PARTICLES, 
		    MAX_GLOBAL_PARTICLES, PERM_GLOBAL_PARTICLES, TRUE);
	particle_pool_create_cache(particle_pool, MIN_GLOBAL_PARTICLES);
	pool_adjust_cache(particle_pool, MIN_GLOBAL_PARTICLES);
    }

    /*
     * Init the global header pool
     */
    header_pool = pak_pool_create("Header", POOL_GROUP_PUBLIC, 0,
				  POOL_DYNAMIC | POOL_PUBLIC_DUPLICATE,
				  NULL);
    if (header_pool) {
        pool_adjust(header_pool, HEADER_MIN1, SVIP_DMA_BUFFERS,
                    SVIP_DMA_BUFFERS, TRUE);
    }

    /*
     * Init the pool of static paktypes for fastswitching
     */
    pas_init_fspak_pool();
 
    /*
     * Initialize the DMA particle pools
     */
    svip_init_dma_pools();

    vip_registry_init();		/* setup VIP specific registry 	*/
    pas_init_jumptable();		/* initialize Net IO jumptable 	*/
    
    /* pas init_network_level not called because dma handler runs off of 
       same (already initialized) stack */

    pas_init_pa_nmi_handler();		/* initialize PA Hi IRQ stack */

    /*
     * Add a callback for interface final initialization
     */
    reg_add_if_final_init(pas_if_final_init, "pas_if_final_init");
}

boolean (*platform_interface_ordering) (hwidbtype *new_idb, hwidbtype *old_idb)

     = idb_slotted_ordering;

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
 * and r4k_4ms_pipe_ticks. Support divide by 2 (100Mhz) only for svip.
 * In case, different CPU speed has to be supported in the future,
 * analyze the EC bits of the config reg.
 */
void platform_r4k_set_clock_params (ulong config_reg)
{
    /*
     * It is a 100mhz pipe (divide by 2), load the
     * appropriate values for pipe_speed and period in nanosecs.
     */
    r4k_1ms_pipe_ticks = R4K_1MS_AT_100MHZ;
    r4k_4ms_pipe_ticks = R4K_4MS_AT_100MHZ;
    r4k_pipe_speed = 100;
    r4k_count_period_ns = R4K_NS_PERIOD_AT_100MHZ;
}

/*
 * pci_memaddr_convert:
 * Convert a memory address to the correct cached view 
 */
void *pci_memaddr_convert (void *addr, int attr)
{
    ulong taddr;

    taddr = (ulong) addr;

    /* force address to uncached address */
    taddr &= ~PCIMEM_CWB_OFS;

    switch (attr) {
      case TLB_CACHE_WT:
	taddr |= PCIMEM_CWT_OFS;
	break;

      case TLB_CACHE_NONCOHERENT:
	taddr |= PCIMEM_CWB_OFS;
	break;

      case TLB_UNCACHED:
	break;

      default:
	/* No other modes supported */
	break;
    }

    return((void *)taddr);
}

/*
 * pas_interface_fallback_pool:
 * Return a pointer to an interface's fallback pool
 */
pooltype *pas_interface_fallback_pool (hwidbtype *idb)
{
    return(particle_pool);
}

/*
 * pas_interface_header_pool:
 * Return a pointer to an interface's header pool
 */
pooltype *pas_interface_header_pool (hwidbtype *idb)
{
    return(header_pool);
}

/*
 * vip_dma_buffer_mempool:
 * Return a pointer to the pool to be used for a particle cache
 * by the DMA engine sucking Tx buffers from memd
 */
mempool *vip_dma_buffer_mempool (void)
{
    return(&pcimem_mempool);
}

/*
 * pas_buffer_mempool:
 * Return a pointer to the memory pool that an interface should use
 * for its buffer pool
 */
mempool *pas_buffer_mempool (hwidbtype *idb)
{
    return(&pcimem_mempool);
}

/*
 * pas_netio_interrupts:
 * Control the network IO interrupt enables for a pa bay. Must be 
 * called with interrupts masked.
 */
void pas_netio_interrupts (uint pa_bay, boolean onoff)
{
    volatile ushort *reg;

    reg = (volatile ushort *)&(IO_ASIC->pa_imr);
    if (pa_bay == PA0) {
	if (onoff) {
	    *reg |= PA_IMR_PA0_MASK;
	} else {
	    *reg &= ~PA_IMR_PA0_MASK;
	}
    } else if (pa_bay == PA1) {
	if (onoff) {
	    *reg |= PA_IMR_PA1_MASK;
	} else {
	    *reg &= ~PA_IMR_PA1_MASK;
	}
    } else {
	errmsg(&msgsym(BADPABAY, PA), pa_bay);
    }

    /*
     * Save the pa interrupt mask for scheduler interval implementation
     */
    memd_cntl.pa_int_mask = *reg;
}

/*
 * pas_mngmt_interrupts:
 * Control the PA management interrupt enables for a bay. Must be called with
 * interrupts masked.
 */
void pas_mngmt_interrupts (uint pa_bay, boolean onoff)
{
    volatile ushort *reg;

    reg = (volatile ushort *)&(IO_ASIC->pa_mimr);
    if (pa_bay == PA0) {
	if (onoff) {
	    *reg |= PA_MIMR_PA0_MASK;
	} else {
	    *reg &= ~PA_MIMR_PA0_MASK;
	}
    } else if (pa_bay == PA1) {
	if (onoff) {
	    *reg |= PA_MIMR_PA1_MASK;
	} else {
	    *reg &= ~PA_MIMR_PA1_MASK;
	}
    } else {
	errmsg(&msgsym(BADPABAY, PA), pa_bay);
    }
}

/*
 * pas_install_interrupt_status_reg:
 * Install the address of the interfaces interrupt status register in
 * a format suitable for the posted read mechanism.
 */
void pas_install_status_reg (hwidbtype *idb, 
			     void *addr,
			     boolean memory_mapped_io)
{
    leveltype level;
    pas_cbtype *pascb;
    pa_jumptable_t *ptr;
    int intfc, i;
    ulong pciaddr = 0;

    pascb = INSTANCE(idb);

    if (pascb->pa_bay >= MAX_PA_BAYS) {
	errmsg(&msgsym(BADPABAY, PA), pascb->pa_bay);
	crashdump(0);
    }

    ptr = &pa_jumptable[pascb->pa_bay];
    intfc = pascb->pa_device;

    if (intfc > MAX_INTERFACES_PER_PA) {
	errmsg(&msgsym(BADINTERFACE, PA), intfc, pascb->pa_bay);
	crashdump(0);

    } else {
	level = raise_interrupt_level(ALL_DISABLE);

	(ulong)addr &= PMA_PR_ADDR_MASK;
	if (memory_mapped_io) 
	    (ulong)addr |= PMA_MEM_PR_ACCESS;
	else
	    (ulong)addr |= PMA_IO_PR_ACCESS;
	ptr->s_reg[intfc] = addr;

	reset_interrupt_level(level);
    }

    /* Work around PMA Posted Read register hardware bug */
    if (pas_pa_present(0))
        pciaddr = get_pci_device_base_offset(0,0);
    else if (pas_pa_present(1))
        pciaddr = get_pci_device_base_offset(1,0);
    
    if (pciaddr) {
        for (i = 0; i < 16; i++)
	  PMA_ASIC->posted_rd[i].pr_reg = pciaddr;
    }
}

/*
 * pas_platform_if_final_init: 
 * Gets called off of the if_final_init registry. We loop
 * thru all the interfaces, putting them into the admin down state.
 */
void pas_platform_if_final_init (void)
{
    hwidbtype *idb;

    FOR_ALL_HWIDBS(idb) {
	idb->oir_flags &= ~IDB_FINAL_INIT;
	net_cstate(idb, IDBS_ADMINDOWN);
	if (idb->shutdown && !(idb->status & IDB_NOHARDWARE))
	    (*idb->shutdown)(idb, IDBS_ADMINDOWN);
    }

    /*
     * Init the VIP ipc
     */
    vip_init_ipc();
}

/*
 * vip_sw_version:
 * Return the VIP IOS software version
 */
ushort vip_sw_version (void)
{
    return(vip_sw_vint);
}

/*
 * vip_vpld_version:
 * Return the VIP VPLD version. Note, should be a Rom Monitor EMT call
 * when the monitor supports such stuff
 */
ushort vip_vpld_version (void)
{
    return(0xffff);
}

/*
 * vip_rom_version:
 * Return the VIP Rom version. 
 * Parses the version from the mon_vstring() EMT call.
 * Typical mon_vstring output: 
 *   ROM: System Bootstrap, Version 11.1(11368) [pgreenfi 17], INTERIM SOFTWARE
 *                                                        ^^
 * This would be version 17.
 */
ushort vip_rom_version (void)
{
    ushort version;
    char   *parse_version;

    parse_version = strchr(mon_vstring(), '[');
    if (parse_version)
	parse_version = strchr(parse_version, ' ');
    if (parse_version)
	version = 256*atoi(parse_version);
    else
	version = 0xffff;

    return(version);
}

/*
 * pas_activate_bay :
 * Update the slots structure. Will be moved to pas_controller.c
 */
void pas_activate_bay (uint pa_bay)
{
}

/*
 * pas_slots_init :
 * Reset slots flags. Will be moved to pas_controller.c
 */
void pas_slots_init (void)
{
}

/*
 * pas_get_pa_type :
 * Return pa controller type.
 */
ushort pas_get_pa_type (uint pa_bay)
{
    return(get_pa_type(pa_bay));
}

/*
 * pas_pa_exists :
 * Update slots that pa exists. Will be moved to pas_controller.c.
 */
void pas_pa_exists (uint pa_bay)
{
}

/*
 * pas_pa_analyzed :
 * Determine if this PA is already analyzed. Will be moved to pas_controller.c
 */
boolean pas_pa_analyzed (uint pa_bay)
{
    return(FALSE);
}

/*
 * pas_add_card_entry :
 * Add a Port Adapter to the chassis MIB. Will be moved to pas_controller.c.
 */
void pas_add_card_entry (uint pa_bay)
{
}

/*
 * pas_set_fci_type :
 * Set the idb fci type. Will be moved to pas_controller.c.
 */
void pas_set_fci_type (hwidbtype *idb, uint slot)
{
}

/*
 * pas_delay:
 * platform dependant delay routine.
 */
void pas_delay(uint time)
{
    DELAY(time);
}

/*
 * oir_active:
 * Test if OIR is active
 */ 
boolean pas_oir_active (void) {
    return (FALSE);
}
/* end of file */
