/* $Id: fddi.c,v 3.11.12.16 1996/09/03 17:57:58 hou Exp $
 * $Source: /release/112/cvs/Xsys/if/fddi.c,v $
 *------------------------------------------------------------------
 * fddi.c -- Fiber Distributed Data Interface 
 *
 * September, 1989  Robert Fletcher
 * may 1992, steve elias
 *
 * Copyright (c) 1986-1996, 1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: fddi.c,v $
 * Revision 3.11.12.16  1996/09/03  17:57:58  hou
 * CSCdi59943:  vLAN  802.10 bridging fails translation from native to
 *              802.10 packet
 * Branch: California_branch
 *
 * Revision 3.11.12.15  1996/08/28  12:48:00  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.11.12.14  1996/08/26  15:08:52  ppearce
 * CSCdi66582:  Implement SRB vector table to cut size of SRB_CORE subsys
 * Branch: California_branch
 *
 * Revision 3.11.12.13  1996/08/19  18:51:58  fsunaval
 * CSCdi64853:  Misaligned access when srb on fip-fip
 * Branch: California_branch
 *
 * Revision 3.11.12.12  1996/08/13  02:18:43  gstovall
 * CSCdi39484:  Giant packets output on ethernet interface
 * Branch: California_branch
 * Make sure we drop giants, even if they are just a wee bit too big.
 *
 * Revision 3.11.12.11  1996/08/07  08:59:46  snyder
 * CSCdi65343:  more things that should be declared const
 *              char * [] -> char *const []
 *              char *    -> const char []
 *              savings 17,496 data space, 1,852 image space
 * Branch: California_branch
 *
 * Revision 3.11.12.10  1996/07/09  05:58:44  ppearce
 * CSCdi59527:  inlines waste code space
 * Branch: California_branch
 *   Just say "no" to inline abuse - SRB subblock inlines removed
 *
 * Revision 3.11.12.9  1996/06/27  09:33:36  fox
 * CSCdi61238:  linktype is an overloaded symbol
 * Branch: California_branch
 * Change idb->linktype to idb->getlink.
 *
 * Revision 3.11.12.8  1996/06/10  03:48:56  fsunaval
 * CSCdi57322:  Alignment Corrections Recorded in fddi_smt_debug
 * Branch: California_branch
 *
 * Revision 3.11.12.7  1996/05/17  11:22:31  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.11.2.5  1996/05/05  23:09:29  ppearce
 * Sync to IbuMod_Calif_baseline_960504
 *
 * Revision 3.11.2.4  1996/05/02  22:47:41  kchiu
 * CSCdi56517:  Image build error for c1005 images in IbuMod_Calif_branch
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.11.2.3  1996/04/26  15:20:24  ppearce
 * IBU modularity - SRB subblock (part 1)
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.11.2.2  1996/04/15  00:09:03  kchiu
 * Branch: IbuMod_Calif_branch
 * Add dlsw+/fddi support
 *
 * Revision 3.11.2.1  1996/04/03  14:37:13  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.11.12.6  1996/05/09  14:29:52  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.11.12.5  1996/05/01  09:15:38  shaker
 * CSCdi54444:  Booting causes FIP FDDI to beacon
 * Branch: California_branch
 * Wait around in the process that starts CMT for it to finish
 * so that we know that no other code disables interrupts,
 * preventing us from bringing the MAC online, causing the
 * ring to beacon.
 *
 * Revision 3.11.12.4.4.1  1996/04/27  07:03:42  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.11.12.4  1996/04/22  18:12:48  gstovall
 * CSCdi53071:  media idb init routines will fail on second pass
 * Branch: California_branch
 * Allow media idb init routines to be called again.  Also clean up the
 * handling of the media idb queues.
 *
 * Revision 3.11.12.3.4.1  1996/04/08  01:54:32  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 3.11.12.3  1996/03/29  03:20:24  hou
 * CSCdi50046:  vLAN 802.10 supports only IPX Arpa encaps.
 * Branch: California_branch
 * - should support default novell-ether IPX encap.
 *
 * Revision 3.11.12.2  1996/03/21  22:48:44  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.11.12.1  1996/03/18  20:13:35  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.11.14.1  1996/03/28  17:25:20  rbadri
 * Branch: LE_Syn_Cal_V111_1_3_branch
 * MTU support for sub-interfaces (used by ATM interfaces)
 *
 * Revision 3.11  1996/02/13  08:12:53  dstine
 * CSCdi48797:  Extraneous definitions in interface_private.h
 *
 * Revision 3.10  1996/02/06  00:01:24  sberl
 * CSCdi47471:  CLS LLC2 sessions may use wrong RIF
 * Check address+RIF_DTE explicitly for 0x01 instead of depending on
 * RIF_LENGTH() macro.
 *
 * Revision 3.9  1996/02/05  02:06:56  mmcneali
 * CSCdi46205:  IPX packets are corrupted by vLAN 802.10 fast-switch.
 *
 * Revision 3.8  1996/01/25  23:11:19  gstovall
 * CSCdi47390:  down-when-looped command not working (regression)
 * Remember to reset the itemlist before tossing it up to the next level.
 *
 * Revision 3.7  1996/01/23  08:48:57  mmcneali
 * CSCdi47323:  vLAN encapsulated IP SSE switched.
 *
 * Faster Baby, Faster!
 *
 * Revision 3.6.14.1  1996/01/24  22:18:08  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.6  1995/12/07  22:41:45  hampton
 * Label the large chunks of memory allocated at system initialization
 * time.  This makes it easier to see where all the memory in router is
 * going.  [CSCdi45209]
 *
 * Revision 3.5  1995/12/06  18:29:46  gstovall
 * CSCdi44769:  Interface idb queues are not getting populated correctly
 * Make sure the hwidbs are in the same order on the media hwidbQs as on
 * the hwidbQ.
 *
 * Revision 3.4  1995/11/21  23:14:40  shaker
 * CSCdi44143:  FIP doesnt EOIR in 7500 since CSCdi42010 was applied
 * If system is not yet configured or if hotstall is still active when an
 * init_cmt() is normally done in fddi_reset(), fork a 'one shot' process
 * to do it later instead of leaving the FDDI interface dead.
 *
 * Revision 3.3  1995/11/17  09:27:34  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:37:38  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:49:39  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.6  1995/11/08  21:02:53  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.5  1995/09/09  00:52:27  ronnie
 * CSCdi35837:  ping works, but traceroute fails to go across ATM interface
 * Introduce a new variable type, fuzzy, for hwidb->broadcast to return
 * instead of boolean.  This allows the return of NOT_SURE.  Adjust all
 * hwidb->broadcast routines to return fuzzy instead of boolean.
 *
 * Revision 2.4  1995/08/07  05:36:37  richl
 * CSCdi38315:  makefile rework
 *              move inlines to tring_inline and utilities to rif_util.c
 *
 * Revision 2.3  1995/06/19  01:05:47  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.2  1995/06/13  03:28:09  alam
 * CSCdi35570:  Remove extraneous functions
 * Use llc_process_test and llc_process_xid for FDDI as well as ethernet
 * and FR.
 * Remove FDDI packet debug.
 *
 * Revision 2.1  1995/06/07  20:52:03  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/***********************************************************************
				Includes
***********************************************************************/
/* cisco includes */
#include "master.h"
#include "logger.h"
#include "../les/msg_fddi.c"		/* Not a typo, see logger.h */
#include "msg_datalink.c"		/* Not a typo, see logger.h */
#include "../xdi/cschdrsif.h"
#include "../if/ether.h"
#include "../if/tring.h"
#include "../if/network.h"
#include "../ui/debug.h"
#include "../srt/span.h"
#include "../srt/srb_core.h"
#include "../if/fddi_debug.h"
#include "media_registry.h"
#include "fastswitch_registry.h"
#include "subsys.h"
#include "tring.h"
#include "rif.h"
#include "arp.h"
#include "../srt/fddisrb.h"
#include "rif_inline.h"
#include "../ip/ip_registry.h"
#include "../util/itemlist.h"

/* XDI includes */
#include "../xdi/xdihdrsif.h"
#include "../xdi/fddihdr.h"
#include "../xdi/csphdr.h"
#include "../xdi/fbmhdr.h"
#include "../xdi/fbmframe.h"
#include "../xdi/smtmsg.h"
#include "../xdi/cspproto.h"
#include "../xdi/fbmproto.h"

#include "if_fddi.h"

#include "../tbridge/tbridge_sde.h"
#include "../llc2/llc2.h"

#include "../vlan/vlan.h"

#include "../xns/novell.h"
#include "../parser/parser_defs_parser.h"

/***********************************************************************
				Defines
***********************************************************************/


/***********************************************************************
				Types
***********************************************************************/


/***********************************************************************
				Prototypes
***********************************************************************/
/* --- external --- */
/* --- internal --- */


/***********************************************************************
				Variables
***********************************************************************/
/* --- internal --- */
StnDCBType cscStnDCB[MAX_STATION_COUNT];
queuetype smtbufQ;
int fddi_smtqsize;

/*
*	SMT configuration structures
*/
SMTCfgType SASSMTCfg = {
	{ 0x00, 0x00, 0x0C },			/* manufacturerOUI[3] */
	0,					/* pad1 */
	(uChar *)"       Cisco Systems, Inc.",	/* pmanufacturerData */
	(uChar *)CSC_STATION_STRING,		/* pUserData */
	Cap_None,				/* ConfigCapabilities */
	CFP_None,				/* ConfigPolicy */
	CON_CSC_DEFAULT,			/* ConnectionPolicy */
	T_Notify_Default,			/* T_Notify */
	Trace_Max_Default,			/* Trace_MaxExpiration */
	TRUE,					/* StatRptPolicy */
	FALSE,					/* BypassPresent */
	AP_primary,				/* AvailablePaths */
	0,					/* pad2 */
	{
	    RMT_DAC_LEAVE,			/* RMTDupCapabilities */
	    { 0, 0, 0 },	       		/* pad[3] */
	    PKG_manufacturerdata |		/* packages */
            PKG_parametermanagement |
            PKG_smtvendorspecific
	}
};

SMTCfgType DASSMTCfg = {
	{ 0x00, 0x00, 0x0C },			/* manufacturerOUI[3] */
	0,					/* pad1 */
	(uChar *)"       Cisco Systems, Inc.",	/* pmanufacturerData */
	(uChar *)CSC_STATION_STRING,		/* pUserData */
	Cap_None,				/* ConfigCapabilities */
	CFP_None,				/* ConfigPolicy */
	CON_CSC_DEFAULT,			/* ConnectionPolicy */
	T_Notify_Default,			/* T_Notify */
	Trace_Max_Default,			/* Trace_MaxExpiration */
	TRUE,					/* StatRptPolicy */
	FALSE,					/* BypassPresent */
	AP_primary | AP_secondary,		/* AvailablePaths */
	0,					/* pad2 */
	{
	    RMT_DAC_LEAVE,			/* RMTDupCapabilities */
	    { 0, 0, 0 },			/* pad[3] */
	    PKG_manufacturerdata |		/* packages */
            PKG_parametermanagement |
            PKG_smtvendorspecific
	}
};

/*
*	MAC configuration structures
*/
MACCfgType SingleBMACMACCfg = {
    1,						/* MacCt */
    { 0, 0 },					/* pad[2] */
    {
	{
	    PRIM_MAC_INDEX,			/* MacIndex */
	    TRUE,				/* MA_UnitdataEnable */
	    AP_primary,				/* AvailablePaths */
	    0,					/* FrameErrorThreshold */
	    0,					/* NotCopiedThreshold */
	    FS_None,				/* FrameStatusFunctions */
	    BT_None,				/* BridgeFunctions */
	    {
		{ 0, 0, 0 },			/* pad[3] */
		PP_primary_pref,       		/* RequestedPaths */
	    },
	    0xFF000000L,			/* T_MaxCapability */
	    0xFF000000L,			/* TVXCapability */
	    {
		PKG_framenotcopied |		/* packages */
		PKG_framenotcopiedmanagement |
		PKG_tvxexpired |
		PKG_ringopcount |
		PKG_longgrpaddress |
		PKG_shortgrpaddress |
		PKG_tokencounter |
		PKG_bridgefunctions |
		PKG_macvendorspecific
	    }
	}
    }
};

MACCfgType SingleFORMACMACCfg = {
    1,						/* MacCt */
    { 0, 0 },					/* pad[2] */
    {
	{
	    PRIM_MAC_INDEX,			/* MacIndex */
	    TRUE,				/* MA_UnitdataEnable */
	    AP_primary,				/* AvailablePaths */
	    0,					/* FrameErrorThreshold */
	    0,					/* NotCopiedThreshold */
	    FS_None,				/* FrameStatusFunctions */
	    BT_None,				/* BridgeFunctions */
	    {
		{ 0, 0, 0 },			/* pad[3] */
		PP_primary_pref,		/* RequestedPaths */
	    },
	    0xFFE00000L,			/* T_MaxCapability */
	    0xFFFF0000L,			/* TVXCapability */
	    {
		PKG_framenotcopied |		/* packages */
                PKG_framenotcopiedmanagement |
                PKG_tvxexpired |
                PKG_ringopcount |
                PKG_longgrpaddress |
                PKG_shortgrpaddress |
                PKG_tokencounter |
                PKG_bridgefunctions |
                PKG_macvendorspecific
	    }
	}
    }
};

/*
*	PATH Resources configuration structure
*/
PATHRESCfgType S_MAC_SPATHCfg = {
	2,					/* ResCt */
	{ 0, 0 },				/* pad[2] */
	{
	    {
		S_INDEX,			/* ResIndex */
		RT_port,			/* ResType */
		PRIM_MAC_INDEX,			/* OutResIndex */
		RT_mac,				/* OutResType */
		PRIM_MAC_INDEX,			/* InResIndex */
		RT_mac				/* InResType */
	    }, {
		PRIM_MAC_INDEX,			/* ResIndex */
		RT_mac,				/* ResType */
		S_INDEX,			/* OutResIndex */
		RT_port,			/* OutResType */
		S_INDEX,			/* InResIndex */
		RT_port				/* InResType */
	    }
	}
};

PATHRESCfgType A_MAC_BPATHCfg = {
	3,					/* ResCt */
	{ 0, 0 },				/* pad[2] */
	{
	    {
		A_INDEX,			/* ResIndex */
		RT_port,			/* ResType */
		PRIM_MAC_INDEX,			/* OutResIndex */
		RT_mac,				/* OutResType */
		B_INDEX,			/* InResIndex */
		RT_port				/* InResType */
	    }, {
		B_INDEX,			/* ResIndex */
		RT_port,			/* ResType */
		A_INDEX,			/* OutResIndex */
		RT_port,			/* OutResType */
		PRIM_MAC_INDEX,			/* InResIndex */
		RT_mac				/* InResType */
	    }, {
		PRIM_MAC_INDEX,			/* ResIndex */
		RT_mac,				/* ResType */
		B_INDEX,			/* OutResIndex */
		RT_port,			/* OutResType */
		A_INDEX,			/* InResIndex */
		RT_port				/* InResType */
	    }
	}
};

PATHRESCfgType B_APATHCfg = {
	2,					/* ResCt */
	{ 0, 0 },					/* pad[2] */
	{
	    {
		A_INDEX,			/* ResIndex */
		RT_port,			/* ResType */
		B_INDEX,			/* OutResIndex */
		RT_port,			/* OutResType */
		B_INDEX,			/* InResIndex */
		RT_port				/* InResType */
	    }, {
		B_INDEX,			/* ResIndex */
		RT_port,			/* ResType */
		A_INDEX,			/* OutResIndex */
		RT_port,			/* OutResType */
		A_INDEX,			/* InResIndex */
		RT_port				/* InResType */
	    }
	}
};

/*
*	PATH configuration structure
*/
PATHCfgType SASPATHCfg = {
    1,						/* PathCt */
    { 0, 0 },					/* pad[2] */
    {
	{
	    PI_primary,				/* PathIndex */
	    { 0, 0 },				/* pad[2] */
	    0xFFFF85EE,				/* TVXLowerBound */
	    0xFFE0875C,				/* T_MaxLowerBound */
	    DEFAULT_TRT_TIME,			/* MaxT_Req */
	    &S_MAC_SPATHCfg,			/* ResCfgPtr */
	    {
		PKG_tracestatus	|		/* packages */
                PKG_restrictedtoken |
                PKG_pathvendorspecific
	    }
	}
    }
};

PATHCfgType DASPATHCfg = {
    2,						/* PathCt */
    { 0, 0 },					/* pad[2] */
    {
	{
	    PI_primary,				/* PathIndex */
	    { 0, 0 },				/* pad[2] */
	    0xFFFF85EE,				/* TVXLowerBound */
	    0xFFE0875C,				/* T_MaxLowerBound */
	    DEFAULT_TRT_TIME,			/* MaxT_Req */
	    &A_MAC_BPATHCfg,			/* ResCfgPtr */
	    {
		PKG_tracestatus	|		/* packages */
                PKG_restrictedtoken |
		PKG_pathvendorspecific
	    }
	}, {
	    PI_secondary,			/* PathIndex */
	    { 0, 0 },				/* pad[2] */
	    0xFFFF85EE,				/* TVXLowerBound */
	    0xFFE0875C,				/* T_MaxLowerBound */
	    DEFAULT_TRT_TIME,			/* MaxT_Req */
	    &B_APATHCfg,			/* ResCfgPtr */
	    {
		PKG_tracestatus	|		/* packages */
                PKG_restrictedtoken |
                PKG_pathvendorspecific
	    }
	}
    }
};

/*
*	PORT configuration structures
*/
PORTCfgType MSASPORTCfg = {
    1,						/* PortCt */
    { 0, 0 },					/* pad[2] */
    {
	{
	    S_INDEX,				/* PortIndex */
	    PT_s,				/* My_Type */
	    CP_None,				/* ConnectionPolicies */
	    CP_None,				/* ConnectionCapabilities */
	    LER_CUTOFF_DEFAULT,			/* Ler_Cutoff */
	    LER_ALARM_DEFAULT,			/* Ler_Alarm */
	    LS_qls,				/* Maint_LS */
	    PMD_multimode,			/* PMDClass */
	    {
		0,				/* pad */
		{
		    RP_S_NONE_DEFAULT,		/* RequestedPaths[RP_none] */
		    RP_S_TREE_DEFAULT,		/* RequestedPaths[RP_tree] */
		    RP_S_PEER_DEFAULT		/* RequestedPaths[RP_peer] */
		}
	    },
	    0,					/* MacLoop_Time */
	    AP_primary,				/* AvailablePaths */
	    { 0, 0, 0 },			/* pad[3] */
	    {
		PKG_maint_LS |			/* packages */
                PKG_maint_LSmanagement |
                PKG_portvendorspecific
	    }
	}
    }
};

PORTCfgType MMDASPORTCfg = {
    2,						/* PortCt */
    { 0, 0 },					/* pad[2] */
    {
	{
	    A_INDEX,				/* PortIndex */
	    PT_a,				/* My_Type */
	    CP_None,				/* ConnectionPolicies */
	    CP_None,				/* ConnectionCapabilities */
	    LER_CUTOFF_DEFAULT,			/* Ler_Cutoff */
	    LER_ALARM_DEFAULT,			/* Ler_Alarm */
	    LS_qls,				/* Maint_LS */
	    PMD_multimode,			/* PMDClass */
	    {
		0,				/* pad */
		{
		    RP_A_NONE_DEFAULT,		/* RequestedPaths[RP_none] */
		    RP_A_TREE_DEFAULT,		/* RequestedPaths[RP_tree] */
		    RP_A_PEER_DEFAULT		/* RequestedPaths[RP_peer] */
		}
	    },
	    0,					/* MacLoop_Time */
	    AP_primary | AP_secondary,		/* AvailablePaths */
	    { 0, 0, 0 },			/* pad[3] */
	    {
		PKG_maint_LS |			/* packages */
                PKG_maint_LSmanagement |
                PKG_portvendorspecific
	    }
	}, {
	    B_INDEX,				/* PortIndex */
	    PT_b,				/* My_Type */
	    CP_None,				/* ConnectionPolicies */
	    CP_None,				/* ConnectionCapabilities */
	    LER_CUTOFF_DEFAULT,			/* Ler_Cutoff */
	    LER_ALARM_DEFAULT,			/* Ler_Alarm */
	    LS_qls,				/* Maint_LS */
	    PMD_multimode,			/* PMDClass */
	    {
		0,				/* pad */
		{
		    RP_B_NONE_DEFAULT,		/* RequestedPaths[RP_none] */
		    RP_B_TREE_DEFAULT,		/* RequestedPaths[RP_tree] */
		    RP_B_PEER_DEFAULT_SM	/* RequestedPaths[RP_peer] */
		}
	    },
	    0,					/* MacLoop_Time */
	    AP_primary | AP_secondary,		/* AvailablePaths */
	    { 0, 0, 0 },			/* pad[3] */
	    {
		PKG_maint_LS |			/* packages */
                PKG_maint_LSmanagement |
                PKG_portvendorspecific
	    }
	}
    }
};

PORTCfgType SSDASPORTCfg = {
    2,						/* PortCt */
    { 0, 0 },					/* pad[2] */
    {
	{
	    A_INDEX,				/* PortIndex */
	    PT_a,				/* My_Type */
	    CP_None,				/* ConnectionPolicies */
	    CP_None,				/* ConnectionCapabilities */
	    LER_CUTOFF_DEFAULT,			/* Ler_Cutoff */
	    LER_ALARM_DEFAULT,			/* Ler_Alarm */
	    LS_qls,				/* Maint_LS */
	    PMD_single_mode1,			/* PMDClass */
	    {
		0,				/* pad */
		{
		    RP_A_NONE_DEFAULT,		/* RequestedPaths[RP_none] */
		    RP_A_TREE_DEFAULT,		/* RequestedPaths[RP_tree] */
		    RP_A_PEER_DEFAULT		/* RequestedPaths[RP_peer] */
		}
	    },
	    0,					/* MacLoop_Time */
	    AP_primary | AP_secondary,		/* AvailablePaths */
	    { 0, 0, 0 },			/* pad[3] */
	    {
		PKG_maint_LS |			/* packages */
                PKG_maint_LSmanagement |
                PKG_portvendorspecific
	    }
	}, {
	    B_INDEX,				/* PortIndex */
	    PT_b,				/* My_Type */
	    CP_None,				/* ConnectionPolicies */
	    CP_None,				/* ConnectionCapabilities */
	    LER_CUTOFF_DEFAULT,			/* Ler_Cutoff */
	    LER_ALARM_DEFAULT,			/* Ler_Alarm */
	    LS_qls,				/* Maint_LS */
	    PMD_single_mode1,			/* PMDClass */
	    {
		0,				/* pad */
		{
		    RP_B_NONE_DEFAULT,		/* RequestedPaths[RP_none] */
		    RP_B_TREE_DEFAULT,		/* RequestedPaths[RP_tree] */
		    RP_B_PEER_DEFAULT_SM	/* RequestedPaths[RP_peer] */
		}
	    },
	    0,					/* MacLoop_Time */
	    AP_primary | AP_secondary,		/* AvailablePaths */
	    { 0, 0, 0 },			/* pad[3] */
	    {
		PKG_maint_LS |			/* packages */
                PKG_maint_LSmanagement |
                PKG_portvendorspecific
	    }
	}
    }
};
/*
* SMT print-out strings data structure
*/
SMTMsgType SMTMsg = {
	{
	    EC_STATE_MAXID,			/* ECM State */
	    {
		"out", "in", "trace", "leave", "path_test",
		"insert", "check", "deinsert"
	    }
	}, {
	    EC_SIGNAL_MAXID,			/* ECM Signal */
	    {
		"", "Connect", "Disconnect", "TrProp", "PathTest"
	    }
	}, {
	    PC_STATE_MAXID,			/* PCM State */
	    {
		"off", "break", "trace", "connect", "next",
		"signal", "join", "verify", "active", "maintenance"
	    }
	}, {
	    CF_STATE_MAXID,			/* CFM State */
	    {
		"isolated", "local_a", "local_b", "local_ab",
		"local_s", "wrap_a", "wrap_b", "wrap_ab",
		"wrap_s", "c_wrap_a", "c_wrap_b", "c_wrap_s",
		"thru"
	    }
	}, {
	    RM_STATE_MAXID,			/* RMT State */
	    {
		"isolated", "non_op", "ring_op", "detect",
		"non_op_dup", "ring_op_dup", "directed", "trace"
	    }
	}, {
	    PHY_TYPE_MAXID,			/* PHY Type */
	    {
		"A", "B", "S", "M"
	    }
	}, {
	    MAC_TYPE_MAXID,			/* MAC Type */
	    {
		"P", "S"
	    }
	}
};

/*
 * Fddi state/config vectors.
 */
static boolean
fddi_hw_set_config (hwidbtype *hwidb, itemlist *ilist, fddisb *fsb)
{
    itemdesc	*item;
    boolean	any_unhandled = FALSE;	/* assume we've handled all items */
    tinybool	this_item_unhandled;

    while ((item = itemlist_getnext(ilist))) {
	if (item->handled)
	    continue;
	this_item_unhandled = FALSE;
	switch (item->item) {
	case FDDI_HW_TVX:
	    fsb->fddi_tvx = item->u.l_value;
	    break;

	case FDDI_HW_TRT:
	    fsb->fddi_trt = item->u.l_value;
	    break;

	case FDDI_HW_TRT_NEG:
	    fsb->fddi_trt_neg = item->u.l_value;
	    break;

	case FDDI_HW_BURST_COUNT:
	    fsb->fddi_burst_count = item->u.l_value;
	    break;

	default:
	    this_item_unhandled = TRUE;
	    any_unhandled = TRUE;
	    break;
	}
	if (!this_item_unhandled)
	    item->handled = TRUE;
    }
    return (!any_unhandled);
}

static boolean
fddi_hw_read_config (hwidbtype *hwidb, itemlist *ilist, fddisb *fsb)
{
    itemdesc	*item;
    boolean	any_unhandled = FALSE;	/* assume we've handled all items */
    tinybool	this_item_unhandled;

    while ((item = itemlist_getnext(ilist))) {
	if (item->handled)
	    continue;
	this_item_unhandled = FALSE;
	switch (item->item) {
	case FDDI_HW_TVX:
	    item->u.l_value= fsb->fddi_tvx;
	    item->length = sizeof(ulong);
	    break;

	case FDDI_HW_TRT:
	    item->u.l_value = fsb->fddi_trt;
	    item->length = sizeof(ulong);
	    break;

	case FDDI_HW_BURST_COUNT:
	    item->u.l_value = fsb->fddi_burst_count;
	    item->length = sizeof(int);
	    break;

	case FDDI_HW_TRT_NEG:
	    item->u.l_value = fsb->fddi_trt_neg;
	    item->length = sizeof(ulong);
	    break;

	default:
	    this_item_unhandled = TRUE;
	    any_unhandled = TRUE;
	    break;
	}
	if (!this_item_unhandled)
	    item->handled = TRUE;
    }
    return (!any_unhandled);
}

static boolean
fddi_hw_set_state (hwidbtype *hwidb, itemlist *ilist, fddisb *fsb)
{
    itemdesc	*item;
    boolean	any_unhandled = FALSE;	/* assume we've handled all items */
    tinybool	this_item_unhandled;

    while ((item = itemlist_getnext(ilist))) {
	if (item->handled)
	    continue;
	this_item_unhandled = FALSE;
	switch (item->item) {

	case FDDI_HW_CLAIMS:
	    fsb->fddi_claims = item->u.l_value;
	    break;

	case FDDI_HW_BEACONS:
	    fsb->fddi_beacons =  item->u.l_value;
	    break;

	case FDDI_HW_TRACE_COUNT:
	    fsb->fddi_trace_count = item->u.l_value;
	    break;

	case FDDI_HW_SI_CLAIMS:
	    fsb->si_fddi_claims = item->u.l_value;
	    break;

	case FDDI_HW_SI_BEACONS:
	    fsb->si_fddi_beacons = item->u.l_value;
	    break;

	case FDDI_HW_SI_TRACE_COUNT:
	    fsb->si_fddi_trace_count = item->u.l_value;
	    break;

	case FDDI_HW_FRINC:
	    fsb->fddi_frinc = item->u.l_value;
	    break;

	case FDDI_HW_CFM_STATE:
	    fsb->cfm_state = item->u.s_value[0];
	    break;

	case FDDI_HW_SHUTDOWN_REASONS:
	    fsb->fddi_shutdown_reasons = item->u.s_value[0];
	    break;

	case FDDI_HW_CLEAR_STATS:
	    fsb->si_fddi_claims = fsb->fddi_claims;
	    fsb->si_fddi_beacons = fsb->fddi_beacons;
	    fsb->si_fddi_trace_count = fsb->fddi_trace_count;
	    break;

	default:
	    this_item_unhandled = TRUE;
	    any_unhandled = TRUE;
	    break;
	}
	if (!this_item_unhandled)
	    item->handled = TRUE;
    }
    return (!any_unhandled);
}
static boolean
fddi_hw_read_state (hwidbtype *hwidb, itemlist *ilist, fddisb *fsb)
{
    itemdesc	*item;
    boolean	any_unhandled = FALSE;	/* assume we've handled all items */
    tinybool	this_item_unhandled;

    while ((item = itemlist_getnext(ilist))) {
	if (item->handled)
	    continue;
	this_item_unhandled = FALSE;
	switch (item->item) {

	case FDDI_HW_CLAIMS:
	    item->u.l_value = fsb->fddi_claims;
	    item->length = sizeof(ulong);
	    break;

	case FDDI_HW_BEACONS:
	    item->u.l_value = fsb->fddi_beacons;
	    item->length = sizeof(ulong);
	    break;

	case FDDI_HW_TRACE_COUNT:
	    item->u.l_value = fsb->fddi_trace_count;
	    item->length = sizeof(ulong);
	    break;

	case FDDI_HW_SI_CLAIMS:
	    item->u.l_value = fsb->si_fddi_claims;
	    item->length = sizeof(ulong);
	    break;

	case FDDI_HW_SI_BEACONS:
	    item->u.l_value = fsb->si_fddi_beacons;
	    item->length = sizeof(ulong);
	    break;

	case FDDI_HW_SI_TRACE_COUNT:
	    item->u.l_value = fsb->si_fddi_trace_count;
	    item->length = sizeof(ulong);
	    break;

	case FDDI_HW_FRINC:
	    item->u.l_value = fsb->fddi_frinc;
	    item->length = sizeof(ulong);
	    break;

	case FDDI_HW_CFM_STATE:
	    item->u.s_value[0] = fsb->cfm_state;
	    item->length = sizeof(ushort);
	    break;

	case FDDI_HW_OPERATIONAL:
	    item_put_timestamp(ilist, item, fsb->fddi_operational);
	    break;

	case FDDI_HW_SHUTDOWN_REASONS:
	    item->u.s_value[0] = fsb->fddi_shutdown_reasons;
	    item->length = sizeof(ushort);
	    break;

	case FDDI_HW_CLAIMS_STAT:
	    item->u.l_value = fsb->fddi_claims - fsb->si_fddi_claims;
	    item->length = sizeof(ulong);
	    break;

	case FDDI_HW_BEACONS_STAT:
	    item->u.l_value = fsb->fddi_beacons - fsb->si_fddi_beacons;
	    item->length = sizeof(ulong);
	    break;

	case FDDI_HW_TRACE_COUNT_STAT:
	    item->u.l_value = fsb->fddi_trace_count - fsb->si_fddi_trace_count;
	    item->length = sizeof(ulong);
	    break;

	default:
	    this_item_unhandled = TRUE;
	    any_unhandled = TRUE;
	    break;
	}
	if (!this_item_unhandled)
	    item->handled = TRUE;
    }
    return (!any_unhandled);
}

static boolean
fddi_hwidb_state_config (hwidbtype *hwidb, ulong opcode, itemlist *ilist)
{
    boolean	ret;
    fddisb	*fsb;

    if (!(hwidb && ilist))
	return (FALSE);

    if (ilist->item_count == -1)
	ilist->item_count = itemlist_count(ilist);

    fsb = idb_use_hwsb_inline(hwidb, HWIDB_SB_FDDI);
    if (fsb == NULL)
	return (FALSE);

    switch (opcode) {
    case IDB_CONTROL_SET_CONFIG:
	ret = fddi_hw_set_config(hwidb, ilist, fsb);
	break;

    case IDB_CONTROL_READ_CONFIG:
	ret = fddi_hw_read_config(hwidb, ilist, fsb);
	break;

    case IDB_CONTROL_SET_STATE:
	ret = fddi_hw_set_state(hwidb, ilist, fsb);
	break;

    case IDB_CONTROL_READ_STATE:
	ret = fddi_hw_read_state(hwidb, ilist, fsb);
	break;

    default:
	ret = FALSE;
	break;
    }
    idb_release_hwsb_inline(hwidb, HWIDB_SB_FDDI);

    if (!ret) {
	itemlist_reset(ilist);
	ret = (*fsb->super_state_config)(hwidb, opcode, ilist);
    }
    return (ret);
}

/*
 * fddi_extract_addr
 *
 * Given an FDDI packet, extract the requested MAC level address into a
 * hwaddrtype structure.  Assumes pak->mac_start is set.
 */
static boolean fddi_extract_addr (
    paktype *pak,
    hwaddrtype *address,
    enum SRCDST which)
{
    fddi_hdr *addrs = NULL;

    /*
     * This is dlsw/fddi, extract it in token ring format.
     */
    if (pak->enc_flags & ENC_DLSW) {
        tr_extract_addr(pak, address, which);
        return (TRUE);
    }
    addrs = (fddi_hdr *)pak->mac_start;
    address->type = STATION_IEEE48;
    address->length = STATIONLEN_IEEE48;
    address->target_type = TARGET_UNICAST;
    switch (which) {
      case SOURCE:
	ieee_copy(addrs->saddr, address->addr);
 	if (pak->if_input->hwptr->type == IDBTYPE_FDDIT)
 	  address->addr[0] &= ~FDDIT_RII;
 	else
 	  address->addr[0] &= ~FDDI_RII;
	break;
      case DESTINATION:
	ieee_copy(addrs->daddr, address->addr);
	break;
    }

    if (pak->if_input->hwptr->type != IDBTYPE_FDDIT)
	ieee_swap(address->addr, address->addr);

    if (is_ieee_bcast(address->addr))
	address->target_type = TARGET_BROADCAST;
    else if (ieee_equal(esis_8023_all_is_multi, address->addr))
	address->target_type = TARGET_OSI_ALL_IS;
    else if (ieee_equal(esis_8023_all_es_multi, address->addr))
	address->target_type = TARGET_OSI_ALL_ES;

    return(TRUE);
}

/*
 * fddi_make_addr_native
 *
 * Given a MAC address in canonical form, modify the MAC address to be
 * in native (to the controller) bit order.
 */
static void fddi_make_addr_native (hwidbtype *idb, uchar *addr)
{
    if (idb->type != IDBTYPE_FDDIT)
	ieee_swap(addr, addr);
}

/*
 * fddi_parse_packet
 *
 * Set up a packet buffer so that the network level protocol fields
 * are initialized.
 *
 * If data is NULL, pak->datagramstart is assumed to point to the
 * start of the frame to parse. No attempt will be made to align
 * the frame to The Line.
 *
 * WARNING: It it up to the calling routine to insure that the
 * (potential) snap header is in a location that it *byte* accessible.
 * For MCI/Cbus devices, this means reading this information off of the
 * controller and into local memory.  THe same rules apply for all other
 * drivers.
 */

static iqueue_t fddi_parse_packet (
    hwidbtype *idb,
    paktype *pak,
    uchar *data)
{
    int		info_size=0, riflen=0;
    snap_hdr	*snap=NULL;
    ushort	dsap_ssap;

    /*
     * Check to see if building a frame in a virgin packet, or whether
     * we're just setting up media pointers into the frame
     */
    if (data) {
	/*
	 * We've got a virgin packet. 
	 */
	snap = (snap_hdr *)data;
	riflen = idb->rxrifwordlen * 2;
    } else {
	/*
	 * Soiled packet. Start from datagramstart
	 */
	pak->riflen = riflen = get_fddi_riflen_pak(pak,
		      idb->type == IDBTYPE_FDDIT ? FDDIT_RII : FDDI_RII);
	pak->rif_start = pak->datagramstart + FDDI_8025_ENCAPBYTES;
	snap = (snap_hdr *)(pak->rif_start + riflen);
    }

    /*
     * CSCdi16607
     * Novell Kludge: We want to bridge novell-ether over fddi.
     *   This is not LLC type 1 - so we don't assign it as ET_SAP
     *   This is not LLC type 2, either. We will drop it later if
     *   we assign it as ET_LLC2.
     *
     * NetBIOS : The packet got here because we don't have a
     *   NetBIOS stack. This is a LLC type 2 packet but we don't
     *   want to pass it to LLC2 and drop it - we want to bridge it.
     *
     * ET_NOVELL_ETHER and ET_UNSUPPORTED are not supported -
     *   Those two kinds of packets will be classified as
     *   LINK_ILLEGAL as linktype, and will be bridged.
     *
     * Check dsap AS WELL AS ssap here, and this is what we should do.
     *   dsap is not necessarily equal to ssap.
     *   Retain the code which check dsap only.
     */
    dsap_ssap = GETSHORT(&snap->dsap);
    if (dsap_ssap == SNAPSNAP) {
	if (GETSHORT(&snap->type) == TYPE_APOLLO) {
	    pak->enctype = ET_APOLLO;
	    info_size = SNAP_HDRBYTES + APOLLO_EXTRABYTES;
	} else {
	    pak->enctype = ET_SNAP;
	    info_size = SNAP_HDRBYTES;
	}
    } else if (dsap_ssap == SAPSAP_NOVELL_ETHER) {
        pak->enctype = ET_NOVELL_FDDI;
        info_size = 0;
    } else if (dsap_ssap == SAPSAP_NETBIOS) {
        pak->enctype = ET_UNSUPPORTED;
        info_size = SAP_HDRBYTES;
    } else {
	switch (snap->dsap) {
	case SAP_SNAP:
	    if (GETSHORT(&snap->type) == TYPE_APOLLO) {
		pak->enctype = ET_APOLLO;
		info_size = SNAP_HDRBYTES + APOLLO_EXTRABYTES;
	    } else {
		pak->enctype = ET_SNAP;
		info_size = SNAP_HDRBYTES;
	    }
	    break;
	    
	case SAP_CLNS:
	    pak->enctype = ET_SAP;
	    info_size = SAP_HDRBYTES;
	    break;
	    
        case SAP_SDE:
            pak->enctype = ET_SDE;
            info_size = SDE_ENCAPBYTES;
            break;

	case SAP_IBMNM:
	case SAP_CONS:
	default:
	    pak->llc2_sapoffset = FDDI_8025_ENCAPBYTES + riflen;
	    if ((snap->control & ~LLC1_P) == LLC1_UI) {
		pak->enctype = ET_SAP;
		info_size = SAP_HDRBYTES;
	    } else if ( (dsap_ssap == SAPSAP_NULL) && 
		       (snap->control == 0) ) { /* Some terminal servers use */
					        /* this - CSCdi20448 */
		pak->enctype = ET_SAP;
		info_size = SAP_HDRBYTES;
	    } else {
		pak->enctype = ET_LLC2;
		info_size = SAP_LONGHDRBYTES;
	    }
	    break;
	}
    }

    /*
     * If we're building a virgin frame, we have to set datagramstart
     */
    if (data) {
	pak->datagramstart = pak_center(pak) - 
	    FDDI_8025_ENCAPBYTES - riflen - info_size;
    }
    pak->mac_start     = pak->datagramstart;
    pak->addr_start    = pak->mac_start + FDDI_FC;
    if (riflen) {
	pak->rif_start = pak->mac_start + FDDI_8025_ENCAPBYTES;
    } else {
	pak->rif_start     = NULL;
    }
    
    pak->riflen = riflen;

    if (info_size || (pak->enctype == ET_NOVELL_FDDI)) {
	pak->info_start    = pak->mac_start  + FDDI_8025_ENCAPBYTES +
	    riflen;
    } else {
	pak->info_start = NULL;
    }

    pak->network_start = pak->info_start + info_size;

    /*
     * Record just how big this encapsulation was rather than every protocol
     * have it's own fun and wacky way to work it out.
     */
    pak->encsize = pak->network_start - pak->datagramstart;

    return(raw_enqueue);
}

/*
 * fddi_broadcast
 * Return true if the datagram is a physical broadcast.
 */
static fuzzy fddi_broadcast (hwidbtype *hwidb, paktype *pak)
{
    fddi_hdr *fddihdr;
    
    fddihdr = (fddi_hdr *)pak->datagramstart;
    return(is_ieee_bcast(fddihdr->daddr) ? FUZ_TRUE : FUZ_FALSE);
}

/*
 * fddi_getlink
 *
 * Return the generic link code for a FDDI datagram.
 */
static long fddi_getlink (paktype *pak)
{
    sap_hdr *sap;
    snap_hdr *snap;
    
    switch (pak->enctype) {
      case ET_SAP:
	sap = (sap_hdr *)pak->info_start;
	pak->llc2_cb = NULL;	/* indicate UI for LLC2 */
	return(sap2link(sap->dsap));

      case ET_SNAP:
	snap = (snap_hdr *)pak->info_start;
	return(type2link(GETSHORT(&snap->type)));

      case ET_LLC2:
	return(sap2link(pak->llc2_sap));

      case ET_APOLLO:      return(LINK_APOLLO);

      case ET_NOVELL_FDDI: return(LINK_NOVELL);

      case ET_SDE:
        /*
         * ipx novell-fddi with sde encap
         */
          if (GETSHORT(pak->datagramstart + FDDI_ENCAPBYTES +
                       SDE_ENCAPBYTES) == NOVELL_NO_CHECKSUM) {
              pak->network_start  = (pak->datagramstart + SDE_ENCAPBYTES +
                                     FDDI_ENCAPBYTES);
              pak->info_start += SDE_ENCAPBYTES;
              return(LINK_NOVELL);
          } else {
              sap = (sap_hdr *)pak->info_start;
              return(sap2link(sap->dsap));
          }

      default:
	return(reg_invoke_media_getlink( pak->enctype,
			       pak->if_input, pak));
    }
    return(LINK_ILLEGAL);
}

/*
 * get_bridge_entry
 * Function to determine if a mac level address exists in the bridge database.
 */
static inline btetype *get_bridge_entry (
    idbtype *idb,
    uchar *mac_addr)
{
    ulong ultemp;
    spantype *span;
    btetype *mac;
    leveltype level;

    ultemp = mac_addr[4];
    ultemp ^= mac_addr[5];
    span = idb->span_ptr;
    if (span) {
	level = raise_interrupt_level(NETS_DISABLE);
	for (mac = span->bridge_tbl[ultemp]; mac; mac = mac->bte_next) {
	    if (ieee_equal(mac_addr, (uchar *) &mac->bte_mac_addr.sword[0])) {
		reset_interrupt_level(level);
		if ((mac->bte_action == BRIDGE_RECEIVE) && 
		    !(mac->bte_attributes & BTE_LAT))
		  return(NULL);
		return(mac);
	    }
	}
	reset_interrupt_level(level);
    }
    return(NULL);
}

/*
 * fddi_bridge_datagram
 * Determine how we should encapsulation an IP, ARP, or LAT datagram.
 * For IP, we need to look up the address using the ARP table. For
 * the other protocols, we just use the specified address.
 *
 * Note that this routine only makes sense to call if we are an
 * encapsulating fddi bridge.
 */

static uchar *fddi_bridge_datagram (paktype *pak, long address)
{
    btetype *mac;
    uchar *mac_dest;

    if (pak->linktype == LINK_IP) {
	mac_dest = reg_invoke_ip_transit_bridging(address, pak->if_output);
	if (!mac_dest)
	    return(NULL);
    } else {
       if ((pak->enctype == ET_BRIDGE_ENCAPS) &&
	   is_ieee_bcast((uchar *)address)) {
           /*
            * ET_BRIDGE_ENCAPS will only be set by the arp routines when we
            * wish to force transit bridge encapsulation no matter if
            * mac_dest is in the bridge table or not.  The call to
            * ieee_equal() is only a sanity check.  The only difference
            * between this encapsulation and the following is the use of
            * the bridge multicast address instead of something from the
            * bridge table.
            */
           if (ether_vencap(pak, address))
	       return(bit_swapped_transit_multicast);
	   return(NULL);
       }
       mac_dest = (uchar *) address;
    }

    /*
     * IP datagram AND we know the MAC level address of where to send it.
     * If the datagram is in the bridging database -- encapsulate it as
     * a transit bridging datagram, else return FALSE (FDDI encapsulation).
     */
    mac = get_bridge_entry(pak->if_output, mac_dest);
    if (mac) {
	pak->enctype =  ET_BRIDGE;
	if (ether_vencap(pak, address))
	    return((uchar *)&mac->bte_proxy_addr);
    }
    return(NULL);
}

/*
 * fddi_vencap
 *
 * Called by higher layers to encapsulate a datagram for the FDDI media.
 */
static boolean fddi_vencap (paktype *pak, long address)
{
    uchar *daddr = NULL;
    uchar const *other_info = zerosnap;
    int enctype = ET_SNAP;
    idbtype *interface;
    hwidbtype *hwidb;
    uchar source[IEEEBYTES], destination[IEEEBYTES], *saddr;
    uchar swapped_daddr[IEEEBYTES], swapped_saddr[IEEEBYTES];
    int indx;
    boolean canonical;
    sap_hdr *sap;
    snap_hdr *snap;
    fddi_hdr *fddi;
    riftype *rifptr;
    int riflen;
    riftype rif_local_copy;
    leveltype level;
    ulong rif_flags = RIF_NOT_PRESENT;

    /*
     * If datagramstart not setup, we have a bad caller
     */
    if (pak->datagramstart == NULL)
 	return(FALSE);
    
    interface = pak->if_output;
    hwidb = pak->if_output->hwptr;

    rifptr = NULL;
    canonical = (hwidb->type == IDBTYPE_FDDIT);

    /*
     * Do address resolution.  We want an encapsulation method, a
     * destination MAC address, and an optional pointer to any other
     * addressing information (presently any SNAP information).  We are
     * careful to reset pak->enctype with any new information.  If the
     * encapsulation returned is ET_NULL, it indicated that the packet
     * has already been fully encapsulated, and that this routine should
     * terminate now with a TRUE value.
     */ 
    if (!reg_invoke_media_fddi_macaddr(pak->linktype, pak, address, &enctype,
				       &daddr, &other_info, &rif_flags)) {
	if (!reg_used_media_fddi_macaddr(pak->linktype))
	    errmsg(&msgsym(BADMACREG, LINK), pak->if_output->namestring,
		   pak->linktype);
	return(FALSE);
    }
    if (enctype == ET_NULL)
	return(TRUE);
    pak->enctype = enctype;

    /*
     * Set up correct source addresses for later use.
     */
    if (pak->flags & PAK_LACK_OUT) {
	ieee_copy((uchar *)(&(daddr[LMAC_DTE])), source);
	/* Don't change source address, but build FCS */
	pak->flags &= ~PAK_SRB_OUT;
	pak->flags |= PAK_SRB_EXPLORER;
    } else {
	ieee_copy(hwidb->hardware, source);
    }

    /*
     * Look the daddr up in the rif cache to see how we should handle it.
     */
    level = raise_interrupt_level(NETS_DISABLE);
    if (rif_flags & RIF_IN_LLC_DTE) {
	if (*((uchar*)address + RIF_DTE) == 0x01) {
	    /* 
	     * Illegal rif control byte signals that riflength is
	     * really zero! Send no RIF.
	     */
	    riflen = 0;
	} else {
	    riflen = RIF_LENGTH(*((uchar*)address + RIF_DTE));
	    bcopy((uchar *)(address + RIF_DTE), 
		  &rif_local_copy.rif[0], 
		  riflen);
	}
	rif_local_copy.length = riflen;
	goto rif_found;

    } else  if (pak->flags & PAK_LACK_OUT) {
	ieee_swap((uchar *)address, swapped_daddr);
	ieee_swap((uchar *)daddr+TR_SA_OFFSET, swapped_saddr);
	rifptr = rif_lookup(swapped_daddr, hwidb, 
			    swapped_saddr, rif_flags);
    } else if (rif_flags & RIF_REQUIRED) {
	ieee_swap((uchar *)address, swapped_daddr);
	rifptr = rif_lookup(swapped_daddr, hwidb, NULL, rif_flags);
    } else if (rif_flags & RIF_PRESENT) {
	/*
	 * If not found then dump.  This may get us into trouble but to
	 * hang onto the packet now while doing route exploration has
	 * significant structural ramifications.  That is our caller
	 * expects for things to either work or not work.  We now have
	 * another possibility that of delayed.  Gag.  Because we are an
	 * idb function this potentially effects the entire driver layer.
	 *
	 * This routine will return the proper RIFs for broadcast,
	 * group, and functional addresses.
	 */
	ieee_swap(daddr, swapped_daddr);
	rifptr = rif_lookup_callback(swapped_daddr, hwidb, NULL,
				     rif_flags, pak->linktype,
				     interface, address);
	if (!rifptr) {
	    reset_interrupt_level(level);
	    return(FALSE);
	}
    } else {
	rifptr = NULL;
    }
    if (rifptr) {
	bcopy(rifptr, &rif_local_copy, sizeof(riftype));
    } else {
	memset(&rif_local_copy, 0, sizeof(riftype));
    }

 rif_found:
    rifptr = &rif_local_copy;
    riflen = rifptr->length;
    reset_interrupt_level(level);

    /*
     * Get the correct source and destination addresses.  This includes
     * swapping the bits in destination address, if the output is not a
     * canonical media.
     * The source MAC address may be the interface's MAC address, or it
     * may be supplied in the packet.  If the later, deal with swapping
     * the bit order here.  Also, only some controllers support FDDI
     * stripping, which is required in order to generate traffic whose source
     * MAC is other than the station's primary MAC.  (So special case check
     * for old FCI cards which do not.)
     */
    if (canonical) {
	ieee_copy(daddr, destination);
	if (pak->enc_flags & ENC_SRCMAC_OVERRIDE)
	    ieee_copy(pak->data_area, source);
        else if (pak->enc_flags & ENC_DLSW) {
            ieee_swap((uchar *)address, destination);
            ieee_swap((uchar *)daddr+TR_SA_OFFSET, source);
        }
	else
	    ieee_copy(hwidb->hardware, source);
    } else {
	for (indx = 0; indx < IEEEBYTES; indx++) 
	    destination[indx] = bit_swaps[*daddr++];
	if ((pak->enc_flags & ENC_SRCMAC_OVERRIDE) &&
	    (hwidb->type != IDBTYPE_FDDI)) {
	    saddr = pak->data_area;
	    for (indx = 0; indx < IEEEBYTES; indx++) 
		source[indx] = bit_swaps[*saddr++];
	} else {
	    ieee_copy(hwidb->bit_swapped_hardware, source);
	}
    }

    /*
     * We know the destination FDDI address, now encapsulate the datagram
     * according to method.  Quit if no method has emerged by now.
     */
    if (!enctype || !daddr)
	return(FALSE);

    switch (enctype) {

      case ET_SAP:
	pak->datagramstart -= (FDDI_SAP_ENCAPBYTES + riflen);
	pak->datagramsize += (FDDI_SAP_ENCAPBYTES + riflen);
	fddi = (fddi_hdr *)pak->datagramstart;
	ieee_copy(destination, fddi->daddr);
	ieee_copy(source,      fddi->saddr);
	if (riflen > 0) {
	    if (hwidb->type == IDBTYPE_FDDIT)
		fddi->saddr[0] |= FDDIT_RII;	/* indicate there is a RIF */
	    else
		fddi->saddr[0] |= FDDI_RII;
	    bcopy(rifptr->rif, fddi->data, riflen);
	}

	sap = (sap_hdr *)(&fddi->data[riflen]);
	sap->dsap    = other_info[0];
	sap->ssap    = other_info[1];
	sap->control = other_info[2];

	pak->mac_start  = (uchar *)fddi;
	pak->addr_start = fddi->daddr;
	pak->rif_start  = (riflen ? fddi->data : NULL);
	pak->info_start = (uchar *)sap;
	pak->encsize    = FDDI_SAP_ENCAPBYTES + riflen;
	break;

      case ET_SNAP:
      case ET_SDE:

        /*
         * For traffic we generate (e.g. Routing Protocol updates) through
         * dot10 vLAN configured FDDI interfaces (i.e. "ET_SDE"), we assume
         * them to start out as SNAP encapsulated and will then rewrite the
         * header as appropriate below ...
         */

	pak->datagramsize += (FDDI_SNAP_ENCAPBYTES + riflen);
	pak->datagramstart -= (FDDI_SNAP_ENCAPBYTES + riflen);
	fddi = (fddi_hdr *)pak->datagramstart;

	ieee_copy(destination, fddi->daddr);
	ieee_copy(source,      fddi->saddr);
	if (riflen > 0) {
	    if (hwidb->type == IDBTYPE_FDDIT)
		fddi->saddr[0] |= FDDIT_RII;
	    else
		fddi->saddr[0] |= FDDI_RII;	/* indicate we are here */
	    bcopy(rifptr->rif, fddi->data, riflen);
	}
	snap = (snap_hdr *)(&fddi->data[riflen]);
	snap->dsap    = SAP_SNAP;
	snap->ssap    = SAP_SNAP;
	snap->control = LLC1_UI;
	snap->oui[0]  = other_info[0];
	snap->oui[1]  = other_info[1];
	snap->oui[2]  = other_info[2];
	PUTSHORT(&snap->type, link2snaptype(pak->linktype));

	pak->mac_start  = (uchar *)fddi;
	pak->addr_start = fddi->daddr;
	pak->rif_start  = (riflen ? fddi->data : NULL);




	pak->info_start = fddi->data;
	pak->encsize    = FDDI_SNAP_ENCAPBYTES + riflen;
	break;
	
      default:
	if (!reg_invoke_media_fddi_macwrite(enctype, pak, hwidb, pak->linktype,
					    destination, source,
					    other_info, rifptr)) {

	    errmsg(&msgsym(BADENCAP, LINK), pak->if_output->namestring,
		   enctype);
	    return(FALSE);
	}
	break;
    }
    
    /*
     * Now add in the FC byte.  Doing it here means that all the private
     * media routines don't have to worry about getting it right.
     */
    *pak->datagramstart = FDDI_LLC_FC_BYTE;

    /*
     * Now check to see if the outgoing interface is actually a VLAN interface
     * If so then build a dot10 packet, (currently ISL not supported on FDDI).
     * Note the encapsulation at this point will be ARPA (Ethernet V2) which
     * becomes the interior packet's encapsulation, this is as expected.
     *
     * Increase the encapsulation size as used by the cache update routines
     * to figure out how much MAC to prepend.
     *
     * Note this is a generic Process Switched path routine, so need to use
     * a fast registry call, which enforces pointer arguments :-(
     */

    if (IS_DOT10_VLAN_SUBINTERFACE(interface)) {

	pak->addr_start = pak->datagramstart + FDDI_FC;
	
	if (pak->linktype == LINK_NOVELL) {
	  /* 
	   * we need to send raw fddi packet to support the default
           * Novell-Ether on ether interface.
           */
	    pak->datagramsize -= SNAP_HDRBYTES;
	    pak->encsize -= SNAP_HDRBYTES;
	    pak->datagramstart -= (SDE_ENCAPBYTES - SNAP_HDRBYTES);

	} else {
	    pak->datagramstart -= SDE_ENCAPBYTES;
	}
	

        *pak->datagramstart = FDDI_LLC_FC_BYTE;

        if (!reg_invoke_write_dot10_encapsulation(pak, interface))
            return(FALSE);

        reg_invoke_vlan_packet_counters(pak, ET_SDE);

        pak->datagramsize += SDE_ENCAPBYTES;
        pak->encsize += SDE_ENCAPBYTES;
    }

    /*
     * Possibly add the entry to the fast switching cache(s).
     */

    reg_invoke_fs_cache_update(pak->linktype, pak, 0L);
    return(TRUE);
}


/*
 * loop_macaddr_fddi
 *
 * Turn an Fddinet loopback address into an encapsulation type and
 * a MAC address. Called by SERVICE_FDDI_MACADDR, indexed by LINK_LOOP.
 */
static boolean loop_macaddr_fddi (paktype *pak, long address, int *enctype,
				  uchar **daddr, uchar const **snapaddr,
				  ulong *rif_flags)
{
    *enctype = ET_SNAP;
    *daddr = (uchar *)address;
    *snapaddr = NULL;
    return(TRUE);
}

/*
 * Get the max MTU for a particular protocol.
 *
 * The default is the max MTU of the interface for all protocols except IP;
 * for IP the default is the RFC1188 value.
 */

static int fddi_max_proto_mtu (idbtype *swidb, link_t linktype,
			       boolean get_default)
{
    int value;

    value = reg_invoke_media_fddi_mtu(linktype, swidb, get_default);
    return(value ? value : swidb->hwptr->maxmtu);
}


/*
 * fddi_idb_enqueue()
 * Add an idb to the FDDI idb list
 */
void fddi_idb_enqueue (hwidbtype *hwidb)
{
    ulong idblist_type;

    if (!(hwidb->status & IDB_FDDI)) return;

    /*
     * Check for fddi hwidb list if not there, create it.
     */
    if (idb_get_listhead(HWIDBLIST_FDDI) == NULL) {
	if (!idb_create_list(HWIDBLIST_FDDI, &idblist_type) ||
	    idblist_type != HWIDBLIST_FDDI) {
	    if (fddi_debug) {
		idb_report_create_error("FDDI");
	    }
	    return;
	}
    }

    /*
     * Add the idb to the fddi idb list
     */
    if (!idb_add_hwidb_to_list(hwidb, HWIDBLIST_FDDI)) {
	if (fddi_debug) {
	    idb_report_add_error("FDDI");
	}
    }
}


/*
 * fddi_idb_dequeue()
 * Dequeue an idb from the FDDI idb list
 */
void fddi_idb_dequeue (hwidbtype *hwidb)
{
    if (!idb_remove_from_list(hwidb, HWIDBLIST_FDDI)) {
	if (fddi_debug) {
	    idb_report_removal_error("FDDI");
	}
    }
}


/*
 * fddi_idb_init
 *
 * Initialize IDB with default values for FDDI media.
 */
void fddi_idb_init (hwidbtype *idb)
{
    idbtype *swidb = idb->firstsw;
    fddisb *fsb;
    hwidb_sb_t type = HWIDB_SB_FDDI;

    /*
     * Create the subblock, if needed
     */
    fsb = idb_get_hwsb_inline(idb, HWIDB_SB_FDDI);
    if (fsb == NULL) {
	fsb = malloc(sizeof(fddisb));
	if (fsb == NULL) {
	    /*
	     * This is way bad, we have no memory, crash and burn
	     */
	    crashdump(0);
	}
	
	if (!idb_add_hwsb(idb, &type, fsb))
	    return;
    }

    /*
     * Set the fastswitch flavor
     */
    idb->fast_switch_type = FS_FDDI;

    GET_TIMESTAMP(idb->state_time);
    idb->vencap = fddi_vencap;
    idb->deencap = NULL;
    idb->bridge_vencap = fddi_bridge_datagram;
    idb->getlink = fddi_getlink;
    set_default_queueing(idb);
    idb->iqueue = raw_enqueue;
    idb->parse_packet = fddi_parse_packet;
    idb->extract_hwaddr = ieee_extract_hwaddr;
    idb->extract_snpa = fddi_extract_addr;
    idb->make_snpa_native = fddi_make_addr_native;
    idb->broadcast = fddi_broadcast;
    idb->show_support = fddi_interface;
    holdq_init(idb, FDDI_HOLDQ);
    idb->maxmtu = idb->maxmtu_def = MAXSNAPFDDISIZE;
    idb->firstsw->sub_mtu = idb->maxmtu_def;

    idb->max_pak_size_def = FDDI_MAXDGRAM;
    if_maxdgram_adjust(idb, FDDI_MAXDGRAM);

    /*
     * If the state config isn't set up yet, do so now
     */
    if (idb->state_config != fddi_hwidb_state_config) {
	fsb->super_state_config = idb->state_config;
	idb->state_config = fddi_hwidb_state_config;
    }

    /*
     * The default encapsulation on FDDI is SNAP.
     */
    idb->enctype = idb->enctype_def = idb->enctype_cfg = ET_SNAP;
    idb->name = IFNAME_FDDI;			/* set preferred device name */
    set_default_interface_bandwidth(idb->firstsw, VISIBLE_FDDI_BANDWIDTH);
    set_default_interface_delay(idb->firstsw, FDDI_DELAY);
    idb->reliability = 255;
    idb->load = 1;

    reg_invoke_media_enctype_setup(swidb, ET_SNAP);
    swidb->ip_enctype = ET_SNAP;

    idb->state = IDBS_DOWN;
    idb->nokeepalive = TRUE;
    idb->max_proto_mtu = fddi_max_proto_mtu;

    /*
     * Set up how ARP is done.
     */
    swidb->arp_arpa = swidb->arp_arpa_def = FALSE;
    swidb->arp_probe = swidb->arp_probe_def = FALSE;
    swidb->arp_snap = swidb->arp_snap_def = TRUE;
    swidb->arp_timeout = ARP_MAXAGE;
    swidb->arp_gra_time = FDDI_ARP_GRATUITOUS_HOLD;
}


/*
 * get_fddi_fc_string
 */
static char *get_fddi_fc_string (
    char fc)
{
    switch(fc) {
    case SMT_NSA_FC:
	return("NSA");
    case SMT_INFO_FC:
	return("SMT");
    default:
	return("???");
    }
}

/*
 * get_smt_class_string
 */
static const char *const smt_class_string[11] = {
    "???",
    "NIF",
    "SIF Confg",
    "SIF Oper",
    "Echo",
    "RAF",
    "RDF",
    "SRF",
    "PMF Get",
    "PMF Set",
    "ESF"};	/* leave ESF as the last entry */


static const char *get_smt_class_string (
    uchar class)
{
    int indx;
    
    indx = class;
    if (indx == ESF_CLASS)
	indx = 10;
    if ((indx < NIF_CLASS) || (indx > PMFSET_CLASS)) {
	indx = 0;
    }
    return(smt_class_string[indx]);
}	

/*
 * get_smt_type_string
 */
static const char *const smt_type_string[4] = {
    "???",
    "Announcement",
    "Request",
    "Response"};

static const char *get_smt_type_string (
    uchar type)
{
    if (type > 3)
	type = 0;
    return(smt_type_string[type]);
    
}

/*
 * fddi_smt_debug
 * Function to print out a SMT frame.
 */
void fddi_smt_debug (paktype *pak, boolean output)
{
    fddi_hdr *fddi;
    smthdrtype *smt;
    hwidbtype *idb;
    ushort *sptr;
    uchar *cptr;
    int len, indx, jndx;
    
    fddi = (fddi_hdr *) pak->datagramstart;
    smt = (smthdrtype *) fddi->data;
    idb = hwidb_or_null(pak->if_input);
    if (output)
	idb = pak->if_output->hwptr;
    buginf("\nSMT %s: %s, FC=%s, DA=%e, SA=%e, \n  class=%s, type=%s, vers=%d, station_id=%e, len=%d",
	   (output ? "O" : "I"),
	   idb->hw_namestring, get_fddi_fc_string(fddi->fc), fddi->daddr,
	   fddi->saddr,
	   get_smt_class_string(smt->frame_class),
	   get_smt_type_string(smt->frame_type), GETSHORT(&smt->version_id),
	   &smt->station_id[2], GETSHORT(&smt->info_field_length));
    len = GETSHORT(&smt->info_field_length);
    sptr = (ushort *) ((uchar *)smt + sizeof(smthdrtype));
    while (len) {
	jndx = GETSHORT(sptr++);
	indx = GETSHORT(sptr++);
	buginf("\n  - code %x, len %d -- ", jndx, indx);
	cptr = (uchar *)sptr;
	len -= (indx + SMT_OVERHEAD);	
	sptr = (ushort *) sptr + (indx / 2);
	while (indx--)
	    buginf("%-02x", *cptr++);
    }
    buginf("\n");
}


/*
 * fddiwrap_check
 * check for fddi state changes
 */

static void fddiwrap_check (void)
{
    hwidbtype *idb;
    int fddistat;
    fddisb *fsb;
    list_element *l_elt;

    FOR_ALL_HWIDBS_IN_LIST(HWIDBLIST_FDDI, l_elt, idb) {
	fsb = idb_use_hwsb_inline(idb, HWIDB_SB_FDDI);
	fddistat = fsb->cfm_state;
	if (fddistat != idb->newfddistat) {
	    errmsg(&msgsym(FDDISTAT, LINK),
		   idb->hw_namestring,
		   get_smt_string (&SMTMsg.cf_state, fsb->cfm_state));
	    idb->newfddistat = fddistat;
	}
	idb_release_hwsb_inline(idb, HWIDB_SB_FDDI); 
    }
}


/*
 * fddi_init
 *
 * Initialize global FDDI related information.
 */
static void fddi_init (subsystype* subsys)
{

    /*
     * Initialize RIF support
     */
    rif_init();

    /*
     * Initialize FDDI parser support
     */
    fddi_parser_init();

    /*
     * Set up debugging flags
     */
    fddi_debug_init();
    
    /*
     * Initialize the registries.
     */
    reg_add_onemin(fddiwrap_check, "fddiwrap_check");
    reg_add_media_fddi_macaddr(LINK_LOOP,
			       loop_macaddr_fddi,
			       "loop_macaddr_fddi");
    reg_add_media_hwidb_enqueue_private_list(fddi_idb_enqueue,
					     "fddi_idb_enqueue");
    reg_add_default_media_fddi_macaddr((service_media_fddi_macaddr_type)
				       return_false, "return_false");
    reg_add_default_media_fddi_macwrite((service_media_fddi_macwrite_type)
					return_false, "return_false");
    reg_add_media_rif_lookup(rif_lookup, "rif_lookup");
    
    queue_init(&smtbufQ, 0);
}

/*
 * FDDI subsystem header
 */

#define FDDI_MAJVERSION 1
#define FDDI_MINVERSION 0
#define FDDI_EDITVERSION 1

SUBSYS_HEADER(fddi, FDDI_MAJVERSION, FDDI_MINVERSION, FDDI_EDITVERSION,
	      fddi_init, SUBSYS_CLASS_KERNEL,
	      NULL,
	      NULL);


/*
 * Convert an XDI index into an ASCII string.
 */
char *get_smt_string (void *ptr, ushort index)
{
    if (index <= ((struct SmtMsg *)ptr)->max)
	return (((struct SmtMsg *)ptr)->str[index]);
    else
	return ("Unknown");
}


/*
 * Convert XDI PCM signal type into an ASCII string.
 */
char *get_pcm_signal_string (uInt16 pc_signal)
{
    switch (pc_signal) {
      case SIG_PC_Enable:
	return("Enable");
      case SIG_PC_Disable:
	return("Disable");
      case SIG_PC_Start:
	return("Start");
      case SIG_PC_Maint:
	return("Maint");
      case SIG_PC_Trace:
	return("Trace");
      case SIG_PC_Stop:
	return("Stop");
      default:
	return("Unknown");
    }
}


/*
 * Convert XDI line_state into an ASCII string.
 */
char *get_ls_string (uInt16 ls)
{
    static char buffer[21];
    ushort i = 0;

    /*
     * Initialize buffer so that string will be
     * 4 or 20 bytes long (tabular output).
     */
    for (i=1; i<20; i++)
	buffer[i] = ' ';
    buffer[0] = '?';
    buffer[4] = 0;
    buffer[20] = 0;

    /* Include each LS in the output string */
    i = 0;
    if (ls & QLS)
	buffer[i++] = 'Q';
    if (ls & HLS)
	buffer[i++] = 'H';
    if (ls & MLS)
	buffer[i++] = 'M';
    if (ls & ILS)
	buffer[i++] = 'I';
    if (ls & SILS) {
	buffer[i++] = 'I';
	buffer[i++] = 'I';
    }
    if (ls & ALS)
	buffer[i++] = 'A';
    if (ls & PDRLS)
	buffer[i++] = 'P';
    if (ls & NLS)
	buffer[i++] = 'N';

    /* Return string address */
    return (buffer);
}


/*
 * got_another_smt_buf
 * malloc and enqueue a new FDDI SMT buffer
 * Return TRUE if we got another one, else return FALSE
 */

static boolean got_another_smt_buf (void)
{
    char *cp;

    cp = malloc_named(FDDI_FRAME_SIZE + sizeof(uchar *), "SMT Buffer");
    if ( !cp ) {
	return(FALSE);
    }
    p_enqueue(&smtbufQ, cp);
    return(TRUE);
}




/*
 * freed_smt_buf
 * Dequeue and free a FDDI SMT buffer
 * Return TRUE if we freed one, else return FALSE
 */

static boolean freed_smt_buf (void)
{
    char *cp;

    cp = p_dequeue(&smtbufQ);
    if (cp) {
	free(cp);
	return(TRUE);
    }
    return(FALSE);
}




/*
 * fddi_smt_bufs_needed
 * Return quantity of SMT buffers needed for installed FDDIs
 */
int fddi_smt_bufs_needed (void)
{
    return(nfddinets * fddi_smtqsize);
}



/*
 * adjust_smt_bufs
 * Ensure that we have the correct number of FDDI SMT buffers
 * allocated for the number of FDDIs currently installed
 */

void adjust_smt_bufs (void)
{
    int smt_desired;

    smt_desired = fddi_smt_bufs_needed();
    if (fddi_debug)
	buginf("\nChecking number of FDDI smt buffers. Current %d. Desired %d",
	       smtbufQ.maximum, smt_desired);
    if (smtbufQ.maximum < smt_desired) { /* Not enough */
	if (fddi_debug)
	    buginf("\n Need %d more FDDI smt buffers",
		   (smt_desired - smtbufQ.maximum));
	while (smtbufQ.maximum < smt_desired) {
	    if (got_another_smt_buf()) {
		smtbufQ.maximum++;
		if (fddi_debug)
		    buginf(".");
	    } else {
		if (fddi_debug)
		    buginf("E");
		break;
	    }
	}
    }
    if (smtbufQ.maximum > smt_desired) { /* Too many */
	if (fddi_debug)
	    buginf("\n Need %d less FDDI smt buffers",
		   (smtbufQ.maximum - smt_desired));
	while (smtbufQ.maximum > smt_desired) {
	    if (freed_smt_buf()) {
		smtbufQ.maximum--;
		if (fddi_debug)
		    buginf(".");
	    } else {
		if (fddi_debug)
		    buginf("E");
		break;
	    }
	}
    }
}




/*
 * Allocate n SMT buffers at startup time.
 */

ushort get_smt_bufs (ushort n)
{
    adjust_smt_bufs();
    if (smtbufQ.maximum == n)
	return(0);
    else
	return(smtbufQ.maximum);
}


/*
 * Connect FDDI station.
 */
void fddi_connect (ushort stnid)
{
    leveltype	SR;

    if (fddi_debug)
        buginf("\nIn fddi_connect");

    /* Send Connect Action signal to station's ECM state machine */
    SendSignalMulti (stnid, SIG_Connect, 0, (uInt32) 0);

    /* Process signals */
    if (!EmptySignalQueue()) {
	MDisableCSPInterrupts(&SR);
	ProcessCSP (&SR);
	MRestoreCSPInterrupts(&SR);
    }
}


/*
 * Disconnect FDDI station.
 */
void fddi_disconnect (ushort stnid)
{
    leveltype	SR;

    if (fddi_debug)
        buginf("\nIn fddi_disconnect");

    /* Send Connect Action signal to station's ECM state machine */
    SendSignalMulti (stnid, SIG_Disconnect, 0, (uInt32) 0);

    /* Process signals */
    if (!EmptySignalQueue()) {
	MDisableCSPInterrupts(&SR);
	ProcessCSP (&SR);
	MRestoreCSPInterrupts(&SR);
    }
}


/*
 * Cisco actions when a duplicate MAC is found on the ring.
 */
static void fddi_dup_address (hwidbtype *idb)
{
    errmsg(&msgsym(FDDIFAIL, FDDI), "fddi0"
           "Duplicate MAC address detected ", "", idb->unit);
    if (idb->shutdown)
      (*idb->shutdown)(idb, IDBS_ADMINDOWN);
}


/*
 * Shutdown FDDI station with all its associated MACs.
 */
void fddi_dup_addr_shutdown (ushort stn_id, ushort mac_id)
{
    StnDCBType      *stnPtr;
    hwidbtype       *idb;
    ushort		i;

    /* notify system of DA condition */
    stnPtr = &STAINST (stn_id);
    fddi_dup_address (stnPtr->hwIDB[mac_id]);

    /* shutdown all other station MACs as well */
    for (i=0; i<MACCFG (stn_id, MacCt); i++) {
        idb = stnPtr->hwIDB[mac_id];
        if (stnPtr->hwPresent && idb && idb->shutdown)
            (*idb->shutdown)(idb, IDBS_ADMINDOWN);
    }
}


/*
 * Process input FDDI SMT frames as a normal priority process.
 */
process fddi_smt_input (void)
{
    leveltype SR;

    while (TRUE) {

        /* Sleep until there are some SMT packets to process */
	process_wait_for_event();

#if DEBUG_ALL
        bprintf("\smt_input woke up for smt or timer\n");
#endif

	MDisableInterrupts(&SR);
        ProcessFBM(&SR); /* Process packets */
	MRestoreInterrupts(&SR);
    }
}


/*
 * XDI independent wrapper for queueing frames to SMT.
 */
void fddi_smt_frame (hwidbtype *idb,
                     uchar *ptr,
                     ushort size,
                     uchar eac_bits)
{
    PostFBMQueueMulti(idb->fddi_stn_id,
		      FBM_Q_FRAME,
		      ptr,
		      size,
		      0,
		      eac_bits);
    process_wakeup(smt_process_id);
}


/*
 * Reinitialize CMT only, vectors already installed.
 */
void init_cmt (hwidbtype *hwidb)
{
    if (fddi_debug) {
        buginf("\nDoing CMT init for %s, intlevel=%d",
	       hwidb->hw_namestring, get_interrupt_level());
	bugtrace();
    }

    if (CSPMainMulti (hwidb->fddi_stn_id)) {
        errmsg(&msgsym(NOMEMORY, FDDI), hwidb->unit, "CSPMainMulti");
        return;
    }

    /*
     * Send Connect Action Message to ECM
     */
    fddi_connect(hwidb->fddi_stn_id);
}
