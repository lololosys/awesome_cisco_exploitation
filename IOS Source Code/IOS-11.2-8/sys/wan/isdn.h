/* $Id: isdn.h,v 3.3.62.6 1996/08/26 23:09:04 wmay Exp $
 * $Source: /release/112/cvs/Xsys/wan/isdn.h,v $
 *------------------------------------------------------------------
 * Prototypes for isdn.c
 *
 * January 1993, Dave Katz
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: isdn.h,v $
 * Revision 3.3.62.6  1996/08/26  23:09:04  wmay
 * CSCdi66907:  ISDN incoming calls getting rejected - enforce call ranges
 * for modem and dialer type calls
 * Branch: California_branch
 *
 * Revision 3.3.62.5  1996/06/16  21:21:07  vnguyen
 * Finally committing Brasil to California_branch
 *
 * Revision 3.3.62.4  1996/05/19  03:10:51  hrobison
 * CSCdi56813:  CSCdi46413 breaks platform independent file modularity
 * Branch: California_branch
 * With the excuse of changes required by the new compiler, take the
 * opportunity to remove platform dependancies, improve code
 * modularity and do some general code clean-up.
 *
 * Revision 3.3.62.3  1996/05/10  01:45:04  wmay
 * CPP and PPP half-bridge commit
 * Branch: California_branch
 *
 * Revision 3.3.62.2  1996/05/07  04:15:00  hrobison
 * CSCdi56472:  CSCdi46413 broke modularity if ISDN not included
 * Branch: California_branch
 * Quick fix: Change external reference to structure in ../isdn to
 * a registry call.  Code is scheduled for clean-up in CSCdi56813.
 *
 * Revision 3.3.62.1  1996/05/01  14:52:50  hrobison
 * CSCdi46413:  MIP when used as PRI must be in slot 0-4
 * Branch: California_branch
 * Change PRI initialization: for the 7xxx platform the
 * dsl id is now assigned independently of the MIP card
 * slot/subunit location.  The number of PRI interfaces
 * per router is still limited to 10.  The dsl assignment
 * for the 4xxx platform is based on the slot location
 * and corrected to allow only 1 dsl id per slot.
 *
 * Read bug release notes for details on how the ISDN
 * debug output and ISDN show commands have changed.
 * The debug output now uses the hw_short_namestring
 * rather than PRI<dsl #> or BRI<dsl #>.  The show commands
 * now take the interface name or the dsl id as input.
 *
 * Revision 3.3  1995/11/17  18:03:01  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:50:06  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:49:43  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/10/21  06:37:50  bdas
 * CSCdi36921:  ISDN MIB needs to contain info about active calls
 *
 * Revision 2.2  1995/06/15  23:09:35  bdas
 * CSCdi33538:  incomplete/wrong object instances returned; ISDN MIB,
 * Call History Mib.
 *
 * Revision 2.1  1995/06/07  23:18:25  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#define T1_MAX_PRI 24
#define T1_D_CHAN_PRI 23
#define E1_D_CHAN_PRI 15
#define VISIBLE_T1_ISDN_BANDWIDTH 64	/* BRI visible bandwidth */
#define T1_ISDN_DELAY 2000              /* BRI Serial delay */
#define ISDN_MAX_CHAN 31		/* 24 channels in a T1, 31 on E1 */ 
#define ISDN_MIN_CHAN 1		        /* first channel for PRI */
#define ISDN_FASTROLLOVER_DELAY_DEF 0   /* default rollover delay */

/* The dsl id is no longer based
 * on slot/subunit number.
 */
#define ISDN_NO_DSL -1
#define ISDN_DSL_RESERVED -2

/*
 * ISDN speeds
 *
 * These constants are used throughout the signalling software to represent
 * channel bandwidths internally.  They should not be confused with the
 * IDB bandwidth parameters, etc. (though they happen to be the same).
 */
#define ISDN_SPEED_64 64		/* 64 Kbps */
#define ISDN_SPEED_56 56		/* 56 Kbps */
#define ISDN_SPEED_16 16		/* 16 Kbps */
#define ISDN_SPEED_VOICE 256            /* flag for incoming voice */

#define ISDN_DIALER_CALL_ID_MIN 0x8000
#define ISDN_DIALER_CALL_ID_MAX 0x9fff
#define ISDN_MODEM_CALL_ID_MIN  0xa000
#define ISDN_MODEM_CALL_ID_MAX  0xafff

static inline ushort call_id_for_dialer (ushort *Call_id)
{
    ushort ret_val = *Call_id;
    ret_val++;
    if ((ret_val < ISDN_DIALER_CALL_ID_MIN) ||
	(ret_val > ISDN_DIALER_CALL_ID_MAX))
	ret_val = ISDN_DIALER_CALL_ID_MIN;
    *Call_id = ret_val;
    return(ret_val);
}

static inline ushort call_id_for_modem (ushort *Call_id)
{
    ushort ret_val;
    ret_val = *Call_id;
    ret_val++;
    if ((ret_val < ISDN_MODEM_CALL_ID_MIN) ||
	(ret_val > ISDN_MODEM_CALL_ID_MAX))
	ret_val = ISDN_MODEM_CALL_ID_MIN;
    *Call_id = ret_val;
    return(ret_val);
}
extern void isdn_setup(hwidbtype *);
extern void isdn_parser_init(void);
extern void show_isdn_status(int);
extern ulong DEFAULT_T200;
extern ulong DEFAULT_N200;
/*
 * isdn_setup.c
 */
extern void isdn_setup_global2(setup_struct *sp);
extern void isdn_call_connect(hwidbtype *);
extern void isdn_call_disconnect(hwidbtype *);
extern void isdn_mib_update(hwidbtype *, char *, boolean);
extern char *isdn_get_number(hwidbtype *hwidb, int port);

extern hwidbtype *isdn_idblist[];
/*
 * isdn.c
 */
extern int isdn_fork;
