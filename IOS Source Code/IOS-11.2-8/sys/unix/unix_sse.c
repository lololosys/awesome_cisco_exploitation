/* $Id: unix_sse.c,v 3.3.62.2 1996/09/13 04:15:05 smackie Exp $
 * $Source: /release/112/cvs/Xsys/unix/unix_sse.c,v $
 *------------------------------------------------------------------
 * unix_sse.c - Unix support for a fake SSE
 *
 * May 1993 Tony Li
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: unix_sse.c,v $
 * Revision 3.3.62.2  1996/09/13  04:15:05  smackie
 * Fix some of the major unix image unhappiness. (CSCdi69048)
 *
 *    o Remove problematic unix NIT support
 *    o Change default memory pool alignment to align 64 bit integers
 *    o Fix message compilation errors
 *
 * Branch: California_branch
 *
 * Revision 3.3.62.1  1996/03/18  22:30:14  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.26.2  1996/03/07  10:59:23  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.3.26.1  1996/01/24  23:29:37  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.3  1995/11/17  17:55:49  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:46:33  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:42:46  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/06/28  09:32:18  smackie
 * Repair widespread subsystem header braindamage. (CSCdi23568)
 *
 *   o Fix subsystem entry points to be foo(subsystype *) not foo(void)
 *   o Repair nonsensical and redundant subsystem sequences
 *   o Use NULL where no property is required
 *
 * Revision 2.1  1995/06/07  23:12:27  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <ctype.h>
#include "interface_private.h"
#include "packet.h"
#include "logger.h"
#include "subsys.h"
#include "sys_registry.h"
#include "../ip/ip.h"
#include "../hes/if_mci.h"
#include "../ip/ipfast.h"
#include "../xns/novfast.h"
#include "config.h"
#include "../parser/parser_defs_sse.h"
#include "../util/range.h"
#include "../srt/span.h"

#include "../hes/msg_sse.c"
#include "../hes/sse_public.h"
#include "../hes/sse.h"
#include "../hes/sse_private.h"
#include "unix.h"
#include "unix_sse.h"
#include "../hes/sse_registry.h"


/*
 * sse_unix_reset
 * Reset the SSE data structure.  Process level work.
 */

void sse_unix_reset (mcitype *cbus)
{
    sse_finish_reset(cbus, 64*1024, 256*1024, FALSE);
}

/*
 * load_sse
 * Load the IP cache
 */

static void load_sse (parseinfo *csb)
{
    char *name;
    uchar *buf, *end;
    int protocol;
    int bufsize = 700000;
    long got;

    /*
     * Really just load a file to pre-prime the fast switching cache.
     */
    buf = malloc(bufsize);
    if (!buf) {
	buginf("\nSSE: malloc failed");
	return;
    }
    protocol = GETOBJ(int,1);
    name = GETOBJ(string,2);
    if (!name) {
	buginf("\nSSE: no name");
	goto done;
    }
    if (sse_debug)
	buginf("\nSSE: parsing %s", name);
    if (!unix_read_file(name, buf, &got, (long) bufsize)) {
	buginf("\nSSE: read failed");
	goto done;
    }
    end = buf + got;

    reg_invoke_sse_preload_cache(protocol, buf, end);

 done:
    free(buf);
}

/*
 * show_sse_memory
 * Dump the sse memory.
 */

void show_sse_memory (ssetype *sse, int addr)
{
    printf("Not implemented");
}

/*
 * show_sse_rewrite
 * Dump the sse rewrite memory.
 */
void show_sse_rewrite (ssetype *sse, int addr)
{
    printf("Not implemented");
}

/*
 * unix_sse_idb_capable
 *
 * Return TRUE for debugging purposes.
 */

boolean unix_sse_idb_capable (hwidbtype *idb, boolean fast_ok)
{
    return(TRUE);
}

/*
 * unix_sse_present
 *
 * Return TRUE to look like we have an SSE.
 */
static boolean
unix_sse_present (void)
{
    return(TRUE);
}

/*
 * unix_sse_bus_reset
 *
 * Analog to cbus_init()
 */
static void
unix_sse_bus_reset (mcitype *cbus, boolean critical)
{
    return;
}

/*
 * unix_sse_final_init
 *
 * Do final initializations.
 */
static mcitype cbus;			/* dummy cbus structure */
queuetype cbusQ;

static void
unix_sse_final_init (void)
{
    enqueue(&cbusQ, &cbus);
    sse_init(&cbus);
}

/*
 * unix_sse_idb_bridging_capable
 *
 * Return TRUE for debugging purposes.
 */

boolean unix_sse_idb_bridging_capable (hwidbtype *idb)
{
    return(TRUE);
}

/*
 * unixssp_subsys_init
 *
 * Register the subsystem.
 */

void unixssp_subsys_init (subsystype *subsys)
{
    unix_sse_chain_init();
    reg_add_sse_idb_capable(unix_sse_idb_capable, 
			    "unix_sse_idb_capable");
    reg_add_sse_idb_bridging_capable(unix_sse_idb_bridging_capable,
				     "unix_sse_idb_bridging_capable");
    reg_add_sse_load(load_sse, "load_sse");
    reg_add_sse_board_reset(sse_unix_reset, "sse_unix_reset");
    reg_add_sse_present(unix_sse_present, "unix_sse_present");
    reg_add_if_final_init(unix_sse_final_init, "unix_sse_final_init");
    reg_add_sse_bus_reset(unix_sse_bus_reset, "unix_sse_bus_reset");
}

/* 
 * Unix SSE subsystem header
 */

#define UNIXSSP_MAJVERSION 1
#define UNIXSSP_MINVERSION 0
#define UNIXSSP_EDITVERSION 1

SUBSYS_HEADER(unixssp, UNIXSSP_MAJVERSION, UNIXSSP_MINVERSION,
	      UNIXSSP_EDITVERSION,
	      unixssp_subsys_init, SUBSYS_CLASS_DRIVER,
	      "seq: sse",
	      "req: sse");
