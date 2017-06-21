/* $Id: ilmi_private.h,v 3.5.6.4 1996/08/07 08:56:46 snyder Exp $
 * $Source: /release/112/cvs/Xsys/atm/ilmi_private.h,v $
 *------------------------------------------------------------------
 *
 * March, 1995   Kartik Chandrasekhar
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ilmi_private.h,v $
 * Revision 3.5.6.4  1996/08/07  08:56:46  snyder
 * CSCdi65343:  more things that should be declared const
 *              char * [] -> char *const []
 *              char *    -> const char []
 *              savings 17,496 data space, 1,852 image space
 * Branch: California_branch
 *
 * Revision 3.5.6.3  1996/06/18  06:31:01  kartik
 * Branch: California_branch
 * CSCdi50421:  show atm ilmi command returns error when no interface
 * provided
 *              Added more details to show.
 *
 * Revision 3.5.6.2  1996/05/09  14:11:28  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.5.6.1.2.1  1996/04/27  06:33:51  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.5.6.1  1996/04/25  23:02:49  ronnie
 * CSCdi55766:  Hey!  This ATM and Frame Relay is no different than in 11.1
 * Feature commit for ATM and Frame Relay into 11.2
 * Branch: California_branch
 *
 * Revision 3.5.22.2  1996/04/10  23:35:49  jwjang
 * CSCdi54325:  commit uni 3.1 to LE_Cal_V112_0_2_branch
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 3.5.22.1  1996/04/08  01:44:31  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 3.5.12.1  1996/03/25  02:18:23  cakyol
 * sync LE_Syn_Cal_V111_1_3_branch to LE_Cal_V111_1_0_3_branch
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.5  1996/03/04  21:47:03  rbadri
 * CSCdi50537:  Router sends malformed OID in LECS address getnext request.
 * In getnext request to get the LECS address from the switch, specify
 * OID with and without length field(separate request) to work with
 * LS100/LS1010.
 *
 * Revision 3.4.28.2  1996/03/21  21:50:48  cakyol
 * Kartik's fix for "ILMI hung" problem picked up manually.
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 3.5  1996/03/04  21:47:03  rbadri
 * CSCdi50537:  Router sends malformed OID in LECS address getnext request.
 * In getnext request to get the LECS address from the switch, specify
 * OID with and without length field(separate request) to work with
 * LS100/LS1010.
 *
 * Revision 3.4  1995/12/05  22:29:57  kartik
 * CSCdi42922:  ILMI should do bookkeeping about registrations/deregs
 * ILMI address registration will now ignore the selector byte.
 *
 * Revision 3.3  1995/11/24  16:37:20  jjohnson
 * CSCdi42637:  platform-specific code should be moved out of sub_snmp
 * Install an API for accessing platform-specific SNMP objects
 *
 * Revision 3.2  1995/11/17  08:46:57  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:00:50  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/08/08  05:33:30  schrupp
 * CSCdi38341:  Dont cancel ATM NSAP if never registered
 *
 * Revision 2.2  1995/06/16  22:53:03  kartik
 * CSCdi35409:  ILMI doesnt work in some images, works in others
 * Use if_statechange_complete registry to detect hardware reset.
 *
 * Revision 2.1  1995/06/07  20:12:32  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#ifndef _ILMI_PRIVATE_H_ 
#define _ILMI_PRIVATE_H_ 



#define ILMI_ID      			1
/*
 * ILMI Traps
 */

/*
 * Poll Related.
 */

#define ILMI_MAXPOLLHISTORY 	10
#define ILMI_DEFPOLLHISTORY 	4			/* As per Spec for NW-side */
#define ILMI_MAXPOLLPERIOD 	    60000		/* Milliseconds */
#define ILMI_DEFPOLLPERIOD 	    10000		/* Milliseconds  As per Spec for */

#define ILMI_REQTIMEOUT 	    3000        /* Milliseconds */
#define ILMI_MAXREQRETRY      	3        	/* MAX number of retries */
#define ILMI_POLL_RETRYS		1
#define ILMI_ADDREG_RETRYS		10
#define ILMI_PREFIX_WAITTIME    5000        /* Waiting time before prod
					     * the  switch for the prefix.
                                             */
#define ILMI_PEERADDRCHECK_TIMEOUT 30000


/*
 *    Type of ILMI
 */

#define ILMI_NETWORKSIDE  		151 
#define ILMI_USERSIDE			152

#define ILMI_ADDREG_BACKOFFTIME 15000


/*
 * We limit the number of messages that be will processed each time a 
 * process is invoked. This will prevent one process from hogging the CPU
 * if it gets swamped with messages in its queue.
 */
#define MAX_MESSAGES            50


#define ILMI_REMOTE_RESTART             301
#define ILMI_LOCAL_RESTART              302

/*
 * SNMP Related
 */
#define ILMI_ACCESS_LIST 0xDEADBEEF


/* The hash table size for the outstanding request queue. */
#define ILMI_REQUEST_HASHSIZE           64

/*
 * The ATM address Table.
 * If the ILMItype == ILMI_USERSIDE the address type will be ilmiPrefixAddress
 * or else it would be the ilmiHostddress.
 */

typedef struct ilmi_atmaddressRow_ {
    struct ilmi_atmaddressRow_ *nxt;
	ilmiAddrType               type;
	ilmiIntf                   intf_id;    /* The physical interface(UNI)*/ 
	OctetString                *addr;       /* The Host/Prefix address. */
	ilmiAddrState              status;        /* The status of the entry. */
} ilmi_atmaddressRow;



/*
 *
 *  The ILMI message structure. This is the general data unit which is used 
 *  across the ILMI Module. It has two parts one for the client which it fills 
 *  initially before passing it to the ILMI. The ILMI itself acts as a Client
 *  with a client_id=1  for internal functions such as polling etc. The 
 *  other part is exclusively for the ILMI procedures to use.
 *
 *
 */

typedef struct ilmi_msgblock_ {
	struct 		  ilmi_msgblock_ *nxt;
	struct            ilmi_msgblock_ *hash_next;
	ushort 		  client_id;           /* Client using the
						ILMI services. */
	ushort 		  intf_id;	       /* The UNI */
	uint   		  transid;             /* Exclusively for Client */
	ilmiOperType 	  oper; 	       /* Operation Type Get/Set etc.*/
	ilmiErrorType 	  errcode;             /* NoResponse or Valid */ 
	ilmiResponseType  response;            /* What kind of response
					        * received.  Maps to SNMP
						* V1 error code. Set if and
						* when a response is
						* received
						*/
	VarBind 	  *vblist;             /* The Varbind  object-value
						* pairs.
						*/
	ilmiReqID   	  req_id;              /* Request id to match
						* response to request.
						*/ 
	ilmiFunctionType  ilmi_func;           /* Set to some ILMI function
						* such  as
						* ILMI_POLL_LINKSTATUS
						*/
	ushort 		  retry_count;         /* Number of times ILMI
						* attempts to  xmit
						*/
	sys_timestamp 	  timeout;   	       /* Time in secs before ILMI
						* retrys  to xmit
						*/
	boolean           address_reg_flag;    /* If true it indicates
						* that this message carries
						* a address registration
						* set request.
						*/
	int               encodedReqLen;       /* The  length of the ASN.1
						* encoded  request
						*/
	uchar 	          *encodedReq;         /* The  ASN.1 encoded data */
		
} ilmi_msgblock;


/* ----------------------------*/
/*
 *  The ILMI client structure. The ilmi creates one of these whenever a 
 *  client registers itself. Subsequently the callbacks are assigned as
 *  the client registers for any notifications.
 */

typedef struct ilmi_clientblock_ {
	struct ilmi_clientblock_ *nxt;
	ilmiClient   client_id;             /* Client using the ILMI services. */
	ilmiLabel    name;
	ilmiNotifyCB address_add;          /* Address notification callback    */
	ilmiNotifyCB address_del;          /* Address notification callback    */
	ilmiNotifyCB address_valid;        /* Address validation callback. */
	ilmiNotifyCB trap_notify;          /* Trap notification callback. */
	ilmiNotifyCB response_notify;      /* ILMI calls this to forward response*/
	ilmiNotifyCB linkdown;             /* Link Down Notification */
        ilmiNotifyCB universion_rcv;      /* uni version callback */
} ilmi_clientblock;


typedef struct ilmi_intfblock_ {
    struct ilmi_intfblock_ *nxt;
    ilmiIntf               intf_id;            	  /* Interface number. */
    hwidbtype              *idb;                  /* ATM Port Identifier */
    ilmiIntfState          state;		  /* State of the UNI */
    int                    initial_set_count;     /* Used when ILMI address
						   * registration is
						   * turned off.
						   */
    queuetype              localAddressTabQ;
    queuetype              peerAddressTabQ;
    /* Poll Related */
    short curindex;
    boolean history[ILMI_MAXPOLLHISTORY];     /* Circular buffer for poll
						 history how far back the
						 history is kept depends on
						 the value of the variable
						 ilmi_pollhistory. The default
						 is ILMI_DEFPOLLHISTORY and
						 the maximum is
						 ILMI_MAXPOLLHISTORY
						 */
} ilmi_intfblock;


typedef struct ilmi_eventQblk_ {
struct ilmi_eventQblk_ *nxt;
int                freq;              /* The frequency of recurrence. 
										 in seconds */
ilmiEventType      type;              /* The event */
ilmiIntf           intf_id;           /* The interface. */
ilmiEvContext      context;            /* Type specific for context */
mgd_timer          m_timer;
} ilmi_eventblk;




/* ----------------------------*/
/*
 * E X T E R N S 
 */

extern queuetype           ilmi_requestQ;
extern queuetype           ilmi_responseQ;
extern queuetype           ilmi_inputQ;
extern queuetype           ilmi_addressTabQ;
extern queuetype           ilmi_clientQ;
extern queuetype           ilmi_intfQ;
extern queuetype           ilmi_eventQ;
extern queuetype           ilmi_eventDoneQ;
extern boolean             ilmi_queue_empty;

/* 
 * Poll related and configurable from the 
 * CLI sometime in the future
 */

extern boolean             ilmi_pollcfg;
extern unsigned short  	   ilmi_pollhistory;
extern unsigned short  	   ilmi_pollperiod;
extern sys_timestamp       ilmi_nextpoll;

extern sys_timestamp       ilmi_nextretrycheck;
extern sys_timestamp       ilmi_nextintfcheck;
extern ilmiClient          client_count;
extern unsigned short      ilmi_type ;
extern int memcnt ;
extern char *const ilmi_snmp_errors[];



/* 
 * Functions
 */
extern void                ilmi_notify_Address(ilmiClientNotifyType,
											  ilmi_clientblock *, ilmiIntf intf_id,OctetString *); 
extern void                ilmi_parser_init (void);
extern boolean             ilmi_processBLOCK(void);
extern forktype            ilmi_Process(void);
extern void                ilmi_snmp_init(void); 
extern void                ilmi_process_input(ilmi_msgblock *);

extern void                ilmi_process_response(ilmi_msgblock *);
extern void                ilmi_process_poll(ilmiIntf);
extern ilmi_intfblock      *ilmi_getintfblk(ilmiIntf);

extern                     ilmi_clientblock *ilmi_getclientblk(ilmiClient);
extern void                ilmi_free_msgblk(ilmi_msgblock *);
extern void                ilmi_init_polldata(void);
extern void                ilmi_update_polldata (uint,ilmiErrorType);

extern ilmi_atmaddressRow  *ilmi_getrow_addressTab (uint, OctetString *);
extern void                ilmi_addaddress_informclient (uint, OctetString *);
extern void                ilmi_deladdress_informclient (uint, OctetString *);

extern uint                ilmi_validate_address(uint,OctetString *,int);
extern void                ilmi_initiate_addreg(ilmi_intfblock *);
extern boolean 			   ilmi_encode_message(ilmi_msgblock *,ilmiOperType);	
extern boolean 			   ilmi_send_pkt(ilmi_msgblock *);

extern boolean             atm_ilmi_encap(paktype *);   
extern boolean 			   ilmi_trap_encode(ilmiTrapType,int,int,int,uchar **,int *);
extern boolean             ilmi_req_encode(ilmi_msgblock *);
extern boolean             ilmi_access_check(int, addrtype *);

extern boolean             ilmi_snmp_callback(Pdu *);
extern boolean             ilmi_int_encode(int,uchar **, int *);
extern boolean             ilmi_length_encode(int,uchar **, int *);
extern boolean             ilmi_name_encode(OID *,uchar **, int *);

extern ilmi_atmaddressRow  *snmp_get_atmaddReg(int, long, OctetString *);
extern atm_db_t            *snmp_get_atm(int, long *);
extern ilmi_atmaddressRow  *atm_find_addregSNMP(long *, boolean);

extern int                 ilmi_CmpAddress(OctetString *,OctetString *);
extern int 		   ilmi_add_addressTab(ilmiIntf, OctetString *, boolean *);
extern ilmi_atmaddressRow  *ilmi_search_addrTab(ilmiIntf,OctetString *,int);
extern int                 snmp_set_atmaddReg(ilmiIntf,OctetString *,ilmiAddrState);

extern void 			   ilmi_log_error(ilmiClient,ilmiErrorType, char *);
extern void                ILMI_ChangeState(idbtype *, hwidbtype *);
extern void                ilmi_clear_addrTab(ilmi_intfblock *);

extern void                ilmi_snmp_initcb(void);
extern                     ilmi_atmaddressRow 	*ilmi_getnext_addressTab(ilmiIntf,OctetString *);
extern void 			   ilmi_process_intfRestart(ilmi_intfblock *); 
extern boolean             ilmiAddTableEmpty(ilmi_msgblock *);

extern boolean             ilmi_validate_intf(ilmi_intfblock *);
extern void                ilmi_initial_sets(ilmi_intfblock *);
extern char                *ilmiMakeStrFromOstr(OctetString *);
extern ilmi_eventblk       *ilmi_getevent_fromQ(ilmiIntf , ilmiEventType, ilmiEvContext);
extern void                ilmi_destroy_intfblk(ilmi_intfblock *);


extern void                ilmi_reqmsg_hashenter(ilmi_msgblock *);
extern ilmi_msgblock       *ilmi_reqmsg_hashretreive(int);
extern void                init_ilmi_timers (void);
extern void                ilmi_periodic(void);
extern void                ilmi_enable_eventtimer(ilmi_eventblk *);
extern void                ilmi_disable_eventtimer(ilmi_eventblk *);
extern void                ilmi_destroyevent_forintf(ilmi_intfblock *);
extern void                ilmi_send_initialreq (ilmi_intfblock *,OID *,
                              ilmiFunctionType);
extern void                ilmi_print_address(OctetString *);

extern boolean             ilmi_validate_localtable(ilmi_msgblock *,
						    OctetString *address,
						    boolean *send);
extern OctetString         *ilmi_addressreg_vb(ilmi_msgblock *);
extern boolean             ilmi_checkreg_addlocal(OctetString *, int, boolean *);
extern boolean             ilmi_checkdereg_removelocal(OctetString *, int, boolean *);
extern void                ilmi_delete_localdb(ilmi_intfblock *);

extern int                 ilmi_CmpNOctetString (OctetString *, OctetString *, int);
extern void                ilmi_print_addTable(ilmi_intfblock *);
extern void                ilmi_sync_localpeertable(ilmi_intfblock *);
extern void                ilmi_set_peeraddress (ilmi_intfblock *,OctetString *,ilmiAddrState);
extern void                ilmi_updatepeer_table(ilmi_msgblock *);
extern void                ilmi_print_nsap (OctetString *);
extern void                ilmi_notify_universion (ilmi_intfblock *, int );
extern int                 ilmi_lecs_response(ilmi_clientblock *, ilmi_msgblock *);
extern void                ilmi_dump_msgblock(ilmi_msgblock *);

    
/* Debug related. */
extern void      		   ilmi_state_transit(ilmiIntf,int ,int, char *);
extern char                *ilmi_print_state(int );
extern boolean atm_ilmi_debug;
extern char  *ilmi_namestring_forintf(ilmi_intfblock *);
extern char  *ilmi_string_for_reqtype(ilmiOperType);
extern char  *ilmi_string_forclient(ilmiClient);


#endif
