/* $Id: ip_sse.c,v 3.4.22.3 1996/08/28 12:45:24 thille Exp $
 * $Source: /release/112/cvs/Xsys/hes/ip_sse.c,v $
 *------------------------------------------------------------------
 * ip_sse.c -- IP based SSE support functions (NOT switching)
 *
 * Oct 1993 Tony Li
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ip_sse.c,v $
 * Revision 3.4.22.3  1996/08/28  12:45:24  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.4.22.2  1996/06/28  23:06:17  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.4.22.1  1996/03/18  19:42:41  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.12.2  1996/02/20  00:52:50  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.4  1996/01/24  20:13:50  jenny
 * CSCdi47046:  TCP/IP need code clean up (part 3)
 * . removed ip/tuba.c, ip/tuba_default.c and ip/tuba.h
 * . removed all includes for ip/tuba.h.
 * . removed more TUBA related code.
 *
 * Revision 3.3.12.1  1996/01/24  21:47:35  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.3  1995/12/28  18:26:48  jenny
 * CSCdi46011:  TCP/IP need to do code clean up.
 * This is the first patch of code changes for TCP/IP code cleanup.
 * . removed all references to TUBA_IP and TUBA_CLNS.
 * . renamed open_tuba_socket() to ip_open_socket().
 * . renamed tcp_tuba_open() to tcp_open().
 * . pass addrtype * instead of ipaddrtype for local and foreight addresses
 *   to tcp_open() for feature portability.
 *
 * Revision 3.2  1995/11/17  09:19:09  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:41:21  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/07/16  18:04:08  thille
 * CSCdi37049:  Console output from SNMP WriteNet SET confuses console user
 * Bad programmer, no doggie biscuit.
 *
 * Revision 2.3  1995/07/15  02:36:52  thille
 * CSCdi37049:  Console output from SNMP WriteNet SET confuses console user
 * Add a silent_mode parameter to some routines and registries and set it
 * to TRUE when making the calls from SNMP WriteNet, netConfigSet, or
 * hostConfigSet.  Also, add an errmsg for each so the event can still get
 * logged, and make it clear what caused the event.  Fix other callers to
 * pass FALSE so current behavior will be maintained.
 *
 * Revision 2.2  1995/06/18  23:17:33  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.1  1995/06/07  20:42:44  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "interface_private.h"
#include "packet.h"
#include "logger.h"
#include "../hes/msg_sse.c"		/* Not a typo, see logger.h */
#include "ttysrv.h"		/* WHP4 - look at new copy command code */
#include "if_mci.h"
#include "name.h"
#include "config.h"
#include "file.h"
#include "../ip/ip.h"
#include "../ip/tftp.h"
#include "../util/range.h"
#include "../ip/ip_registry.h"

#include "../parser/parser_defs_sse.h"
#include "sse.h"
#include "sse_private.h"


/* 
 * write_sse_rewrite_memory 
 */
static void write_sse_rewrite_memory (ipsocktype* soc, char* namep)
{
    mcitype *cbus;
    ssetype *sse;

    for (cbus = (mcitype *) cbusQ.qhead; cbus; cbus = cbus->next) {
	if (!cbus->sseptr)
	    continue;
	sse = cbus->sseptr;
	tftp_write_socket(soc, namep, SSE_REWRITE, "octet",
			  sse->rwbytes_avail);
    }
}

/* 
 * write_sse_memory
 * Copy tree memory to TFTP.  Hack alert: there is no nice way to access
 * tree memory in a linear fashion, so allocate memory and copy it out.
 * Then TFTP it out.
 */
static void write_sse_memory (ipsocktype* soc, char* namep)
{
    shadowwordtype *shadowmemory;
    mcitype *cbus;
    ssetype *sse;
    int i;

    for (cbus = (mcitype *) cbusQ.qhead; cbus; cbus = cbus->next) {
	if (!cbus->sseptr)
	    continue;
	sse = cbus->sseptr;
	shadowmemory = malloc(sizeof(shadowwordtype) *
						sse->words_avail);
	if (NULL == shadowmemory) {
	    printf(nomemory);
	    break;
	}
	for (i = 0; i < sse->words_avail; i++) 
	    sse_peek_treemem(sse, i, &shadowmemory[i]);
	tftp_write_socket(soc, namep, (uchar *)shadowmemory, "octet",
			  sizeof(shadowwordtype) *
			  sse->words_avail);
	free(shadowmemory);
    }
}

/*
 * copy_sse_command
 * Copy sse memory structures to a tftp server for debugging.
 */

void copy_sse_command (parseinfo* csb)
{
    addrtype host;
    nametype *ptr;
    ipsocktype* soc;
    int errcode;
    char *namep, *bp, *filename;
#define BUFLEN 80


  /*
   * Allocate buffer for file name
   * Allocate buffer used for tftp and host input
   */
    if (!(namep = malloc(BUFLEN)) || 
        !(bp = malloc(BUFLEN)) ||
	!(filename = malloc(BUFLEN))) {
        printf(nomemory);
        return;
    }
    printf("IP address or name of remote host? ");
    rdtty((char *)bp, 64);
    if (*bp) {
	if (!numeric_lookup(ADDR_IP, bp, &host)) {
	    if ((ptr = (nametype *) name_lookup(bp, &errcode, NAM_IP)))
		address_copy(&host, &ptr->v.ip.adr[0]);
	    else {
		printf("\n?Bad address or host name");
		return;
	    }
	}
    } else {
	printf("\nNo address or host name");
	return;
    }
    /*
     * Get filename.
     */
    printf("\nName of file to write ");
    switch (GETOBJ(int,1)) {
    case SSE_CMD_MEMORY:
	sprintf(filename, "%s-sse.mem", hostname);
	break;
    case SSE_CMD_REWRITE:
	sprintf(filename, "%s-sse.macrw", hostname);
	break;
    }
    printf("[%s]? ", filename);
    rdtty(namep, 63);
    if (!*namep)
	strcpy(namep, filename);

    soc = reg_invoke_ip_tftp_open_socket(&host, SOCK_ANYSRC);
    if (!soc) {
	printf("\nCannot open socket to host %s", address_string(&host));
	return;
    }
    switch (GETOBJ(int,1)) {
    case SSE_CMD_MEMORY:
	write_sse_memory(soc, namep);
	break;
    case SSE_CMD_REWRITE:
	write_sse_rewrite_memory(soc, namep);
	break;
    }
    close_ipsocket(soc);
}
