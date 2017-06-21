/************************************************************************
 *                                                                      *
 *                              NOTE WELL                               *
 * This is vendor-supplied and vendor-supported code.  Do not make any  *
 * modifications to this code without the knowledge and consent of the  *
 * SNMP agent group.                                                    *
 *                                                                      *
 ************************************************************************/
/* $Id: sr_debug.c,v 3.2.62.1 1996/03/18 21:48:26 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/snmp/sr_debug.c,v $
 *------------------------------------------------------------------
 * SNMP/SNMPv2 bi-lingual agent code.
 *
 * March 1994, SNMP Research
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: sr_debug.c,v $
 * Revision 3.2.62.1  1996/03/18  21:48:26  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.2  1996/03/07  12:33:34  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.2.26.1  1996/02/20  17:14:09  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.2  1995/11/17  18:59:06  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:16:42  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:48:55  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
/*
 *
 * Copyright (C) 1994 by SNMP Research, Incorporated.
 *
 * This software is furnished under a license and may be used and copied
 * only in accordance with the terms of such license and with the
 * inclusion of the above copyright notice. This software or any other
 * copies thereof may not be provided or otherwise made available to any
 * other person. No title to and ownership of the software is hereby
 * transferred.
 *
 * The information in this software is subject to change without notice
 * and should not be construed as a commitment by SNMP Research, Incorporated.
 *
 * Restricted Rights Legend:
 *  Use, duplication, or disclosure by the Government is subject to
 *  restrictions as set forth in subparagraph (c)(1)(ii) of the Rights
 *  in Technical Data and Computer Software clause at DFARS 52.227-7013
 *  and in similar clauses in the FAR and NASA FAR Supplement.
 *
 */

/*
 *                PROPRIETARY NOTICE
 *
 * This software is an unpublished work subject to a confidentiality agreement
 * and is protected by copyright and trade secret law.  Unauthorized copying,
 * redistribution or other use of this work is prohibited.
 *
 * The above notice of copyright on this source code product does not indicate
 * any actual or intended publication of such source code.
 *
 */

#if (! ( defined(lint) ) && defined(SR_RCSID))
static char     rcsid[] = "debug.c,v 1.18 1995/02/17 21:28:31 spakes Exp";
#endif				/* (! ( defined(lint) ) && defined(SR_RCSID)) */

/*
 * Revision History:
 *
 * debug.c,v
 * Revision 1.18  1995/02/17  21:28:31  spakes
 * Added [GS]etLogLevel() functions for binary customers.
 *
 * Revision 1.17  1995/02/06  18:11:08  battle
 * Changes for vrtx.
 *
 * Revision 1.16  1994/10/28  16:37:25  vaughn
 * Placed new copyright in the file
 *
 * Revision 1.15  1994/10/26  21:51:32  vaughn
 * Changed copyrights to the new copyright structure
 *
 * Revision 1.14  1994/09/19  13:39:03  levi
 * Removed #include "snmpuser.h" from files that don't need it.
 *
 * Revision 1.13  1994/08/04  15:37:22  battle
 * pSOS 2.0 changes
 *
 * Revision 1.12  1994/04/18  18:28:24  battle
 * pSOS changes
 *
 * Revision 1.11  1994/03/03  15:39:09  vaughn
 * Changed 0's in DPRINTF to APALWAYS.
 *
 * Revision 1.10  1994/02/15  19:57:40  kallen
 * Getting copyright notices where they belong KIA
 *
 * Revision 1.9  1994/02/14  16:38:56  pearson
 * Changed copyright notice to be current for 1994, and added an
 * #ifdef SR_RCSID
 * around the static  declarations of rcsid in each file.
 *
 * AMP
 *
 * Revision 1.8  1994/01/13  15:51:47  battle
 * Switch DYNIX to use stdarg.h instead of varargs.h.
 *
 * Revision 1.7  1994/01/13  15:32:19  battle
 * Changes for Dynix/ptx.
 *
 * Revision 1.6  1994/01/05  17:04:42  reid
 * Modified MS Windows DPRINTF support
 *
 * Revision 1.3  1994/01/05  15:59:28  reid
 * Added DPRINTF support for MS Windows (SR_WINDOWS)
 *
 * Revision 1.2  1993/12/23  16:53:53  battle
 * Added cvs headers to all .c files.
 *
 *
 * 20-Sep-1993 DBL
 *   Removed from prnt_lib.c to allow dprintf to be overridden.
 */


#include "master.h"
#include <ctype.h>
#include <stdarg.h>


#define WANT_ALL_ERRORS

#include "sr_snmp.h"
#include "sr_diag.h"

#ifdef DEBUG


/*
 *    dprintf is a wrapper around an output function ( here fprintf() )
 *    which decides whether or not to output based upon the value of the
 *    external LogLevel and the passed parameter log_level.  In this
 *    example, the test is a simple less-than.  LogLevel could be
 *    treated as a bit field and log_level could be an and-ing of various
 *    bits.
 *
 *    As well, the output function could be something other than a
 *    print function.  As befits a daemon in UNIX, it could easily be syslog()
 *    The calling convention is dprintf( log_level, format_string, argv);
 *
 *    It is necessary for this routine to grab the format string to pass to
 *    the next function.  The format string in printf is used to count the
 *    remaining varargs.  Thus it is not a vararg itself and should be
 *    explicitly gotten and passed on.
 *
 *    It is intended that dprintf() be used in conjunction with the DPRINTF
 *    macro.  DPRINTF should be defined in the following fashion:
 *
 *    #ifdef DEBUG
 *    #define DPRINTF( X ) dprintf X
 *    #else
 *    #define DPRINTF( X )
 *    #endif
 *
 *    As well, the main routine should declare an external variable LogLevel
 *    and set it to a value related to the amount of debugging noise needed.
 *
 *    And the call would be:    DPRINTF(( log_level, format_str, argv ));
 *
 *    This allows DPRINTF to be defined as nothing so that in a system
 *    which is pressed for text space it can be totally eliminated.
 *
 */


int             LogLevel = APERROR | APWARN;

static          previous_log_level;
static char    *previous_format;
static char     progname[80];


/*
 * SetLogLevel:
 *
 * This function is an interface to the LogLevel variable to allow
 * developers without source code to change the value inside the
 * method routines.
 */

int
SetLogLevel(log_level)
    int             log_level;
{
    int             previous_log_level;

    previous_log_level = LogLevel;
    LogLevel = log_level;
    return (previous_log_level);
}


/*
 * GetLogLevel:
 *
 * This function is an interface to the LogLevel variable to allow
 * developers without source code to obtain the value inside the
 * method routines.
 */

int
GetLogLevel()
{
    return (LogLevel);
}


#define strrchr(a,b) NULL

/*
 * SetProgName:
 *
 *   The name of this routine (pre-snmp12.1.0.0 release) was set_progname().
 */
void
SetProgname(s)
    char           *s;
{
    char           *p, *tmp;

    p = s;
    if ((tmp = strrchr(s, '/')) != NULL) {
	p = tmp + 1;
    }
    strcpy(progname, p);
}

/*
 * dprintf:
 */
int
dprintf(int log_level, const char *format, ...)
{
    va_list         arg;

    va_start(arg, format);

    previous_log_level = log_level;	/* for dlfprint below */
    /*
     * This line hinders portability to other printf() like functions. Alter
     * it if necessary for other functions interfaces.
     */
    previous_format = (char *) format;


    /* DPRINTF calls which pass 0 as the first arg are always printed */
    if (log_level == 0 || (log_level & LogLevel) != 0) {
	if (progname[0] != '\0') {
	    fprintf(stderr, "%s: ", progname);
	}
	_doprnt(NULL, stderr, format, arg, FALSE);
    }

    va_end(arg);

    return 0;
}				/* dprintf() */

void
dlfprint(line, file)
    int             line;
    char           *file;
{
    extern          LogLevel;

    if (previous_format != NULL && (strlen(previous_format) > (unsigned) 0)) {
	if (previous_format[strlen(previous_format) - 1] != '\n')
	    return;
    }
    if (previous_log_level == 0 ||
	(previous_log_level & LogLevel) != 0) {
	(void) fprintf(stderr, "                              at line %d in file %s\n",
		       line, file);
    }
}				/* dlfprint() */

#endif /* DEBUG */
