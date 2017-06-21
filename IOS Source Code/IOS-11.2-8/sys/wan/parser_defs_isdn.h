/*
 *	P A R S E R _ D E F S _ I S D N . H
 *
 * $Id: parser_defs_isdn.h,v 3.2.16.1 1996/06/16 21:21:09 vnguyen Exp $
 * $Source: /release/112/cvs/Xsys/wan/parser_defs_isdn.h,v $
 * 
 * Copyright (c) 1985-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: parser_defs_isdn.h,v $
 * Revision 3.2.16.1  1996/06/16  21:21:09  vnguyen
 * Finally committing Brasil to California_branch
 *
 * Revision 3.2  1996/02/15  18:03:12  gtaylor
 * CSCdi45779:  Leased Line ISDN work.
 *
 * Revision 3.1  1996/02/01  05:14:33  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 1: Create copies of the files to be moved in their new location.
 *
 * Revision 3.3  1996/01/29  07:00:58  hampton
 * Move parser command defines from the common "exec" file to their
 * respective parser_defs_xxx.h files.  [CSCdi47717]
 *
 * Revision 3.2  1995/11/17  18:51:14  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:55:32  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/10/17  22:17:10  rbeach
 * CSCdi36915:  voice calls are not handled correctly over ISDN.
 * Allow configuration for data over voice on both outgoing and
 * incoming calls. For outgoing there is a new option for the
 * dialer map command called class. For incoming there is a new
 * interface option "isdn incoming-voice data". This will allow
 * the isdn code to answer incoming voice calls.
 *
 * Revision 2.1  1995/06/07  22:30:18  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 *  ISDN commands
 */
#define PARSER_ISDN_BCHANNELS            1
#define PARSER_ISDN_TEI                  2
#define PARSER_ISDN_SWITCH               3
#define PARSER_ISDN_LEASED_LINE          4
#define ISDN_SWITCH_CCITT                5
#define ISDN_SWITCH_BASIC1TR16           6
#define ISDN_SWITCH_PRIMARY1TR16         7
#define ISDN_SWITCH_NTT                  8
#define ISDN_SWITCH_KDD                  9
#define ISDN_SWITCH_VN2                 10
#define ISDN_SWITCH_BASICDMS100         11
#define ISDN_SWITCH_PRIMARYDMS100       12
#define ISDN_SWITCH_BASIC5ESS           13
#define ISDN_SWITCH_PRIMARY5ESS         14
#define ISDN_SWITCH_PRIMARY4ESS         15
#define ISDN_SWITCH_BASIC_NET3          16
#define ISDN_SWITCH_NONE                17
#define ISDN_SPID2_STRING               18
#define ISDN_SPID_STRING                19
#define ISDN_CALLER_STRING              20
#define ISDN_ANSWER1_STRING             21
#define ISDN_ANSWER2_STRING             22
#define ISDN_CALLING_NUMBER_STRING      23
#define ISDN_T200_TIMER			24
#define ISDN_N200_RETRIES		25
#define ISDN_SWITCH_PRIMARYNTT       	26	
#define ISDN_NOT_END_TO_END				27
#define ISDN_VOICE_DATA 	        28
#define ISDN_VOICE_MODEM 	        29
#define ISDN_FASTROLLOVER_DELAY         30
#define ISDN_MODEM_BUSY_CAUSE           31
#define ISDN_SENDING_COMPLETE           32
#define ISDN_TWAIT_DISABLED             33

/* These must be 0 and 1 */
#define ISDN_TEI_FIRST_CALL              0
#define ISDN_TEI_POWERUP                 1


/* ISDN show commands */
enum {
    SHOW_ISDN_CHANNELS,
    SHOW_ISDN_MEMORY,
    SHOW_ISDN_TIMERS,
    SHOW_ISDN_STATUS,
    SHOW_ISDN_HISTORY,
    SHOW_ISDN_ACTIVE,
};

/* fast rollover delay defs */
#define ISDN_FASTROLLOVER_DELAY_MIN  1
#define ISDN_FASTROLLOVER_DELAY_MAX  45
