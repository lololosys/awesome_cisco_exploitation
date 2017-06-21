/* $Id: epad_show_chain.h,v 3.1.64.3 1996/08/12 16:06:42 widmer Exp $
 * $Source: /release/112/cvs/Xsys/pas/epad_show_chain.h,v $
 *------------------------------------------------------------------
 * epad_show_chain.h - Encryption Port Adaptor Driver show parser 
 *			chains.
 *
 * August 1995, Paulina D. Tran
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: epad_show_chain.h,v $
 * Revision 3.1.64.3  1996/08/12  16:06:42  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.1.64.2  1996/06/27  07:04:25  ptran
 * CSCdi58378:  Encryption PA need performance improvement.
 * Branch: California_branch
 *
 * Revision 3.1.64.1  1996/04/19  15:25:58  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commit
 *
 * Revision 3.1.2.5  1996/04/16  21:54:49  ptran
 * EPA driver clean up. Removed statistic counters used for debugging
 * purposes.
 * Removed parser commands used for testing. Modified to compile in
 * California
 * branch.  Incorporate code review comments.
 *
 * Branch: IosSec_branch
 *
 * Revision 3.1.2.3  1996/02/14  23:45:00  ptran
 * Changed the default of 4E to use I/O mapped I/O.  This change only
 * effect
 * IosSec_branch.
 * Added parser commands to test the encryption key management.
 * Fixed lost of pak and particles when descriptor error occur.
 * Added statistic counters and parser command to display these
 * statistic.
 *
 * Branch: IosSec_branch
 *
 * Revision 3.1.2.2  1996/01/08  19:23:50  ptran
 * Branch: IosSec_branch
 *
 * Revision 3.1.2.1  1995/12/06  21:48:53  rmontino
 * Branch: IosSec_branch
 *
 * Revision 3.1  1995/11/09  12:57:23  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/08/23  02:27:24  ptran
 * Placeholder file for new development.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
/******************************************************************
 * clear statistic command 
 */
EOLNS    (epad_clear_ds_eol, epad_clear_ds_cmd);
NUMBER(epad_clear_ds_ring, epad_clear_ds_eol, no_alt,
        OBJ(int, 1), 0, 1, "0=frame 1=key");
KEYWORD (epad_clear_ds, epad_clear_ds_ring, no_alt,
         "clear", "Clear driver instance structure", PRIV_ROOT);

/*
 * show commands for epad driver
 */

/******************************************************************
 * Show epa frame or key descriptor ring
 *   desc ring <frame/key ring>
 */
EOLNS    (epad_show_desc_ring_eol, epad_show_desc_rings_cmd);
NUMBER(epad_show_desc_ring_option, epad_show_desc_ring_eol, no_alt,
        OBJ(int, 1), 0, 1, "0=frame 1=key");
KEYWORD (epad_show_desc_ring, epad_show_desc_ring_option, epad_clear_ds,
         "desc_ring", "descriptor rings", PRIV_ROOT);

/******************************************************************
 * Show epa idb structure
 *   idb
 */ 
EOLNS    (epad_show_idb_eol, epad_show_idb_cmd);
KEYWORD (epad_show_idb, epad_show_idb_eol, epad_show_desc_ring,
         "idb", "Print idb", PRIV_ROOT);
 
/******************************************************************
 * Show epa device instance structure
 *   ds <frame/key ring>
 */ 
EOLNS    (epad_show_ds_eol, epad_show_ds_cmd);
NUMBER(epad_show_ds_ring, epad_show_ds_eol, no_alt,
        OBJ(int, 1), 0, 1, "0=frame 1=key");
KEYWORD (epad_show_ds, epad_show_ds_ring, epad_show_idb,
         "ds", "Print driver instance structure", PRIV_ROOT);
 
/******************************************************************
 * show pas epa
 */  
KEYWORD (show_epad, epad_show_ds, ALTERNATE,
         "epa", "Show EPA driver information", PRIV_ROOT);
 
#undef ALTERNATE
#define ALTERNATE show_epad

