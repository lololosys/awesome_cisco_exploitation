/* $Id: capture_cmds.c,v 3.6.10.7 1996/08/28 13:07:40 thille Exp $
 * $Source: /release/112/cvs/Xsys/pgen/capture_cmds.c,v $
 *------------------------------------------------------------------
 * Packet capturing mode commands
 *
 * Nov 1995 skoh
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: capture_cmds.c,v $
 * Revision 3.6.10.7  1996/08/28  13:07:40  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.6.10.6  1996/08/03  23:36:05  fox
 * CSCdi63400:  high priority compatibility queue must die
 * Branch: California_branch
 * Remove high priority compatibility queue and activatehigh flag.
 * Convert high priority cfork'd processes to new-style processes with
 * watched queues, timers, and booleans.  Split Multilink PPP input
 * into separate high priority process.  Split load and busy calculations
 * into separate processes.  Speed up PPP input and scheduler processing.
 * Miscellaneous code changes.
 *
 * Revision 3.6.10.5  1996/06/28  23:25:24  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.6.10.4  1996/06/27  09:39:26  fox
 * CSCdi61238:  linktype is an overloaded symbol
 * Branch: California_branch
 * Change idb->linktype to idb->getlink.
 *
 * Revision 3.6.10.3  1996/05/21  10:00:24  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.6.10.2  1996/04/27  06:41:24  syiu
 * Branch: California_branch
 * Commit Dial1_branch
 *
 * Revision 3.4.10.2  1996/04/26  01:04:09  syiu
 * Branch: Dial1_branch
 * Dial1_branch to IosSec_postmerge_960419
 *
 * Revision 3.4.10.1  1996/04/17  03:52:44  syiu
 * Branch: Dial1_branch
 * Sync Dial1_branch to V111_1_3
 *
 * Revision 3.6.10.1  1996/03/18  21:35:11  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.4.14.4  1996/03/16  07:24:56  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.4.14.3  1996/03/13  01:52:22  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.4.14.2  1996/03/07  10:40:58  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.4.14.1  1996/02/20  16:58:03  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.6  1996/02/22  14:37:13  widmer
 * CSCdi49281:  make all parser_extension_request static const
 * make parser_extension_request typedef const
 * make all parser_extension_request static
 *
 * Revision 3.5  1996/01/29  07:29:23  hampton
 * Remove unnecessary includes of parser definition files.  [CSCdi47717]
 *
 * Revision 3.4  1995/12/17  18:36:11  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.3  1995/11/17  18:55:57  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:21:53  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:58:49  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.5  1995/09/14  01:51:55  biren
 * CSCdi40312:  Move pakgen and pgen (Pagent 2.2) to seperate subsystems
 *
 * Revision 2.4  1995/08/07  05:41:10  richl
 * CSCdi38315:  makefile rework
 *
 * Revision 2.3  1995/07/16  18:04:21  thille
 * CSCdi37049:  Console output from SNMP WriteNet SET confuses console user
 * Bad programmer, no doggie biscuit.
 *
 * Revision 2.2  1995/07/15  02:37:12  thille
 * CSCdi37049:  Console output from SNMP WriteNet SET confuses console user
 * Add a silent_mode parameter to some routines and registries and set it
 * to TRUE when making the calls from SNMP WriteNet, netConfigSet, or
 * hostConfigSet.  Also, add an errmsg for each so the event can still get
 * logged, and make it clear what caused the event.  Fix other callers to
 * pass FALSE so current behavior will be maintained.
 *
 * Revision 2.1  1995/06/07  22:33:52  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include <ctype.h>
#include <stdarg.h>
#include <ciscolib.h>
#include "sys_registry.h"
#include "config.h"
#include "interface_private.h"
#include "packet.h"
#include "ttysrv.h"
#include "file.h"

#include "ieee.h"
#include "../if/fddi.h"
    
#include "logger.h"
#include "../os/msg_system.c"		/* Not a typo, see logger.h */

#include "parser.h"
/*#include "../parser/parser_defs_breakset.h"
  */
#include "../parser/parser_actions.h"
#include "../parser/parser_defs_parser.h"
#include "../parser/macros.h"
#include "capture_private.h"
#include "pgen_private.h"

/* inline declarations */

inline void capture_swap_fddi_hdr (paktype *pak)
{
    fddi_hdr *fddi_stuff;
    
    fddi_stuff = (fddi_hdr *) pak->datagramstart;
    ieee_swap(fddi_stuff->daddr, fddi_stuff->daddr);
    ieee_swap(fddi_stuff->saddr, fddi_stuff->saddr);
    
}


/*
 *parser chain for "caputre [<number>]" command
 */
#define ALTERNATE NONE
#include "./cfg_int_capture.h"
LINK_POINT(capture_ci_commands, ALTERNATE);
#undef ALTERNATE


/*
 *parser chain for "show capture interface <interface>" commnad
 */
#define ALTERNATE NONE
#include "./exec_show_captured.h"
LINK_POINT(capture_show_commands, ALTERNATE);
#undef ALTERNATE

/*
 *parser chain for "capture dump interface <tftp_server> <filename>"
 */

#define ALTERNATE NONE
#include "./exec_capture_dump_tftp.h"
LINK_POINT(capture_exec_dump_commands, ALTERNATE);
#undef ALTERNATE

/*
 *parser chain for "clear capture interface <interface>"
 */

#define ALTERNATE NONE
#include "./exec_clear_capture.h"
LINK_POINT(capture_clear_command, ALTERNATE);
#undef ALTERNATE

static parser_extension_request capture_chain_init_table[] = {
    { PARSE_ADD_SHOW_CMD, &pname(capture_show_commands) },
    { PARSE_ADD_CFG_INT_CMD, &pname(capture_ci_commands) },
    { PARSE_ADD_EXEC_CMD, &pname(capture_exec_dump_commands) },
    { PARSE_ADD_CLEAR_CMD, &pname(capture_clear_command) },
    { PARSE_LIST_END, NULL}
};


/*
 *	capture_parser_init:
 *	initialize the parser support for the capture
 */
void capture_parser_init (void)
{
    parser_add_command_list(capture_chain_init_table, "pgen");
 
}

/*	capture_save_iqueue:
 *	routine to save the original driver vector
 */
static void capture_save_iqueue (hwidbtype *hwidb)
{
    hwidb->firstsw->CAPTURE_SAVED_IQUEUE = hwidb->iqueue;
}

/*	capture_restore_iqeue:
 *	routine to restore the original driver vector
 */
	
static void capture_restore_iqueue (hwidbtype *hwidb)
{
    hwidb->iqueue = hwidb->firstsw->CAPTURE_SAVED_IQUEUE;
}
    


/*
 *	capture94_empty_Q:
 *	specific routine to dequeue any pkt from the hwidb->CAPTURE_Q
 *	and datagram done the dequeued pkt
 *
 */
static void capture_empty_Q(queuetype *Q)
{
    paktype *pak;
    leveltype level;

    /* Disable interrupt while dequeuing */
    level = raise_interrupt_level(NETS_DISABLE);
    while (!QUEUEEMPTY(Q)) {
	pak = p_dequeue(Q);
	datagram_done(pak);
    }
    reset_interrupt_level(level);
}


/*
 *	capture_activate:
 *	When the "csb->sense" is TRUE, the function will init. all data
 *	structures for the capturing, and set the hwidb->CAPTURE_FLAG
 *	to TRUE; otherwise, simply set the hwidb->CAPTURE_FLAG to FALSE
 *	
 */
   
void capture_activate (hwidbtype *hwidb, parseinfo *csb)
{
    ushort total;
    
    if (!csb->sense) {			   /* to deactivate the capturing*/
	capture_restore_iqueue(hwidb);     /* "no" has been parsed */
	hwidb->CAPTURE_FLAG = FALSE;
	capture_empty_Q(&hwidb->CAPTURE_Q);
    } else {
	/*
	 * make sure that the queue is cleaned out before restarting it.
	 */
	if (hwidb->CAPTURE_FLAG == TRUE) {
	    capture_restore_iqueue(hwidb);
	    capture_empty_Q(&hwidb->CAPTURE_Q);
	}

	total = GETOBJ(int,1);
	if ((total<1) || (total>100)) {	/* sanity chk */
	    total = CAPTURE_DEFAULT_PKT;	/* make it the default size */
	}

	queue_init(&hwidb->CAPTURE_Q, total); /* init the Q */
	capture_save_iqueue(hwidb);	/* save the orig. vector */
	if (csb->sense) {
	  hwidb->iqueue = capture_enqueue; /* by pass regular vector */
	}
	hwidb->CAPTURE_FLAG = TRUE;
    }

    return;
}
  
  

/*	capture_enqueue:
 *	routine to enqueue captured pkt's to the hwidb->CAPTURE_Q
 */

void capture_enqueue (hwidbtype *hwidb, paktype *pak)
{
    paktype *captured_pak;		/* ptr to the captured packet */
    
    GET_TIMESTAMP(hwidb->lastinput);	/* remember time of last input */
    
    if (!QUEUEFULL(&hwidb->CAPTURE_Q)) {	
	pak->linktype = (*hwidb->getlink)(pak);
	captured_pak = pak_duplicate(pak);
	if (captured_pak) {	
	    /* for the cbusI fddi, its addr's need be swapped */
	    if (hwidb->type == IDBTYPE_FDDI) { 
		capture_swap_fddi_hdr(captured_pak);
	    }
	    /* now we could queue the captured packet */
	    p_enqueue(&hwidb->CAPTURE_Q, captured_pak);
	}
    }

    /* hand the packet to the system, in any case */
    (*hwidb->firstsw->CAPTURE_SAVED_IQUEUE)(hwidb, pak); 

    return;
}

    

	    
/*
 *	capture_display_hdr:
 *	routine to display the header
 *
 */

static void capture_display_hdr (const hwidbtype *hwidb)
{
    
    printf("\n%s has %d captured packets", hwidb->hw_namestring,
	   hwidb->CAPTURE_Q.count);

    return;
}
 


/*	captuure_print_data:
 *	routine to display captured data
 */
static void capture_print_data (uchar data, ushort colum)
{
    /*
     * we only print certain colums a line.
     * And in each line we print data byte by byte w/ 0 fill in hex.
     */
    if (!(colum % CAPTURE_DISPLAY_COLUMNS)) {
	printf ("\n  %02x", data);	
    } else {
	printf ("  %02x", data);
    }

    return;
}



/*
 *	capture_display_pak:
 *	a routine to display a packet
 */

static void capture_display_pak(const paktype *pak, ushort i)
{
    uchar *ptr;
    ushort colum, byte_count;

    colum = byte_count = 0;
    ptr = pak->datagramstart;
    printf("\n  Packet %d", i);

    while (byte_count++ < pak->datagramsize) {
	capture_print_data(*ptr++, colum++);
    }

    return;
}


    
/*
 * 	show_capturedinfo:
 *	display info. for the selected interf. which is in the
 *	capturing mode
 *
 */

void show_capturedinfo (parseinfo *csb)
{
    idbtype *idb;
    hwidbtype *hwidb;
    paktype *pak;
    ushort i;
    
    idb = GETOBJ(idb,1);
    hwidb = hwidb_or_null(idb);

    if (!idb || !hwidb) {
	errmsg(&msgsym(NULLIDB, SYS), "show_capturedinfo");
	return;
    }

    if(!is_subinterface(idb)) {
	if (!hwidb->CAPTURE_FLAG) {
	    printf("\n Interface %s is not in the capturing mode",
		   hwidb->hw_namestring);
	} else {
	/*
	 *FIXME: WE MIGHT WANT TO LOCK OUT INTERRUPT while walking through
	 *       the CAPTURE_Q to prevent race condition.
	 */
	    automore_enable(NULL);
	    capture_display_hdr (hwidb);
	    if (!QUEUEEMPTY(&hwidb->CAPTURE_Q)) {
	       i=1;
	       /* walk through the queue to display each captured packet*/
               for (pak = (paktype *) hwidb->CAPTURE_Q.qhead; pak;
		    pak = pak->next) {
		   capture_display_pak(pak, i++);
	       }
	   }
	   automore_disable();
	}
	
    }
	
    return;
}
			    


/*  
 *	capture_command:
 *	Put a selected interface in the capturing mode and
 * 	initialize internal data structures for the capture.
 *
 */

void capture_command (parseinfo *csb)
{
    hwidbtype *hwidb;
    idbtype *swidb;

    swidb = csb->interface;
    hwidb = hwidb_or_null(csb->interface);

    if (!swidb || !hwidb) {
	errmsg(&msgsym(NULLIDB, SYS), "capture_command");
	return;
    }
 
    if (csb->nvgen) {
	if (!is_subinterface(swidb)) {   /* we do NOT deal w/ subinterf */
	    nv_write(hwidb->CAPTURE_FLAG,"in capturing mode,for %u packets",
			 hwidb->CAPTURE_Q.maximum);
	}
    } else {
	capture_activate(hwidb, csb);
    }
    return;
}

/*   clear the buffer for either given interface or all interfaces
*    in capturing mode.
*/
void capture_clear_command (parseinfo *csb) {
  
  idbtype *idb;
  hwidbtype *hwidb;

  idb = GETOBJ(idb,1);
  hwidb = hwidb_or_null(idb);
  if (csb->which == CLEAR_CAPTURE) {
    if(hwidb == (hwidbtype *) NULL) { /* clear all interfaces */
      FOR_ALL_HWIDBS(hwidb) {
	if (!hwidb->CAPTURE_FLAG) { /* not in capture mode */
	  continue;
	} else {
	  if (!QUEUEEMPTY(&hwidb->CAPTURE_Q)) {
	    capture_empty_Q(&hwidb->CAPTURE_Q);
	  }
	}
      }
    } else { /* clear only given interface */
      if (!hwidb->CAPTURE_FLAG) { /* not in capture mode */
	printf("\n Interface %s is not in the capturing mode",
	       hwidb->hw_namestring);
      } else {
	if (!QUEUEEMPTY(&hwidb->CAPTURE_Q)) {
	  capture_empty_Q(&hwidb->CAPTURE_Q);
	}
      }
    }
  }
  return;
}

/*	capture_dump_hdr:
 *	routine to prepare the header for the tftp dump.
 *	when the input "buffer" is a NULL ptr (i.e. buffer == NULL
 *	is true), then the routine actually caculates the size of
 *	header in bytes. see btprintf.
 */
static ushort capture_dump_hdr (const hwidbtype *hwidb, char **buffer)
{
    return (btprintf (buffer, NULLTTY, "\n%s has %d captured packets",
			 hwidb->hw_namestring, hwidb->CAPTURE_Q.count));
}

/*	capture_dump_data
 *	routine to organize the data, which will be saved in the "buffer",
 *	for the tftp dump .
 *	when the input "buffer" is a NULL ptr (i.e. buffer == NULL is true)
 *	the routine actually will calculate the size of formated
 *	output of the data in bytes. see btprtinf.
 */	
static ushort capture_dump_data (uchar data, ushort colum, char **buffer)
{
    
    if (!(colum % CAPTURE_DISPLAY_COLUMNS)) {  /* only certain
							colums a line */
	return (btprintf (buffer, NULLTTY, "\n  %02x", data));
    } else {
	return (btprintf (buffer, NULLTTY, "  %02x", data));
    }

}


/*	capture_dump_pak:
 *	routine to formate the output for a captured packet into
 *	the "buffer" for the tftp dump.
 *	when the input "buffer" is a NULL ptr (i.e. buffer == NULL
 *	is true) the routine will calculate the size of formated
 *	data of the packet. see btprintf.
 */
static ushort capture_dump_pak (const paktype *pak, ushort i, char **buffer)
{
    
    ushort colum, byte_count, size;
    uchar *ptr;

    colum = size = byte_count = 0;
    ptr = pak->datagramstart;
    size += btprintf(buffer, NULLTTY, "\n  Packet %d", i);

    while (byte_count++ < pak->datagramsize) {
	size += capture_dump_data(*ptr++, colum++, buffer);
    }

    return size;
}


/*	capture_dump_all:
 *	routine to formate all the captured packets for
 *	the tftp dump into the "buffer".
 *	when the input "buffer" is a NULL pointer (i.e. buffer == NULL
 *	is true) the routine will actually calculate the size of the
 *	all formated data in bytes. see btprintf
 */
 
static ulong capture_dump_all (const hwidbtype *hwidb, char *buffer)
{
    ulong size;
    ushort i;
    paktype *pak;
    queuetype Q;

    char *current_ptr = buffer;
    i = 0;
    size = 0;

    if (hwidb && hwidb->CAPTURE_FLAG) {
	size += capture_dump_hdr (hwidb, &current_ptr);
	Q=hwidb->CAPTURE_Q;
	if (!QUEUEEMPTY(&Q)) {
	    i = 1;
	    for (pak = (paktype *) hwidb->CAPTURE_Q.qhead; pak;
		 pak = pak->next) {
		size += capture_dump_pak (pak, i++, &current_ptr);
	    }
	}
    }

    return size;
	
}



/* 	capture_dump_commands:
 *	service/action routine for the exec cmd:
 *	"capture dump <interface> <tftp_destination> <file_name>"
 */
void capture_dump_commands (parseinfo *csb)
{
 
    hwidbtype *hwidb;
    idbtype *swidb;
    char *filename, *buffer;
    addrtype *host;
    filetype file;
    ulong size;

    size = 0;
    buffer = NULL;

    swidb = GETOBJ(idb,1);
    hwidb = hwidb_or_null(swidb);
    if (!swidb || !hwidb) {
	errmsg(&msgsym(NULLIDB, SYS), "capture_dump_commands");
	return;
    }

    /* in the parser, the subinterfaces is already eliminated.
     * This chk here is for the future use when we support
     * non-IEEE interfaces.
     */
    if (is_subinterface(swidb)) { /* we do NOT deal w/ subinterf. */
	return;
    }
    if (!hwidb->CAPTURE_FLAG) {
	printf("Interface %s is not in the capturing mode",
	       hwidb->hw_namestring);
	return;
    }
    filename = GETOBJ(string,1);
    host = GETOBJ(paddr,1);

    /* we will write all the captured data in a formated
     * form onto a tftp server. We need to determine the size of
     * these formated data to allocate a buffer to hold it.
     */
    size = capture_dump_all (hwidb, NULL);

    if(CAPTURE_IS_LONG_VALUE(size)) {
	printf("\nThe size for the TFTP dump is %lx, which is beyond
                 the integer range we could handle", size);
	return;
    }

    buffer = malloc (size);
    /*FIXME: we need to limit the size for tftp write to uint
      */
    if (buffer == NULL) {
	printf("\nNot enough memory for TFTP dump, which needs %d bytes",
	       size);
	return;
    }
    (void) capture_dump_all (hwidb, buffer);
    
    file.filename = filename;
    file.proto = FILE_ACCESS_TFTP;
    bcopy(host, &file.ps.tuba.boothost, sizeof(addrtype));
    if (!reg_invoke_file_write(file.proto, &file, buffer, size)) {
	printf ("\nTFTP transfer failed.");
    }
    free (buffer);
    
    
}
