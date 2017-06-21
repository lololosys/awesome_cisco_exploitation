/* $Id: hostname.c,v 3.3.20.3 1996/08/28 13:03:18 thille Exp $
 * $Source: /release/112/cvs/Xsys/os/hostname.c,v $
 *------------------------------------------------------------------
 * hostname.c -- Hostnames, Filenames, Prompts, and Banners
 *
 * 27-May-1993, Kirk Lougheed
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: hostname.c,v $
 * Revision 3.3.20.3  1996/08/28  13:03:18  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.3.20.2  1996/06/28  23:24:47  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.3.20.1  1996/03/18  21:29:59  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.3  1996/03/09  05:09:43  hampton
 * Slim down the kernel of the router.
 * Branch: DeadKingOnAThrone_branch
 *
 * Revision 3.2.26.2  1996/03/07  10:18:44  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.2.26.1  1996/02/20  16:39:46  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.3  1996/02/05  15:50:03  irfan
 * CSCdi47506:  Router reboots when enter hostname ""
 * treat it same as <CmdBold>no hostname<noCmdBold>
 *
 * Revision 3.2  1995/11/17  18:48:19  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:46:11  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/08/08  16:58:08  widmer
 * CSCdi37574:  long  commands truncated via tftp download
 * cut & paste programming errors changes wrong variables
 *
 * Revision 2.3  1995/06/19  06:48:44  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.2  1995/06/09  13:12:05  hampton
 * Rename old scheduler calls to fit the new naming convention.
 * [CSCdi35514]
 *
 * Revision 2.1  1995/06/07 21:55:40  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <string.h>		/* for strcat() */
#include <ciscolib.h>
#include "sys_registry.h"
#include "interface_private.h"
#include "packet.h"
#include "ttysrv.h"
#include "config.h"
#include "file.h"
#include "parser.h"
#include "name.h"
#include "logger.h"
#include "../ip/ip.h"
#include "../parser/parser_defs_config.h"
#include "../parser/msg_parser.c"	/* Not a typo, see logger.h */
#include "../if/network.h"
#include "hostname.h"
#include "exception.h"

char *hostname;			/* our hostname */
char *default_domain;		/* default domain name */
char *banner_motd;		/* pointer to message of the day banner */
char *banner_incoming;		/* Banner to print on incoming connections */
char *banner_exec;		/* Normal greeting message */
char *banner_temp;		/* temporary banner set by SNMP */
char *banner_login;		/* banner to print for login */
boolean gotname;		/* TRUE if we know our name */
char *Eprompt;			/* EXEC prompt */
char *Pprompt;			/* privileged EXEC prompt */
boolean dumpfile_def = TRUE;	/* True if dumpfile has default name */
boolean sl_dumpfile_def = TRUE; /* True if sl_dumpfile has default name */


/*
 * hostname_init
 * Initialize our hostname, prompt, and banner variables
 */

void hostname_init (void)
{
    /*
     * Set the default hostname
     */
    gotname = FALSE;
    hostname = NULL;
    queue_init(&hostconfgQ, 0);
    set_hostname(platform_get_string(PLATFORM_STRING_DEFAULT_HOSTNAME));

    /*
     * No banners yet
     */
    banner_motd = NULL;
    banner_exec = NULL;
    banner_incoming = NULL;
    banner_temp = NULL;
    banner_login = NULL;

    /*
     * Dump filenames not yet setup
     */
    dumpfile_def = TRUE;	/* True if dumpfile has default name */
    dumpfile = NULL;
    sl_dumpfile_def = TRUE;     /* True if slave dumpfile has default name */
    sl_dumpfile = NULL;

}

/*
 * whoami
 * Determine our hostname and build related strings.
 * Returns TRUE if we actually determined our name, FALSE if we defaulted.
 */

boolean whoami (void)
{
    char *cp;
    int ntries, success;
    char *tempname, *defstr;
    char tmpbuf[200];

    if (gotname)		/* don't do this again if it worked once */
	return(TRUE);

    ntries = 2;
    tempname = NULL;
    success = FALSE;
    while (usable_interfaces() && ntries--) {
	if (reg_invoke_name_lookup_self(tmpbuf, sizeof(tmpbuf))) {
	    setstring(&tempname, tmpbuf);
	    cp = tempname;
  	    while (*cp && (*cp != '.'))	 /* break on NUL or domain name */
		cp++;
	    *cp = '\0';			/* clobber NULL or dot */
	    lowercase(tempname);	/* force lowercase */
	    success = TRUE;
	    break;
	}
        process_sleep_for(ONESEC);
    }

    gotname = success;
    if (!success) {
	defstr = platform_get_string(PLATFORM_STRING_DEFAULT_HOSTNAME);
        setstring(&tempname, defstr);
    }
    set_hostname(tempname);
    free(tempname);
    return(gotname);
}

/*
 * set_hostname
 * Given a hostname, set our name, prompts, and host configuration filename
 * Set host configuration filename only if we don't have one.  Flag the
 * that filename as default so that the "boot" command flushes it.
 */

void set_hostname (char *name)
{
    int size, hostlen;
    filetype *cfg, *dos_cfg;
    char *tmp;
    char *bootsuffix = "(boot)";

    setstring(&hostname, name);
    hostlen = strlen(hostname);
    reg_invoke_set_hostname();

    /* Dump any old prompt strings */
    if (Eprompt) {
	if (Eprompt != nomemory)
	    free(Eprompt);
	Eprompt = NULL;
    }
    if (Pprompt) {
	if (Pprompt != nomemory)
	    free(Pprompt);
	Pprompt = NULL;
    }

    Eprompt = malloc(hostlen + strlen(bootsuffix) + 3);
    if (Eprompt != NULL) {
	Eprompt[0] = '\000';
	strcat(Eprompt, hostname);
	if (bootstrap_enable)
	    strcat(Eprompt, bootsuffix);
    } else
	Eprompt = nomemory;
    Pprompt = malloc(hostlen + strlen(bootsuffix) + 3);
    if (Pprompt != NULL) {
	Pprompt[0] = '\000';
	strcat(Pprompt, hostname);
	if (bootstrap_enable)
	    strcat(Pprompt, bootsuffix);
    } else
	Pprompt = nomemory;

    if (dumpfile_def) {
	if (dumpfile) {
	    free(dumpfile);
	    dumpfile = NULL;
	}

	dumpfile = malloc(hostlen + sizeof("-core"));
	if (dumpfile)
	  concat(hostname, "-core", dumpfile);
    }

    if (sl_dumpfile_def) {
        if (sl_dumpfile) {
            free(sl_dumpfile);
            sl_dumpfile = NULL;
        }
 
        sl_dumpfile = malloc(hostlen + sizeof("-core") + sizeof("-slotX"));
        if (sl_dumpfile)
          concat(hostname, "-core", sl_dumpfile);
    }

    if (QUEUEEMPTY(&hostconfgQ)) {
	cfg = malloc(sizeof(filetype));
	if (!cfg)
	    return;
	cfg->next = NULL;
	cfg->proto = FILE_ACCESS_TFTP;
	cfg->ps.tuba.boothost.type = ADDR_IP;
	cfg->ps.tuba.boothost.ip_addr = -1;
	cfg->defaultname = TRUE;
	cfg->resetsubr = reset_host;
	size = hostlen + sizeof(DASH_CONFG);
	cfg->filename = malloc(size);
	if (cfg->filename) {
	    concat(hostname, DASH_CONFG, cfg->filename);
	    lowercase(cfg->filename);
	    enqueue(&hostconfgQ, cfg);
	} else
	    free(cfg);

	/*
	 * Add a filetype to the hostconfgQ that honors MS-DOS file
	 * naming conventions.  If hostname is the default "Router",
	 * then use the default DOS config file name.  If hostname is
	 * not "Router" then truncate the config file name to 8 
	 * characters, if necessary.  In all cases, the config file
	 * name will end in ".cfg".
	 */
	dos_cfg = malloc(sizeof(filetype));
	if (!dos_cfg)
	    return;
	dos_cfg->next = NULL;
	dos_cfg->proto = FILE_ACCESS_TFTP;
	dos_cfg->ps.tuba.boothost.type = ADDR_IP;
	dos_cfg->ps.tuba.boothost.ip_addr = -1;
	dos_cfg->defaultname = TRUE;
	dos_cfg->resetsubr = reset_host;
	if (strcmp(hostname, "Router") == 0)
	    size = strlen(DEFAULT_DOS_CONFIG_FILE) + 1;
	else
	    size = (((hostlen > MAX_DOS_FILENAME) ? 
		    MAX_DOS_FILENAME : hostlen) + sizeof(DOT_CFG));

	dos_cfg->filename = malloc(size);
	if (dos_cfg->filename) {
	    if (strcmp(hostname, "Router") == 0)
		strcpy(dos_cfg->filename, DEFAULT_DOS_CONFIG_FILE);
	    else if (hostlen <= MAX_DOS_FILENAME)
	        concat(hostname, DOT_CFG, dos_cfg->filename);
	    else {
		sstrncpy(dos_cfg->filename, hostname, MAX_DOS_FILENAME + 1);
		strcat(dos_cfg->filename, DOT_CFG);
	    }
	    lowercase(dos_cfg->filename);
	    enqueue(&hostconfgQ, dos_cfg);
	} else
	    free(dos_cfg);
    } else {
	/*
	 * Are we still trying to boot a default host config file?  If
	 * so, change the name to match our new hostname.  This is
	 * mainly intended for when you figure out your name after the
	 * first call to whoami(), which sets up the gateway-confg or
	 * ts-confg defaults.
	 */
	for (cfg = (filetype *) hostconfgQ.qhead; cfg; cfg = cfg->next) {
	    if (cfg->defaultname == TRUE) {
		if (strstr(cfg->filename, DASH_CONFG))
		    size = hostlen + sizeof(DASH_CONFG);
		else if (strstr(cfg->filename, DOT_CFG)) {
		    if (strcmp(hostname, "Router") == 0)
	    		size = strlen(DEFAULT_DOS_CONFIG_FILE) + 1;
		    else
		        size = (((hostlen > MAX_DOS_FILENAME) ? 
			        MAX_DOS_FILENAME : hostlen) + sizeof(DOT_CFG));
		}
		else
		    continue; /* Unknown config file name, so skip it */
		tmp = malloc(size);
		if (tmp) {
		    if (strstr(cfg->filename, DASH_CONFG))
			concat(hostname, DASH_CONFG, tmp);
		    else if (strstr(cfg->filename, DOT_CFG)) {
		        if (strcmp(hostname, "Router") == 0)
			    strcpy(tmp, DEFAULT_DOS_CONFIG_FILE);
			else {
			    sstrncpy(tmp, hostname, MAX_DOS_FILENAME + 1);
			    strcat(tmp, DOT_CFG);
			}
		    }
		    lowercase(tmp);
		    free(cfg->filename);
		    cfg->filename = tmp;
		}
	    }
	}
    }
}

/*
 * hostname_command
 * Set our hostname
 */

void hostname_command (parseinfo *csb)
{

    char *new_hostname;

    if (csb->nvgen) {
	nv_write((boolean)hostname, "!\n%s %s\n!", csb->nv_command, hostname);
	return;
    }

    new_hostname = GETOBJ(string,1);
    if ((NULL == new_hostname) || ('\0' == new_hostname[0])) {
	csb->sense = FALSE;  /* treat as ""  as  "no hostname" command */
    }

    if (csb->sense) {
	set_hostname(GETOBJ(string,1));
	gotname = TRUE;
    } else {
	if (router_enable) {
	    set_hostname("Router");
	} else {
	    set_hostname("TS");
	}
	gotname = FALSE;
    }
    reg_invoke_hostname_changed(gotname);
}

/*
 * banner
 * Extract a banner message from the configuration file.
 * Format is the keyword "banner", followed by whitespace, followed by
 * a break character, followed by one or more lines of text terminated
 * by a second occurence of the break character.
 *
 * WARNING: This function contains multiple return points, each of which
 *  may need to free the malloc'ed buffer.
 */

#define	BANNER_SIZE	SABUFSIZE
#define	BANNER_SPACE	2
/*
 * BANNER_TERMINATOR was added to prevent a crash listed in CSCdi78265.  The
 * crash happened on two boundary conditions with different input data. In 
 * both cases the "\0" got overwritten.  An additional two bytes had to be
 * subtracted from the BANNER_SIZE to prevent the problem.
 */
#define BANNER_TERMINATOR  2



void banner (parseinfo *csb, char **banner_ptr)
{
    char *start, *temp;
    int count, i;
    char breakchar, lastchar;
    char *buffer, *tmpbuff;
    boolean first = TRUE;

    if (csb->nvgen) {
	if (*banner_ptr) {
	    char *startfrom;

	    startfrom = buffer = *banner_ptr;

	    nv_write(TRUE, "%s \003", csb->nv_command);
	    while (*buffer) {
		if ((*buffer == '\r') && (*(buffer+1) == '\n')) {
		    /* 
		     * temporarly alter banner_ptr to remove the \r and
		     * shift the \n forward one byte
		     */
		    *buffer = '\n'; 
		    *(buffer + 1) = '\0';
		    nv_add(TRUE, "%r", startfrom);
		    /* 
		     * restore buffer 
		     */
		    *buffer = '\r';
		    *(buffer + 1) = '\n';
		    /* 
		     * bump buffer to point to next character 
		     */
		    buffer += 2;	
		    startfrom = buffer;
		}
		buffer++;
	    }
	    if (startfrom != buffer) {
		nv_add(TRUE, "%r", startfrom);
	    }
	    nv_add(TRUE, "\003");
	}
	return;
    }

    free(*banner_ptr);
    *banner_ptr = NULL;
    if (csb->sense == FALSE)
 	return;

    tmpbuff = malloc(BANNER_SIZE);
    if (tmpbuff == NULL) {
	return;
    }
    strcpy(tmpbuff, GETOBJ(string,1));
    buffer = (char *) deblank(tmpbuff);
    breakchar = *buffer;			/* identify break char */
    if ((breakchar == '\0') || (breakchar == '\r') || (breakchar == '\n'))
	goto punt;

    start = ++buffer;			/* remember starting byte */
    /*
     * Continue to read characters until the breakchar is encountered or
     * until maximum banner size.  Subtract BANNER_TERMINATOR from
     * BANNER_SIZE to prevent CSCdi78265 memory corruption problem.  The 
     * BANNER_TERMINATOR accounts for 2 extra bytes which prevents the 
     * "\0" from being overwritten.
     */

     while ((*buffer != breakchar) &&
           ((buffer - tmpbuff) < (BANNER_SIZE - BANNER_SPACE - BANNER_TERMINATOR))) {

	if (*buffer == '\0') {
	    if (first) {
		if (csb->flags & CONFIG_TERM) {
		    printf("\nEnter TEXT message.  End with the character"
			   " '%c'.\n", breakchar);
		}
		first = FALSE;
	    }
	    i = FALSE;
	    /*
	     * perhaps read more data from terminal
	     */
	    temp = buffer;
	    *temp++ = '\r';
	    *temp++ = '\n';
	    if (csb->flags & CONFIG_TERM) {
		i = texti(temp,
			  (BANNER_SIZE - BANNER_SPACE) - (buffer - tmpbuff),
			  "\r\n", &lastchar);
	    } else {
		i = batch_input(csb, temp, (BANNER_SIZE - BANNER_SPACE) -
				(buffer - tmpbuff), "\r\n", &lastchar);
	    }
	    if (i == FALSE) {
		csb->flags |= CONFIG_ABORT;
		goto punt;
	    } else if (lastchar == '\032')
		csb->flags |= CONFIG_DONE;
	    else if (*buffer != '\0') 
		continue;
	    printf("\nUnterminated message string");
	    goto punt;
	} /* *buffer == 0 */
	buffer++;
    }  /* while */
    count = 1 + (int)(buffer - start);	/* Count bytes in message */
    *banner_ptr = malloc(count);	/* Get a buffer */
    if (!*banner_ptr) {
	goto punt;
    }
    temp = *banner_ptr;
    for (i = 1; i != count; i++)
	*temp++ = *start++;			/* copy banner */
    *temp++ = '\0';				/* tie off buffer */
 punt:
    if (tmpbuff)
	free(tmpbuff);
}

/*
 * banner_command
 * Set system banner
 */

void banner_command (parseinfo *csb)
{
    char **banner_ptr = &banner_motd;

    switch (GETOBJ(int,1)) {
      case BANNER_EXEC:
	banner_ptr = &banner_exec;
	break;
      case BANNER_INCOMING:
	banner_ptr = &banner_incoming;
	break;
      case BANNER_LOGIN:
	banner_ptr = &banner_login;
	break;
      default:
      case BANNER_MOTD:
	banner_ptr = &banner_motd;
	break;
    }
    banner(csb,banner_ptr);
}
