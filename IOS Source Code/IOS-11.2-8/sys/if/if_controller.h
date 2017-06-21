/* $Id: if_controller.h,v 3.4.12.4 1996/08/13 02:18:46 gstovall Exp $
 * $Source: /release/112/cvs/Xsys/if/if_controller.h,v $
 *------------------------------------------------------------------
 * if_controller.h - 
 *     Controller definitions.(based on hes/controller.h by Ronnie Kon)
 *
 * Jan 30 1995, Manoj Leelanivas
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: if_controller.h,v $
 * Revision 3.4.12.4  1996/08/13  02:18:46  gstovall
 * CSCdi39484:  Giant packets output on ethernet interface
 * Branch: California_branch
 * Make sure we drop giants, even if they are just a wee bit too big.
 *
 * Revision 3.4.12.3  1996/07/08  22:31:01  mwu
 * CSCdi60922:  11.2 Core Features Commit
 * Branch: California_branch
 *
 * Revision 3.4.12.2  1996/06/26  03:04:01  sdurham
 * CSCdi54840:  Channelized T1 needs support for ds1 physical interface in
 * snmp
 * Branch: California_branch
 * add support in ifmib for cdbtype
 *
 * Revision 3.4.12.1  1996/05/06  23:23:32  sdurham
 * CSCdi54843:  RFC1406 MIB implementation has table indices starting at 0
 * Branch: California_branch
 *      initialize seed value for 4000 as is done for mip.
 *
 * Revision 3.4  1996/02/13  08:12:57  dstine
 * CSCdi48797:  Extraneous definitions in interface_private.h
 *
 * Revision 3.3  1995/11/17  09:27:53  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:37:53  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:50:05  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/11/08  21:03:01  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.2  1995/08/09  16:25:54  fred
 * CSCdi38126:  bandwidth command and fair-queueing result in 0 tql
 * Add a routine to find the idb given the cdb.
 *
 * Revision 2.1  1995/06/07  20:52:36  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#ifndef __IF_CONTROLLER_H__
#define __IF_CONTROLLER_H__


/*
 * Define for SLOT based CDB platforms.
 * (Platforms which go by slot:appl_no)
 */
#if defined(C7000) || defined(RSP)
#define SLOT_BASED_CDB_PLATFORM 	TRUE
#endif


/*
 *  Per-controller data structure. Note that a controller is defined
 *  to be something which provides physical media for data.
 *
 *  o  For the high-end, every applique on an IP will have a cdb
 *     structure.
 *  o  For the low-end, a single physical termination of multiple
 *     interfaces(like E1/T1) use the controller structure.
 */
enum pif_types_ {	/* the physical interface type */
    PIFT_UNKNOWN,
    PIFT_ETHERNET,
    PIFT_TOKENRING,
    PIFT_SERIAL,	
    PIFT_HSSI,
    PIFT_FDDI,
    PIFT_ATM,
    PIFT_T1,
    PIFT_CHANNEL,
    PIFT_E1,
    PIFT_FASTETHER,
};

/*
 * cdb->status bits definitions
 */
#define CDB_CONFIGURE	0x00000001	/* configurable controller */
#define	CDB_NOHARDWARE	0x00000002	/* board has been hot-removed */
#define	CDB_CRASHED	0x00000004	/* board has crashed */
#define CDB_EOIR_RESET	0x00000008	/* board is getting EOIR reset */

struct cdbtype_ {
    struct cdbtype_	*next;
    enum pif_types_	pif_type;	/* physical interface type */
    uint	        ctrlr_type;     /* type ? */
    int		        slot;           /* slot for high-end/midrange */
    int                 appl_no;        /* applique number */
    int                 unit;           /* unit # for all boxes */
    int                 subunit;        /* sub unit ? */
    int			encsize;        /* in words */
    enum IDBTYPE	idbtype;        /* idbtype which our IDBs will have */
    int			hw_if_index;    /* SNMP index (1-n)--see hwidb field */
    const char		*cdb_name;
    char		*cdb_namestring;
    char		*cdb_description;   /* user-given description */
    enum IDB_STATES	cdb_state;	    /* current interface state */

    /*
     * Function vectors off the controller.
     */
    void		(*reset)(struct cdbtype_ *);
    void		(*love_note)(struct cdbtype_ *, ushort);
    void		(*idb_init)(struct cdbtype_ *, hwidbtype *);
    void		(*clear_counts)(struct cdbtype_ *);

    /*
     * controller specific info
     */
    int			cdb_status;
    boolean		input_throttled;

    /*
     * platform specific handle
     */
    void *platform_hware_handle;
    

    /*
     * Platform specific stuff..could be moved later into
     * another void * block....
     */
    ushort		maxmtu;
    ushort		max_buffers;
    ulong		buffersize;
    ulong		max_buffer_size;
    ushort		cbus_burst_count;
    short		mci_txcount;
    boolean		mci_txcount_enabled;
    long		buffer_pool;
    ulong		bandwidth_def;
    int			mci_txcount_default;
    int  		mci_txqloc;

    uint snmp_if_index;		    /* snmp i/f index if registered, else 0 */

};

/*
 *  Walk the CDB Queue.
 *  Note that this only looks at cdb's presently installed in the system.
 *  It does not look at the removed_cdbQ (the cdbs with CDB_NOHARDWARE lit)
 */
#define FOR_ALL_CDBS(cdb)	\
     for ((cdb) = (cdbtype *)cdbQ->qhead ; (cdb) != NULL ; (cdb) = (cdb)->next)

/*
 * Controller specific stuff to be exported.
 */
extern queuetype 	*const cdbQ;		/* Q of active cdbs */
extern queuetype 	cdbpreQ;                /* Preallocated Q of cdbs */
extern queuetype 	*const removed_cdbQ;	/* Discarded Q of cdbs */
extern cdbtype          **cdb_list;             /* Storage for all cdb ptrs*/
extern uint             max_ctrlr_interfaces;   /* Max applique in slot
						   based platforms and
						   Max units on unit based
						   platforms */
extern uint             ctrlr_units;            /* total # of controllers
						   in the box; used by
						   unit based platforms*/



extern unsigned int snmp_dsx1LineIndex_seed; /* dump when we support ifIndex */
/*
 * Inline for calculating the cdb index, given slot, appl_no
 */
static inline uint controller_index_inline (int slot,
					    int appl_no,
					    int max_ctrlrs)
{
    return(slot * max_ctrlrs + appl_no);
}

/*
 * Inline for referencing idb from a cdb, if it exists.
 */
static inline hwidbtype* controller_find_from_cdb (cdbtype *cdb)
{
    hwidbtype *idb = NULL;
    
    FOR_ALL_HWIDBS(idb) 
	if ((cdb->slot == idb->slot) && (cdb->appl_no == idb->slotunit))
	    break;
    return(idb);    /* Note that this could return NULL */
}

/*
 * Inline for referencing cdb from a idb, if it exists.
 */
static inline cdbtype* controller_find_from_idb (hwidbtype *idb)
{
    cdbtype *cdb = NULL;
    
    FOR_ALL_CDBS(cdb) 
	if ((cdb->slot == idb->slot) && (cdb->appl_no == idb->slotunit))
	    break;
    return(cdb);    /* Note that this could return NULL */
}


/*
 * Common function prototypes for controllers.
 */
extern void    controller_init(boolean, uint, uint);
extern cdbtype *controller_find(int, int);
extern cdbtype *controller_find_for_SNMP_hw_if(long *,
					       enum pif_types_, boolean);
extern void    controller_remove(cdbtype *);
extern void    controller_destroy(cdbtype *);
extern char    *print_cdbstate(enum IDB_STATES state);
extern void    controller_init_common(cdbtype *cdb);
extern const   char *pif_name(enum pif_types_ pif_type);
extern void init_cdb_name(cdbtype *);

/*
 * controller_chain.c
 */
extern void    controller_parser_init(void);

#endif	/* __IF_CONTROLLER_H__ */

