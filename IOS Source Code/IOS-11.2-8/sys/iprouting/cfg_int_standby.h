/* $Id: cfg_int_standby.h,v 3.5.44.1 1996/05/09 14:31:29 rbadri Exp $
 * $Source: /release/112/cvs/Xsys/iprouting/cfg_int_standby.h,v $
 *------------------------------------------------------------------
 * cfg_int_standby.h -- Configuration commands for hot standby protocol
 *
 * Sep 1993 Tony Li
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cfg_int_standby.h,v $
 * Revision 3.5.44.1  1996/05/09  14:31:29  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.5.70.1  1996/04/27  07:05:30  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.5.60.1  1996/04/08  01:55:57  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 3.5.50.1  1996/03/22  22:52:44  rlowe
 * Non-sync of Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.5.36.1  1996/03/05  06:14:48  rlowe
 * Apply LANE content to merge branch.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 3.5.26.1  1996/02/27  21:09:24  cakyol
 * non sync sync of LE_Cal_V111_0_16_branch to V111_1_0_3 yielding
 * LE_Cal_V111_1_0_3_branch
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 3.5.6.1  1996/01/11  01:45:44  cakyol
 * non sync sync of LE_Calif_branch to V111_0_16 yielding
 *     LE_Cal_V111_0_16_branch
 *
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 3.5  1995/12/07  17:45:12  widmer
 * CSCdi45134:  Replace occurences of Ifelse with appropriate Test* macros
 *
 * Revision 3.4.10.1  1995/12/29  01:06:10  cakyol
 * Initial commits for:
 *
 * LANE Fault Tolerant LECS Servers
 * LANE Fault Tolerant LES Servers
 * HSRP over LANE Client
 *
 * Branch: LE_Calif_branch
 *
 * Revision 3.4  1995/11/19  08:38:45  bcole
 * CSCdi44156:  HSRP: Should be configurable over 802.10 VLANs
 *
 * Allow configuration of HSRP over subinterface'd LAN media.  Add
 * more configuration sanity checking.
 *
 * Revision 3.3  1995/11/19  08:10:58  bcole
 * CSCdi29702:  HSRP: Secondary address support should be improved
 *
 * Abstract implementation of virtual IP address, so that multiple may
 * now be configured per HSRP group.  Add nob.
 *
 * Revision 3.2  1995/11/17  17:31:33  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:04:48  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/01  12:15:37  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize iprouting
 *
 * Revision 2.1  1995/06/07  22:07:31  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/**************************************************************************
 * standby [group] ip [<address> [secondary]]
 *
 * (*OBJ(paddr,1)) = <address>
 */

EOLS	(standby_ip_eol, standby_command, STANDBY_CMD_IP);

KEYWORD_ID(standby_ip_sec, standby_ip_eol, standby_ip_eol,
	   OBJ(int,1), TRUE, "secondary",
	   "Specify an additional Hot standby IP address",
	   PRIV_CONF|PRIV_SUBIF);
IPADDR	(standby_ip_addr, standby_ip_sec, standby_ip_eol,
	 OBJ(paddr,1), "Hot standby IP address");
KEYWORD	(standby_ip, standby_ip_addr, no_alt,
	 "ip", "Enable hot standby protocol for IP", PRIV_CONF|PRIV_SUBIF);


/**************************************************************************
 * standby [group] authentication <auth-string>
 *
 * OBJ(STRING,1) = <auth-string>
 */

EOLS	(standby_auth_eol, standby_command, STANDBY_CMD_AUTH);

STRING	(standby_auth_string, standby_auth_eol, no_alt,
	 OBJ(string,1), "authentication string" );

NOPREFIX (standby_auth_noprefix, standby_auth_string, standby_auth_eol);
KEYWORD	(standby_auth, standby_auth_noprefix, standby_ip,
	 "authentication", "Authentication string", PRIV_CONF|PRIV_SUBIF);

/**************************************************************************
 * standby [group] preempt
 */

EOLS	(standby_preempt_eol, standby_command, STANDBY_CMD_PREEMPT);

KEYWORD	(standby_preempt, standby_preempt_eol, standby_auth,
	 "preempt", "Overthrow lower priority designated routers",
	 PRIV_CONF|PRIV_SUBIF);

/**************************************************************************
 * standby [group] track <interface> [<if-priority>]
 *
 * OBJ(idb,1) = <interface>
 * OBJ(int,1) = <if-priority>
 */

EOLS	(standby_track_eol, standby_command, STANDBY_CMD_TRACK);

NUMBER	(standby_track_pri, standby_track_eol, standby_track_eol,
	 OBJ(int,1), 1, 255, "Priority decrement");
FORWARD_INTERFACE(standby_track_int, standby_track_pri, no_alt, OBJ(idb,1), 
	  (IFTYPE_ANYSUB & ~IFTYPE_NULL));
KEYWORD	(standby_track, standby_track_int, standby_preempt,
	 "track", "Priority tracks this interface state",
	 PRIV_CONF|PRIV_SUBIF);

/**************************************************************************
 * standby [group] priority <number>
 *
 * OBJ(int,1) = <number>
 */

EOLS	(standby_priority_eol, standby_command, STANDBY_CMD_PRIORITY);

NUMBER	(standby_priority_value, standby_priority_eol, no_alt,
	 OBJ(int,1), 0, 255, "Priority value");

NOPREFIX (standby_priority_noprefix, standby_priority_value,
	  standby_priority_eol);
KEYWORD	(standby_priority, standby_priority_noprefix, standby_track,
	 "priority", "Priority level", PRIV_CONF|PRIV_SUBIF);

/**************************************************************************
 * standby [group] timers [msec] <hellotime> [msec] <holdtime>
 *
 * OBJ(int,1) = <hellotime>
 * OBJ(int,2) = <holdtime>
 * OBJ(int,5) = if "msec" is specified for hellotime
 * OBJ(int,6) = if "msec" is specified for holdtime
 */

EOLS	(standby_timers_eol, standby_command, STANDBY_CMD_TIMERS);

NUMBER	(standby_timers_holdtime_sec, standby_timers_eol, no_alt,
	 OBJ(int,2), 1, 255, "Holdtime in seconds");

NUMBER	(standby_timers_holdtime_ms, standby_timers_eol, no_alt,
	 OBJ(int,2), 20, ONESEC, "Holdtime in milliseconds");

KEYWORD_ID (standby_timers_holdtime_unit, standby_timers_holdtime_ms,
            standby_timers_holdtime_sec,
            OBJ(int, 6), TRUE, "msec", "specify holdtime in milliseconds",
            PRIV_CONF|PRIV_SUBIF);

NUMBER	(standby_timers_hellotime_sec, standby_timers_holdtime_unit, no_alt,
	 OBJ(int,1), 1, 255, "Hello interval in seconds");

NUMBER	(standby_timers_hellotime_ms, standby_timers_holdtime_unit, no_alt,
	 OBJ(int,1), 20, ONESEC, "Hello interval in milliseconds");

KEYWORD_ID (standby_timers_hellotime_unit, standby_timers_hellotime_ms,
            standby_timers_hellotime_sec,
            OBJ(int, 5), TRUE, "msec", "specify hellotime in milliseconds",
            PRIV_CONF|PRIV_SUBIF);

NOPREFIX (standby_timers_noprefix, standby_timers_hellotime_unit,
	  standby_timers_eol);
KEYWORD	(standby_timers, standby_timers_noprefix, standby_priority,
	 "timers", "Hot standby timers", PRIV_CONF|PRIV_SUBIF);

/**************************************************************************
 * OBJ(int,3) = <group>
 * OBJ(int,4) = <group> is set
 */
SET (standby_int_group_set, standby_timers, OBJ(int,4), TRUE);
NUMBER (standby_int_group, standby_int_group_set, standby_timers,
	OBJ(int,3), 0, 255, "group number");
NUMBER (standby_int_group_tr, standby_int_group_set, standby_timers,
	OBJ(int,3), 0, 2, "group number");
TEST_IDBSTATUS(standby_int_group_test, standby_int_group_tr,
	       standby_int_group, NONE, IDB_TR);

/**************************************************************************
 * standby use-bia
 */

EOLS    (standby_bia_eol, standby_command, STANDBY_CMD_USEBIA);
KEYWORD (standby_bia, standby_bia_eol, standby_int_group_test,
         "use-bia", "Hot standby uses interface's burned in address",
         PRIV_CONF|PRIV_SUBIF);

NVGENS (standby_nvgen, standby_bia, standby_command, STANDBY_CMD_ALL);
KEYWORD	(standby_int, standby_nvgen, NONE,
	 "standby", "Hot standby interface subcommands", PRIV_CONF|PRIV_SUBIF);

TEST_IDBSTATUS(standby_int_type, standby_int, NONE, ALTERNATE, IDB_IEEEMASK);

ASSERT (laneClient, standby_int, standby_int_type, 
	is_atm(csb->interface->hwptr));

#undef	ALTERNATE
#define	ALTERNATE	laneClient

