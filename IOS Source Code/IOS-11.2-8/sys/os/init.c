/* $Id: init.c,v 3.7.20.11 1996/08/28 13:03:22 thille Exp $
 * $Source: /release/112/cvs/Xsys/os/init.c,v $
 *------------------------------------------------------------------
 * init.c -- handle CPU independent system initialization functions
 *
 * 27-June-1987, Kirk Lougheed
 *
 * Copyright (c) 1985-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: init.c,v $
 * Revision 3.7.20.11  1996/08/28  13:03:22  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.7.20.10  1996/08/07  09:01:50  snyder
 * CSCdi65343:  more things that should be declared const
 *              char * [] -> char *const []
 *              char *    -> const char []
 *              savings 17,496 data space, 1,852 image space
 * Branch: California_branch
 *
 * Revision 3.7.20.9  1996/07/10  22:22:15  dstine
 * CSCdi62471:  Superfluous #include files in atalk, atm, os files
 * Branch: California_branch
 *
 * Revision 3.7.20.8  1996/06/18  01:48:28  hampton
 * Split the monolithic traffic array into smaller per-protocol traffic
 * arrays.  [CSCdi59224]
 * Branch: California_branch
 *
 * Revision 3.7.20.7  1996/06/17  08:34:42  jhernand
 * Commit Modular Cobra to California_branch
 *
 * Revision 3.7.20.6  1996/05/21  09:58:12  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.7.20.5  1996/05/17  11:34:15  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.7.6.3  1996/05/05  23:40:38  ppearce
 * Sync to IbuMod_Calif_baseline_960504
 *
 * Revision 3.7.6.2  1996/04/26  07:56:52  dstine
 * Branch: IbuMod_Calif_branch
 * - commit of IBU modularity work, phase 1.
 *
 * Revision 3.7.6.1  1996/04/03  21:10:33  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.7.20.4  1996/05/09  14:39:49  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.7.20.3  1996/05/04  01:34:37  wilber
 * Branch: California_branch
 *
 * Integrated Routing and Bridging
 * - Rip out MTR bridging support.
 *
 * Constrained Multicast Flooding
 * - Add the default function return_one.
 * - Initialize the default function for tbridge_cmf to return_one.
 *
 * Name Access List
 *
 * Revision 3.7.20.2.6.1  1996/04/27  07:17:02  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.7.20.2  1996/04/13  06:05:34  dkatz
 * CSCdi53190:  Init time memory charged to *Dead* process
 * Branch: California_branch
 * Ensure that memory allocated by the ephemeral Init process is charged
 * to *Init*.  Copy the allocator PC from the allocated block into the
 * fragment when fragmenting memory.  Add an option to "show memory" to
 * display the allocating process with each block.
 *
 * Revision 3.7.20.1.8.2  1996/04/10  23:48:29  cakyol
 * change misleading variable name
 * from "global_boottime_config_version"
 * to "global_parsed_swversion".
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 3.7.20.1.8.1  1996/04/08  01:59:49  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 3.7.20.1  1996/03/18  21:30:04  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.7.22.1  1996/03/25  02:20:31  cakyol
 * sync LE_Syn_Cal_V111_1_3_branch to LE_Cal_V111_1_0_3_branch
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.7.4.2  1996/03/22  05:25:53  cakyol
 * Found a better way of determining the version number of the
 * config file at boot time.  Leverage from parsing of the
 * "version xx.yy" command.  Define a new variable which
 * represents this value named "global_boottime_config_version".
 * Restore all the other files back to what they were before this.
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 3.7.4.1  1996/03/20  23:50:04  cakyol
 * The 11.2 LECS parser can now FULLY recognize pre 11.2 LECS
 * commands .  As a result of this, we also have a new systemwide
 * global uint variable named "global_nvconfig_version", which
 * reflects the nvram version number in it.
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 3.7  1996/01/25  11:20:25  smackie
 * Registry code gets some much-needed hot-oil massaging, free liposuction
 * and a makeover.  (CSCdi47101)
 *
 *    o Compress registry structures. Saves over 120K for all platforms
 *    o Add registry subsystems and remove feature registry initializion
 *      from registry.c to fix woeful initialization scaling problems
 *    o Remove unused and seemingly unloved registry debugging code
 *    o Add registry memory statistics to shame people into action
 *
 * Revision 3.6  1996/01/05  10:18:00  hampton
 * Move/rename the files containing the deprecated timer callback
 * routines.  [CSCdi46482]
 *
 * Revision 3.5  1995/12/17  18:34:15  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.4  1995/11/22  05:42:59  eschaffe
 * CSCdi42241:  PROTOCONVERT is TRUE for images that don't include the PT
 * option.
 * Obsolete PROTOCONVERT flag in makefiles.  The control for defining
 * protocolconversion for an image is now platform_specific.
 *
 * Revision 3.3  1995/11/17  18:48:34  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:17:59  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:46:21  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.8  1995/11/08  21:18:54  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.7  1995/09/12  21:10:45  pst
 * CSCdi39958:  ip ethernet 802.3 packets bridged when they should be
 * routed
 *
 * Revision 2.6  1995/07/16  18:04:19  thille
 * CSCdi37049:  Console output from SNMP WriteNet SET confuses console user
 * Bad programmer, no doggie biscuit.
 *
 * Revision 2.5  1995/07/15  02:37:05  thille
 * CSCdi37049:  Console output from SNMP WriteNet SET confuses console user
 * Add a silent_mode parameter to some routines and registries and set it
 * to TRUE when making the calls from SNMP WriteNet, netConfigSet, or
 * hostConfigSet.  Also, add an errmsg for each so the event can still get
 * logged, and make it clear what caused the event.  Fix other callers to
 * pass FALSE so current behavior will be maintained.
 *
 * Revision 2.4  1995/06/30  05:21:36  hampton
 * Move externs for the random number generator to their own file.  Remove
 * redundant initialization of the random number generator.  [CSCdi36588]
 *
 * Revision 2.3  1995/06/09 13:12:07  hampton
 * Rename old scheduler calls to fit the new naming convention.
 * [CSCdi35514]
 *
 * Revision 2.2  1995/06/09 00:14:30  schaefer
 * CSCdi34596: logger not started by bootstrap images
 * restore merge casualty
 *
 * Revision 2.1  1995/06/07  21:55:57  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <string.h>
#include "../../boot/cpu.h"
#include "config_register.h"
#include "../os/old_timer_callbacks.h"
#include "subsys.h"
#include "sys_registry.h"
#include "media_registry.h"
#include "interface_private.h"
#include "sched_private.h"
#include "packet.h"
#include "config.h"
#include "ttysrv.h"
#include "logger.h"
#include "free.h"
#include "pool.h"
#include "../snmp/snmp_api.h"
#include "name.h"
#include "../if/network.h"
#include "../ui/command.h"
#include "../ui/debug.h"
#include "../os/os_debug_flags.h"
#include "../if/network_debug_flags.h"
#include "stacks.h"
#include "../ui/service.h"
#include "../ui/exec.h"
#include "access.h"
#include "../if/priority.h"
#include "boot.h"
#include "file.h"
#include "../os/async.h"
#include "../os/nv.h"
#include "aaa.h"
#include "login.h"
#include "../ui/setup.h"
#include "../os/clock.h"
#include "../os/sum.h"
#include "hostname.h"
#include "init.h"
#include "../dev/monitor1.h"
#include "../parser/parser_actions.h"
#include "../os/msg_system.c"		/* Not a typo, see logger.h */


/*
 * Storage
 */
int init_errors;		/* number of init errors found */
uint config_bytes;		/* byte size of a configuration buffer */
int startup_ttynum;		/* Terminal number to use for startup output */

/*
 * this is the value parsed from the "version XX.Y" command.
 */
uint global_parsed_swversion = SYSTEM_VERSION;

/*
 * This is a global knob that, when FALSE, indicates that the
 * archaic route/bridge behaviour is desired, i.e., that when
 * a given protocol is routed on ANY interface on the box, it
 * is regarded as routed by all interfaces.  Those that are not
 * explicitly configured to route it will simply absorb it if
 * they receive it, typically as a side-effect of bridging.
 *
 * When TRUE, this knob indicates that protocols are to be
 * routed or bridged on a per-interface basis, a.k.a. CRB.
 */

boolean global_crb_enable;

/*
 * This is a global knob that, when TRUE, it indicates that protocols
 * are to be routed, bridged or both on a per-bridge-group basis,
 * a.k.a. IRB.
 */
boolean global_irb_enable;

/*
 * Adjunct to the preceding, this is a single global instance
 * of the per-interface rxtypes_flags which provides a global
 * notion of which protocols are routed and which are bridged.
 * When emulating the archaic behaviour, we point all interfaces
 * at this one instance of this structure (rather than a per
 * interface instance) for a shared notion of which protocols
 * are routed and which are bridged.  This allows for some
 * uniformity and efficiency in the switching path by obviating
 * a test for global_crb_enable.
 */

ushort	global_rxtype_flags[MAX_RXTYPES];

boolean protocolconversion = FALSE;        /* Not doing protocol conversion */

/*
 * The standard error message for a command or function that hasn't
 * been included in the current software load.
 */
const char noasm[] = "\n%%Command or function not supported in this software";

/*
 * Some initialization routines central to the operating system 
 */
extern void connect_parser_init(void);
extern void command_parser_init(void);


/*
 * rxtype_flags_init
 * Initialize rxtype route/bridge indicators.
 */
static void rxtype_flags_init (void)
{
    idbtype *swidb;

    global_crb_enable = FALSE;

    memset(global_rxtype_flags, 0, SIZEOF_RXTYPE_FLAGS);
    global_rxtype_flags[RXTYPE_DODIP] |= RXF_ROUTING;
    global_rxtype_flags[RXTYPE_SNAP_DODIP] |= RXF_ROUTING;
    global_rxtype_flags[RXTYPE_SAP_IP] |= RXF_ROUTING;

    FOR_ALL_SWIDBS(swidb) {
	swidb->rxtype_flags = global_rxtype_flags;
    }

    setup_all_bridging();
}

/*
 * system_init - Initialize the system devices and data structures
 *
 * This routine handles most of system initialization. It used to be called
 * as part of scheduler startup with interrupts set to NETS_DISABLE. Many
 * of the routines called as part of startup expect to be called in this
 * context.
 *
 * In order to not break things, we continue to lock out interrupts
 * around the calls to the routines here. However, all code that is called
 * here should be modified to not expect interrupt locking, and instead
 * explicitly lock out interrupts during critical sections.
 */

boolean system_init (boolean loading)
{
    int result;

    /*
     * Initialize the kernel data structures.
     * These do not depend on drivers or protocols being present.
     */

#ifdef INTERRUPT_TORTURE
    process_sleep_for(10*ONESEC);	/* Look for interrupt bugs */
#endif /* INTERRUPT_TORTURE */

    set_interrupt_level(NETS_DISABLE);

    /*
     * Discover which subsystems are present so we can set presence flags
     */
    protocolconversion = (subsys_find_by_name("pt") != NULL);

    freeLineBase = 1;		/* set first free line to 1 , drivers update */
    clock_init();		/* init the system clock */
    idb_first_init();		/* init early idb data structures */
    hostname_init();		/* init hostname defaults  */
    debug_init();       	/* init debug command support */
    make_reason();		/* create string as to why we rebooted */
    init_checkheaps();		/* start task to validate memory */
    pool_init();		/* init the generic pool support */
    pak_pool_init();		/* init the buffer pool support */
    logger_init();	        /* init logger structures */
    aaa_init();			/* init AAA system */
    login_init();		/* init login functions */
    name_init();		/* init host name cache */
    access_init();		/* init access checking structures */
    priority_init();            /* init priority output queueing */
    timer_init();		/* init the timer process */
    timestamp_printf_init();	/* init printing of timestamps */
    boot_init();		/* init file and boot data structures */
    connect_parser_init();	/* parser connect commands */
    command_parser_init();	/* parser command commands */
    exec_parser_init();		/* parser exec commands */
    snmp_core_init();           /* init some snmp core service */
    null_io_init();		/* initialize the null_io_tty */
    tty_core_init();

    /*
     * Initialize the KERNEL subsystems.
     */
    subsys_init_class(SUBSYS_CLASS_KERNEL);

    /*
     * Initialize checksumming
     */
    sum_init();

    /*
     * Initialize default generic network support and services
     */
    network_init();

    /*
     * Allow platforms to register special services and functions
     */
    platform_interface_init();

    /*
     * Activate driver subsystems
     */
    subsys_init_class(SUBSYS_CLASS_DRIVER); 

    idb_final_init();		/* final interface init routines */
    rxtype_flags_init();	/* Initialize rxtype route/bridge indicators */
    nv_init();			/* discover non-volatile memory */
    config_bytes = nvsize ? nvsize : DEFAULT_CONFIG_BYTES;

    /*
     * If NVRAM is particularly large, add a free list equal to its
     * size.  This will increase the odds that a contiguous block of
     * this size will be available for a "write mem" when things
     * get tight.
     */
    if (config_bytes > LARGEST_DEFAULT_FREELIST_SIZE)
	mempool_add_free_list(MEMPOOL_CLASS_LOCAL, config_bytes);

    /*
     * Activate protocol subsystems
     */
    subsys_init_class(SUBSYS_CLASS_PROTOCOL);

    /*
     * Activate library subsystems
     */
    subsys_init_class(SUBSYS_CLASS_LIBRARY); 

    /*
     * Activate the subsystem management interfaces
     */
    subsys_init_class(SUBSYS_CLASS_MANAGEMENT);

    platform_line_init();	 /* init serial lines, AUX line and VTYs */

    /*
     * Finish initializations that depend on loaded protocols
     */
    service_init();		/* init service booleans */
    boot_bootinfo();		/* how did we get booted? */
    if (!system_loading)	/* print configuration on console */
	Print_Hardware();
    if (!system_loading)
	platform_verify_config();	/* verify hardware/software configuration */
    /*
     * Start the major system processes
     */

    set_interrupt_level(ALL_ENABLE);

    result = process_create(critical_background, "Critical Bkgnd",
			    NORMAL_STACK, PRIO_CRITICAL);
    if (result == NO_PROCESS)
	return(FALSE);
    result = process_create(net_background, "Net Background",
			    NORMAL_STACK, PRIO_NORMAL);
    if (result == NO_PROCESS)
	return(FALSE);
    if (!loading)
	logger_start();
    if (loading) {
	result = process_create(bootload, "Boot Load", LARGE_STACK,
				PRIO_NORMAL);
	if (result != NO_PROCESS) {
	    process_set_arg_num(result, loading);
	    process_set_ttynum(result, startup_ttynum);
	}
    }
    reg_invoke_emergency_message(EMERGENCY_SYS_STARTUP);

    /*
     * Spawn the necessary one second background processes
     */
    result = process_create(tty_background, "TTY Background", NORMAL_STACK,
			    PRIO_NORMAL);
    if (result == NO_PROCESS)
	return(FALSE);
    result = process_create(net_onesecond, "Per-Second Jobs", NORMAL_STACK,
			    PRIO_NORMAL);
    if (result == NO_PROCESS)
	return(FALSE);
    result = process_create(net_periodic, "Net Periodic", NORMAL_STACK,
			    PRIO_NORMAL);
    if (result == NO_PROCESS)
	return(FALSE);

    return(TRUE);
}

/*
 * init_process
 * This process initializes the system. All initialization that does not
 * need to be done from the scheduler context is done here.
 */
#define MAXLEN 128		/* this value cooresponds to FNAME_LEN */
				/* in .../boot/defs.h  */

forktype init_process (boolean loading)
{
    hwidbtype *idb;
    int debugmode, i;
    tt_soc *tty;
    char buff[MAXLEN];
    boolean status;

    /* Note who we are. */

    forkx->init_process = TRUE;

    /*
     * Start up all of the system support.
     */

    status = system_init(loading);
    if (!status) {
	printf("\n\n%%SYSTEM-1-INITFAIL: Cannot initialize system.  "
	       "Not enough memory.\n\n");
	for (i=1000000; i>0; i--);   /* DELAY() is processor dependent */
	mon_reload();
    }

    /*
     * Copy user's bootstrap command line, if any, for later use.
     * The parsing routines are destructive, so a copy is best.
     */
    buff[0] = 0;
    if (system_loading) {
	strcpy(buff,((struct plb_t *)system_loading)->plb_str);
    }

    /*
     * Do not read configuration memory if in debugging mode so as to leave
     * an escape for screwed up configuration memories.  If networks were not
     * pronounced up by configuration memory, go to network to resolve IP
     * addresses.
     */
    debugmode = (configregister & CFG_DIAGNOSTIC);
    if (debugmode || (configregister & CFG_NOCONFIG)) {
	if (!boot_fromflash(&buff[0]))
	    resolve_all_protocols(); /* Go into setup mode right now. */
    } else {
	nv_configure(PRIV_ROOT);

	/*
	 * If we are a Sapphire, we could have a PCMCIA flash card
	 * with a configuration file on it.  If one of the default
	 * config file names exists on the flash card, use it.
	 */
        if ((!nv_valid) && (cpu_type == CPU_SAPPHIRE)) {
            filetype confg;
            boolean success;

            if (reg_invoke_file_exists(FILE_ACCESS_FLASH, DEFAULT_UNIX_CONFIG_FILE))
            {
                confg.proto = FILE_ACCESS_FLASH;
                confg.filename = NULL;
                setstring(&confg.filename, DEFAULT_UNIX_CONFIG_FILE);
                confg.resetsubr = NULL;
                success = read_config_file(&confg, FALSE, PRIV_ROOT);
                free(confg.filename);
            }
            else if (reg_invoke_file_exists(FILE_ACCESS_FLASH, DEFAULT_DOS_CONFIG_FILE))
            {
                confg.proto = FILE_ACCESS_FLASH;
                confg.filename = NULL;
                setstring(&confg.filename, DEFAULT_DOS_CONFIG_FILE);
                confg.resetsubr = NULL;
                success = read_config_file(&confg, FALSE, PRIV_ROOT);
                free(confg.filename);
            }
        }

	/*
	 * Bring up the interfaces we can to give BOOTP the greatest chance
	 * of finding something
	 */
	if (!nv_valid) {
	    reg_invoke_bad_nvram_if();
	}

	/*
	 * If the configuration is corrupt or incomplete, ask ourselves: 
	 * Are we booting from flash?  Yes?  Let next image deal with it.
	 * Are we netbooting or booting roms?  Yes?  Go into [no]setup().
	 */   
	if (!reg_invoke_autoaddr_okay() &&
	    !boot_fromflash(&buff[0])) {
	    reg_invoke_media_update_attempt();/* if a xx, try 10BaseT case */
	    if (!nv_valid)
#ifdef CPU_SPARC
	        netconfig_enable = FALSE;
#else
		netconfig_enable = TRUE;	/* enable service config */
#endif
	    reg_invoke_autoaddr_load();
	}
    }
    system_configured = TRUE;
    process_set_boolean(watched_system_config, TRUE);

    /*
     * perform post-configuration callbacks
     */
    reg_invoke_system_configured();

    /*
     * Alert all the interfaces that care that we've finally parsed NVRAM and
     * started up
     */
    FOR_ALL_HWIDBS(idb) {
	if (idb->system_configured)
	    (*idb->system_configured)(idb);
    }

    /*
     * If we are boot loading, this process is finished.
     */
    if (system_loading) {
	process_kill(THIS_PROCESS);
    }

    /*
     * Boot our configuration files, and be very persistent, allow recursion.
     * Do name lookup after net config file has been booted in case someone
     * is putting our name in that file.
     */
    if (netconfig_enable && !debugmode) {
	if (!netconfg.responder)
	    (void) configure(&netconfgQ, &netconfg, TRUE);
	(void) whoami();
	if (!hostconfg.responder)
	    (void) configure(&hostconfgQ, &hostconfg, TRUE);
	if (reg_invoke_autoaddr_okay())
	    setup_abort();
    }

    /*
     * While we were netbooting, we were pretty much acting like a host,
     * relying on proxy ARP.  We might now have wierd entries in our ARP
     * cache.  Now that we're alive, flush these possibly bogus entries.
     */
    reg_invoke_ar_cache_clear(NULL);

    /*
     * Announce to the world we have arrived
     */
    systeminit_flag = TRUE;
    process_set_boolean(watched_system_init, TRUE);
    errmsg(&msgsym(RESTART, SYS), version);
    reg_invoke_restarted();

    /*
     * Tell the world we're up
     */
    for (i = 0; i <= (nttylines+nauxlines); i++) {
	if (!(tty = MODEMS[i]))
	    continue;
	/* Don't print hello world message on lines that might be
           running system startup chat script */
	if (tty->vacantsuppressed || (tty->capabilities & NOEXEC) ||
	    (tty->statbits & HASPROC))
	    continue;
	if ((tty->statbits & NOBANNER) == 0) {
	    if (tty->vacantstring)
		logout_banner(tty);
	    else
		ttyprintf(tty,"\07\n\n\nPress RETURN to get started!\n\n");
	    ttyflush(tty);
	}
    }

    /*
     * We are now up, so kill off this process
     */
    process_kill(THIS_PROCESS);
}

/*
 * systeminitBLOCK
 * Block until system has been fully initialized, optionally watching a
 * queue and dumping any packets received on it.
 */

boolean systeminitBLOCK (queuetype *qptr)
{
    paktype *pak;

    if (systeminit_flag)
	return(FALSE);		/* Don't block if system is up */
    while ((pak = p_dequeue(qptr))) {
	datagram_done(pak);	/* Flush any packets received */
    }
    return(TRUE);		/* System not up yet, block */
}

static void start_sysinit (subsystype *subsys)
{
    pid_t pid;

    pid = cfork((forkproc *) init_process, (long)system_loading,
		2000, "Init", startup_ttynum);
    if (pid != NO_PROCESS)
	sysinit_process_started = TRUE;
}

#define INIT_MAJVERSION 2
#define INIT_MINVERSION 0
#define INIT_EDITVERSION 1

SUBSYS_HEADER(INIT, INIT_MAJVERSION, INIT_MINVERSION, INIT_EDITVERSION,
	      start_sysinit, SUBSYS_CLASS_SYSINIT,
	      NULL,
	      NULL);
      
