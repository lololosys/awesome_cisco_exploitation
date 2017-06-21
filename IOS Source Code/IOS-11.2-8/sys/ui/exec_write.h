/* $Id: exec_write.h,v 3.3 1996/03/09 22:58:10 widmer Exp $
 * $Source: /release/111/cvs/Xsys/ui/exec_write.h,v $
 *------------------------------------------------------------------
 * E X E C _ W R I T E . H
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: exec_write.h,v $
 * Revision 3.3  1996/03/09  22:58:10  widmer
 * CSCdi47890:  HTTP server doesnt work
 * Add HTTP server
 *
 * Revision 3.2  1995/11/17  17:48:53  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:41:22  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/04  01:59:35  tli
 * CSCdi36508: Rationalize parser file placement, part I
 * Rationalize cls dev dlsw dspu filesys irix lat les lex llc2 parser pgen
 * pt servers smf smrp snmp src-36-c1000 src-4k-c4000 src-68-c3000
 * src-68-c4000 src-c4000 src-rsp srt tcl ts ui
 *
 * Revision 2.1  1995/06/07  22:28:08  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/***************************************************************
 *  write <cr>
 *
 */
EOLS	(write_eol, write_memory, WRITE_MEMORY);
ASSERT	(write_test, write_eol, NONE,
	 (csb->priv == PRIV_ROOT));

LINK_TRANS(exec_write_extend_here, write_test);

/***************************************************************
 * write terminal
 *
 */
EOLS	(write_term_eol, write_terminal, WRITE_TERMINAL);
KEYWORD	(write_terminal, write_term_eol, exec_write_extend_here,
	 "terminal", "Write to terminal", PRIV_ROOT);

/***************************************************************
 * write network
 */
EOLS	(write_net_eol, write_network, WRITE_NETWORK);
SET     (write_net_set_def, write_net_eol,
         OBJ(int,2), COPY_RUNNING_CONFIG);
KEYWORD_ID (write_network, write_net_set_def, write_terminal,
	    OBJ(int,1), COPY_TFTP,
	    "network", "Write to network TFTP server",
	    PRIV_ROOT | PRIV_INTERACTIVE);

/***************************************************************
 * write memory
 */
EOLS	(write_mem_eol, write_memory, WRITE_MEMORY);
KEYWORD	(write_mem, write_mem_eol, write_network,
	 "memory", "Write to NV memory", PRIV_ROOT);

/***************************************************************
 * write erase
 */
EOLS	(write_erase_eol, write_erase, WRITE_ERASE);
KEYWORD	(write_erase, write_erase_eol, write_mem,
	 "erase", "Erase NV memory", PRIV_ROOT);

/***************************************************************
 * write core [{ <ipaddr> | <hostname> } [<filename>] ]
 *
 * (*OBJ(paddr,1)) = <ipaddr> or <hostname> coverted to an <ipaddr>
 * OBJ(string,1) = <filename>
 */
EOLS	(write_core_eol, manual_write_core, WRITE_CORE);
PRIV_TEST(write_core_priv, write_core_eol, no_alt, NONE,
	  PRIV_ROOT | PRIV_INTERACTIVE);
TEXT	(write_core_file, write_core_eol, write_core_priv,
	 OBJ(string,1), NULL);
IPADDR_NAME (write_core_host, write_core_file, write_core_priv,
	     OBJ(paddr,1), NULL);
KEYWORD	(write_core, write_core_host, write_erase,
	 "core", NULL, PRIV_ROOT | PRIV_HIDDEN);

KEYWORD	(exec_write, write_core, ALTERNATE,
	 "write", 
	 "Write running configuration to memory, network, or terminal", PRIV_ROOT);

#undef	ALTERNATE
#define	ALTERNATE	exec_write
