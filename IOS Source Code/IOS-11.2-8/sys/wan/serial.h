/* $Id: serial.h,v 3.7.12.8 1996/09/09 01:32:59 xtang Exp $
 * $Source: /release/112/cvs/Xsys/wan/serial.h,v $
 *------------------------------------------------------------------
 * serial.h -- synchronous serial line definitions
 *
 * 2-October-1986, Kirk Lougheed
 *
 * Copyright (c) 1986-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: serial.h,v $
 * Revision 3.7.12.8  1996/09/09  01:32:59  xtang
 * CSCdi56632:  unknown clock rates on 4T PA, FSIP
 * Branch: California_branch
 *         add ALT_HELPS to use csb->which in different
 *         driver procedures, and print the
 *         driver specific info. In this case, the clockrate
 *         option table, and csb->which = CFG_INT_CLOCKRATE
 *
 * Revision 3.7.12.7  1996/08/29  02:25:42  xtang
 * CSCdi60447:  FSIP max MTU=14816 not 18000
 * Branch: California_branch
 *              Implemented new FSIP command 93 to pass the available
 *              buffer size in the controller to the system.
 *              The corresponding microcode change are in
 *              cvs -d /micro fsip_q (fsip187-0)
 *              and cvs -d /micro fsip_i (rsp_fsip204-0).
 *
 * Revision 3.7.12.6  1996/08/13  02:27:07  gstovall
 * CSCdi39484:  Giant packets output on ethernet interface
 * Branch: California_branch
 * Make sure we drop giants, even if they are just a wee bit too big.
 *
 * Revision 3.7.12.5  1996/07/08  22:58:56  mwu
 * CSCdi60922:  11.2 Core Features Commit
 * Branch: California_branch
 *
 * Revision 3.7.12.4  1996/06/17  08:36:52  jhernand
 * Commit Modular Cobra to California_branch
 *
 * Revision 3.7.12.3  1996/04/22  18:17:25  gstovall
 * CSCdi53071:  media idb init routines will fail on second pass
 * Branch: California_branch
 * Allow media idb init routines to be called again.  Also clean up the
 * handling of the media idb queues.
 *
 * Revision 3.7.12.2  1996/03/22  00:15:18  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.7.12.1  1996/03/18  22:48:37  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.2.3  1996/03/18  23:57:46  mbeesley
 * Branch: ELC_branch
 * Sync to V111_1_3
 *
 * Revision 3.3.2.2  1996/01/26  02:51:34  gstovall
 * Branch: ELC_branch
 * Oodles of fixes and enhancements:
 *         - Added support for the crc command to the predator 4T
 *         - Added support for the ignore-dcd command to the predator 4T
 *         - Do not keep stats on a VIP 4T
 *         - Pulled in a needed bugfix (CSCdi47390)
 *         - Revived the show controller command, it should work correctly
 *           with FDDI and token ring now.
 *
 * Revision 3.3.2.1  1995/12/11  19:52:08  mbeesley
 * Branch: ELC_branch
 * Sync ELC_branch to ELC_baseline_120995.
 *
 * Revision 3.7  1996/02/13  21:27:01  rbatz
 * CSCdi30258:  Fixed the following:
 *                  Duplicate half-duplex commands.
 *                  Could not do half-duplex stun.
 *                  Could not tune half-duplex timers for bstun.
 *
 * Revision 3.6  1996/02/08  20:14:51  thille
 * CSCdi48347: format string in inlines, code size impact
 * Pass 1 - Clean up some literal strings in inlines, duplicate literals,
 * etc.  11,356 bytes saved on gs7-j-m, 9336 bytes saved on igs-j-l
 *
 * Revision 3.5.16.1  1996/01/24  23:32:05  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.5  1995/12/06  18:33:42  gstovall
 * CSCdi44769:  Interface idb queues are not getting populated correctly
 * Make sure the hwidbs are in the same order on the media hwidbQs as on
 * the hwidbQ.
 *
 * Revision 3.4  1995/11/21  20:44:22  turadek
 * CSCdi43841:  add frame relay payload compression feature
 * syncing code to 11.0
 *
 * Revision 3.3  1995/11/17  18:04:26  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:50:33  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:50:52  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/09/13  20:59:36  fowler
 * CSCdi36199:  Frame relay IETF encapsulation for OSI doesnt comply with
 * 1490
 *
 * Revision 2.3  1995/09/09  00:54:56  ronnie
 * CSCdi35837:  ping works, but traceroute fails to go across ATM interface
 * Introduce a new variable type, fuzzy, for hwidb->broadcast to return
 * instead of boolean.  This allows the return of NOT_SURE.  Adjust all
 * hwidb->broadcast routines to return fuzzy instead of boolean.
 *
 * Revision 2.2  1995/06/28  09:32:56  smackie
 * Repair widespread subsystem header braindamage. (CSCdi23568)
 *
 *   o Fix subsystem entry points to be foo(subsystype *) not foo(void)
 *   o Repair nonsensical and redundant subsystem sequences
 *   o Use NULL where no property is required
 *
 * Revision 2.1  1995/06/07  23:19:28  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __SERIAL_H__
#define __SERIAL_H__

/*
 * Largest serial datagram we can handle, not counting encapsulation
 * of RSRB or STUN over an MCI serial interface can use larger buffers
 * but this constant must NOT be changed.
 */
#define	MINSERIALSIZE	2
#define MAXSERIALSIZE	1500
#define MAX_SERIAL_PAK	20000
#define MAXDS3SIZE      4470            /* match fddi interfaces */

#define HDLC_KEEPALIVE_WINDOW	3
#define HDLC_SLARP_WINDOW	30
#define HDLC_LOAD_THRESHOLD	1000	/* 1000pps */
#define HDLC_CRC                2
#define HDLC_ADDR               2
#define HDLC_TYPE               2
#define HDLC_HEADER_SIZE        (HDLC_ADDR + HDLC_TYPE)
#define DS3_SLOP                6 /* the link controller is trash! */
#define DS3_OVERHEAD_BYTES      HDLC_HEADER_SIZE
#define DS3_BAGGAGE             ((HDLC_CRC * 2) + DS3_SLOP) /* CRC32 */
  

/*
 * HDLC encapsulation definitions
 */

#define HDLC_BCAST 	0x8000		/* broadcast on HDLC */
#define HDLC_PADDED 	0x4000		/* a pad byte was appended */
#define HDLC_COMPRESSED 0x2000		/* compressed HDLC packet */
#define HDLC_STATION 	0x0F00		/* station address for raw HDLC */
#define HDLC_ENCAPBYTES 	4	/* bytes of HDLC encapsulation */
#define HDLC_BRIDGECODE 0x0F006558L	/* HyBridge encapsulation code */

#define HDLC_IPCODE	0x0F000800L	/* IP encapsulation code */
#define HDLC_DLSW       0x0F001995L     /* DLSw raw serial encapsulation */
#define HDLC_RSRB	0x0F001996L	/* RSRB raw serial encapsulation */
#define HDLC_STUN	0x0F001997L	/* STunnel raw serial encapsulation */
#define HDLC_DECCODE	0x0F006003L	/* DECnet encapsulation code */
#define HDLC_XNSCODE	0x0F000600L	/* XNS encapsulation code */
#define HDLC_NOVCODE	0x0F008137L	/* Novell encapsulation code */
#define HDLC_VINESCODE	0x0F0080C4L	/* Vines encapsulation code */
#define HDLC_ATALKCODE	0X0F00809BL     /* AppleTalk encapsulation code */
#define HDLC_KEEPALIVES 0x8F008035L	/* Normal HDLC keepalive code */
#define	HDLC_HOLDQ 40			/* maximum size of hold queue */

#define hdlcflags e.encaps[ENCAPWORDS-2] /* first 16 bits */
#define hdlctype e.encaps[ENCAPWORDS-1]  /* last 16 bits */

typedef struct _vhdlctype {
    ushort var_hdlcflags;
    ushort var_hdlctype;
    uchar  data[0];
} vhdlctype;

/* Half-duplex flags */
#define HDX_FLAG_CAPABLE        0x01            /* half-duplex indicator   */
#define HDX_FLAG_ON             0x02            /* HDX capable  */

/*
 * Default Half Duplex timer values (Units are millisecs)
 */
#define HDX_RTS_TIMEOUT_DEFAULT         3
#define HDX_CTS_DELAY_DEFAULT		0
#define HDX_RTS_DROP_DELAY_DEFAULT	3
#define HDX_CTS_DROP_TIMEOUT_DEFAULT	250
#define HDX_DCD_TXSTART_DELAY_DEFAULT	100
#define HDX_DCD_DROP_DELAY_DEFAULT	100
#define HDX_TRANSMIT_DELAY_DEFAULT	0

/*
 * This hack value was put in so that the old 
 * default of 3 counts for cts-delay could be 
 * maintained when units changed from 
 * "counts" to "milliseconds"
 */
#define HDX_CTS_DELAY_OFFSET            3

/*
 * Used for conversion between msec and counts
 */
#define HD64570_COUNTS_PER_MSEC         154
#define HD64570_MAX_MSEC_CTS_RTS        425

#define MCI_COUNTS_PER_MSEC             225
#define MCI_MAX_MSEC_CTS_RTS            290


/*
 * Serial Line RARP
 */
#define SLARP_REQUEST	0
#define SLARP_REPLY	1
#define SLARP_LINECHECK	2

/*
 * Special frames for PC interface through ISA bus
 */
#define SLARP_SPECIAL_REQUEST   0x80000001
#define SLARP_SPECIAL_REPLY     0x80000002

/*
 * for the ALT_HELPS macro
 */
#define CFG_INT_CLOCKRATE                1

struct oldslarp {
    long code;
    union {
	struct {			/* address resolution functions */
	    ulong address;
	    ulong mask;
	    ushort unused;
	} add;
	struct {			/* keep alive probing functions */
	    ulong mysequence;
	    ulong yoursequence;
	    ushort reliability;
	} chk;
    } t;
};

struct slarp {
    long code;
    union {
	struct {			/* address resolution functions */
	    ulong address;
	    ulong mask;
	    ushort unused;
	} add;
	struct {			/* keep alive probing functions */
	    ulong mysequence;
	    ulong yoursequence;
	    ushort reliability;
	    ulong PACKED(unique);
	} chk;
    } t;
};

 
typedef struct slarp_special_reply slarp_sp_reply_t;
struct slarp_special_reply {
    long code;
    uchar type;            /* 1 for TR based, 2 for Ethernet based */
    uchar speed;           /* TR only; 4 = 4Mb, 16 = 16Mb */
    uchar address[6];      /* MAC address of TR or Ethernet */
    uchar status;          /* 0 = Interface is unshut, 1 = Interface is shut */
    uchar LNM_support;     /* 0 = LNM not supported, 1 = supported */
    ushort keep_period;    /* keepalive period in secs */
    ushort mtu;            /* PC bus interface MTU */
    ulong ip_address;      /* PC bus IP address */
    ulong ip_netmask;      /* PC bus IP address mask */
    ulong ip_reserved_for_future_use;
    ulong purge_ring_counter; /* TR interface Purge ring token counter */
    ulong claim_ring_counter; /* TR interface Claim ring token counter */
    uchar adapter_status;  /* 0 = Adapter close, 1 = Adapter open */
    uchar filler;          /* align to a WORD boundary */
    ushort tr_actualring;  /* actual ring number on interface TR 0 */
};


#ifndef _WAN_STUFF_
#define _WAN_STUFF_

#define FR_IETF_IP 		0x03cc
#define ATM_DXI_IETF_IP 	FR_IETF_IP
#define FR_CLNP    		0x0381
#define ATM_DXI_CLNP		FR_CLNP
#define FR_ESIS    		0x0382
#define ATM_DXI_ESIS		FR_ESIS
#define FR_ISIS			0x0383
#define FR_ISO_IGRP             0x0384
#define ATM_DXI_ISIS 		FR_ISIS
#define ATM_DXI_CLNP_102        0x0081
#define ATM_DXI_ESIS_102        0x0082
#define ATM_DXI_ISIS_102        0x0083
#define FR_IETF_COMPRESSED	0x03b0

#endif _WAN_STUFF_

/*
 * Defs for G.703
 */
#define G703_INTERNAL	1    

/*
 * Serial hardware interface specific flags for hwidb->serial_flags
 */
#define SERIAL_FLAGS_NONE	     0x00000000 /* No flags set */
#define SERIAL_FLAGS_NO_CLOCKRATE    0x00000001 /* No clockrate changes */
#define SERIAL_FLAGS_CAN_ALT_FLAG    0x00000002 /* Can do alternate flags */
#define SERIAL_FLAGS_DO_ALT_FLAG     0x00000004 /* Do alternate flags */
#define SERIAL_FLAGS_CAN_CLK_INV     0x00000008 /* Can TX clock invert */
#define SERIAL_FLAGS_NO_DCE_CLK_INV  0x00000010 /* No DCE TX clock invert */
#define SERIAL_FLAGS_CAN_INT_TX      0x00000020 /* Can do internal TX clock */
#define SERIAL_FLAGS_NO_DTE_INT_TX   0x00000040 /* No DTE internal TX clock */
#define SERIAL_FLAGS_CAN_NRZI        0x00000080 /* Can do NRZI */
#define SERIAL_FLAGS_CAN_DIS_RX_SYNC 0x00000100 /* Can disable RX sync */
#define SERIAL_FLAGS_CAN_IGNORE_DCD  0x00000200 /* Can ignore DCD */
#define SERIAL_FLAGS_CAN_CRC_32      0x00000400 /* Can do CRC-32 */
#define SERIAL_FLAGS_SYNCASYNC       0x00000800 /* Is a sync/async interface */
#define SERIAL_FLAGS_CAN_IGNORE_LOCAL_LOOPBACK 0x00001000

enum SERIAL_HW {

    /*
     * List of fields in serial subblock
     */
    SERIAL_HW_HSSI_LOOP_REQ = IDB_TYPE_BASE_SERIAL+1,
    SERIAL_HW_HSSI_ICLK,
    SERIAL_HW_HSSI_LC_INHIBIT,

    SERIAL_HW_APPLIQUE,
    SERIAL_HW_RX_DISABLED,
    SERIAL_HW_RX_AIS,
    SERIAL_HW_RX_LOF,
    SERIAL_HW_RX_LOS,
    SERIAL_HW_RX_RAI,
    
    SERIAL_HW_SI_APPLIQUE,
    SERIAL_HW_SI_RX_DISABLED,
    SERIAL_HW_SI_RX_AIS,
    SERIAL_HW_SI_RX_LOF,
    SERIAL_HW_SI_RX_LOS,
    SERIAL_HW_SI_RX_RAI,

    SERIAL_HW_SERIAL_PROTOCOL,
    SERIAL_HW_SERIAL_RESTARTTIMER,
    SERIAL_HW_SERIAL_RESTARTDELAY,
    SERIAL_HW_SERIAL_RESTARTDELAY_DEF,
    SERIAL_HW_SERIAL_CLOCKINDEX,
    SERIAL_HW_DCE_TERMINAL_TIMING_ENABLE,
    SERIAL_HW_INVERT_TXC,
    SERIAL_HW_NRZI_ENABLE,
    SERIAL_HW_CRC_32_ENABLED,
    SERIAL_HW_START_SLOT,
    SERIAL_HW_STOP_SLOT,
    SERIAL_HW_TIMESLOT16_ENABLED,
    SERIAL_HW_CRC4_ENABLED,
    SERIAL_HW_CLOCKSOURCE_ENABLED,
    SERIAL_HW_RX_SYNC_DISABLE,
    SERIAL_HW_IGNORE_DCD,
    SERIAL_HW_IGNORE_LOCAL_LOOPBACK,
    SERIAL_HW_DOWNIFLOOPED,
    SERIAL_HW_RTS_TIMEOUT,
    SERIAL_HW_CTS_DELAY,
    SERIAL_HW_RTS_DROP_DELAY,
    SERIAL_HW_CTS_DROP_TIMEOUT,
    SERIAL_HW_DCD_TXSTART_DELAY,
    SERIAL_HW_DCD_DROP_DELAY,
    SERIAL_HW_TRANSMIT_DELAY,
    SERIAL_HW_CONTROLLED_CARRIER,

    SERIAL_HW_POSI_CLOCK,
    SERIAL_HW_POSI_LOOP_LOCAL,
    SERIAL_HW_POSI_LOOP_EXTERNAL,
    SERIAL_HW_POSI_FRAMING,
    /*
     * Special codes
     */
    SERIAL_HW_APPLIQUE_STAT,
    SERIAL_HW_RX_AIS_STAT,
    SERIAL_HW_RX_LOF_STAT,
    SERIAL_HW_RX_LOS_STAT,
    SERIAL_HW_RX_RAI_STAT,

    SERIAL_HW_CLEAR_STATS,

    SERIAL_HW_SET_CLOCKRATE,

    SERIAL_HW_TEST_RTS,
    SERIAL_HW_TEST_CTS,
    SERIAL_HW_TEST_DTR,
    SERIAL_HW_TEST_DSR,
    SERIAL_HW_TEST_DCD,
    SERIAL_HW_TEST_LOOPBACK,
    SERIAL_HW_TEST_TRANSMIT,
    SERIAL_HW_HDX_LOG_ENABLE,
    SERIAL_HW_HDX_LOG_CLEAR
};


/*
 * Prototypes:
 */
boolean hdlc_setup(parseinfo *, hwidbtype *);
int link2serial(long);
int serial2link(long);
long hdlc_getlink(paktype *);
void serial_queue_for_process(hwidbtype *, paktype *);
void serial_idb_enqueue(hwidbtype *hwidb);
void serial_idb_dequeue(hwidbtype *hwidb);

/*
 * Externs
 */
extern boolean serial_default_check_mtu(hwidbtype *hwidb, ushort mtu);
extern const char fmt_Illegal_serial_link_type_code[];
#endif __SERIAL_H__





