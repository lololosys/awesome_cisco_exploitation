/* $Id: spx.h,v 3.2 1995/11/17 19:24:45 gstovall Exp $
 * $Source: /swtal/cherf.111/ios/sys/xns/spx.h,v $
 *------------------------------------------------------------------
 * spx.h   Sequenced Packet Exchange Protocol  (header include file)
 *
 * September 1995, Syed Mohammad Irfan Ashraf
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: spx.h,v $
 * Revision 3.2  1995/11/17  19:24:45  gstovall
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  14:00:11  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:39:59  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/09/26  11:57:15  irfan
 * Sequenced Packet Exchange Protocol: holding area
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Some structures
 */


/*
 * The SPX socket data structure
 */
struct spxsocktype_ {
    struct spxsocktype_ *next;	        /* pointer to next socket */
    ushort sock_num;			/* Socket Number */
    ulong state;			/* current state of socket */
    ulong flags;                        /* user specified flags */
    queuetype spxconQ;                  /* connections on this socket */
    ulong open_cb;                      /* new connection callback function */
    ulong close_cb;                     /* close connection callback */
};

typedef struct spxsocktype_ spxsocktype;
typedef nov_spx_header spxhdr;


/*
 * The SPX connection data structure
 */
struct spxcontype_ {
    struct spxcontype_ *next;
    spxsocktype *spxsock;
    ushort local_con;                   /* connection number..our end */
    ushort remote_con;                  /* connection number..remote */
    ulong  remote_net;                  /* remote net */
    uchar remote_host[IEEEBYTES];       /* remote host */
    ushort remote_sock;			/* remote socket */
    ulong local_net;                    /* local net */
    uchar local_host[IEEEBYTES];	/* local host */
    ushort local_sock;		        /* local socket */
    ulong state;                        /* state of connection */
    ulong flags;                        /* various appl. defined flags */
    queuetype inQ;            		/* input queue */
    queuetype outQ;			/* output queue */
    queuetype unackedQ;   		/* data pkts. waiting for ack */
    ushort local_next_seq;              /* our next seq */
    ushort remote_next_seq;             /* remote's next seq = our ack */
    ushort local_alloc_num;             /* this is available buffers */
    ushort remote_alloc_num;            /* this is available buffers */
    idbtype *idb;                       /* idb on which, we are talking */
    nidbtype *nidb;                     /* nidb on which, we are talking */
    sys_timestamp purge_timer;          /* various state timers */
    sys_timestamp send_verify_watchdog; /* send a WD if this fires */
    sys_timestamp abort_timer;          /* abort session if this fires */
    int retries;                        /* various retry counters */
                                        /* NASI specific area */
    uint nasi_flags;                    /* Various NASI flags */
    tt_soc *tty;                        /* which tty r v running on ? */
    int ttynum;                         /* OK how bout the tty # ? */
    int process;			/* process on this connection */
    uchar *nasi_inhead;
    uchar *nasi_intail;
    uchar *nasi_instart;
    uchar *nasi_inend;
    uchar *nasi_outhead;
    uchar *nasi_outtail;
    uchar *nasi_outstart;
    uchar *nasi_outend;
    int nasi_insize;
    int nasi_outsize;
    int nasi_incount;
    int nasi_outcount;
    int nasi_isavedchar;
    int nasi_inoverflow;
    
};

typedef struct spxcontype_ spxcontype;
typedef boolean (*spx_listen_cb)(spxsocktype *, spxcontype *);
typedef boolean (*spx_close_cb)(spxcontype *);

/*
 * Internal states for SPX connection structures
 *    kept in spxcon->state
 */
enum SPXCON_STATES {
    SPXCON_ST_INITIAL,
    SPXCON_ST_OPEN,
    SPXCON_ST_CLOSING,
    SPXCON_ST_PURGING
};

/*
 * Some constants
 */
#define SPX_MINSIZE 42                 /* IPX + SPX (30 + 12) */
#define SPXCON_MAX_OUT_PENDING  30     /* Max pending output packets */
/*
 * Flags used at time of opening socket
 */
#define SPXCON_PACKET_IO  0x00000001    /* Reads will be in packets */

/*
 * csb->which values for "show spx" command options
 */
enum SPX_SHOW_TYPES {
    SHOW_SPX_CON,
    SHOW_SPX_ALL
};

/*
 * Some macros
 */
#define FOR_ALL_SPX_SOCKETS(spx_sock) \
    for ((spx_sock) = ((spxsocktype *)spxsockQ.qhead); (spx_sock); \
	 (spx_sock) = (spx_sock)->next)

#define FOR_ALL_SPXCON_ON_SOCK(spx_sock, spx_con) \
    for ((spx_con) = ((spxcontype *)((spx_sock)->spxconQ.qhead)); (spx_con); \
	 (spx_con) = (spx_con)->next)

/*
 * SPX header
 *    defined in <novell.h>
 */

/*
 * Connection control field in SPX header
 */
#define SPX_CONCTL_XHD         0x01    /* Reserved for SPX-II */
#define SPX_CONCTL_RES1        0x02    /* Undefined */
#define SPX_CONCTL_NEG_SIZE    0x04    /* SPX-II negotiate size */
#define SPX_CONCTL_SPXII       0x08    /* SPX-II negotiate size */

#define SPX_CONCTL_EOM         0x10    /* End of session requested */
#define SPX_CONCTL_ATTN        0x20    /* Attention packet */
#define SPX_CONCTL_NEED_ACK    0x40    /* Data in this pkt. Need ack */
#define SPX_CONCTL_SYS         0x80    /* System packet */

#define SPX_CONCTL_NEWCON    (SPX_CONCTL_SYS | SPX_CONCTL_NEED_ACK)
#define SPX_CONCTL_WATCHDOG  (SPX_CONCTL_SYS | SPX_CONCTL_NEED_ACK)

/*
 * Datastream field in SPX header
 */
#define SPX_DATASTREAM_EOM_REQ 0xFE    /* End of session requested */
#define SPX_DATASTREAM_EOM_ACK  0xFF   /* End of session ack */

#define SPX_CONID_CONREQUEST  0xFFFF   /* new connection request */

#define SPX_ALLOCATION_UNITS  5        /* alloc units. kind of */
                                       /*    windowing based flow control */

/*
 * Some SPX timer constants
 *   Ideally they should be configurable
 */
#define SPX_VERIFY_WATCHDOG_TIMER (5*ONESEC) /* Send next watchdog at */
#define SPX_ABORT_WATCHDOG_TIMER (30*ONESEC) /* Send next watchdog at */
#define SPX_PURGE_TIMER (10 * ONEMIN)        /* Wait before freeing Con */

/*
 * Return codes for SPX API
 */
enum SPX_RET_CODES {
    SPX_RET_NODATA = 0,
    SPX_RET_DATA = 1,
    SPX_RET_OK= 2,
    SPX_RET_CONCLOSED = 3,
    SPX_RET_NOMEM = 4,
    SPX_RET_BADCON = 5,
    SPX_RET_FAILED = 6
    };


/*
 * External declarations
 *    Includes the SPX_API_ type functions
 */
extern spxsocktype *spx_api_listen_socket(ushort, ulong,
					  spx_listen_cb, spx_close_cb);
extern int spx_api_read_chunk(spxcontype *, paktype **, int *);
extern int spx_api_send_data(spxcontype *, uchar *, int);
extern int spx_api_inputsize(spxcontype *);
extern int spx_api_outputsize(spxcontype *);
extern void spx_api_push(spxcontype *, boolean, boolean);
extern boolean spx_api_con_alive(spxcontype *);
extern void spx_api_closecon(spxcontype *);
extern boolean spx_api_con_closing(spxcontype *);


/* spx_chain.c */
extern void spx_setup_parser(void);

/* spx.c */
extern boolean spx_running;
extern sys_timestamp spx_wake_time;
extern queuetype spxsockQ;
