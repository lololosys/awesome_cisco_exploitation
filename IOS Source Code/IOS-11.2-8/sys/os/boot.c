/* $Id: boot.c,v 3.6.12.5 1996/08/28 13:02:39 thille Exp $
 * $Source: /release/112/cvs/Xsys/os/boot.c,v $
 *------------------------------------------------------------------
 * boot.c -- network configuration and loading support
 * 
 * Bill Palmer, September 1988
 *
 * Copyright (c) 1988-1996,1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: boot.c,v $
 * Revision 3.6.12.5  1996/08/28  13:02:39  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.6.12.4  1996/07/10  22:21:32  dstine
 * CSCdi62471:  Superfluous #include files in atalk, atm, os files
 * Branch: California_branch
 *
 * Revision 3.6.12.3  1996/05/21  09:58:04  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.6.12.2  1996/05/17  11:34:04  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.6.2.2  1996/05/02  22:06:21  hampton
 * Remove support for old Multibus token ring cards.
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.6.2.1  1996/04/03  21:09:21  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.6.12.1  1996/03/18  21:28:51  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.4.12.3  1996/03/14  01:21:21  gstovall
 * Branch: DeadKingOnAThrone_branch
 * The King is now ready for action.
 *
 * Revision 3.4.12.2  1996/03/07  10:17:45  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.4.12.1  1996/02/20  16:36:54  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.6  1996/02/12  18:18:17  abaindur
 * CSCdi48693:  show process mem does not include alternate mempool mallocs
 *
 * Revision 3.5  1996/02/07  16:14:12  widmer
 * CSCdi48285:  Remove function name from bad_parser_subcommand
 *
 * Revision 3.4  1995/12/17  18:34:09  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.3  1995/11/17  18:44:33  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:16:55  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:43:45  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.10  1995/10/06  18:13:24  enf
 * CSCdi40422:  Cant netboot router with boothelper image
 *
 * Revision 2.9  1995/09/23  05:57:43  enf
 * CSCdi39189:  100X need insurance against loading incorrect image
 *
 * Revision 2.8  1995/09/08  00:46:18  smaser
 * CSCdi38454:  boot system rom fails
 *
 * Revision 2.7  1995/08/01  00:00:33  enf
 * CSCdi33605:  System image file is eprom:c1000-rboot-r, booted via
 *
 * Revision 2.6  1995/07/20  23:47:12  dcoli
 * CSCdi37277:  %SYS-2-FREEBAD error message during boot load
 * Initialize filetype variable for both cases of if-then-else.
 *
 * Revision 2.5  1995/07/16  18:04:17  thille
 * CSCdi37049:  Console output from SNMP WriteNet SET confuses console user
 * Bad programmer, no doggie biscuit.
 *
 * Revision 2.4  1995/07/15  02:37:02  thille
 * CSCdi37049:  Console output from SNMP WriteNet SET confuses console user
 * Add a silent_mode parameter to some routines and registries and set it
 * to TRUE when making the calls from SNMP WriteNet, netConfigSet, or
 * hostConfigSet.  Also, add an errmsg for each so the event can still get
 * logged, and make it clear what caused the event.  Fix other callers to
 * pass FALSE so current behavior will be maintained.
 *
 * Revision 2.3  1995/06/19  06:48:39  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.2  1995/06/09  13:12:03  hampton
 * Rename old scheduler calls to fit the new naming convention.
 * [CSCdi35514]
 *
 * Revision 2.1  1995/06/07 21:53:25  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <string.h>
#include <ciscolib.h>
#include "sys_registry.h"
#include "config_register.h"
#include "../dev/flash_registry.h"
#include "../dev/monitor1.h"
#include "../../boot/reason.h"
#include "interface_private.h"
#include "packet.h"
#include "config.h"
#include "file.h"
#include "ttysrv.h"
#include "logger.h"
#include "free.h"
#include "boot.h"
#include "../if/network.h"
#include "parser.h"
#include "nv.h"
#include "../ui/command.h"
#include "hostname.h"
#include "../parser/parser_actions.h"
#include "../os/msg_system.c"		/* Not a typo, see logger.h */

static void rom_file_parse(filetype *file, parseinfo *csb);


/*
 * Local Storage
 */
filetype hostconfg;		/* default host config info */
filetype netconfg;		/* default net config info */
filetype sysconfg;		/* default system config info */

queuetype hostconfgQ;		/* list of host config file,addr pairs */
queuetype netconfgQ;		/* list of net config file,addr pairs */
queuetype sysconfgQ;		/* list of system config file,addr pairs */
queuetype bootconfgQ;		/* list of bootstrap config file,addr pairs */

/*
 * probe_config
 * The actual probing process.
 */

static process probe_config (void)
{
    filetype *cfg, *result, savecfg;
    int minutes;
    queuetype *confgQ;

    if (!process_get_arg_ptr((void**)&confgQ))
	process_kill(THIS_PROCESS);
    result = NULL;
    if (confgQ == & hostconfgQ)
	result = & hostconfg;
    if (confgQ == & netconfgQ)
	result = & netconfg;
    minutes = ONEMIN;			/* initially dismiss for one minute */
    while (TRUE) {
	process_sleep_for(minutes);	/* dismiss */
	(void) whoami();		/* make sure we have a name */
	if (!netconfig_enable || result->responder)
	    break;
	for (cfg = (filetype *)confgQ->qhead; cfg; cfg = cfg->next) {
	    /*
	     * Save the config block we are currently trying to load,
	     * because it may change out from under us.  (If we process a
	     * hostname command or boot command).
	     */
	    savecfg = *cfg;
	    savecfg.filename = NULL;
	    setstring(&savecfg.filename, cfg->filename);
	    if (read_config_file(&savecfg, FALSE, PRIV_ROOT)) {
		if (result) {
		    *result = savecfg;
		    result->filename = NULL;
		    setstring(&result->filename, savecfg.filename);
		}
		free(savecfg.filename);
		(void) reg_invoke_autoaddr_okay();
		process_kill(THIS_PROCESS);
	    }
	    free(savecfg.filename);
	}
	minutes = 10*ONEMIN;		/* pause for ten minutes hereafter */
    }
    process_kill(THIS_PROCESS);		/* terminate process */
}

/*
 * boot_parse_filespec
 * This routine parses a boot file specification given when hand-booting
 * the system from the bootstrap monitor.  When adding new sources for
 * bootfiles, the command syntax is
 * 
 *        <method> <filename> [<address1>] [<address2>]
 *
 * However, for backwards compatibility we will allow "b <filename>" as
 * a shorthand for "b tftp <filename>".  If you want to specify a hostname,
 * you'll have to use the modern syntax.  Examples include:
 *
 *	       b <filename > [<ip-address-or-hostname>]
 *             b tftp [<filename> [<ip-address-or-hostname>]]
 *             b mop [<filename> [<MAC-address> [<interface>]]]
 *             b flash [<filename>]
 *
 */

static boolean boot_parse_filespec (char *cmdline, filetype *cfg)
{
    char protocol[MAXCHRS_BOOT_PROTOCOL];
    static char filename[MAXCHRS_BOOT_FILENAME];
    static char hostname[MAXCHRS_BOOT_HOSTNAME];
    char argument[MAXCHRS_BOOT_ARGUMENT];
    char *ptr = cmdline;
    char *protoptr = protocol;
    char *fileptr = filename;
    char *tmpptr;

    /*
     * Say the file type is totally unknown.
     * Blow out early if null command line
     */
    cfg->proto = FILE_ACCESS_UNKNOWN;
    if (null(cmdline))
	return(FALSE);

    memset(filename, 0, MAXCHRS_BOOT_FILENAME);
    memset(hostname, 0, MAXCHRS_BOOT_HOSTNAME);
    memset(argument, 0, MAXCHRS_BOOT_ARGUMENT);

    /*
     * Pull apart command line into as many as four arguments.
     * Loop through the registered file name parsing functions.
     * When one of them hits, it returns TRUE, having set up the protocol
     * dependent fields of the cfg file.  We set the filename and are done.
     */
    tmpptr = gettoken(ptr, protocol, MAXCHRS_BOOT_PROTOCOL, WORD);
    if (!reg_invoke_bootfile_parse(cfg, protoptr, NULL, NULL)) {
	/*
	 * It failed the new way, let's try the old way
	 */
	protoptr = "tftp";
    }
    else 
        ptr = tmpptr; /* protocol done */

    ptr = gettoken(ptr, filename, MAXCHRS_BOOT_FILENAME, WORD);
    ptr = gettoken(ptr, hostname, MAXCHRS_BOOT_HOSTNAME, WORD);
    (void)gettoken(ptr, argument, MAXCHRS_BOOT_ARGUMENT, WORD);

    if (!reg_invoke_bootfile_parse(cfg, protoptr, hostname, argument)) {
        printf("\n%% Unable to parse boot request\n");
	flush();
	return(FALSE);
    }

    /* 
     * a filename is not required when booting from flash
     */
    fileptr = *fileptr ? fileptr : NULL;
    setstring(&cfg->filename, fileptr);
    return(TRUE);
}

/*
 * file_equal
 * See if two file descriptors are the same
 */

static boolean file_equal (filetype *cfg1, filetype *cfg2)
{
    /*
     * Make sure names and access protocols match,
     * Then compare access protocol dependent information.
     */
    if (strcasecmp(cfg1->filename, cfg2->filename))
	return(FALSE);
    if (cfg1->proto != cfg2->proto)
	return(FALSE);
    return(reg_invoke_file_match(cfg1->proto, cfg1, cfg2));
}

/*
 * rom_file_source
 * Used to complete the booted from .... string when we've booted
 * from ROM
 */

static void rom_file_source (filetype *file, uchar *buffer)
{
    strcpy(buffer, "ROM");
}

/*
 * boot_command
 * Specify the boot filename and boot host for net or host configuration file.
 * [no] boot {<type>} tftp <filename> [<ipaddr>]
 * [no] boot {<type>} mop <filename> [{ <enaddr> | <interface> }]
 * no boot bootstrap <type>
 * [no] boot system rom
 * [no] boot system flash [<filename>]
 *
 * where <type> is one of { bootstrap | host | network | system }
 *
 * builds list in order of entry
 * doesn't add entries if already on the list
 * configuration file loader loops through each entry on list until successful
 * no boot network|host|system <filename> <addr>
 *  removes (<filename>,<addr>) from list 
 * no boot network|host|system 
 *   deletes entire list
 * boot network|host|system <filename> <addr>
 *   adds (<filename>,<addr>) to end of list, iff not already on the list
 * boot network|host|system <filename>
 *   adds (<filename>,<255.255.255.255>) to end of list
 */

void boot_command (parseinfo *csb)
{
    queuetype *cfgQ;
    filetype *ourcfg = NULL;
    filetype *cf = NULL;
    int keyword = BOOT_NONE;
    boolean found = FALSE;
    filetype *dflt = NULL;

    cfgQ = NULL;
    keyword = GETOBJ(int,1);

    switch (csb->which) {
      case BOOT_BUFFERSIZE:
	if (csb->nvgen) {
	    nv_write(config_bytes != (nvsize ? nvsize : DEFAULT_CONFIG_BYTES),
		     "%s %d", csb->nv_command, config_bytes);
	    return;
	}
	if (!csb->sense)
	    config_bytes = nvsize ? nvsize : DEFAULT_CONFIG_BYTES;
	else 
	    config_bytes = GETOBJ(int,1);
	return;

      case BOOT_FILE:
	/* Determine which type for the boot <type> <file> ... command */
	switch (GETOBJ(int,1)) {

	  case BOOT_HOST:
	    cfgQ = &hostconfgQ;
	    break;

	  case BOOT_NET:
	    cfgQ = &netconfgQ;
	    break;

	  case BOOT_SYS:
	    cfgQ = &sysconfgQ;
	    break;

	  case BOOT_BOOT:
	    cfgQ = &bootconfgQ;
	    break;
#ifdef NOTYET
	  case BOOT_MODULE:
	    if (csb->nvgen)
		return;
	    if (!module_load_file(GETOBJ(string, 1)))
		printf("\n%% Load failed");
	    return;
#endif
	  default:
	    bad_parser_subcommand(csb, GETOBJ(int,1));
	    return;
	}
	break;

      default:
	bad_parser_subcommand(csb, csb->which);
	return;
    }

    if (csb->nvgen) {
	for (cf = (filetype *)(cfgQ->qhead); cf; cf = cf->next) {
	    if (!cf->defaultname) {
		nv_write(TRUE, "%s ", csb->nv_command);
		reg_invoke_file_parse(cf->proto,
			      cf, csb);
	    }
	}
	return;
    } else {
	if (csb->sense) {
	    /* Future:
	     * may wish to tftp image and check it 
	     * if that fails we offer a warning message to user.
	     */
	}
    }

    /*
     * Possibly we need to delete an entire file list.
     * GETOBJ(string,1) is the filename, which will be empty for
     * the 'no boot <type>' case.  Also check for presence of
     * "rom" keyword, signified by GETOBJ(int,2) being non-zero.
     */
    if (!csb->sense && !GETOBJ(string,1)[0] && GETOBJ(int,2) == 0) {
	/*
	 * "no boot system|network|host"
	 * delete entire list for configuration file type
	 */
	while ((cf = dequeue(cfgQ))) {
	    free(cf->filename);
	    free(cf);		/* destroy list and free memory */
	}
	return;
    }

    /*
     * Create a new file descriptor
     */
    ourcfg = malloc(sizeof(filetype));
    if (ourcfg == NULL) {
	printf(nomemory);
	return;
    }
    ourcfg->defaultname = FALSE;
    ourcfg->next = NULL;
    ourcfg->resetsubr = NULL;
    if (keyword == BOOT_HOST)
	ourcfg->resetsubr = reset_host;
    setstring(&ourcfg->filename, GETOBJ(string,1));
    ourcfg->ps.tuba.username = NULL;
    ourcfg->ps.tuba.display =  TRUE;
    ourcfg->proto = GETOBJ(int,2);
    reg_invoke_file_parse(ourcfg->proto, ourcfg, csb);

    /*
     * Now check if we should add or delete it from the appropriate list.
     */
    if (csb->sense) {
	/*
	 * boot network|host|system <filename> [addr]
	 */ 
	found = FALSE;
	if (!QUEUEEMPTY(cfgQ)) {
	    for (cf = (filetype *)cfgQ->qhead; !found && cf; cf = cf->next) {
		if (cf->defaultname == TRUE) 
		    dflt = cf;	/* zap this guy later */
		else if (file_equal(cf, ourcfg))
		    found = TRUE;
	    }
	}

	while (dflt) {		/* zap ersatz default once we get a boot cmd */
	    unqueue(cfgQ, dflt);
	    free(dflt->filename);
	    free(dflt);

	    dflt = NULL;
	    for (cf = (filetype *)cfgQ->qhead; !found && cf; cf = cf->next) {
		if (cf->defaultname == TRUE) {
		    dflt = cf;  /* zap this guy later */
		}
	    }
	}
	if (!found) {		/* empty queue or we weren't on it */
	    enqueue(cfgQ, ourcfg); /* so add this entry */
	    return;
	}
    } else {
	/*
	 * no boot network|host|system <filename> <addr>
	 */
	found = FALSE;
	if (!QUEUEEMPTY(cfgQ)) {
	    for (cf = (filetype *)cfgQ->qhead; !found && cf; cf = cf->next) {
		if (file_equal(cf, ourcfg)) {
		    found = TRUE;
		    break;	/* preserve cf */
		}
	    }
	}
	if (found) {		/* we were on the queue */
	    unqueue(cfgQ, cf);	/* so remove this entry */
	    free(cf->filename);
	    free(cf);
	}
    }
    free(ourcfg->filename);
    free(ourcfg);
}

/*
 * boot_init
 * Initialize network config lookup stuff
 * Called from system_init (os/init.c)
 */

void boot_init (void)
{
    filetype *cfg, *dos_cfg;

    netconfg.filename = NULL;
    netconfg.proto = FILE_ACCESS_TFTP;
    netconfg.responder = FALSE;
    netconfg.ps.tuba.boothost.type = ADDR_IP;
    netconfg.ps.tuba.boothost.ip_addr = -1;
    netconfg.ps.tuba.respondent.type = ADDR_IP;
    netconfg.ps.tuba.respondent.ip_addr = 0;
    netconfg.ps.tuba.username = NULL;
    netconfg.ps.tuba.display =  TRUE;
    netconfg.resetsubr = NULL;

    hostconfg.filename = NULL;
    hostconfg.responder = FALSE;
    hostconfg.proto = FILE_ACCESS_TFTP;
    hostconfg.ps.tuba.boothost.type = ADDR_IP;
    hostconfg.ps.tuba.boothost.ip_addr = -1;
    hostconfg.ps.tuba.respondent.type = ADDR_IP;
    hostconfg.ps.tuba.respondent.ip_addr = 0;
    hostconfg.ps.tuba.username = NULL;
    hostconfg.ps.tuba.display =  TRUE;
    hostconfg.resetsubr = reset_host;

    memset(&sysconfg, 0, sizeof(filetype));

    /*
     * Initialize lists of configuration filename,address pairs.
     * Then add default network configuration filename entry.
     */
    queue_init(&sysconfgQ, 0);
    queue_init(&netconfgQ, 0);
    queue_init(&bootconfgQ, 0);

    cfg = malloc(sizeof(filetype));
    if (cfg) {
	setstring(&(cfg->filename), "network-confg");
	cfg->proto = FILE_ACCESS_TFTP;
	cfg->ps.tuba.boothost.type = ADDR_IP;
	cfg->ps.tuba.boothost.ip_addr = -1L;
	cfg->ps.tuba.boothost.type = ADDR_IP;
	cfg->ps.tuba.respondent.ip_addr = 0;
        cfg->ps.tuba.username = NULL;
        cfg->ps.tuba.display = TRUE;
	cfg->resetsubr = NULL;
	cfg->defaultname = TRUE;
	enqueue(&netconfgQ, cfg);
    }
    dos_cfg = malloc(sizeof(filetype));
    if (dos_cfg) {
        setstring(&(dos_cfg->filename), "cisconet.cfg");
        dos_cfg->proto = FILE_ACCESS_TFTP;
        dos_cfg->ps.tuba.boothost.type = ADDR_IP;
        dos_cfg->ps.tuba.boothost.ip_addr = -1L;
        dos_cfg->ps.tuba.boothost.type = ADDR_IP;
        dos_cfg->ps.tuba.respondent.ip_addr = 0;
        dos_cfg->resetsubr = NULL;
        dos_cfg->defaultname = TRUE;
        enqueue(&netconfgQ, dos_cfg);
    }
    reg_add_default_file_read((service_file_read_type)return_false,
			      "return_false");
    reg_add_default_file_match((service_file_match_type)return_true, 
			       "return_true");
    reg_add_default_bootinfo_get((service_bootinfo_get_type)return_nothing,
				 "return_nothing");
    reg_add_file_parse(FILE_ACCESS_ROM, rom_file_parse, "rom_file_parse");
    reg_add_file_source(FILE_ACCESS_ROM, rom_file_source, "rom_file_source");
}

/*
 * boot_bootinfo
 * Get information regarding our boot filename and source host
 * The various protocol registries must be initialized before we
 * are called to ensure that protocol specific information is recorded
 * correctly.
 */

void boot_bootinfo (void)
{
    bootinfo_t *bootinfo;
    ipaddrtype bootaddr;

    /*
     * Fill in the information about the boot of the current system image.
     * The service point records access protocol specific information.
     * If it is an older system, we have to use the IP specific function. 
     */
    sysconfg.filename = NULL;		/* Assume no filename */
    sysconfg.responder = FALSE;		/* And that we didn't get booted */
    bootinfo = (bootinfo_t *)mon_get_bootinfo();
    if (bootinfo && ((long)bootinfo != -1L)) {
	/* 
	 * If bootinfo->protocol == 0, then we're running rom software and
	 * sysconfg.responder should be FALSE.
	 */
	if (bootinfo->protocol) {
	    sysconfg.responder = TRUE;
	    sysconfg.proto = bootinfo->protocol;
	    sysconfg.ps.tuba.username = NULL;
            sysconfg.ps.tuba.display = TRUE;
	    setstring(&sysconfg.filename, bootinfo->filename);
	    reg_invoke_bootinfo_get(bootinfo->protocol, bootinfo, &sysconfg);
	}
    } else {
	bootaddr = mon_bootipaddr();
	if (bootaddr) {
	    sysconfg.responder = TRUE;
	    sysconfg.proto = FILE_ACCESS_TFTP;
	    sysconfg.ps.tuba.respondent.ip_addr = bootaddr;
	    sysconfg.ps.tuba.respondent.type = ADDR_IP;
            sysconfg.ps.tuba.username = NULL;
            sysconfg.ps.tuba.display = TRUE;
	}
    }
}

/*
 * read_config_file
 * Load in a configuration file
 */
boolean read_config_file (filetype *confg, boolean direct_to_nv, uchar priv)
{
    ulong buffsize;
    uchar *buffer;
    boolean verbose, status = TRUE;  /* assume success */
    int success;
    uchar source[MAXCHRS_BOOT_HOSTNAME];

    if (!nnets)			/* if no nets, pretend we succeeded */
	return(TRUE);
    verbose = !(stdio->statbits & CTRLC_ENBL);
    buffsize = nv_get_config_size();
    buffer = malloc(buffsize);
    if (buffer == NULL) {
	printf(nomemory);
	return(FALSE);
    }
    success = reg_invoke_file_read(confg->proto,
			      confg, buffer, &buffsize, verbose, NULL, NULL);
    if ((success == ERROR_OK) && buffsize) {
	confg->responder = TRUE;
	if (confg->resetsubr)
	    (*confg->resetsubr)();
	if (!nv_valid && netconfg.responder)
	    netconfig_enable = FALSE;     /* turn off default service config */
	reg_invoke_file_source(confg->proto, confg, source);
	if (direct_to_nv) {
	    if (!nv_buffer_to_nv((char *) buffer, buffsize, TRUE)) {
		errmsg(&msgsym(CONFIG_NV_ERR, SYS));
		status = FALSE;
	    } else
		errmsg(&msgsym(CONFIG_NV, SYS), confg->filename, 
		       user_on_stdio_str(), source);
	} else {
	    parse_configure((char *)buffer, TRUE, RES_CONFIG, priv);
	    errmsg(&msgsym(CONFIG, SYS), confg->filename, 
		   user_on_stdio_str(), source);
	}
    } else
	status = FALSE;
    free(buffer);
    return(status);
}

/*
 * configure
 * Try to find a configuration file on the queue of such file,addr pairs.
 * If persistence flag set, loop over the list until successful.
 */

boolean configure (
    queuetype *confgQ,
    filetype *confg,
    boolean persistence)
{
    filetype *cfg, savecfg;
    pid_t pid;

    if (QUEUEEMPTY(confgQ))
	return(FALSE);

    for (cfg = (filetype *)confgQ->qhead; cfg; cfg = cfg->next) {
	/*
	 * Save the config block we are currently trying to load, because
	 * it may change out from under us.  (If we process a hostname
	 * command or boot command).
	 */
	savecfg = *cfg;
	savecfg.filename=NULL;
	setstring(&savecfg.filename, cfg->filename);
	if (read_config_file(&savecfg, FALSE, PRIV_ROOT)) {
	    *confg = savecfg;
	    confg->filename = NULL;
	    setstring(&confg->filename, savecfg.filename);
	    free(savecfg.filename);
	    return(TRUE);
	} else 
	  free(savecfg.filename);
    }
    if (persistence) {
	pid = process_create(probe_config, "Config Probe", NORMAL_STACK,
			     PRIO_NORMAL);
	if (pid != NO_PROCESS)
	    process_set_arg_ptr(pid, confgQ);
    }
    return(FALSE);
}

/*
 * read_bootfile
 * Helper routine for bootload process
 */

static boolean read_bootfile (struct plb_t *plb, filetype *cfg, uchar **bufp,
			      ulong *size)
{
    int countdown;
    uchar *saved_buff = *bufp;
    ulong saved_size = *size;
    ulong result;

    plb->plb_size = 0;

    /*
     * If we are not using a local method to access the bootfile, and there
     * are no usable interfaces, hang here until at least one usable interface
     * is up. The longest we should ever have to wait is 17 seconds
     * (for the CTR to come onto the ring).  So we wait up to 20 seconds.
     */
    if ((cfg->proto != FILE_ACCESS_FLASH) && !usable_interfaces()) {
	countdown = 4;
	process_sleep_for(5*ONESEC);
	while (!usable_interfaces()) {
	    countdown--;
	    if (countdown == 0) {
		plb->plb_str = "No usable interfaces";
		return(FALSE);
	    }
	    process_sleep_for(5*ONESEC);
	}
    }
    if (!reg_invoke_file_boot_in_place(cfg->proto, cfg->filename, bufp, size)) {
        /*
         * NOT run-in-place. Either download to RAM or file not
         * accessible. Determine this from the returned size. If
         * file is not accessible, returned size will be zero.
         */
        if (*size == 0) {  /* Not accessible. Return failure */
            plb->plb_str = "File boot failed -- File not accessible";
            *size = saved_size;  /* Restore buffer pointer and size */
            *bufp = saved_buff;
            return (FALSE);
        }

        /*
         * If filename has been found by the boot_in_place function,
         * it will be set in sysconfg.filename. If so, set cfg->filename
         * to the filename found.
         */
         if (sysconfg.filename)
             setstring(&cfg->filename, sysconfg.filename);

        /*
         * Read file into RAM buffer.
         */
        *size = saved_size;  /* Restore buffer pointer and size */
        *bufp = saved_buff;
        result = reg_invoke_file_read(cfg->proto, cfg, *bufp, size,
                                      TRUE, NULL, NULL);
 
        if (result != ERROR_OK) {
            switch (result) {
            case ERROR_NOFILE:
                plb->plb_str = "File read failed -- File not accessible";
                break;
 
            case ERROR_TIMEOUT:
                plb->plb_str = "File read failed -- Time out";
                break;
 
            case ERROR_NOMEM:
                plb->plb_str = "File read failed -- No Mem";
                break;
 
            case ERROR_TOOBIG:
                plb->plb_str = "File read failed -- Buffer Overflow";
                break;
 
            default:
                plb->plb_str = "File read failed";
                break;
            }
            *size = saved_size;  /* Restore buffer pointer and size */
            *bufp = saved_buff;
            return(FALSE);
        } else {
	    if (reg_invoke_flash_boot_verify( *bufp,*size, (void *)0)) {
		*size = saved_size;  /* Restore buffer pointer and size */
		*bufp = saved_buff;
		return(FALSE);
	    }
	}
    }
 
    /*
     * If filename has been found by the boot_in_place function,
     * it will be set in sysconfg.filename. If so, set cfg->filename
     * to the filename found.
     */
     if (sysconfg.filename)
         setstring(&cfg->filename, sysconfg.filename);

    /*
     * Record boot information.
     * The bootstrap will use this to relocate the image before starting it.
     */
    plb->plb_addr = *bufp;
    plb->plb_size = *size;
    return(TRUE);
}

/*
 * configuredBLOCK
 * Block until we have a configuration of some sort.
 */
boolean configuredBLOCK (void)
{
    return(system_configured ? FALSE : TRUE);
}

/*
 * bootload
 * Load a binary bootable image from across the network
 */

#define MAX_NETBOOT_RETRIES  6
process bootload (void)
{
    uchar *buff = NULL;
    long nbytes;
    ulong size;
    ulong sleep;
    filetype *cfg;
    filetype *cfgbase;
    filetype cmdcfg;
    filetype *successfile = NULL;
    char bootname[16];		/* For bootstrap names */
    boolean load_bootstrap;
    bootinfo_t *bootinfo;
    long *boot_data;
    const char *file;
    int retry_cnt;
    boolean netboot_req, flashboot_req;
    struct plb_t *plb;

    /*
     * First let init configure the world.
     */
    if (!process_get_arg_ptr((void**)&plb))
	process_kill(THIS_PROCESS);
    process_wait_on_system_config();

    boot_data = mon_lasterror();
    /*
     * See if we need to act as the flash load helper.
     * If yes, call the flash load helper. 
     * If flash download error or problem, increment retry
     * count in boot_data[BOOT_ADDRESS] and return to ROMMON.
     * If two retries done, then attempt a boot, the same
     * way as in the case of a success.
     */
    if ((plb->plb_magic == PLBMAGIC) &&
 	(plb->plb_option & (BOOTCOPYTFTPFLASH | 
			    BOOTCOPYRCPFLASH | 
			    BOOTCOPYMOPFLASH))) {
	if (reg_invoke_flh_call((void *)plb) < 0) { /* failure */
	    boot_data[BOOT_ADDRESS]++;
	    if (boot_data[BOOT_ADDRESS] < 2) {
		plb->plb_result = PLB_ERROR_RETRY;
 	        goto error;		/* return to ROM MON */
	    }
	}
	plb->plb_result = PLB_ERROR_NORETRY; /* end of retries */
	plb->plb_str = "flash";
	boot_data[BOOT_ADDRESS] = 0; /* reset */
	boot_data[BOOT_REASON] = EXC_RELOAD;
    }
            
    /*
     * Allocate memory for boot buffer
     */
    nbytes = mempool_get_largest_block(MEMPOOL_CLASS_LOCAL);
    nbytes -= (3*config_bytes);
    buff = malloc(nbytes);
    if (buff == NULL) {
        plb->plb_size = 0;
        plb->plb_str = "Out of memory";
        goto error;
    } 

    /*
     * Determine whether we are loading a bootstrap or a system image.
     * We are supposed to load a bootstrap if this is not a bootstrap
     * image, and the secondary bootstrap bit is set in the configuration
     * register.
     */

    if (!bootstrap_enable && (configregister & CFG_SECONDARY)) {
	cfgbase = (filetype *)bootconfgQ.qhead;
					/* Point to start of bootstrap list */
	load_bootstrap = TRUE;		/* Flag bootstrap loading */
    } else {
	load_bootstrap = FALSE;
	if ((configregister & CFG_FILE) == 0 ||
            (plb->plb_option & BOOTIGNORECONFIGBITS)) {
	    cfgbase = NULL;		/* Don't try "boot" commands */
	} else {
	    cfgbase = (filetype *)sysconfgQ.qhead;
	    				/* Try the boot command list */
	}
    }

    boot_data = mon_lasterror();

    memset(&cmdcfg, 0, sizeof cmdcfg);
    cmdcfg.ps.tuba.display = TRUE;

    if (!cfgbase) {			/* No file list, or it is being
					 * supressed now */
	if (boot_parse_filespec(plb->plb_str, &cmdcfg)) {
	    if (load_bootstrap) {
		char *procname;

		procname = mon_procname();
		if ((ulong)procname == -1L)
		    procname = "UNKNOWN";
		concat("boot-", procname, bootname);
		lowercase(bootname);
		setstring(&cmdcfg.filename, bootname);
	    }
	    size = nbytes;

	    /*
	     * if we have a new style plb header (plb_magic == PLBMAGIC)
	     * which contains the options field and the options filed says
	     * to ignore the config bits, don't force booting of the first
	     * file in flash, instead do what the boot string says
	     */
	    if (!(plb->plb_magic == PLBMAGIC &&
		  (plb->plb_option & BOOTIGNORECONFIGBITS))) {
    
		/*
		 * If the FILE register is set, attempt to boot the flash card
		 * first.  Especially important for a new box with no nv-ram.
		 * If we know we have a flash card, redo the cfg to our needs.
		 */
		if (configregister & CFG_FILE) { /* boot bits set */
		    /*
		     * See if we have a default file to boot in flash.
		     */
		    file = reg_invoke_flash_bootfile_default();
		    if (file) {  /* File in Flash. Boot it */
			cmdcfg.proto = FILE_ACCESS_FLASH;
			setstring(&cmdcfg.filename, (char *)file); /* first bootable file */
		    }
		    /*
		     * This will try to boot up either the Flash file (if
		     * found), or the default "ciscoN-<platform>" file.
		     * If we can, we are happy. Else, ask the ROM Mon
		     * to boot up the ROM or bootflash image. 
		     */
		    if (read_bootfile(plb, &cmdcfg, &buff, &size)) {
			successfile = &cmdcfg;
			goto worked;
		    }
		    if (boot_data != (long *)-1) /* Force ROM image boot up */
			boot_data[BOOT_ADDRESS] = ROMBOOT_REQUEST-1;
		    goto error;
		}
	    }

	    /* 
	     * boot user typed file, or default cisco<N>-<platform> file 
	     */
	    if (!read_bootfile(plb, &cmdcfg, &buff, &size))
	      goto error;
	    successfile = &cmdcfg;
            /*
             * Drop down to 'worked:'.
             */
	} else {
	    plb->plb_str = "Command line parse error";
	    plb->plb_size = 0;
	    goto error;
	}
    } else {
	sleep = 2;  /* This represents 2 secs */
        netboot_req = FALSE; 
	flashboot_req = FALSE;
        retry_cnt = 0;
	while (retry_cnt < MAX_NETBOOT_RETRIES) {
	    if (retry_cnt) { /* not the first time in this loop */
                printf("\nSleeping for %d secs before next netboot attempt",
                       sleep);
                flush();
                process_sleep_for(sleep * ONESEC);
	        sleep *= sleep;
	        if (sleep > 5*60)  /* 5 mins or 300 secs */
		    sleep = 5*60;
            }

            /*
             * Go through the 'boot system' config commands.
             */
	    for (cfg = cfgbase; cfg; cfg = cfg->next) {
		size = nbytes;
		if ((strcmp("rom", cfg->filename) == 0) ||
                    (cfg->proto == FILE_ACCESS_ROM)) {
		    /*
                     * 'boot system rom'.
		     * We want to boot from the default ROM software.
		     * To do so, we need to pretend that bootloading failed,
		     * and tell the ROM monitor that we really want it to
		     * boot from ROM, not attempt to netboot.
		     * Use ROMBOOT_REQUEST-1 because it is incremented
		     * before the test.
		     */
		    if (boot_data != (long *)-1) {
			boot_data[BOOT_ADDRESS] = ROMBOOT_REQUEST-1;
			/*
			 * This call is here to signal the rom monitor to explicitly
			 * try to boot the rboot image. This is required because
			 * ROMBOOT_REQUEST can be used to signal two different 
			 * events. One being that a request to boot the rom software
			 * has been detected and the other being that the rom monitor
			 * should resort to some default booting action because
			 * none of the explicitly requested files could be booted.
			 * Setting this flag allows the C1000 rom monitor to distinguish
			 * between the two thus enabling it to try to boot the first
			 * file in flash before resorting to booting the rboot image.
			 */ 
			mon_boot_sys_rom_flg_set(); /*
						     * Tell rommon to explicitly boot
						     * rboot code rather than trying
						     * first file in flash.
						     */
			goto error;
		    }
		}
                /*
                 * If we have tried a flash image and failed,
                 * don't try again. Only retry TFTPs.
		 * Keep a track of whether a flash boot was tried.
		 */
		if (cfg->proto == FILE_ACCESS_FLASH) {
		    flashboot_req = TRUE;
		    if (retry_cnt)
			continue;
		}

                /*
                 * See if we have any netboot request. If so,
                 * flag it. We don't want to go through this loop
                 * more than once if there are no netboot cmnds.
                 */
		if (cfg->proto != FILE_ACCESS_FLASH)
		    netboot_req = TRUE;

		if (read_bootfile(plb, cfg, &buff, &size)) {
                    successfile = cfg;
		    goto worked;
		}
	    }  /* End of 'for' for all config 'boot system' entries. */
            /*
             * If the boot-default-ROM-software bit is set in
             * the config register, and we have already retried
             * once, bail out.
             */
            if (configregister & CFG_LANCONFIG) {
                break;
            }

            if (netboot_req == FALSE)  /* No netboot request; exit */
                break;

            retry_cnt++;  /* Increment retry count */
	}
	/*
	 * Did we ever try to boot from flash ?
	 * If not, ie., all we tried was netbooting, then try
	 * to boot from flash now.
	 */
	if (!flashboot_req) {
	    if (configregister & CFG_FILE) { /* boot bits set */
		/*
		 * See if we have a default file to boot in flash.
		 */
		file = reg_invoke_flash_bootfile_default();
		if (file) {  /* File in flash. Boot it. */
		    cmdcfg.proto = FILE_ACCESS_FLASH;
		    setstring(&cmdcfg.filename, (char *)file); /* first bootable file */
		    if (read_bootfile(plb, &cmdcfg, &buff, &size)) {
                        successfile = &cmdcfg;
			goto worked;
		    }
		}
	    }
	}
	/*
	 * We get here if none of the images in the boot command list could be
	 * booted. Tell the  ROM monitor to boot the appropritae default software.
	 * This may mean either booting the first file in flash or the rom resident
	 * rboot image. See the rom monitor code for your platform of interest for
	 * more details.
	 */
	if (boot_data != (long *)-1) {
	    boot_data[BOOT_ADDRESS] = ROMBOOT_REQUEST-1;
	}
	goto error;
    }

  worked:

    /*
     * Copy file descriptor of sucessfully loaded file to
     * sysconfg where we will use it later for show commands, etc.
     */
    if (successfile) {
	successfile->responder = TRUE;
	bcopy(successfile, &sysconfg, sizeof(filetype));
    }

    /*
     * Save the bootstrap information for the ROM monitor to pass to
     * the next system image. Save generic, then access method specific
     * information.
     */
    bootinfo = (bootinfo_t *)mon_get_bootinfo();
    if (bootinfo && ((long)bootinfo != -1L)) {
	strcpy(bootinfo->filename, sysconfg.filename);
	bootinfo->protocol = sysconfg.proto;
	reg_invoke_bootinfo_set(sysconfg.proto, bootinfo, &sysconfg);
    }
    /*
     * Pass back file name in plb_str.
     */
    plb->plb_str = bootinfo->filename;

    /*
     * Here when done or have completely failed.
     * We turn off the scheduler, which returns us to the bootstrap
     */
  error:
    if (some_errmsgs_have_printed)
      errmsg(&msgsym(BOOT_MESSAGES,SYS));
    process_sleep_for(ONESEC);		/* Pause for output to finish */
    system_running = FALSE;
    process_kill(THIS_PROCESS);
}

/*
 * boot_fromflash
 * Returns TRUE if we will be booting an image from flash.
 * This assumes we only use this routine if nvram erased or non-existant.
 *
 * If nvram is trashed, we will eventually need to go into setup.  But
 * if we are about to boot a flash file, dont run the setup() on the roms,
 * but instead wait for the on on the flash image.
 */

boolean boot_fromflash (char *bootstring)
{
    filetype file;

    file.filename = NULL;
    file.ps.tuba.username = NULL;
    file.ps.tuba.display = TRUE;

    if (boot_parse_filespec(bootstring, &file)) {

	/* case 1 : user typed "boot flash" with config 0x0 */
	if (file.proto == FILE_ACCESS_FLASH) 
	    return(TRUE);
	/* 
	 * case 2 : default filename ciscoN-P with config 0x2, and flash.
	 * This is inside the conditional for sanity that we have the 
	 * valid ciscoN-P bootstring.
	 */
	if ((configregister & CFG_FILE) && reg_invoke_flash_bootfile_default())
	    return(TRUE);
    }
    return(FALSE);
}

/* 
 * return TRUE if bootflash exists on this platform.
 * (platform_get_string returns NULL if no bootflash.)
 */
boolean bootflash_exists (void)
{
    boolean result = FALSE;
    if (platform_get_string(PLATFORM_STRING_BOOTROM_OR_BOOTFLASH))
	result = TRUE;
    return(result);
}

/*
 * rom_file_parse
 * Parse the "rom" filename.
 * Fill in protocol specific information when parsing a file specification
 * Called by SERVICE_FILE_PARSE, index is FILE_ACCESS_ROM
 */
static void rom_file_parse (filetype *file, parseinfo *csb)
{
    char *bufptr;
    char *no_or_null;   /* pointer to the string "no " or to "" */
    if (!bootflash_exists()) {  
	if (csb->nvgen) {
	    nv_add(TRUE,"rom");
	    /*	
	     * Now add boot command to ROM monitor BOOT variable.
	     */	
	    bufptr = reg_invoke_monvar_buff(ROMMON_BOOT_VAR);
	    if (bufptr) {
		bufptr += strlen(bufptr);
		sprintf(bufptr, "rom:,%d;", BOOTRETRIES);
	    }
	    return;
	}
	setstring(&file->filename, "rom");
	file->proto = FILE_ACCESS_ROM;
    }
    else { /* platform has bootflash - no system image in rom */
	if (csb->nvgen) {
	    nv_add(TRUE,"flash bootflash:");
	    /*	
	     * Now add boot command to ROM monitor BOOT variable.
	     */	
	    bufptr = reg_invoke_monvar_buff(ROMMON_BOOT_VAR);
	    if (bufptr) {
		bufptr += strlen(bufptr);
		sprintf(bufptr, "bootflash:,%d;", BOOTRETRIES);
	    }
	    return;
	}

	/* 
	 * warn user only if it's a manual config.
	 */
	no_or_null = "";
	if (!csb->sense)
	    no_or_null = "no ";
	if (csb->resolvemethod == RES_MANUAL)
	    printf("the %c%sboot system rom%c "
		   "command is not valid for this platform.\n"
		   "it has been translated to "
		   "%c%sboot system flash bootflash:%c",
                 '"',no_or_null,'"','"',no_or_null,'"');
	setstring(&file->filename, "bootflash:");
	file->proto = FILE_ACCESS_FLASH;
    }
}
