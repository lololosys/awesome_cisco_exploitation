/* $Id: exec_ctrace.h,v 3.1.40.1 1996/08/12 16:08:12 widmer Exp $
 * $Source: /release/112/cvs/Xsys/testtools/exec_ctrace.h,v $
 *------------------------------------------------------------------
 * Parse chain for ctrace command
 *
 * January 1996, Lakshmankumar Mukkavilli
 *
 * Copyright (c) 1996-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: exec_ctrace.h,v $
 * Revision 3.1.40.1  1996/08/12  16:08:12  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.1  1996/01/31  18:24:16  lakshman
 * CSCdi47664:  Augment cflow with tracing capability
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/***************************************************************
 *
 * ctrace {on [nowrap] | off | status | fastcopy |
 *                  copy [{ <ipaddr> | <hostname> } [<filename>]}]
 *                               
 */

/* ctrace copy [{ <ipaddr> | <hostname> } [<filename>]] */
EOLNS (exec_ctrace_eol, ctrace_writetofile);
TEXT (exec_ctrace_filename, exec_ctrace_eol, exec_ctrace_eol,
      OBJ(string,1), NULL/*"Filename to contain trace info."*/);
IPADDR_NAME (exec_ctrace_host, exec_ctrace_filename, exec_ctrace_eol,
	     OBJ(paddr,1),
             NULL/*"Name or address of host to write trace info."*/); 
KEYWORD (exec_ctrace_copy, exec_ctrace_host, no_alt,
	 "copy", NULL/*"Copy ctrace info. to a file"*/,
         PRIV_ROOT | PRIV_HIDDEN);

/* ctrace fastcopy */
EOLNS (exec_ctrace_fastcopy_eol, ctrace_fastcopy);
KEYWORD (exec_ctrace_fastcopy, exec_ctrace_fastcopy_eol, exec_ctrace_copy,
	 "fastcopy",
         NULL/*"Copy ctrace info. using default hostaddr and filename"*/,
         PRIV_ROOT | PRIV_HIDDEN);

/* ctrace status */
EOLNS (exec_ctrace_status_eol, ctrace_display_status);
KEYWORD (exec_ctrace_status, exec_ctrace_status_eol, exec_ctrace_fastcopy,
	 "status", NULL/*"Display the status"*/, PRIV_ROOT | PRIV_HIDDEN);

/* ctrace off */
EOLNS (exec_ctrace_off_eol, ctrace_disable);
KEYWORD (exec_ctrace_off, exec_ctrace_off_eol, exec_ctrace_status,
	 "off", NULL/*"Disable function tracing"*/, PRIV_ROOT | PRIV_HIDDEN);

/* ctrace on [nowrap] */
EOLNS (exec_ctrace_on_eol, ctrace_enable);
EOLNS (exec_ctrace_on_nowrap_eol, ctrace_enable_nowrap);
KEYWORD (exec_ctrace_on_nowrap, exec_ctrace_on_nowrap_eol, exec_ctrace_on_eol,
	 "nowrap", NULL/*"Do not wraparound the trace buffer"*/,
	 PRIV_ROOT | PRIV_HIDDEN);
KEYWORD (exec_ctrace_on, exec_ctrace_on_nowrap, exec_ctrace_off,
	 "on", NULL/*"Enable function tracing"*/, PRIV_ROOT | PRIV_HIDDEN);

KEYWORD (exec_ctrace, exec_ctrace_on, ALTERNATE,
	 "ctrace", NULL/*"Function trace operation"*/,
	 PRIV_ROOT | PRIV_HIDDEN);

#undef	ALTERNATE
#define	ALTERNATE exec_ctrace
