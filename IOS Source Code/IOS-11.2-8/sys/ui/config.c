/* $Id: config.c,v 3.6.18.6 1996/09/06 20:09:55 bew Exp $
 * $Source: /release/112/cvs/Xsys/ui/config.c,v $
 *------------------------------------------------------------------
 * config.c -- parse and act upon configuration commands
 *
 * June 1985, Kirk Lougheed
 *
 * Copyright (c) 1985-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: config.c,v $
 * Revision 3.6.18.6  1996/09/06  20:09:55  bew
 * CSCdi67482:  crypto dss can be displayed
 * Branch: California_branch
 * When evaluating the parse chain, don't display an eror message for
 * and unrecognized command if the parse chain is the private config.
 * Before, data which was intended to be hidden was displayed.
 *
 * Revision 3.6.18.5  1996/08/28  13:18:40  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.6.18.4  1996/05/21  10:07:41  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.6.18.3  1996/05/09  14:47:53  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.6.18.2  1996/04/27  06:29:16  syiu
 * Branch: California_branch
 * Commit Dial1_branch
 *
 * Revision 3.6.18.1.18.1  1996/04/27  07:37:54  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.6.18.1.8.2  1996/04/10  23:48:46  cakyol
 * change misleading variable name
 * from "global_boottime_config_version"
 * to "global_parsed_swversion".
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 3.6.18.1.8.1  1996/04/08  02:10:48  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 3.6.18.1  1996/03/18  22:28:54  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.6.20.2  1996/04/05  03:19:55  cakyol
 * started using the %Cx Cisco extensions to printf.  These are:
 *
 *   j: uchar*, atm RAW bytes.
 *
 *   h: hwaddrtype*, where type is either STATION_IEEE48 or
 *      STATION_AIPNSAP.
 *
 *   w: MaskedAddress*, where type is either STATION_IEEE48 or
 *      STATION_AIPNSAP.
 *
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.6.20.1  1996/03/25  02:21:26  cakyol
 * sync LE_Syn_Cal_V111_1_3_branch to LE_Cal_V111_1_0_3_branch
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.6.2.1  1996/03/22  05:26:04  cakyol
 * Found a better way of determining the version number of the
 * config file at boot time.  Leverage from parsing of the
 * "version xx.yy" command.  Define a new variable which
 * represents this value named "global_boottime_config_version".
 * Restore all the other files back to what they were before this.
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 3.6  1996/02/07  16:16:40  widmer
 * CSCdi48285:  Remove function name from bad_parser_subcommand
 *
 * Revision 3.5  1995/12/17  18:40:14  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.4  1995/11/22  03:19:46  irfan
 * CSCdi41430:  Async lines may reset their configuration
 * Obituary: tty_reset_line_defaults() died today. All time favourite pain
 * in the neck.
 *
 * Revision 3.3  1995/11/17  17:47:28  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:45:59  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:40:46  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.5  1995/08/15  23:25:08  tli
 * CSCdi36981:  Need to be able to generate downward compatible
 * configurations
 *
 * Revision 2.4  1995/06/20  07:19:42  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.3  1995/06/16  04:30:04  dkatz
 * CSCdi35882:  Time-of-day code needs cleanup
 *
 * Revision 2.2  1995/06/09  23:40:37  achopra
 * CSCdi04836:  Provide ip-on-break functionality on TS
 *
 * Revision 2.1  1995/06/07  23:11:05  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include <ciscolib.h>
#include "interface_private.h"
#include "ttysrv.h"
#include "config.h"
#include "logger.h"
#include "parser.h"
#include "aaa.h"
#include "../parser/parser_actions.h"
#include "../parser/msg_parser.c"	/* Not a typo, see logger.h */
#include "command.h"
#include "../os/async.h"
#include "../parser/parser_defs_breakset.h"
#include "../parser/parser_defs_config.h"
#include "../parser/parser_defs_exec.h"
#include "../os/clock.h"
#include "sys_registry.h"
#include "../dev/flash_registry.h"
#include "../os/msg_system.c"		/* Not a typo, see logger.h */
#include "../os/config_compress.h"

/*
 * External declarations
 */
extern long fastscan_fork, fastscan_delta;
extern void modemscan_command(parseinfo *csb);

/*
 * Local variables
 */

static clock_epoch last_config_time; /* Last time a config command was done */
static boolean config_was_done;	 /* TRUE if a config was done */
static char *last_config_user;	/* Name of user who updated config */
static clock_epoch last_write_mem_time; /* Last time a "write mem" was done */
static char *last_write_mem_user; /* Name of user who did write mem */
static char timestring[80];
static uint downward_major;
static uint downward_minor;

/*
 * note_config_time - note the current time
 *
 * Don't note it for the bootup config.
 */
static void note_config_time (void)
{
    /* Don't do it if this is the boot time configuration */

    if (system_configured) {

	/* Only update the time if the clock is valid. */

	if (clock_is_probably_valid()) {
	    clock_get_time(&last_config_time);
	}
	config_was_done = TRUE;

	/* Update the user name. */

	if (last_config_user) {
	    free(last_config_user);
	    last_config_user = NULL;
	}
	if (old_access_control) {
	    if (stdio && stdio->user) {
		setstring(&last_config_user, stdio->user);
	    }
	} else { /* ! old_access_control */
	    aaa_get_cred(stdio->ustruct, &last_config_user,
			 NULL, NULL, NULL, NULL);
	}
    }
}

/*
 * note_write_mem_time - note the current time
 */
void note_write_mem_time (void)
{

    /* Only update the time if the clock is valid. */

    if (clock_is_probably_valid()) {
	clock_get_time(&last_write_mem_time);

	/* Update the user name. */

	if (last_write_mem_user) {
	    free(last_write_mem_user);
	    last_write_mem_user = NULL;
	}
	if (old_access_control) {
	    if (stdio && stdio->user) {
		setstring(&last_write_mem_user, stdio->user);
	    }
	} else { /* ! old_access_control */
	    aaa_get_cred(stdio->ustruct, &last_write_mem_user,
			 NULL, NULL, NULL, NULL);
	}
    }
}

/*
 * last_config_time_string - return a string with the last config time
 *
 * returns a nice string to print, or NULL
 */
char *last_config_time_string (void)
{
    /* If we haven't configured yet but we're keeping time, say so. */

    if (clock_is_probably_valid() && !config_was_done)
	return("! No configuration change since last restart");

    /*
     * If we have configured but the time wasn't written down, don't
     * say anything--we're not keeping decent time.
     */

    if (last_config_time.epoch_secs == 0)
	return(NULL);

    /* Otherwise, make a nice string. */

    sprintf(timestring, "! Last configuration change at %CC", "%G",
	    &last_config_time);
    if (last_config_user) {
	sprintf(&timestring[strlen(timestring)], " by %s", last_config_user);
    }
    return(timestring);
}

/*
 * last_write_mem_time_string - return a string with the last "write mem" time.
 *
 * returns a nice string to print, or NULL.
 */
char *last_write_mem_time_string (void)
{
    /* If we haven't written down the time, don't say anything. */

    if (last_write_mem_time.epoch_secs == 0)
	return(NULL);

    /* Otherwise, make a nice string. */

    sprintf(timestring, "! NVRAM config last updated at %CC", "%G",
	    &last_write_mem_time);

    if (last_write_mem_user) {
	sprintf(&timestring[strlen(timestring)], " by %s", 
		last_write_mem_user);
    }
    return(timestring);
}


void parse_configure_idb (char *buffer, int permanent,
		      int resolvemethod, uchar priv, idbtype *idb)
{
    int error;
    boolean end_found;
    parseinfo *csb;

    csb = get_csb("Parse_Configure Parser CSB");
    if (csb == NULL) {
	return;
    }

    csb->priv = priv;
    csb->resolvemethod = resolvemethod;
    csb->mode = config_mode;
    csb->break_chars = NVCONFIG_BREAKSET;
    csb->batch_buf = buffer;
    csb->batch_offset = buffer;
    csb->permanent = permanent;
    csb->flags = 0;

    if (idb)
	csb->interface=idb;

    end_found = FALSE;
    while (batch_input(csb, csb->line, PARSEBUF, 
		       csb->break_chars, &csb->lastchar)) {
	if (*csb->line && (*csb->line != '!')) {    /* Skip empty lines */
	    if (((error = parse_cmd(csb)) != 0) && 
		(resolvemethod != RES_PRIVATE_NONVOL)) {
		/* 
		 * Only print an error message if the config statement was NOT
		 * in the private configuration. If a private config statement
		 * is printed, that which was never supposed to be revealed to
		 * the user is revealed.
		 */
		report_error(csb, error, TRUE);
	    }
	    if (strcmp(csb->line, "end") == 0) {
		end_found = TRUE;
	        break;
	    }
	}
    }
    if (!end_found) {
        errmsg(&msgsym(BADCFG, PARSER));
    }
    /* Mind our Free's & Q's */	
    free_csb(&csb);
}

/*
 * parse_configure
 * Parse contents of the configuration buffer
 */
void parse_configure (char *buffer, int permanent,
		      int resolvemethod, uchar priv)
{
    parse_configure_idb( buffer, permanent, resolvemethod, priv, NULL);
}


/* gettoken
 * Return a token, either a single word or a single line.
 * Callers must be able to cope with a nil pointer being returned
 */

char *gettoken (char *buffer, char *token, register int toklen,
		int which)
{
    *token = '\0';			/* tie off token string */
    if (!buffer)			/* guard against a NULL pointer */
	return(NULL);
    while ((*buffer == ' ') || (*buffer == '\t'))
	buffer++;
    switch (which) {
    case WORD:
	while (term_char(*buffer) == FALSE) {
	    if (--toklen == 0) {
		*token = '\0'; /* close token w/ NULL */
		return(NULL);
	    }
	    *token++ = *buffer++;
	}
	break;
    case LINE:
	while ((*buffer != '\r') && (*buffer != '\n') &&
	       (*buffer != '\0')) {
	    if (--toklen == 0) {
		*token = '\0'; /* close token w/ NULL */
		return(NULL);
	    }
	    *token++ = *buffer++;
	}
	break;
    }
    *token++ = '\0';
    if (*buffer != '\0') {
	return(buffer);
    } else {
	return(NULL);
    }
}

/*
 * parse_var_unsigned
 * 
 * Parse an unsigned number, allowing -1 as legitimate, based on variable
 * format. Currently supports octal ('o') and decimal ('d').
 */
boolean parse_var_unsigned (char *buff, char fmt, ulong *naddr)
{
    char *s;
    char maxchar;
    int  multiplier;
    
    if (!buff || (*buff == '\0'))
	return(FALSE);
    s = deblank(buff);
    *naddr = 0;
    if (fmt == 'o') {
	maxchar = '7';
	multiplier = 8;
    } else {
	maxchar = '9';
	multiplier = 10;
    }
    while (*s) {
	if ((*s < '0') || (*s > maxchar))
	    return(FALSE);
	*naddr = (multiplier * *naddr) + (*s++ - '0');
    }
    return(TRUE);
}


/*
 * get_ttyline
 * Extract an octal line number from a buffer.
 * Verify that it is a number and that we have such a terminal line.
 * Returns line number if good, -1 otherwise
 */

int get_ttyline (char *buff)
{
    int i;
    boolean parsed;
    
    buff = (char *) deblank (buff);	/* clear leading white space */
    if (null(buff) || !t_number(buff))
	return (-1);
    if (decimaltty_enable)
	i = parse_unsigned(buff, &parsed);
    else
	i = atoo(buff);
    if ((!parsed) || (i < 0) || (i >= ALLlines))
	return (-1);
    return (i);
}

/*
 * settoken
 * Put string in csb->token into a tty data block
 */

void settoken (parseinfo *csb, char *str, void *dummy)
{		
    int i, offset;
    uchar **dest;
    uchar *string = NULL;
    uchar *modemzero;
    
    modemzero = (uchar *)dummy;
    offset = (int) (modemzero - (uchar *) &console->type);
    setstring((char **) &string, str);
    
    for (i = csb->line1; i < csb->line2; i++) {
	if (MODEMS[i]) {
	    dest = (uchar **) ((uchar *) MODEMS[i] + offset);
	    free(*dest);
	    if (string)
		mem_lock(string);
	    *dest = string;
	}
    }
    free(string);
}

/*
 * setinteger
 * Set an integer value into a tty data block
 */

void setinteger (parseinfo *csb, int value, void *dummy)
{		
    int i, offset;
    uchar *modemzero;
    
    modemzero = (uchar *)dummy;
    offset = (int) (modemzero - (uchar *)&console->type);
    for (i = csb->line1; i < csb->line2; i++) {
	*((int *) ((uchar *) MODEMS[i] + offset)) = value;
    }
}

/*
 * setuser_integer
 * Like setinteger, only set in both default and user-changable data areas.
 * create the default area if it doesn't exists yet.
 */

void setuser_integer (parseinfo *csb, int value, void *dummy)
{		
    int i, offset;
    tt_soc *tty;
    uchar *modemzero;
    
    modemzero = (uchar *)dummy;
    offset = (int) (modemzero - (uchar *)&console->current);
    for (i = csb->line1; i < csb->line2; i++) {
	tty = MODEMS[i];
	if (csb->permanent == 0 && tty->defaults == 0)
	    tty_savedefaults(tty);
	if (csb->permanent && tty->defaults)
	    *((int *) ((uchar *) tty->defaults + offset)) = value;
	*((int *) ((uchar *) (&tty->current) + offset)) = value;
    }
}

/*
 * docaps
 * Set or clear default capabilities
 */

void docaps (parseinfo *csb, ulong capflag, int index)
{
    int i;
    tt_soc *tty;
    
    for (i = csb->line1; i < csb->line2; i++) {
	tty = MODEMS[i];
        if (csb->sense) {
            switch (index) {
                case FIRST_CAP:
	    	    if (csb->permanent) {
		        tty->def_capabilities |= capflag;
	    	    }
		    tty->capabilities |= capflag;
	 	break;
		case SECOND_CAP:
	    	    if (csb->permanent) {
		        tty->def_capabilities2 |= capflag;
	    	    }
		    tty->capabilities2 |= capflag;
	        break;
            }
        } else {
            switch (index) {
                case FIRST_CAP:
                    if (csb->permanent) {
                        tty->def_capabilities &= ~capflag;
                    }
                    tty->capabilities &= ~capflag;
                break;
                case SECOND_CAP:
                    if (csb->permanent) {
                        tty->def_capabilities2 &= ~capflag;
                    }
                    tty->capabilities2 &= ~capflag;
                break;
            }
        }
    }
}		

/*
 * reset_host()
 * Reset configurable host parameters to their initial defaults. 
 */

void reset_host (void)
{
    /*
     * this used to hold:     tty_reset_line_defaults();
     * but has been dropped now because it added no functionality
     * instead kept causing problems and side effects.
     * This function is left in place for other areas of IOS
     * to possibly use in future
     */

}

/*
 * defaultval_command
 * allow default values for configuration paramters to be specified once,
 * rather than for every (interface/tty/whatever).
 * Currently only used for system-wide character masks...
 */
void defaultval_command (parseinfo *csb)
{
    if (csb->nvgen) {
	switch (csb->which) {
	case PARSER_DEFAULTCMD_EXECMASK:
	    nv_write(execmask_default != 0x7F, "%s 8", csb->nv_command);
	    break;
	case PARSER_DEFAULTCMD_SPECIALMASK:
	    nv_write(specialmask_default != 0x7F, "%s 8", csb->nv_command);
	    break;
	case PARSER_DEFAULTCMD_DATAMASK:
	    nv_write(datamask_default != 0xFF, "%s 7", csb->nv_command);
	    break;
	case PARSER_DEFAULTCMD_MODINT:
	    nv_write(fastscan_fork != 0, "%s %d", csb->nv_command,
		     fastscan_delta);
	    break;
	default:
	    bad_parser_subcommand(csb, GETOBJ(int,1));
	    break;
	}
	return;
    }
    
    switch (csb->which) {
    case PARSER_DEFAULTCMD_EXECMASK:
	setdef_mask(csb, &execmask_default);
	break;
    case PARSER_DEFAULTCMD_SPECIALMASK:
	setdef_mask(csb, &specialmask_default);
	break;
    case PARSER_DEFAULTCMD_DATAMASK:
	setdef_mask(csb, &datamask_default);
	break;
    case PARSER_DEFAULTCMD_MODINT:
	modemscan_command(csb);
	break;
    default:
	bad_parser_subcommand(csb, GETOBJ(int,1));
	break;
    }
}

void exit_config_command (parseinfo *csb)
{
    parser_mode *alt_mode;
    
    if (csb->nvgen) {
	if (csb->which == CMD_END) {
	    /* Reset buffer size which was reduced in os/nv_common.c
	     * to always save room for "\nend".
	     */
	    configbufsize += ENDSTRLEN;
	    nv_write(TRUE, "%s\n", csb->nv_command);
	}
	return;
    }
    alt_mode = get_alt_mode(csb->mode);
    while (alt_mode) {
	(void) mode_save_vars(csb);
	csb->mode = alt_mode;
	alt_mode = get_alt_mode(csb->mode);
    }
    
    if (csb->flags & CONFIG_TERM) {
	notify_config("console");
	stdio->texti_info.history = &(stdio->texti_info.exec_history);
    }
    note_config_time();
    csb->mode = exec_mode;
    csb->permanent = FALSE;
    csb->interface = 0;
    csb->protocol = 0;
    csb->line1 = 0;
    csb->line2 = 0;
}

/*
 * version_command
 * The version command just tells what version software wrote
 * the current configuration. It gets NV generated, but is
 * ignored when parsed.
 */

void version_command (parseinfo *csb)
{
    int maj, min;
    
    if (csb->nvgen) {
	/*
	 *	To generate the distilled version command
	 *	into nvram, if it is to be distilled version.
	 */
	reg_invoke_generate_nv_distilled_version(csb);
	nv_write(TRUE,"%s %d.%d",csb->nv_command,
		 SYS_MAJORVERSION, SYS_MINORVERSION);
	return;
    }
    
    maj = GETOBJ(int, 1);
    min = GETOBJ(int, 2);
    global_parsed_swversion = (256*maj + min);

    if ((maj > SYS_MAJORVERSION) ||
	((maj == SYS_MAJORVERSION) && (min > SYS_MINORVERSION))) {
	errmsg(&msgsym(CONFIG_NEWER, SYS), maj, min);
    }
}

/*
 * downward_command
 * The downward-compatible-config command just records the software version
 * number that the user requested.  The respective nv-generation routines
 * are responsible for performing appropriate actions.
 */

void downward_command (parseinfo *csb)
{
    if (csb->nvgen) {
	if (downward_major || downward_minor) {
	    if ((downward_major != SYS_MAJORVERSION) ||
		(downward_minor != SYS_MINORVERSION))
		nv_write(TRUE,"%s %d.%d", csb->nv_command, downward_major,
			 downward_minor);
	}
	return;
    }
    
    if (csb->sense) {
	downward_major = GETOBJ(int, 1);
	downward_minor = GETOBJ(int, 2);
    } else {
	downward_major = SYS_MAJORVERSION;
	downward_minor = SYS_MINORVERSION;
    }
}

/*
 * config_before
 * Return TRUE if the user requested a configuration which predates the
 * passed version numbers.  Normally you pass in the version number of the
 * release in which a feature was introduced.
 */

boolean config_before (uint major, uint minor)
{
    if (!(downward_major || downward_minor))
	return(FALSE);
    if (downward_major < major)
	return(TRUE);
    if ((downward_major == major) && (downward_minor < minor))
	return(TRUE);
    return(FALSE);
}

/*
 * config_atleast
 * Issue appropriate warnings if the version of the config that we're
 * trying to generate isn't at least the stated version.
 */

void config_atleast (parseinfo *csb, uint major, uint minor)
{
    if (config_before(major, minor))
	csb->down_gen_failure = TRUE;
}
