/* $Id: cflow.h,v 3.2 1996/02/14 05:49:34 lakshman Exp $
 * $Source: /release/111/cvs/Xsys/testtools/cflow.h,v $
 *------------------------------------------------------------------
 * Header file for controlflow coverage programs
 *
 * January 1996, Lakshmankumar Mukkavilli
 *
 * Copyright (c) 1996-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cflow.h,v $
 * Revision 3.2  1996/02/14  05:49:34  lakshman
 * CSCdi47664:  Augment cflow with tracing capability
 *
 * Revision 3.1  1996/01/31  18:24:00  lakshman
 * CSCdi47664:  Augment cflow with tracing capability
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/* This file used to be called cflow_bb.h */

/*
 * prevent multiple inclusions
 */
#ifndef __CFLOW_H__
#define __CFLOW_H__

/*
 * bb is extracted from libgcc2.c (part of the GCC compiler distribution)
 * except for the comments alongside elements of the structure.
 * This structure needs to be identical to one used by the compiler.
 * Any changes to the bb structure in the compiler  should be
 * reflected here. Cygnus Support is responsible for the maintenance
 * of the compiler. Every object file that is created using controlflow
 * coverage options contains a copy of bb structure. bb contains
 * information about basic blocks (hence the name bb) in an object
 * module. bb also contains a pointer to a table containing
 * instrumented arcs(that is what we are interested in).
 * cflow's use of bb structure is a little different from the way
 * it was intended.
 * In order to implement tracing feature we need to have a function
 * call _bb_func_init every time (not just first time). So we leave
 * zero_word as zero and use line_nums for the purpose of maintaining
 * the list of bb structures.
 */
struct bb {
  long            zero_word;	/* This is used to ensure that
				 * bb_init_func is only called once
				 * per module (file).  */
                                /* cflow leaves this field to be zero always,
                                   because we want bb_init_func to be called
                                   everytime(not just once). */
  
  const char     *filename;	/* The name of the output file where
				 * the bb execution counts are
				 * written.  */
  
  long           *counts;	/* The address of a table containing the bb
				 * execution counts, one entry per
				 * instrumented arc */
  
  long            ncounts;    /* The number of table entries.  */
  
  struct bb      *next;	/* Ptr to next bb struct. Initialised to zero
			 * by the compiler */
  
  const unsigned long *addresses;	/* The address of a table containing
					 * the addresses of the bb's in the
					 * code.  Note that these addresses
					 * may not be aligned if the host has
					 * variable length instructions. */
  
  
  /* Older GCC's did not emit these fields. Not used for coverage  */
  long            nwords;        /* Length of this structure in bytes.
                                    Currently this field is initialized
                                    by the compiler but never used. */
  const char    **functions;
  const long     *line_nums;     /* cflow uses this field as an alternative
                                    to zero_word.
                                    Initially this field is zero and when
                                    a function in .c file is invoked
                                    for the first time this is set to one.*/
  const char    **filenames;
};

extern struct bb *bb_list_head;

#define MAX_FILENAME_LENGTH 100
#define MAX_FILENAME_SUFFIX_LENGTH 20
extern char ctrace_default_filename[];
extern ipaddrtype cflow_tftphost;

void cflow_parser_init(void);
void ctrace_parser_init(void);
void cflow_get_tftphost_and_imagenames(void);
/*
 * ctrace_write_entry, ctrace_start, ctrace_start_nowrap, ctrace_stop
 * ctrace_dont_wraparound and ctrace_copyfast are called by other
 * routines that want to use ctrace service.
 */
boolean ctrace_write_entry(uchar *caller, uchar *callers_caller);
void ctrace_start(void);
void ctrace_start_nowrap(void);
void ctrace_stop(void);
void ctrace_dont_wraparound(void);
void ctrace_fastcopy(void);
/*
 * cflow_save, cflow_clear, cflow_killsaved, cflow_fastcopy and 
 * cflow_fastcopy_saved are called by other routines that want 
 * to use cflow service.
 */
void cflow_save(void);
void cflow_clear(void);
void cflow_killsaved(void);
void cflow_fastcopy(void);
void cflow_fastcopy_saved(void);

/*
 * Use CTRACE_BUFFER_ENTRY to make an entry in trace buffer with
 * with the PC and the callers PC.
 * Use SAVE_CALLER() before CTRACE_BUFFER_ENTRY and any non-inline
 * function call.
 */
#define CTRACE_BUFFER_ENTRY ctrace_write_entry((uchar *)current_pc(),(uchar *)caller());

#endif                          /* #ifndef __CFLOW_H__ */
