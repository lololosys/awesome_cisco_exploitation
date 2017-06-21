/* $Id: cfg_srb.h,v 3.3.4.1 1996/08/12 16:07:46 widmer Exp $
 * $Source: /release/112/cvs/Xsys/srt/cfg_srb.h,v $
 *------------------------------------------------------------------
 * C F G _ S R B . H
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_srb.h,v $
 * Revision 3.3.4.1  1996/08/12  16:07:46  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.3  1996/03/11  14:05:50  rbatz
 * CSCdi45116:  Duplicate All-Routes Explorers unnecessarily
 *              forwarded in redundant topologies.
 *
 * Revision 3.2  1995/11/17  18:53:22  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:30:38  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:30:04  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/07/04  01:58:00  tli
 * CSCdi36508: Rationalize parser file placement, part I
 * Rationalize cls dev dlsw dspu filesys irix lat les lex llc2 parser pgen
 * pt servers smf smrp snmp src-36-c1000 src-4k-c4000 src-68-c3000
 * src-68-c4000 src-c4000 src-rsp srt tcl ts ui
 *
 * Revision 2.1  1995/06/07  22:13:54  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

LINK_TRANS(csrb_extend_here, no_alt);

/**********************************************************************
 * [no] source-bridge explorer-dup-ARE-filter
 */
EOLS    (srb_explorer_dupfilter_eol,srb_command,SRB_EXPLORER_DUP_ARE_FILTER);
KEYWORD (srb_explorer_dupfilter, srb_explorer_dupfilter_eol, csrb_extend_here,
        "explorer-dup-ARE-filter",
	"Enable filtering of duplicate all-routes explorers", PRIV_CONF);

/**********************************************************************
 * [no] source-bridge explorer-fastswitch
 */
EOLS    (srb_explorer_fastswitch_eol,srb_command,SRB_EXPLORER_FASTSWITCH);
KEYWORD (srb_explorer_fastswitch, srb_explorer_fastswitch_eol, 
        srb_explorer_dupfilter, "explorer-fastswitch",
	"Enable local explorer fastswitching", PRIV_CONF);

/**********************************************************************
 * [no] source-bridge explorer-maxrate <max-bytes-per-second>
 */
PARAMS (srb_explorer_maxrate, srb_explorer_fastswitch, 
	"explorer-maxrate", OBJ(int,1), 100, -1,
        srb_command, SRB_EXPLORER_MAXRATE,
	"Maximum SRB local explorer data rate in bytes/sec",
	"Maximum data rate for fastswitched explorers", PRIV_CONF);

/**********************************************************************
 * [no] source-bridge explorerQ-depth <max-queued-packets>
 */
PARAMS (srb_explorerQ_depth, srb_explorer_maxrate, 
	"explorerQ-depth", OBJ(int,1), 1, 500,
        srb_command, SRB_EXPLORERQ_DEPTH,
	"Maximum SRB explorer queue depth per interface",
	"Maximum queue size for process switched explorers", PRIV_CONF);

/**********************************************************************
 *	source-bridge largest-frame <ring-group> <size>
 *	no source-bridge largest-frame <ring-group> [<size>]
 *
 * OBJ(int,1) = <ring-group>
 * OBJ(int,2) = <size>
 */
EOLS	(srb_lframe_eol, srb_command, SRB_FRAMESIZE);
KEYWORD_ID(srb_lframe_17800, srb_lframe_eol, no_alt,
           OBJ(int,2), 17800,
	   "17800", "17800 byte maximum frame size", PRIV_CONF);
KEYWORD_ID(srb_lframe_11454, srb_lframe_eol, srb_lframe_17800,
           OBJ(int,2), 11454,
	   "11454", "11454 byte maximum frame size", PRIV_CONF);
KEYWORD_ID(srb_lframe_11407, srb_lframe_eol, srb_lframe_11454,
           OBJ(int,2), 11407,
	   "11407", "11407 byte maximum frame size", PRIV_CONF);
KEYWORD_ID(srb_lframe_8144, srb_lframe_eol, srb_lframe_11407,
           OBJ(int,2), 8144,
	   "8144", "8144 byte maximum frame size", PRIV_CONF);
KEYWORD_ID(srb_lframe_4472, srb_lframe_eol, srb_lframe_8144,
           OBJ(int,2), 4472,
	   "4472", "4472 byte maximum frame size", PRIV_CONF);
KEYWORD_ID(srb_lframe_2052, srb_lframe_eol, srb_lframe_4472,
           OBJ(int,2), 2052,
	   "2052", "2052 byte maximum frame size", PRIV_CONF);
KEYWORD_ID(srb_lframe_1500, srb_lframe_eol, srb_lframe_2052,
           OBJ(int,2), 1500,
	   "1500", "1500 byte maximum frame size", PRIV_CONF);
KEYWORD_ID(srb_lframe_1470, srb_lframe_eol, srb_lframe_1500,
           OBJ(int,2), 1470,
	   "1470", "1470 byte maximum frame size", PRIV_CONF);
KEYWORD_ID(srb_lframe_516, srb_lframe_eol, srb_lframe_1470,
           OBJ(int,2), 516, "516", "516 byte maximum frame size", PRIV_CONF);
NOPREFIX(srb_lframe_noprefix, srb_lframe_516, srb_lframe_eol);
NUMBER	(srb_lframe_ring, srb_lframe_noprefix, no_alt,
	 OBJ(int,1), 1, SRB_MAX_RING_NUM, "Ring Group");
KEYWORD	(srb_lframe, srb_lframe_ring, srb_explorerQ_depth, 
	 "largest-frame", "Largest frame size to use in a ring-group",
	 PRIV_CONF);

/**********************************************************************
 *	[no] source-bridge ring-group <ring> [<virtual-mac-address>]
 */
EOLS    (srb_ringgrp_eol, srb_command, SRB_RINGGR);
ENADDR	(srb_ringgrp_vmac, srb_ringgrp_eol, srb_ringgrp_eol, 

	 OBJ(hwaddr,1), "Virtual MAC address for the ring");
NUMBER	(srb_ringgrp_num, srb_ringgrp_vmac, srb_ringgrp_vmac, 
	 OBJ(int,1), 1, SRB_MAX_RING_NUM, "Virtual Ring Number");
KEYWORD	(srb_ringgrp, srb_ringgrp_num, srb_lframe,
	 "ring-group", 
	 "Define a ring group", PRIV_CONF);

/**********************************************************************
 * 
 * source-bridge old-oui
 */
PRINT  (srb_oldoui_print, NONE,
	"\n\"source-bridge old-oui\" has been replaced by \"bridge old-oui\"");
KEYWORD	(srb_oldoui, srb_oldoui_print, srb_ringgrp,
	 "old-oui", NULL, PRIV_CONF|PRIV_HIDDEN|PRIV_NONVGEN);


/**********************************************************************
 * Do NV generation all at once because the ring-group info needs to
 * be generated in order
 */
NVGENNS	(csrb_nvgen, srb_oldoui, srb_command);
KEYWORD	(csrb, csrb_nvgen, NONE, "source-bridge",
	 "Source-route bridging ring groups", PRIV_CONF);

TEST_BOOLEAN(csrb_test, csrb, NONE, ALTERNATE, bridge_enable);

#undef	ALTERNATE
#define	ALTERNATE	csrb_test
