/* $Id: dec21140_test_chain.h,v 3.2.62.1 1996/08/12 16:06:40 widmer Exp $
 * $Source: /release/112/cvs/Xsys/pas/dec21140_test_chain.h,v $
 *------------------------------------------------------------------
 * dec21140_test_chain.h : DEC 21140  test parser chains
 *
 * April 1995, Steve Jacobson
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: dec21140_test_chain.h,v $
 * Revision 3.2.62.1  1996/08/12  16:06:40  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.2  1995/11/17  18:53:37  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:57:18  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:21:04  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
/*
 * ------- TEST COMMANDS ---------
 */

/******************************************************************
 * Issue software reset to dec21140
 * 
 */

EOLNS    (dec21140_swreset_eol, dec21140_swreset);

KEYWORD (dec21140_swreset, dec21140_swreset_eol, no_alt,
	 "swreset", "Issue SW reset to DEC21140",
	 PRIV_ROOT);

/******************************************************************
 * Cause dec21140 interrupt service
 * 
 */

EOLNS    (dec21140_rxpoll_eol, dec21140_rxpoll);

KEYWORD (dec21140_rxpoll, dec21140_rxpoll_eol, dec21140_swreset,
	 "rxpoll", "Force DEC RX ring poll",
	 PRIV_ROOT);

/******************************************************************
 * test dec21140 TX ring poll
 * 
 */

EOLNS    (dec21140_txpoll_eol, dec21140_txpoll);

KEYWORD (dec21140_txpoll, dec21140_txpoll_eol, dec21140_rxpoll,
	 "txpoll", "Force DEC TX ring poll",
	 PRIV_ROOT);

/******************************************************************
 * test dec21140 RX ring poll
 * 
 */

EOLNS    (dec21140_intsrv_eol, dec21140_intsrv);

KEYWORD (dec21140_intsrv, dec21140_intsrv_eol, dec21140_txpoll,
	 "intsrv", "Call DEC TX, RX interrupt handlers",
	 PRIV_ROOT);

/******************************************************************
 * Test dec21140 MII enable on port 0
 * 
 */

EOLNS    (dec21140_miien0_eol, dec21140_miien0);

KEYWORD (dec21140_miien0, dec21140_miien0_eol, dec21140_intsrv,
	 "miien0", "MII enable on port 0",
	 PRIV_ROOT);

/******************************************************************
 * Test dec21140 MII enable on port 1
 * 
 */

EOLNS    (dec21140_miien1_eol, dec21140_miien1);

KEYWORD (dec21140_miien1, dec21140_miien1_eol, dec21140_miien0,
	 "miien1", "MII enable on port 1",
	 PRIV_ROOT);

/******************************************************************
 * Test dec21140 MII disable on port 0
 * 
 */

EOLNS    (dec21140_miidis0_eol, dec21140_miidis0);

KEYWORD (dec21140_miidis0, dec21140_miidis0_eol, dec21140_miien1,
	 "miidis0", "MII disable on port 0",
	 PRIV_ROOT);

/******************************************************************
 * Test dec21140 MII disable on port 1
 * 
 */

EOLNS    (dec21140_miidis1_eol, dec21140_miidis1);

KEYWORD (dec21140_miidis1, dec21140_miidis1_eol, dec21140_miidis0,
	 "miidis1", "MII disable on port 1",
	 PRIV_ROOT);

/******************************************************************
 * test segpak level dec21140 packet blasting on port 0
 * 
 */

EOLNS    (dec21140_sblast0_eol, dec21140_sblast0);

KEYWORD (dec21140_sblast0, dec21140_sblast0_eol, dec21140_miidis1,
	 "sblast0", "Segpak-level packet blast on port 0",
	 PRIV_ROOT);

/******************************************************************
 * test segpak level dec21140 packet blasting on port 1
 * 
 */

EOLNS    (dec21140_sblast1_eol, dec21140_sblast1);

KEYWORD (dec21140_sblast1, dec21140_sblast1_eol, dec21140_sblast0,
	 "sblast1", "Segpak-level packet blast on port 1",
	 PRIV_ROOT);

/******************************************************************
 * test mini-buffer level dec21140 packet blasting on port 0
 * 
 */

EOLNS    (dec21140_mblast0_eol, dec21140_mblast0);

KEYWORD (dec21140_mblast0, dec21140_mblast0_eol, dec21140_sblast1,
	 "mblast0", "Mbuf-level packet blast on port 0",
	 PRIV_ROOT);

/******************************************************************
 * test mini-buffer level dec21140 packet blasting on port 1
 * 
 */

EOLNS    (dec21140_mblast1_eol, dec21140_mblast1);

KEYWORD (dec21140_mblast1, dec21140_mblast1_eol, dec21140_mblast0,
	 "mblast1", "Mbuf-level packet blast on port 1",
	 PRIV_ROOT);

/******************************************************************
 * test dec21140 multiple checking pings on port 0
 * 
 */

EOLNS    (dec21140_bping0_eol, dec21140_bping0);

KEYWORD (dec21140_bping0, dec21140_bping0_eol, dec21140_mblast1,
	 "bping0", "Segpak level checking ping on port 0",
	 PRIV_ROOT);

/******************************************************************
 * test dec21140 multiple checking pings on port 1
 * 
 */

EOLNS    (dec21140_bping1_eol, dec21140_bping1);

KEYWORD (dec21140_bping1, dec21140_bping1_eol, dec21140_bping0,
	 "bping1", "Segpak level checking ping on port 1",
	 PRIV_ROOT);

/******************************************************************
 * test dec21140 single ping on port 0
 * 
 */

EOLNS    (dec21140_aping0_eol, dec21140_aping0);

KEYWORD (dec21140_aping0, dec21140_aping0_eol, dec21140_bping1,
	 "aping0", "Segpak level ping on port 0",
	 PRIV_ROOT);

/******************************************************************
 * test dec21140 single ping on port 1
 * 
 */

EOLNS    (dec21140_aping1_eol, dec21140_aping1);

KEYWORD (dec21140_aping1, dec21140_aping1_eol, dec21140_aping0,
	 "aping1", "Segpak level ping on port 1",
	 PRIV_ROOT);

/******************************************************************
 * test dec21140 arp on port0
 * 
 */

EOLNS    (dec21140_send_arp0_eol, dec21140_send_arp0);

KEYWORD (dec21140_send_arp0, dec21140_send_arp0_eol, dec21140_aping1,
	 "arp0", "Send arp request to Seattle on port 0",
	 PRIV_ROOT);

/******************************************************************
 * test dec21140 arp on port1
 * 
 */

EOLNS    (dec21140_send_arp1_eol, dec21140_send_arp1);

KEYWORD (dec21140_send_arp1, dec21140_send_arp1_eol, dec21140_send_arp0,
	 "arp1", "Send arp request to Seattle on port 1",
	 PRIV_ROOT);

/******************************************************************
 * test dec21140
 * 
 */

KEYWORD (test_dec21140, dec21140_send_arp1, ALTERNATE,
	    "dec21140", "DEC 21140 test system",
	    PRIV_ROOT);

#undef  ALTERNATE
#define ALTERNATE test_dec21140

/* end of file */

 
