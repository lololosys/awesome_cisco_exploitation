/* $Id: platform_ags.c,v 3.8.12.8 1996/09/06 01:12:05 snyder Exp $
 * $Source: /release/112/cvs/Xsys/src-68-ags/platform_ags.c,v $
 *------------------------------------------------------------------
 * platform_ags.c - Platform specific support for the CSC3/4 
 *
 * February 1995, Scott Mackie (moved from init_68k.c)
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: platform_ags.c,v $
 * Revision 3.8.12.8  1996/09/06  01:12:05  snyder
 * CSCdi68277:  make shr_core, shr_ukernel, sub_cxxxx sub_core_platform
 *              things const, save 712 out of data, 208 from image
 * Branch: California_branch
 *
 * Revision 3.8.12.7  1996/08/28  19:49:14  thille
 * CSCdi67083:  Message externs need a cleanup.
 * Branch: California_branch
 * Fix breakage of ags build by adding variable into platform_ags.c.
 *
 * Revision 3.8.12.6  1996/08/12  16:07:24  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.8.12.5  1996/06/19  04:44:15  smackie
 * Buffered logging support gets a hot wax and shine. (CSCdi53462)
 *
 *   o Restructure buffered logging to be a independant device
 *   o Add support for "clear logging"
 *   o Enable buffered logging by default on most platforms
 *   o Start hiding logger structures in private include files
 *
 * Branch: California_branch
 *
 * Revision 3.8.12.4  1996/05/29  00:59:22  snijsure
 * CSCdi58178:  HTTP problem with flash access
 * Branch: California_branch
 * Fixed broken AGS build.
 *
 * Revision 3.8.12.3  1996/05/17  11:53:25  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.8.2.3  1996/05/02  22:07:02  hampton
 * Remove support for old Multibus token ring cards.
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.8.2.2  1996/04/26  07:57:26  dstine
 * Branch: IbuMod_Calif_branch
 * - commit of IBU modularity work, phase 1.
 *
 * Revision 3.8.2.1  1996/04/03  21:40:22  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.8.12.2  1996/05/10  19:56:41  ronnie
 * CSCdi57382:  AGS image no longer links
 * Branch: California_branch
 *
 * Revision 3.8.12.1  1996/03/18  21:58:58  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.6.12.3  1996/03/09  05:12:08  hampton
 * Slim down the kernel of the router.
 * Branch: DeadKingOnAThrone_branch
 *
 * Revision 3.6.12.2  1996/03/07  10:43:18  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.6.12.1  1996/02/20  17:54:44  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.8  1996/02/13  20:39:20  hampton
 * Move buffer initialization to the platform_xxx.c files to eliminate
 * platform dependencies from common code.  [CSCdi48844]
 *
 * Revision 3.7  1996/02/08  23:10:52  jjohnson
 * CSCdi48362:  Need a consistent, global mechanism for writing plural
 * strings.  Standardize on the Plural and ArgAndPlural macros
 *
 * Revision 3.6  1996/01/05  04:56:43  hampton
 * Remove extraneous includes of h/timer.h.  [CSCdi46482]
 *
 * Revision 3.5  1995/12/15  08:01:54  rramacha
 * CSCdi42101:  sh ver does not recognize CX-SMIP or CX-SSIP cards
 *
 * Revision 3.4  1995/11/28  03:16:31  getchell
 * CSCdi42660:  Mic reload results in increment of reported interfaces
 *
 * Revision 3.3  1995/11/17  18:42:41  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:30:42  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:24:37  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.5  1995/11/08  21:27:05  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.4  1995/10/15  18:54:57  eschaffe
 * CSCdi40294:  Increase the number of Protocol Translation sessions
 *
 * Revision 2.3  1995/10/04  22:59:51  hampton
 * The platform_get_value() routine should return CPU and FAMILY values
 * for the hardware it is currently executing upon, not some compiled in
 * value.  This is required to validate that the correct software image
 * has been booted onto a hardware platform.  [CSCdi41098]
 * Revision 1.1.8.2  1995/11/07  23:06:33  getchell
 * CSCdi42660:  Mic reload results in increment of reported interfaces
 * Branch: VIP_branch
 *
 * Revision 1.1.8.1  1995/07/24  21:19:35  getchell
 * Sync VIP_branch between VIP_baseline_950523 and VIP_baseline_950720
 * Branch: VIP_branch
 *
 * Revision 2.2  1995/06/28  23:14:43  tkolar
 * CSCdi35465:  I/O memory needs to be included when dumping core
 * Add flag for regions to determine if they are dumped or not.
 *
 * Revision 2.1  1995/06/07  22:54:08  hampton
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
#include "sys_registry.h"
#include "config.h"
#include "plural.h"
#include "boxtypes.h"
#include "../os/buffers.h"
#include "../os/free.h"
#include "../os/pool.h"
#include "../os/region.h"
#include "../../boot/src-68-ags/defs_csc2.h"
#include "../dev/monitor1.h"
#include "../filesys/rommon_filesys.h"
#include "../os/hostname.h"
#include "../hes/ec.h"
#include "../hes/if_mci.h"
#include "../os/init.h"


/*
 * Memory pools
 */
static mempool pmem_mempool;
static mempool multibus_mempool;
/*
 * Memory regions
 */
static regiontype main_region;
static regiontype text_region;
static regiontype data_region;
static regiontype bss_region;
static regiontype pmem_region;
static regiontype multibus_region;

/*
 * Freelists
 */
static const ulong freelist_multibus_block_sizes[] = {
    FREESIZE(0), FREESIZE(1), FREESIZE(2), FREESIZE(3), FREESIZE(4),
    10000, 65536
};
static const ulong nfreelist_multibus_block_sizes =
    sizeof(freelist_multibus_block_sizes)/sizeof(ulong);

/*
 * Externs
 */
extern int ncBus;		/* number of cisco cBus controllers */
extern int nSBE16;		/* number of obsolete SBE-16 controllers */
extern int nSTR;		/* number of obsolete STR controllers */


/*
 * Miscellaneous local storage
 */
boolean oir_debug;
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
int nSVIP;                    /* number of cisco SVIP controllers */
int fsip_ser;                 /* sum of all serials on all FSIP's */
int ssip_ser;                 /* sum of all serials on all SSIP's */
int cip_channel;              /* sum of all channels on all CIPs */
int cip2_channel;             /* sum of all channels on all CIP2s */
int feip_feth;                /* sum of all fast ethernets on all FEIPs */
int rvip_eth;                 /* sum of all ethernets on all RVIPs */
int svip_eth;                 /* sum of all ethernets on all SVIPs */
int rvip_feth;                /* sum of all fast ethernets on all RVIPs */
int svip_feth;                /* sum of all fast ethernets on all SVIPs */
int rvip_tr;                  /* sum of all token rings on all RVIPs */
int svip_tr;                  /* sum of all token rings on all SVIPs */
int rvip_ser;                 /* sum of all serials on all RVIPs */
int svip_ser;                 /* sum of all serials on all RVIPs */

int nSTPlines = 0;  	      /* the number of STP serial lines */
int STPBase;		      /* the base where STP lines start */

/*
 * Multibus memory constants.
 */
#define MULTIBASE ((char *)MULTIMEMBASE)
#define MULTIMAX (64*1024)


/*
 * platform_main
 *
 * Called from main() to allows platforms to initialize basic functions.
 * The AGS+, being *really* basic, doesn't do anything.
 */

void platform_main (void)
{
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
    int multisize;
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
     * Find, count, and initialize our multibus memory.
     * Touch each short.  Faster than byte accesses, just as accurate
     * and avoids a rom monitor bug in version 2.1.
     */
    multisize = 0;
    while (touch_device(&MULTIBASE[multisize])) {
        multisize += 2;
        if (multisize >= MULTIMAX)
            break;
    }
    if (multisize) {
	/*
	 * Declare a region from the end of BSS to the end of main dram
	 * and create a "local" mempool based on it.
	 */
	region_create(&multibus_region, "multibus",
		      MULTIBASE, multisize,
		      REGION_CLASS_MULTIBUS, REGION_FLAGS_DEFAULT);

	mempool_create(&multibus_mempool, "Multibus", &multibus_region, 0,
		       freelist_multibus_block_sizes,
		       nfreelist_multibus_block_sizes,
		       MEMPOOL_CLASS_MULTIBUS);
    }

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
	switch (cpu_type) {
	  case CPU_CSC1: value = "CSC1"; break;
	  case CPU_CSC2: value = "CSC2"; break;
	  case CPU_CSC3: value = "CSC3"; break;
	  case CPU_CSC4: value = "CSC4"; break;
	  default:       value = "Unknown"; break;
	}
	break;

      case PLATFORM_STRING_DEFAULT_HOSTNAME:
	if (router_enable)
	    value = "Router";
	else if (protocolconversion && nttylines == 0)
	    value = "PT";
	else
	    value = "CS";
	break;

      case PLATFORM_STRING_PROCESSOR:
	switch (cpu_type) {
	  case CPU_CSC1: value = "68000"; break;
	  case CPU_CSC2: value = "68020"; break;
	  case CPU_CSC3: value = "68020"; break;
	  case CPU_CSC4: value = "68040"; break;
	  default:       value = "Unknown"; break;
	}
	break;
	
      case PLATFORM_STRING_VENDOR:
	value = "cisco";
	break;
	
      case PLATFORM_STRING_PROCESSOR_REVISION:
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
 * Returns a variety of integer values unique to the AGS
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
	switch (cpu_type) {
	  case CPU_CSC3:
	  case CPU_CSC4:
	    value = FAMILY_CSC2;
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
 * platform_print_memory
 *
 */

static void platform_print_memory (void)
{
    if (nvsize)
      printf("%dK bytes of non-volatile configuration memory.\n",
	     nvsize/1024);

    if (multibus_region.size)
	printf("%dK bytes of multibus memory.\n",
	       (multibus_region.size/1024));
}


/*
 * platform_print_controllers
 *
 */

static void platform_print_controllers (void)
{
    if (nMCI) 
	printf("%d MCI controller%s (%d Ethernet, %d Serial).\n", 
	       ArgAndPlural(nMCI,"","s"), mci_eth, mci_ser);
    if (ncBus) 
	printf("%d cBus controller%s.\n", ArgAndPlural(ncBus,"","s"));
    if (nMEC) 
	printf("%d MEC controller%s (%d Ethernet).\n",
	       ArgAndPlural(nMEC,"","s"), mec_eth);
    if (nSBE16)
	printf("%d CSC-R16 controller%s. [obsolete]\n",
	       ArgAndPlural(nSBE16,"","s"));
    if (nSTR)
	printf("%d CSC-1(2)R controller%s. [obsolete]\n",
	       ArgAndPlural(nSTR,"","s"));
    if (ec_present)
	printf("Environmental Controller.\n");
}


/* platform_interface_init
 * 
 * Initialize platform specific features for interfaces and the like
 */

void platform_interface_init (void)
{
    init_errors = 0;			/* no init errors yet */

    /*
     * Add platform registry services
     */
    reg_add_print_memory(platform_print_memory, "platform_print_memory");
    reg_add_print_controllers(platform_print_controllers,
			      "platform_print_controllers");
}

boolean (*platform_interface_ordering) (hwidbtype *new_idb, hwidbtype *old_idb)

     = idb_default_ordering;

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
     * STP lines are discovered during driver subsystem
     * initialization.  The variable nSTPlines is set there.
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
     * Set up the maximum number of virtual terminals if there is the
     * protocol translator option. Ensure that we always have 5 vtys for
     * management purposes.  note that if_init sets the number of VTYs for
     * boxes without protocol translation.
     */
    if (protocolconversion) {
	switch (nSTPlines) {
	  case 0:               /* 0 ports allow maximum of 118 VTYs */
	    maxvtylines = MAXLINES - VTYBase;
	    break;
	  case 96:		/* 96 ports, allow 22 VTYs */
	    maxvtylines = 22;
	    break;
	  case 112:             /* Fully loaded 112 ports, allow 6 VTYS */
	    maxvtylines = 6;
	    break;		
	  default:
	    maxvtylines = 96 - nSTPlines;
	    break;
	}
    }

    ALLlines = nconlines + nttylines + nauxlines + nvtylines;

    /*
     * Now that we've set up our indices, complete the initialization
     * of the MODEMS[] data structure.  We've already initialized
     * the data structures associated with the console and any STP lines.
     */
    auxline_init();
    if (nvtylines)
	vty_init();
    /*
     * If we have teminal lines, change our default hostname to "CS".
     * The hostname_init() code sets the default to "Router".
     */
    if (nttylines)
	set_hostname("CS");
}

static void verify_config_router (void)
{
#ifndef GLOBAL_DEBUG
    if (nttylines) {
	printf("\nGateway Server does not support terminal lines");
	DELAY(2*ONESEC);
	mon_reload();
    }
#endif
}

static void verify_config_terminal_server (void)
{
#ifndef GLOBAL_DEBUG
    if (nnets > 1) {
        printf("\nImproper number of network interfaces");
        flush();
        DELAY(2*ONESEC);
        mon_reload();
    }
#endif
}

static void verify_config_protocol_translator (void)
{
    idbtype *idb;

#ifndef GLOBAL_DEBUG
    if ((nethernets != 1) || (nnets > (nethernets+nserialnets))) {
	printf("\nImproper number or type of network interfaces");
	DELAY(2*ONESEC);
	mon_reload();
    }
    if (nttylines != 0) {
	printf("\nProtocol Translator does not support async lines");
	DELAY(2*ONESEC);
	mon_reload();
    }
#endif
    /*
     * Disable IP on the Serial interface
     */
    FOR_ALL_SWIDBS(idb) {
	if (idb->hwptr->status & IDB_SERIAL)
	    idb->ip_enabled = FALSE;
    }

    /*
     * Use decimal tty numbers
     */
    decimaltty_enable = TRUE;
}

/*
 * platform_verify_config
 *
 * Validate hardware/software compatability.
 */

void platform_verify_config(void)
{
    switch (boxtype) {
      case BOXTYPE_GS:
        verify_config_router();
	break;
      case BOXTYPE_TS:
        verify_config_terminal_server();
	break;
      case BOXTYPE_PT:
        verify_config_protocol_translator();
	break;
      default:
	printf("\n\n%%Unknown software image type %d.\n\n", boxtype);
	DELAY(2*ONESEC);
	mon_reload();

    }
}

/*
 *  We don't have an ATM interface, but we no longer link without this
 *  stub.  Given that this is a dead platform, I'm going to leave it this
 *  this way
 */
void
atm_parser_init(void)
{
	;
}



/*
 * These following (FS_open, FS_read...) routines allow one to open, read 
 * flash files on all other platforms except AGS. The following stubs are
 * introduced only in case AGS, so that one can still build the gs3 images. 
 * 
 * These functions themselves are intemediate measure till full POSIX 
 * like "portable" file system API is implemented in future (colorado).
 */

int FS_open (const char *nm, int input_flag, ulong mode)
{
   return -1;
}

int FS_close (int fd)
{
    return -1;
}

int FS_read (int fd, void *buffer, int byte_count)
{
    return -1; 
}


int FS_stat (const char *name, stat_t *buf)
{
    return -1;
}
