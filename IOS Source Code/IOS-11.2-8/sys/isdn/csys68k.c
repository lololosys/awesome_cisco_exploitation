/* $Id: csys68k.c,v 3.2 1995/11/17 17:40:19 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/isdn/csys68k.c,v $
 *------------------------------------------------------------------
 * csys68k.c  MC 68000 C Library
 *
 * March 1992, Ronnie Kon
 *
 * Copyright (c) 1992-1994 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: csys68k.c,v $
 * Revision 3.2  1995/11/17  17:40:19  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:13:11  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:14:49  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
/*
.Vt,08000 CSYS68K.C 16-Oct-91,15:24:26,`ELO' initial
.I 342
*/
/*
*    MC 68000 C Library - Copyright 1985 to 1987 Microtec Research, Inc.
*    This program is the property of Microtec Research, Inc,
*/

/*      @(#)csys68k.c   1.21.1.1 11/20/90       */

/* This library assumes that the following UNIX style system routines exist:

char *sbrk (size) int size;
    This should increment the heap pointer by size bytes
    and return the old heap pointer.  If there is not enough space on the
    heap, it should return -1.

int open(filename,mode) char *filename; int mode;
    Opens the specified file, depending on the indicated mode,  the modes are
    0   => open for reading
    1   => open for writing
    2   => open for reading & writing (not used)
    1|_IO_EOF   => open for writing and seek to eof
    open should return a number between 0 and 19 that may be associated with
    an element in the iob array.  On failure it should return -1.

int creat(filename,prot) char *filename; int prot;
    Creates and opens (for writing) a file named filename with protection
    as specified by prot.  For things like terminals that cannot be created
    it should just open the device.  It should return a number between 0-19
    that may be associated with an iob array element.  On failure it should
    return -1.

int read(fno,buf,nbyte) int fno, nbyte; char *buf;
    Reads at most nbyte bytes into buf from the file connected to fno (where
    fno is one of the file descriptors returned by open() or creat()). It should
    return the number of bytes read (a value of 0 is used to indicate EOF),
    or a -1 on error.

int write(fno,buf,nbyte) int fno, nbyte; char *buf;
    Writes at most nbyte bytes into the file connected to fno (where fno is
    one of the file descriptors returned by open() or creat()) into buf.  It
    should return the number of bytes written, or a -1 to indicate an error.

int close(fno) int fno;
    Closes the file associated with the file descriptor fno (returned by open()
    or creat()).  Should return 0 if all goes well or -1 on error.

void exit(val) int val;
    Closes all open files and then calls _exit using "val" as the argument.
    Does not return.

void _exit(code) int code;
    Exits from the program, with a status code specified by code.
    Should not return.
    NOTE:  Code for _exit previously was located in this module.  _exit has
           been relocated to "entry.s".

void abort(void);
    Causes an abnormal termination.  While "abort" normally executes a
    "raise (SIGABRT)" to accomplish this, this implementation simply calls
    "exit ()".  The "C" library routines never actually call "abort",
    however the "C++" class libraries do.

long lseek(fno, offset, whence);
    Positions a file associated with the file descriptor fno to an arbitrary
    byte position specified by offset and returns the new position as a long.
    If whence is 0, the new position is relative to the beginning of the
    file.  If whence is 1, the new positon is relative to the current
    position.  If whence is 2, the new position is relative to the end
    of the file.

These routines should use the variable errno to explain what went wrong when
problems occur (the various error codes are defined in errno.h).

The following is a very trivial version of these calls (except sbrk()) that
assumes the following: 

  1. there is one input device of interest, and characters can be read
        from it by calling _INCHRW() which returns an integer of value 0..255
        or -1 for error.
  2. there is one output device of interest, and characters can be written
        to it by calling _OUTCHR (ch).  Its return will be ignored.
  3. there is a way of entering the program, setting up the heap and stack
        pointers, and then it will call START() which in turn calls the
        user's main() function.

The user still has to provide 4 routines (written in assembly language):
(1) initialization routine, (2) sbrk(), (3) _INCHRW(), and (4) _OUTCHR().
*/

/******************************************************************************
If the user's program performs no i/o using functions defined in <stdio.h>,
the struct "_iob" may be removed from the executable file by
        1.  removing the "#include <stdio.h>" statement in this routine,
        2.  removing the "FILE _iob[FOPEN_MAX];" statement in this routine,
        3.  removing all "fopen" and "setbuf" function calls in this routine,
        4.  handling the error returns from "initcopy ()" in some way other
            than printing a message,
            and
        5.  recompiling this routine and linking it ahead of the libraries.
******************************************************************************/

/* """
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
""" */

#define fputs(str, fp)	printf("\n%s", str)	/* === */

#define eoln 2          /* Used to be 1 in previous versions */

                        /*   eoln is used to indicate the end of line characters
                            for the host system.
                        eoln =  0   carriage return (13) as end of line, ex :
                                    Microtec 68000 simulator
                             =  1   carriage return (13) followed by line feed 
                                    (10) as end of line, ex : MSDOS, debugger
                             =  2   line feed (10) as end of line, ex : UNIX
                       */
                                   

/************************************************************************/
/*      Note:   The following variables, _simulated_input and           */
/*              _simulated_output, are used by inchrw.c and outchr.c    */
/*              to allow reading and writing of data through the        */
/*              debugger.  They should be removed when inchrw.c and     */
/*              outchr.c are rewritten for a particular application.    */      
/*                                                                      */
/*              The variables are initialized to place them in the      */
/*              initialized data section.  The #pragma is then used to  */
/*              place the variables in a unique section called          */
/*              "ioports".                                              */
/************************************************************************/
#pragma options -NIioports
volatile char _simulated_input = 0;     /* used by simulator */
volatile char _simulated_output= 0;     /* used by simulator */ 

FILE _iob[FOPEN_MAX];
int errno;

int  _lastp;            /* variable to be used in 'strtok'              */
long _randx;            /* variable to be used in 'rand'                */
char _ctbuf[26];        /* array used by ctime to return time date string */

#define HEADER struct mem

HEADER {
        HEADER    *next;
        unsigned  size;
        };

HEADER  _membase;
HEADER *_avail;
char    _badlist;

unsigned short  _fp_status, _fp_control;

/* This is a sample of the _START() rotuine which is called by the user's
   initialization routine to start a C program.  It opens stdin, stdout and
   stderr to the console.  Then it calls the user's main() function.

   new added: 
        defines and initializes variables for run-time functions
                'strtok', 'malloc', 'rand'.
        stdin, stdout, and stderr are opened as unbuffered.

   Before entering _START(), the execution environment should already be
   set up, including the heap, the stack, and, optionally, the command line.

   The routine _initcopy is used to initialize RAM when the lnk68k
   INITDATA command is used.  Compile this routine using the additional
   command line option "-D_INITDATA".

   Initcopy is called in this routine to insure that stderr is opened
   when it is called.  The user may adjust the manner in which initcopy
   is called and how error conditions are handled to fit his application.
*/

_START (argc, argv, env)
int argc;
char *argv[], *env[];

{
#if _INITDATA
    int _initcopy ();
    int initReturn;
#endif

    _randx = 1;                         /* initialize for 'rand'  */

    _membase.next = &_membase;          /* initialize for malloc */
    _avail = &_membase;

#if 0	/* === */
    fopen ("/dev/tty", "r");            /* stdin */
    fopen ("/dev/tty", "w");            /* stdout */
    fopen ("/dev/tty", "w");            /* stderr */

                                        /* make unbuffered */
    setbuf (stdin,  NULL);              /* stdin */
    setbuf (stdout, NULL);              /* stdout */
    setbuf (stderr, NULL);              /* stderr */
#endif /* === */

#if _INITDATA
    if (initReturn = _initcopy ())
        {
        fputs ("Data initialization failed - ", stderr);
        if (initReturn == 1)
            fputs ("No starting point\n", stderr);
        else if (initReturn == 2)
            fputs ("Unknown operation\n", stderr);
        else
            fputs ("No end mark\n", stderr);
        exit (1);                       /* fatal error -- abort execution */
        }
#endif

    exit (main (argc, argv, env));
}


static find_free()      /* returns the number of a free _iob structure */
{
    register int i;

    for (i=0; i < FOPEN_MAX; i++) {
        if (!(_iob[i]._flag&(_IOREAD|_IOWRT)))
            return (i);
    }
    errno = EMFILE;
    return (-1);
}

open (ignore, mode)
char *ignore;
int mode;
{
    register int fno;

    if ((fno=find_free())== -1)
        return (-1);
    mode &= 3;
    _iob[fno]._file = fno;
    if (mode != 1)
        _iob[fno]._flag = _IOREAD;
    if (mode != 0)
        _iob[fno]._flag |= _IOWRT;
    return (fno);
}

creat (ignore, prot)
char *ignore;
int prot;
{
    return (open (ignore, 1));
}

close (fno)
int fno;
{
    if (!(_iob[fno]._flag&(_IOREAD|_IOWRT))) {
        errno = EBADF;
        return (-1);
    }

    if (_iob[fno]._flag & _IOWRT) 
        if (fflush (&_iob[fno]) == -1)
            return (-1);

    if (_iob[fno]._flag & _IOMYBUF)
        free (_iob[fno]._base);         /* free system buffer */

    _iob[fno]._flag = 0;
    return (0);
}

read (fno,buf,size)
int fno;
register char *buf;
register int size;
{
    register int cnt= 0, ch;

    if (!(_iob[fno]._flag&_IOREAD)) {
        errno = EBADF;
        return (-1);
    }
    while (--size >= 0) {
        if ((ch = _INCHRW()) == -1) {    /* read in one char */
            errno = EIO;
            return (-1);
        }
/* Add this for file I/O, skip '\r'
        if (eoln == 1 && ch == '\r') continue;  
*/
        if (eoln == 0 && ch == '\r') ch = '\n'; /* replace '\r' by '\n' */
        *(buf++) = ch;
        cnt++;
        if (ch == '\r' || ch == '\n')   /* CR or LF terminates the read */
            break;
    }
    return (cnt);
}

twork_write (fno,buf,size)	/* === */
int fno;
register char *buf;
register int size;
{
    register int cnt= 0;

    if (!(_iob[fno]._flag&_IOWRT)) {
        errno = EBADF;
        return (-1);
    }
    while (--size >= 0) {
        if (eoln == 0 && *buf == '\n') *buf = '\r';  /* replace by '\r' */
        if (eoln == 1 && *buf == '\n') _OUTCHR('\r');/* output a '\r' first */
        _OUTCHR (*(buf++));              /* output one char */
        cnt++;
    }
    return (cnt);
}

void exit (int val)
{
    register int i;

    for (i=0; i < FOPEN_MAX; i++) {
        if (_iob[i]._flag & (_IOREAD|_IOWRT)) {
            if (_iob[i]._flag & _IOMYBUF)
                fclose (&_iob[i]);
            else
                close (i);
        }
    }
    _exit(val);
}

void abort (void)
{
    exit (1);                           /* cause unsuccessful termination */
}

long lseek (int d, long offset, int whence)
        {
        return 0; /* stub */
        }
