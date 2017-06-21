/* $Id: exception.c,v 3.11.4.4 1996/08/28 13:03:08 thille Exp $
 * $Source: /release/112/cvs/Xsys/os/exception.c,v $
 *------------------------------------------------------------------
 * exception.c - System exception handler
 *  
 * October 1, 1990, Kevin Paul Herbert
 *
 * Copyright (c) 1990-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: exception.c,v $
 * Revision 3.11.4.4  1996/08/28  13:03:08  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.11.4.3  1996/07/10  22:22:00  dstine
 * CSCdi62471:  Superfluous #include files in atalk, atm, os files
 * Branch: California_branch
 *
 * Revision 3.11.4.2  1996/06/28  23:24:40  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.11.4.1  1996/03/18  21:29:30  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.6.2.4  1996/03/16  07:20:25  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.6.2.3  1996/03/09  05:09:14  hampton
 * Slim down the kernel of the router.
 * Branch: DeadKingOnAThrone_branch
 *
 * Revision 3.6.2.2  1996/03/07  10:18:15  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.6.2.1  1996/02/20  16:38:18  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.11  1996/03/09  22:56:31  widmer
 * CSCdi47890:  HTTP server doesnt work
 * Add HTTP server
 *
 * Revision 3.10  1996/02/12  18:18:21  abaindur
 * CSCdi48693:  show process mem does not include alternate mempool mallocs
 *
 * Revision 3.9  1996/02/07  16:14:22  widmer
 * CSCdi48285:  Remove function name from bad_parser_subcommand
 *
 * Revision 3.8  1996/01/31  04:24:16  hampton
 * Fix router so that writes a core file instead of hanging.  [CSCdi47877]
 *
 * Revision 3.7  1996/01/24  20:39:39  jenny
 * CSCdi47046:  TCP/IP need code clean up (part 3)
 *
 * Revision 3.6  1996/01/22  06:56:06  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.5  1995/11/30  00:02:35  hampton
 * Print a stack traceback before attempting to dump core.  [CSCdi44670]
 *
 * Revision 3.4  1995/11/21  07:41:56  jenny
 * CSCdi43195:  TFTP: cannot specify source address
 *
 * Revision 3.3  1995/11/17  18:46:30  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:17:34  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:45:05  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.7  1995/11/08  21:18:38  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.6  1995/09/25  08:37:01  tli
 * CSCdi40960:  ISP jumbo patch 2
 *
 * Revision 2.5  1995/08/03  07:42:40  hampton
 * EIGRP stops processing input.  Convert the IP socket input queue into a
 * watched_queue.  This allows the scheduler to see if there is input
 * waiting when the process dismisses.  [CSCdi38156]
 *
 * Revision 2.4  1995/07/18 06:48:11  hampton
 * Miscellaneous changes to eliminate direct references to the system
 * clock and to convert to always using the timer macros to access timers.
 * [CSCdi37217]
 *
 * Revision 2.3  1995/06/28 23:14:13  tkolar
 * CSCdi35465:  I/O memory needs to be included when dumping core
 * Add flag for regions to determine if they are dumped or not.
 *
 * Revision 2.2  1995/06/18  02:30:17  hampton
 * The crashdump routing should clear the blocking disabled flag.
 * [CSCdi36036]
 *
 * Revision 2.1  1995/06/07 21:54:38  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <string.h>
#include <ciscolib.h>
#include "config.h"
#include "ttysrv.h"
#include "exception.h"
#include "logger.h"
#include "packet.h"
#include "file.h"
#include "../ip/ip.h"
#include "parser.h"
#include "../parser/parser_defs_misc.h"
#include "registry.h"
#include "sys_registry.h"
#include "interface_private.h"
#include "../ip/ip_registry.h"
#include "free.h"
#include "region.h"
#include "../os/msg_system.c"		/* Not a typo, see logger.h */

/* Exported stuff */
file_access_protocol sl_exception_dumpproto = FILE_ACCESS_TFTP;

#define DEF_EXCEPTION_REGION_SIZE	16384

/* Local definitions */
static ulong exception_memmin = 0L;	       /* memory leak exception */
static ulong exception_memfrg = 0L;	       /* fragment exception */
static file_access_protocol exception_dumpproto = FILE_ACCESS_TFTP;
static ipsocktype *sl_exception_soc = NULL;   /* Socket to tftp write on (slave) */
static ulong exception_region_size = DEF_EXCEPTION_REGION_SIZE;
static regiontype exception_region;
static mempool exception_mempool;
static uchar *exception_memory;

    /* maximum length for the name of a core file being written */
#define DUMPNAME_MAXLEN 32 	


/*
 * build_dump_fullname - assemble a name for a core file
 */

static void build_dump_fullname (char *base, char *aux, char *fullname)
{
    memset(fullname, 0, DUMPNAME_MAXLEN);

    /* stay compatible with existing conventions */
    if (!strcmp(aux, "main")) {
	strcpy(fullname, base);
	return;
    }

    if (strlen(base) < DUMPNAME_MAXLEN-strlen(aux)) {
	strcpy(fullname, base);
	strcat(fullname, aux);
    } else {
	sstrncpy(fullname, base, DUMPNAME_MAXLEN-strlen(aux));
	strcat(fullname, aux);
    }
}

/*
 * exception_crash_process - Process which writes out core files
 */

static process exception_crash_process (void)
{
    regiontype *region;
    ipsocktype *exception_soc;
    char dump_fullname[DUMPNAME_MAXLEN];

    /*
     * Attempt to set watch points on the queues now, while we know there
     * is memory available.  If this is attempted later, there may not be
     * any memory left to allocate the watch blocks.
     */
    region = region_find_by_attributes(REGION_CLASS_ANY,
				       REGION_STATUS_ANY,
				       REGION_MEDIA_ANY);
    while (region) {
	if (region->flags & REGION_FLAGS_EXCEPTION_DUMP) {
	    exception_soc = region->context;
	    if (exception_soc)
		process_watch_queue(exception_soc->inq, ENABLE, RECURRING);
	}
	region = region_find_next_by_attributes(region, REGION_CLASS_ANY,
					        REGION_STATUS_ANY,
					        REGION_MEDIA_ANY);
    }

    /*
     * Wait for a crash.
     */
    process_set_wakeup_reasons(BOOLEAN_EVENT, ONE_SHOT);
    process_watch_boolean(system_crashed, ENABLE, ONE_SHOT);
    process_wait_for_event();

    region = region_find_by_attributes(REGION_CLASS_ANY,
				       REGION_STATUS_ANY,
				       REGION_MEDIA_ANY);
    while (region) {
	if (region->flags & REGION_FLAGS_EXCEPTION_DUMP) {
	    build_dump_fullname(dumpfile, region->name, dump_fullname);
	    if (reg_invoke_core_dump(exception_dumpproto,
			     region->start, region->size,
			     dump_fullname, dumphost.ip_addr, 
			     region->context))
		printf(" [OK]");
	    else
		printf(" [Failed]");
            /*
             * In case the core dump operation generated some debugging
             * messages, flush them to console.
             */
            reg_invoke_flush_logging();  

	}
	region = region_find_next_by_attributes(region, REGION_CLASS_ANY,
					        REGION_STATUS_ANY,
					        REGION_MEDIA_ANY);
    }

    flush();			/* Start up the output */
    process_sleep_for(ONESEC);		/* Wait for it */
    raise_interrupt_level(ALL_DISABLE);	/* Stop the world... */

    gdb_crash_restore();	/* Restore context and go back to monitor */
}

/*
 * exception_regionsize_config
 *
 * Set up the requested exception region-size.
 */

static void exception_regionsize_config (parseinfo *csb)
{
    mempool *mempool;
    regiontype *region;
    ulong oldsize;

    /*
     * If the pool already exists (neat trick), let the user know that
     * we'll be ignoring his request since we can't destroy the pool
     * and region.
     */
    mempool = mempool_find_by_class(MEMPOOL_CLASS_LOCAL_EXCEPTION);
    if (mempool) {
	region = LIST_HEAD_DATA(&mempool->regionlist);
	printf("\n%% Exception memory pool already created, size %d bytes",
	       region->size);
	return;
    }

    /*
     * Set up the region size and allocate the memory block for
     * the region (freeing the old one if it exists) if the size
     * has changed.
     */
    oldsize = exception_region_size;
    exception_region_size = csb->sense ? GETOBJ(int,1) :
	DEF_EXCEPTION_REGION_SIZE;

    if (oldsize != exception_region_size) {
	if (exception_memory)
	    free(exception_memory);
	exception_memory = malloc(exception_region_size);
    }
}

/*
 * exception_mempool_init
 *
 * Function called when the memory manager wants to start using a
 * pool's exception pool.  This routine will actually create the
 * required memory region and mempool.  It must supply all data
 * structures to the create routines so allocation is not required.
 */

static void exception_mempool_init (mempool *mem)
{
    regiontype *region;
    mempool *new_mempool;

    if (exception_memory == NULL)
	return;

    /*
     * Create the region that can be malloced from.
     */
    region = region_create(&exception_region, "exception", exception_memory,
			   exception_region_size, REGION_CLASS_LOCAL, 0);
    if (region == NULL) {
	return;
    }

    /*
     * Create the mempool.
     */
    new_mempool = mempool_create(&exception_mempool, "Exception",
				 &exception_region, 0, NULL, 0,
				 MEMPOOL_CLASS_LOCAL_EXCEPTION);
    if (new_mempool == NULL) {
	return;
    }

    mempool_add_exception_pool(mem, &exception_mempool);
}

/*
 * exception_reserve_memory
 *
 * Set aside a smallish amount of memory to serve as a fallback
 * pool when the processor memory pool is marked corrupt but we
 * still to allocate memory to perform a core dump.
 */

static boolean exception_reserve_memory (boolean reserve)
{
    mempool *local;

    local = mempool_find_by_class(MEMPOOL_CLASS_LOCAL);
    if (reserve) {
	/*
	 * Reserve the memory for the exception pool.
	 */

	/*
	 * See if anything needs to be done first.
	 */
	if (exception_memory)
	    return(TRUE);

	/*
	 * Create the memory block.
	 */
	exception_memory = malloc(exception_region_size);
	if (exception_memory == NULL)
	    return(FALSE);

	/*
	 * Set up the function with the pool so the mempool
	 * gets initialized when the forced crash occurs.
	 */
	mempool_add_exception_pool_init(local, &exception_mempool_init);
    } else {
	/*
	 * Un-reserve the memory for the exception pool.
	 */

	/*
	 * If the pool exists, we can't remove it easily,
	 * so just bail.
	 */
	if (mempool_find_by_class(MEMPOOL_CLASS_LOCAL_EXCEPTION))
	    return(TRUE);

	/*
	 * Free the memory block and remember that it's gone.
	 */
	if (exception_memory) {
	    free(exception_memory);
	    exception_memory = NULL;
	}
    }
    return(TRUE);
}

/*
 * exception_core_config
 */

static void exception_core_config (parseinfo *csb)
{
    if (csb->sense) {	/* exception core-file <GETOBJ(string,1)> */
	setstring(&dumpfile, GETOBJ(string,1));
	dumpfile_def = FALSE;	/* Not the default */
    } else {		/* no exception core-file */
	free(dumpfile);
	dumpfile = malloc(strlen(hostname) + sizeof("-core"));
	if (dumpfile) {
	    concat(hostname, "-core", dumpfile);
	}
	dumpfile_def = TRUE;
    }
}

/*
 * sl_exception_core_config
 */
 
static void sl_exception_core_config (parseinfo *csb)
{
    if (csb->sense) {   /* exception-slave core-file <GETOBJ(string,1)> */
        setstring(&sl_dumpfile, GETOBJ(string,1));
        sl_dumpfile_def = FALSE;        /* Not the default */
    } else {            /* no exception core-file */
        free(sl_dumpfile);
        sl_dumpfile = malloc(strlen(hostname) + sizeof("-core") +
                        sizeof("-slotX"));
        if (sl_dumpfile) {
            /* Concat "core" now and slot number later when we know it */
            concat(hostname, "-core", dumpfile);
        }
        sl_dumpfile_def = TRUE;
    }
}

/*
 * exception_dump_config
 */
static void exception_dump_config (parseinfo *csb)
{
    regiontype *region;

    if (!csb->sense) {		/* No exception dump */
	if (exception_pid) {
	    process_kill(exception_pid);
	    exception_pid = 0;
	}

        /* 
	 *  cycle through the memory regions, closing any sockets
	 *	that might be open 
	 */
	region = region_find_by_attributes(REGION_CLASS_ANY,
					   REGION_STATUS_ANY,
					   REGION_MEDIA_ANY);
	while (region) {
	    if (region->context && 
			(region->flags & REGION_FLAGS_EXCEPTION_DUMP)) {
		reg_invoke_ip_close_socket(region->context);
		region->context = NULL;
	    }
	    region = region_find_next_by_attributes(region, REGION_CLASS_ANY,
						    REGION_STATUS_ANY,
						    REGION_MEDIA_ANY);
	}
	dumphost.ip_addr = 0L;

	exception_reserve_memory(FALSE);

	return;
    }

    if (reg_invoke_ip_good_address(GETOBJ(paddr,1)->ip_addr, 0, FALSE)) {
	dumphost.ip_addr = GETOBJ(paddr,1)->ip_addr;
	dumphost.type = ADDR_IP;

	exception_reserve_memory(TRUE);

	/* 
	 * open a socket for each region of memory we wish to dump.
	 */
	region = region_find_by_attributes(REGION_CLASS_ANY,
					   REGION_STATUS_ANY,
					   REGION_MEDIA_ANY);
	while(region) {
	    if (region->flags & REGION_FLAGS_EXCEPTION_DUMP) {
		if (region->context) {
		    reg_invoke_ip_close_socket(region->context);
		    region->context = NULL;
		}
		/*
		 * Note: we are ignoring the global tftp source address
		 * here to avoid reajusting socket structure if user 
		 * decides to change the tftp source address again. The 
		 * global tftp source address will be used as the local 
		 * address when we actually do the exception core dump, 
		 * if the local address is different from the global 
		 * source address.
		 */
		region->context = reg_invoke_ip_tftp_open_socket(&dumphost,
								 SOCK_ANYSRC);
	    }
	    region = region_find_next_by_attributes(region, REGION_CLASS_ANY,
						    REGION_STATUS_ANY,
						    REGION_MEDIA_ANY);
	}

	if (!exception_pid) {
	    exception_pid = process_create(exception_crash_process,
					   "Crash writer", NORMAL_STACK,
					   PRIO_NORMAL);
	    if (exception_pid != NO_PROCESS) {
		process_set_ttynum(exception_pid, CONSOLE_LINENUM);
	    } else {
		exception_pid = 0;
	    }
	}
    }
    return;
}

/*
 * sl_exception_dump_config
 */
static void sl_exception_dump_config (parseinfo *csb)
{
    if (!csb->sense) {          /* No exception dump */
        if (sl_exception_soc) {
            reg_invoke_ip_close_socket(sl_exception_soc);
            sl_exception_soc = NULL;
        }
        sl_dumphost.ip_addr = 0L;
        return;
    }
 
    if (reg_invoke_ip_good_address(GETOBJ(paddr,1)->ip_addr, 0, FALSE)) {
        sl_dumphost.ip_addr = GETOBJ(paddr,1)->ip_addr;
        sl_dumphost.type = ADDR_IP;
        if (sl_exception_soc) {
            reg_invoke_ip_close_socket(sl_exception_soc);
        }
	sl_exception_soc = reg_invoke_ip_tftp_open_socket(&sl_dumphost,
							  SOCK_ANYSRC);
    }
    return;
}

/*
 * exception_mempool_check
 *
 * Called to check any exception conditions for local mempools
 */

static void exception_mempool_check (void)
{
    static boolean  exception_trip = FALSE;
    ulong           freemem;
    mempool        *mempool; 

    if (!systeminit_flag)
	return;

    mempool = mempool_find_by_class(MEMPOOL_CLASS_LOCAL);
    if (!mempool)
	return;

    if (exception_memmin) {
	freemem = mempool_get_free_bytes(MEMPOOL_CLASS_LOCAL,
					 MEMPOOL_ALTERNATE_INCLUDE);
	if (freemem < exception_memmin && exception_trip == FALSE) {
	    exception_trip = TRUE;
	    errmsg(&msgsym(LOWMEM, SYS), exception_memmin, freemem);
	    crashdump(10);
	}
    }

    if (exception_memfrg) {
	if (!memory_available(exception_memfrg, mempool)) {
	    exception_trip = TRUE;
	    errmsg(&msgsym(NOMEMFRG, SYS), exception_memfrg);
	    crashdump(11);
	}
    }
}

/*
 * exception_config - Handle the exception configuration command
 */
void exception_config (parseinfo *csb)
{
    boolean install = FALSE;

    if (csb->nvgen) {
	switch (csb->which) {
	  case EXCEPTION_CORE:
	    nv_write(!dumpfile_def, "%s %s", csb->nv_command, dumpfile);
	    break;
	  case EXCEPTION_DUMP:
	    nv_write(dumphost.ip_addr, "%s %i",
		     csb->nv_command, dumphost.ip_addr);
	    break;
	  case EXCEPTION_MEMFRG:
	    nv_write(exception_memfrg, "%s %u", csb->nv_command,
		     exception_memfrg);
	    break;
	  case EXCEPTION_MEMMIN:
	    nv_write(exception_memmin, "%s %u", csb->nv_command,
		     exception_memmin);
	    break;
	  case EXCEPTION_SIMULATE:
	  case EXCEPTION_CRASHDUMP:
	    /* Do nothing */
	    break;
	  case EXCEPTION_PROTO:
	    switch (exception_dumpproto) {
	    case FILE_ACCESS_TFTP:
		/* default, show nothing */
		break;
	    case FILE_ACCESS_RCP:
		nv_write(TRUE, "%s rcp", csb->nv_command);
		break;
	    case FILE_ACCESS_FTP:
		nv_write(TRUE, "%s ftp", csb->nv_command);
		break;
	    default:
		nv_write(TRUE, "%s unknown", csb->nv_command);
		break;
	    }
	    break;
	  case EXCEPTION_SPURIOUS:
	    nv_write(spurious_threshold, "%s %d",
		     csb->nv_command, spurious_threshold);
	    break;
	  case EXCEPTION_REGIONSIZE:
	    nv_write(exception_region_size != DEF_EXCEPTION_REGION_SIZE,
		     "%s %d", csb->nv_command, exception_region_size);
	    break;
	}
	return;
    }

    switch (csb->which) {
      case EXCEPTION_CORE:
	exception_core_config(csb);
	break;
      case EXCEPTION_DUMP:
	exception_dump_config(csb);
	break;
      case EXCEPTION_MEMMIN:
	exception_memmin = GETOBJ(int,1);
	install = TRUE;
	break;
      case EXCEPTION_MEMFRG:
	exception_memfrg = GETOBJ(int,1);
	install = TRUE;
	break;
#ifdef GLOBAL_DEBUG
      case EXCEPTION_SIMULATE:
	printf("\nEnd of world: %c", *((char *) -1));
	printf(" ...try 2: %d\n", *((short *)1));
	break;
      case EXCEPTION_CRASHDUMP:
	crashdump(1);
	break;
#endif
      case EXCEPTION_PROTO:
	exception_dumpproto = csb->sense ? GETOBJ(int,1) : FILE_ACCESS_TFTP;
	break;
      case EXCEPTION_SPURIOUS:
	spurious_threshold = GETOBJ(int, 1);
	break;
      case EXCEPTION_REGIONSIZE:
	exception_regionsize_config(csb);
	break;

      default:
	bad_parser_subcommand(csb, csb->which);
	break;
    }

    if (install) {
	reg_add_onemin(exception_mempool_check, "exception_mempool_check");
    }

}

/*
 * sl_exception_config - Handle the exception-slave configuration command
 */
void sl_exception_config (parseinfo *csb)
{
    if (csb->nvgen) {
        switch (csb->which) {
          case EXCEPTION_CORE:
            nv_write(!sl_dumpfile_def, "%s %s", csb->nv_command, sl_dumpfile);
            break;
          case EXCEPTION_DUMP:
            nv_write(sl_dumphost.ip_addr, "%s %i",
                     csb->nv_command, sl_dumphost.ip_addr);
            break;
          case EXCEPTION_PROTO:
            nv_write(sl_exception_dumpproto != FILE_ACCESS_TFTP, "%s %s",
                     csb->nv_command,
                     (sl_exception_dumpproto == FILE_ACCESS_RCP) ?
                        "rcp" : "unknown");
            break;
        }
        return;
    }
 
    switch (csb->which) {
      case EXCEPTION_CORE:
        sl_exception_core_config(csb);
        break;
      case EXCEPTION_DUMP:
        sl_exception_dump_config(csb);
        break;
      case EXCEPTION_PROTO:
        sl_exception_dumpproto = csb->sense ? GETOBJ(int,1) : FILE_ACCESS_TFTP;
        break;
      default:
        bad_parser_subcommand(csb, csb->which);
        break;
    }
}

/*
 * manual_write_core
 * Use the appropriate protocol to manually write a core file.
 */

void manual_write_core (parseinfo *csb)
{
    regiontype *region;
    addrtype    dumphost;
    int         parsed;
    char       *filename = dumpfile;
    char dump_fullname[DUMPNAME_MAXLEN];


    if (GETOBJ(paddr,1)->type == ADDR_IP) {
	address_copy(&dumphost, GETOBJ(paddr,1));
	parsed = TRUE;
    } else {
	printf("Remote host? ");
	parsed = rdtty(GETOBJ(string,2), PARSEBUF);
	if (parsed) {
	    parsed = reg_invoke_name_parse_hostname(ADDR_IP,
						    GETOBJ(string,2),
						    &dumphost);
	}
    }
    if (!parsed || reg_invoke_name_martian(ADDR_IP, &dumphost, TRUE)) {
	printf("\n?Bad IP address or host name");
	return;
    }

    /*
     * Get core file name
     */
    if (*GETOBJ(string,1) != '\0') {
	filename = GETOBJ(string,1);
	parsed++;
    } else {
	printf("\nName of core file to write [%s]? ", filename);
	parsed = rdtty(GETOBJ(string,1), PARSEBUF);
	if (parsed) {
	    char *cp;

	    cp = deblank(GETOBJ(string,1));
	    if (cp && *cp)
		filename = cp;
	}
    }
    if (!parsed) {
	printf("%%Bad file name");
	return;
    }
    if (!(csb->flags & CONFIG_HTTP)) {
	printf("\nWrite file %s on host %i? ", filename, dumphost.ip_addr);
	if (!confirm(NULL)) {
	    return;
	}
    }

    region = region_find_by_attributes(REGION_CLASS_ANY,
				       REGION_STATUS_ANY,
				       REGION_MEDIA_ANY);
    while (region) {
	if (region->flags & REGION_FLAGS_EXCEPTION_DUMP) {
	    build_dump_fullname(filename, region->name, dump_fullname);
	    if (reg_invoke_core_dump(exception_dumpproto,
				     region->start, region->size,
				     dump_fullname, dumphost.ip_addr, 
				     NULL))
		printf(" [OK]");
	    else
		printf(" [Failed]");
	}
	region = region_find_next_by_attributes(region, REGION_CLASS_ANY,
					        REGION_STATUS_ANY,
					        REGION_MEDIA_ANY);
    }
}
