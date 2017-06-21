/* $Id: platform_c7000.c,v 3.8.12.5 1996/08/12 16:07:27 widmer Exp $
 * $Source: /release/112/cvs/Xsys/src-68-c7000/platform_c7000.c,v $
 *------------------------------------------------------------------
 * platform_c7000.c - Platform specific support for the RP1
 *
 * February 1995, Scott Mackie (moved from init_68k.c)
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: platform_c7000.c,v $
 * Revision 3.8.12.5  1996/08/12  16:07:27  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.8.12.4  1996/06/21  18:20:31  kmitchel
 * CSCdi47923:  DBUS-3-NORESP DBUS-3-BADCTRLRTYPE msgs when booting on RSP
 * Branch: California_branch
 *
 * Revision 3.8.12.3  1996/06/19  04:44:26  smackie
 * Buffered logging support gets a hot wax and shine. (CSCdi53462)
 *
 *   o Restructure buffered logging to be a independant device
 *   o Add support for "clear logging"
 *   o Enable buffered logging by default on most platforms
 *   o Start hiding logger structures in private include files
 *
 * Branch: California_branch
 *
 * Revision 3.8.12.2  1996/05/17  11:53:44  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.8.2.2  1996/04/26  07:57:37  dstine
 * Branch: IbuMod_Calif_branch
 * - commit of IBU modularity work, phase 1.
 *
 * Revision 3.8.2.1  1996/04/03  21:41:31  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.8.12.1  1996/03/18  22:00:10  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.6.12.3  1996/03/09  05:12:38  hampton
 * Slim down the kernel of the router.
 * Branch: DeadKingOnAThrone_branch
 *
 * Revision 3.6.12.2  1996/03/07  10:43:35  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.6.12.1  1996/02/20  17:57:11  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.8  1996/02/13  20:39:31  hampton
 * Move buffer initialization to the platform_xxx.c files to eliminate
 * platform dependencies from common code.  [CSCdi48844]
 *
 * Revision 3.7  1996/02/08  23:10:56  jjohnson
 * CSCdi48362:  Need a consistent, global mechanism for writing plural
 * strings.  Standardize on the Plural and ArgAndPlural macros
 *
 * Revision 3.6  1996/01/05  04:56:48  hampton
 * Remove extraneous includes of h/timer.h.  [CSCdi46482]
 *
 * Revision 3.5  1995/12/15  08:01:56  rramacha
 * CSCdi42101:  sh ver does not recognize CX-SMIP or CX-SSIP cards
 *
 * Revision 3.4  1995/11/28  03:16:40  getchell
 * CSCdi42660:  Mic reload results in increment of reported interfaces
 *
 * Revision 3.3  1995/11/17  18:44:31  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:31:47  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:26:02  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.6  1995/11/08  21:27:21  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.5  1995/10/15  18:55:06  eschaffe
 * CSCdi40294:  Increase the number of Protocol Translation sessions
 *
 * Revision 2.4  1995/10/04  23:00:06  hampton
 * The platform_get_value() routine should return CPU and FAMILY values
 * for the hardware it is currently executing upon, not some compiled in
 * value.  This is required to validate that the correct software image
 * has been booted onto a hardware platform.  [CSCdi41098]
 * Revision 1.1.8.3  1995/11/07  23:06:36  getchell
 * CSCdi42660:  Mic reload results in increment of reported interfaces
 * Branch: VIP_branch
 *
 * Revision 1.1.8.2  1995/08/16  23:27:40  fsunaval
 * Branch: VIP_branch
 * 1. Sync VIP_branch between VIP_baseline_950720 and VIP_baseline_950811.
 *
 * Revision 1.1.8.1  1995/07/24  21:20:13  getchell
 * Sync VIP_branch between VIP_baseline_950523 and VIP_baseline_950720
 * Branch: VIP_branch
 *
 * Revision 2.3  1995/07/27  04:45:24  hampton
 * The 7000 RP serial number should be accessible via the platform_get_xxx
 * routines.  [CSCdi37744]
 *
 * Revision 2.2  1995/06/28 23:14:51  tkolar
 * CSCdi35465:  I/O memory needs to be included when dumping core
 * Add flag for regions to determine if they are dumped or not.
 *
 * Revision 2.1  1995/06/07  22:55:47  hampton
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
#include "plural.h"
#include "../os/buffers.h"
#include "../os/free.h"
#include "../os/pool.h"
#include "../os/region.h"
#include "../../boot/src-68-c7000/defs_rp1.h"
#include "../dev/monitor1.h"
#include "../hes/if_mci.h"
#include "../hes/if_fci.h"
#include "../hes/dbus.h"
#include "../os/init.h"

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
 * Externs
 */
extern boolean ec_present;
extern int ncBus;             /* number of cisco cBus controllers */
 

/*
 * Local Storage
 */
int mci_eth;                  /* sum of all ethernets on all mci's */
int mci_ser;                  /* sum of all serials on all mci's */
int mec_eth;                  /* sum of all ethernets on all mec's */
int ctr_trs;                  /* sum of all token rings on all ctr's */
int nMEC;                     /* number of cisco MEC controllers */
int nCTR;                     /* number of cisco CTR controllers */
int nFSIP;                    /* number of cisco FSIP controllers */
int nSSIP;                    /* number of cisco SSIP controllers */
int nSMIP;                    /* number of cisco SMIP controllers */
int nFIP;                     /* number of cisco FIP controllers */
int nHIP;                     /* number of cisco HIP controllers */
int nAIP;                     /* number of cisco AIP controllers */
int nMIP;                     /* number of cisco MIP controllers */
int nCIP;                     /* number of cisco CIP controllers */
int nCIP2;                    /* number of cisco CIP2 controllers */
int nFEIP;                    /* number of cisco FEIP controllers */
int nRVIP;                    /* number of cisco RVIP controllers */
int fsip_ser;                 /* sum of all serials on all FSIP's */
int ssip_ser;                 /* sum of all serials on all SSIP's */
int cip_channel;              /* sum of all channels on all CIPs */
int cip2_channel;             /* sum of all channels on all CIP2s */
int feip_feth;                /* sum of all fast ethernets on all FEIPs */
int rvip_eth;                 /* sum of all ethernets on all RVIPs */
int rvip_feth;                /* sum of all fast ethernets on all RVIPs */
int rvip_tr;                  /* sum of all token rings on all RVIPs */
int rvip_ser;                 /* sum of all serials on all RVIPs */

static ulong hw_serial;
static ulong hw_revision;
static char hw_serial_str[20];
static char hw_revision_str[20];



/*
 * platform_shutdown_ips
 *
 * Invoked via the hardware_shutdown registry when the system is
 * terminating.
 * 
 * Reset all interface processor cards.
 */
static void
platform_shutdown_ips (void)
{
    dbus_reset_IP_slots();
}

/*
 * platform_main
 *
 * Called from main() to allows platforms to initialize basic functions
 * such as LEDs and Rom Monitor workarounds.
 */

void platform_main (void)
{
    dgtype *dgbus;
    
    /*
     * Initialize the dbus and assure all
     * interface processor cards are reset.
     */
    dbus_startup();
    
    /*
     * Build platform strings.
     */
    dgbus = dbus_rpslot_to_dbus();
    hw_serial = dbus_serial_number(dgbus);
    sprintf(hw_serial_str, "%08x", hw_serial);
    hw_revision = dbus_hw_revision(dgbus);
    if (hw_revision < 0x50 || hw_revision == 0xff) {
	hw_revision_str[0] = '\0';;
    } else {
	sprintf(hw_revision_str, "%c%d",
		'A' + ((hw_revision - 0x50) / 8), hw_revision & 0x7);
    }
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
    ulong memsize;

    /*
     * Find out how much main dram the Rom Monitor thinks we've got
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

    mempool_create(&pmem_mempool, "Processor", &pmem_region, 0,
		   NULL, 0, MEMPOOL_CLASS_LOCAL);

    /*
     * Add aliases for mandatory mempools
     */
    mempool_add_alias_pool(MEMPOOL_CLASS_IOMEM, &pmem_mempool);
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
	value = "RP1";
	break;

      case PLATFORM_STRING_DEFAULT_HOSTNAME:
	value = "Router";
	break;

      case PLATFORM_STRING_PROCESSOR:
	value = "68040";
	break;
	
      case PLATFORM_STRING_VENDOR:
	value = "cisco";
	break;
	
      case PLATFORM_STRING_HARDWARE_REVISION:
	value = hw_revision_str;
	break;

      case PLATFORM_STRING_HARDWARE_SERIAL:
	value = hw_serial_str;
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
	value = hw_revision;
	break;

      case PLATFORM_VALUE_HARDWARE_SERIAL:
	value = hw_serial;
	break;

      case PLATFORM_VALUE_VENDOR:
	value = VENDOR_CISCO;
	break;

      case PLATFORM_VALUE_CPU_TYPE:
	value = mon_proctype();
	break;

      case PLATFORM_VALUE_FAMILY_TYPE:
	value = (cpu_type == CPU_RP1) ? FAMILY_RP1 : FAMILY_UNKNOWN;
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
 * platform_print_memory
 *
 */

static void platform_print_memory (void)
{
    if (nvsize)
      printf("%dK bytes of non-volatile configuration memory.\n",
	     nvsize/1024 + 1);
}


/*
 * platform_print_controllers
 *
 */

static void platform_print_controllers (void)
{
    if (nMCI) 
	printf("%d SIP controller%s (%d Serial).\n",
	       ArgAndPlural(nMCI,"","s"), mci_ser);
    if (ncBus) 
	print_cxbus_controllers();
    if (nMEC) 
	printf("%d EIP controller%s (%d Ethernet).\n",
	       ArgAndPlural(nMEC,"","s"), mec_eth);
    if (nFEIP)
	printf("%d FEIP controller%s (%d FastEthernet).\n",
	       ArgAndPlural(nFEIP,"","s"), feip_feth);
    if (nCTR) 
	printf("%d TRIP controller%s (%d Token Ring).\n",
	       ArgAndPlural(nCTR,"","s"), ctr_trs);
    if (nFIP) 
	printf("%d FIP controller%s (%d FDDI).\n",
	       ArgAndPlural(nFIP,"","s"), nfddinets);
    if (nHIP) 
	printf("%d HIP controller%s (%d HSSI).\n",
	       ArgAndPlural(nHIP,"","s"), nhssinets);
    if (nFSIP)
	printf("%d FSIP controller%s (%d Serial).\n",
	       ArgAndPlural(nFSIP,"","s"), fsip_ser);
    if (nSSIP)
	printf("%d SSIP controller%s (%d Serial).\n",
	       ArgAndPlural(nSSIP,"","s"), ssip_ser);
    if (nAIP)
	printf("%d AIP controller%s (%d ATM).\n",
	       ArgAndPlural(nAIP,"","s"), natmnets);
    if (nMIP) {
        printf("%d MIP controller%s ", ArgAndPlural(nMIP,"","s"));
	if (nt1ports > 0 && ne1ports > 0)
	    printf("(%d T1, %d E1).\n", nt1ports, ne1ports);
	else if (nt1ports)
	    printf("(%d T1)\n", nt1ports);
	else printf("(%d E1)\n", ne1ports);
    }
    if (nSMIP) {
        printf("%d SMIP controller%s ", ArgAndPlural(nSMIP,"","s"));
	if (nst1ports > 0 && nse1ports > 0)
	    printf("(%d T1, %d E1).\n", nst1ports, nse1ports);
	else if (nst1ports)
	    printf("(%d T1)\n", nst1ports);
	else printf("(%d E1)\n", nse1ports);
    }
    if (nCIP)
	printf("%d CIP controller%s (%d IBM Channel).\n",
	       ArgAndPlural(nCIP,"","s"), cip_channel);
    if (nCIP2)
	printf("%d CIP2 controller%s (%d IBM Channel).\n",
	       ArgAndPlural(nCIP2,"","s"), cip2_channel);
    if (nRVIP) {
	printf("%d VIP controller%s", ArgAndPlural(nRVIP,"","s"));
	if (rvip_feth)
	    printf("(%u FastEthernet)", rvip_feth);
	if (rvip_eth)
	    printf("(%u Ethernet)", rvip_eth);
	if (rvip_ser)
	    printf("(%u Serial)", rvip_ser);
	if (rvip_tr)
	    printf("(%u Token Ring)", rvip_tr);
	printf(".\n");
    }
    if (ec_present)
	printf("Environmental Controller.\n");
}


/* platform_interface_init
 * 
 * Initialize platform interface specific features
 */

void platform_interface_init (void)
{
    init_errors = 0;			/* no init errors yet */

    /*
     * Add platform registry services
     */
    reg_add_hardware_shutdown(platform_shutdown_ips, "platform_shutdown_ips");

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

    /*
     * Discover the hardware.
     * AUX line and console are part of the console driver, which is
     * always present.
     */
    if (system_loading) {
	nvtylines = 0;
	maxvtylines = 0;
	defvtylines = 0;
    } else {
	nvtylines = maxvtylines = defvtylines = DEFAULT_NVTYS;

    }
    nauxlines = 1;
    nconlines = 1;

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
     * of the MODEMS[] data structure.  We've already initialized
     * the data structures associated with the console and any STP lines.
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




    
