/* $Id: exec_show_memory.h,v 3.4.20.2 1996/08/12 16:05:13 widmer Exp $
 * $Source: /release/112/cvs/Xsys/os/exec_show_memory.h,v $
 *------------------------------------------------------------------
 * E X E C _ S H O W _ M E M O R Y . H
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: exec_show_memory.h,v $
 * Revision 3.4.20.2  1996/08/12  16:05:13  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.4.20.1  1996/04/13  06:05:23  dkatz
 * CSCdi53190:  Init time memory charged to *Dead* process
 * Branch: California_branch
 * Ensure that memory allocated by the ephemeral Init process is charged
 * to *Init*.  Copy the allocator PC from the allocated block into the
 * fragment when fragmenting memory.  Add an option to "show memory" to
 * display the allocating process with each block.
 *
 * Revision 3.4  1996/01/29  07:09:08  hampton
 * Remove command number arguments passed to parse routines that only
 * perform a single function.  [CSCdi47717]
 *
 * Revision 3.3  1995/12/14  13:07:28  widmer
 * CSCdi45510:  Add General_String macro to replace other string macros
 * Change Var_String, String, Txt_Line, and Text to use General_String
 * Add fail argument to Priv_Test
 * Change Neg1 to use Keyword_Id
 * Remove Sense macro
 *
 * Revision 3.2  1995/11/17  18:46:57  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:45:21  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/11/08  21:18:39  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.2  1995/08/06  14:23:45  jacobt
 * CSCdi36607:  need show memory summary command
 *
 * Revision 2.1  1995/07/02  10:51:11  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize os, hes, les, if, and ibm.
 *
 * Revision 2.2  1995/06/18  06:05:29  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase I--add centralized processing for malloc failures.
 *
 * Revision 2.1  1995/06/07  22:25:04  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/******************************************************************
 * show memory { <start_address> [end_addresss] |
 *       {[ processor | io | fast | multibus | pci ] [free | dead | allocating-process]} }
 * 
 * OBJ(int,1) = <start_address>, -1 if no address
 *
 * OBJ(int,2) = <end_address>,  -1 if no address
 *
 * OBJ(int,3) = MEMPOOL_CLASS_xxx (memory type)
 *
 * OBJ(int,4) =	TRUE = The 'free' keyword was parsed.
 *		FALSE = No 'free' keyword.
 *
 * OBJ(int,5) =	TRUE = The 'dead' keyword was parsed.
 *		FALSE = No 'dead' keyword.
 * OBJ(int,6) = TRUE = The 'summary' keyword was parsed.
 *              False = No 'summary' keyword was parsed.
 * OBJ(int,7) = TRUE = The 'allocating-process' keyword was parsed.
 *		FALSE = No 'allocating-process' keyword.
 */

EOLNS    (show_memory_eol, show_memory);
TEST_MULTIPLE_FUNCS(show_memory_test, show_memory_eol, no_alt, NONE);
NUMBER  (show_memory_addr_end, show_memory_test, show_memory_test,
        OBJ(int,2), 0, MAXUINT-1, "Dump memory ending at <address>");
NUMBER  (show_memory_addr_start, show_memory_addr_end, show_memory_test,
        OBJ(int,1), 0, MAXUINT-1, "Dump memory starting at <address>");
PRIV_TEST(show_memory_addr_test, show_memory_addr_start, NONE,
          show_memory_test, PRIV_OPR);
SPLIT   (show_memory_param_test, show_memory_addr_test, show_memory_eol,
	 OBJ(int,3), MEMPOOL_CLASS_UNKNOWN);
KEYWORD_ID (show_memory_dead, show_memory_eol, show_memory_param_test,
	OBJ(int,5), TRUE,
	"dead", "Memory owned by dead processes", PRIV_USER);
KEYWORD_ID (show_memory_alloc, show_memory_eol, show_memory_dead,
 	    OBJ(int,7), TRUE, "allocating-process",
 	    "Show allocating process name", PRIV_USER);
KEYWORD_ID (show_memory_free, show_memory_eol, show_memory_alloc,
	OBJ(int,4), TRUE,
	"free", "Free memory stats", PRIV_USER);
KEYWORD_ID (show_memory_pci, show_memory_free, show_memory_free,
        OBJ(int,3), MEMPOOL_CLASS_PCIMEM,
        "pci", "PCI memory stats", PRIV_USER);
KEYWORD_ID (show_memory_multi, show_memory_pci, show_memory_pci,
        OBJ(int,3), MEMPOOL_CLASS_MULTIBUS,
        "multibus", "Multibus memory stats", PRIV_USER);
KEYWORD_ID (show_memory_sram, show_memory_free, show_memory_multi,
        OBJ(int,3), MEMPOOL_CLASS_FAST,
        "fast", "Fast memory stats", PRIV_USER);
KEYWORD_ID (show_memory_io, show_memory_free, show_memory_sram,
        OBJ(int,3), MEMPOOL_CLASS_IOMEM,
        "io", "IO memory stats", PRIV_USER);
KEYWORD_ID (show_memory_proc, show_memory_free, show_memory_io,
        OBJ(int,3), MEMPOOL_CLASS_LOCAL,
        "processor", "Processor memory stats", PRIV_USER);
KEYWORD_ID (show_memory_summary, show_memory_eol, show_memory_proc,
        OBJ(int,6), TRUE,
        "summary", "Summary of memory usage per alloc PC", PRIV_USER);
SET     (show_memory_set3, show_memory_summary, OBJ(int,3), MEMPOOL_CLASS_UNKNOWN);
SET     (show_memory_set2, show_memory_set3, OBJ(int,2), MAXUINT);
SET     (show_memory_set, show_memory_set2, OBJ(int,1), MAXUINT);

/******************************************************************
 * show memory failures alloc
 */
EOLNS	(show_memory_allocfail_eol, show_memory_allocfail);
KEYWORD (show_memory_allocfail, show_memory_allocfail_eol, no_alt,
	 "alloc", "Memory allocation failures",
	 PRIV_USER|PRIV_USER_HIDDEN);

KEYWORD (show_memory_fail, show_memory_allocfail, show_memory_set, "failures",
	 "Memory failures", PRIV_USER|PRIV_USER_HIDDEN);

KEYWORD (show_command_show_memory, show_memory_fail, ALTERNATE,
         "memory", "Memory statistics", PRIV_USER|PRIV_USER_HIDDEN);

#undef	ALTERNATE
#define	ALTERNATE	show_command_show_memory
