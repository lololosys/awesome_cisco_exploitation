/* $Id: cfg_int_compress.h,v 3.2.62.1 1996/08/12 16:09:43 widmer Exp $
 * $Source: /release/112/cvs/Xsys/x25/cfg_int_compress.h,v $
 *------------------------------------------------------------------
 * CFG_INT_COMPRESS.H
 *
 * July 29, 1993, Bill Thomas
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_int_compress.h,v $
 * Revision 3.2.62.1  1996/08/12  16:09:43  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.2  1995/11/17  18:05:39  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:51:54  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/09/18  18:13:14  gtaylor
 * CSCdi29250:  PPP compression should support the existing compress
 * command. For ppp encapsulation compression now appears without the
 * ppp prefix. The command can still be entered with the prefix.
 *
 * Revision 2.1  1995/07/02  01:44:59  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize xns, atalk, vines, decnet, xremote, x25, unix, wan, tn3270,
 * clns, cdp
 *
 * Revision 2.1  1995/06/07  22:04:17  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * compress <type> 
 *   type
 *      predictor 
 *      stac
 * no compress 
 */


EOLS	(ci_compress_eol, compress_command, COMP_RAND);
KEYWORD_ID	(ci_compress_pred, ci_compress_eol, no_alt, 
            OBJ(int,1), COMP_RAND, "predictor",
	    "predictor compression type", PRIV_CONF);
ASSERT (ci_compress_pred_test, ci_compress_pred, NONE,
	    (csb->interface->hwptr->enctype == ET_LAPB)
      ||(csb->interface->hwptr->enctype == ET_PPP));

KEYWORD_ID	(ci_compress_stac, ci_compress_eol, ci_compress_pred_test,
            OBJ(int,1), COMP_STAC, "stac",
	    "stac compression algorithm", PRIV_CONF);

NOPREFIX (ci_compress_noprefix, ci_compress_stac, ci_compress_eol);
NVGENNS	(ci_compress_nvgen, ci_compress_noprefix, compress_command );

KEYWORD	(ci_compress, ci_compress_nvgen, NONE, "compress",
	    "Set serial interface for compression", PRIV_CONF);
ASSERT (ci_compress_test, ci_compress, ALTERNATE,
	(csb->interface->hwptr->status & IDB_SERIAL) 
	 && ( (csb->interface->hwptr->enctype == ET_LAPB)
            ||(csb->interface->hwptr->enctype == ET_HDLC)
            ||(csb->interface->hwptr->enctype == ET_PPP)
	    ||(csb->interface->hwptr->enctype == ET_X25) ) );

#undef	ALTERNATE
#define	ALTERNATE	ci_compress_test
