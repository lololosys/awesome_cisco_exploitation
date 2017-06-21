/* $Id: snmp_interface.h,v 3.4.4.7 1996/08/13 14:58:34 tylin Exp $
 * $Source: /release/112/cvs/Xsys/h/snmp_interface.h,v $
 *------------------------------------------------------------------
 * S N M P _ I N T E R F A C E . H
 *
 * August 1994, Nick Thille
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 * 
 * Definition of interface accounting records 
 *------------------------------------------------------------------
 * $Log: snmp_interface.h,v $
 * Revision 3.4.4.7  1996/08/13  14:58:34  tylin
 * CSCdi61338:  ATM LEC sub-interf missing from PIM-MIB
 *    Add ifmib_swidb_get() declaration.
 * Branch: California_branch
 *
 * Revision 3.4.4.6  1996/07/17  06:51:26  bchan
 * CSCdi63198:  Fix appn/dlur mib for tn3270 server
 * Branch: California_branch
 *
 * Revision 3.4.4.5  1996/05/21  09:46:50  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.4.4.4  1996/05/21  06:27:53  mordock
 * Branch: California_branch
 * Improve snmp modularity via creating services to register interfaces
 * and chassis cards.
 * Add syslog mib.
 *
 * Revision 3.4.4.3  1996/05/17  11:16:14  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.3.40.5  1996/05/05  23:05:36  ppearce
 * Sync to IbuMod_Calif_baseline_960504
 *
 * Revision 3.3.40.4  1996/04/26  07:53:01  dstine
 * Branch: IbuMod_Calif_branch
 * - commit of IBU modularity work, phase 1.
 *
 * Revision 3.3.40.3  1996/04/03  14:26:03  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.3.40.2  1996/03/26  00:23:34  bchan
 * CSCdi52555:  Allow cip index to be passed in the community string
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.3.40.1  1996/03/17  17:48:06  ppearce
 * Sync IbuMod_Calif_branch to V111_1_3
 *
 * Revision 3.4.4.2  1996/04/30  23:15:13  sdurham
 * CSCdi46300:  Interface missing from MIB
 * Branch: California_branch
 *          add atm and aal5 layers to the ifmib
 *
 * Revision 3.4.4.1  1996/03/18  19:37:32  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.20.3  1996/03/16  06:44:53  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.3.20.2  1996/03/13  01:20:39  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.3.20.1  1996/02/20  14:17:33  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.4  1996/03/09  01:55:31  sdurham
 * CSCdi51080:  Remove unneeded object from tty data structure
 *             tty->snmp_if_index
 *
 * Revision 3.3  1995/11/20  22:23:40  sdurham
 * CSCdi42520:  IF MIB breaks modularity in oddball images
 *        fixed IFMIB subsystem, and moved linkUp/Down trap code to IFMIB.
 *
 * Revision 3.2  1995/11/17  09:13:26  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:36:08  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/11/08  20:59:02  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.2  1995/07/17  07:31:22  bchan
 * CSCdi34760:  Ifindex usage incorrect
 *
 * Revision 2.1  1995/06/07  20:36:32  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __SNMP_INTERFACE_H__
#define __SNMP_INTERFACE_H__

/*
 * ensure that the required include files are included.
 */

#include "interface.h"                  /* hwidbtype, idbtype, iabtype  */
#include "ttysrv.h"			/* tt_soc */

extern queuetype *snmpidbQ;		/* head of list of SNMP IDBs */

/*
 * SNMP interface type values for snmp_if_type
 */

enum SNMP_IF_DATA_TYPE {
    SNMP_IF_NONE   = 0x00,		/* no interface, NULL block */
    SNMP_IF_HWIDB  = 0x01,		/* i/f is a hardware IDB */
    SNMP_IF_SWIDB  = 0x02,		/* i/f is a software IDB only */
    SNMP_IF_TTY    = 0x04,		/* i/f is a TTY */
    SNMP_IF_CDB    = 0x08,		/* i/f is a CDB */
    SNMP_IF_IAB    = 0x10,		/* no i/f, acccounting record only */
    SNMP_IF_SUBIAB = 0x20		/* sub i/f, acccounting record only */
};

struct snmpifptrtype_ {
    union {
	hwidbtype       *hwidb;		/* hardware idb pointer */
	idbtype         *swidb;		/* software idb pointer */
	tt_soc          *tty;		/* tty line pointer */
	cdbtype         *cdb;		/* cdb pointer */
	iabtype         *iab;		/* i/f accounting block pointer */
	subiabtype      *subiab;	/* sub i/f accounting block pointer */
	} ifptr;
};

struct snmpidbtype_ {
    struct snmpidbtype_   *next;	/* ptr to next snmpidb in queue */
    long                   snmp_if_index; /* what is the SNMP i/f index */
    enum SNMP_IF_DATA_TYPE snmp_if_struct; /* what kind of interface is it? */
    snmpifptrtype          snmp_if_ptr;	/* pointer to the interface */
};



/*
 * Interface Accounting Block
 *
 * PLEASE keep this long word aligned for speed considerations. Add padding
 * if you must to get 32 bit variables stored on long word boundaries.
 *
 * This data structure contains the accounting information for an
 * interface.  It will be embedded in the hardware IDB, with accounting
 * parameters being moved from the IDB to the IAB.  The reason for
 * this is so you can have an IAB for an common_str_interface that you might
 * not want to have a full IDB for, such as a FLEX interface.
 *
 * IAB function vectors.
 *
 *     (*iab_get_stats_t)(iabtype *iab) [required]
 *         Go out, touch any hardware (possibly remote) and do
 *         whatever is required so that the stats in the iab cells are
 *         current.  Returns TRUE if succesful, FALSE otherwise.
 *     (*iab_iftype_get_t)(iabtype *iab) [required]
 *         returns the interface type.
 *     (*iab_hw_if_up_t)(iabtype *iab) [required]
 *         returns TRUE in hardware up, FALSE if down.
 *     (*iab_rx_cumbytes_t)(iabtype *iab) [required]
 *         returns the cumulative total number of bytes received on
 *         the interface since system restart.
 *     (*iab_inputs_t)(iabtype *iab) [required]
 *         returns the cumulative total number of packets received on
 *         the interface since system restart.
 *     (*iab_tx_cumbytes_t)(iabtype *iab) [required]
 *         returns the cumulative total number of bytes sent on the
 *         interface since system restart.
 *     (*iab_outputs_t)(iabtype *iab) [required]
 *         returns the cumulative total number of packets received on
 *         the interface since system restart.
 *     (*iab_oqueue_len_t)(iabtype *iab) [required]
 *         returns the length of the output queue.  (Number of output
 *         packets queued.)
 *     (*iab_update_ifalias_t)(iabtype *iab, iab_ifalias_t ifalias,
 *             boolean set) [optional]
 *         performs operations on the ifAlias value for this interface.
 *         if set == TRUE, ifalias contains the new value for ifAlias.
 *         if set == FALSE, the current value of ifAlias for this
 *         interface should be placed in the ifalias struct.
 *         in either case, the return value should indicate whether
 *         the set or retrieval of ifAlias succeeded.
 */

/* 
 * Prototypes for IAB vectors
 */
typedef boolean (*iab_get_stats_t)(iabtype *);
typedef long (*iab_iftype_get_t)(iabtype *);
typedef boolean (*iab_hw_if_up_t)(iabtype *);
typedef ulong (*iab_rx_cumbytes_t)(iabtype *);
typedef ulong (*iab_inputs_t)(iabtype *);
typedef ulong (*iab_tx_cumbytes_t)(iabtype *);
typedef ulong (*iab_outputs_t)(iabtype *);
typedef ulong (*iab_oqueue_len_t)(iabtype *);
typedef struct iab_ifalias_ {
    unsigned char *value;
    uint length;
} iab_ifalias_t;
typedef boolean (*iab_update_ifalias_t)(iabtype *, iab_ifalias_t *, boolean);

struct iabtype_ {
    /*
     * Function vectors & data pointer
     */
    iab_get_stats_t get_stats;	    /* vector to get fresh stats into iab */
    iab_iftype_get_t iftype_get;    /* get the interface type for SNMP */
    iab_hw_if_up_t hw_if_up;	    /* vector for is this i/f up? */
    iab_rx_cumbytes_t rx_cumbytes;  /* vector for input bytes on iab */
    iab_inputs_t inputs;	    /* vector for inputs on this iab */
    iab_tx_cumbytes_t tx_cumbytes;  /* vector for output bytes on iab */
    iab_outputs_t outputs;	    /* vector to get outputs on this iab */
    iab_oqueue_len_t oqueue_len;    /* vector to return output Q length */
    iab_update_ifalias_t update_ifalias; /* vector to update ifAlias */
    void *data_ptr;		    /* data ptr, used by vectored functions */

    /*
     * Generic data.
     */
    char *if_namestring;	    /* copy of firstsw->namestring */
    long snmp_if_index;		    /* snmp i/f index if registered, else 0 */
    uchar hardware[8];		    /* the def/cur hardware address (MAC) */
    ushort maxmtu;		    /* largest possible packet for IP (and
				     * similarly encapsulated protocols) */
    ulong status;		    /* hardware status bits */
    enum IDB_STATES state;	    /* current interface state */
    sys_timestamp state_time;	    /* and when it was last changed */
    int output_qcount;		    /* size of all priority output queues */
    ulong *visible_bandwidth_ptr;   /* bandwidth in Kbps */

    /*
     * dial on demand fields
     */
    int dialer;				/* what dialer type to use */
    boolean dialer_spoofing_int;    /* flag shows if spoofing interface */
    boolean dialer_spoofing_prot;   /* flag shows if spoofing line protocol */
    boolean inloopback;		    /* interface actually is looped */

    /*
     * Generic interface statistics.
     */
    ulong input_broad;		    /* no. broadcasts received */
    ulong input_crc;		    /* no. received with CRC errors */
    ulong input_error;		    /* no. damaged packets received */
    ulong input_frame;		    /* no. rcvd with framing/alignment error*/
    ulong input_overrun;	    /* no. overruns and resource errors */
    ulong giants;		    /* no. too large packets received */
    ulong nobuffers;		    /* no. dropped because no buffers */
    ulong output_broad;		    /* no. broadcasts transmitted */
    ulong output_deferred;	    /* tx delayed but successful */
    ulong output_error;		    /* no. packets erred on output */
    ulong outputdrops;		    /* all output drops (inc. fast-switch) */
    ulong transitions;		    /* no. of carrier transitions */
    ulong rx_frames[ACCT_PROTO_LAST];	/* no. (slow only) packets */
    ulong inputs_fast[ACCT_PROTO_LAST]; /* no. packets in fast path */

    /*
     * Ethernet interface statistics.
     */
    ulong output_collision;	    /* no. of packets collided on output */
    ulong output_excessive_collision; /* Number of excessive collisions */
    ulong output_late_collision;    /* no. of late collision errors */
    ulong output_more_collision;    /* no. of multi collision errors, tx ok */
    ulong output_one_collision;	    /* no. of packets w/ just one collision */
};

/*
 * Sub Interface Accounting Block
 *
 * PLEASE keep this long word aligned for speed considerations. Add padding
 * if you must to get 32 bit variables stored on long word boundaries.
 *
 * This data structure contains the accounting information for a sub
 * interface, used by snmp IF-MIB.  
 *
 * Note - it is important that the pointers used here point to the
 * same data strutures used by the CLI so snmp and CLI info track.
 * These pointers shouldn't use mallocs unless the they aren't free'd
 * until the interface is deregisterd!
 * If you are hanging data structures off of the subiab which should
 * be freed when the interface is deregistered, the subiab_cleaner_t
 * callback should be set.  This function will be called when the
 * interface is deregistered, just prior to freeing the subiab.
 */

/*
 * Prototoypes for SUBIAB vectors
 */
typedef void (*subiab_cleaner_t)(subiabtype *);

struct subiabtype_ {
    void  *data_ptr;	    /* data ptr, used by vectored functions */

    snmpifptrtype  master_ptr;	    /* used to point back at
				       controlling idb (hwidb or
				       swidb) -only for sublayers that
				       have no associated idbs (ie
				       aal5 layer) */

    enum SNMP_IF_DATA_TYPE master_type; /* what kind of interface is 
					   master_ptr */

    subiab_cleaner_t cleaner;      /* cleanup callback */

    /*
     * Generic data.
     */
    char *if_descrstring;	    /* copy of firstsw->namestring ifDescr */
    char  *if_name;	            /* name of top level  physical interface */
    long snmp_if_index;		    /* snmp i/f index if registered, else 0 */
    const uchar * ifPhysAddr;  	    /* physical address of this interface */
    uint *ifPhysAddrLen;	    /* length of phys address */
    uint ifType;                    /* IANAType of interface */
    ushort * maxmtu;		    /* largest possible packet for IP (and
				     * similarly encapsulated protocols) */

    sys_timestamp last_change;	    /* and when it was last changed */


};


#define MAX_SNMP_INTERFACES  0x7fffffff	/* Allow a full long's worth
					   of snmpidbs */
#define MAX_SNMPIDB_TYPES 10   /* for CASE registry */
/*
 * Macro to traverse all hardware IDBs
 */

#define FOR_ALL_SNMPIDBS(snmpidb) \
    for ((snmpidb) = (snmpidbtype *) snmpidbQ->qhead; (snmpidb); \
	 (snmpidb) = (snmpidb)->next) 

/*
 * signature of tester routines used in idb query routines
 */
typedef boolean (*ProcTest)(hwidbtype* hwidb, long* indexP);
typedef boolean (*ProcTestSwidb)(idbtype* swidb, long* indexP);
typedef boolean (*ProcTestSnmp)(snmpidbtype *snmpidb, long* indexP);

#endif	/* __SNMP_INTERFACE_H__ */

