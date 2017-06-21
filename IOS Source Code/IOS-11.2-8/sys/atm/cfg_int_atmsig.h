/* $Id: cfg_int_atmsig.h,v 3.2.56.3 1996/08/02 16:39:39 jwjang Exp $
 * $Source: /release/112/cvs/Xsys/atm/cfg_int_atmsig.h,v $
 *------------------------------------------------------------------
 * parser/cfg_int_atmsig.h:  Configuration command parsing code
 *
 * November, 1993,  Percy P. Khabardar 
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All Rights Reserved.
 *
 * Configuration commands for Q.9231 (ATM Signalling).
 *------------------------------------------------------------------
 * $Log: cfg_int_atmsig.h,v $
 * Revision 3.2.56.3  1996/08/02  16:39:39  jwjang
 * CSCdi64888:  asig command should be hidden from user
 * Branch: California_branch
 *
 * Revision 3.2.56.2  1996/05/09  14:11:24  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.2.56.1.18.1  1996/04/27  06:33:47  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.2.56.1.8.1  1996/04/10  23:35:44  jwjang
 * CSCdi54325:  commit uni 3.1 to LE_Cal_V112_0_2_branch
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 3.2.56.1  1996/03/18  18:59:48  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.2  1996/03/13  01:09:05  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.2.26.1  1996/02/20  13:35:20  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.2  1995/11/17  08:46:40  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:00:23  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/08/18  14:23:19  schrupp
 * CSCdi38181:  Add ATMSIG throttling and SSCOP changes
 *
 * Revision 2.1  1995/07/04  09:35:09  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize arap, atm, bstun
 *
 * Revision 2.2  1995/06/24  14:34:01  schrupp
 * CSCdi36345:  Add T398, T399 timers to parser
 *
 * Revision 2.1  1995/06/07  22:03:44  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 *
 * ATM Signalling interface config commands
 */

/***************************************************************
 * atmsig max-conn-pending <n>
 */
PARAMS_KEYONLY(atmsig_max_conn_pending, no_alt, "max-conn-pending",
               OBJ(int,1), 1, 255,
               atmSig_interface_commands, ATMSIG_MAX_CONN_PENDING,
               "Max number of local connections pending",
               "Max number of local connections pending",
               PRIV_CONF | PRIV_HIDDEN);

/***************************************************************
 * asig max-retry-count <n>
 */
PARAMS_KEYONLY(atmsig_retry_count, atmsig_max_conn_pending, "max-retry-count",
               OBJ(int,1), 1, 255,
               atmSig_interface_commands, ATMSIG_MAX_RETRY,
               "Number of times router should retry various operations",
               "Number of times router should retry various operations",
               PRIV_CONF | PRIV_HIDDEN);

/***************************************************************
 *
 * asig timer399 <n>
 */
PARAMS_KEYONLY(atmsig_timer399, atmsig_retry_count, "timer399",
               OBJ(int,1), 1, 60000,
               atmSig_interface_commands, ATMSIG_TIMER399,
               "Add Party message sent timer",
               "Add Party message sent timer",
               PRIV_CONF | PRIV_HIDDEN);

/***************************************************************
 *
 * asig timer398 <n>
 */
PARAMS_KEYONLY(atmsig_timer398, atmsig_timer399, "timer398",
               OBJ(int,1), 1, 60000,
               atmSig_interface_commands, ATMSIG_TIMER398,
               "Drop Party message sent timer",
               "Drop Party message sent timer",
               PRIV_CONF);

/***************************************************************
 *
 * asig timer316 <n>
 */
PARAMS_KEYONLY(atmsig_timer316, atmsig_timer398, "timer316",
               OBJ(int,1), 1, 60000,
               atmSig_interface_commands, ATMSIG_TIMER316,
               "Restart message sent timer",
               "Restart message sent timer",
               PRIV_CONF | PRIV_HIDDEN);

/***************************************************************
 *
 * asig timer308 <n>
 */
PARAMS_KEYONLY(atmsig_timer308, atmsig_timer316, "timer308",
               OBJ(int,1), 1, 60000,
               atmSig_interface_commands, ATMSIG_TIMER308,
               "Release message sent timer",
               "Release message sent timer",
               PRIV_CONF | PRIV_HIDDEN);

/***************************************************************
 *
 * asig timer322 <n>
 */
PARAMS_KEYONLY(atmsig_timer322, atmsig_timer308, "timer322",
               OBJ(int,1), 1, 60000,
               atmSig_interface_commands, ATMSIG_TIMER322,
               "Status Enquiry message sent timer",
               "Status Enquiry message sent timer",
               PRIV_CONF | PRIV_HIDDEN);

/***************************************************************
 *
 * asig timer313 <n>
 */
PARAMS_KEYONLY(atmsig_timer313, atmsig_timer322, "timer313",
               OBJ(int,1), 1, 60000,
               atmSig_interface_commands, ATMSIG_TIMER313,
               "Connect message sent timer",
               "Connect message sent timer",
               PRIV_CONF | PRIV_HIDDEN);

/***************************************************************
 *
 * asig timer310 <n>
 */
PARAMS_KEYONLY(atmsig_timer310, atmsig_timer313, "timer310",
               OBJ(int,1), 1, 60000,
               atmSig_interface_commands, ATMSIG_TIMER310,
               "Call Proceeding message sent timer",
               "Call Proceeding message sent timer",
               PRIV_CONF | PRIV_HIDDEN);

/***************************************************************
 *
 * asig timer309 <n>
 */
PARAMS_KEYONLY(atmsig_timer309, atmsig_timer310, "timer309",
               OBJ(int,1), 1, 60000,
               atmSig_interface_commands, ATMSIG_TIMER309,
               "Connection request timer",
               "Connection request timer",
               PRIV_CONF | PRIV_HIDDEN);



/***************************************************************
 *
 * asig timer303 <n>
 */
PARAMS_KEYONLY(atmsig_timer303, atmsig_timer309, "timer303",
               OBJ(int,1), 1, 60000,
               atmSig_interface_commands, ATMSIG_TIMER303,
               "Setup message sent timer",
               "Setup message sent timer",
               PRIV_CONF | PRIV_HIDDEN);


/***************************************************************
 *
 * [no] asig side [network | user]
 */
EOLS (atmsig_side_eol, atmSig_interface_commands, ATMSIG_UNI_SIDE);

KEYWORD_ID(atmsig_side_network, atmsig_side_eol, atmsig_side_eol,
           OBJ(int,1), ATMSIG_UNI_SIDE_NETWORK,
           "network", "UNI Network Side", PRIV_CONF | PRIV_HIDDEN);
KEYWORD_ID(atmsig_side_user, atmsig_side_eol, atmsig_side_network,
           OBJ(int,1), ATMSIG_UNI_SIDE_USER,
           "user", "UNI User Side", PRIV_CONF | PRIV_HIDDEN);
NVGENS (atmsig_side_nvgen, atmsig_side_user, atmSig_interface_commands,
                                                 ATMSIG_UNI_SIDE);
KEYWORD (atmsig_side, atmsig_side_nvgen, atmsig_timer303,
         "side", "uni side", PRIV_CONF | PRIV_HIDDEN);

/****************************************************************/

KEYWORD (asig, atmsig_side, NONE, "asig",
         "ATM Signalling Interface Subcommands", PRIV_CONF | PRIV_HIDDEN);

ASSERT (asig_test, asig, ALTERNATE, is_atm(csb->interface->hwptr));

#undef  ALTERNATE
#define ALTERNATE  asig_test 














