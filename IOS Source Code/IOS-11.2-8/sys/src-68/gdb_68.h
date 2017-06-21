/* $Id: gdb_68.h,v 3.1 1996/03/09 00:56:05 hampton Exp $
 * $Source: /release/111/cvs/Xsys/src-68/gdb_68.h,v $
 *------------------------------------------------------------------
 * gdb_68.h -- M68K specific gdb header file
 *  
 * October 1993, Michael Beesley
 *
 * Copyright (c) 1985-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: gdb_68.h,v $
 * Revision 3.1  1996/03/09  00:56:05  hampton
 * Migrate platform specific files to src-xxx directories.  [CSCdi51128]
 *   Step 1: Create copies of the files in their new location.
 *
 * Revision 3.2  1995/11/17  18:48:05  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:46:01  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:55:25  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * In order to run with the Sun3 GDB, we pretend to have floating point
 * registers.
 */
#define NUMREGBYTES 188
#define NUMREGS (NUMREGBYTES/4)

#define REGBYTES 72		/* The real value */

/*
 * typedef for signal handler
 */
typedef void (*SIG_PF)(void);

/* 
 * Define the structure used to save 68020 exception frames
 */
typedef struct FrameStruct
{
    int       exceptionPC;      /* pc value when this frame created */
    int       exceptionVector;  /* cpu vector causing exception     */
    short     frameSize;        /* size of cpu frame in words       */
    ushort    sr;               /* ps at time of exception	    */
    void      *pc;
    ushort    format;
    ushort    framedata[19];
} Frame;

/*
 * typedef for complete cpu context on MC68020 platforms
 * IE registers and frame
 */
typedef struct cpu_context_ {
    int registers[NUMREGBYTES/4];	/* Saved registers */
    Frame saveFrame;			/* Saved frame */
} cpu_context_t;

/* 
 * Define a macro for breakpoint instruction on MC68020
 */
#define BREAKPOINT() 		\
    asm("trap #1");

enum regnames {D0,D1,D2,D3,D4,D5,D6,D7, 
               A0,A1,A2,A3,A4,A5,A6,A7, 
               PS,PC,
               FP0,FP1,FP2,FP3,FP4,FP5,FP6,FP7,
               FPCONTROL,FPSTATUS,FPIADDR
	       ,FPCODE,FPFLAGS
              };

/*
 * Prototypes
 */
extern void handle_exception(int);

/* End of file */
