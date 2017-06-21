/* $Id: alignment_chain.c,v 3.4.10.3 1996/06/17 23:36:49 hampton Exp $
 * $Source: /release/112/cvs/Xsys/src-4k/alignment_chain.c,v $
 *------------------------------------------------------------------
 * alignment_chain.c - contructs and "connects" alignment parser cmds
 *
 * April 1994, Jim Hayes
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: alignment_chain.c,v $
 * Revision 3.4.10.3  1996/06/17  23:36:49  hampton
 * Implement the long sought parser "default" keyword.  [CSCdi60510]
 * Branch: California_branch
 *
 * Revision 3.4.10.2  1996/05/21  10:03:06  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.4.10.1  1996/03/18  21:57:49  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.14.3  1996/03/16  07:32:35  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.3.14.2  1996/03/09  05:11:40  hampton
 * Slim down the kernel of the router.
 * Branch: DeadKingOnAThrone_branch
 *
 * Revision 3.3.14.1  1996/02/20  17:49:42  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.4  1996/02/22  14:38:05  widmer
 * CSCdi49281:  make all parser_extension_request static const
 * make parser_extension_request typedef const
 * make all parser_extension_request static
 *
 * Revision 3.3  1995/12/17  18:37:36  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.2  1995/11/17  18:40:28  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:20:15  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:52:50  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include <ciscolib.h>
#include "subsys.h"
#include "config.h"
#include "parser.h"
#include "gdb_4k.h"
#include "alignment.h"
#include "stacks.h"
#include "../parser/parser_actions.h"
#include "../parser/macros.h"

/*
 * External references
 */
extern boolean alignment_activated;
extern alignment_t *align;
extern const align_access_t align_access[];

static void show_context_command (parseinfo *);

/*
 * Include the chain of "service", "show" and "test" commands...
 */

#include "alignment_parser_defs.h"
#include "alignment_chain.h"

/*
 * Parse chain registration array for alignment commands
 */
static parser_extension_request alignment_chain_init_table[] = {
    { PARSE_ADD_CFG_SERVICE_CMD, &pname(service_alignment_commands) },
    { PARSE_ADD_SHOW_CMD, &pname(show_alignment_command) },
    { PARSE_ADD_SHOW_CMD, &pname(context_show_commands) },
    { PARSE_ADD_TEST_CMD, &pname(test_alignment_command) },
   { PARSE_LIST_END, NULL }
};

/*
 * alignment_parser_init - Initialize alignment parser support
 */
void alignment_parser_init (void)
{
    parser_add_command_list(alignment_chain_init_table, "alignment commands");
}

/*
 * alignment_test
 *
 * Test MIPS mis-alignment handler
 */

#define SHORT_ALIGNED             0x0123
#define SHORT_MISALIGNED          0x2345
#define TEST_SHORT                0xa5a5
#define INT_ALIGNED               0x01234567
#define INT_BYTE_MISALIGNED       0x23456789
#define TEST_INT_BYTE             0xbeefcafe
#define INT_WORD_MISALIGNED       0x456789ab
#define TEST_INT_WORD             0x12121212

static const char test_ok[] = "OK\n";
static const char test_failed[] = "failed\n";
static const char test_str[] = "Testing %s access to %s address... ";

static void alignment_test(void)
{
    uchar buf[16];
    int i;

    /*
     * Test byte aligned unsigned shorts
     */
    for (i=0; i<16; i++)
	buf[i] = ((i*2)<<4) + ((i*2)+1);

    printf(test_str, "short", "byte-aligned");

    if ((*(ushort *)(&buf[0]) != SHORT_ALIGNED) ||
	(*(ushort *)(&buf[1]) != SHORT_MISALIGNED) ||
	(*(ushort *)(&buf[1]) = TEST_SHORT,
	 *(ushort *)(&buf[1]) != TEST_SHORT)
	) {
	printf(test_failed);
	return;
    } else {
	printf(test_ok);
    }

    /*
     * Test byte aligned ints
     */
    for (i=0; i<16; i++)
	buf[i] = ((i*2)<<4) + ((i*2)+1);

    printf(test_str, "int", "byte-aligned");

    if ((*(int *)(&buf[0]) != INT_ALIGNED) ||
	(*(int *)(&buf[1]) != INT_BYTE_MISALIGNED) ||
	(*(int *)(&buf[1]) = TEST_INT_BYTE,
	 *(int *)(&buf[1]) != TEST_INT_BYTE)
	) {
	printf(test_failed);
	return;
    } else {
	printf(test_ok);
    }

    /*
     * Test 16bit aligned ints
     */
    for (i=0; i<16; i++)
	buf[i] = ((i*2)<<4) + ((i*2)+1);

    printf(test_str, "int", "short-aligned");

    if ((*(int *)(&buf[0]) != INT_ALIGNED) ||
	(*(int *)(&buf[2]) != INT_WORD_MISALIGNED) ||
	(*(int *)(&buf[2]) = TEST_INT_WORD,
	 *(int *)(&buf[2]) != TEST_INT_WORD)
	) {
	printf(test_failed);
	return;
    } else {
	printf(test_ok);
    }

    /*
     * Test read cycles to an aligned address half way between 0 and
     * the ALIGN_ADDR_LIMIT literal
     */
    i = (ALIGN_ADDR_LIMIT / 2) & ~0x03;

    printf(test_str, "byte", "low");
    if (*(volatile uchar *)(i) != 0 ||
	*(volatile char *)(i) != 0) {
	printf(test_failed);
	return;
    } else {
	printf(test_ok);
    }

    printf(test_str, "short", "low");
    if (*(volatile ushort *)(i) != 0 ||
	*(volatile short *)(i) != 0) {
	printf(test_failed);
	return;
    } else {
	printf(test_ok);
    }

    printf(test_str, "long", "low");
    if (*(volatile ulong *)(i) != 0 ||
	*(volatile long *)(i) != 0) {
	printf(test_failed);
	return;
    } else {
	printf(test_ok);
    }
}

/*
 * print_unaligned_summary 
 *
 * Display the unaligned trap data base
 */

static void print_unaligned_summary(void)
{
    alignment_entry_t *ptr;
    int i;

    printf("\n\nTotal Corrections %d, Recorded %d, Reads %d, Writes %d\n",
	   align->total, align->recorded, align->reads, align->writes);

    printf("\n%-8s%-16s", "Initial", "Initial");
    printf("\n%s%s%s", " Address  Count", "  Access Type ", "  Traceback");
    for (i=0; i < align->tail; i++) {
	int ix;

	ptr = &align->table[i];
	printf("\n%-8x%-7u%-4ubit %6s ",
	       ptr->addr, ptr->count,
	       16 << align_access[ptr->access_type].shift16,
	       (align_access[ptr->access_type].read) ? "read " : "write");
	for (ix = 0; ix < ALIGN_FRAMES; ++ix) {
	    if (ptr->trace.pc[ix] == 0)
		break;
	    printf(" 0x%08x", ptr->trace.pc[ix]);
	}
    }
}

/*
 * print_null_summary:
 * Print out summary of Null pointer reads
 */
static void
print_null_summary (void)
{
    alignment_entry_t *ptr;
    int i;
    
    printf("\n\nTotal Spurious Accesses %d, Recorded %d\n", align->nulltotal,
	   align->nullrecorded);

    printf("\n%s%s", " Address  Count", "  Traceback");
    for (i=0; i < align->nulltail; i++) {
	int ix;

	ptr = &align->nulltable[i];
	printf("\n%-8x%-7u ", ptr->addr, ptr->count);
	for (ix = 0; ix < ALIGN_FRAMES; ++ix) {
	    if (ptr->trace.pc[ix] == 0)
		break;
	    printf(" 0x%08x", ptr->trace.pc[ix]);
	}
    }
}

/*
 * show_alignment
 *
 * Printout the mis-aligned data base
 */

void show_alignment (parseinfo *csb)
{
    if (!alignment_activated) {
	printf("%% Alignment detection is currently disabled.\n");
	return;
    }

    automore_enable(NULL);
    mem_lock(align);

    if (align->tail || align->nulltail) {
	printf("Alignment data for:\n%s", oemversion);
    }

    if (!align->tail) {
	printf("\n\nNo alignment data has been recorded.");
    } else {
	print_unaligned_summary();
    }

    if (!align->nulltail) {
	printf("\n\nNo spurious memory references have been recorded.\n");
    } else {
	print_null_summary();
    }
    free(align);
    automore_disable();
}

/*
 * show context:  Calls fault_history_display() to display the saved
 * register context from NVRAM.  Also get the reason why we restarted
 * from stack_whyreload().
 */

static void
show_context_command (parseinfo *csb)
{
    stack_whyreload();
    fault_history_display();
}


/*
 * alignment_command
 *
 * Parser-hook for alignment commands
 */

void alignment_command (parseinfo *csb)
{
    if (!csb->nvgen) {
	switch (csb->which) {
	case ALIGN_TEST:
	    alignment_test();
	    break;
	    
	case ALIGN_ENABLE:
	    if (csb->set_to_default || csb->sense)
		alignment_handler_enable();
	    else
		alignment_handler_disable();
	    break;
	    
	case ALIGN_LOGGING:
	    if (csb->set_to_default)
		align->logging = ALIGN_LOG_DEFAULT;
	    else
		align->logging = csb->sense;
	    break;
	}
    } else {
	switch (csb->which) {
	case ALIGN_ENABLE:
	    if (!alignment_activated) 
		nv_write(TRUE, "no %s", csb->nv_command);
	    break;
	    
	case ALIGN_LOGGING:
	    if (alignment_activated) {
		nv_write(!align->logging, "no %s", csb->nv_command);
	    }
	    break;
	}
    }
}

static void alignment_chain_subsys_init(subsystype* subsys)
{
    alignment_parser_init();
}

#define ALIGN_CHAIN_MAJOR_VERSION 1
#define ALIGN_CHAIN_MINOR_VERSION 0
#define ALIGN_CHAIN_EDIT_VERSION 2

SUBSYS_HEADER(alignment_chain,
	      ALIGN_CHAIN_MAJOR_VERSION, ALIGN_CHAIN_MINOR_VERSION,
	      ALIGN_CHAIN_EDIT_VERSION,
	      alignment_chain_subsys_init, SUBSYS_CLASS_KERNEL,
	      NULL, NULL);
