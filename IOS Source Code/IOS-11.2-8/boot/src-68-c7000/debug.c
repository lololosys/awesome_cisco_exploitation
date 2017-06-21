/* $Id: debug.c,v 3.1 1995/11/09 09:10:02 shaker Exp $
 * $Source: /swtal/cherf.111/ios/boot/src-68-c7000/debug.c,v $
 *------------------------------------------------------------------
 * Support for RP1 specific debugging flags
 *
 * April 1995, Stig Thormodsrud
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: debug.c,v $
 * Revision 3.1  1995/11/09  09:10:02  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  19:16:41  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 * Revision 1.2  1995/06/07 09:05:04  smackie
 * Fold back Arizona_branch into mainline.
 *
 * Revision 1.1.4.1  1995/05/02  23:22:21  sthormod
 * Branch: Arizona_branch
 * Rom monitor support for RP1 flash credit card
 *
 * Revision 1.1  1995/04/23  00:22:55  sthormod
 * Placeholders for Arizona branch.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Simply way of providing run-time set-able debugging flags without
 * defining global variable that use up our very limited bss/stack
 * space. 
 */


#include "../defs.h"
#include "../stdio.h"
#include "../reason.h"
#include "../extern.h"
#include "nvmonvars.h"
#include "debug.h"

char *debug_flags[NUM_DEBUG_FLAGS] = { 
    DEBUG_BOOT, 
    DEBUG_MONLIB,
    DEBUG_ELF
};

void debug_set (char *arg)
{
    int i, retval;
    char buf[80];

    if (!arg || arg[0] == 0) {
	for (i=0; i<NUM_DEBUG_FLAGS; i++) {
	    if (getvar(nvram->evartbl, debug_flags[i])) {
		printf("\t%s\n", debug_flags[i]);
	    }
	}
    } else {
	for (i=0; i<NUM_DEBUG_FLAGS; i++) {
	    if (strncmp(debug_flags[i], arg, strlen(debug_flags[i])) == 0) {
		arg = (char *)next_char(next_space(arg));
		sprintf(buf,"%s=%s",debug_flags[i], arg);
		retval = setmonvar(buf);
		if (retval < 0)
		    printf("[FAILED]\n");
		else 
		    printf("[OK]\n");
		return;
	    }
	}
	printf("Unknow debug flag [%s]\n", arg);
    } 

}

void debug_unset (char *arg)
{
    int i;
    int retval;
    
    for (i=0; i<NUM_DEBUG_FLAGS; i++) {
	if (strncmp(debug_flags[i], arg, strlen(debug_flags[i])) == 0) {
	    retval = unsetvar(nvram->evartbl, debug_flags[i]);
	    if (retval < 0)
		printf("[FAILED]\n");
	    else 
		printf("[OK]\n");
	    savenv();
	    return;
	}
    }
    printf("Unknow debug flag [%s]\n", arg);
}

boolean debug (char *what)
{
    char *value;

    value = (char *)getvar(nvram->evartbl, what);
    if (value)
	return TRUE;
    else
	return FALSE;
}

char *debug_value (char *what)
{
    char *value;

    value = (char *)getvar(nvram->evartbl, what);
    
    return value;
}

