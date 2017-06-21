/* $Id: error.c,v 3.2.58.2 1996/03/21 23:27:08 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/rommon/src/error.c,v $
 *------------------------------------------------------------------
 * error.c
 *
 * August 1992, Rob Clevenger
 *
 * Copyright (c) 1994 - 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Message printing routines for diagnostice.
 *------------------------------------------------------------------
 * $Log: error.c,v $
 * Revision 3.2.58.2  1996/03/21  23:27:08  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.2.58.1  1996/03/18  21:46:21  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.2.1  1995/12/01  04:20:15  mbeesley
 * Branch: ELC_branch
 * Reorganize Rom monitor header files to make it easier to port
 * to new platforms.
 *
 * Revision 3.2  1995/11/17  18:41:42  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:05:19  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:37:11  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/*
** Routines used to print messages during diagnostics.  Use testname()
** at the beginning of a diagnostic routine to set the testname for the
** other message routines.  Use prpass() to print out pass or phase
** messages.  Use prcomplete() at the end of a diagnostic.  If any
** errors are discovered in the diagnostic use cterr() to display the
** error message.
*/
#include "monitor.h"
#include <stdarg.h>
#include "error.h"
#include "mon_defs.h"

/*
** External variables
*/

#define CBUFSIZ 400

unsigned long testpass, errcount, err_accum;
char testnamebuf[TESTNAMEBUFSIZ];
static char *errlogptr;      /* pointer to the end of the error log */


void
testname(char *string, ...)
{
    char *bptr, buffer[CBUFSIZ/2];
    va_list ap;

    if((envflag & INDIAG) &&
       (DIAGFLAG & D_NESTED)) return;  /* this is a nested diag */
  
    va_start(ap, string);
    sprintf(testnamebuf,string,va_arg(ap, int),va_arg(ap, int));
    va_end(ap);
    if(!(envflag & INDIAG)) return;  /* only print testname in diags */
    if(DIAGFLAG & D_QUIETMODE) return;  /* unless in quiet mode */
    bptr = buffer;
    if(DIAGFLAG & D_CONTINUOUS && testpass)
	bptr += sprintf(bptr, "pass %d, ", testpass);
    sprintf(bptr, "%s test ",testnamebuf);
    clrline(buffer);
}

void
cterr(char errtype, int errnum, char *errstr, ...)
{
    char *bptr;
    char buffer[CBUFSIZ];
    int t_arg1, t_arg2, t_arg3, t_arg4, t_arg5, t_arg6;
    va_list ap;

    errcount++;
    if((envflag & INDIAG) && (DIAGFLAG & D_QUIETMODE)) return;

    va_start(ap, errstr);
    bptr = buffer;
    bptr += sprintf(bptr,"\n*** ");
    switch(errtype) {
    case 'f':
	bptr += sprintf(bptr,"Fatal error");
	break;
    case 'w':
	bptr += sprintf(bptr,"Warning");
	break;
    default:
	bptr += sprintf(bptr,"??? error");
	break;
    }
    bptr += sprintf(bptr," during %s test,",testnamebuf);
    if(testpass) bptr += sprintf(bptr, "  pass %d,", testpass);
    *bptr++ = '\n';
    /*
    ** FIXME - nested varargs!
    ** Do this cuz we seem to be having problems with nesting
    ** this vararg stuff.
    */
    t_arg1 = va_arg(ap, int);
    t_arg2 = va_arg(ap, int);
    t_arg3 = va_arg(ap, int);
    t_arg4 = va_arg(ap, int);
    t_arg5 = va_arg(ap, int);
    t_arg6 = va_arg(ap, int);
    bptr += sprintf(bptr, errstr, t_arg1, t_arg2, t_arg3, t_arg4,
		    t_arg5, t_arg6);
    va_end(ap);
    *bptr++ = '\n';
    *bptr++ = '\0';
    logprintf(buffer);
    if(!(DIAGFLAG & D_QUIETMODE)) bell(5);
    if(stoponerr()) longjmp(monjmpptr, 1);
}

int
stoponerr(void)
{
    if(DIAGFLAG & D_STOPONERR && monjmpptr) {
	puts("test stopped on error\n");
	return(1);
    } else return(0);
}

/*
** Put the error number in the LEDs.
*/
void
errleds(int errnum)
{
#ifndef __mips
    asm("movl sp@(8),d0");       /* get error number */
    asm("jmp lights");
#else
    asm(".set noreorder");
    asm("     j    lights");     /* Parameter is in A0 allready */
    asm("     nop");
    asm(".set reorder");
#endif
}

/*
** Put the mesage on the current line wiping out what
** was previously there.  Includes code to handle a
** string with newline(s).
** The console output routine keeps track of the line
** position (linepos).
*/
extern short linepos;

void
clrline(char *string)
{
    int i = linepos;  /* get line position */
    int tmp;
    char *cptr;
    extern char *strchr();

    cptr = strchr(string, '\n');  /* does the string have a newline? */
    if(cptr) *cptr = '\0';  /* replace the first one with NULL */
    putchar('\r');  /* this will set linepos to 0 */
    i -= printf(string);
    if(i > 0) {  /* last line was longer - wipe the rest of it */
	tmp = i;  /* remember this location */
	while(i--) putchar(' '); /* clear rest of line */
	i = tmp;
	while(i--) putchar('\b');  /* back the cursor up */
	linepos -= tmp;  /* correct for above spaces */
    }
    if(cptr) {
	*cptr = '\n';  /* replace newline */
	puts(cptr);    /* finish the string */
    }
}

void
prpass(int pass, char *msg, ...)
{
    char *bptr, buffer[CBUFSIZ/2];
    int t_arg1, t_arg2;
    va_list ap;

    if(!(envflag & INDIAG) ||
       DIAGFLAG & (D_QUIETMODE | D_NESTED)) return;
    va_start(ap, msg);
    bptr = buffer;
    if(pass) bptr += sprintf(bptr, "pass %d, ", pass);
    /*
    ** FIXME - nested varargs
    */
    t_arg1 = va_arg(ap, int);
    t_arg2 = va_arg(ap, int);
    if(msg) bptr += sprintf(bptr, msg, t_arg1, t_arg2);
    va_end(ap);
    sprintf(bptr, " %s test ", testnamebuf);
    clrline(buffer);
}

void
prcomplete(int pass, int errcount, char *msg, ...)
{
    char *bptr, buffer[CBUFSIZ/2];
    va_list ap;

    if(!(envflag & INDIAG) ||
       DIAGFLAG & (D_QUIETMODE | D_NESTED)) return;
    va_start(ap, msg);
    bptr = buffer;
    bptr += sprintf(bptr, "%s test completed, ", testnamebuf);
    if(pass) bptr += sprintf(bptr, "%d passes, ", pass);
    bptr += sprintf(bptr, "... %d errors", errcount);
    if(msg) {
	*bptr++ = ',';
	*bptr++ = ' ';
	sprintf(bptr, msg, va_arg(ap, int), va_arg(ap, int));
    }
    va_end(ap);
    clrline(buffer);
    putchar('\n');  /* do not give newlines to clrline() */
}

/*
 * Beep the terminal 'n' times.
 */
void
bell(int n)
{
    volatile int i;
     volatile int *j;

    while(n) {
	putchar('\007');
	for( i=0; i<0x1000; i++ ) j=&i;
	n--;
    }
}

void
logprintf(char *buffer)
{
    int size;

    size = strlen(buffer);
    puts(buffer);  /* display the message */
    if( (size + errlogptr) < (errlog_start + ERRLOG_SIZE) ) {
	/* there is enough space in the error log buffer */
	movbyte(buffer, errlogptr, size); /* log the error */
	errlogptr += size;
    } else {
	puts("(unable to log item - log buffer full)\n");
    }
}

int
dumperrlog(void)
{
    char *errlogp = errlogptr;
    char *ptr = errlog_start;
    extern short linecnt;

    if(errlogp <= ptr) {
	puts("No errors in errlog\n");
	return(0);
    }
    /* setmore() ??? */
    puts("\nError log:\n");
    while(ptr < errlogp)
	putchar(*ptr++);
    return(0);
}

void
clrerrlog(void)
{
    errlogptr = errlog_start;
}
