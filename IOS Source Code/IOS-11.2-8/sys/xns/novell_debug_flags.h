/* $Id: novell_debug_flags.h,v 3.2.62.1 1996/05/03 02:18:27 sluong Exp $
 * $Source: /release/112/cvs/Xsys/xns/novell_debug_flags.h,v $
 *------------------------------------------------------------------
 * novell_debug_flags.h -- Debugging flag declarations for Novell
 *
 * May 1993, John Bashinski
 *
 * Copyright (c) 1993-1996 by Cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: novell_debug_flags.h,v $
 * Revision 3.2.62.1  1996/05/03  02:18:27  sluong
 * Branch: California_branch
 * IPX ACL Violation Logging, Plaining English Filters, Display SAP by
 * name,
 * NLSP MIBS, Scaleable RIP/SAP, Modularity, and SAP query by name.
 *
 * Revision 3.2  1995/11/17  19:23:08  gstovall
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:59:04  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/11/08  21:39:11  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.2  1995/08/23  18:05:29  dkatz
 * CSCdi37692:  LSP checksum errors can melt network
 * Add a knob to discard incoming LSPs with checksum errors, rather
 * than purging them.
 *
 * Add additional debugging capabilities.
 *
 * Revision 2.1  1995/06/07  23:28:40  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * We get included (multiple times) from Novell code to define
 * debugging flags. For this, we use some macros that get defined
 * by debug_macros.h. What definitions the macros get depends on
 * which magic flags have been set by the file that includes us.
 */
#include "../ui/debug_macros.h"

/* Now define the actual flags and the array that points to them */
DEBUG_ARRDECL(novell_debug_arr)

DEBUG_FLAG(novell_debug,DEBUG_NOVELL,"IPX packet")
DEBUG_FLAG(novelligrp_debug,DEBUG_NOVELLIGRP,"IPX EIGRP")
DEBUG_FLAG(novelligrp_event_debug,DEBUG_NOVELLIGRPEVT,"IPX EIGRP events")
DEBUG_FLAG(novellrip_debug,DEBUG_NOVELLRIP,"IPX routing")
DEBUG_FLAG(novellrip_event_debug,DEBUG_NOVELLRIPEVT,"IPX routing events")
DEBUG_FLAG(novellsap_debug,DEBUG_NOVELLSAP,"IPX service")
DEBUG_FLAG(novellsap_event_debug,DEBUG_NOVELLSAPEEVT,"IPX service events")
DEBUG_FLAG(novellipxwan_debug,DEBUG_NOVELLIPXWAN,"IPX IPXWAN events")
DEBUG_FLAG(novellisis_debug,DEBUG_NOVELLISIS,"IPX ISIS")
DEBUG_FLAG(novellisis_event_debug,DEBUG_NOVELLISISEVT,"IPX ISIS events")
DEBUG_FLAG(novellnlsp_debug,DEBUG_NOVELLNLSPACT,"IPX NLSP activity")
DEBUG_FLAG(novellnlsp_event_debug,DEBUG_NOVELLNLSPEVT,"IPX NLSP events")
DEBUG_FLAG(novellcompression_debug, DEBUG_NOVELLCOMPRESSIONACT,
	   "IPX compression")
DEBUG_FLAG(novellcompression_event_debug, DEBUG_NOVELLCOMPRESSIONEVT,
 	   "IPX compression events")
DEBUG_FLAG(novellredist_debug,DEBUG_NOVELLREDIST,"IPX route redistribution")
DEBUG_FLAG(novell_verbose_debug,DEBUG_NOVELLVERBOSE,"IPX verbose")
DEBUG_FLAG(novell_private_debug,DEBUG_NOVELLPVT,"IPX internal")
DEBUG_FLAG(novell_ripupd_debug,DEBUG_NOVELL_RIP_UPD,"IPX RIP daemon")
DEBUG_FLAG(novell_sapupd_debug,DEBUG_NOVELL_SAP_UPD,"IPX SAP daemon")
DEBUG_FLAG(novell_spoof_debug,DEBUG_NOVELLSPOOF,"IPX and SPX spoofing")
DEBUG_FLAG_NO_DECL(novellnlsp_adj_packet_debug,DEBUG_NLSPADJPKT,
		   "NLSP Adjacency related packets")
DEBUG_FLAG_NO_DECL(novellnlsp_spf_event_debug,DEBUG_NLSPSPFEVNT,
		   "NLSP SPF events")
DEBUG_FLAG_NO_DECL(novellnlsp_update_packet_debug,DEBUG_NLSPUPDATEPKT,
		   "NLSP Update related packet")
DEBUG_FLAG_NO_DECL(novellnlsp_spf_statistics_debug,DEBUG_NLSPSPFSTATS,
		   "NLSP SPF Timing and Statistics Data")
DEBUG_FLAG_NO_DECL(novellnlsp_spf_trigger_debug,DEBUG_NLSPSPFTRIGGERS,
		   "NLSP SPF triggering events")
DEBUG_FLAG_NO_DECL(novellnlsp_snp_debug,DEBUG_NLSPSNPPACKETS,
		   "NLSP CSNP/PSNP packets")
DEBUG_FLAG_NO_DECL(novellnlsp_local_update_debug,DEBUG_NLSPLOCUPDATE,
		   "NLSP local updates")
DEBUG_FLAG_NO_DECL(novellnlsp_cksum_error_debug,DEBUG_NLSPCKSUMERR,
		   "NLSP checksum errors")
DEBUG_FLAG_NO_DECL(novellnlsp_proto_error_debug,DEBUG_NLSPPROTOERR,
		   "NLSP protocol errors")

DEBUG_ARRDONE
