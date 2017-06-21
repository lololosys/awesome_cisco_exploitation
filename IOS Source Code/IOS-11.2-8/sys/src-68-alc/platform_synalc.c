/* $Id: platform_synalc.c,v 3.1.64.5 1996/09/06 00:00:48 rbadri Exp $
 * $Source: /release/112/cvs/Xsys/src-68-alc/platform_synalc.c,v $
 *------------------------------------------------------------------
 * platform_synalc.c -- Platform specific support for Synergy ATM line card
 * (based on platform_c3000.c and platform_xx.c)
 *
 * Aug 31, 1995, Rick Lowe
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: platform_synalc.c,v $
 * Revision 3.1.64.5  1996/09/06  00:00:48  rbadri
 * CSCdi64212:  martian table bogusly externed in a .c file.
 * Branch: California_branch
 * Use registry to adjust the martian table. Don't manipulate it
 * directly with redecalared data structures.
 *
 * Revision 3.1.64.4  1996/06/27  16:36:43  hampton
 * Keep your mitts off the scheduler private data structures.  [CSCdi61518]
 * Branch: California_branch
 *
 * Revision 3.1.64.3  1996/06/19  04:44:18  smackie
 * Buffered logging support gets a hot wax and shine. (CSCdi53462)
 *
 *   o Restructure buffered logging to be a independant device
 *   o Add support for "clear logging"
 *   o Enable buffered logging by default on most platforms
 *   o Start hiding logger structures in private include files
 *
 * Branch: California_branch
 *
 * Revision 3.1.64.2  1996/05/24  21:54:53  jjohnson
 * CSCdi58698:  unable to build alc image
 * every platform needs a platform_interface_ordering function
 *
 * Revision 3.1.64.1  1996/05/09  14:41:12  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.1.92.1  1996/04/27  07:25:26  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.1.82.3  1996/04/25  05:46:18  rlowe
 * Clean out obsolete files and code.
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 3.1.82.2  1996/04/10  06:40:49  rlowe
 * Fix up ifdefs; clean-out old, obsolete code.
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 3.1.82.1  1996/04/08  02:09:00  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 3.1.70.4  1996/03/27  21:55:32  sakumar
 * Got rid of some compiler warnings.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.1.70.3  1996/03/23  04:54:43  rlowe
 * More sync-time fun and games.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.1.70.2  1996/03/22  23:40:16  rlowe
 * Give relative paths to wbu include files.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.1.70.1  1996/03/22  09:43:44  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.1.54.1  1996/03/03  21:24:56  rlowe
 * Apply SYNALC content to merge branch.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 3.1.52.1  1996/03/02  01:21:11  rlowe
 * Non-sync sync to mainline tag V111_1_0_3.
 * Branch: Synalc3_Cal_V111_1_0_3_branch
 *
 * Revision 3.1.36.1  1996/02/29  07:50:18  rlowe
 * First pass non-sync sync for Synergy ATM line card to V111_0_16.
 * Branch: Synalc3_Cal_V111_0_16_branch
 *
 * Revision 3.1  1995/11/09  13:24:54  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1.32.1  1996/02/03  07:08:35  rlowe
 * Non-sync sync of Synalc_catchup_branch to V111_0_5.
 * Branch: Synalc_catchup_pretal_branch
 *
 * Revision 2.1.2.2.2.6.2.2  1996/01/20  08:03:33  sgarcia
 * CSCdi47232:  sync 11.1 branch to head of 11.0 branch
 * Branch: Synalc_catchup_branch
 * Sync to Synalc2_LE2_V110_1_0_3_branch_end_of_life.
 *
 * Revision 2.1.2.2.2.6.2.1  1996/01/19  21:57:27  sgarcia
 * CSCdi47203:  sync 11.1 catchup branch to 11.0 ATMFLASH_V11_951214 tag
 * of Synalc2_
 * Branch: Synalc_catchup_branch
 * Patch 11.0 Synalc2_LE2_V110_1_0_3_branch diffs between tags
 * Synalc2_LE2_V110_1_0_3_arizona_arkansas_split_951103 and
 * ATMFLASH_V11_951214
 * into 11.1 branch.
 *
 * Revision 2.1.2.2.2.9  1995/12/27  01:18:08  rlowe
 * Change default hostname for ATM line card to match default config.
 * Branch: Synalc2_LE2_V110_1_0_3_branch
 *
 * Revision 2.1.2.2.2.8  1995/12/02  04:41:53  rlowe
 * Increase the number of queue elements at startup to accommodate
 * large number of buffers allocated then.
 * Branch: Synalc2_LE2_V110_1_0_3_branch
 *
 * Revision 2.1.2.2.2.7  1995/12/01  01:21:37  rlowe
 * Allow platform startup to populate public system buffer pools.  ALC
 * does
 * not run ip-routing and so does not get up-sized buffer pools like
 * routers
 * do.
 * Branch: Synalc2_LE2_V110_1_0_3_branch
 *
 * Revision 2.1.2.2.2.6  1995/10/20  04:21:34  rlowe
 * Enable MFP HW guard timer support for device drivers.
 * Branch: Synalc2_LE2_V110_1_0_3_branch
 *
 * Revision 2.1.2.2.2.5  1995/10/10  00:54:18  rbau
 * use LED driver API to turn on status LED
 * Branch: Synalc2_LE2_V110_1_0_3_branch
 *
 * Revision 2.1.2.2.2.4  1995/09/28  02:14:58  rlowe
 * Do not allow netconfig on the SYNALC; want to use default config
 * instead.
 *
 * Branch: Synalc2_LE2_V110_1_0_3_branch
 *
 * Revision 2.1.2.2.2.3  1995/09/27  15:57:23  sakumar
 * Register PMD interrupt handler after the type of chip has been
 * determined.
 * Branch: Synalc2_LE2_V110_1_0_3_branch
 *
 * Revision 2.1.2.2.2.2  1995/09/27  02:42:40  sakumar
 * Correctly register the interrupt handler for the PMC Sierra OC3 PMD.
 * Branch: Synalc2_LE2_V110_1_0_3_branch
 *
 * Revision 2.1.2.2.2.1  1995/09/13  02:16:23  sakumar
 * Some cleanup to allow the Catalyst5000 Supervisor Card to "session"
 * to the ATM card.
 * Branch: Synalc2_LE2_V110_1_0_3_branch
 *
 * Revision 2.1.2.2  1995/09/08  02:11:18  sakumar
 * Post system-configured commands for the parser to ensure that telnet
 * will
 * work from the Catalyst 5000 Supervisor card.
 * Branch: Synalc2_branch
 *
 * Revision 2.1.2.1  1995/08/31  14:18:40  rlowe
 * Carryover useful code from old Synergy_ATMlc_branch (10.3) to new
 * Synalc2_branch (11.0).  Provide platform-dependent code for IOS
 * re-port to Synergy ATM line card.
 *
 * Branch: Synalc2_branch
 *
 * Revision 2.1  1995/08/31  14:12:49  rlowe
 * Placeholders for Synalc2_branch.
 *
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../../boot/cpu.h"
#include "../../boot/buff.h"
#include "registry.h"
#include "ttysrv.h"
#include "logger.h"
#include "stacks.h"
#include "interface_private.h"
#include "packet.h"
#include "exception.h"
#include "config.h"
#include "sys_registry.h"
#include "../os/free.h"
#include "../os/region.h"
#include "../os/pool.h"
#include "../os/nv.h"
#include "../les/if_les.h"
#include "../dev/monitor1.h"
#include "../../boot/reason.h"
#include "../../boot/save_stack.h"
#include "parser.h"

#include "../../wbu/cat5k/alc_cominc/basic_types.h"
#include "../../wbu/cat5k/alc_include/acp_basic_types.h"
#include "../../wbu/cat5k/alc_include/mfp_hw_defs.h"
#include "../../wbu/cat5k/alc_include/mfp_acp_defs.h"
#include "../../wbu/cat5k/alc_cominc/sar_if.h"
#include "../../wbu/cat5k/alc_cominc/lec_if.h"
#include "../../wbu/cat5k/alc_cominc/cam_if.h"
#include "../../wbu/cat5k/alc_cominc/atmdrv_if.h"
#include "../../wbu/cat5k/alc_include/ntc_client.h"
#include "../../wbu/cat5k/alc_include/bdg_if.h"
#include "../../wbu/cat5k/alc_include/lcp_api.h"
#include "alc_lec.h"
#include "../../wbu/cat5k/alc_include/led_if.h"

#include "../lane/lane_registry.h"

#define	NULL_STR	""

/*
 * Values for populating various public buffer pools.  Perhaps some of this
 * belongs in os/buffers.c and os/buffers.h ?  See platform_interface_init.
 */
#define	SMALL_MIN_ALC		1024	/* Minimum free buffers for SYNALC. */
#define	MID_MIN_ALC		256
#define	BIG_MIN_ALC		64
#define	VERYBIG_MIN_ALC		0
#define	LARGE_MIN_ALC		0
#define	HUGE_MIN_ALC		0

#define	SMALL_MAX_ALC		2048	/* Maximum free buffers for SYNALC. */
#define	MID_MAX_ALC		512
#define	BIG_MAX_ALC		256
#define	VERYBIG_MAX_ALC		64
#define	LARGE_MAX_ALC		30
#define	HUGE_MAX_ALC		13

#define	SMALL_INIT_ALC		2048	/* Initial free buffers for SYNALC. */
#define	MID_INIT_ALC		512
#define	BIG_INIT_ALC		128
#define	VERYBIG_INIT_ALC	10
#define	LARGE_INIT_ALC		10
#define	HUGE_INIT_ALC		0


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
 * Local strings
 */

static char hw_revision[20];
static char hw_serial[20];

/*
 * Local miscellaneous variables
 */
cookie_t *cookie;		/* Address of cookie PROM */
extern const cookie_t ACP_cookie ;

interrupt_array_t alc_int_array [ALC_MAX_INTER] = {} ;
const ushort ALC_SHORT_Zero = 0 ;
const ushort *status_ptr = & ALC_SHORT_Zero ;   /* Fake status register. */

hwidbtype *network_a_idb;	/* IDB addresses for interfaces */
hwidbtype *network_b_idb;
hwidbtype *network_c_idb;
hwidbtype *network_c0_idb;
hwidbtype *network_c1_idb;
hwidbtype *network_d_idb;

/*
 * Externs
 */
extern void level2_dummy_rte(void);

extern void post_config(void);



static void platform_post_config (void)
{
    /* call the post_config() function to setup stuff for telnet from NMP */
    post_config();

}


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
    if ((ulong)cookie == -1)
        cookie = (cookie_t *)ADRSPC_COOKIE;
 
    /*
     * Build platform strings
     */
    hw_version = mon_get_rev();
    if (hw_version && (hw_version <= ('Z' - 'A')))
	sprintf(hw_revision, "%c", (hw_version - 1 + 'A'));
    else
	sprintf(hw_revision, "0x%02x", hw_version);
    
    serial = GETLONG(cookie->serial);
    if ((serial & 0x0f) < 10) {
	serial = (((serial & 0x0f) - 10) << 28) + (serial >> 4);
	sprintf(hw_serial, "%u", serial);
    } else {
	sprintf(hw_serial, "%08x", serial);
    }

    /*
     * The routine gets called so early, nothing is initialized yet
     */
    cputype = mon_proctype();
 
    /*
     * Turn on the Status lamp to indicate that we're up and running
     */
    set_led(ACP_WRRG_PMD_STAT_LED, ACP_PMD_STAT_LED_GRN);
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
    ulong mainsize;

    /*
     * Find out how much main dram and iomemory the Rom Monitor thinks
     * we've got
     */
    mainsize = mon_getmemsize();

    /*
     * Add a region to describe all of main DRAM
     */
    region_create(&main_region, "main",
		  (void *)(RAMBASE), mainsize,
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
		  _END, (mainsize + (RAMBASE) - (ulong)_END),
		  REGION_CLASS_LOCAL, REGION_FLAGS_DEFAULT);

    mempool_create(&pmem_mempool, "Processor", &pmem_region, 0,
		   NULL, 0, MEMPOOL_CLASS_LOCAL);

    /*
     * Add aliases for mandatory mempools
     */
    mempool_add_alias_pool(MEMPOOL_CLASS_IOMEM, &pmem_mempool);
    mempool_add_alias_pool(MEMPOOL_CLASS_FAST,  &pmem_mempool);
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
    /*
     * Populate memory pools with revised sizes for ALC.  Our min
     * sizes are large to accommodate interrupt handlers using public
     * pools.  Note the increase in memory element to handle the
     * increased buffer pool capacities.
     */

    switch (type) {
      case PLATFORM_BUFFER_BOOTSTRAP:
      case PLATFORM_BUFFER_NORMAL:
      case PLATFORM_BUFFER_ROUTING:

	grow_element (SMALL_MIN_ALC + MID_MIN_ALC + BIG_MIN_ALC +
			VERYBIG_MIN_ALC + LARGE_MIN_ALC + HUGE_MIN_ALC) ;

	pool_adjust
	    (small,   SMALL_MIN_ALC,   SMALL_MAX_ALC,   SMALL_INIT_ALC,   TRUE);
	pool_adjust
	    (middle,  MID_MIN_ALC,     MID_MAX_ALC,     MID_INIT_ALC,     TRUE);
	pool_adjust
	    (big,     BIG_MIN_ALC,     BIG_MAX_ALC,     BIG_INIT_ALC,     TRUE);
	pool_adjust
	    (verybig, VERYBIG_MIN_ALC, VERYBIG_MAX_ALC, VERYBIG_INIT_ALC, TRUE);
	pool_adjust
	    (large,   LARGE_MIN_ALC,   LARGE_MAX_ALC,   LARGE_INIT_ALC,   TRUE);
	pool_adjust
	    (huge,    HUGE_MIN_ALC,    HUGE_MAX_ALC,    HUGE_INIT_ALC,    TRUE);

	break;
    }
}


/*
 * platform_get_string
 *
 * Returns a variety of strings unique to the SYNALC
 */

char *platform_get_string (platform_string_type type)
{
    char  *value;

    switch (type) {
      case PLATFORM_STRING_NOM_DU_JOUR:
	value = "C5kALC";
	break;

      case PLATFORM_STRING_DEFAULT_HOSTNAME:
	value = "ATM";
	break;

      case PLATFORM_STRING_PROCESSOR:
	value = "68ec030";
	break;

      case PLATFORM_STRING_PROCESSOR_REVISION:
	value = NULL_STR ;
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
	value = NULL_STR ;
	break;
    }

    return(value);
}

/*
 * platform_get_value
 *
 * Returns a variety of integer values unique to the SYNALC
 */

uint platform_get_value (platform_value_type type)
{
    uint value;

    switch (type) {
      case PLATFORM_VALUE_SERVICE_CONFIG:
	/* NEVER go out to the network to grab a config. */
	value = FALSE ;
	break;
	
      case PLATFORM_VALUE_FEATURE_SET:
	value = 0x0000;
	break;
	
      case PLATFORM_VALUE_HARDWARE_REVISION:
	value = 0x0000;
	break;

      case PLATFORM_VALUE_HARDWARE_SERIAL:
	value = 0x0000;
	break;

      case PLATFORM_VALUE_VENDOR:
	value = VENDOR_CISCO;
	break;

      case PLATFORM_VALUE_CPU_TYPE:
	value = mon_proctype();
	break;

      case PLATFORM_VALUE_FAMILY_TYPE:
	value = FAMILY_SYNALC;
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
 * platform_print_features
 *
 */

static void platform_print_features (void)
{
    uchar feature_set = platform_get_value(PLATFORM_VALUE_FEATURE_SET);

    /*
     * Print out SW supported ie. enterprise, desktop, etc.
     * Print out cookie value.
     */
    switch (feature_set) {
	  default:
	    printf("Authorized for ATM software set. (0x%x)  ",
		   feature_set);
	    printf("\n");
	    break;
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

/*
 * StackBindVector
 * Bind interrupt handlers to vector table slots.
 */
void
StackBindVector (long VecNum, long index, void * routine)
{
    register unsigned long      *p ;
 
#ifdef DEBUG
    fprintf(CONTTY,
            "StackBindVector (VecNum=%d, Index=%d, VecAddr=%x, routine=%#x)\n",
            VecNum, index, p, routine);
#endif
 
    /*
     * Assign handler to interrupt vector.  Wrapper NOT used for non-MFP
     * interrupts, or MFP system timer interrupt.
     */
    if ((MIN_USR_VECTOR <= VecNum) && (VecNum < MAX_VECS_IN_TBL) &&
                                                (VecNum != SYSTIMR_IVECTOR))
    {
        p = & alc_int_array [index] . function ;
        *p++ = (ulong) (routine) ;              /* function */
        *p++ = (ulong) (index) ;                /* idb1     */
        *p++ = (ulong) (NULL) ;                 /* idb2     */
        *p++ = 0 ;                              /* icount   */

        switch (index)
        {
            case ALC_INTR_TSAR:
		routine = level2_ALC_1 ;
                break ;
            case ALC_INTR_RSAR:
		routine = level2_ALC_2 ;
                break ;
            case ALC_INTR_LCPT:
		routine = level2_ALC_3 ;
                break ;
            case ALC_INTR_LCPR:
		routine = level2_ALC_4 ;
                break ;
            case ALC_INTR_NTC:
		routine = level2_ALC_5 ;
                break ;
            case ALC_INTR_CONS:
/*          case ALC_INTR_BRG:          ** SYNALC:  Pseudo-Tx for console. */
            default:                    /* What else can we do ? */
		routine = level2_ALC_0 ;
                break ;
        }
    }

    exceptionHandler (VecNum, routine) ;
}


/*
 * createlevel_ALC
 * Set up interrupt level data structures, do sanity checking
 * Make use of interrupt vectors, instead of assuming autovectors.
 */
 
void createlevel_ALC (
    int level,
    void (*routine) (void),
    char *string,
    long VecNum,
    long IntrIndex)
{
        createlevel (level, routine, string) ;
        StackBindVector (VecNum, IntrIndex, (void *) routine) ;
 
        if (VecNum == CONSOLE_IVECTOR)
                for (level = 1 ; level < NBR_CONSOLE_IVECS ; ++ level)
                        StackBindVector (VecNum + level, IntrIndex,
                                                        (void *) routine) ;
}


/* platform_interface_init
 * 
 * Initialize platform specific interface features
 */

void platform_interface_init (void)
{
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
     * Create the network interface interrupt stack;
     * This also binds the interrupt handlers to their vectors.
     * This also inits the "alc_int_array" handler array.
     */
    createlevel_ALC (SAR_INTLEVEL, level2_nop, "Xmt Sar",
                                         TXSAR_IVECTOR, ALC_INTR_TSAR) ;
    createlevel_ALC (SAR_INTLEVEL, level2_nop, "Rcv Sar",
                                         RXSAR_IVECTOR, ALC_INTR_RSAR) ;
    createlevel_ALC (LCP_INTLEVEL, level2_nop, "LCP TxP",
                                         LCPTX_IVECTOR, ALC_INTR_LCPT) ;
    createlevel_ALC (LCP_INTLEVEL, level2_nop, "LCP RxD",
                                         LCPRX_IVECTOR, ALC_INTR_LCPR) ;
    createlevel_ALC (NTC_INTLEVEL, level2_nop, "NTC intr",
                                         NTC_IVECTOR, ALC_INTR_NTC) ;

    /*
     * Add platform registry services
     */
    reg_add_print_memory(platform_print_memory, "platform_print_memory");
    reg_add_print_features(platform_print_features, "platform_print_features");
    reg_add_system_configured(platform_post_config, "platform_post_config");

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
    nauxlines = analyzeAUXports();
    maxvtylines = defvtylines = nvtylines = DEFAULT_NVTYS ;

    /*
     * Assign base indexes into the MODEMS[] data structure.
     */
    AuxBase = freeLineBase;
    freeLineBase += nauxlines;
    VTYBase = freeLineBase;

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
}

/*
 * platform_verify_config
 *
 * Validate hardware/software compatability.
 */

void platform_verify_config(void)
{
    /* For plug-and-play, always assume that NV config is good. */
    /* If it is not, then our default config (from code image)  */
    /* will be good.  We definitely do NOT want to go through   */
    /* NV config Q & A setup.                                   */

    nv_valid = TRUE ;
}

/*
 * alc_spurious_level2 / no_alc_level2_int
 *
 * Stub handler for SYNALC assembly wrappers
 */

void alc_spurious_level2 (int int_num)
{
    buginf("\nInterrupt (%d) from an interface with no handler\n", int_num);
}


/*
 * Setup bindings, etc. for SYNALC atm-driver.
 * Do NOT allow NTC interrupts at this time !
 */
 
uchar ALC_InitAtmDriver (void)                                  /* SYN2FIX */
{
		/* ALC drivers for IOS poll for HW handshakes rather */
		/* than blocking the caller.  Thus the guard timer   */
		/* for these HW handshakes is also polled.           */
	MFP_HwtInit (TRUE) ;

        MFP_MaskIntrW ((MFP_IMSK_LCPRD | MFP_IMSK_LCPTP), TRUE) ;
 
                /* LCP Tx Posted is ACP Rx.  ACP Tx Posted is LCP Rx. */
        createlevel_ALC (LCP_INTLEVEL, LCP_Rx_Isr, "LCP TxP",
                                         LCPTX_IVECTOR, ALC_INTR_LCPT) ;
 
        createlevel_ALC (LCP_INTLEVEL, LCP_Tx_Isr, "LCP RxD",
                                         LCPRX_IVECTOR, ALC_INTR_LCPR) ;
 
        MFP_MaskIntrW ((MFP_IMSK_RSAR | MFP_IMSK_TSAR), TRUE) ;
 
        createlevel_ALC (SAR_INTLEVEL, txsar_isr, "Xmt Sar",
                                         TXSAR_IVECTOR, ALC_INTR_TSAR) ;
 
        createlevel_ALC (SAR_INTLEVEL, rxsar_isr, "Rcv Sar",
                                         RXSAR_IVECTOR, ALC_INTR_RSAR) ;
 
                /* Init drivers, then allow SAR interrupts and init TSAR CAM. */
        if (! BDG_DvrInit ())
                return (FALSE) ;

   	if (pmd_chip_type != PMD_PMC) {
        	createlevel_ALC (NTC_INTLEVEL, ntc_isr, "NTC intr",
                     		 	NTC_IVECTOR, ALC_INTR_NTC) ;
    	} else {
        	createlevel_ALC (NTC_INTLEVEL, syn_atm_pmc_oc3_isr, "PMC intr",
                     			NTC_IVECTOR, ALC_INTR_NTC) ;
    	}

 
        MFP_UnmaskIntrW ((MFP_IMSK_LCPRD | MFP_IMSK_LCPTP), FALSE, FALSE) ;
 
        MFP_UnmaskIntrW ((MFP_IMSK_RSAR | MFP_IMSK_TSAR), FALSE, FALSE) ;
 
        if (! BDG_CamInit ())
                return (FALSE) ;
 
        /*
        **      XXX
        ** This should be in the Synergy subsystem init routine.
        */
        lec_alc_init();
 
        return (TRUE) ;
}

void ALC_SW_WDog (void)
{
        /* Do nothing for test purposes. */
}
 

/*
 *  Part of the standard LES test commands.  See les/les_chain.c, etc.
 */
void test_leds (parseinfo *csb)
{
    int	i ;

    for (i = 0; i < 30; i++) {
        /*
         * Turn all LEDs off
         */
        process_sleep_for (100);	/* Let other processes run again */

        /*
         * Turn all LEDs on
         */
        process_sleep_for (100);	/* Let other processes run again */
    }
}

/* End of file */

