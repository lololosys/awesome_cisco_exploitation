/* $Id: monlib.h,v 3.2 1995/11/17 18:42:39 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/rommon/src/monlib.h,v $
 *------------------------------------------------------------------
 * monlib.h
 *
 * January 1994, Rob Clevenger
 *
 * Copyright (c) 1994 - 1995 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Structures for the Monitor's interface to monlib.
 *------------------------------------------------------------------
 * $Log: monlib.h,v $
 * Revision 3.2  1995/11/17  18:42:39  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:06:25  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:38:01  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#ifndef __MONLIB_H__
#define __MONLIB_H__

/*
** Structures for the Monitor's interface to monlib.
*/

/*
** This is the Monitor to monlib interface structure
*/

#ifndef ASMINCLUDE
struct monlib_iface {
    char *monlib_vers;            /* pointer to version string */
    struct mon_iface *mon_iface;  /* pointer to mon_iface struct */
    int (*init)();
    int (*open)();
    int (*close)();
    int (*read)();
    int (*write)();
    int (*lseek)();
    int (*fstat)();
    int (*getdents)();
    int (*delete)();
    int (*squeeze)();
    int (*istat)();
    int (*fsstat)();
    int (*settype)();
    int (*reserved)();
    int (*boot)();
    int (*bcopy)();
    int (*getfirst)();
};

/*
** And this is the monlib (or any external program) 
** to Monitor interface structure
**
** Note that this structure is accessed from an assembler routine
** (scrt0.S) and therefore the offset defines must reflect
** the structure.  If you change or add to one, change the other.
*/

struct mon_iface {
    struct monlib_iface *mlifp;       /* pointer to monlib iface struct (0x0) */
    char  *mon_vers;                  /* pointer to version string (0x4) */
    int   (*ra_read)();
    int   (*ra_write)();
    int   (*ra_status)();             /* raw status (0x10) */
    int   (*ra_control)();            /* control function (0x14) */
    int   (*ra_yield)();              /* yield control of processor (0x18) */
    int   (*ra_time)();               /* get current time (0x1c) */
    int   (*ra_reserv1)();            /* reserved (0x20) */
    int   (*ra_reserv2)();            /* reserved (0x24) */
    int   (*ra_reserv3)();            /* reserved (0x28) */
    int   (*ra_getdevidx)();          /* translate device name to index (0x2c) */
    int   (*open)();                  /* the open system call routine (0x30) */
    int   (*close)();                 /* the close system call routine (0x34) */
    int   (*read)();                  /* the read system call routine (0x38) */
    int   (*write)();                 /* the write system call routine (0x3c) */
    int   (*lseek)();                 /* the lseek system call routine (0x40)*/
    int   (*stat)();                  /* the stat system call routine (0x44) */
    int   (*fstat)();                 /* the fstat system call routine (0x48) */
    int   (*getdents)();              /* the getdirent system call routine (0x4c) */
    int   (*printf)();                /* the monitor printf routine (0x50) */
    int   (*sprintf)();               /* the monitor sprintf routine (0x54) */
    void  (*puts)();                  /* the monitor puts routine (0x58) */
    void  (*putchar)();               /* the monitor putchar routine (0x5c) */
    char  (*getchar)();               /* the monitor getchar routine (0x60) */
    int   (*getline)();               /* the monitor getline routine (0x64) */
    void  (*shcmdline)();             /* process a shell command line (0x68) */
    long  (*getfreememstart)();       /* get the start of frree memeory (0x6c) */
    int   (*launch)();                /* launch a process (0x70) */
    int   (*conin)();                 /* non-blocking console input routine */
    int   (*bzero)();                 /* byte zero routine */
    int   (*bcopy)();                 /* byte copy routine */
    int   (*strlen)();                /* string length routine */
    int   (*strcpy)();                /* string copy routine */
    int   (*getnum)();                /* ascii to integer routine */
    int   (*strcmp)();                /* string compare routine */
    int   (*strchr)();                /* string char routine */
    SIG_PF (*signal)();               /* set up the signal for applic. (0x94) */
/* more to come */
};
#endif  /* ASMINCLUDE */


#define MIF_MLIF_PTR         0x0
#define MIF_MON_VERS         0x4
#define MIF_RA_READ          0x8
#define MIF_RA_WRITE         0xc
#define MIF_RA_STATUS        0x10
#define MIF_RA_CONTROL       0x14
#define MIF_RA_YIELD         0x18
#define MIF_RA_TIME          0x1c
#define MIF_RA_GETDEVNUM     0x2c
#define MIF_OPEN             0x30
#define MIF_CLOSE            0x34
#define MIF_READ             0x38
#define MIF_WRITE            0x3c
#define MIF_LSEEK            0x40
#define MIF_STAT             0x44
#define MIF_GETDIRENT        0x48
#define MIF_FSTAT            0x4c
#define MIF_PRINTF           0x50
#define MIF_SPRINTF          0x54
#define MIF_PUTS             0x58
#define MIF_PUTCHAR          0x5c
#define MIF_GETCHAR          0x60
#define MIF_GETLINE          0x64
#define MIF_SHCMDLINE        0x68
#define MIF_GETFREEMEMSTART  0x6c
#define MIF_LAUNCH           0x70
#define MIF_CONIN            0x74
#define MIF_BZERO            0x78
#define MIF_BCOPY            0x7c
#define MIF_STRLEN           0x80
#define MIF_STRCPY           0x84
#define MIF_GETNUM           0x88
#define MIF_STRCMP           0x8c
#define MIF_STRCHR           0x90
#define MIF_SIGNAL           0x94


#ifndef ASMINCLUDE

extern struct monlib_iface ml_iface;
extern struct mon_iface m_iface, *mifp;

/* related prototypes */
extern void init_ml_iface(void);
extern int initfs(int argc, char *argv[]);
#endif  /* ASMINCLUDE */

#endif  /* __MONLIB_H__ */
