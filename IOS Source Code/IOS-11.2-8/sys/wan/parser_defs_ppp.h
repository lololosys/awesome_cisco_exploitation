/*
 *	P A R S E R _ D E F S _ P P P . H
 *
 * $Id: parser_defs_ppp.h,v 3.2.10.5 1996/05/19 05:48:59 dblair Exp $
 * $Source: /release/112/cvs/Xsys/wan/parser_defs_ppp.h,v $
 * 
 * Copyright (c) 1985-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: parser_defs_ppp.h,v $
 * Revision 3.2.10.5  1996/05/19  05:48:59  dblair
 * CSCdi57979:  Multilink clean up from Dial1_branch
 * Branch: California_branch
 * Optimize fragmentation to remove multiply and divides from the
 * data path.
 * Xmit fragments from the mlp_holdq_enqueue.
 * update BW when links are added and removed from bundle
 * Fix byte counts and packet counts on the virtual bundle interface
 * Clean up
 *
 * Revision 3.2.10.4  1996/05/10  01:45:13  wmay
 * CPP and PPP half-bridge commit
 * Branch: California_branch
 *
 * Revision 3.2.10.3  1996/05/06  17:03:25  fox
 * CSCdi49278:  Update PAP handling of outbound calls
 * Branch: California_branch
 * Three bug fixes in one!
 * Disable remote PAP authentication support by default.  Add 'ppp pap
 * sent-username xxx password yyy' to re-enable support if needed.  Warn
 * if sent-username/password matches entry in local database. (CSCdi49278)
 * Add 'ppp chap hostname xxx' command to allow multiple boxes to use the
 * same hostname in their CHAP Challenges.  (CSCdi44884)
 * Add 'ppp chap password yyy' command to allow a box to respond to many
 * unknown peers with the same password.  (CSCdi54088)
 *
 * Revision 3.2.10.2  1996/04/27  06:37:14  syiu
 * Branch: California_branch
 * Commit Dial1_branch
 *
 * Revision 3.2.28.2  1996/04/25  23:59:49  syiu
 * Branch: Dial1_branch
 * Dial1_branch to IosSec_postmerge_960419
 *
 * Revision 3.2.28.1  1996/04/17  00:01:47  syiu
 * Branch: Dial1_branch
 * Sync Dial1_branch to V111_1_3
 *
 * Revision 3.2.10.1  1996/03/27  10:36:29  irfan
 * CSCdi36933:  Customer desires configurable PAP/CHAP retries
 * Branch: California_branch
 * Add PPP UI to make us accept more than one PAP/CHAP Authentication
 * attempt. Instead of hanging up the physical layer right away.
 *
 * Revision 3.2  1996/02/22  05:21:00  fox
 * CSCdi35908:  Should allow CHAP and PAP clients to same dialin pool
 * Add support for multiple acceptable inbound PPP authentication methods
 *
 * Revision 3.1  1996/02/01  05:14:35  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 1: Create copies of the files to be moved in their new location.
 *
 * Revision 3.2  1995/11/17  18:51:57  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:55:54  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/09/20  22:37:30  dblair
 * CSCdi40745:  PPP Multilink feature commit
 *
 * Revision 2.1  1995/06/07  22:30:51  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __PARSER_DEFS_PPP_H__
#define __PARSER_DEFS_PPP_H__

/*
 * ppp commands
  * Caution *** Warning ****:
  *   When adding ppp commands, consider adding them to copy fields
  *     so that changes to a dialer or ISDN interface will get
  *     propogated down to B-channels or Huntgroup members
 */
#define PPP_LINK_QUALITY        1
#define PPP_MAX_TERMINATE       2
#define PPP_MAX_CONFIGURE       3
#define PPP_MAX_FAILURE         4
#define PPP_RESTART_TIMER       5
#define PPP_AUTHENTICATION      6
#define PPP_AUTH_CHAP           7
#define PPP_AUTH_PAP         	8 
#define PPP_USE_TACACS          9
#define PPP_AUTH_IFNEED         10
#define PPP_COMPRESSION         11
#define PPP_COMP_STACKER        12
#define PPP_COMP_PREDICTOR      13
#define PPP_RELIABLE_LINK       14
#define PPP_CALLBACK            15
#define PPP_CALLBACK_ACCEPT     16
#define PPP_CALLBACK_REQUEST    17  
#define PPP_CALLBACK_INITIATE   18
#define PPP_MULTILINK           19
#define PPP_MAX_BAD_AUTHS       20
#define PPP_PAP_SENT_NAME       21
#define PPP_PASSWORD            22
#define PPP_CHAP_NAME           23
#define PPP_BRIDGE		24
#define PPP_NEG_TIMEOUT         25
#define PPP_REFUSE_CHAP		26
#define PPP_REFUSE_CHAP_CALLIN	27
#define PPP_CALL_DIRECTION	28
#define PPP_CHAP_WAIT		29
#define PPP_CHAP_IGNOREUS	30
#define PPP_CHAP_SPLIT_NAMES	31
#ifdef NOTDONE
#define PPP_CHAP_DUAL_SECRETS	32
#define PPP_CHAP_QUAD_SECRETS	33
#endif

/*
 * PPP Call direction values
 *  Do not define a call direction value of zero as one of the direction
 *  fields uses zero to indicate that the direction has not been set!
 */
#define PPP_NO_CALL     1
#define PPP_CALL_IN     2
#define PPP_CALL_OUT    3

#define PPP_AUTH_CHAP_MASK	0x01
#define PPP_AUTH_PAP_MASK	0x02

#define PPP_AUTH_NUM		2
#define PPP_AUTH_MSCHAP         0x80	/* not supported currently */

/*
 * MLP global command
 */
#define PPP_MULTILINK_DEFAULT   0
#define PPP_MULTILINK_VTEMPLATE 1

/*
 * Multilink interface commands
 */
#define MULTILINK_THRESHOLD   1
#define MULTILINK_MIN_LINKS   2
#define MULTILINK_MAX_LINKS   3
#define MULTILINK_MAX_FRAGS   4

enum MULTILINK_THRESHOLD_DIRECTION {
    MULTILINK_THRESH_OUTBOUND = 0, 
    MULTILINK_THRESH_EITHER,
    MULTILINK_THRESH_INBOUND
}; 

/*
 *  MLP_DEFAULT_MAX_FRAGS
 *  default for the maximum number of fragments per packet
 *  !!!!! warning !!!!! danger:  This default must always be a power of 2.
 */
#define MLP_DEFAULT_MAX_FRAGS_EXP        4
#define MLP_DEFAULT_MAX_FRAGS            0x00001 << MLP_DEFAULT_MAX_FRAGS_EXP

#endif /* __PARSER_DEFS_PPP_H__ */

