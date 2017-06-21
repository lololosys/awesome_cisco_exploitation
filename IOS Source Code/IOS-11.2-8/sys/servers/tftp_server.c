/* $Id: tftp_server.c,v 3.7.18.6 1996/09/06 21:01:05 snyder Exp $
 * $Source: /release/112/cvs/Xsys/servers/tftp_server.c,v $
 *------------------------------------------------------------------
 * tftp_server.c -- Server for remote TFTP bootstrap requests
 *
 * 27-April-1990, Kevin Paul Herbert
 *
 * Copyright (c) 1990-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: tftp_server.c,v $
 * Revision 3.7.18.6  1996/09/06  21:01:05  snyder
 * CSCdi68334:  dump unused spinner support, save memory
 *              412 image, 4 data
 * Branch: California_branch
 *
 * Revision 3.7.18.5  1996/08/03  01:29:40  ahh
 * CSCdi59753:  tftp server code does not perform next hop routing properly
 * Nail down the source address, not the output interface, so as not
 * to short-circuit routing.
 * Branch: California_branch
 *
 * Revision 3.7.18.4  1996/06/28  23:25:46  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.7.18.3  1996/05/04  01:37:53  wilber
 * Branch: California_branch
 *
 * Name Access List
 *
 * Revision 3.7.18.2  1996/04/15  21:22:47  bcole
 * CSCdi54649:  IP Host Modularity, IP Automatic Stub Routing feature
 * commit
 * Branch: California_branch
 *
 * Revision 3.7.18.1  1996/03/18  21:47:12  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.5.12.2  1996/03/07  10:42:18  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.5.12.1  1996/02/20  17:10:04  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.7  1996/02/07  16:15:10  widmer
 * CSCdi48285:  Remove function name from bad_parser_subcommand
 *
 * Revision 3.6  1996/01/24  20:19:57  jenny
 * CSCdi47046:  TCP/IP need code clean up (part 3)
 * . removed ip/tuba.c, ip/tuba_default.c and ip/tuba.h
 * . removed all includes for ip/tuba.h.
 * . removed more TUBA related code.
 *
 * Revision 3.5  1995/12/28  18:28:37  jenny
 * CSCdi46011:  TCP/IP need to do code clean up.
 * This is the first patch of code changes for TCP/IP code cleanup.
 * . removed all references to TUBA_IP and TUBA_CLNS.
 * . renamed open_tuba_socket() to ip_open_socket().
 * . renamed tcp_tuba_open() to tcp_open().
 * . pass addrtype * instead of ipaddrtype for local and foreign addresses
 *   to tcp_open() for future portability.
 *
 * Revision 3.4  1995/12/13  23:45:22  jenny
 * CSCdi44339:  configuring a dump host breaks tftp server
 *
 * Revision 3.3  1995/11/22  17:49:10  albright
 * CSCdi43791:  Helper addresses dont work over unnumbered interfaces
 *
 * Revision 3.2  1995/11/17  18:52:04  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:12:12  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/10/20  18:06:15  albright
 * CSCdi42154:  ip helper-address fails on unnumbered interfaces
 *
 * Revision 2.3  1995/08/03  07:42:48  hampton
 * EIGRP stops processing input.  Convert the IP socket input queue into a
 * watched_queue.  This allows the scheduler to see if there is input
 * waiting when the process dismisses.  [CSCdi38156]
 *
 * Revision 2.2  1995/06/09 13:18:45  hampton
 * Rename old scheduler calls to fit the new naming convention.
 * [CSCdi35514]
 *
 * Revision 2.1  1995/06/07 22:44:30  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <ciscolib.h>
#include "../dev/monitor1.h"
#include "interface_private.h"
#include "sys_registry.h"
#include "../dev/flash_registry.h"
#include "packet.h"
#include "../ui/debug.h"
#include "../ip/ip.h"
#include "access.h"
#include "../ip/ipaccess.h"
#include "../ip/udp_debug.h"
#include "config.h"
#include "parser.h"
#include "file.h"
#include "../parser/parser_defs_misc.h"
#include "../os/boot.h"
#include "../ip/tftp.h"
#include "../ip/ip_registry.h"

extern boolean tftp_debug;
tftpdatatype *tftpdata;

#define ROMIMAGE_ABSENT	-1

/*
 * Structures defining header layout of EPROMs
 */
struct cload {
    uchar *addr;			/* Absolute physical address */
    ulong len;				/* Length of data */
};

struct cdata {
    ushort opaque1;
    short number;			/* Number of banks */
    ushort opaque2;
    struct cload bank[0];		/* Per bank info */
};

/*
 * tftp_serve_last_block
 * Handle the last block of the entire transfer.  Must send
 * a block that isn't TFTP_MAXDATA in size to indicate completion.
 */

static void tftp_serve_last_block (ipsocktype *soc, int blk, uchar *addr, int len)
{
    boolean success;

    success = reg_invoke_ip_tftp_sendblock(soc, blk++, addr, len, TRUE);
    if ((len == TFTP_MAXDATA) && success) {
	(void) reg_invoke_ip_tftp_sendblock(soc, blk, addr, 0, TRUE);
    }
}

/*
 * tftp_serve_eprom
 * Serve EPROM image via TFTP
 */

static void tftp_serve_eprom (ipsocktype *soc, struct cdata *romaddr)
{
    uchar *block, *segaddr, *addr;
    boolean success;
    ulong seglen, nblks, len, partial, i, j, blk;
    ushort nbanks;

    /* Before we go ahead, check if there's a ROM image to serve.
     * Default value if ROM image is absent is -1. But it seems the
     * 4000 (and others ?) return NULL. So check for both values
     * and return if ROM image not present.
     */
    if ((romaddr == NULL) || (romaddr == (struct cdata *)ROMIMAGE_ABSENT)) {
        if (tftp_debug)
	    buginf("\nTFTP: no ROM file to serve for socket_id %#x", soc);
        return;
    }
    if (tftp_debug)
	buginf("\nTFTP: sending server's ROM file, socket_id %#x", soc);
    block = malloc(TFTP_MAXDATA);
    if (block == NULL)
	return;

    /*
     * Get number of rom data segments
     */
    nbanks = romaddr->number;

    segaddr = romaddr->bank[0].addr;
    if (!segaddr) {
        if (tftp_debug)
	    buginf("\nTFTP: Cannot find address of EPROM image");
	return;
    }
    seglen = romaddr->bank[0].len;
    blk = 0;
    success = TRUE;
    for (i = 0; i < nbanks; i++) {
	addr = segaddr;
	len = seglen;
	nblks = (seglen / TFTP_MAXDATA) + 1;
	for (j = 1; j < nblks; j++) {
	    success = reg_invoke_ip_tftp_sendblock(soc, blk + j, addr,
						   TFTP_MAXDATA, TRUE);
	    if (!success) {
		free(block);
		return;
	    }
	    addr += TFTP_MAXDATA;
	    len -= TFTP_MAXDATA;
	}

	/*
	 * Now have between 0 and TFTP_MAXDATA bytes left (inclusive)
	 * to transmit from current bank.  Value is in len.
	 */

	if (i == nbanks - 1) {
	    tftp_serve_last_block(soc, blk + j, addr, len);
	} else {
	    /*
	     * Have to build block of data since rom image
	     * becomes non-contiguous at this point.
	     */
	    bcopy(addr, block, len);
	    segaddr = romaddr->bank[i+1].addr;
	    seglen = romaddr->bank[i+1].len;
	    partial = TFTP_MAXDATA - len;
	    if (partial >= seglen) {
		/*
		 * The next bank has TFTP_MAXDATA or less bytes of data.
		 * This non-contiguous block must be the last block of the
		 * transfer.
		 */
		if (seglen)
		    bcopy(segaddr, block+len, seglen);
		tftp_serve_last_block(soc, blk + j, block, len + seglen);
		break;
	    }
	    bcopy(segaddr, block+len, partial);
	    segaddr += partial;
	    seglen -= partial;
	    success = reg_invoke_ip_tftp_sendblock(soc, blk + j, block,
						   TFTP_MAXDATA, TRUE);
	    if (!success)
		break;
	    blk += nblks;
	}
    }
    free(block);
}

/*
 * tftp_server
 * Actual process that listens for TFTP requests
 */

#define	TFTP_TIMEOUT	(ONEMIN)

forktype tftp_server (ipsocktype *soc)
{
    paktype *pak;
    idbtype *idb;
    ushort *data;
    udptype *udp;
    tftpdatatype *tp;
    struct cdata *romaddr;
    uchar *offset;
    register ipsocktype *psoc;
    iphdrtype *ip;
    char *filename_id;

    if (tftp_debug)
	buginf("\nTFTP: Server request for port %d, socket_id %#x",
	       soc->fport, soc);
    romaddr = (struct cdata *)mon_romaddr();
    while (TRUE) {
	pak = read_ipsocket(soc,TRUE,TFTP_TIMEOUT,&udp,&offset);
	if (!pak)
	    break;
	
	/*
	 * Set up a IP pointer to the network start, just in case the
	 * frame is an IP one.
	 */
	ip = (iphdrtype *)ipheadstart(pak);

	idb = pak->if_input;
	data = (unsigned short *) offset;
	if (tftp_debug)
	    buginf("\nTFTP: %s from host %s(%d) via %s",
		   tftp_opcode_string(n2h(*data)),
		   address_string(&soc->faddress),
		   udp->sport, idb->namestring);

	/*
	 * Don't believe martians
	 */
	if (martian(ip->srcadr, TRUE)) {
	    retbuffer(pak);
	    continue;
	}
	switch (n2h(*data++)) {
	case RRQ:
	    /*
	     * Check that filename is null terminated
	     */
	    offset = (uchar *)data;
	    while (offset < pak->datagramstart + pak->datagramsize) {
		if (*offset == '\0')
		    break;
		offset++;
	    }
	    if (*offset++ != '\0')
		break;
	    /*
	     * Check that it matches our filename
	     */
	    tp = tftpdata;
            /*
             * With a TFTP transfer, there is no dev component.
             * The reg_invoke_flash_file_match() call will perform
             * a comparison on the whole dev:part:filename, therefore
             * pass tftpdata filename ptr positioned past the  dev component.
             */
	    while (tp) {
                filename_id = tp->filename;
	        while ((*filename_id != ':') && (*filename_id != '\0')){
	           filename_id++;
	        }
	        if (*filename_id == ':')
	           filename_id++;
	        else
	           filename_id = tp->filename;

		if (tp->alias && !strcasecmp(tp->alias, (char *)data))
		    break;
		if ((tp->filelocation == TFTP_ROM) &&
		    !strcasecmp(tp->filename, (char *)data))
		    break;
		if ((tp->filelocation == TFTP_FLASH) &&
		    (!tp->alias) &&
		    reg_invoke_flash_file_match(filename_id, (char *)data))
		    break;
		tp = tp->next;
	    }
	    if (!tp) {
		if (!ip_broadcast(ip->dstadr, idb))
		    tftp_ERROR(soc, ERROR_NOFILE, "No such file");
		break;
	    }

	    /*
	     * Make sure that the mode is null terminated
	     */
	    data = (ushort *)offset;
	    while (offset < pak->datagramstart + pak->datagramsize) {
		if (*offset == '\0')
		    break;
		offset++;
	    }
	    if (*offset != '\0')
		break;
	    /*
	     * Check mode of matched filename
	     */
	    if (!ip_old_accesscheck(pak, tp->acl))
		break;
	    
	    /*
	     * We were seeing client may spawn multiple tftp
	     * sessions to attempt to netboot. This happens when there are
	     * multiple parallel paths from the client to the server.
	     * This might be removed later when a client fix is done.
	     */
	    for (psoc = (ipsocktype *)IPsocketQ.qhead; psoc; psoc=psoc->next) {
		if ((psoc != soc) && 
		    (psoc->protocol == UDP_PROT) &&
		    (psoc->lport == TFTP_PORT) && 
		    (psoc->faddress.ip_addr == soc->faddress.ip_addr) &&
		    (psoc->fport == soc->fport)) {
		    if (tftp_debug)
			buginf("\nTFTP: 2nd request for server download"
			       " ignored, socket_id %#x", soc);
		    reg_invoke_ip_close_socket(soc);
		    udp_decrement_server_count();
		    process_kill(THIS_PROCESS);
		}
	    }

	    /*
	     * See if the selected filename resides in flash memory.
	     * If so, serve it up.
	     * Otherwise, fall through and try to serve rom image.
             * If the ROM image does not exist, romaddr will be either
             * NULL or -1. tftp_serve_eprom() will do the right thing 
             * and return back without causing any fireworks.
	     */
	    if (!reg_invoke_flash_file_server(soc, tp->filename,
					      (udp_debug | tftp_debug)))
                tftp_serve_eprom(soc, romaddr);
	    break;
	case WRQ:
	    tftp_ERROR(soc, ERROR_ACCESS, "Access denied");
	    break;
	case ACK:
	case DATA:
	case ERROR:
	    break;
	default:
	    tftp_ERROR(soc, ERROR_ILLOP, "Illegal operation");
	    break;
	}
	retbuffer(pak);
    }
    reg_invoke_ip_close_socket(soc);
    udp_decrement_server_count();
    process_kill(THIS_PROCESS);
}

/*
 * tftp_service
 * Parse the tftp-server command options
 * [no] tftp-server system <filename> [<list>]
 * [no] tftp-server rom alias <filename> [<list>]
 * [no] tftp-server flash <filename1> [alias <filename2>] [<list>]
 */
void tftp_service (parseinfo *csb)
{
    uchar *romaddr;
    tftpdatatype *tp, *ptp;
    char *alias, *filename;

    romaddr = mon_romaddr();

    if (csb->nvgen) {
	switch (csb->which) {
	case TFTP_SYSTEM:
	    return;
	case TFTP_ROM:
	    if (!tftp_serving) 
		return;

            /* If the system does not have a ROM image, return.
             * Do not create the 'tftp-server rom' entry.
             */
            if ((romaddr == (uchar *)ROMIMAGE_ABSENT) || (romaddr == NULL))
	        return;

	    tp = tftpdata;
	    for ( ; tp; tp = tp->next) {
		if (tp->filelocation != TFTP_ROM)
		    continue;
		nv_write(TRUE, "%s alias %s", csb->nv_command,
			 tp->filename);
		nv_add(tp->acl, " %d", tp->acl);
	    }
	    break;
	case TFTP_FLASH:
	    if (!tftp_serving) 
		return;

	    tp = tftpdata;
	    for ( ; tp; tp = tp->next) {
		if (tp->filelocation != TFTP_FLASH)
		    continue;
		nv_write(TRUE, "%s %s", csb->nv_command,
			 tp->filename);
		nv_add(tp->alias != NULL, " alias %s", tp->alias);
		nv_add(tp->acl, " %d", tp->acl);
	    }
	    break;
	default:
	    bad_parser_subcommand(csb, csb->which);
	    break;
	}
        return;
    }

    /*
     * First, look for a match in the database.  The filename and file
     * location must match.
     */
    alias = NULL;
    filename = NULL;
    setstring(&filename, GETOBJ(string,1));
    setstring(&alias, GETOBJ(string,2));
    for (ptp = tp = tftpdata ; tp; ptp = tp, tp = tp->next) {
	if (tp->filelocation != csb->which)
	    continue;
	if (!strcasecmp(filename, tp->filename)) {
	    if (!csb->sense)
		break;
	    if (!alias && !tp->alias)
		break;
	    if (alias && !strcasecmp(alias, tp->alias))
		break;
	}
    }

    if (!csb->sense) {

	/*
	 * Delete the database entry, if any.
	 */
	if (!tp) 
	    printf("\nFile name %s not found", filename);
	else {
	    if (tp == ptp) 
		tftpdata = tp->next;
	    else 
		ptp->next = tp->next;
	    free(tp->filename);
	    free(tp);
	}
	if (!tftpdata) 
	    tftp_serving = FALSE;
	free(filename);
	return;
    }

    /*
     * Must be a request to add service.
     */
    switch (csb->which) {
    case TFTP_SYSTEM:

	/*
	 * For backwards compatibility, try to figure out where the file is
	 * for the user.  Then convert to use the resulting file location.
	 */
	if (reg_invoke_file_exists(FILE_ACCESS_FLASH, filename))
	    csb->which = TFTP_FLASH;
	else
	    csb->which = TFTP_ROM;

	/*
	 * Mebbe the user forgot and is typing in old commands.  Let's help
	 * as best we can and try another lookup using the new location.
	 */
	if (!tp) {
	    for (ptp = tp = tftpdata ; tp; ptp = tp, tp = tp->next) {
		if (tp->filelocation != csb->which)
		    continue;
		if (!strcasecmp(filename, tp->filename)) 
		    break;
	    }
	}
	break;
    case TFTP_ROM:
	/* bail */
	break;
    case TFTP_FLASH:
	if (!reg_invoke_file_exists(FILE_ACCESS_FLASH, filename))
	    printf("\nWarning: %s not in Flash for TFTP serving", filename);
	break;
    default:
	free(filename);
    	bad_parser_subcommand(csb, csb->which);
	return;
    }

    /*
     * A 'tftp-server system' command may have been converted
     * to a 'tftp-server rom' command if the file was not found 
     * in flash. Check for such a conversion and reject it if
     * the ROM monitor reported that there's no ROM image.
     */
    if (csb->which == TFTP_ROM)
        if ((romaddr == (uchar *)ROMIMAGE_ABSENT) || (romaddr == NULL)) {
            printf("\nTFTP serving of ROM file not supported or ROM "
                   "file not present");
	    free(filename);
            return;
        }

    if (tp) {

	/*
	 * Update alias, access list number if it is present
	 */
	if (GETOBJ(int,1) > 0) 
	    tp->acl = GETOBJ(int,1);
	else 
	    tp->acl = 0;
	free(tp->alias);
	tp->alias = alias;
    } else {

	/*
	 * Entry not present; add it in
	 */
	tp = malloc(sizeof(tftpdatatype));
	if (!tp) {
	    free(filename);
	    free(alias);
	    return;
	}
	tp->filename = filename;
	filename = NULL;
	tp->alias = alias;
	tp->filelocation = csb->which;
	if (GETOBJ(int,1) > 0) 
	    tp->acl = GETOBJ(int,1);
	else 
	    tp->acl = 0;
	tp->next = NULL;
	if (!tftpdata) 
	    tftpdata = tp;
	else 
	    ptp->next = tp;
    }
    tftp_serving = TRUE;
    free(filename);
}

/*
 * ip_tftp_server_init
 */

ipsocktype *ip_tftp_server_init (addrtype *newsrc, addrtype *faddr, 
				 ushort dport, ushort sport)
{
    ipsocktype *soc;
    pid_t pid;
    addrtype laddr;

    if (!tftp_serving)
	return(NULL);

    /*
     * Allow configuration to override the source address.
     * Note that this may cause the server to respond to the client
     * from a different address.
     */
    if (tftp_get_laddr(&laddr))
	newsrc = &laddr;

    /*
     * Nail down source IP address so that it doesn't change (via
     * fixup_ipsockets()) when an interface flaps.  Otherwise, TFTP
     * clients stop seeing our response packets.
     */
    soc = open_ipsocket(UDP_PROT, newsrc, faddr, dport, sport,
			SOCK_SRC_SPECIFIED);
    if (soc) {
	pid = cfork((forkproc *)tftp_server, (long)soc, 0, "TFTP Server", 0);
	if (pid == NO_PROCESS) {
	    close_ipsocket(soc);
	    return(NULL);
	} else {
	    udp_increment_server_count();
	}
    }
    return(soc);
}
