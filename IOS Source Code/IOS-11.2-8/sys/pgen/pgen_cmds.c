/* $Id: pgen_cmds.c,v 3.3.44.3 1996/06/28 23:25:27 hampton Exp $
 * $Source: /release/112/cvs/Xsys/pgen/pgen_cmds.c,v $
 *------------------------------------------------------------------
 * P G E N _ C M D S . C
 *
 * Routines that implement the packet generator commands, along with
 * some user interface support code. Basically the whole UI is in here.
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: pgen_cmds.c,v $
 * Revision 3.3.44.3  1996/06/28  23:25:27  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.3.44.2  1996/05/21  10:00:31  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.3.44.1  1996/03/18  21:35:19  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.12.3  1996/03/13  01:52:25  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.3.12.2  1996/03/07  10:41:05  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.3.12.1  1996/02/20  16:58:24  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.3  1995/12/17  18:36:13  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.2  1995/11/17  18:56:31  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:59:11  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.6  1995/09/14  01:52:07  biren
 * CSCdi40312:  Move pakgen and pgen (Pagent 2.2) to seperate subsystems
 *
 * Revision 2.5  1995/07/16  18:04:22  thille
 * CSCdi37049:  Console output from SNMP WriteNet SET confuses console user
 * Bad programmer, no doggie biscuit.
 *
 * Revision 2.4  1995/07/15  02:37:13  thille
 * CSCdi37049:  Console output from SNMP WriteNet SET confuses console user
 * Add a silent_mode parameter to some routines and registries and set it
 * to TRUE when making the calls from SNMP WriteNet, netConfigSet, or
 * hostConfigSet.  Also, add an errmsg for each so the event can still get
 * logged, and make it clear what caused the event.  Fix other callers to
 * pass FALSE so current behavior will be maintained.
 *
 * Revision 2.3  1995/06/20  07:17:42  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.2  1995/06/09  13:18:05  hampton
 * Rename old scheduler calls to fit the new naming convention.
 * [CSCdi35514]
 *
 * Revision 2.1  1995/06/07 22:34:12  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include <string.h>
#include <ctype.h>
#include <stdarg.h>
#include <stdlib.h>
#include <ciscolib.h>
#include "sys_registry.h"
#include "config.h"
#include "file.h"
#include "ttysrv.h"
#include "parser.h"
#include "../parser/parser_defs_breakset.h"
#include "interface_private.h"
#include "packet.h"
#include "../parser/parser_actions.h"
#include "pgen_registry.h"
#include "pgen_private.h"

/* CPU hog timer variables (used to enable and disable checking */
extern int sched_max_task_time;
extern int watchdog_time;
static int orig_max_task_time;
static int orig_watchdog_time;

/* pgen_command (not to be confused with pgen_commands) is the top-level
 * routine that gets called from the EXEC to enter packet generation mode.
 * It sets up basic packet generator data structures and fixes up the CSB
 * so that future user input is parsed as packet generation commands.
 */
void pgen_command (parseinfo *csb)
{
    /* Get some memory for the packet generator context. */
    if (PGEND(csb) == (pgen_context *) NULL) {
        SETPGEND(csb, malloc(sizeof(pgen_context)));
        if (PGEND(csb) == (pgen_context *) NULL) {
	    printf ("\nNot enough memory to create packet generator context");
  	    return;
        }

        /* Initialize the fields in the context */
        memset(&(PGEND(csb)->workspace), 0, sizeof(queuetype));
        PGEND(csb)->bufsize = 0;
        PGEND(csb)->interface = SWIDBQ_HEAD;
        PGEND(csb)->batchdepth = 0;
    }

    /* Set things up so the EXEC will start doing PGEN commands instead
     * of normal commands.
     */
    set_mode_byname(&csb->mode, "pgen", MODE_VERBOSE);

    /* Disable CPU hog message generation (we're always a hog) */
    orig_max_task_time = sched_max_task_time;
    sched_max_task_time = 0;
    orig_watchdog_time = watchdog_time;
    watchdog_time = 0;
}

/* pgen_version_command displays the version number of the packet generator.
   This information is used to track the version of PaGenT as they are made
   available in the many versions of the OS
 */
void pgen_version_command (parseinfo *csb)
{
    printf("\n%s", PGEN_VERSION_STRING);
}

/* pgen_add_patch_command implements the "add" and "patch" commands, which
 * allow the user to edit the contents of packets in the workspace. The
 * way this actually works is that the parameters for the command are
 * validated, then saved in the packet generator context attached
 * to the CSB. We then check to see if the user has to type any hex
 * data to complete the command. If so, we point the EXEC at a parse
 * tree that collects multiple lines of data and saves them in a buffer
 * in the context area. When the user exits that mode, pgen_do_add_patch
 * is called to the the actual work. If no hex data are needed, we
 * call pgen_do_add_patch directly from this routine.
 */
void pgen_add_patch_command (
    parseinfo *csb,		/* Parse context */
    pgen_context *context,	/* Packet generator context */
    unsigned addpatch,		/* Add or patch? */
    char *begrange,		/* Name/number of first packet */
    char *endrange,		/* Name/number of last packet */
    unsigned edfunc,		/* "byte", "insert", "delete", or nothing? */
    unsigned edoffset,		/* Offset at which to do editing */
    unsigned edlen,		/* # bytes to insert, overwrite, or delete */
    uchar *edbuf,		/* Pointer to data for ins or overwrite */
    unsigned ttosend,		/* Time-to-send value */
    unsigned fixlen,		/* Length to which to force packet */
    unsigned repeat,		/* Number of times to repeat sending */
    unsigned interval,		/* Minimum interval between packets */
    idbtype *interface,		/* Interface on which to send packet */
    char *name)			/* Name to assign to packet */
{
    unsigned ibegrange;		/* Integer packet index for first packet */
    unsigned iendrange;		/* Integer packet index for last packet */
    int i;			/* Scanning index */

    /* Start by checking out the range the user specified. If it's not
     * cool, pgen_rangechk will print an error message, and we'll
     * return without doing anything. If we're doing an add, the packets
     * in the range don't need to exist already. If it's a patch, they
     * do.
     */
    if (!pgen_rangechk(context, begrange, endrange, &ibegrange, &iendrange,
		       (addpatch == PGEN_PATCH)))
	return;

    /* Even "add" requires the *first* packet to be either inside the
     * workspace or just off the end.
     */
    if ((addpatch == PGEN_ADD) &&
	(ibegrange > pgen_wscount(&(context->workspace)) + 1)) {
	printf ("\nCan't add packets beyond end of workspace");
	return;
    }

    /* If they've somehow managed to specify more data on the command
     * line than will fit into the buffer in the CSB, they lose. This
     * shouldn't happen, since at initial writing the most they can do
     * on the command line is 256 bytes, and the buffer is 8k. Anyway,
     * if it won't fit, complain, and if it will fit, copy it. The
     * data are only relevant for the "byte" (overwrite) and "insert"
     * keywords, so those are the only cases where we do anything at all.
     */
    if ((edfunc == PGEN_EDBYTE) || (edfunc == PGEN_EDINS)) {
	if (edlen > PGEN_HEXBUFSIZE) {
	    printf ("\nData too long for parse buffer. Truncated.");
	    edlen = PGEN_HEXBUFSIZE;
	}
	bcopy (edbuf, context->apeddata, edlen);
    }

    /* Save the other stuff that's been passed into us in the appropriate
     * variables in the packet generator context.
     */
    context->apedlen = edlen;
    context->apfunc = addpatch;
    context->apfirst = ibegrange;
    context->aplast = iendrange;
    context->apedfunc = edfunc;
    context->apedoffs = edoffset;
    context->apttosend = ttosend;
    context->apfixlen = fixlen;
    context->aprepeat = repeat;
    context->apinterval = interval;
    context->apif = interface;

    /* Validate any name the user specified before we do anything with it */
    if ((name == NULL) || (*name == '\0')) {
	context->apname = NULL;
    } else {
	/* A symbolic name must contain a nonnumeric character */
	for (i = 0; name[i] != '\0'; i++)
	    if (!isdigit(name[i])) break;
	if (name[i] == '\0') {
	    printf ("\nA symbolic name must contain a nonnumeric character");
	    return;
	}
	/* Can't embed a quote in a packet name */
	for (i = 0; name[i] != '\0'; i++)
	    if (name [i] == '"') {
		printf ("\nCan't embed a double quote in a packet name");
		return;
	    }
	/* Not allowed to give a packet a name that already exists in the
	 * workspace... not even if it's the same packet that already
	 * has the name.
	 */
	if ((i = pgen_wsbyname(&(context->workspace), name)) != 0) {
	    printf ("\nAlready have a packet named %s", name);
	    return;
	}
	/* Can't do a multipacket command with a name */
	if (ibegrange != iendrange) {
	    printf ("\nCan't give more than one packet the same name");
	    return;
	}
	/* Can't name a packet with the wildcard */
	if (!strcmp(name, "*")) {
	    printf ("\n\"*\" means all packets; can't name a packet that.");
	    return;
	}
	/* Can't name a packet with the end-of-workspace character */
	if (!strcmp(name, "$")) {
	    printf ("\n\"$\" means last packet; can't name a packet that.");
	    return;
	}
	/* Can't name a packet with one-off-end */
	if (!strcmp(name, "$$")) {
	    printf ("\n\"$$\" means one off end; can't name a packet that.");
	    return;
	}
	/* If the name looks OK, allocate some space and make a copy of it. */
	context->apname = strdup(name);
	if (context->apname == NULL)
	    printf ("\nNot enough memory for packet name %s- name lost.",
		    name);
    }

    /* If we've been called with a "byte" or "insert" keyword, but the user
     * didn't give any data on the command line, we need to go into hex mode
     * to collect the data. We'll defer processing this command until the
     * user exits hex mode. If the user gave data or didn't ask for "byte"
     * or "insert", we can go ahead and process the command now.
     */
    if (((edfunc == PGEN_EDBYTE) || (edfunc == PGEN_EDINS)) &&
	edlen == 0) {
	set_mode_byname(&csb->mode, "pgen-hex", MODE_VERBOSE);
	return;
    }

    pgen_do_add_patch (context);
}

void pgen_do_add_patch (pgen_context *context)
{
    unsigned pakno;		/* Index of packet being added or patched */
    paktype *pak;		/* Pointer to packet being added or patched */
    int i;			/* General-purpose index variable */
    unsigned edlen;		/* Length needed for packets being changed */

    /* We implement "add" as packet creation followed by a patch. If we're
     * doing an "add", create the workspace entries now. We don't do any
     * backout; if we can only add some of the packets, we just do the first
     * however many we can really do. To avoid having this be *really*
     * confusing, however, we do make sure to preallocate enough space
     * for the data each packet will have to contain before we put it
     * in the workspace.
     */
    if (context->apfunc == PGEN_ADD) {

	/* If there's a "byte" or "insert" keyword, we have to make space
	 * for the data.
	 */
	if ((context->apedoffs == PGEN_EDINS) ||
	    (context->apedoffs == PGEN_EDBYTE))
	    edlen = context->apedoffs + context->apedlen;
	else
	    edlen = 0;

	/* If the user is forcing us to a fixed length, we need to make
	 * space for that, too.
	 */
	if (context->apfixlen != -1)
	    edlen = max (edlen, context->apfixlen);

	/* Allocate and initialize all the packets */
	for (i = 0; i < (context->aplast - context->apfirst + 1); i++) {

	    /* Get the packet... assuming there's memory for it */
	    pak = pgen_wsnew (&(context->workspace), context->apfirst, edlen);

	    /* If we couldn't get a buffer for this packet, issue an error
	     * message and give up on getting any more.
	     */
	    if (pak == NULL) {
		printf ("\nNot enough memory to complete \"add\"; "
			"%d packets created", i);

		if (i == 0)
		    return;         /* Not adding any packets */
		
		/* If we're not adding our full complement of packets,
		 * we need to adjust the range to just cover the ones
		 * we *are* adding.
		 */
		context->aplast = context->apfirst + i - 1;

		/* No need to try for any more packets */
		break;
	    }
	    
	    /* Initialize all the fields of this packet to the defaults */
	    pak->datagramsize  = 0;
	    pak->if_output     = context->interface;
	    pak->pgen_output   = context->interface;
	    pak->pgen_rptcount = 1;
	    pak->pgen_interval = 0;

	    /* Default time-to-send is offset, because we may be called
	     * from a TFTP load with an "offset" clause.
	     */
	    pak->pgen_ttosend = context->timeoffs;

	    pak->pgen_nameptr = NULL;

	    /* Let other processes run */
	    process_may_suspend();
	}

    }

    /* Now we do the various "patch" functions. */
    for (pakno = context->apfirst; pakno <= context->aplast; pakno++) {

	/* Get the packet to play with */
	pak = pgen_wspick(&(context->workspace), pakno);

	/* Start with the data editing stuff, since it has a significant
	 * chance of failing due to low memory.
	 */
	if (context->apedfunc == PGEN_EDINS) {

	    /* Inserting data within the packet. First, let's figure out
	     * exactly how long the finished packet will be. We have to
	     * extend the packet by the length of the data to be inserted,
	     * and we may have to extend it further if the insertion starts
	     * at an offset that's off the end of the packet.
	     */
	    edlen = pak->datagramsize + context->apedlen;
	    if (pak->datagramsize < context->apedoffs)
		edlen += (context->apedoffs - pak->datagramsize);

	    /* Get the new, presumably larger, packet */
	    pak = pgen_wsgrow (&(context->workspace), pakno, edlen);
	    if (pak == NULL) {
		printf ("\nCouldn't enlarge packet %d; presumably no memory. "
			"\nAborting add/patch midstream.", pakno);
		return;
	    }

	    /* If the insertion offset was off the end, we need to
	     * zero out the automatic extension area.
	     */
	    if (pak->datagramsize < context->apedoffs)
		memset(pak->datagramstart + pak->datagramsize, 0,
		       context->apedoffs - pak->datagramsize);

	    /* If there are data after the insertion point, we need
	     * to move them up. One would like to use bcopy for
	     * this, but one would be screwed, because our bcopy
	     * doesn't handle the "overlapping buffers" case.
	     */
	    if (pak->datagramsize > context->apedoffs)
		for (i = edlen - 1;
		     i >= context->apedoffs + context->apedlen;
		     i--)
		    (pak->datagramstart)[i] =
			(pak->datagramstart)[i - context->apedlen];

	    /* Now we can actually stick in the inserted data. */
	    bcopy (context->apeddata, pak->datagramstart + context->apedoffs,
		   context->apedlen);

	    /* Finally, adjust datagramsize to reflect the change */
	    pak->datagramsize = edlen;

	} else if (context->apedfunc == PGEN_EDBYTE) {

	    /* Overwriting bytes in the packet. How long will it end up
	     * being? It won't get any shorter, and the data could run
	     * off the end.
	     */
	    edlen = max (pak->datagramsize,
			 context->apedoffs + context->apedlen);

	    /* Grow the packet if necessary */
	    if (edlen > pak->datagramsize) {
		pak = pgen_wsgrow (&(context->workspace), pakno, edlen);
		if (pak == NULL) {
		    printf ("\nCouldn't enlarge packet %d; presumably no "
			    "memory.\nAborting add/patch midstream.", pakno);
		    return;
		}
	    }

	    /* If there's a space between the old end of the packet and
	     * the place where we'll be putting our data, zero it out.
	     */
	    if (pak->datagramsize < context->apedoffs)
		memset(pak->datagramstart + pak->datagramsize, 0,
		       context->apedoffs - pak->datagramsize);

	    /* Copy in our data */
	    bcopy (context->apeddata, pak->datagramstart + context->apedoffs,
		   context->apedlen);

	    /* Adjust datagramsize */
	    pak->datagramsize = edlen;
	    
	} else if (context->apedfunc == PGEN_EDDEL) {

	    /* Deleting existing data in the packet. Set edlen to be
	     * the number of bytes we really delete (the user could
	     * have asked us to delete bytes that were off the end
	     * of the packet). Note that this differs from the "byte"
	     * and "insert" cases, where edlen is set to the overall
	     * length of the finished packet. We also check for the
	     * case where the data to be "deleted" don't exist.
	     */
	    if (context->apedoffs >= pak->datagramsize) {
		printf ("\nNothing to delete; packet %d is only %d bytes long",
			pakno, pak->datagramsize);
		return;
	    }
	    edlen = context->apedlen;
	    if ((edlen + context->apedoffs) > pak->datagramsize)
		edlen = pak->datagramsize - context->apedoffs;

	    /* Actually delete the data and move down whatever follows */
	    if (edlen != 0) {
		/* Move the data */
		for (i = context->apedoffs; i < pak->datagramsize - edlen; i++)
		    (pak->datagramstart)[i] = (pak->datagramstart)[i + edlen];
		/* Adjust datagram size */
		pak->datagramsize -= edlen;
	    }
	}

	/* Is the user trying to force the packet to a fixed length? */
	if (context->apfixlen != -1) {
	    /* Getting longer; need to extend it */
	    if (context->apfixlen > pak->datagramsize) {
		pak = pgen_wsgrow (&(context->workspace), pakno,
				   context->apfixlen);
		if (pak == NULL) {
		    printf ("\nCouldn't enlarge packet %d; presumably no "
			    "memory.\nAborting add/patch midstream.", pakno);
		    return;
		}
		/* Initialize the new area with zeroes */
		memset(pak->datagramstart + pak->datagramsize, 0,
		       context->apfixlen - pak->datagramsize);
	    }
	    pak->datagramsize = context->apfixlen;
	}

	/* Did the user ask for a new time-to-send? */
	if (context->apttosend != -1) {
	    /* Save user's time-to-send value */
	    pak->pgen_ttosend = context->apttosend + context->timeoffs;
	}

	/* New repeat count? */
	if (context->aprepeat != -1)
	    pak->pgen_rptcount = context->aprepeat;

	/* New interval value? */
	if (context->apinterval != -1)
	    pak->pgen_interval = context->apinterval;

	/* New interface? */
	if (context->apif != NULL) {
	    pak->if_output = context->apif;
	    pak->pgen_output = context->apif;
        }

	/* New name? */
	if (context->apname != NULL) {
	    /* Free any old name */
	    if (pak->pgen_nameptr != NULL)
		free (pak->pgen_nameptr);
	    /* And install the new one */
	    pak->pgen_nameptr = (uchar *) context->apname;
	}

	/* Let somebody else do something for a while */
	process_may_suspend();
    }

    return;			/* It's soup. */
}

/* pgen_clear_command implements the "clear" command, which deletes packets
 * from the workspace.
 */
void pgen_clear_command (
    pgen_context *context,	/* Context to use */
    char *firststr,		/* First packet name/number */
    char *laststr)		/* Last packet name/number */
{
    unsigned firstnum;		/* First packet number */
    unsigned lastnum;		/* Last packet number */
    int i;			/* Loop index */

    if (!pgen_rangechk (context, firststr, laststr, &firstnum, &lastnum, TRUE))
	return;
    for (i = 0; i < (lastnum - firstnum + 1); i++) {
	pgen_wsdel(&(context->workspace), firstnum);
	process_may_suspend();
    }
}

/* pgen_count_command implements the "count" command, which puts
 * the given interface in normal or (fast) count-only mode.
 */
void pgen_count_command (
    pgen_context *context,      /* Context to use */
    unsigned action,            /* on, off or status */
    idbtype *ifptr)             /* Interface to apply above action to */
{
    reg_invoke_pgen_count(hwidb_or_null(ifptr), action);
}

/* pgen_examine_command implements the "examine" command, which displays
 * packet contents and attributes in a relatively human-readable form.
 */
void pgen_examine_command (
    pgen_context *context,	/* Context to use */
    char *firststr,		/* First packet name/number */
    char *laststr,		/* Last packet name/number */
    unsigned bytecnt)		/* Max bytes to show per packet (-1 for all) */
{
    unsigned firstnum;		/* First packet number */
    unsigned lastnum;		/* Last packet number */
    int i;			/* Loop index */
    paktype *pak;		/* Packet being displayed */

    if (!pgen_rangechk (context, firststr, laststr, &firstnum, &lastnum, TRUE))
	return;

    automore_enable ("");
    for (i = firstnum; i <= lastnum; i++) {
	pak = pgen_wspick(&(context->workspace), i);
	printf ("\n\nPacket %d, ", i);
	if (pak->pgen_nameptr != NULL)
	    printf ("named %s, ", pak->pgen_nameptr);
	printf ("%d bytes, buffer at 0x%08x"
		"\nTime-to-send %d uSecs, repeat count %d,"
		" interval %d, interface %s\n---",
		pak->datagramsize, pak,
		pak->pgen_ttosend, pak->pgen_rptcount, pak->pgen_interval,
		pak->pgen_output->hwptr->hw_short_namestring);
	dump_mem (pak->datagramstart,
		  min(pak->datagramsize, bytecnt),
		  pak->datagramstart);
	process_may_suspend();
	if (automore_quit())
	    break;
    }
    automore_disable ();

}

/* pgen_interface_command implements the common_str_interface command, which sets
 * up the default interface to be used for newly-created packets.
 */
void pgen_interface_command (pgen_context *context, idbtype *ifptr)
{
    if (ifptr != NULL)
	context->interface = ifptr;
    else
	printf ("\nDefault interface is %s", context->interface->namestring);
}

/* pgen_sort_command implements the "sort" command, which sorts the workspace
 * (or a subrange thereof) into order by increasing time-to-send
 */
void pgen_sort_command (
    pgen_context *context,	/* Context to use */
    char *firststr,		/* Name/number of packet at start of range */
    char *laststr)		/* Packet at end of range */
{
    unsigned firstnum;		/* Number of packet to start with */
    unsigned lastnum;		/* Number of packet to end with */

    /* Check the range */
    if (!pgen_rangechk (context, firststr, laststr, &firstnum, &lastnum, TRUE))
	return;

    /* Do the sort */
    pgen_wssort (&(context->workspace), firstnum, lastnum);
}

/* pgen_buffer_command implements the "buffer" command, which changes the
 * size of the buffer the packet generator uses for TFTP transfers.
 */
void pgen_buffer_command (pgen_context *context, unsigned bufsize)
{
    if (bufsize != -1)
	context->bufsize = bufsize;

    printf ("\nTFTP buffers will be %d bytes long",
	    context->bufsize ? context->bufsize : PGEN_DEFTFTP);
}

/* pgen_dump_cmds_command implements the "dump commands" command, which saves
 * part or all of the current workspace as a file of commands.
 */
void pgen_dump_cmds_command (
    pgen_context *context,	/* Packet generator context */
    char *firststr,		/* Name/number of first packet to dump */
    char *laststr,		/* Name/number of last packet to dump */
    unsigned target,		/* Code for TFTP, RCP, TTY dump */
    char *filename,		/* Name of file to dump to if not TTY */
    addrtype *host)		/* IP address of host for dump */
{
    uint firstnum;		/* Number of first packet */
    uint lastnum;		/* Number of last packet */
    uint   size; 		/* Length of the dumped data */
    char *buffer = NULL;	/* Buffer for "dump commands tftp|rcp" */
    filetype file;

    /* Start by validating range and getting numeric bounds... */
    if (!pgen_rangechk (context, firststr, laststr, &firstnum, &lastnum, TRUE))
	return;

    /* Do the dump. What we do depends on whether it's a TFTP, RCP, or TTY */
    if (target != PGEN_DMPTTY) {
	/* Call to pgen_dumppaks to precompute output size */
	size = pgen_dumppaks (context, firstnum, lastnum, NULL, NULLTTY);

	/* Tell the user how much space she's sucking up */
	printf ("\nFormatted commands take %d bytes", size);
	
	/* Get a buffer of the needed size */
	buffer = malloc (size);
	if (buffer == NULL) {
	    printf ("\nNot enough memory for TFTP dump; need %d bytes", size);
	    return;
	}

	/* Fill the buffer with data */
	(void) pgen_dumppaks (context, firstnum, lastnum, buffer, NULLTTY);

	/*
	 * Dump the file to the host
	 */
	file.filename = filename;
	if (target == PGEN_DMPTFTP) {
	    file.proto = FILE_ACCESS_TFTP;
        } else {
	    file.proto = FILE_ACCESS_RCP;
	}
	bcopy(host, &file.ps.tuba.boothost, sizeof(addrtype));
	if (!reg_invoke_file_write(file.proto,
				 &file, buffer, size -1))
	    printf ("\nTFTP transfer failed.");

	/* Free the buffer */
	free (buffer);

    } else {			/* TTY dump */
	/* File won't start with a newline. Our output needs to. */
	printf ("\n");
	/* Send it off... */
	pgen_dumppaks (context, firstnum, lastnum, NULL, stdio);
    }

    return;			/* All done. */
}

/* pgen_load_command implements the "load" command, which reads files of
 * commands or binary Sniffer files.
 */
void pgen_load_command (
    pgen_context *context,	/* Packet generation context */
    unsigned filekind,		/* Commands? Sniffer file? */
    int offsettime,		/* Offset to apply to time-to-sends */
    unsigned target,            /* Code for TFTP or RCP dump */
    char *filename,		/* Name of file to load */
    addrtype *host)		/* Address of host to get file from */
{
    ulong bufsize;		/* Size of buffer to use */
    char *tftpbuf;		/* Buffer we load the file into */
    filetype file;

    /* We'll just load that little sucker right on in here. First, we
     * need to get a buffer to do that with.
     */
    if (context->bufsize != 0)
	bufsize = context->bufsize;
    else
	bufsize = PGEN_DEFTFTP;
    tftpbuf = malloc(bufsize);

    if (tftpbuf == NULL) {
	printf ("\nNot enough memory for load buffer");
	return;
    }

    /* Leave space in the buffer for a terminating NUL */
    if (filekind == PGEN_LDCMDS)
	bufsize -= 1;

    /*
     * Read in the file
     */
    if (target == PGEN_DMPTFTP) {
 	file.proto = FILE_ACCESS_TFTP;
    } else {
        file.proto = FILE_ACCESS_RCP;
    }
    file.ps.tuba.boothost.ip_addr = host->ip_addr;
    file.ps.tuba.boothost.type = ADDR_IP;
    file.filename = filename;
    file.ps.tuba.username = NULL;
    if (reg_invoke_file_read(file.proto,
			     (filetype *)&file, (uchar *)tftpbuf, &bufsize,
			     TRUE, NULL, NULL) != ERROR_OK) {
	printf("\nTFTP transfer failed");
	return;
    }

    /* Set up the offset time that will get applied to the packets we load */
    context->timeoffs += offsettime;

    switch (filekind) {

    case PGEN_LDCMDS:
        /* Make sure there's a terminator on the buffer */
	tftpbuf[bufsize] = '\0';
	
	/* Parse the commands. The time offset is applied to all times
	 * specified.
	 */
	pgen_parse_buffer (tftpbuf, context);

	break;

    case PGEN_LDSNIF:
	pgen_sniftows (context, tftpbuf, bufsize);
	break;
    }

    /* Reset time offset to what it was when we started */
    context->timeoffs -= offsettime;

    /* Free and return */
    free (tftpbuf);
    return;
}

/* pgen_replay_command implements the "replay" command, which does the actual
 * packet generation.
 */
void pgen_replay_command (
    pgen_context *context,	/* Packet generator context */
    char *firststr,		/* First packet to replay */
    char *laststr,		/* Last packet to replay */
    int offset,			/* uSec offset for time-to-sends */
    unsigned delay,		/* uSec delay between repeats of replay */
    unsigned repeat)		/* Number of times to repeat replay */
{
    unsigned firstnum;		/* Number of first packet */
    unsigned lastnum;		/* Number of last packet */
    paktype *firstpak;		/* First packet to send */
    paktype *succpak;		/* Packet *after* the last one to send */
    paktype *pak;		/* Packet we're currently working with */
    int itemp;			/* Integer temporary storage */

    /* Make sure the packets the user asked for are really there */
    if (!pgen_rangechk (context, firststr, laststr, &firstnum, &lastnum, TRUE))
	return;

    /* Did the user specify a repeat count? If not, it's one. */
    if (repeat == -1)
	repeat = 1;

    /* We want the user to be able to interrupt this */
    psion (stdio);

    /* Say what we're about to do, in case they haven't figured it out... */
    printf ("\nReplaying packets...");
    
    /* Find the first packet and the packet after the last one */
    firstpak = pgen_wspick (&(context->workspace), firstnum);
    /* Paranoia */
    if (firstpak == NULL) {
	printf ("\nPAGENT internal error; no first packet to send");
	return;
    }
    succpak = pgen_wspick (&(context->workspace), lastnum+1);  /* NULL is OK */

    /* Run through the packets and set up adjusted versions of
     * their time-to-send fields. These "ticks" fields are in milliseconds
     * (to match msclock) instead of the microseconds the user specifies,
     * and are adjusted by the time offset. We precalculate them to avoid
     * having to do too much weird math in the main loop.
     */
    for (pak = firstpak; pak != succpak; pak = pak->next) {
	/* Compute target time in usecs */
	itemp = (pak->pgen_ttosend + offset);
	/* Negative times make no sense */
	if (itemp < 0) itemp = 0;
	/* Anything after tick xxx is in tick xxx + 1 */
	pak->pgen_tick = (itemp + 999) / 1000;
    }

    /* Transmit packets as fast as supported by this platform */
    reg_invoke_pgen_send(firstpak, succpak, repeat, delay);

    /* Disable soft interrupts and return */
    psioff (stdio);
}

/* pgen_quit_command implements the "quit" command, which frees all packet
 * generator data structures and returns the user to the regular EXEC
 * command mode.
 */
void pgen_quit_command (parseinfo *csb, int clear_flag)
{
    /* Quitting from a batch file isn't implemented. */
    if (PGEND(csb)->batchdepth) {
	printf ("\nBatch file contained \"quit\" command. Ignored.");
	return;
    }

    /* Free workspace & context memory if "clear" option specified on quit */
    if (clear_flag) {

        /* Give back the packets in the workspace, if any */
        pgen_wsfreeall(&(PGEND(csb)->workspace));

        /* Give back the context memory */
        free (PGEND(csb));
        SETPGEND (csb, (pgen_context *) NULL);
    }

    /* Tell the EXEC to start acting normal again */
    csb->mode = exec_mode;

    /* Reinable CPU hog message */
    sched_max_task_time = orig_max_task_time;
    watchdog_time = orig_watchdog_time;
}

/* dump_mem displays the contents of a block of memory in the "traditional"
 * hex dump format, with addresses down the left side, 16 bytes of hex data
 * per line, and the ASCII interpretation of the data along the right
 * side. block is the address at which to start, len is the length in bytes
 * of the block to be dumped, and zero is used to adjust the displayed
 * addresses; the address given is zero. Thus, dump_mem(x,100,x) will
 * dump 100 bytes from x, and the "address" column will show offsets
 * from x instead of actual addresses.
 */
void dump_mem (void *iblock, unsigned len, void *izero)
{
    uchar *block = (uchar *) iblock;
    uchar *zero = (uchar *) izero;
    unsigned lineoffs;
    unsigned byteoffs;

    /* Loop over lines... 16 bytes to the line */
    for (lineoffs = 0; lineoffs < len; lineoffs += 16) {
	/* Print an address heading at the beginning of each line */
	printf ("\n  %04x(%04d): ", (block + lineoffs) - zero,
		(block + lineoffs) - zero);

	/* Now, print the hex data */
	for (byteoffs = lineoffs;
	     (byteoffs < (lineoffs + 16)) && (byteoffs < len);
	     byteoffs++) {
	    if (((byteoffs-lineoffs) % 4) == 0) {
		printf (" ");
		if ((byteoffs-lineoffs) == 8)
		    printf (" ");
	    }
	    printf ("%02x", block[byteoffs]);
	}
	/* If we didn't do a full line of hex, pad it out with spaces... */
	for (; byteoffs < (lineoffs + 16); byteoffs++) {
	    if (((byteoffs-lineoffs) % 4) == 0) {
		printf (" ");
		if ((byteoffs-lineoffs) == 8)
		    printf (" ");
	    }
	    printf ("  ");
	}

	/* Tab over to the beginning of the ASCII field */
	printf ("      ");

	/* Now do the ASCII column */
	for (byteoffs = lineoffs;
	     (byteoffs < (lineoffs + 16)) && (byteoffs < len);
	     byteoffs++) {
	    printf ("%c",
		    ((block[byteoffs] >= ' ') && (block[byteoffs] < 0x7f)) ?
		    block[byteoffs] : '.');
	}
    }
}
	

/* pgen_commands is the dispatch routine the parser calls to
 * process packet generator commands.
 */
void pgen_commands (parseinfo *csb)
{
    switch (csb->which) {
    case PGEN_ADDPATCH:
	pgen_add_patch_command(csb, PGEND(csb), GETOBJ(int,1), 
			       GETOBJ(string,1), GETOBJ(string,2),
			       GETOBJ(int,5), GETOBJ(int,6),
			       GETOBJ(int,7), (uchar *) GETOBJ(string,4),
			       GETOBJ(int,4), GETOBJ(int,8), GETOBJ(int,9),
			       GETOBJ(int,10),GETOBJ(idb,1), GETOBJ(string,3));
	break;
    case PGEN_CLEAR:
	pgen_clear_command (PGEND(csb), GETOBJ(string,1), GETOBJ(string,2));
	break;
    case PGEN_COUNT:
	pgen_count_command (PGEND(csb), GETOBJ(int,1), GETOBJ(idb,1));
	break;
    case PGEN_EXAMINE:
	pgen_examine_command (PGEND(csb), GETOBJ(string,1), GETOBJ(string,2),
			      GETOBJ(int,1));
	break;
    case PGEN_INTERFACE:
	pgen_interface_command (PGEND(csb), GETOBJ(idb,1));
	break;
    case PGEN_SORT:
	pgen_sort_command(PGEND(csb), GETOBJ(string,1), GETOBJ(string,2));
	break;
    case PGEN_BUFFER:
	pgen_buffer_command(PGEND(csb), GETOBJ(int,2));
	break;
    case PGEN_DUMPCMDS:
	pgen_dump_cmds_command(PGEND(csb), GETOBJ(string,1), GETOBJ(string,2),
			       GETOBJ(int,1), GETOBJ(string,3),
			       GETOBJ(paddr,1));
	break;
    case PGEN_LOAD:
	pgen_load_command(PGEND(csb), GETOBJ(int,1), GETOBJ(int,4),
			  GETOBJ (int,2), GETOBJ(string,3), GETOBJ(paddr,1));
	break;
    case PGEN_REPLAY:
	pgen_replay_command(PGEND(csb), GETOBJ(string,1), GETOBJ(string,2),
			    GETOBJ(int,4), GETOBJ(int,5), GETOBJ(int,6));
	break;
    case PGEN_QUIT:
	pgen_quit_command(csb, GETOBJ(int,1));
	break;
    case PGEN_VERSION:
	pgen_version_command(csb);
	break;
    default:
	printf("\nPacket generator parsing error: bad \"which\" value");
	break;
    }
}

/* pgen_hexmode is the dispatch routine for the hex data parsing
 * mode. It accumulates data onto the end of the hex buffer in the packet
 * generator context until it runs out of space. When the user types "q",
 * it gets out of hex mode (into which it was put by pgen_add_patch_command)
 * and calls pgen_do_add_patch to actually do the user's bidding.
 */
void pgen_hexmode (parseinfo *csb)
{
    unsigned cpylen;		/* Number of bytes to be copied */

    switch (csb->which) {

    /* User typed a line of data. OBJ(int,1) is the length, and OBJ(string,1)
     * contains the actual data.
     */
    case PGEN_HEX_DATA:
	cpylen = GETOBJ(int,1);	/* Get length of data parsed */
	if ((cpylen + PGEND(csb)->apedlen) > PGEN_HEXBUFSIZE) {
	    printf ("\nData too long for parse buffer. Truncating.");
	    cpylen = PGEN_HEXBUFSIZE - PGEND(csb)->apedlen;
	}
	bcopy (GETOBJ(string,1),
	       (PGEND(csb)->apeddata + PGEND(csb)->apedlen),
	       cpylen);
	PGEND(csb)->apedlen += cpylen;
	break;

    /* User is done with hex mode. */
    case PGEN_HEX_QUIT:
	/* Switch back to regular packet generator command mode */
	set_mode_byname(&csb->mode, "pgen", MODE_VERBOSE);

	/* We wouldn't have gotten into hex mode if an "add" or "patch"
	 * weren't waiting to happen. The parameters for that "add" or
	 * patch are saved in the packet generator context pointed to
	 * by the CSB. Call pgen_do_add_patch to do the actual work.
	 */
	pgen_do_add_patch(PGEND(csb));
	break;

    /* This should never happen */
    default:
	printf ("\nHex-mode parsing error: bad \"which\" value");
	break;
    }
}


/* pgen_rangechk takes beginning-of-range and end-of-range strings
 * parsed from a user command line and
 *
 * o Converts symbolic names to integer packet numbers, complaining
 *   if it can't find any packet with a given name.
 *
 * o Treats any range with "*" as the first element as covering all
 *   the packets in the workspace.
 *
 * o Converts wholly numeric strings to packet numbers, optionally
 *   checking to see if the numbers the user specified refer to
 *   packets that actually exist.
 *
 * o Makes sure the "first" packet really is at or before the "last"
 *   packet, and adjusts the range if it isn't.
 *
 * o Handles the case where the user gave only one packet name to
 *   specify a single-packet range.
 *
 * The function returns TRUE if everything looked OK, FALSE otherwise.
 *
 * Some of this code probably belongs in the parser, but the idea of
 * writing a new parser action routine for a packet generator range is
 * just too daunting...
 */
boolean pgen_rangechk (
    pgen_context *context,	/* Current context */
    char *begstr,		/* Name or number of first packet */
    char *endstr,		/* Name or number of last packet */
    unsigned *begnum,		/* Filled in with output "first" packet num */
    unsigned *endnum,		/* Filled in with output "last" packet num */
    boolean existchk)		/* Make sure packets actually exist? */
{
    unsigned i;			/* Index for scanning strings */
    unsigned wslen = pgen_wscount (&(context->workspace));

    /* If we get a null first string (really shouldn't happen) or a
     * first string of "*", the range is the entire workspace.
     */
    if ((begstr == NULL) || (begstr[0] == '\0') || (!strcmp(begstr, "*"))) {
	if (wslen == 0) {
	    printf ("\nNo packets in the workspace");
	    return FALSE;
	}
	*begnum = 1;
	*endnum = wslen;
	return TRUE;
    }

    /* Scan through the first string looking for nonnumeric characters.
     * If we find any, the string is taken as a symbolic packet name. If 
     * not, it's just a number. If it *is* a number, check to make sure
     * it's in range.
     */
    for (i=0; begstr[i] != '\0'; i++)
	if (!isdigit(begstr[i])) break;
    if (begstr[i] != '\0') {
	if (!strcmp(begstr, "$")) {
	    if (wslen == 0) {
		printf ("\nNo packets in the workspace");
		return FALSE;
	    }
	    *begnum = wslen;
	} else if (!strcmp(begstr, "$$")) {
	    if (existchk) {
		printf ("\nOne-off-end (\"$$\") not valid here");
		return FALSE;
	    }
	    *begnum = wslen + 1;
	} else {
	    *begnum = pgen_wsbyname (&(context->workspace), begstr);
	    if (*begnum == 0) {
		printf ("\nNo packet named %s", begstr);
		return FALSE;
	    }
	}
    } else {
	*begnum = atoi(begstr);
	if (*begnum == 0) {
	    printf ("\nPacket numbers start at one");
	    return FALSE;
	}
	if (existchk && (*begnum > wslen)) {
	    printf ("\nPacket %d out of range; workspace is only %d long.",
		    *begnum, wslen);
	    return FALSE;
	}
    }

    /* Second string is treated similarly to the first if it exists. Otherwise,
     * we take the value *from* the first, to make a one-packet range. A
     * "*" as the second string means the last packet in the workspace.
     */
    if ((endstr != NULL) && (*endstr != '\0')) {
	for (i=0; endstr[i] != '\0'; i++)
	    if (!isdigit(endstr[i])) break;
	if (endstr[i] != '\0') {
	    if (!strcmp(endstr, "*"))
		*endnum = wslen;
	    else if (!strcmp(endstr, "$")) {
		if (wslen == 0) {
		    printf ("No packets in workspace");
		    return FALSE;
		}
		*endnum = wslen;
	    } else if (!strcmp(endstr, "$$")) {
		if (existchk) {
		    printf ("\nOne-off-end (\"$$\") not valid here");
		    return FALSE;
		}
		*endnum = wslen + 1;
	    } else {
		*endnum = pgen_wsbyname (&(context->workspace), endstr);
		if (*endnum == 0) {
		    printf ("\nNo packet named %s", endstr);
		    return FALSE;
		}
	    }
	} else {
	    *endnum = atoi(endstr);
	    if (*endnum == 0) {
		printf ("\nPacket numbers start at one");
		return FALSE;
	    }
	    if (existchk && (*endnum > wslen)) {
		printf ("\nPacket %d out of range; workspace is only %d long.",
			*endnum, wslen);
		return FALSE;
	    }
	}
    } else
	*endnum = *begnum;

    /* If the "last" comes before the first, swap them. */
    if (*endnum < *begnum) {
	i = *endnum;
	*endnum = *begnum;
	*begnum = i;
    }

    return TRUE;
}

/* pgen_parse_buffer runs packet generator commands read from a buffer
 * full of data. The code is a shameless ripoff of parse_configure.
 */
void pgen_parse_buffer (
    char *buffer,		/* Buffer of commands to parse */
    pgen_context *context)	/* Packet generation context */
{
    int error;
    parseinfo *csb;

    csb = get_csb("PGEN Parser CSB");
    if (csb == NULL) {
	printf ("\nCan't get memory to parse commands");
	return;
    }
    csb->priv = PRIV_ROOT;
    set_mode_byname(&csb->mode, "pgen", MODE_VERBOSE);
    csb->break_chars = NVCONFIG_BREAKSET;
    csb->batch_buf = buffer;
    csb->batch_offset = buffer;
    csb->flags = 0;
    SETPGEND(csb, context);
    /* Things like the "quit" command don't want to work in batch mode.
     * Set a flag to let them know we're in batch.
     */
    context->batchdepth++;
    while (batch_input(csb, csb->line, PARSEBUF, 
		       csb->break_chars, &csb->lastchar)){
	if (*csb->line && (*csb->line != '!') ){    /* Skip empty lines */
	    if ((error = parse_cmd(csb)) != 0){
		report_error(csb, error, TRUE);
	    }
	    if( strcmp(csb->line, "end") == 0 ){
		break;
	    }
	}
    }
    /* Done doing batch */
    context->batchdepth--;
    /* Mind our Free's & Q's */	
    free_csb( &csb );
}

/* pgen_dumppaks creates a series of commands that, if reread, would
 * regenerate a range of packets, appending them to the then-existing
 * workspace. It puts the command text in a buffer or on a tty, and returns
 * the size of the finished text. If it's given a null buffer pointer and
 * a tty value of NULLTTY, it doesn't actually create the data, but returns
 * the length the buffer *would* have had if it had done so.
 *
 * Instead of setting the interface attribute on each packet using the
 * common_str_interface clause on the "add" command, the file created sets the
 * default interface before doing any adds, and changes it whenever it
 * gets to a packet with a different interface than the preceding one.
 * This has the disadvantage that the default interface is changed when
 * the file is loaded, but it has the advantage that the packets will
 * end up getting loaded *somehow* even if the requested interface doesn't
 * exist on the system doing the loading.
 *
 * This routine doesn't do any size checking on the output buffer. It
 * expects the calling routine to make sure the buffer is big enough,
 * presumably by precounting the output size before the buffer is
 * allocated. Likewise, it doesn't do any range checking on the "first"
 * and "last" arguments; they're just supposed to be right.
 */
unsigned pgen_dumppaks (
    pgen_context *context,
    unsigned first,
    unsigned last,
    char *buffer,
    tt_soc *tty)
{
    unsigned size = 0;		/* Accumulates total size of output */
    unsigned pindex;		/* Index in the workspace for current packet */
    unsigned bindex;		/* Offset within packet data */
    paktype *pak;		/* Pointer to current packet */
    idbtype *lastif = NULL;	/* Interface of last packet printed */
    char *curptr = buffer;	/* Pointer to where we're writing in buffer */

    for (pindex = first; pindex <= last; pindex++) {
	pak = pgen_wspick (&(context->workspace), pindex);

	/* Set up default interface if needed */
	if (pak->pgen_output != lastif) {
	    size += btprintf (&curptr, tty,
                "!\ninterface %s\n", pak->pgen_output->namestring);
	    lastif = pak->pgen_output;
	}

	/* Separator*/
	size += btprintf (&curptr, tty, "!\n");
	
	/* Command to create the packet, set its attributes, and fill in
	 * its data. First, we print the invariant part. Packets with data
	 * will also get trailing "byte" clauses, and packets with names
	 * will get "name" clauses.
	 */
	size += btprintf (&curptr, tty,
			  "add $$ time %d repeat %d",
			  pak->pgen_ttosend,
			  pak->pgen_rptcount);

	/* Have a name? */
	if (pak->pgen_nameptr != NULL)
	    size += btprintf (&curptr, tty, " name \"%s\"", pak->pgen_nameptr);

	/* How about contents? */
	if (pak->datagramsize == 0)
	    /* No data */
	    size += btprintf (&curptr, tty, "\n");
	else {
	    /* Packet has data */
	    size += btprintf (&curptr, tty, " byte 0");
	    for (bindex = 0; bindex < pak->datagramsize; bindex++) {
		/* Lines are 32 bytes long. Space after every 4 bytes. */
		if ((bindex % 32) == 0)
		    size += btprintf (&curptr, tty, "\n");
		else if ((bindex % 4) == 0)
		    size += btprintf (&curptr, tty, " ");
		/* Print actual data byte */
		size += btprintf (&curptr, tty, "%02x",
				  (pak->datagramstart)[bindex]);
	    }
	    /* Last data byte printed. Need a "quit" command. */
	    size += btprintf (&curptr, tty, "\nquit\n");
	}
	
	/* Let other people run after each packet */
	process_may_suspend ();
    }

    /* If we've been writing to a buffer, add a terminating null. Regardless,
     * allow space for it.
     */
    if (buffer != NULL)
	*curptr = '\0';
    size++;

    /* All done. */
    return size;
}

/* btprintf is a weird printf variant. It takes two initial arguments:
 * a pointer to a pointer to a buffer (bufferp), and a pointer to a
 * tt_soc structure (tty), followed by the traditional format string and
 * data to be formatted. It always returns the length of the formatted
 * data. The tty and bufferp arguments are used as follows:
 *
 * o If tty is NULLTTY and both bufferp and *bufferp are non-NULL, the
 *   formatted data are appended to the buffer at *bufferp, and *bufferp
 *   is adjusted to point to the location following them.
 *
 * o If tty is NULLTTY and either bufferp or *bufferp is NULL, the formatted
 *   data are thrown away, but the length of the data that would otherwise
 *   have been created is still returned. This is useful for figuring out how
 *   much space a buffer will need to have.
 *
 * o If tty is not NULLTTY and either bufferp or *bufferp is NULL, the
 *   formatted data are printed on the appropriate tty.
 *
 * o If tty is not NULLTTY and both bufferp and *bufferp are non-NULL,
 *   what happens is undefined.
 */

unsigned btprintf (char **bufferp, tt_soc *tty, char *fmt, ...)
{
    va_list ap;			/* Context for grabbing args off stack */
    int len;			/* Length of formatted data */

    /* Get arguments */
    va_start(ap, fmt);

    /* Do formatted output. Update *bufferp if appropriate. */
    if (bufferp != NULL) {
	len = _doprnt (*bufferp, tty, fmt, ap, FALSE);
	if (*bufferp != NULL)
	    *bufferp += len;
    } else {
	len = _doprnt (NULL, tty, fmt, ap, FALSE);
    }

    return len;
}

/*
 * Default registry service, disables fast counting on unsupported platforms.
 */
void pgen_count (
    hwidbtype *hwidb,           /* Hardware idb for counting interface */
    unsigned mode)              /* Mode: 0 == off, 1 == on, 2 == status */
{
    printf("\n%%Fast counting is not supported on this platform");
}
