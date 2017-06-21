/* $Id: exec_test_sse.h,v 3.2 1995/11/17 09:16:03 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/hes/exec_test_sse.h,v $
 *------------------------------------------------------------------
 * exec_test_sse.h
 *
 * May., 1992 Tony Li
 *
 * Copyright (c) 1992-1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: exec_test_sse.h,v $
 * Revision 3.2  1995/11/17  09:16:03  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:38:38  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  10:51:58  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize os, hes, les, if, and ibm.
 *
 * Revision 2.1  1995/06/07  22:27:35  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * test sse manager
 * test sse memory
 * test sse microcode
 * test sse rewrite
 * test sse sanity
 *
 */

LINK_TRANS(test_sse_extend_here, no_alt);

EOLS	(test_sse_sanity_eol, test_sse, SSE_TEST_SANITY);
KEYWORD (test_sse_sanity, test_sse_sanity_eol, test_sse_extend_here,
	 "sanity", 
	 "Toggle SSE sanity checks", PRIV_OPR); 

EOLS	(test_sse_rewrite_eol, test_sse, SSE_TEST_REWRITE);
KEYWORD (test_sse_rewrite, test_sse_rewrite_eol, test_sse_sanity,
	 "rewrite", 
	 "Compare rewrite memory and shadow rewrite memory", PRIV_OPR); 

EOLS	(test_sse_microcode_eol, test_sse, SSE_TEST_MICROCODE);
KEYWORD (test_sse_microcode, test_sse_microcode_eol, test_sse_rewrite,
	 "microcode", "Toggle SSE microcode", PRIV_OPR);

EOLS	(test_sse_memory_eol, test_sse, SSE_TEST_MEMORY);
KEYWORD (test_sse_memory, test_sse_memory_eol, test_sse_microcode,
	 "memory", "Compare memory and shadow memory", PRIV_OPR);

EOLS	(test_sse_manager_eol, test_sse, SSE_TEST_MANAGER);
KEYWORD (test_sse_manager, test_sse_manager_eol, test_sse_memory,
	 "manager", "Toggle SSE manager process", PRIV_OPR);

KEYWORD (test_sse, test_sse_manager, ALTERNATE,
	"sse", "Silicon Switching Engine", PRIV_OPR);

#undef	ALTERNATE
#define	ALTERNATE	test_sse


