/* $Id: cfg_prompt.h,v 3.2.62.1 1996/08/12 16:05:52 widmer Exp $
 * $Source: /release/112/cvs/Xsys/parser/cfg_prompt.h,v $
 *------------------------------------------------------------------
 * C F G _ P R O M P T . H
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_prompt.h,v $
 * Revision 3.2.62.1  1996/08/12  16:05:52  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.2  1995/11/17  18:44:59  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:51:36  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:11:53  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * [no] prompt <prompt-string>
 */
EOLNS	(prompt_eol, prompt_command);

STRING	(prompt_name, prompt_eol, no_alt, OBJ(string,1),
	"Prompt string");

NOPREFIX (prompt_noprefix, prompt_name, prompt_eol);
NVGENNS	(prompt_nvgen, prompt_noprefix, prompt_command);
KEYWORD	(sys_prompt, prompt_nvgen, ALTERNATE, "prompt",
	"Set system's prompt", PRIV_CONF);

#undef ALTERNATE
#define ALTERNATE sys_prompt
