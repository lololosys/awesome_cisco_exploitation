/* $Id: dmdp.h,v 3.2 1995/11/17 18:50:54 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/servers/dmdp.h,v $
 *------------------------------------------------------------------
 * dmdp.h - Dnsix-NAT suppport
 *
 * Feb 1993, Phanindra Jujjavarapu
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All rights reserved.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: dmdp.h,v $
 * Revision 3.2  1995/11/17  18:50:54  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:11:31  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:43:52  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#define	DMDP_VERSION    1

/*
 * DMDP Message Types
 */

#define	DMDP_ENC               0       /* Encryption Authentication Employed */
#define DMDP_ACK               1       /* Positive Acknowledgement */
#define DMDP_NACK              2       /* Negative Acknowledgement */
#define DMDP_DATA              3       /* A message packet */
#define DMDP_UNROPT            4       /* Unrecognized option */
#define DMDP_NOENC             5       /* Encryption authentication */ 
				       /* not supported */

/* Values 6 and 7 are reserved */

/*
 * default configuration parameters
 */
#define DMDP_QTIME    20    /* 20 seconds */
#define DMDP_RETRIES       4    /* 4 times */


/*
 * DMDP header
 */
typedef struct dmdptype_ {
    uchar     control;                 /* */
    uchar     version :4;
    uchar     data_offset :4;
    ushort    chksum;
    ulong     seq[2];
    uchar     data[0];
} dmdptype;

typedef struct dmdpdatatype {
    ulong     handle;
    ushort    retries;
    ulong     PACKED(send_seq[2]);
    ulong     PACKED(send_length);
    ipaddrtype PACKED(faddress);
    ushort    fport;
} dmdpdatatype;

typedef void (*receivetype) (paktype *, uchar *);
typedef boolean (*notifytype) (ulong, int, ipaddrtype, boolean);
    
typedef struct dmdplisttype_ {
    struct dmdplisttype_ *next;
    ipsocktype *socket;
    receivetype receive;
    notifytype notify;
    ulong rcv_seq[2];
    ulong send_seq[2];
    queuetype *pak_queue;
    timer_t   *retrans_timer;
}dmdplisttype;

#define DMDP_HDR_SIZE     12

#define DMDP_RETRANS_TIME 10

/*
 * DMDP global block
 */
typedef struct dmdpblocktype {
    boolean serving;
    ulong pid;
    ushort retries;
    ulong qtime;
    dmdplisttype *client_list;
}dmdpblocktype;

#define dnsix_dmdp_buginf	if (dnsix_dmdp_debug) buginf

extern boolean dmdp_send(ipsocktype *, paktype *, ipaddrtype, ushort, ulong);
extern void dmdp_register (ipsocktype *, receivetype, notifytype);
extern void dmdp_deregister (ipsocktype *);
extern void dmdp_rcv_ack(ipsocktype *, paktype *, dmdptype *);
extern void dmdp_send_ack(dmdplisttype *);
extern void dmdp_rcv_data(ipsocktype *, paktype *, dmdptype *);
extern void dmdp_retrans(dmdplisttype *, uchar *, uchar *);
extern void dmdp_server(void);
extern void dmdp_init(void);
extern void dmdp_start(void);
extern dmdplisttype * dmdp_find_client(ipsocktype *);
extern boolean dmdpBLOCK(void);
extern void dmdp_service_stop (void);
extern ushort dmdp_chksum(char *, ushort);
extern void dmdp_delete_client(dmdplisttype *);
extern void dmdp_dump_pak(ipsocktype *, paktype *);

/*
 * dnsix_chain.c
 */
extern void dnsix_parser_init(void);
