/* $Id: moncmds_rp1.c,v 3.1.58.1 1996/03/18 18:34:52 gstovall Exp $
 * $Source: /release/111/cvs/Xboot/src-68-c7000/moncmds_rp1.c,v $
 *------------------------------------------------------------------
 * Support for RP1 specific rom monitor commands
 *
 * February 1995, Stig Thormodsrud
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: moncmds_rp1.c,v $
 * Revision 3.1.58.1  1996/03/18  18:34:52  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.1.26.1  1996/03/16  06:10:33  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.1  1995/11/09  09:10:21  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  19:17:07  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 * Revision 1.2  1995/06/07 09:05:13  smackie
 * Fold back Arizona_branch into mainline.
 *
 * Revision 1.1.4.1  1995/05/02  23:22:34  sthormod
 * Branch: Arizona_branch
 * Rom monitor support for RP1 flash credit card
 *
 * Revision 1.1  1995/04/23  00:22:46  sthormod
 * Placeholders for Arizona branch.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "../defs.h"
#include "../reason.h"
#include "../stdio.h"
#include "../extern.h"
#include "nvmonvars.h"


char *next_space (char *s)
{
    if (!s)
	return NULL;

    while (*s && *s != ' ' && *s != '\n') /* go to next whitespace separator */
        s++;
    return(s);
}


char *next_char (char *s)
{
    if (!s)
	return NULL;

    while (*s == ' ')       /* skip the whitespaces */
        s++;
    return(s);
}

inline boolean token_match (char *command, char *arg) 
{
    if (strncmp(command, arg, strlen(command)) == 0) 
	return TRUE;
    else
	return FALSE;
}

/*
 * consume_rp1_commanad
 *
 * examine the command, return FALSE if we didn't handle the command
 * in this routine.
 */
boolean consume_rp1_command (char *command)
{
    int retval;
    char *arg;
    extern void show_dev(void);

    /*
     * brute force approach to command parsing
     */
    arg = next_char(command);

    if (*arg == '?' || *arg == 'h') {
	printf("dev          Show valid devices\n");  
	printf("dir [device] Show files on device\n");
	printf("set          Show environment variables\n");
	printf("unset [var]  Remove an environment variable\n");
	return FALSE;  /* show other help menu */
    }

    if (token_match("dev", arg)) {
	show_dev();
	return TRUE;
    }
    
    if (token_match("dir", arg)) {
	arg = next_char(next_space(arg));
	if (!arg || !*arg)
	    goto dir_usage;
	if (token_match("slot0:", arg)) {
	    arg = next_char(next_space(arg));
	    if (token_match("/all", arg)) 
		show_flash_card(TRUE); 
	    else
		show_flash_card(FALSE);
	    return TRUE;
	} 
	if (token_match("flash:", arg)) {
	    arg = next_char(next_space(arg));
	    if (token_match("/all", arg)) 
		show_flash(TRUE);
	    else 
		show_flash(FALSE);
	    return TRUE;
	}
	puts("bad device name\n");
    dir_usage:
	puts("usage: dir <device>\n");
	return TRUE;
    }
    
    if (token_match("set", arg)) {
	printtbl(nvram->evartbl);
	return TRUE;
    }
    
    if (token_match("unset", arg)) {
	arg = next_char(next_space(arg));
	if (!arg || *arg == '\0') {
	    printf("No environment variable specified\n");
	    return TRUE;
	} else {
	    retval = unsetvar(nvram->evartbl, arg);
	    if (retval < 0)
		printf("[FAILED]\n");
	    else {
		retval = setmonvar(NULL);
		printf("[OK] %d bytes free\n", retval);
	    }
	    savenv();
	    return TRUE;
	}
    }

    if (token_match("nvinit", arg)) {
	nvram_reset();
	retval = setmonvar(NULL);
	printf("[OK] %d bytes free\n", retval);
	return TRUE;
    }

    if (token_match("bootinit", arg)) {
	reset_autoboot();
	return TRUE;
    }

    if (token_match("bootstat", arg)) {
        show_autoboot();
	return TRUE;
    }

    if (token_match("monlib", arg)) {
	if (init_monlib(TRUE))
	    monlib_initialized = TRUE;
	else 
	    printf("Error: Failed to initialize monlib\n");
	return TRUE;
    }
    
    if (token_match("stack", arg)) {
	check_stack_corruption(TRUE);
	return TRUE;
    }

    if (token_match("debug", arg)) {
	arg = next_char(next_space(arg));
	debug_set(arg);
	if (token_match("monlib", arg)) {
	    change_monlib_flags();
	}
	return TRUE;
    }

    if (token_match("undebug", arg)) {
	arg = next_char(next_space(arg));
	debug_unset(arg);
	if (token_match("monlib", arg)) {
	    change_monlib_flags();
	}
	return TRUE;
    }

    /*
     * Set environment variable
     *
     * expected syntax:  <VARNAME>=<VALUE>
     */
    if (strchr(arg, '=') != '\0') {
	retval = setmonvar(arg);
	if (retval < 0) 
	    printf("[FAILED]\n");
	else
	    printf("[OK] %d bytes free\n", retval);
	return TRUE;
    }

    /*
     * Return FALSE so that the common commands are checked
     */
    return FALSE;
}

void show_dev (void)
{
    printf("Devices in device table:\n");
    printf("        id  name\n");
    printf("    flash:  internal flash\n");
    if (flash_card_valid())
	printf("    slot0:  flash credit card\n");
}



