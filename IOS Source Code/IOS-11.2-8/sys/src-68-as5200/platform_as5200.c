/* $Id: platform_as5200.c,v 1.1.70.2 1996/09/06 01:12:08 snyder Exp $
 * $Source: /release/112/cvs/Xsys/src-68-as5200/platform_as5200.c,v $
 *------------------------------------------------------------------
 * platform_as5200.c - Platform specific support for the AS5200
 *
 * December 1995, Joe Turner
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: platform_as5200.c,v $
 * Revision 1.1.70.2  1996/09/06  01:12:08  snyder
 * CSCdi68277:  make shr_core, shr_ukernel, sub_cxxxx sub_core_platform
 *              things const, save 712 out of data, 208 from image
 * Branch: California_branch
 *
 * Revision 1.1.70.1  1996/06/16  21:19:42  vnguyen
 * Finally committing Brasil to California_branch
 *
 * Revision 1.1  1995/12/03  21:25:15  jturner
 * Placeholder for Brasil.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
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
#include "../../boot/cpu.h"

/*
 * terminator/compression board installed bit in MB config register
 */
#define TERM_BOARD_INSTALLED_MASK      8        /* installed if 0 */

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

static char hw_revision[20];
static char hw_serial[20];

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
static char *as5200_nom_du_jour(void);
static void as5200_reset_interrupts(int cputype, cookie_t *cookie);

/*
 * Externs
 */
extern void level4_hi_irq_dispatch(void);

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
    int cputype;

    /*
     * Whilst we're in here, set up the cookie and status register
     */
    cookie = mon_get_cookie();
    if ((ulong)cookie == (ulong)-1)
        cookie = (cookie_t *)ADRSPC_COOKIE;
    status_ptr = (volatile ushort *)ADRSPC_STATUS;
 
    /*
     * Build platform strings
     */
    hw_version = mon_get_rev();
    if (hw_version && (hw_version <= ('Z' - 'A')))
	sprintf(hw_revision, "%c", (hw_version - 1 + 'A'));
    else
	sprintf(hw_revision, "0x%02x", hw_version);
    
    /*
     * Format the serial number of the cookie.
     */
    serial = GETLONG(cookie->serial);
    sprintf(hw_serial, "%08x", serial);

    /*
     * The routine gets called so early, nothing is initialized yet
     */
    cputype = mon_proctype();
 
    /*
     * Reset any oustanding interrupt sources
     */
    as5200_reset_interrupts(cputype, cookie);
    
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
 * platform_memory_init
 *
 * Define and declare all the memory regions and mempools for this platform
 */

void platform_memory_init (void)
{
    ulong mainsize, iosize;
    uchar *iostart;

    /*
     * Find out how much main dram and iomemory the Rom Monitor thinks
     * we've got
     */
    mainsize = mon_getmemsize();
    iosize   = mon_get_iosize();
    iostart  = mon_get_iobase();

    /*
     * Check io memory size and set/reset the bus error boundary in the
     * MB control1 register for the 2M and 8M simms.  This is needed to
     * fix a ROM monitor problem that exists in the early shipped systems.
     */
    if (iosize == EIGHT_MEG) {
	*(volatile ushort *)ADRSPC_BRASIL_MB_CTRL1_REG =
		(*(volatile ushort *)ADRSPC_BRASIL_MB_CTRL1_REG & ~PKT_MEM_MASK)
		| (PKT_MEM_EIGHTMEGS<<PKT_MEM_SHIFT);
    }
    else if (iosize == TWO_MEG) {
	*(volatile ushort *)ADRSPC_BRASIL_MB_CTRL1_REG =
		(*(volatile ushort *)ADRSPC_BRASIL_MB_CTRL1_REG & ~PKT_MEM_MASK)
		| (PKT_MEM_TWOMEGS<<PKT_MEM_SHIFT);
    }


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
     * Allow main memory to steal from shared memory under
     * low memory situations.
     *
     * Should we allow this on Brasil?  Brasil has a seperate, non
     * contiguous packet memory and CPU accesses to this memory are
     * arbitrated differently.
     */
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
 * Returns a variety of strings unique to Brasil
 */

char *platform_get_string (platform_string_type type)
{
    char  *value;

    switch (type) {
      case PLATFORM_STRING_NOM_DU_JOUR:
	value = as5200_nom_du_jour();
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
	value = "cisco";
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

      default:
	value = NULL;
	break;
    }
    return(value);
}

/*
 * platform_get_value
 *
 * Returns a variety of integer values unique to Brasil
 */

uint platform_get_value (platform_value_type type)
{
    uint value;

    switch (type) {
      case PLATFORM_VALUE_SERVICE_CONFIG:
	value = nvsize ? FALSE : TRUE;
	break;
	
      case PLATFORM_VALUE_FEATURE_SET:
	if ((cookie->magic[0] == COOKIE_MAGIC_0) &&
	    (cookie->magic[1] == COOKIE_MAGIC_1))
	    value = cookie->capabilities[1];
	else
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
	value = FAMILY_BRASIL;
	break;

      case PLATFORM_VALUE_REFRESH_TIME:
	value = REFRESHTIME;
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
 * Note: this routine is used for compatibility only and since
 *	BRI is not available (currently), it will always return 0
 */
int count_bri_hardware (void)
{
	return 0;
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
    if (cpu_type == CPU_CANCUN) {
	/*  This space reserved for CiscoPro  */
        if (touch_device((uchar *)ADRSPC_BRASIL_MB_CONFIG_REG)) {
            if (*(volatile ushort *)ADRSPC_BRASIL_MB_CONFIG_REG &
                TERM_BOARD_INSTALLED_MASK) {
                printf("Mother board without terminator card.\n");
            } else {
                printf("Mother board with terminator card.\n");
            }
        }
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
    int    i;
    short  temp;

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
    createlevel(CARRIER_CHANGE, NULL, "DSX1 interface");

    /*
     * Set up interrupt pointers
     *
     * Level 3 Serial interface change interrupt needs to be disabled here
     * for subset images that do not include the serial driver.
     * If the serial driver subsystem is present, this interrupt will be
     * enabled when the serial subsystem is activated.
     */
    *((ushort *)ADRSPC_DEVICE_SERIAL_0) &= ~DEVICE_SERIAL_INT_ENB;
    *((ushort *)ADRSPC_DEVICE_SERIAL_1) &= ~DEVICE_SERIAL_INT_ENB;
    exceptionHandler(IRQ3Vector, level3_brasil);

    /*
     * Enable level 4 & 6 interrupts from T1 nim and level 1, 2 & 3
     * interrupt from modem nim
     */
    temp = *((ushort *)ADRSPC_BRASIL_MB_CTRL1_REG); 
    temp |= (AS5200_LEVEL1_EN | AS5200_LEVEL2_EN | AS5200_LEVEL3_EN | 
       AS5200_LEVEL4_EN  | AS5200_LEVEL6_EN);
    *((ushort *)ADRSPC_BRASIL_MB_CTRL1_REG) = temp;

    /*
     * Enable platform level 4 & 6 handlers.
     */
    switch (cookie->interfaces) {
	case INTERFACE_1E2T:
	    exceptionHandler(IRQ4Vector, level4_brasil);
	    exceptionHandler(IRQ6Vector, level6_brasil);
	    break;
	default:
	    exceptionHandler(IRQ4Vector, level4_nop);
	    exceptionHandler(IRQ4Vector, level6_nop);
	    break;
    }
    
    /*
     * Initialize the handler array
     */
    for (i = 0; i < PAN_MAX_INTER; i++) {
	pan_int_array[i].function = (ulong)level4_nop;
	pan_int_array[i].idb1 = (hwidbtype *)i;
    }

    for (i = 0; i < PAN_MAX_INTER; i++) {
	t1_int_array[i].function = (ulong)level6_nop;
    }

    /*
     * Init  slot interrupt handler.
     */
    pan_int_array[2].function = (ulong)level4_hi_irq_dispatch;

    /*
     * Add platform registry services
     */
    reg_add_print_memory(platform_print_memory, "platform_print_memory");
    reg_add_print_features(platform_print_features, "platform_print_features");
    reg_add_subsys_init_class(nim_subsys_init_class,
                              "nim_subsys_init_class");    
}

boolean (*platform_interface_ordering) (hwidbtype *new_idb, hwidbtype *old_idb)

     = idb_default_ordering;


/*
 * as5200_enable_console
 *
 * Certain ROM monitors will loop if we execute EMT_RELOAD with
 * the console transmitter disabled. Avoid this by enabling the
 * transmitter right before the system image exits
 */
static void as5200_enable_console (void)
{
    tty_portenable(console);  /* Ensure that the console is enabled */
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

    reg_add_hardware_shutdown(as5200_enable_console, "as5200_enable_console");
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
 * as5200_nom_du_jour
 *
 * Return the textual name for the as5200.
 */
static char *as5200_nom_du_jour (void)
{
    char *value;

    switch (cpu_type) {
      /*
       * Brasil, the AS5200, is a superset of Cancun
       */
      case CPU_CANCUN:
	value = "AS5200";
	break;
      default:
	value = "Unknown";
	break;
    }

    return(value);
}

/*
 * as5200_spurious_level4
 *
 * Stub handler for assembly wrappers
 */
void as5200_spurious_level4 (int int_num)
{
    buginf("\nInterrupt (%d) from an interface with no handler\n", int_num);
}

/*
 * as5200_spurious_level6
 *
 * Stub handler for assembly wrappers
 */
void as5200_spurious_level6 (int int_num)
{
    buginf("\nInterrupt (%d) from an interface with no handler\n", int_num);
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
    switch (cookie->interfaces) {
	/*
	 * Check to see if we have two serial interfaces
	 * Brasil only currently comes with a fixed 1e2t config.
	 */
	case INTERFACE_1E2T:
	    return(2);
		
	default:
	/*
	 * Bummer. Unknown derivative
	 */
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

/* as5200_reset_interrupts
 *
 * This routine is called at early system startup, before interrupts
 * are enabled, to quiet any devices that may interrupt. While the ROM
 * monitor has the primary responsibility for doing this, some devices
 * are not properly quiesced by the monitor so we need to do them here.
 */
static void as5200_reset_interrupts (int mycputype, cookie_t *mycookie)
{
    reset_hd64570_online(mycputype, mycookie);
}
