/* $Id: vip_chain.h,v 3.4.20.2 1996/08/12 16:07:40 widmer Exp $
 * $Source: /release/112/cvs/Xsys/src-vip/vip_chain.h,v $
 *------------------------------------------------------------------
 * vip_chain.h - Parser support for VIP
 *
 * March 1995, Michael Beesley
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: vip_chain.h,v $
 * Revision 3.4.20.2  1996/08/12  16:07:40  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.4.20.1  1996/04/19  15:31:56  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commit
 *
 * Revision 2.1.6.4  1996/02/21  04:09:53  che
 * Branch: IosSec_branch
 * Sync to 11.1 Mainline (02/20/96)
 *
 * Revision 2.1.6.3  1996/01/09  19:13:33  xliu
 * Branch: IosSec_branch
 * Delete obsolete prototyping functions.
 *
 * Revision 2.1.6.2  1995/12/17  02:22:31  che
 * Branch: IosSec_branch
 * Sync to Arkansas
 *
 * Revision 2.1.6.1  1995/11/17  18:18:54  carrel
 * Branch: IosSec_branch
 * Sync to Arkansas
 *
 * Revision 3.4  1996/02/01  23:34:33  widmer
 * CSCdi45325:  Change Keyword/Set to use Keyword_Id macro
 *
 * Revision 3.3  1995/12/09  21:56:56  mbeesley
 * CSCdi45202:  VIP/PCI parse chains need slight reorganization
 *
 * Revision 3.2  1995/11/17  18:51:41  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:29:46  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:29:36  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/06/07  22:59:34  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * ------- SHOW COMMANDS ---------
 */
/******************************************************************
 * show vip command-queue
 * 
 */

EOLNS    (vip_stats_eol, vip_show_statistics);

KEYWORD (vip_stats, vip_stats_eol, no_alt,
	 "statistics", "Show VIP network level statistics",
	 PRIV_ROOT);

/******************************************************************
 * show vip command-queue
 * 
 */

EOLNS    (vip_cmdq_eol, vip_show_cmdq);

KEYWORD (vip_cmdq, vip_cmdq_eol, vip_stats,
	 "command-queue", "Show VIP Malu Attention command queue",
	 PRIV_ROOT);

/******************************************************************
 * show vip dma receive queue
 * 
 */

EOLNS    (vip_drq_eol, vip_show_drq);

KEYWORD (vip_drq, vip_drq_eol, vip_cmdq,
	 "drq", "Show VIP DMA receive queue",
	 PRIV_ROOT);

/******************************************************************
 * show vip dma transmit queue
 * 
 */

EOLNS    (vip_dtq_eol, vip_show_dtq);

KEYWORD (vip_dtq, vip_dtq_eol, vip_drq,
	 "dtq", "Show VIP DMA transmit queue",
	 PRIV_ROOT);

/******************************************************************
 * show vip dma-engine
 * 
 */

EOLNS    (vip_dma_eol, vip_show_dma);

KEYWORD (vip_dma, vip_dma_eol, vip_dtq,
	 "dma-engine", "Show VIP DMA engine",
	 PRIV_ROOT);

/******************************************************************
 * show vip memd
 * 
 */

EOLNS    (vip_memd_eol, vip_show_memd);

KEYWORD (vip_memd, vip_memd_eol, vip_dma,
	 "memd", "Show VIP MEMD buffers/queues",
	 PRIV_ROOT);

/******************************************************************
 * show vip
 * 
 */

KEYWORD_MM(show_vip, vip_memd, NONE,
	   "vip", "VIP system information", PRIV_ROOT, 3);

/*
 * ------- DEBUG COMMANDS ---------
 */

/******************************************************************
 * [no] debug vip all
 * 
 */

EOLNS    (debug_vip_all_eol, vip_set_debug_all);

KEYWORD (debug_vip_all, debug_vip_all_eol, no_alt,
	 "all", "All VIP debugging messages",
	 PRIV_OPR);

/******************************************************************
 * [no] debug vip command-queue
 * 
 */

EOLS    (debug_vip_cmdq_eol, debug_command, DEBUG_VIP_CMDQ);

KEYWORD (debug_vip_cmdq, debug_vip_cmdq_eol, debug_vip_all,
	 "command-queue", "VIP CBus commands",
	 PRIV_OPR);

/******************************************************************
 * [no] debug vip memd
 * 
 */

EOLS    (debug_vip_memd_eol, debug_command, DEBUG_VIP_MEMD);

KEYWORD (debug_vip_memd, debug_vip_memd_eol, debug_vip_cmdq,
	 "memd", "VIP memd usage",
	 PRIV_OPR);

/******************************************************************
 * [no] debug vip dma-tx
 * 
 */

EOLS    (debug_vip_dma_tx_eol, debug_command, DEBUG_VIP_DMA_TX);

KEYWORD (debug_vip_dma_tx, debug_vip_dma_tx_eol, debug_vip_memd,
	 "dma-tx", "VIP DMA activity (Tx direction)",
	 PRIV_OPR);

/******************************************************************
 * [no] debug vip dma-rx
 * 
 */

EOLS    (debug_vip_dma_rx_eol, debug_command, DEBUG_VIP_DMA_RX);

KEYWORD (debug_vip_dma_rx, debug_vip_dma_rx_eol, debug_vip_dma_tx,
	 "dma-rx", "VIP DMA activity (Rx direction)",
	 PRIV_OPR);

/******************************************************************
 * debug crypto engine 
 * 
 */

EOLS    (debug_vip_crypto_engine_eol, debug_command, DEBUG_VIP_CRYPTO_ENGINE);

KEYWORD (debug_vip_crypto_engine, debug_vip_crypto_engine_eol, debug_vip_dma_rx,
	 "crypto-engine", "VIP crypto engine", PRIV_OPR);

/******************************************************************
 * debug vip
 * 
 */

KEYWORD_MM (debug_vip_root, debug_vip_crypto_engine, no_alt,
	    "vip", "VIP system debugging",
	    PRIV_ROOT, 3);

SET     (debug_vip, debug_vip_root, OBJ(pdb,1), vip_debug_arr);

LINK_POINT (debug_vip_commands, debug_vip);
LINK_POINT (show_vip_commands, show_vip);

/* end of file */
