/* $Id: gdb_sp.h,v 3.1 1996/03/09 00:56:11 hampton Exp $
 * $Source: /release/111/cvs/Xsys/unix/gdb_sp.h,v $
 *------------------------------------------------------------------
 * gdb_sp.h -- SPARC specific gdb header file
 *  
 * October 1993, Michael Beesley
 *
 * Copyright (c) 1985-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: gdb_sp.h,v $
 * Revision 3.1  1996/03/09  00:56:11  hampton
 * Migrate platform specific files to src-xxx directories.  [CSCdi51128]
 *   Step 1: Create copies of the files in their new location.
 *
 * Revision 3.2  1995/11/17  18:48:11  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:46:05  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:55:30  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * This is just a copy of the MC68020 context. It needs to
 * be completed correctly if gdb debugging is ever needed on SPARC's
 */

#define NUMREGBYTES 188
#define NUMREGS (NUMREGBYTES/4)

#define REGBYTES 72		/* The real value */

/*
 * typedef for signal handler
 */
typedef void (*SIG_PF)(void);

/*
 * Define the structure used to save SPARC frames
 */
typedef struct FrameStruct
{
    int       exceptionVector;  /* cpu vector causing exception     */
    short     frameSize;        /* size of cpu frame in words       */
} Frame;

/*
 * typedef for complete cpu context on SPARC platforms
 * IE registers and frame
 */
typedef struct cpu_context_ {
    int registers[NUMREGBYTES/4];	/* Saved registers */
    Frame saveframe;			/* Saved frame */
} cpu_context_t;

/* 
 * Define a macro for breakpoint instruction on SPARC
 */
#define BREAKPOINT()

/* End of file */
