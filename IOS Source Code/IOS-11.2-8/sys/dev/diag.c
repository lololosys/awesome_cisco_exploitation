/* $Id: diag.c,v 3.2.60.2 1996/08/12 16:01:08 widmer Exp $
 * $Source: /release/112/cvs/Xsys/dev/diag.c,v $
 *------------------------------------------------------------------
 * diag.c -- System Diagnostic Routines
 *
 * 5-February-1989, Kirk Lougheed
 *
 * Copyright (c) 1989-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 * These routines are primarily intended for the manufacturing process.
 * They need to return very simple, yes/no results for the operator.
 * Keep this in mind when adding additional functionality.
 *------------------------------------------------------------------
 * $Log: diag.c,v $
 * Revision 3.2.60.2  1996/08/12  16:01:08  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.2.60.1  1996/03/18  19:12:20  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.3  1996/03/13  02:35:29  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.2.26.2  1996/03/07  08:42:55  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.2.26.1  1996/02/20  13:45:22  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.2  1995/11/17  09:00:05  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:18:32  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/08/08  16:45:54  hampton
 * Miscellaneous changes to eliminate direct references to the system
 * clock and to convert to always using the timer macros to access timers.
 * [CSCdi38391]
 *
 * Revision 2.2  1995/06/09 12:59:48  hampton
 * Rename old scheduler calls to fit the new naming convention.
 * [CSCdi35514]
 *
 * Revision 2.1  1995/06/07 20:24:01  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <string.h>
#include "interface_private.h"
#include "packet.h"
#include "../ip/ip.h"
#include "ttysrv.h"
#include "logger.h"
#include "../os/free.h"
#include "../os/region.h"
#include "../if/network.h"
#include "../os/nv.h"
#include "../dev/monitor1.h"
#include "registry.h"
#include "../ip/ip_registry.h"
#include "../h/sys_registry.h"
#include "../ui/command.h"
#include "config.h"
#include "../os/init.h"


/*
 * externs
 */
extern void test_parity_main(volatile uint *);
extern void test_parity_io(void);
extern void test_addr_err(void);
extern void test_jump_zero(void);


static int ping_test(idbtype *, int, int);


/*
 * test_memory
 * Run simple memory test on a section of memory.
 * Argument is pointer to base address and byte count.
 * Returns TRUE if passes test, FALSE otherwise.
 */

static boolean test_memory (ushort *base, int size)
{
    int i;
    ushort *ptr;

    size = btow(size);

    /*
     * Test for all zeros
     */
    for (i = 0, ptr = base; i < size; i++) {
	*ptr++ = 0;
    }
    for (i = 0, ptr = base; i < size; i++) {
	if (*ptr++ != 0) {
	     return(FALSE);
	 }
    }

    /*
     * Test for all ones
     */
    for (i = 0, ptr = base; i < size; i++) {
	*ptr++ = 0xFFFF;
    }
    for (i = 0, ptr = base; i < size; i++) {
	if (*ptr++ != 0xFFFF) {
	     return(FALSE);
	 }
    }

    /*
     * Test for address+1
     */
    for (i = 0, ptr = base; i < size; i++, ptr++) {
	*ptr = (ushort)((ulong)ptr + 1);
    }
    for (i = 0, ptr = base; i < size; i++, ptr++) {
	if (*ptr != (ushort)((ulong)ptr + 1)) {
	     return(FALSE);
	 }
    }
    return(TRUE);
}

#ifdef CSC2
#define NV_IDBP		nv_idbp
#else
#define NV_IDBP		NULL
#endif
#define MULTIMEMTEST_MAGIC	0x03191985

/* 
 * test_NVandMB_memory
 * Test non-volatile and multibus memory
 */

void test_NVandMB_memory (void)
{
    mempool *multibus_mempool = NULL;
    regiontype  *multibus_region = NULL;
    nvtype  *nv;

    if (nvsize && !NV_IDBP) {
	clearinput(stdio);
	printf("\nTest NVRAM card [y/n] ? ");
	if (doit()) {
	    nv = nv_getptr();
	    if (nv == NULL) {
		printf("\nNVRAM in use");
	    } else { 
		printf(test_memory((ushort *)nv, nvsize) ?
		       "Passed" : "Failed");
		nv_done(nv);
	    }
	}
    }

    multibus_region  = region_find_by_class(REGION_CLASS_MULTIBUS);
    multibus_mempool = mempool_find_by_class(MEMPOOL_CLASS_MULTIBUS);
    if ((multibus_region) && (multibus_mempool)) {
	clearinput(stdio);
	printf("\nTest Multibus memory card [y/n] ? ");
	if (doit()) {
	    printf("Warning: Software will need to be reloaded after test."
		   "  Continue [y/n] ? ");
	    if (doit()) {
		multibus_mempool->memtest = MULTIMEMTEST_MAGIC;
		printf(test_memory((ushort *)multibus_region->start,
				   multibus_region->size)
		       ? "Passed" :"Failed");  
		printf("\nYou should now reboot the system.");
		return;
	    }
	}
    }
    if (!(nvsize && !NV_IDBP) && !(multibus_mempool)) {
	/* nothing to test */
	printf("\n%% Nothing to test");
    }
}

/*
 * test_specific_interface
 * Collect data for the generic interface after pinging itself.
 */
void test_specific_interface (hwidbtype *idb, int *fail, int *pass, int *skip)
{
    int drops;
    ulong ierrors, oerrors;
    char *reason = NULL;
	
    ierrors = idb->counters.input_error;
    oerrors = idb->counters.output_error;
    drops = ping_test(idb->firstsw, 94, 100);
    
    if (drops == 0) {
	if (ierrors != idb->counters.input_error) 
	    reason = "reception";
	else if (oerrors != idb->counters.output_error) 
	    reason = "transmission";
	else 
	    reason = NULL;
    } else {
	reason = hw_interface_up(idb) ? "timeout" : "not connected";
    }
    if (reason) {
	printf(" Failed - %s problem", reason);
	(*fail)++;
    } else {
	printf(" Passed");
	(*pass)++;
    }
}



/*
 * test_interfaces
 * Run basic functionality tests on network interfaces.
 *
 */
void test_interfaces (parseinfo *csb)
{    
    int ntest, nfailed, npassed, nnotest, nskipped;
    static boolean first_time = TRUE;
    hwidbtype *idb;

    if (init_errors) {
        printf("\nThere were card initialization errors. Test Aborted.");
        return;
    }
    ntest = 0;    /* no of tested interfaces */
    nfailed = 0;
    npassed = 0;
    nnotest = 0;
    nskipped = 0;

    if (first_time) {
	/*
	 * Register default test interfaces routines.
	 */
	reg_add_default_test_specific_interface(test_specific_interface,
						"test_specific_interface");
	first_time = FALSE;
    }

    idb = hwidb_or_null(GETOBJ(idb,1));
    
    /*
     * If no interface number is entered in `test interface' command, test
     * all interfaces on the router. Otherwise only test that interface.
     */
    if (idb == NULL) {
        FOR_ALL_HWIDBS(idb) {
            ntest++;
            if (idb->firstsw->ip_address == 0L) {
                printf("\nNo IP address for %s.\nSkipping...", idb->hw_namestring);
                nnotest++;
                continue;
            }
            clearinput(stdio);
            printf("\nTest %s [y/n] ? ", idb->hw_namestring);
            if (!doit()) {
                nskipped++;
                continue;
            }
	    if (reg_invoke_test_interfaces_setup(idb->type, idb) == -1) {
		nfailed++;
		continue;
	    }
	    reg_invoke_test_specific_interface(idb->type, idb, &nfailed,
					       &npassed, &nskipped);
	}
    } else {
	ntest++;
	if (idb->firstsw->ip_address == 0L) {
	    printf("\nNo IP address for %s.\nSkipping...",
		   idb->hw_namestring);
	    nnotest++;
	} else {
	    if (reg_invoke_test_interfaces_setup(idb->type, idb) == -1) 
		nfailed++;
	    else 
	       reg_invoke_test_specific_interface(idb->type, idb, &nfailed,
					       &npassed, &nskipped);
	}
    }
    
    /*
     * Summarize results
     */
    printf("\n%d interfaces: %d passed, %d failed, %d skipped, %d untestable", ntest, npassed, nfailed, nskipped, nnotest);
}


/*
 * ping_test
 * Send a number of ICMP Echos out an interface and return count
 * of failures.  Prints a '!' on stdio on success, '.' on failure.
 * Give up after three failures.
 *
 * Alternate between even and odd length packets, gradually increasing
 * the length of the packet each time.
 */
static int ping_test (idbtype *idb, int size, int repeat)
{
    echotype data;
    int fail, i;

    memset(&data, 0, sizeof(echotype));

    for (i = 0, fail = 0; ((i < repeat) && (fail < 3)); i++) {
	data.id = i;
	TIMER_START(data.timeout, ONESEC);
	if (reg_invoke_ip_send_echo(idb->ip_address, &data, size, 0xABCD,
				    idb->ip_address, FALSE, NULL)) {
	    if (data.how != '!') {
		fail++;
	    }
	    printc(data.how);
	} else {
	    printc('.');
	    fail++;                             
	}
    }
    return(fail);
}


/*
 * test_crash
 * Cause specific crashes or exceptions while in system image
 * to test ROM monitor and system image crash information code:
 */

enum CRASH {
    CRASH_INVALID_ADDR,		/* Bus error from Invalid address */
    CRASH_PARITY_ERR_MAIN,	/* Main memory parity error */
    CRASH_PARITY_ERR_IO,	/* I/O memory parity error */
    CRASH_ADDRESS_ERR,		/* Address error */
    CRASH_JUMP_ZERO,		/* Jump to zero */
    CRASH_FORCE_CRASH,		/* Software forced crash */
    CRASH_READ_ZERO,		/* Illegal read of address zero */
    CRASH_DIV_ZERO,		/* Divide by zero */
    CRASH_CORRUPT_MEMORY,	/* Corrupt memory */
    /* Add more here */
    CRASH_EXIT,			/* Exit the crash menu */
    CRASH_LAST
};

static struct diag_menu_choice {
    char ch;
    enum CRASH type;
    char *desc;
} crash_menu[] = {
    { '1', CRASH_INVALID_ADDR, "Bus Error, due to invalid address access" },
    { '2', CRASH_PARITY_ERR_MAIN, "Bus Error, due to parity error in Main memory" },
    { '3', CRASH_PARITY_ERR_IO, "Bus Error, due to parity error in I/O memory" },
    { '4', CRASH_ADDRESS_ERR, "Address Error, due to fetching code from odd address" },
    { '5', CRASH_JUMP_ZERO, "Jump to zero" },
    { '6', CRASH_FORCE_CRASH, "Software forced crash" },
    { '7', CRASH_READ_ZERO, "Illegal read of address zero" },
    { '8', CRASH_DIV_ZERO, "Divide by zero" },
    { '9', CRASH_CORRUPT_MEMORY, "Corrupt memory" },
    { 'x', CRASH_EXIT, "Exit crash menu" },
    { 0, CRASH_LAST, NULL }
};

static void print_crash_menu (void)
{
    int i;

    printf("\n\nType the number for the selected crash:"
	   "\n--------------------------------------");
    for (i = 0; crash_menu[i].type != CRASH_LAST; i++) {
	printf("\n %c  %s", crash_menu[i].ch, crash_menu[i].desc);
    }
    printf("\n? ");
}

static void test_corrupt_memory (void)
{
    mempool *mempool;

    mempool = mempool_find_by_class(MEMPOOL_CLASS_LOCAL);
    mempool->memhead->blockmagic = 0xdeadbeef;
    printf("\n(The system will reload when the heap checker runs.)");
}

void test_crash (void)
{
    int selection, i;
    volatile uint *address;
    volatile uint data;
    leveltype level;

    logger_flush();		/* Flush any accumulated log messages */

    while (1) {
	print_crash_menu();
	flush();
	selection = getcwait(stdio);
	if (selection < 0)
	    return;

	for (i = 0; crash_menu[i].type != CRASH_LAST; i++) {
	    if (selection == crash_menu[i].ch)
		break;
	}
	printf("\n");
	flush();
	process_sleep_for(100);		/* let output out */
	if (crash_menu[i].type == CRASH_LAST) {
	    printf("Bad selection %c.\n", selection);
	    continue;
	}

	switch (crash_menu[i].type) {
	case CRASH_INVALID_ADDR:
	    /*
	     * Create a Local bus timeout error by accessing an
	     * invalid memory address:
	     */
	    address = (volatile uint *)BAD_ADDRESS;
	    printf("Creating a Local bus timeout error by "
		   "reading from address %#x...\n", address);
	    flush();
	    process_sleep_for(100);	/* let output out */
	    
	    level = raise_interrupt_level(ALL_DISABLE);
	    data = *address;	/* Access the illegal address */
	    reset_interrupt_level(level);
	    break;
	
	case CRASH_PARITY_ERR_MAIN:
	    /*
	     * Generate a RAM Parity error in Main memory via emt call.
	     * EMT_WRITE_BAD_MAIN_MEMORY_PARITY will:
	     *  o Set processor to generate even parity (odd by default).
	     *  o Write data with even parity.
	     *  o Set processor to generate and check odd parity.
	     *
	     * Then, we read the data with even parity while checking for
	     * odd parity to cause a parity error.
	     */
	    test_parity_main((volatile uint *)0x40004);
	    break;
	    
	case CRASH_PARITY_ERR_IO:
	    /*
	     * Generate a RAM Parity error in IO memory:
	     *  o Set processor to generate even parity (odd by default).
	     *  o Write data with even parity.
	     *  o Set processor to generate and check odd parity.
	     *  o Read the data with even parity while checking for odd
	     *    parity to cause a parity error.
	     */
	    test_parity_io();
	    break;

	case CRASH_ADDRESS_ERR:
	    test_addr_err();
	    break;

	case CRASH_JUMP_ZERO:
	    test_jump_zero();
	    break;
	    
	case CRASH_FORCE_CRASH:
	    printf("Causing a software forced crash...\n");
	    flush();
	    process_sleep_for(100);	/* let output out */

	    crashpoint();
	    break;

	case CRASH_READ_ZERO:
	    printf("Reading from address zero...\n");
	    flush();
	    process_sleep_for(100);	/* let output out */
	    
	    address = (uint *)0;
	    data = *address;
	    printf("Address zero returned : %#x\n", data);
	    break;

	case CRASH_DIV_ZERO:
	    data = 0;
	    printf("Causing a divide-by-zero exception...\n");
	    flush();
	    process_sleep_for(100);	/* let output out */

	    data = 0x1234 / data;
	    break;

	case CRASH_CORRUPT_MEMORY:
	    test_corrupt_memory();
	    break;

	case CRASH_EXIT:
	    return;

	default:
	    break;
	}
    }
}

/* End of file */
