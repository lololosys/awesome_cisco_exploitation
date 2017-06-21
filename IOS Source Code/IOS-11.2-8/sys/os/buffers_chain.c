/* $Id: buffers_chain.c,v 3.10.10.7 1996/09/15 18:18:54 srane Exp $
 * $Source: /release/112/cvs/Xsys/os/buffers_chain.c,v $
 *------------------------------------------------------------------
 * buffers_chain.c - Parse chains and code for buffer pool commands
 *
 * March 1995, Scott Mackie
 *
 * Copyright (c) 1995-1996,1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: buffers_chain.c,v $
 * Revision 3.10.10.7  1996/09/15  18:18:54  srane
 * CSCdi62516:  buffer changes lost after reload on a serial SMDS interface
 * Branch: California_branch
 *
 * Revision 3.10.10.6  1996/08/28  13:02:51  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.10.10.5  1996/07/30  01:08:54  mbeesley
 * CSCdi55762:  Need to support fast IP multicasting on platforms
 * capable of scatter/gather IE c7200
 * Branch: California_branch
 *
 * Revision 3.10.10.4  1996/07/23  13:27:00  thille
 * CSCdi63329: Centralize logging for getbuffer() and pak_duplicate()
 * failures
 * Branch: California_branch
 * Shamelessly rip off Mr. Katz' code from CSCdi36037.  Provide
 * centralized logging for getbuffer() and pak_duplicate() failures and
 * then rip out the 9 bazillion different messages that where in the
 * various callers to report the failure.
 *
 * Revision 3.10.10.3  1996/05/21  09:58:06  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.10.10.2  1996/04/15  14:58:54  widmer
 * CSCdi47180:  No Single Command to Collect General Router Data
 * Branch: California_branch
 * Add "show tech-support" command
 * Change references to encryption types to uint
 * Change tt_soc to use password_struct
 *
 * Revision 3.10.10.1  1996/03/18  21:29:00  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.5.6.4  1996/03/16  07:20:20  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.5.6.3  1996/03/09  05:08:55  hampton
 * Slim down the kernel of the router.
 * Branch: DeadKingOnAThrone_branch
 *
 * Revision 3.5.6.2  1996/03/07  10:17:48  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.5.6.1  1996/02/20  16:37:07  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.10  1996/02/22  14:36:34  widmer
 * CSCdi49281:  make all parser_extension_request static const
 * make parser_extension_request typedef const
 * make all parser_extension_request static
 *
 * Revision 3.9  1996/02/10  22:10:24  widmer
 * CSCdi48633:  show_buffers_by_parameters() does not init suspend timer
 * correctly
 *
 * Revision 3.8  1996/02/07  16:14:14  widmer
 * CSCdi48285:  Remove function name from bad_parser_subcommand
 *
 * Revision 3.7  1996/02/01  06:07:34  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.6  1996/01/26  22:20:47  smackie
 * Fix subtle infinite-loop problem in pool_periodic(). Tidy up naming for
 * pool item queue inlines. Make all pool list operations interrupt safe
 * out of heightened paranoia. (CSCdi47641)
 *
 * Revision 3.5  1996/01/09  22:42:47  anil
 * CSCdi46567:  show buffers old skips old buffers
 *
 * Revision 3.4  1995/12/17  18:34:11  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.3  1995/11/17  18:44:53  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:17:09  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:43:55  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.10  1995/11/08  21:18:24  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.9  1995/10/11  12:12:33  tli
 * CSCdi41857:  show buffers variables should be unsigned
 *
 * Revision 2.8  1995/09/13  23:49:43  smackie
 * Rework buffer pool resize logic to allow the "huge" pool to be resized
 * for a user even if buffers are outstanding. (CSCdi38912)
 *
 * Revision 2.7  1995/09/06  20:15:47  smackie
 * Add code review comments. (CSCdi37152)
 *
 * Revision 2.6  1995/08/12  07:09:32  smackie
 * Repair various buffer and memory display foibles. (CSCdi37152)
 *
 *   o Unify memory dump code to one simple general purpose routine
 *   o Enhance buffer debugging by extending buffer display commands
 *   o Show all private buffer pools (so that BRI pools show up)
 *
 * Revision 2.5  1995/07/25  06:04:17  irfan
 * CSCdi37609:  show buff alloc dump will dump till infinity
 * some bastard packets don't maintain a link to their pools.
 *
 * Revision 2.4  1995/07/18  06:48:02  hampton
 * Miscellaneous changes to eliminate direct references to the system
 * clock and to convert to always using the timer macros to access timers.
 * [CSCdi37217]
 *
 * Revision 2.3  1995/07/12 20:06:49  smackie
 * Fix stupid if() polarity error that caused NULL dereferences for
 * show buffers alloc. (CSCdi36987).
 *
 * Revision 2.2  1995/07/02  10:50:54  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize os, hes, les, if, and ibm.
 *
 * Revision 2.1  1995/06/07  21:53:43  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include "ttysrv.h"
#include "subsys.h"
#include "logger.h"
#include "exception.h"
#include "config.h"
#include "interface_private.h"
#include "sys_registry.h"
#include "packet.h"
#include "element.h"
#include "parser.h"
#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "../parser/parser_defs_parser.h"
#include "parser_defs_buffers.h"
#include "free.h"
#include "pool.h"
#include "particle.h"
#include "buffers.h"
#include "buffers_private.h"
#include "buffers_inline.h"
#include "../ui/debug.h"
#include "../os/os_debug.h"
#include "../if/network.h"
#include "techsupport.h"

/*
 * Parse chains for buffer exec commands
 */
#define ALTERNATE       NONE
#include	"exec_show_buffers.h"
LINK_POINT(buffer_show_commands, ALTERNATE);
#undef  ALTERNATE

/*
 * Parse chains for buffer config commands
 */
#define ALTERNATE       NONE
#include	"cfg_buffers.h"
LINK_POINT(buffer_config_commands, ALTERNATE);
#undef  ALTERNATE

/*
 * Parse chains for buffer debug commands
 */
#define ALTERNATE       NONE
#include	"exec_debug_buffer.h"
LINK_POINT(buffer_debug_commands, ALTERNATE);
#undef  ALTERNATE

/*
 * Parse chain registration array for buffer commands
 */
static parser_extension_request buffer_chain_init_table[] = {
    { PARSE_ADD_SHOW_CMD, &pname(buffer_show_commands) },
    { PARSE_ADD_CFG_TOP_CMD, &pname(buffer_config_commands) },
    { PARSE_ADD_DEBUG_CMD, &pname(buffer_debug_commands) },
    { PARSE_LIST_END, NULL }
};

static void show_buffers_techsupport (parseinfo *csb)
{
    show_techsupport_command(csb, "show buffers");
}


/*
 * buffer_ui_init
 *
 * Initialize buffer parser support.
 *
 */
static void buffer_ui_init (subsystype *subsys)
{
    parser_add_command_list(buffer_chain_init_table, "buffers");
    reg_add_show_techsupport(show_buffers_techsupport,
			     "show_buffers_techsupport");
}


/*
 * show_pool
 * Show contents of pool.
 */

void show_pool (pooltype *pool)
{
    int need;

    if (pool->flags & POOL_INVISIBLE)
	return;

    printf("%s buffers, %d bytes (total %d, permanent %d",
	   pool->name, pool->size, pool->total, pool->perm);

    need = pool->permtarget - pool->perm;
    if (need > 0)
 	printf(", need %d", need);
    if (need < 0)
	printf(", extra %d", -need);

    printf("):\n     %d in free list (%d min, %d max allowed)\n",
	   pool->free, pool->minfree, pool->maxfree);

    /*
     * for interface pools, indicate that a miss is actually a
     * fallback to its corresponding public buffer pool
     */
    if ((pool->group != POOL_GROUP_PUBLIC) && (pool->fallback != NULL))
        printf("     %u hits, %u fallbacks",
               pool->hit, pool->miss);
    else
        printf("     %u hits, %u misses", pool->hit, pool->miss);

    if (pool->flags & POOL_DYNAMIC) {
	printf(", %u trims, %u created\n", pool->trims, pool->grows);
	printf("     %u failures (%u no memory)",
	       pool->failures, pool->nostorage);
    }
    
    printf("\n");
    
    if (pool->flags & POOL_CACHE)
	printf("     %d max cache size, %d in cache\n",
	       pool->cache_size, pool->cache_end);
    if (pool->cache_item.threshold)
	printf("     %d buffer threshold, %lu threshold transitions\n",
	       pool->cache_threshold, pool->cache_threshold_count);
}

/*
 * show_buffers_statistics
 *
 * Show information for all public and interface pools.
 */

static void show_buffers_statistics (parseinfo *csb)
{
    list_element *element;
    pooltype     *pool;

    automore_enable(NULL);
    printf("\nBuffer elements:\n");
    printf("     %d in free list (%d max allowed)\n",
	   elements, INITELEMENTS);
    printf("     %u hits, %u misses, %u created\n",
	   elementshit, elementsmissed, elementsmade);

    printf("\nPublic buffer pools:\n");
    FOR_ALL_DATA_IN_LIST(&publicpoolQ, element, pool) {
	show_pool(pool);
    }

    if (!LIST_EMPTY(&privatepoolQ)) {
	printf("\nInterface buffer pools:\n");

	FOR_ALL_DATA_IN_LIST(&privatepoolQ, element, pool) {
	    show_pool(pool);
	}
    }

    if (!LIST_EMPTY(&headerpoolQ)) {
	printf("\nHeader pools:\n");

	FOR_ALL_DATA_IN_LIST(&headerpoolQ, element, pool) {
	    show_pool(pool);
	}
    }

    show_particle_statistics(csb);
      
    printf("\n");
    automore_disable();
}

static const char buffer_summary_header[] = "\n\n"
"  Header  DataArea   Pool  Rcnt   Size  Link   Enc     Flags   Input  Output"
"\n";
static const char buffer_summary_format[] = "\n"
"%-8x  %-8x  %-5s  %-4d  %-5d  %-4d  %-4d  %-8x  %-6s  %-6s";

/*
 * show_buffers_summmary_header
 *
 * Print a one line summary header for the buffer given
 */

static void show_buffers_summary_header (paktype *pak)
{
    idbtype  *if_input, *if_output;
    pooltype *pool;
    char     *pool_name, *input_namestring, *output_namestring;
    uchar    *data_area;
    short     refcount;
    ushort    linktype;
    ushort    enctype;
    flags_t   flags;
    ushort    datagramsize;
#ifdef BUFDEBUG
    sys_timestamp inputtime;
    ulong caller_pc;
#endif
    
    leveltype level; 

    pool = pak->pool;
    pool_name = (POOLVALID(pool)) ? pool->name : NULL;

    level = raise_interrupt_level(ALL_DISABLE);

    if_output = pak->if_output;
    output_namestring = "None";
    if (if_output != NULL) {
	if (validmem(if_output) &&
	    validmem(if_output->short_namestring))
	    output_namestring = if_output->short_namestring;
	else
	    output_namestring = "Error!";
    }

    if_input = pak->if_input;
    input_namestring = "None";
    if (if_input != NULL) {
	if (validmem(if_input) &&
	    validmem(if_input->short_namestring))
	    input_namestring = if_input->short_namestring;
	else
	    input_namestring = "Error!";
    }

    data_area    = pak->data_area;
    refcount     = pak->refcount;
    linktype     = pak->linktype;
    enctype      = pak->enctype;
    flags        = pak->flags;
    datagramsize = pak->datagramsize;
#ifdef BUFDEBUG
    inputtime    = pak->inputtime;
    caller_pc    = pak->caller_pc;
#endif

    reset_interrupt_level(level);

    printf(buffer_summary_format,
	   pak, data_area, pool_name, refcount, datagramsize,
	   linktype, enctype, flags, input_namestring, output_namestring); 
		  
#ifdef BUFDEBUG
	    printf ("\nAlloc PC = %8x  Inputtime = %#Ta", caller_pc, inputtime);
#endif 
}
/*
 * show_buffers_full_header
 *
 * Print a full header for the buffer given
 */

static void show_buffers_full_header (paktype *pak)
{
    idbtype  *if_input, *if_output;
    pooltype *pool;
    char     *pool_name, *input_namestring, *output_namestring;
    ushort   *datastart;
    int       pool_size;
    paktype   header;
    leveltype level; 

    /*
     * Copy header whilst protected to preserve buffer sanity
     */
    level = raise_interrupt_level(ALL_DISABLE);
    bcopy(pak, &header, sizeof(paktype));
    reset_interrupt_level(level);

    if_output = header.if_output;
    output_namestring = "None";
    if (if_output != NULL) {
	if (validmem(if_output) &&
	    validmem(if_output->namestring))
	    output_namestring = if_output->namestring;
	else
	    output_namestring = "Error!";
    }

    if_input = header.if_input;
    input_namestring = "None";
    if (if_input != NULL) {
	if (validmem(if_input) &&
	    validmem(if_input->namestring))
	    input_namestring = if_input->namestring;
	else
	    input_namestring = "Error!";
    }

    pool = header.pool;
    pool_name = (POOLVALID(pool)) ? pool->name : "Unknown";
    pool_size = (POOLVALID(pool)) ? pool->size : 0;
    pool_size += BUFFEROVERHEADBYTES;

    printf("\n\nBuffer information for %s buffer at 0x%x",
	   pool_name, pak);
    printf("\n  data_area 0x%x, refcount %d, next 0x%x, flags 0x%x",
	   header.data_area, header.refcount, header.next, header.flags);
    printf("\n  linktype %d (%s), enctype %d (%s), encsize %d, rxtype %d",
	   header.linktype,
	   (header.linktype) ? link2name(header.linktype) : "None",
	   header.enctype,
	   (header.enctype) ? encstring(header.enctype) : "None",
	   header.encsize, header.rxtype);
    printf("\n  if_input 0x%x (%s), if_output 0x%x (%s)",
	   header.if_input, input_namestring,
	   header.if_output, output_namestring);
    printf("\n  inputtime %#Ta, outputtime %#Ta, oqnumber %d",
	   header.inputtime, header.outputtime, header.oqnumber);
    printf("\n  datagramstart 0x%x, datagramsize %u, maximum size %u",
	   header.datagramstart, header.datagramsize, pool_size);
    printf("\n  mac_start 0x%x, addr_start 0x%x, info_start 0x%x",
	   header.mac_start, header.addr_start, header.info_start);
    printf("\n  network_start 0x%x, transport_start 0x%x",
	   header.network_start, header.transport_start);
    printf("\n");
    reg_invoke_print_network_header(header.linktype, &header, &datastart);
}

/*
 * show_buffers_by_parameters
 *
 * For a particular set of properties, run through the list of blocks
 * and print out any buffers found matching the search criteria
 */

static void show_buffers_by_parameters (mempool             *mempool,
					void                *address,
					pooltype            *pool,
					idbtype             *if_input,
					show_buffers_search  search,
					show_buffers_dump    dump)
{
    blocktype *block;
    paktype   *pak;
    char      *end, *start;
    ulong      paksize;
    boolean    loopabort = FALSE;
    sys_timestamp suspend_timer;

    /*
     * Some sanity checking
     */
    if (!mempool)
	return;

    TIMER_STOP(suspend_timer);			/* Intialize suspend timer */

    if (search == SHOW_BUFFERS_SEARCH_ADDRESS)
	block = data_to_block(address);
    else
	block = mempool->memhead;

    automore_enable(NULL);
    
    if (dump == SHOW_BUFFERS_DUMP_NONE)
	printf(buffer_summary_header);
    
    /*
     * Whilst we have a valid search and a non-NULL block pointer
     */
    for ( ;!loopabort && block; block = block->next) {
	/*
	 * Kinda sneaky. If we have an address, it's a oneshot deal. Set
	 * the loopabort flag so we don't loop
	 */
	if (search == SHOW_BUFFERS_SEARCH_ADDRESS)
	    loopabort = TRUE;
	
	/*
	 * Make sure we don't hog too much time walking the mempool list
	 */
	block = block_suspend(mempool, block, &suspend_timer);
	if (!block)
	    break;

	/*
	 * Is this a valid block to consider as a possible packet?
	 */
	if (!validblock_gentle(block, mempool))
	    break;

	/*
	 * Attempt to suspend on a used block that has valid refcount
	 */

	if (!(block->size & MEMINUSE))
	    continue;
	
	/*
	 * Enough already.
	 */
	if (automore_quit())
	    break;
	
	/*
	 * Is this a packet buffer?  Check by the process name
	 */
	if (block->allocator_name != packet_header)
	    continue;
	
	/*
	 * Okay. It's a buffer.
	 */
	pak = (paktype *)block->data_area.data;

	/*
	 * Check to see what sort of buffers we want
	 */
	switch (search) {
	  case SHOW_BUFFERS_SEARCH_ASSIGNED:
	    if (pak->refcount < 1)
		continue;
	    break;

	  case SHOW_BUFFERS_SEARCH_FREE:
	    if (pak->refcount != 0)
		continue;
	    break;
	    
	  case SHOW_BUFFERS_SEARCH_OLD:
	    if (!TIMER_RUNNING(pak->inputtime))
		continue;

	    if (!pak->if_input ||
		CLOCK_IN_INTERVAL(pak->inputtime, ONEMIN))
		continue;
	    break;
	    
	  case SHOW_BUFFERS_SEARCH_POOL:
	    if (pak->pool != pool)
		continue;
	    break;

	  case SHOW_BUFFERS_SEARCH_IF_INPUT:
	    if (if_input && (!(pak->flags & PAK_INPUTQ) ||
			     !(pak->if_input == if_input)))
		continue;
	    break;

	  default:
	    break;
	}

	/*
	 * Decide how much information to display
	 */
	switch (dump) {
	  case SHOW_BUFFERS_DUMP_ALL:
	    if (POOLVALID(pak->pool)) {
		paksize = pak->pool->size + BUFFEROVERHEADBYTES;
	    } else {
		paksize = DEF_HUGEDATA + BUFFEROVERHEADBYTES;
	    }
	
	    /*
	     * We want everything displayed
	     */
	    show_buffers_full_header(pak);
	    printf("\n");

	    start = (uchar *)data_area_to_pakdata(pak->data_area);
	    end   = start + paksize;

	    if (!memory_dump(start, end, FALSE,
			     MEMORY_DUMP_DATA_TYPE_ASCII,
			     (memory_dump_output_t)printf)) {
		loopabort = TRUE;
	    }
	    break;
	    
	  case SHOW_BUFFERS_DUMP_PACKET:
	    /*
	     * We want just the packet data displayed
	     */
	    show_buffers_full_header(pak);
	    printf("\n");
	    
            /*
             * Particle-based systems have pools of pak headers which
             * have no associated buffers.  Don't try to display data
             * if none exists (pak->datagramstart == NULL).
             */
            if (pak->datagramstart) {
	        start = pak->datagramstart;
	        end   = start + pak->datagramsize;
	        if (!memory_dump(start, end, FALSE,
		                 MEMORY_DUMP_DATA_TYPE_ASCII,
		                 (memory_dump_output_t)printf)) {
		    loopabort = TRUE;
	        }
            }
	    break;

	  case SHOW_BUFFERS_DUMP_HEADER:
	    /*
	     * We want just the packet header displayed
	     */
	    show_buffers_full_header(pak);
	    break;

	  case SHOW_BUFFERS_DUMP_NONE:
	  default:
	    /*
	     * All we want is a short'n'sweet summary
	     */
	    show_buffers_summary_header(pak);
	    break;
	}
    }
    printf("\n");
    automore_disable();
}

/*
 * show_buffers
 *
 * Handle the display of various buffer related statistics and information
 */

void show_buffers (parseinfo *csb)
{
    show_buffers_search  search;
    show_buffers_dump    dump;
    idbtype             *if_input;
    void                *address;
    pooltype            *pool;
    mempool             *mempool;

    search   = GETOBJ(int,1);
    dump     = GETOBJ(int,2);
    address  = (void *)GETOBJ(int,3);
    if_input = GETOBJ(idb,1);
    pool     = pak_pool_find_by_name(GETOBJ(string,1));

    switch (search) {
      case SHOW_BUFFERS_SEARCH_NONE:
	show_buffers_statistics(csb);
	break;
	
      case SHOW_BUFFERS_SEARCH_ASSIGNED:
      case SHOW_BUFFERS_SEARCH_FREE:
      case SHOW_BUFFERS_SEARCH_IF_INPUT:
      case SHOW_BUFFERS_SEARCH_OLD:
      case SHOW_BUFFERS_SEARCH_POOL:
      case SHOW_BUFFERS_SEARCH_ALL:
	mempool = mempool_find_by_class(MEMPOOL_CLASS_LOCAL);
	if (!mempool)
	    return;
	show_buffers_by_parameters(mempool, address, pool,
				   if_input, search, dump);
	if (mempool->alternate) {
	    mempool = mempool->alternate;
	    show_buffers_by_parameters(mempool, address, pool,
				       if_input, search, dump);
	}
	break;

      case SHOW_BUFFERS_SEARCH_ADDRESS:
	mempool = mempool_find_by_addr(address);
	if (!mempool)
	    return;
	show_buffers_by_parameters(mempool, address, NULL, NULL, search, dump);
	break;

      default:
	bad_parser_subcommand(csb, search);
	break;
    }
}


/*
 * buffers_command
 * Set pool parameters.
 * 	[no] buffers (small|medium|big|verybig|large|huge) 
 *		     (permanent|min-free|max-free|initial) n
 *      [no] buffers huge size n
 */
/*
 * buffers_command
 * Set pool parameters.
 * 	[no] buffers (small|medium|big|verybig|large|huge) 
 *		     (permanent|min-free|max-free|initial) n
 *      [no] buffers huge size n
 */
void buffers_command (parseinfo *csb)
{
    int i, size;
    pooltype *pool;
    idbtype *swidb;
    hwidbtype *hwidb;

    /*
     * Ignore the command if we are booting.
     */
    if (system_loading) {
	return;
    }

    swidb = GETOBJ(idb,1);
    hwidb = swidb ? swidb->hwptr : NULL;

    if (hwidb) {
	/*
	 * check if an interface buffer pool exists
	 */
	pool = hwidb->pool;
	if (!pool)
	    return;
    } else {
      switch (GETOBJ(int,1)) {
         case BUF_CMD_SMALL:
	   pool = small;
	   break;
	 case BUF_CMD_MIDDLE:
	   pool = middle;
	   break;
	 case BUF_CMD_BIG:
	   pool = big;
	   break;
         case BUF_CMD_VERYBIG:
           pool = verybig;
           break;
	 case BUF_CMD_LARGE:
	   pool = large;
	   break;
	 case BUF_CMD_HUGE:
	   pool = huge;
	   break;
	 default:
	   bad_parser_subcommand(csb, GETOBJ(int,1));
	   return;
	 }
    }
    if (csb->nvgen) {
	switch (GETOBJ(int,2)) {
	  case BUF_CMD_PERM:
	    nv_write((pool->userflags & POOL_USER_PERM),
		     "%s %d", csb->nv_command,
		     pool->permtarget);
	    break;
	  case BUF_CMD_MAXFREE: 
	    nv_write((pool->userflags & POOL_USER_MAX),
		     "%s %d", csb->nv_command,
		     pool->maxfree);
	    break;
	  case BUF_CMD_MINFREE:
	    nv_write(pool->userflags & POOL_USER_MIN,
		     "%s %d", csb->nv_command,
		     pool->minfree);
	    break;
	  case BUF_CMD_INIT:
	    nv_write(pool->init_tmp,
		     "%s %d", csb->nv_command,
		     pool->init_tmp);
	    break;
	  case BUF_CMD_SIZE:
	    nv_write((huge->size != DEF_HUGEDATA),
		     "%s %d", csb->nv_command,
		     huge->size);
	    break;
	  default:
	    bad_parser_subcommand(csb, GETOBJ(int,2));
	    break;
	}
	return;
    }

    switch (GETOBJ(int,2)) {
	case BUF_CMD_PERM:
	    if (csb->sense) {
		pool->userflags |= POOL_USER_PERM;
	    } else {
		GETOBJ(int,3) = pool->defperm;
		pool->userflags &= ~POOL_USER_PERM;
	    }
	    pool_adjust(pool, pool->minfree, pool->maxfree,
			GETOBJ(int,3), FALSE);
	    break;
	case BUF_CMD_MAXFREE:
	    if (csb->sense) {
		pool->maxfree = GETOBJ(int,3);
		pool->userflags |= POOL_USER_MAX;
	    } else {
		pool->maxfree = pool->defmaxfree;
		pool->userflags &= ~POOL_USER_MAX;
	    }
	    break;
	case BUF_CMD_MINFREE:
	    if (csb->sense) {
		pool->minfree = GETOBJ(int,3);
		pool->userflags |= POOL_USER_MIN;
	    } else {
		pool->minfree = pool->defminfree;
		pool->userflags &= ~POOL_USER_MIN;
	    }
	    break;
	case BUF_CMD_INIT:
	    if (csb->sense) {
		pool->init_tmp = GETOBJ(int,3);
		if (!systeminit_flag) {
		    void * item;

		    for (i = 0; i < pool->init_tmp; i++) {
			item = (*pool->item.create)(pool,
						    POOL_ITEM_TYPE_TEMPORARY);
			if (!item)
			    break;
			pool_enqueue_item(pool, item,
					  POOL_ITEM_TYPE_TEMPORARY);
		    }
		}
	    } else {
		pool->init_tmp = 0;
	    }
	    break;
	case BUF_CMD_SIZE:
	    if (csb->sense) {
		size = max(GETOBJ(int,3), DEF_HUGEDATA);
	    } else {
		size = DEF_HUGEDATA;
	    }
	    if (huge->size != size) {
		if (size != huge->size) {
		    leveltype level;
		    
		    level = raise_interrupt_level(ALL_DISABLE);
		    pool  = pool_resize(huge, size, huge->defminfree,
                                        huge->defmaxfree, huge->defperm);
		    if (pool)
			huge = pool;
		    else
			printf("\n%%Unable to change pool size to %d", size);
		    reset_interrupt_level(level);
		}
	    }
	    break;
	  default:
	    bad_parser_subcommand(csb, GETOBJ(int,2));
	    break;
    }
}


/*
 * show_particle_statistics
 *
 * Show information for all particle pools.
 */

void show_particle_statistics (parseinfo *csb)
{
    list_element *element;
    pooltype     *pool;

    if (cloneQ.count) {
	printf("\nParticle Clones:\n");
        printf("     %u clones, %u hits, %u misses\n",
	       cloneQ.count, clone_hits, clone_misses);
    }
    if (!LIST_EMPTY(&particlepublicQ)) {
	printf("\nPublic particle pools:\n");

	FOR_ALL_DATA_IN_LIST(&particlepublicQ, element, pool) {
	    show_pool(pool);
	}
    }

    if (!LIST_EMPTY(&particleprivateQ)) {
	printf("\nPrivate particle pools:\n");
	
	FOR_ALL_DATA_IN_LIST(&particleprivateQ, element, pool) {
	    show_pool(pool);
	}
    }
}


/*
 * show_getbuffer_fail
 *
 * show buffers failures exec command.
 */
void show_getbuffer_fail (parseinfo *csb)
{
    int entry_number;
    getbuffer_failures *failptr;

    printf("\nCaller       Pool          Size      When");

    /* Go oldest to newest. */

    entry_number = getbuffer_failure_trace_number;
    while (TRUE) {
	failptr = &getbuffer_failure_trace[entry_number];
	if (failptr->calling_pc) {	/* If active */
	    printf("\n%#10x  %11s %-8d    %TE", failptr->calling_pc,
		   failptr->pool->name, failptr->size, failptr->failtime);
	}
	entry_number = ((entry_number + 1) %
			GETBUFFER_FAILURE_RECORD_COUNT);
	if (entry_number == getbuffer_failure_trace_number) /* Done */
	    break;
    }
}


/*
 * buffer_ui subsystem header
 */
 
#define BUFFER_UI_MAJVERSION    1
#define BUFFER_UI_MINVERSION    0
#define BUFFER_UI_EDITVERSION   1
 
SUBSYS_HEADER(buffer_ui, BUFFER_UI_MAJVERSION, BUFFER_UI_MINVERSION,
              BUFFER_UI_EDITVERSION,
              buffer_ui_init, SUBSYS_CLASS_MANAGEMENT,
              NULL, NULL);

