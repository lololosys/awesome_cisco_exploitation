/* $Id: ciscopro.c,v 3.2.64.1 1996/03/18 20:40:27 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/les/ciscopro.c,v $
 *------------------------------------------------------------------
 * ciscopro.c - CiscoPro command filtering
 *
 * November 1995, Eric Buell
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ciscopro.c,v $
 * Revision 3.2.64.1  1996/03/18  20:40:27  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.28.2  1996/03/07  10:04:59  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.2.28.1  1996/01/24  22:22:12  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.2  1995/12/03  17:57:14  thille
 * CSCdi44905:  ciscopro targets not in 11.1
 * Put them in.
 *
 * Revision 3.1  1995/11/18  07:05:47  thille
 * Add placeholder.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <ctype.h>
#undef   tolower			/* use library function call */
#include "sys_registry.h"
#include "config.h"
#include "subsys.h"
#include "parser.h"

static ciscopro_debug = FALSE;

/*
 * Disallowed commands
 *	Commands must be specified with the shortest keyword
 *	prefixes.  The parse chain pointers determine in which
 *	command/configuration mode the disallowed command is
 *	to be checked in.  Commands issues in modes other than
 *	top or top_configure are often also checked for
 *	validity in top_configure mode (see parser/pasrser.c:
 *	parse_cmd()).
 */

struct disallowed_commands_ {
    char *line;				/* command to be disallowed */
    char *mode_name;			/* name of the parser mode this
					   command applies to */
};

/*
 *	Disallowed commands for the cpa25-y-l image (default)
 */
static struct disallowed_commands_ disallowed_commands_y[] = {
    {"de cu",     "exec"},			/* debug custom-queue ... */
    {"de ei",     "exec"},			/* debug eigrp ... */
    {"de ll",     "exec"},			/* debug llc2 ... */
    {"de pri",    "exec"},			/* debug priority ... */
    {"u cu",      "exec"},			/* undebug custom-queue ... */
    {"u ei",      "exec"},			/* undebug eigrp ... */
    {"u ll",      "exec"},			/* undebug llc2 ... */
    {"u pri",     "exec"},			/* undebug priority ... */
    {"sh ll",     "exec"},			/* show llc2 ... */
    {"sh queuei", "exec"},			/* show queueing... */
    {"sh cm",     "exec"},			/* show cmns ... */
    {"prio",      "configure"},			/* priority-list ... */
    {"q",         "configure"},			/* queue-list ... */
    {"router is", "configure"},			/* router isis ... */
    {"router ei", "configure"},			/* router eigrp ... */
    {"router iso","configure"},			/* router iso-igrp ... */
    {"f s",       "configure"},			/* frame-relay switching */
    {"x25 routi", "configure"},			/* x25 routing ... */
    {"x25 route", "configure"},			/* x25 route ... */
    {"en x dc",   "interface"},			/* encapsulation x25 dce ... */
    {"fr intf n" ,"interface"},			/* frame-relay intf-type nni ... */
    {"tu m g",    "interface"},			/* tunnel mode gre ... */
    {"cm",        "interface"},			/* cmns ... */
    {"cu",        "interface"},			/* custom-queue-list ... */
    {"ll",        "interface"},			/* llc2 ... */
    {"pr",        "interface"},			/* priority-group ... */
    {NULL, NULL}
};

/*
 *	Disallowed commands for the cpa25-cg-l image
 */
static struct disallowed_commands_ disallowed_commands_cg[] = {
    {"de cu",     "exec"},			/* debug custom-queue ... */
    {"de ei",     "exec"},			/* debug eigrp ... */
    {"de ll",     "exec"},			/* debug llc2 ... */
    {"de pri",    "exec"},			/* debug priority ... */
    {"u cu",      "exec"},			/* undebug custom-queue ... */
    {"u ei",      "exec"},			/* undebug eigrp ... */
    {"u ll",      "exec"},			/* undebug llc2 ... */
    {"u pri",     "exec"},			/* undebug priority ... */
    {"sh ll",     "exec"},			/* show llc2 ... */
    {"sh queuei", "exec"},			/* show queueing... */
    {"sh cm",     "exec"},			/* show cmns ... */
    {"prio",      "configure"},			/* priority-list ... */
    {"q",         "configure"},			/* queue-list ... */
    {"router is", "configure"},			/* router isis ... */
    {"router ei", "configure"},			/* router <eigrp> ... */
    {"router iso","configure"},			/* router iso-igrp ... */
    {"f s",       "configure"},			/* frame-relay switching */
    {"x25 routi", "configure"},			/* x25 routing ... */
    {"x25 route", "configure"},			/* x25 route ... */
    {"en x dc",   "interface"},			/* encapsulation x25 dce ... */
    {"fr intf n", "interface"},			/* frame-relay intf-type nni ... */
    {"tu m g",    "interface"},			/* tunnel mode gre ... */
    {"cm",        "interface"},			/* cmns ... */
    {"cu",        "interface"},			/* custom-queue-list ... */
    {"ll",        "interface"},			/* llc2 ... */
    {"pr",        "interface"},			/* priority-group ... */
    {NULL, NULL}
};

/*
 *	Disallowed commands for the cpa1000* and cpa1005* images
 */
static struct disallowed_commands_ disallowed_commands_h[] = {
    {"de ei",     "exec"},			/* debug eigrp ... */
    {"u ei",      "exec"},			/* undebug eigrp ... */
    {"fr sw",     "configure"},			/* frame-relay switching */
    {"router is", "configure"},			/* router isis ... */
    {"router iso","configure"},			/* router iso-igrp ... */
    {"router b",  "configure"},			/* router bgp ... */
    {"router e",  "configure"},			/* router egp + router eigrp */
    {"router o",  "configure"},			/* router ospf */
    {"x25 route", "configure"},			/* x25 route <X.121 address> */
    {"tu m g",    "interface"},			/* tunnel mode gre ... */
    {"ap ei",     "interface"},		        /* appletalk eigrp .. */
    {"ap p e",    "interface"},			/* appletalk protocol eigrp ... */
    {"pr",        "interface"},			/* priority-group ... */
    {"cu",        "interface"},			/* custom-queue-list ... */
    {"en x dc",   "interface"},			/* encapsulation x25 dce ... */
    {"fr intf dc","interface"},			/* frame-relay intf-type dce ... */
    {NULL, NULL}
};

static struct disallowed_commands_ *image_dis_cmd;

static boolean
is_whitespace (char character)
{
    if (character == ' ' || character == '\t')
	return(TRUE);
    return(FALSE);
}

static boolean
is_last_keyword (char *kwp)
{
    while (!is_whitespace(*kwp) && *kwp != '\0')
	kwp++;
    if (*kwp == '\0')
	return(TRUE);
    return(FALSE);
}

static void
skip_to_next_keyword (char **cptr)
{

    while (!is_whitespace(**cptr) && **cptr != '\0') {
	*cptr += 1;
    }
    while (is_whitespace(**cptr))
	*cptr += 1;
}

static boolean
keyword_prefix (char *prefix, char *keyword, int *mismatch_offset)
{
    *mismatch_offset = 0;

    while (!is_whitespace(*prefix) && *prefix != '\0') {
	if (tolower(*prefix) != tolower(*keyword))
	    return(FALSE);
	prefix++;
	keyword++;
	*mismatch_offset += 1;
    }
    return(TRUE);
}

/*
 *  Check for a disallowed command match
 *	Scan for disallowed commands.  CiscoPro disallows
 *	a number of routing/routed protocols and WAN protocols.
 *	Because these protocols don't yet exist as separate subsytems
 *	this is how they are "removed" for CiscoPro.
 */
static boolean
check_for_disallowed_command (parseinfo *csb, char *command_line,
			      char *short_help, int *error_index)
{
    struct disallowed_commands_ *discmdp;
    char *clp;				       /* command line pointer */
    char *dcp;				       /* disallowed command pointer */
    int mismatch_offset;

    for (discmdp = image_dis_cmd; discmdp->line != NULL; discmdp++) {

	if (strcmp(get_mode_name(csb->mode), discmdp->mode_name) != 0) {
				/* if wrong mode for disallowed command (eg.
				   enable vs. config mode) ... */
	    continue;
	}

	clp = command_line;
	while (is_whitespace(*clp))
	    clp++;
	if (keyword_prefix("no", clp, &mismatch_offset)) {
	    skip_to_next_keyword(&clp);
	}
	dcp = discmdp->line;

	while (*clp != '\0'  && *dcp != '\0') {
				/* loop to test each keyword of the disallowed
				   command against the entered command */
	    if (keyword_prefix(dcp, clp, &mismatch_offset) ||
				/* if the disallowed keyword is a prefix
				   of the command keyword or ... */
		(is_last_keyword(dcp) &&
		 *(clp+mismatch_offset) == '\0' &&
		  keyword_prefix(dcp, (char *)short_help, &mismatch_offset))) {
		/* .. we're checking the last disallowed keyword and
		   the match failed because the command keyword was too short
		   and the disallowed keyword is a prefix of the help keyword
		   then consider this to be a match */
		skip_to_next_keyword(&dcp);
		if (*dcp == '\0') {
		    /* if a complete match was found, return point of
		       match in command line */
		    *error_index = clp+mismatch_offset - command_line - 1;
		    if (ciscopro_debug)
			printf("Filtered -- %s %s\n", command_line, short_help);
		    return(TRUE);
		}
		skip_to_next_keyword(&clp);
	    } else {
		break;
	    }
	}
    }
    return(FALSE);		/* command is not disallowed */
}


/*
 * ciscopro_preparse_filter()
 *	Scan for disallowed commands.  Return TRUE if not allowed
 *	with csb->error_index set.
 */
static boolean
ciscopro_preparse_filter (parseinfo *csb)
{
    int error_index;

    if (check_for_disallowed_command(csb, csb->line, "", &error_index)) {
	csb->error_index = error_index;
	return(TRUE);
    }
    return(FALSE);
}


/*
 * ciscopro_long_help_filter()
 *	Scan for disallowed commands.  Don't save help info for
 *	these commands.  Return TRUE if the help should be filtered.
 *      "help" is appended to csb->line to form what would be the
 *      command if the help keyword were used.  csb->line is restored
 *      at exit.
 */
static boolean
ciscopro_long_help_filter (parseinfo *csb, const char *help)
{
    int error_index;
    int command_length;
    boolean result;

    command_length = strlen(csb->line);
    if ((command_length + strlen(help) + 1) > PARSEBUF)
	return(FALSE);

    strcpy(csb->line + command_length, help); 
    result = check_for_disallowed_command(csb, csb->line, "", &error_index);
    *(csb->line + command_length) = '\0';

    return(result);
}


/*
 * ciscopro_short_help_filter()
 *	Scan for disallowed commands.  Don't save help info for
 *	these commands.  Return TRUE if the help should be filtered.
 */
static boolean
ciscopro_short_help_filter (parseinfo *csb, const char *help)
{
    int error_index;

    return(check_for_disallowed_command(csb, csb->line, (char *)help,
					&error_index));
}

/*
 * ciscopro_subsys_init()
 *	Initialize CiscoPro subsystem for disallowing some
 *	commands/protocols
 */
static void
ciscopro_subsys_init (subsystype *subsys)
{
    /*
     *  Select the disallowed command list appropriate for the image
     */
    switch(ciscopro_image) {

    case 1:				/* cpa25-y-l */
	image_dis_cmd = disallowed_commands_y;
	break;

    case 2:				/* cpa25-cg-l */
	image_dis_cmd = disallowed_commands_cg;
	break;

    case 3:				/* cpa1000-h-m */
	image_dis_cmd = disallowed_commands_h;
	break;

    default:				/* probably the most restrictive */
	image_dis_cmd = disallowed_commands_y;
	break;
    }

    reg_add_parser_preparse_filtered(ciscopro_preparse_filter,
				     "ciscopro_preparse_filter");
    reg_add_parser_short_help_filtered(ciscopro_short_help_filter,
				       "ciscopro_short_help_filter");
    reg_add_parser_long_help_filtered(ciscopro_long_help_filter,
				      "ciscopro_long_help_filter");
}

/*
 * CiscoPro subsystem header
 */
#define CISCOPRO_MAJVERSION   1
#define CISCOPRO_MINVERSION   0
#define CISCOPRO_EDITVERSION  1

SUBSYS_HEADER(CiscoPro, CISCOPRO_MAJVERSION, CISCOPRO_MINVERSION,
	      CISCOPRO_EDITVERSION,
	      ciscopro_subsys_init, SUBSYS_CLASS_KERNEL,
	      NULL, NULL);

