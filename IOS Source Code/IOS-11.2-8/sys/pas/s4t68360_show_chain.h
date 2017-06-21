/* $Id: s4t68360_show_chain.h,v 3.2.62.1 1996/08/12 16:06:48 widmer Exp $
 * $Source: /release/112/cvs/Xsys/pas/s4t68360_show_chain.h,v $
 *------------------------------------------------------------------
 * s4t68360_show_chain.h : 4T (QUIC chip) show parser chains
 *
 * June 1995, H.M. Robison
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: s4t68360_show_chain.h,v $
 * Revision 3.2.62.1  1996/08/12  16:06:48  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.2  1995/11/17  18:55:44  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:58:41  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:22:02  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/06/12  20:47:58  mbeesley
 * Add placeholder files for new development.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
/*
 * ------- SHOW COMMANDS ---------
 */

/***************************************************************
 *	peek <addr> 
 *
 */

EOLNS    (s4t_peek_eol, s4t_peek);

NUMBER  (s4t_peek_addr, s4t_peek_eol, no_alt,
        OBJ(int,1), 0x40000000, 0x43000000,
        "R4600 uncached hex address");

KEYWORD (s4t_peek, s4t_peek_addr, no_alt, 
         "peek4600", "Read R4600 uncached address ", 
	 PRIV_ROOT);

/***************************************************************
 *	poke <addr> <data>
 *
 */

EOLNS    (s4t_poke_eol, s4t_poke);

NUMBER  (s4t_poke_data, s4t_poke_eol, no_alt,
        OBJ(int,2), 0, 0xFFFFFFFF,
        "R4600 hex data ");

NUMBER  (s4t_poke_addr, s4t_poke_data, no_alt,
        OBJ(int,1), 0x40000000, 0x43000000,
        "R4600 uncached hex address");

KEYWORD (s4t_poke, s4t_poke_addr, s4t_peek, 
         "poke4600", "Write data to R4600 uncached address", 
	 PRIV_ROOT);

/***************************************************************
 *	peek360 (PA bay> <addr> 
 *
 */

EOLNS    (s4t_peek360_eol, s4t_peek360);

NUMBER  (s4t_peek360_addr, s4t_peek360_eol, no_alt,
        OBJ(int,2), 0, 0xFFFFFFFF,
        "hex address");

NUMBER  (s4t_peek360_bay, s4t_peek360_addr, no_alt,
        OBJ(int,1), 0, 1,
        "PA bay 0 or 1");

KEYWORD (s4t_peek360, s4t_peek360_bay, s4t_poke, 
         "peek360", "Read PA 68360 address ", 
	 PRIV_ROOT);

/***************************************************************
 *	poke360 (PA bay> <addr> <data>
 *
 */

EOLNS    (s4t_poke360_eol, s4t_poke360);

NUMBER  (s4t_poke360_data, s4t_poke360_eol, no_alt,
        OBJ(int,2), 0, 0xFFFFFFFF,
        "hex data ");

NUMBER  (s4t_poke360_addr, s4t_poke360_data, no_alt,
        OBJ(int,1), 0, 0xFFFFFFFF,
        "hex address");

NUMBER  (s4t_poke360_bay, s4t_poke360_addr, no_alt,
        OBJ(int,1), 0, 1,
        "PA bay 0 or 1");

KEYWORD (s4t_poke360, s4t_poke360_bay, s4t_peek360, 
         "poke360", "Write data to PA 68360 address", 
	 PRIV_ROOT);


/******************************************************************
 * print 4T PA PCI registers (AMCC chip)
 *  pregs <PA bay>
 */

EOLNS    (s4t_pregs_eol, s4t_pregs);

NUMBER  (s4t_pregs_bay, s4t_pregs_eol, no_alt,
        OBJ(int,1), 0, 1,
        "port adapter bay 0 or 1");

KEYWORD (s4t_pregs, s4t_pregs_bay, s4t_poke360,
	 "pregs", "Print 4T PA PCI registers",
	 PRIV_ROOT);

/******************************************************************
 * print 4T PA bus operation registers
 * cregs <PA bay>
 */

EOLNS    (s4t_cregs_eol, s4t_cregs);

NUMBER  (s4t_cregs_bay, s4t_cregs_eol, no_alt,
        OBJ(int,1), 0, 1,
        "port adapter bay 0 or 1");

KEYWORD (s4t_cregs, s4t_cregs_bay, s4t_pregs,
	 "cregs", "Print 4T PA operation registers",
	 PRIV_ROOT);



/******************************************************************
 * show s4t68360
 * 
 */

KEYWORD (show_s4t68360, s4t_cregs, ALTERNATE,
	    "4T", "Show 4T/MC68360 Information",
	    PRIV_ROOT);

#undef  ALTERNATE
#define ALTERNATE  show_s4t68360

/* end of file */


