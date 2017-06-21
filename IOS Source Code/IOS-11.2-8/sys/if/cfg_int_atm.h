/* $Id: cfg_int_atm.h,v 3.8.6.5 1996/05/09 14:29:43 rbadri Exp $
 * $Source: /release/112/cvs/Xsys/if/cfg_int_atm.h,v $
 *------------------------------------------------------------------
 * parser/cfg_int_atm.h:
 *
 * November, 1993, Rick Li Fo Sjoe
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cfg_int_atm.h,v $
 * Revision 3.8.6.5  1996/05/09  14:29:43  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.8.6.4.2.1  1996/04/27  07:03:34  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.8.6.4  1996/04/25  23:13:00  ronnie
 * CSCdi55766:  Hey!  This ATM and Frame Relay is no different than in 11.1
 * Feature commit for ATM and Frame Relay into 11.2
 * Branch: California_branch
 *
 * Revision 3.8.6.3  1996/04/09  19:06:09  ronnie
 * CSCdi50951:  Pb using the RS6000 as ATMARP server due to invalid nsap
 * in arp req
 * Branch: California_branch
 *
 * Revision 3.8.6.2.4.3  1996/04/16  02:43:40  cakyol
 * goodbye to PVC support for LANE.
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 3.8.6.2.4.2  1996/04/10  23:23:34  jwjang
 * CSCdi54325:  commit uni 3.1 to LE_Cal_V112_0_2_branch
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 3.8.6.2.4.1  1996/04/08  01:54:26  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 3.8.6.2  1996/03/29  19:26:45  grobelch
 *   Provide traffic shaping for SVCs when possible.
 *   Also:
 *   - Specify map-class rate parameters in kbps.
 *   - Provide a per-interface command to indicate
 *     whether SVC should be established if signalled
 *     parameters cannot be installed.
 *   - Don't attach ATM submaps to map-class block
 *   - Check for map-class params prior to using
 *     them (they can be null).
 *   - Fix help-strings for "clp1" parameters to
 *     indicate for clp0&1 flows.
 *   - Signal best effort if only pcr for clp0&1 are
 *     specified.
 *   - map-class parameters override map-list where
 *     they overlap.
 *   - Fix a couple of erroneous variable setting/usages
 *     in static-map handling.
 * CSCdi51550:  ATMSIG: SVCs should attempt to use traffic shaping
 * Branch: California_branch
 *
 * Revision 3.8.6.1  1996/03/18  20:13:15  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.8.8.1  1996/03/22  09:37:07  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.8  1996/03/07  03:13:45  hxu
 * CSCdi50455:  Wrong max burst value in setting SVC
 * The correct max burst value of is set to be 64k-1.
 *
 * Revision 3.7.10.1  1996/03/05  06:13:41  rlowe
 * Apply LANE content to merge branch.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 3.7.4.1  1996/02/27  21:08:11  cakyol
 * non sync sync of LE_Cal_V111_0_16_branch to V111_1_0_3 yielding
 * LE_Cal_V111_1_0_3_branch
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 3.7  1996/02/01  23:33:30  widmer
 * CSCdi45325:  Change Keyword/Set to use Keyword_Id macro
 *
 * Revision 3.6.2.1  1996/01/11  01:45:20  cakyol
 * non sync sync of LE_Calif_branch to V111_0_16 yielding
 *     LE_Cal_V111_0_16_branch
 *
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 3.6  1996/01/06  02:55:43  ronnie
 * CSCdi46517:  Cant turn off multiple arp server feature
 *
 * Revision 3.5  1996/01/05  16:57:10  rzagst
 * CSCdi44790:  burst size of 0 should not be allowed when configing pvc
 *
 * Revision 3.4  1995/12/07  17:45:00  widmer
 * CSCdi45134:  Replace occurences of Ifelse with appropriate Test* macros
 *
 * Revision 3.3.8.1  1995/12/29  01:05:58  cakyol
 * Initial commits for:
 *
 * LANE Fault Tolerant LECS Servers
 * LANE Fault Tolerant LES Servers
 * HSRP over LANE Client
 *
 * Branch: LE_Calif_branch
 *
 * Revision 3.3  1995/11/23  10:43:28  weismann
 * CSCdi37381:  aver./peak prompts same; aver. prmpt not dynamic but
 *              values are. Fix atm vc average rate issues with parser.
 *
 * Revision 3.2  1995/11/17  09:26:01  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:48:03  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.9  1995/11/08  21:02:43  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.8  1995/10/22  02:41:51  lmercili
 * CSCdi40829:  AAL34SMDS encap on a PVC allows you to specify inarp
 *
 * Revision 2.7  1995/09/27  00:58:38  weismann
 * CSCdi38314:  pvc parse chain option state after burst token not correct
 *
 * Revision 2.6  1995/08/30  02:17:47  ronnie
 * CSCdi39486:  ATM ARP Server timeout configuration is wrong
 *
 * Revision 2.5  1995/07/28  05:01:12  schrupp
 * CSCdi37827:  Undo CSCdi37309, signalling vs. signaling
 *
 * Revision 2.4  1995/07/26  15:14:30  schrupp
 * CSCdi37690:  Support ATM ESI addressing with ILMI prefix
 *
 * Revision 2.3  1995/07/19  14:33:51  schrupp
 * CSCdi37309:  Cisco style guide uses signaling, not signalling
 *
 * Revision 2.2  1995/07/07  17:20:29  awu
 * CSCdi36687:  parser help mis-behaviour on atm pvc command.
 *
 * Revision 2.1  1995/07/02  10:53:06  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize os, hes, les, if, and ibm.
 *
 * Revision 2.4  1995/06/27  00:30:29  wfried
 * CSCdi36069:  Performance tuning and changes on controller screen for
 * new interfac
 * Included more interface status in the show controller output,
 * forced ATM receiving packets to start at 16 byte boundaries for
 * better DBUS utilization, and made some fastswitching optimizations.
 * Created new ATM interface loopback command options and renamed E3
 * framing modes to reflect current terminology
 *
 * Revision 2.3  1995/06/14  19:50:03  awu
 * CSCdi34590:  router incorrectly configs extra rate-queue at boot time.
 *
 * Revision 2.2  1995/06/11  03:42:36  awu
 * CSCdi34590:  router incorrectly configs extra rate-queue at boot time.
 *
 * Revision 2.1  1995/06/07  22:03:37  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 *  This file should be deleted.  Contents were spread to platform-specific
 *  files in 11.2
 */

