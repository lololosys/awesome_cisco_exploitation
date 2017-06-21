/* $Id: s4t68360_test_chain.h,v 3.2.60.1 1996/08/12 16:06:49 widmer Exp $
 * $Source: /release/112/cvs/Xsys/pas/s4t68360_test_chain.h,v $
 *------------------------------------------------------------------
 * s4t68360_test_chain.h : 4T (QUIC chip) test parser chains
 *
 * June 1995, H.M. Robison
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: s4t68360_test_chain.h,v $
 * Revision 3.2.60.1  1996/08/12  16:06:49  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.2  1995/11/17  18:55:46  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:58:42  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:22:03  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/06/12  20:47:59  mbeesley
 * Add placeholder files for new development.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * ------- TEST COMMANDS ---------
 */

/******************************************************************
 * Issue software reset to 4T PA
 * 
 */

EOLNS    (s4t_swreset_eol, s4t_swreset);

NUMBER  (s4t_swreset_bay, s4t_swreset_eol, no_alt,
        OBJ(int,1), 0, 1,
        "PA bay 0 or 1");

KEYWORD (s4t_swreset, s4t_swreset_bay, no_alt,
	 "swreset", "Issue SW reset to 4T PA",
	 PRIV_ROOT);


/******************************************************************
 * Download code to 4T PA
 *   load <PA bay>
 */

EOLNS    (s4t_dwn_ld_eol, s4t_dwn_ld);

NUMBER  (s4t_dwn_ld_bay, s4t_dwn_ld_eol, no_alt,
        OBJ(int,1), 0, 1,
        "PA bay 0 or 1");

KEYWORD (s4t_dwn_ld, s4t_dwn_ld_bay, s4t_swreset,
	 "load", "Download code to 4T PA",
	 PRIV_ROOT);


/******************************************************************
 * Send init block to 4T PA
 *   initblk <PA bay> <port id>
 */

EOLNS    (s4t_initblk_eol, s4t_initblk);

NUMBER  (s4t_initblk_port, s4t_initblk_eol, no_alt,
        OBJ(int,2), 0, 3,
        "PA port 0 to 3");

NUMBER  (s4t_initblk_bay, s4t_initblk_port, no_alt,
        OBJ(int,1), 0, 1,
        "PA bay 0 or 1");

KEYWORD (s4t_initblk, s4t_initblk_bay, s4t_dwn_ld,
	 "initblk", "Send init block to 4T PA",
	 PRIV_ROOT);


/******************************************************************
 * Read status of 4T PAport
 *   status <PA bay> <port id>
 */

EOLNS    (s4t_rd_status_eol, s4t_rd_status);

NUMBER  (s4t_rd_status_port, s4t_rd_status_eol, no_alt,
        OBJ(int,2), 0, 3,
        "PA port 0 to 3");

NUMBER  (s4t_rd_status_bay, s4t_rd_status_port, no_alt,
        OBJ(int,1), 0, 1,
        "PA bay 0 or 1");

KEYWORD (s4t_rd_status, s4t_rd_status_bay, s4t_initblk,
	 "status", "Send init block to 4T PA",
	 PRIV_ROOT);


/******************************************************************
 * Execute both TX & RX ISRs with interrupts disabled on PA
 * 
 */

EOLNS    (s4t_intsrv_eol, s4t_intsrv);

NUMBER  (s4t_intsrv_bay, s4t_intsrv_eol, no_alt,
        OBJ(int,1), 0, 1,
        "PA bay 0 or 1");

KEYWORD (s4t_intsrv, s4t_intsrv_bay, s4t_rd_status,
	 "intsrv", "Call 4T TX, RX interrupt handlers",
	 PRIV_ROOT);


/******************************************************************
 * test segpak level s4t68360 packet blasting on port
 * 
 */

EOLNS    (s4t_sblast_eol, s4t_sblast);

NUMBER  (s4t_sblast_port, s4t_sblast_eol, no_alt,
        OBJ(int,2), 0, 3,
        "PA port 0 to 3");

NUMBER  (s4t_sblast_bay, s4t_sblast_port, no_alt,
        OBJ(int,1), 0, 1,
        "PA bay 0 or 1");

KEYWORD (s4t_sblast, s4t_sblast_bay, s4t_intsrv,
	 "sblast", "4T Segpak-level packet blast on port",
	 PRIV_ROOT);


/******************************************************************
 * test s4t68360 multiple checking pings on port 
 * 
 */

EOLNS    (s4t_bping_eol, s4t_bping);

NUMBER  (s4t_bping_port, s4t_bping_eol, no_alt,
        OBJ(int,2), 0, 3,
        "PA port 0 to 3");

NUMBER  (s4t_bping_bay, s4t_bping_port, no_alt,
        OBJ(int,1), 0, 1,
        "PA bay 0 or 1");

KEYWORD (s4t_bping, s4t_bping_bay, s4t_sblast,
	 "bping", "4T Segpak level checking ping on port",
	 PRIV_ROOT);


/******************************************************************
 * test s4t68360 single ping on port
 * 
 */

EOLNS    (s4t_aping_eol, s4t_aping);

NUMBER  (s4t_aping_port, s4t_aping_eol, no_alt,
        OBJ(int,2), 0, 3,
        "PA port 0 to 3");

NUMBER  (s4t_aping_bay, s4t_aping_port, no_alt,
        OBJ(int,1), 0, 1,
        "PA bay 0 or 1");

KEYWORD (s4t_aping, s4t_aping_bay, s4t_bping,
	 "aping", "4T Segpak level ping on port",
	 PRIV_ROOT);



/******************************************************************
 * test s4t68360 TX ring on <pa bay> <port>
 * 
 */

EOLNS    (s4t_xmit_test_eol, s4t_xmit_test);

NUMBER  (s4t_xmit_test_port, s4t_xmit_test_eol, no_alt,
        OBJ(int,2), 0, 3,
        "PA port 0 to 3");


NUMBER  (s4t_xmit_test_bay, s4t_xmit_test_port, no_alt,
        OBJ(int,1), 0, 1,
        "PA bay 0 or 1");

KEYWORD (s4t_xmit_test, s4t_xmit_test_bay, s4t_aping,
	 "txtest", "4T test tx ring on a port",
	 PRIV_ROOT);


/******************************************************************
 * Enable 4T PAport
 *   enable <PA bay> <port id>
 */

EOLNS    (s4t_en_port_eol, s4t_en_port);

NUMBER  (s4t_en_port_port, s4t_en_port_eol, no_alt,
        OBJ(int,2), 0, 3,
        "PA port 0 to 3");

NUMBER  (s4t_en_port_bay, s4t_en_port_port, no_alt,
        OBJ(int,1), 0, 1,
        "PA bay 0 or 1");

KEYWORD (s4t_en_port, s4t_en_port_bay, s4t_xmit_test,
	 "enable", "Enable port on 4T PA",
	 PRIV_ROOT);

/******************************************************************
 * control 4T PAport
 *   control <PA bay> <port id>
 */

EOLNS    (s4t_ctrl_eol, s4t_ctrl);

NUMBER  (s4t_ctrl_port, s4t_ctrl_eol, no_alt,
        OBJ(int,2), 0, 3,
        "PA port 0 to 3");

NUMBER  (s4t_ctrl_bay, s4t_ctrl_port, no_alt,
        OBJ(int,1), 0, 1,
        "PA bay 0 or 1");

KEYWORD (s4t_ctrl, s4t_ctrl_bay, s4t_en_port,
	 "control", "Control port on 4T PA",
	 PRIV_ROOT);

/******************************************************************
 * Set txdelay 4T PAport
 *   txdelay <PA bay> <port id>
 */

EOLNS    (s4t_set_delay_eol, s4t_set_delay);

NUMBER  (s4t_set_delay_port, s4t_set_delay_eol, no_alt,
        OBJ(int,3), 0, 3,
        "PA port 0 to 3");

NUMBER  (s4t_set_delay_bay, s4t_set_delay_port, no_alt,
        OBJ(int,2), 0, 1,
        "PA bay 0 or 1");

NUMBER  (s4t_set_delay_val, s4t_set_delay_bay, no_alt,
        OBJ(int,1), 0, 131071,
        "transmit delay <0-131071> in microseconds");

KEYWORD (s4t_set_delay, s4t_set_delay_val, s4t_ctrl,
	 "txdelay", "Set port transmit delay on 4T PA",
	 PRIV_ROOT);

/******************************************************************
 * set 4T port mtu
 *   setmtu <PA bay> <port id>
 */

EOLNS    (s4t_set_mtu_eol, s4t_set_mtu);

NUMBER  (s4t_set_mtu_port, s4t_set_mtu_eol, no_alt,
        OBJ(int,3), 0, 3,
        "PA port 0 to 3");

NUMBER  (s4t_set_mtu_bay, s4t_set_mtu_port, no_alt,
        OBJ(int,2), 0, 1,
        "PA bay 0 or 1");

NUMBER  (s4t_set_mtu_val, s4t_set_mtu_bay, no_alt,
        OBJ(int,1), 64, 18000,
        "max transmission unit 64-18000 bytes");

KEYWORD (s4t_set_mtu, s4t_set_mtu_val, s4t_set_delay,
	 "setmtu", "Set port mtu on 4T PA",
	 PRIV_ROOT);

/******************************************************************
 * set 4T port clock rate
 *   control <PA bay> <port id>
 */

EOLNS    (s4t_set_clk_eol, s4t_set_clk);

NUMBER  (s4t_set_clk_port, s4t_set_clk_eol, no_alt,
        OBJ(int,3), 0, 3,
        "PA port 0 to 3");

NUMBER  (s4t_set_clk_bay, s4t_set_clk_port, no_alt,
        OBJ(int,2), 0, 1,
        "PA bay 0 or 1");

NUMBER  (s4t_set_clk_val, s4t_set_clk_bay, no_alt,
        OBJ(int,1), 0, 2000000,
        "clockrate");

KEYWORD (s4t_set_clk, s4t_set_clk_val, s4t_set_mtu,
	 "clk", "Set port DCE clockrate on 4T PA",
	 PRIV_ROOT);


/******************************************************************
 * test s4t68360
 * 
 */

KEYWORD (test_s4t68360, s4t_set_clk, ALTERNATE,
	    "4T", "Test 4T/MC68360 system",
	    PRIV_ROOT);

#undef  ALTERNATE
#define ALTERNATE test_s4t68360

/* end of file */
