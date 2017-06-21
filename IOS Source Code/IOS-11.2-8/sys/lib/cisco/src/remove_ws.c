/* $Id$
 * $Source$
 *------------------------------------------------------------------
 * remove_ws.c -- remove leading and trailing whitespace.
 *
 * June 1985, Kirk Lougheed
 *
 * Copyright (c) 1985-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log$
 *------------------------------------------------------------------
 * $Endlog$
 */

#include <ciscolib.h>

/*
 * remove_ws
 * Remove leading and trailing whitespace
 */

char *
remove_ws (char *s)
{
    char *x;
 
    /* Check for null string */
    if (!s) {
	return(s);
    }
 
    /* Remove leading whitespace */
    while ((*s == ' ') || (*s == '\t')) {
	s++;
    }
 
    /* Move to end of string */
    x = s;
    while (*x) {
	x++;
    }
 
    /* Remove trailing whitespace */
    x--;
    while ((*x == ' ') || (*x == '\t')) {
	*x = '\0';
	x--;
    }
    return(s);
}
