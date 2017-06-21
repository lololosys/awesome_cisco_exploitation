/* $Id: dnsix_nat.h,v 3.2 1995/11/17 18:51:10 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/servers/dnsix_nat.h,v $
 *------------------------------------------------------------------
 * dnsix_nat.h - Dnsix-NAT sussport
 *
 * Feb 1993, Phanindra Jujjavarapu
 *
 * Copyright (c) 1993-1994 by cisco Systems, Inc.
 * All rights reserved.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: dnsix_nat.h,v $
 * Revision 3.2  1995/11/17  18:51:10  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:11:41  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:44:05  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#define	DNSIX_VERSION		1

/*
 * DNSIX NAT Record identifier definitions.
 * are defined in ip.h
 */
/*
 * DNSIX redirect definitions.
 */
#define DNSIX_REDI_IMMEDIATE   0    /* change the NSC immediately */
#define DNSIX_REDI_FAILURE     1    /* change the NSC failure address */

/* some constants */
#define NAT_REDIRECT_COUNT 5        /* count of redirect addresses supported */
#define NAT_DEFAULT_TRANSMIT_COUNT 1/* default transmit count */
#define NAT_AUDIT_DATA_SIZE 1024    /* size of audit data in a packet */
#define SOURCE_ADDR_SIZE  4         /* size of source address */
/*
 * DNSIX record structure
 */

typedef struct dnsixtype_ {
    ushort     length;                 /* */
    ushort     record_id;              /* */
    uchar      extended_id[8];         /* */
    ulong      time_stamp;             /* */
    ulong      time_offset;            /* */
    ipaddrtype source_address;         /* */
    uchar      data_ptr[0];
} dnsixtype;

#define DNSIX_HDR_SIZE    24
    
typedef struct natinfotype {
    ushort     length;                 /* */
    uchar      PACKED(*ptr);
    uchar      flag;
    ushort     PACKED(count);
} natinfotype;

/* flags 
 */
#define ACK_WAITING     (1<<0)
    
/*
 * DNSIX global block
 */

typedef struct dnsixblocktype_ {
    boolean serving;
    ipsocktype *socket;
    ipsocktype *redi_socket;
    ipaddrtype dest;
    ipaddrtype primary;
    ipaddrtype secondary;
    ipaddrtype source;
    ipaddrtype auth_redi[NAT_REDIRECT_COUNT];
    ushort     transmit_count;
    timer_t    *output_timer;
    timer_t    *test_timer;
    ushort     test_time;
    paktype    *current_pak;
    paktype    *buffer_pak;
    uchar      state;
    ushort     atm_record_count;
    ushort     atm_pak_count;
    ushort     atm_redi_count;
} dnsixblocktype;


/*
 * State of NAT
 */
#define NAT_PRIMARY    (1<<0)
#define NAT_SECONDARY  (1<<1)
#define NAT_FAILURE    (1>>2)

#define NAT_CONNECTED   (NAT_PRIMARY | NAT_SECONDARY)

#define INFO_PTR(pak)   ((natinfotype *)(ipheadstart(pak) \
					+ IPHEADERBYTES(NOPT)))

#define dnsix_nat_buginf	if (dnsix_nat_debug) buginf

#define OUTPUT_DELAY_TIME	2	/* output timer value 2 seconds */
extern void dnsix_atm_init(void);
extern boolean check_redi_list(paktype *, char *);
extern void dnsix_rcv_nat(paktype *, uchar *);
extern boolean dnsix_ack_nat(ulong, int, ipaddrtype, boolean);
extern void dnsix_rcv_redi(paktype *, dnsixtype *);
extern boolean start_dnsix_nat_service(ipaddrtype);
extern void stop_dnsix_nat_service(void);
extern void nat_rec_send(void);
extern void dnsix_audit_write(ulong, ulong, uchar *, ushort, ipaddrtype);
extern char * nat_state_string(uchar);
extern void dnsix_output(uchar *, uchar *, uchar *);
extern void dnsix_test(uchar *, uchar *, uchar *);
extern boolean audit_trail_pak(paktype *);
extern void dnsix_init(void);
extern void write_audit(paktype *, ulong);
