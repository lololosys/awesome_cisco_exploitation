/* $Id: pas_diags_chain.h,v 
 *------------------------------------------------------------------
 * pas_diags_chain.h - PA diagnostic menu
 *
 * June 1996, Paulina D. Tran
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 *------------------------------------------------------------------
 * $Endlog$
 */

/*******************************************************************
 * diag pas menu
 * 
 */
 
EOLNS    (diags_pas_eol, pas_diags);
 
KEYWORD (diags_pas, diags_pas_eol, no_alt,
	 "diags", "PA diagnostic menu", PRIV_ROOT);

#undef  ALTERNATE
#define ALTERNATE diags_pas

