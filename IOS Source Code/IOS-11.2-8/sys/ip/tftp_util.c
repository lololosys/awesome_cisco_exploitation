/* $Id: tftp_util.c,v 3.5.22.6 1996/08/28 12:50:27 thille Exp $
 * $Source: /release/112/cvs/Xsys/ip/tftp_util.c,v $
 *------------------------------------------------------------------
 * tftp_util.c -- TFTP-based Utility Functions
 *
 * 16-July-1993, Kirk Lougheed
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 * This file contains various IP-centric file access commands based
 * on the TFTP protocol.  There should not be TFTP support routines here.
 *------------------------------------------------------------------
 * $Log: tftp_util.c,v $
 * Revision 3.5.22.6  1996/08/28  12:50:27  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.5.22.5  1996/05/21  09:51:40  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.5.22.4  1996/05/18  02:14:09  ahh
 * CSCdi58040:  Copying empty NVRAM causes reload
 * nv_review_buffer() can return NULL, so check for it.  Also swap
 * sstrncpy() args so incoming rcps work.
 * Branch: California_branch
 *
 * Revision 3.5.22.3  1996/04/15  21:16:59  bcole
 * CSCdi54649:  IP Host Modularity, IP Automatic Stub Routing feature
 * commit
 * Branch: California_branch
 *
 * Revision 3.5.22.2  1996/04/15  14:58:10  widmer
 * CSCdi47180:  No Single Command to Collect General Router Data
 * Branch: California_branch
 * Add "show tech-support" command
 * Change references to encryption types to uint
 * Change tt_soc to use password_struct
 *
 * Revision 3.5.22.1  1996/03/18  20:17:51  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.4.12.3  1996/03/09  05:06:43  hampton
 * Slim down the kernel of the router.
 * Branch: DeadKingOnAThrone_branch
 *
 * Revision 3.4.12.2  1996/03/07  09:47:13  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.4.12.1  1996/02/20  01:00:41  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.5  1996/01/24  20:15:16  jenny
 * CSCdi47046:  TCP/IP need code clean up (part 3)
 * . removed ip/tuba.c, ip/tuba_default.c and ip/tuba.h
 * . removed all includes for ip/tuba.h.
 * . removed more TUBA related code.
 *
 * Revision 3.4  1995/12/28  18:27:45  jenny
 * CSCdi46011:  TCP/IP need to do code clean up.
 * This is the first patch of code changes for TCP/IP code cleanup.
 * . removed all references to TUBA_IP and TUBA_CLNS.
 * . renamed open_tuba_socket() to ip_open_socket().
 * . renamed tcp_tuba_open() to tcp_open().
 * . pass addrtype * instead of ipaddrtype for local and foreight addresses
 *   to tcp_open() for feature portability.
 *
 * Revision 3.3  1995/12/17  18:29:54  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.2  1995/11/17  09:36:53  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:58:53  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.8  1995/11/08  21:05:17  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.7  1995/09/13  00:26:59  pst
 * CSCdi39650: Incorporate code review comment
 *
 * Revision 2.6  1995/09/11  16:45:24  pst
 * CSCdi39650:  config overwrite is braindead
 *
 * Revision 2.5  1995/07/22  06:05:17  thille
 * CSCdi37049:  Console output from SNMP WriteNet SET confuses console user
 * Fix over-zealousness in limiting output of "Building Configuration..."
 *
 * Revision 2.4  1995/07/16  18:04:15  thille
 * CSCdi37049:  Console output from SNMP WriteNet SET confuses console user
 * Bad programmer, no doggie biscuit.
 *
 * Revision 2.3  1995/07/15  02:36:59  thille
 * CSCdi37049:  Console output from SNMP WriteNet SET confuses console user
 * Add a silent_mode parameter to some routines and registries and set it
 * to TRUE when making the calls from SNMP WriteNet, netConfigSet, or
 * hostConfigSet.  Also, add an errmsg for each so the event can still get
 * logged, and make it clear what caused the event.  Fix other callers to
 * pass FALSE so current behavior will be maintained.
 *
 * Revision 2.2  1995/06/19  01:06:45  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.1  1995/06/07  21:01:34  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include <ciscolib.h>
#include "sys_registry.h"
#include "interface_private.h"
#include "packet.h"
#include "ttysrv.h"
#include "config.h"
#include "logger.h"
#include "ip.h"
#include "file.h"
#include "tftp.h"
#include "name.h"
#include "exception.h"
#include "../if/network.h"
#include "../os/boot.h"
#include "../os/free.h"
#include "../os/nv.h"
#include "../ip/ip_registry.h"
#include "../parser/msg_parser.c"	/* Not a typo, see logger.h */
#include "../parser/parser_defs_flash.h"
#include "../parser/parser_actions.h"
#include "../h/aaa.h"
#include "../tcp/rcp.h"
#include "../snmp/config_history.h"


/*
 * ip_get_config_file
 * Go through a dialog to retrieve a configuration file using TFTP.
 */

void ip_get_config_file (parseinfo *csb, boolean direct_to_nv)
{
    char buff[PARSEBUF];
    int parsed, dummy;
    nametype *ptr;
    filetype confg;
    filetype *cfg = & confg;
    filetype *base = NULL;
    filetype *defcfg = NULL;
    boolean host;
    int source, dest;
    char *ruser;

    /*
     * Determine which type of configuration
     */
    cfg->filename = NULL;

    if (GETOBJ(int,1) == COPY_TFTP)
        cfg->proto = FILE_ACCESS_TFTP;
    else if (GETOBJ(int,1) == COPY_RCP)
        cfg->proto = FILE_ACCESS_RCP;
    else
	cfg->proto = FILE_ACCESS_UNKNOWN;

    cfg->ps.tuba.boothost.type = ADDR_IP;
    cfg->ps.tuba.boothost.ip_addr = -1L;
    cfg->ps.tuba.username = NULL;
    cfg->ps.tuba.display = TRUE;

    host = TRUE;

    if (!direct_to_nv) {
	printf("\nHost or network configuration file [host]? ");
	parsed = rdtty(buff,PARSEBUF);
	if (parsed) {
	    if (null(buff) || partialmatch("host", buff)) {
		host = TRUE;
	    } else if (partialmatch("network", buff)) {
		host = FALSE;
	    } else
		parsed = FALSE;
	}
	if (!parsed) {
	    printf("\n?Must be \"host\" or \"network\"");
	    return;
	}
    }

    if (host) {
	if (hostconfg.responder) {
	    defcfg = &hostconfg;
	} else {
	    defcfg = (filetype *)hostconfgQ.qhead;
	}
	if (defcfg) {
	    setstring(&cfg->filename, defcfg->filename);
	    if (defcfg->proto == cfg->proto) {
		cfg->ps.tuba.boothost = defcfg->ps.tuba.boothost;
	    }
	}
	cfg->resetsubr = direct_to_nv ? NULL : reset_host;
	base = & hostconfg;
    } else if (partialmatch("network", buff)) {
	if (netconfg.responder) {
	    defcfg = &netconfg;
	} else {
	    defcfg = (filetype *)netconfgQ.qhead;
	}
	if (defcfg) {
	    setstring(&cfg->filename, defcfg->filename);
	    if (defcfg->proto == cfg->proto) {
		cfg->ps.tuba.boothost = defcfg->ps.tuba.boothost;
	    }
	}
	cfg->resetsubr = NULL;
	base = & netconfg;
    }

    /*
     * Get or default boot hostname
     */
    printf("Address of remote host [%s]? ",
	   address_string(&cfg->ps.tuba.boothost));
    parsed = rdtty(buff,PARSEBUF);
    if (parsed) {
	if (!null(buff)) {
	    parsed = numeric_lookup(ADDR_IP, buff, &cfg->ps.tuba.boothost);
	    ptr = (nametype *) name_lookup(buff, &dummy, NAM_IP);
	    if (ptr) {
		address_copy(&cfg->ps.tuba.boothost, &ptr->v.ip.adr[0]);
		parsed = TRUE;
	    }
	}
    }
    if (!parsed) {
	printf("\n?Bad address or host name");
	return;
    }
    if (!usable_interfaces()) {
	printf("\nThere are no usable interfaces configured for IP");
	return;
    } 

    /*
     * Get or default boot filename
     */
    printf("\nName of configuration file [%s]? ", cfg->filename);
    parsed = rdtty(buff,PARSEBUF);
    if (parsed) {
	if (!null(buff))
	    cfg->filename = (char *) deblank(buff);
    }
    if (!parsed) {
	printf("\n?Bad file name");
	return;
    }

    /*
     * Have user confirm the configuration request
     */
    printf("\nConfigure using %s from %s? ", cfg->filename,
	   address_string(&cfg->ps.tuba.boothost));
    if (confirm(NULL)) {
	if (direct_to_nv)
	    dest = CONFIG_MEDIUM_STARTUP;
	else
	    dest = CONFIG_MEDIUM_RUNNING;

	ruser = reg_invoke_get_remote_username(cfg->proto);
	if (cfg->proto == FILE_ACCESS_RCP) {
	    source = CONFIG_MEDIUM_NETWORKRCP;
	} else if (cfg->proto == FILE_ACCESS_TFTP)
	    source = CONFIG_MEDIUM_NETWORKTFTP;
	else
	    source = CONFIG_MEDIUM_UNKNOWN;
	    
	if (source != CONFIG_MEDIUM_UNKNOWN) {
	    reg_invoke_config_history_event(CONFIG_COMMAND_SOURCE_COMMANDLINE, 
					    source, dest,
					    confg.ps.tuba.boothost.ip_addr,
					    confg.filename,
					    ruser);
	}
	reg_invoke_access_batch_cmd(ACCESS_BATCH_SET,TRUE);
	if (read_config_file(&confg, direct_to_nv,
			     CUR_PRIV(stdio->privilege))) {
	    base->proto = cfg->proto;
	    base->responder = TRUE;
	    bcopy(&confg.ps.tuba.respondent, &base->ps.tuba.respondent,
		  sizeof(addrtype));
	    bcopy(&confg.ps.tuba.boothost, &base->ps.tuba.boothost,
		  sizeof(addrtype));
	    setstring(&base->filename, confg.filename);
	}
	reg_invoke_access_batch_cmd(ACCESS_BATCH_SET,FALSE);
    }
}

/*
 * ip_put_config_file
 * Use TFTP to write a configuration file to an IP host
 */

void ip_put_config_file (parseinfo *csb)
{
    filetype confg;
    filetype *cfg = & confg;
    filetype *hcfg;
    nvtype *nv;
    uint size;
    int  parsed, dummy;
    char *curcfg;
    nametype *cp;
    uint buffsize;
    int source, dest;
    char *ruser;

    nv = NULL;

    /*
     * Initialize file descriptor for IP.
     */
    if (GETOBJ(int,1) == COPY_TFTP)
	cfg->proto = FILE_ACCESS_TFTP;
    else if (GETOBJ(int,1) == COPY_RCP)
	cfg->proto = FILE_ACCESS_RCP;
    else
	cfg->proto = FILE_ACCESS_UNKNOWN;

    cfg->ps.tuba.boothost.type = ADDR_ILLEGAL;
    cfg->ps.tuba.boothost.ip_addr = 0;
    cfg->ps.tuba.username = NULL;
    cfg->ps.tuba.display = TRUE;
    cfg->filename = NULL;

    /*
     * Default the filename to last loaded in
     */
    cfg->filename = hostconfgQ.qhead ?
	((filetype *) hostconfgQ.qhead)->filename : NULL;

    /*
     * Get or default boot hostname
     * Note the odd while loop.
     */
    while (hostconfg.proto == cfg->proto) {
	if (!martian(hostconfg.ps.tuba.boothost.ip_addr, TRUE)) {
	    cfg->ps.tuba.boothost = hostconfg.ps.tuba.boothost;
	    cfg->ps.tuba.boothost.type = ADDR_IP;
	    break;
	}
	if (!martian(hostconfg.ps.tuba.respondent.ip_addr, TRUE)) {
	    bcopy(&hostconfg.ps.tuba.respondent, &cfg->ps.tuba.boothost,
		  sizeof(addrtype));
	    cfg->ps.tuba.boothost.type = ADDR_IP;
	    break;
	}
	if (!hostconfgQ.qhead)
	    break;
	hcfg = (filetype *) hostconfgQ.qhead;
	if (!martian(hcfg->ps.tuba.boothost.ip_addr, TRUE)) {
	    bcopy(&hcfg->ps.tuba.boothost, &cfg->ps.tuba.boothost,
		 sizeof(addrtype));
	    cfg->ps.tuba.boothost.type = ADDR_IP;
	    break;
	}
	if (!martian(hcfg->ps.tuba.respondent.ip_addr, TRUE)) {
	    bcopy(&hcfg->ps.tuba.respondent, &cfg->ps.tuba.boothost,
		sizeof(addrtype));
	    cfg->ps.tuba.boothost.type = ADDR_IP;
	    break;
	}
	break;
    }

    cfg->resetsubr = NULL;
    printf("Remote host [");
    if (cfg->ps.tuba.boothost.ip_addr) {
	printf("%s]? ", address_string(&cfg->ps.tuba.boothost));
    } else {
	printf("]? ");
    }
    parsed = rdtty(GETOBJ(string,1),PARSEBUF);
    if (parsed) {
	if (!null(GETOBJ(string,1))) {
	    parsed = numeric_lookup(ADDR_IP, GETOBJ(string,1), &cfg->ps.tuba.boothost);
	    if (!parsed) {
		cp = name_lookup(GETOBJ(string,1), &dummy, NAM_IP);
		if (cp) {
		    address_copy(&cfg->ps.tuba.boothost, &cp->v.ip.adr[0]);
		    parsed = TRUE;
		}
	    }
	}
    }
    if (!parsed || (cfg->ps.tuba.boothost.type != ADDR_IP) ||
	martian(cfg->ps.tuba.boothost.ip_addr,TRUE)) {
	printf("\n?Bad address or host name");
	return;
    }

    /*
     * Get or default boot filename
     */
    printf("\nName of configuration file to write [%s]? ", cfg->filename);
    parsed = rdtty(GETOBJ(string,1),PARSEBUF);
    if (parsed) {
        if (!null(GETOBJ(string,1))) {
            cfg->filename = deblank(GETOBJ(string,1));
	}
    }
    if (!parsed) {
        printf("\n?Bad file name");
        return;
    }
    printf("\nWrite file %s on host %s? ", cfg->filename,
           address_string(&cfg->ps.tuba.boothost));
    if (!confirm(NULL)) {
        return;
    }
    buffsize = nv_get_config_size();
    curcfg = malloc(buffsize);

    if (curcfg == NULL) {
        return;
    }

    /*
     * for "running-config", we call nv_current_to_buffer() to get what we
     * want to send.
     * for "startup-config", we call nv_review_buffer() to get what we
     * want to send.
     */
    if (GETOBJ(int,2) == COPY_RUNNING_CONFIG) {
	size = nv_current_to_buffer(curcfg, buffsize, TRUE,
				    CUR_PRIV(stdio->privilege), FALSE);
	source = CONFIG_MEDIUM_RUNNING;
    }
    else if (GETOBJ(int,2) == COPY_STARTUP_CONFIG) {
	nv = nv_review_buffer();
	if (nv == NULL) {
	    free(curcfg);
	    return;
	}
	else if ((size = nv->textsize) > buffsize) {
		nv_done(nv);
		free(curcfg);
		printf("\nCopy aborted. Startup configuration size (%d bytes)\
\nexceeds buffer size configured via 'boot buffersize'.\n", size);
		return;
	} 

	bcopy(nv->textbase, curcfg, size);
	nv_done(nv);
	source = CONFIG_MEDIUM_STARTUP;
    } else {
	free(curcfg);
	return;
    }

    ruser = reg_invoke_get_remote_username(cfg->proto);
    if (cfg->proto == FILE_ACCESS_RCP) {
	dest = CONFIG_MEDIUM_NETWORKRCP;
    } else if (cfg->proto == FILE_ACCESS_TFTP)
	dest = CONFIG_MEDIUM_NETWORKTFTP;
    else
	dest = CONFIG_MEDIUM_UNKNOWN;
	    
    if (source != CONFIG_MEDIUM_UNKNOWN) {
	reg_invoke_config_history_event(CONFIG_COMMAND_SOURCE_COMMANDLINE, 
					source, dest,
					confg.ps.tuba.boothost.ip_addr,
					confg.filename,
					ruser);
    }

    if (reg_invoke_file_write(cfg->proto, cfg, curcfg, size)) {
        printf(" [OK]");
    } else {
        printf(" [Failed]");
    }
    free(curcfg);
}

/*
 * tftp_noninteractively_get_config_file
 * retrieve a configuration file using TFTP.
 * Used for Autoinstall over serial links.
 */

boolean tftp_noninteractively_get_config_file (
	char *filename,
	addrtype *server_ip_addr,
	boolean direct_to_nv)
{
    filetype confg;
    filetype *cfg = &confg;
    filetype *base = NULL;

    /* Initialize host address and other info */
    cfg->filename = NULL;
    cfg->proto = FILE_ACCESS_TFTP;
    cfg->ps.tuba.boothost.type = ADDR_IP;
    cfg->resetsubr = NULL;						
    base = & hostconfg;
    setstring(&cfg->filename, filename);

    /*
     * hostname has been specified by the user
     */
    bcopy((uchar *)server_ip_addr,(uchar *)&cfg->ps.tuba.boothost, 
						sizeof(addrtype));

    if (!usable_interfaces()) {
	printf("\nThere are no usable interfaces configured for IP");
	return(FALSE);
    } 

    /*
     * Go out and get the file!
     */
    if (read_config_file(&confg, direct_to_nv, PRIV_ROOT)) {
	    base->proto = cfg->proto;
	    base->responder = TRUE;
	    bcopy(&confg.ps.tuba.respondent, &base->ps.tuba.respondent,
		  sizeof(addrtype));
	    bcopy(&confg.ps.tuba.boothost, &base->ps.tuba.boothost,
		  sizeof(addrtype));
	    setstring(&base->filename, confg.filename);
	    return(TRUE);
	} else {
	    return(FALSE);
	}
}
