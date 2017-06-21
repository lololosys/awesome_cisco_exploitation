/* $Id: getopt.c,v 3.2.58.1 1996/03/21 23:27:16 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/rommon/src/getopt.c,v $
 *------------------------------------------------------------------
 * getopt.c
 *
 * September 1992, Rob Clevenger
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Convenient routine for parsing argc,argv options.
 *------------------------------------------------------------------
 * $Log: getopt.c,v $
 * Revision 3.2.58.1  1996/03/21  23:27:16  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.2.2.1  1995/12/01  04:20:22  mbeesley
 * Branch: ELC_branch
 * Reorganize Rom monitor header files to make it easier to port
 * to new platforms.
 *
 * Revision 3.2  1995/11/17  18:42:05  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:05:41  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:37:31  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "monitor.h"
#include "proto.h"
#include "sh.h"
#include "strings.h"
#include "extern.h"

#define EOF (-1)


int optind;  /* index into argv array (argv[optind]) */
char *optarg;  /* pointer to an option argument if any */
static int asi;  /* index into argv[] string (argv[optind][asi]) */

int
getopt(int argc, char *argv[], char *optstr)
     /* optstr : string of acceptible options */
{
    int option;
    char *cp;

    if(optind == 0) {  /* beginning of new command */
	asi = 1;
	optind++;
    }
    if(asi == 1) {  /* beginning of new argv item */ 
	if(optind >= argc ||  /* out of arguments */
	   argv[optind][0] != '-' ||  /* options must start with '-' */
	   argv[optind][1] == '\0')  /* '-' by itself is not an option */
	    return(EOF);
	else if(strcmp(argv[optind], "--") == 0) {
	    optind++;  /* "--" means end of option list, skip this arg */
	    return(EOF);
	}
    }
    option = argv[optind][asi];  /* get the option */
    if(option == ':' || (cp=strchr(optstr,option)) == 0) {  /* is it valid? */
	printf("%s: illegal option -- %c\n",argv[0],option);
	if(argv[optind][++asi] == '\0') {  /* more options in this argv? */
	    optind++;  /* no, bump to next argv */
	    asi = 1;
	}
	return('?');  /* '?' means illegal option condition */
    }
    if(*++cp == ':') {  /* the option requires an argument */
	if(argv[optind][asi+1] != '\0')  /* the option arg is in this argv */
	    optarg = &argv[optind++][asi+1];  /* set global pointer */
	else if(++optind >= argc) {
	    /* optarg is not in this argv and there are no more argv's */
	    printf("%s: option requires an argument -- %c\n",argv[0],option);
	    return('?');  /* '?' means illegal option condition */
	} else optarg = argv[optind++];  /* option argument is next argv */
	asi = 1;  /* set to beginning of new argv */
    } else {  /* no option argument */
	optarg = (char *)0;  /* NULL pointer */
	if(argv[optind][++asi] == '\0') {  /* more options in this argv? */
	    optind++;  /* no, bump to next argv */
	    asi = 1;
	}
    }
    return(option);
}

/*
** Shift the arguments in an argv[] array to replace 0 with 1, 1 with 2,
** etc.
** This code relies upon the fact that argv is the first item in the
** shstuff structure.
*/
int
shift(int count, char *argv[])
{
    register i;
    struct shstuff *shp = (struct shstuff *)argv;  /* kinda kludgy */
    
    if(count >= shp->argc) {  /* count is >= number of arguments */
	shp->argc = optind = 0;
	argv[0] = (char *)0;
    } else {  /* count is less than the number of arguments */
	shp->argc -= count;
	optind -= count;
	for(i=0; i <= shp->argc; argv[i++] = argv[count++]);
    }
    return(shp->argc);
}
