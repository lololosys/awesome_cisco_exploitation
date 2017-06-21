/* $Id: sniffer.h,v 3.2 1995/11/17 18:56:48 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/pgen/sniffer.h,v $
 *------------------------------------------------------------------
 * S N I F F E R . H
 *
 * Format of a Sniffer binary file. Mostly cribbed from the Network
 * General Sniffer manual. This file doesn't have definitions for
 * everything one might like, because the manual doesn't have them
 * either.
 *
 * Copyright (c) 1992-1994 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: sniffer.h,v $
 * Revision 3.2  1995/11/17  18:56:48  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:59:24  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:34:24  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 * $Endlog$
 */

/* What the contents of the various fields look like... since IBM/Intel
 * machines don't use the One True Byte Order, we have to do weird things
 * to decode the arithmetic fields.
 *
 */

typedef struct _ltleword {
    uchar lobyte;
    uchar hibyte;
} ltleword;

typedef struct ltlelong {
    uchar lowbyte;
    uchar midlow;
    uchar midhigh;
    uchar highbyte;
} ltlelong;

#define LTLEWTOINT(x) ((unsigned) ((x).lobyte) | \
		       ((unsigned) ((x).hibyte) << 8))
#define LGLELTOINT(x) ((unsigned) ((x).lowbyte) | \
		       (((unsigned) (x).midlow) << 8) | \
		       (((unsigned) (x).midhigh) << 16) | \
		       (((unsigned) (x).highbyte) << 24))

/* DOS date structure */
struct pctime {
    ltleword pct_time;                 /* hh:mm:ss (little endian) */
    ltleword pct_date;                 /* yy:mm:dd (little endian) */
};

/*
 * Shifts and masks for DOS time and date fields, in host byte order.
 */
#define SECSHIFT        0
#define SECMASK         0x1F
#define MINSHIFT        5
#define MINMASK         0x3F
#define HOURSHIFT       11
#define HOURMASK        0x1F

#define DAYSHIFT        0
#define DAYMASK         0x1F
#define MONSHIFT        5
#define MONMASK         0x0F
#define YEARSHIFT       9
#define YEARMASK        0x7F


/* The actual Sniffer structs */

/* The "magic number" on the file is a 17-byte string */
#define SN_MAGIC_DATA "TRSNIFF data    \032"
#define SN_MAGIC_LEN 17

/* Generic header that precedes every record */
struct f_rec_struct {
    ltleword type;		/* Record type (version/data/EOF) */
    ltleword length;		/* Length in bytes excluding this struct */
    ltleword rsvd;		/* Reserved  */
};

/* Type values in the record header */
#define REC_VERS 1		/* Version record */
#define REC_EOF 3		/* End-of-file record */
#define REC_FRAME2 4		/* Frame data */
#define REC_BIGSECRET 6		/* Network General won't say */

/* Version record */
struct f_vers_struct {
    ltleword maj_vers;		/* Sniffer major version */
    ltleword min_vers;		/* Sniffer minor version */
    struct pctime date;		/* DOS-format date */
    uchar type;			/* Type of records that follow */
    uchar network;		/* Type of LAN data came from */
    uchar format;		/* Format version */
    uchar timeunit;		/* Unit for frame timestamps */
    ltleword rsvd[3];		/* Reserved */
};

/* Network type values for "network" field of version record */
#define NETWORK_TRING 0		/* Token Ring */
#define NETWORK_ENET  1		/* Ethernet */
#define NETWORK_ARCNET 2	/* ARCNet */
#define NETWORK_STARLAN 3	/* StarLAN */
#define NETWORK_PCNW 4		/* PC Network broadband */
#define NETWORK_LOCALTALK 4	/* LocalTalk */
#define NETWORK_ZNET 6		/* Znet */
#define NETWORK_UNKNOWN	7	/* Not a Network General value... */
#define SN_MAXNETWORK 7		/* Largest legal value */

/* Conversion from network type codes to network type names */
#define SN_NETNAMES {"Token Ring", "Ethernet", "ARCNet", "StarLAN", \
		     "PC network broadband", "LocalTalk", "Znet", "unknown"}

/* Time unit values */
#define TIMEUNIT_UNSPEC 0	/* Defaults by network type (to what?) */
#define TIMEUNIT_PC 1		/* PC ticks; 0.838096 uSecs */
#define TIMEUNIT_3COM 2		/* 15 uSecs */
#define TIMEUNIT_MICOM 3	/* 0.5 uSecs */
#define TIMEUNIT_SYTEK 4	/* 2 uSecs */

/* Guess at the standard time values; converts from network types to time
 * unit codes. Probably wrong. That's what happens when people don't
 * document things properly.
 */
#define SN_DEFUNITS {TIMEUNIT_MICOM, TIMEUNIT_3COM, TIMEUNIT_PC, \
		     TIMEUNIT_PC, TIMEUNIT_PC, TIMEUNIT_PC, \
		     TIMEUNIT_PC, TIMEUNIT_PC}

/* Conversion tables from time unit values to microsecond conversion factors.
 * To get microseconds, divide by divisor, then multiply by multiplier. The
 * factors for "PC ticks" aren't exact; to get the documented value, you'd
 * have to use 62500 and 52381, which end up shifting the bits around way
 * too much and losing precision in fixed-point arithmetic. The values used
 * are off by one part in 52381.
 */
#define SN_T_DIVISORS	 {0, 3125,  1, 2, 1}
#define SN_T_MULTIPLIERS {0, 2619, 15, 1, 2}

/* Data record */
struct f_frame2_struct {
    ltleword time_low;		/* Low-order 16 bits of time */
    ltleword time_mid;		/* Middle 16 bits of time */
    uchar time_high;		/* High-order 8 bits of time */
    char time_day;		/* Number of days since start of capture */
    ltleword size;		/* Number of data bytes in file for this pkt */
    uchar fs;			/* Frame error bits */
    uchar flags;		/* Buffer flags,  */
    ltleword true_size;		/* Actual size of original LAN frame */
    ltleword rsvd;		/* Reserved */
    uchar framedata[0];		/* The data; length comes from "size" */
};

/* Generic way of getting at Sniffer record */
struct sn_rec {
    struct f_rec_struct hdr;		/* Record header */
    union {
	struct f_vers_struct vers;	/* Version record */
	struct f_frame2_struct frame;	/* Frame data record */
    } u;
};
