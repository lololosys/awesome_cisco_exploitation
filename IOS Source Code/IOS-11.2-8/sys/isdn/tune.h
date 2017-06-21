/* $Id: tune.h,v 3.3.6.3.34.1 1996/08/15 02:23:16 hshi Exp $
 * $Source: /release/112/cvs/Xsys/isdn/tune.h,v $
 *------------------------------------------------------------------
 * tune.h  Tunable system parameters 
 *
 * March 1992, Ronnie Kon
 *
 * Copyright (c) 1992-1997 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: tune.h,v $
 * Revision 3.3.6.3  1996/06/16  21:13:33  vnguyen
 * Finally committing Brasil to California_branch
 *
 * Revision 3.3.6.2  1996/05/01  14:51:32  hrobison
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
 * Revision 3.3.6.1  1996/03/21  22:51:47  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.2.2.2  1996/03/18  23:33:01  mbeesley
 * Branch: ELC_branch
 * Sync to V111_1_3
 *
 * Revision 3.2.2.1  1996/02/16  03:42:15  mbeesley
 * CSCdi49179:  ISDN subsystem does not compile on Predator
 * Branch: ELC_branch
 *
 * Revision 3.3  1996/03/06  16:10:49  dclare
 * CSCdi47302:  segV exception in ClearTimer/LIF_StopTimer. Increase the
 * number of blocks and reflect change in show isdn memory.
 *
 * Revision 3.2  1995/11/17  17:42:43  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:15:22  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:18:03  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
/************************************************************************
*************************************************************************

MODULE:       tune.h                                                  
DESCRIPTION:  Tunable system parameters                               
VERSION:      3.21                                                    

*************************************************************************
****copyright_c**********************************************************

                         COPYRIGHT STATEMENT

                        Copyright (c) 1991,92
                          by Telenetworks
                        Petaluma, CA   94952
                           (707) 778-8737

All rights reserved. Copying, compilation, modification, distribution
or any other use whatsoever of this material is strictly prohibited
except in accordance with a Software License Agreement with
Telenetworks.

****copyright_c**********************************************************
****history**************************************************************
****history**************************************************************
*************************************************************************

GENERAL NOTES

*************************************************************************
*************************************************************************
*/

/* Sizes of memory blocks
 */
#define DEFAULT_N201       260                /* max length of I field */
#define HEADER_LEN         BLKSIZE(4)
#define SML_INFO_LEN       BLKSIZE(28)
/*
 * we need 1 extra byte to use to send a long frame to t'works,
 * and one beyond that to guarantee even alignment
 */
#define LRG_INFO_LEN       BLKSIZE(HEADER_LEN + DEFAULT_N201 + 2)

/************************************************************************
 * NOTE:
 * This file used to have to be tuned for all the platforms. It still
 * needs tuning, but the number of ISDN memory pools has been removed.
 * Users need to update the MAX_DSL value for each particular platform.
 * The internal mailbox sizes are based on the MAX_DSL value, except on
 * the midrange boxes, Brasil causes a few problems.
 * Values which may need tuning:
 *      MAX_DSL     - maximum possible interfaces per platform
 *      xx_QSIZE    - queue sizes for driver, modem and dialer interfaces
 *      xx_mb_size  - internal mailboxes used with the ISDN code
 *
 ************************************************************************/

/*
 * The PAN def is for 3000 and 2500 series units with a 
 * single ISDN BRI interface, the C1000 def is for, well...
 * Sapphire, a 1E1B box and Volcano.
 */
#if defined(PAN) || defined(SAPPHIRE)

/* Max number of Digital Subscriber Loops, or subscriber interface lines.
 * The greater the value of MAX_DSL, the more memory will be consumed
 * in tables and other data structures. Default is 1.
 */
#define MAX_DSL               1

/*
 * Defines the number of internal messages that can be passed between
 * internal processes. Moved here from execuser.h
 */
#define cc_mb_size      (10 * MAX_DSL)
#define L3_mb_size      (10 * MAX_DSL)
#define L2_mb_size      (10 * MAX_DSL)
#define hdlc_mb_size    (10 * MAX_DSL)
#define host_mb_size    (10 * MAX_DSL)
#define me_mb_size      (10 * MAX_DSL)

/*
 * For initialization of message queues
 */
#define MAX_ROUTER_QSIZE  30
#define MAX_DRIVER_QSIZE  30
#define MAX_MODEM_QSIZE    0              /* not used on this platform */

/* The maximum number of mail descriptors represents the total number
 * of pieces of mail that may be in all the mailboxes at a given time.
 * Value must be greater than 2.
 */
#define MAX_MAIL_DESCRIPTORS  60

/* The maximum number of timers represents the total number of
 * outstanding timers that are running at any given time.
 * Value must be greater than 2.
 */
#define MAX_TIMERS            30

/* The maximum number of timers that will be added to the LIF timer table.
 */
#define LIF_MAX_TIMERS        120

/* Max number of logical Hosts or separate Host Tasks.  The Host Specifier
 * is the index used for the Host CB table in Call Control for BRI
 * actually it is the number of TEIs for dms and ni1 implementation
 */
#define MAX_HOSTS             2

/* Max number of voice calls
 */
#define MAX_VOICE_CALLS       2

/* Max number of data calls specifies:
 *  1) the number of PLCBs in P3
 *  2) the number of XCB's in CC_X25
 */
#define MAX_DATA_CALLS        2

/* Max number of data links.  A data link is addressed by DSLCID/CES.
 * Valid range is 1 to 223.  See P3 for use.
 */
#define MAX_DATA_LINKS        2

/* Max number of L2 frames in queue
 * NOTE: MAX_L2_QUEUE_LEN must be large enough to hold the maximum
 * retransmission queue (k max) AND the maximum 'to transmit' queue
 * (application dependent).  Also, MAX_L2_QUEUE_LEN must be a power
 * of 2 between 2 and 128 (because queue index values are calculated
 * based on state variables VA and VS).
 */
#define MAX_L2_QUEUE_LEN      8

/*
 * This seems to be causing a problem with Disconnects, running out of
 * packets. Make it platform dependant as well.
 */
#define MAX_L3_QUEUE_LEN      4      /* max packets in queue */

#endif /*** PAN ***/

/*********************************************/
/*
 * The following configuration is for the c4000, c4500, c7100, and as5200
 */
#if defined(XX) || defined(SIERRA) || defined(PREDATOR) || defined(BRASIL)

/* Max number of Digital Subscriber Loops, or subscriber interface lines.
 * The greater the value of MAX_DSL, the more memory will be consumed
 * in tables and other data structures.
 */
#if defined(BRASIL)
#define MAX_DSL               2

/*
 * Defines the number of internal messages that can be passed between
 * internal processes. Moved here from execuser.h The Brasil causes
 * some complication to using a multiplier.
 */
#define cc_mb_size      130
#define L3_mb_size      130
#define L2_mb_size      130
#define hdlc_mb_size    130
#define host_mb_size    130
#define me_mb_size      130

#else

#define MAX_DSL               16
/*
 * Defines the number of internal messages that can be passed between
 * internal processes. Moved here from execuser.h
 */
#define cc_mb_size      (10 * MAX_DSL)
#define L3_mb_size      (10 * MAX_DSL)
#define L2_mb_size      (10 * MAX_DSL)
#define hdlc_mb_size    (10 * MAX_DSL)
#define host_mb_size    (10 * MAX_DSL)
#define me_mb_size      (10 * MAX_DSL)

#endif

/* for initialization of message queues */
#define MAX_ROUTER_QSIZE  70
#define MAX_DRIVER_QSIZE  30
#if defined(BRASIL)
#define MAX_MODEM_QSIZE   70            
#else
#define MAX_MODEM_QSIZE    0              /* not used on this platform */
#endif

/* The maximum number of mail descriptors represents the total number
 * of pieces of mail that may be in all the mailboxes at a given time.
 * Value must be greater than 2.
 */
#define MAX_MAIL_DESCRIPTORS  1200

/* The maximum number of timers represents the total number of
 * outstanding timers that are running at any given time.
 * Value must be greater than 2.
 */
#define MAX_TIMERS            300

/* The maximum number of timers that will be added to the LIF timer table.
 */
#define LIF_MAX_TIMERS        1200

/* Max number of logical Hosts or separate Host Tasks.  The Host Specifier
 * is the index used for the Host CB table in Call Control for BRI
 * actually it is the number of TEIs for dms and ni1 implementation
*/
#define MAX_HOSTS             4

/* Max number of voice calls
 */
#define MAX_VOICE_CALLS       2

/* Max number of data calls specifies:
 *  1) the number of PLCBs in P3
 *  2) the number of XCB's in CC_X25
 */
#define MAX_DATA_CALLS        2

/* Max number of data links.  A data link is addressed by DSLCID/CES.
 *  Valid range is 1 to 223.  See P3 for use.
 */
#define MAX_DATA_LINKS        2

/* Max number of L2 frames in queue
 * NOTE: MAX_L2_QUEUE_LEN must be large enough to hold the maximum
 * retransmission queue (k max) AND the maximum 'to transmit' queue
 * (application dependent).  Also, MAX_L2_QUEUE_LEN must be a power
 * of 2 between 2 and 128 (because queue index values are calculated
 * based on state variables VA and VS).
 */
#define MAX_L2_QUEUE_LEN      32

/*
 * This seems to be causing a problem with Disconnects, running out of
 * packets. Make it platform dependant as well.
 */
#define MAX_L3_QUEUE_LEN      50      /* max packets in queue */

#endif /* XX, C4500, C7100 and AS5200 */


/*********************************************/
#if defined(RP1) || defined(RSP)	/* the 7xxx platforms with PRI */
/*
 * Max number of Digital Subscriber Loops, or subscriber interface lines.
 * The greater the value of MAX_DSL, the more memory will be consumed
 * in tables and other data structures.
 */
#define MAX_DSL           10

/* for initialization of message queues */
#define MAX_ROUTER_QSIZE 100
#define MAX_DRIVER_QSIZE  30
#define MAX_MODEM_QSIZE    0              /* not used on this platform */

/*
 * Defines the number of internal messages that can be passed between
 * internal processes. Moved here from execuser.h
 */
#define cc_mb_size      (13 * MAX_DSL)
#define L3_mb_size      (13 * MAX_DSL)
#define L2_mb_size      (13 * MAX_DSL)
#define hdlc_mb_size    (13 * MAX_DSL)
#define host_mb_size    (13 * MAX_DSL)
#define me_mb_size      (13 * MAX_DSL)

/* The maximum number of mail descriptors represents the total number
 * of pieces of mail that may be in all the mailboxes at a given time.
 * Value must be greater than 2.
 */
#define MAX_MAIL_DESCRIPTORS  (120 * MAX_DSL)

/* The maximum number of timers represents the total number of
 * outstanding timers that are running at any given time.
 * Value must be greater than 2.
 */
#define MAX_TIMERS            (30 * MAX_DSL)

/* The maximum number of timers that will be added to the LIF timer table.
 */
#define LIF_MAX_TIMERS       (250 * MAX_DSL)

/* Max number of logical Hosts or separate Host Tasks.  The Host Specifier
 * is the index used for the Host CB table in Call Control for BRI
 * actually it is the number of TEIs for dms and ni1 implementation
 */
#define MAX_HOSTS             2

/* Max number of voice calls
 */
#define MAX_VOICE_CALLS       2

/* Max number of data calls specifies:
 *  1) the number of PLCBs in P3
 *  2) the number of XCB's in CC_X25
 */
#define MAX_DATA_CALLS        2

/* Max number of data links.  A data link is addressed by DSLCID/CES.
 * Valid range is 1 to 223.  See P3 for use.
 */
#define MAX_DATA_LINKS        2

/* Max number of L2 frames in queue
 * NOTE: MAX_L2_QUEUE_LEN must be large enough to hold the maximum
 * retransmission queue (k max) AND the maximum 'to transmit' queue
 * (application dependent).  Also, MAX_L2_QUEUE_LEN must be a power
 * of 2 between 2 and 128 (because queue index values are calculated
 * based on state variables VA and VS).
 */
#define MAX_L2_QUEUE_LEN      32

/*
 * This seems to be causing a problem with Disconnects, running out of
 * packets. Make it platform dependant as well.
 */
#define MAX_L3_QUEUE_LEN      40      /* max packets in queue */

#endif /* RP1 */

/*********************************************/
/*
 * The following configuration is for the c3600
 */
#if defined(C3600)
/* Max number of Digital Subscriber Loops, or subscriber interface lines.
 * The greater the value of MAX_DSL, the more memory will be consumed
 * in tables and other data structures. 
 */
#define MAX_DSL               32

/* for initialization of message queues */
#define MAX_ROUTER_QSIZE 100
#define MAX_DRIVER_QSIZE  30
#define MAX_MODEM_QSIZE    0              /* not used on this platform */
 
/*
 * Defines the number of internal messages that can be passed between
 * internal processes. Moved here from execuser.h
 */
#define cc_mb_size      (10 * MAX_DSL)
#define L3_mb_size      (10 * MAX_DSL)
#define L2_mb_size      (10 * MAX_DSL)
#define hdlc_mb_size    (10 * MAX_DSL)
#define host_mb_size    (10 * MAX_DSL)
#define me_mb_size      (10 * MAX_DSL)
 
/* The maximum number of mail descriptors represents the total number
 * of pieces of mail that may be in all the mailboxes at a given time.
 * Value must be greater than 2.
 */
#define MAX_MAIL_DESCRIPTORS  (MAX_DSL * 60)

/* The maximum number of timers represents the total number of
 * outstanding timers that are running at any given time.
 * Value must be greater than 2.
 */
#define MAX_TIMERS            (MAX_DSL * 30)

/* The maximum number of timers that will be added to the LIF timer table.
 */
#define LIF_MAX_TIMERS        (MAX_DSL * 60)

/* Max number of logical Hosts or separate Host Tasks.  The Host Specifier
 * is the index used for the Host CB table in Call Control for BRI
 * actually it is the number of TEIs for dms and ni1 implementation
*/
#define MAX_HOSTS             4

/* Max number of voice calls
 */
#define MAX_VOICE_CALLS       2

/* Max number of data calls specifies:
 *  1) the number of PLCBs in P3
 *  2) the number of XCB's in CC_X25
 */
#define MAX_DATA_CALLS        2

/* Max number of data links.  A data link is addressed by DSLCID/CES.
 *  Valid range is 1 to 223.  See P3 for use.
 */
#define MAX_DATA_LINKS        2

/* Max number of L2 frames in queue
 * NOTE: MAX_L2_QUEUE_LEN must be large enough to hold the maximum
 * retransmission queue (k max) AND the maximum 'to transmit' queue
 * (application dependent).  Also, MAX_L2_QUEUE_LEN must be a power
 * of 2 between 2 and 128 (because queue index values are calculated
 * based on state variables VA and VS).
 */
#define MAX_L2_QUEUE_LEN      (MAX_DSL * 8)

/*
 * This seems to be causing a problem with Disconnects, running out of
 * packets. Make it platform dependant as well.
 */
#define MAX_L3_QUEUE_LEN      (MAX_DSL * 4)    /* max packets in queue */

#endif /* C3600 */
