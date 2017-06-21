/* $Id: exec_cflow.h,v 3.1.40.1 1996/08/12 16:08:11 widmer Exp $
 * $Source: /release/112/cvs/Xsys/testtools/exec_cflow.h,v $
 *------------------------------------------------------------------
 * Parse chain for cflow commands
 * 
 * June-9 1994, Lakshmankumar Mukkavilli
 * 
 * Copyright (c) 1993-1997 by cisco Systems, Inc. All rights reserved.
 * 
 * ------------------------------------------------------------------
 * $Log: exec_cflow.h,v $
 * Revision 3.1.40.1  1996/08/12  16:08:11  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.1  1996/01/31  18:24:14  lakshman
 * CSCdi47664:  Augment cflow with tracing capability
 *
 * ------------------------------------------------------------------
 * $Endlog$
 */

/***************************************************************
 *
 * cflow {save| clear | killsaved | listfiles [all] | status |
 *                  fastcopy [saved] |
 *                  copy [{ <ipaddr> | <hostname> } [<filename>]}] |
 *                  movesaved [{ <ipaddr> | <hostname> } [<filename>]}] |
 *                  [ <ipaddr> [<filename>] ] 
 *                               
 * (*OBJ(paddr,1)) = <ipaddr> or <hostname> coverted to an <ipaddr>
 * OBJ(string,1) = <filename>
 */

/***************************************************************
 * cflow  [ <ipaddr> [<filename>] ] 
 * This format is deprecated but retained for compatibility.
 */
EOLNS	(cflow_old_eol, cflow_writetofile);
TEXT	(cflow_old_file, cflow_old_eol, cflow_old_eol,
	OBJ(string,1), NULL/*"File name into which to write coverage data"*/);
IPADDR (cflow_old_host, cflow_old_file, cflow_old_eol,
	OBJ(paddr,1), NULL/*"Address of host to write coverage data onto"*/);

/* cflow movesaved [{ <ipaddr> | <hostname> } [<filename>]] */
EOLNS (exec_cflow_movesaved_eol, cflow_writetofile_saved);
TEXT (exec_cflow_movesaved_filename, exec_cflow_movesaved_eol,
      exec_cflow_movesaved_eol,
      OBJ(string,1), NULL/*"Filename to contain cflow info."*/);
IPADDR_NAME (exec_cflow_movesaved_host, exec_cflow_movesaved_filename,
             exec_cflow_movesaved_eol, OBJ(paddr,1),
             NULL/*"Name or address of host to write cflow info."*/); 
KEYWORD (exec_cflow_movesaved, exec_cflow_movesaved_host, cflow_old_host,
	 "movesaved", NULL/*"Move saved cflow data to a file"*/,
         PRIV_ROOT | PRIV_HIDDEN);

/* cflow copy [{ <ipaddr> | <hostname> } [<filename>]] */
EOLNS (exec_cflow_copy_eol, cflow_copy);
TEXT (exec_cflow_copy_filename, exec_cflow_copy_eol, exec_cflow_copy_eol,
      OBJ(string,1), NULL/*"Filename to contain trace info."*/);
IPADDR_NAME (exec_cflow_copy_host, exec_cflow_copy_filename,
             exec_cflow_copy_eol,OBJ(paddr,1),
             NULL/*"Name or address of host to write cflow info."*/); 
KEYWORD (exec_cflow_copy, exec_cflow_copy_host, exec_cflow_movesaved,
	 "copy", NULL/*"Copy cflow data to a file"*/,
         PRIV_ROOT | PRIV_HIDDEN);

/* cflow fastcopy [saved] */
EOLNS (exec_cflow_fastcopy_saved_eol, cflow_fastcopy_saved);
EOLNS (exec_cflow_fastcopy_eol, cflow_fastcopy);
KEYWORD (exec_cflow_fastcopy_saved, exec_cflow_fastcopy_saved_eol,
         exec_cflow_fastcopy_eol, "saved",
         NULL/*"Copy saved cflow data using default hostaddr and filename"*/,
         PRIV_ROOT | PRIV_HIDDEN);
KEYWORD (exec_cflow_fastcopy, exec_cflow_fastcopy_saved, exec_cflow_copy,
	 "fastcopy",
         NULL/*"Copy cflow data using default hostaddr and filename"*/,
         PRIV_ROOT | PRIV_HIDDEN);

/* cflow status */
EOLNS (exec_cflow_status_eol, cflow_display_status);
KEYWORD (exec_cflow_status, exec_cflow_status_eol, exec_cflow_fastcopy,
	 "status", NULL/*"Display the status"*/, PRIV_ROOT | PRIV_HIDDEN);

/* cflow listfiles [all] */
EOLNS (exec_cflow_listfiles_eol, cflow_listfiles);
EOLNS (exec_cflow_listfiles_all_eol, cflow_listfiles_all);
KEYWORD (exec_cflow_listfiles_all, exec_cflow_listfiles_all_eol,
         exec_cflow_listfiles_eol, "all",
         NULL/*"List instrumented files touched sofar"*/,
         PRIV_ROOT | PRIV_HIDDEN);
KEYWORD (exec_cflow_listfiles, exec_cflow_listfiles_all, exec_cflow_status,
	 "listfiles",
         NULL/*"List instrumented files touched sofar that have nonzero counts"*/,
         PRIV_ROOT | PRIV_HIDDEN);

/* cflow killsaved */
EOLNS (exec_cflow_killsaved_eol, cflow_killsaved);
KEYWORD (exec_cflow_killsaved, exec_cflow_killsaved_eol,
         exec_cflow_listfiles, "killsaved",
         NULL/*"Free the memory used by saved cflow data"*/,
         PRIV_ROOT | PRIV_HIDDEN);

/* cflow clear */
EOLNS (exec_cflow_clear_eol, cflow_clear);
KEYWORD (exec_cflow_clear, exec_cflow_clear_eol, exec_cflow_killsaved,
	 "clear",
         NULL/*"Set cflow counters to zeros"*/, PRIV_ROOT | PRIV_HIDDEN);

/* cflow save */
EOLNS (exec_cflow_save_eol, cflow_save);
KEYWORD (exec_cflow_save, exec_cflow_save_eol, exec_cflow_clear,
	 "save", NULL/*"Save cflow data in memory"*/,
         PRIV_ROOT | PRIV_HIDDEN);

KEYWORD (exec_cflow, exec_cflow_save, ALTERNATE,
	 "cflow", NULL/*"Controlflow coverage operation"*/,
	 PRIV_ROOT | PRIV_HIDDEN);

#undef	ALTERNATE
#define	ALTERNATE exec_cflow
