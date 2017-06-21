/*
 *	P A R S E R _ D E F S _ B R I D G E . H
 *
 * $Id: parser_defs_bridge.h,v 3.1.40.2 1996/05/09 14:46:33 rbadri Exp $
 * $Source: /release/112/cvs/Xsys/srt/parser_defs_bridge.h,v $
 * 
 * Copyright (c) 1985-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: parser_defs_bridge.h,v $
 * Revision 3.1.40.2  1996/05/09  14:46:33  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.1.40.1  1996/05/04  01:49:04  wilber
 * Branch: California_branch
 *
 * Integrated Routing and Bridging
 * - Bridge-group Virtual Interface configuration & bridging
 *   (process-level) support.
 * - BVI smf functions and registry.
 * - BVI settable mtu support & dynamically select mtu for the BVI.
 * - 'bridge irb' & 'bridge <x> route/bridge <protcocol>' support.
 *
 * Constrained Multicast Flooding
 * - Add the CMF parser link points.
 * - Add the CMF data structures to spantype.
 * - Add the creation and deletion of the tbridge monitor.
 * - Convert tbridge flood array to a linked list of tbifd's.
 * - Manage the CMF data structures when the spanning tree changes.
 *
 * Revision 3.1.70.1  1996/04/27  07:26:16  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.1.58.1  1996/04/08  02:09:44  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 3.1.46.2  1996/03/25  19:02:02  cakyol
 * correcting a merge error in the sync.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.1.46.1  1996/03/25  02:21:12  cakyol
 * sync LE_Syn_Cal_V111_1_3_branch to LE_Cal_V111_1_0_3_branch
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.1.8.1  1996/03/05  01:52:30  cyoung
 * Add management-domain and vlan-id to bridge command.
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 3.1  1996/02/01  04:59:17  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 1: Create copies of the files to be moved in their new location.
 *
 * Revision 3.2  1995/11/17  18:49:30  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:54:33  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/08/18  23:54:32  speakman
 * CSCdi39005:  CRB commands collide
 * Require minimum 3 letter match for 'show interface crb'.
 *
 * Revision 2.2  1995/07/09  21:06:24  speakman
 * CSCdi36848:  Configuration of CRB proves unpopular
 * Introduce explicit, bridge-group specific route/bridge indicators
 * for CRB, and the commands to configure them.
 *
 * Revision 2.1  1995/06/07  22:29:02  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/* Bridge group defines */
#define	BGROUP_ASSIGN                                     1
#define	BGROUP_CBUSBRIDGE				  2
#define	BGROUP_CIRCUIT                                    3
#define	BGROUP_INADDRESS                                  4
#define	BGROUP_INLAT_DENY                                 5
#define	BGROUP_INLAT_PERMIT                               6
#define	BGROUP_INLSAP                                     7
#define	BGROUP_INPATTERN                                  8
#define	BGROUP_INTYPE                                     9
#define	BGROUP_LAT                                       10
#define	BGROUP_OUTADDRESS                                11
#define	BGROUP_OUTLAT_DENY                               12
#define	BGROUP_OUTLAT_PERMIT                             13
#define	BGROUP_OUTLSAP                                   14
#define	BGROUP_OUTPATTERN                                15
#define	BGROUP_OUTTYPE                                   16
#define	BGROUP_PATHCOST                                  17
#define	BGROUP_PRIORITY                                  18
#define	BGROUP_SPANDISABLE                               19
#define	BGROUP_LATSERVICE	                         20
#define	BGROUP_LATCOMPRESSION                            21
#define BGROUP_SSE					 22

/* Bridging defines */
#define	NO_BRIDGE		 0
#define BRIDGE_ACQUIRE           1
#define BRIDGE_ADDRESS          (BRIDGE_ACQUIRE + 1)
#define BRIDGE_DOMAIN           (BRIDGE_ADDRESS + 1)
#define BRIDGE_AGINGTIME	(BRIDGE_DOMAIN + 1)
#define BRIDGE_FORWARDTIME      (BRIDGE_AGINGTIME + 1)
#define BRIDGE_HELLOTIME        (BRIDGE_FORWARDTIME + 1)
#define BRIDGE_MAXAGE           (BRIDGE_HELLOTIME + 1)
#define BRIDGE_MCASTSRC         (BRIDGE_MAXAGE + 1)
#define BRIDGE_PRIORITY         (BRIDGE_MCASTSRC + 1)
#define BRIDGE_PROTOCOL         (BRIDGE_PRIORITY + 1)
#define BRIDGE_LATFILTER	(BRIDGE_PROTOCOL + 1)
#define BRIDGE_FORWARD_TIME	(BRIDGE_LATFILTER + 1)
#define BRIDGE_CCG_PAUSE	(BRIDGE_FORWARD_TIME + 1)
#define BRIDGE_CCG_SRC		(BRIDGE_CCG_PAUSE + 1)
#define BRIDGE_CCG		(BRIDGE_CCG_SRC + 1)
#define BRIDGE_CRB		(BRIDGE_CCG + 1)

#define BRIDGE_IRB            	(BRIDGE_CRB + 1)
#define BRIDGE_ROUTE          	(BRIDGE_IRB + 1)
#define BRIDGE_BRIDGE          	(BRIDGE_ROUTE + 1)
#define BRIDGE_VLANDOMAIN       (BRIDGE_BRIDGE +1)
#define NBRIDGECMDS             (BRIDGE_VLANDOMAIN + 1)


#define SPAN_MAXINDEX	        64
