/* $Id: bytestream.c,v 3.3.10.3 1996/07/23 13:33:24 thille Exp $
 * $Source: /release/112/cvs/Xsys/xremote/bytestream.c,v $
 *------------------------------------------------------------------
 * bytestream.c - Reliable sequenced packet driver for asynchronous lines
 *
 * September 1990, Kevin Herbert
 *
 * Copyright (c) 1990-1996 by cisco Systems, Inc.
 *
 * Portions of this module are based on public domain software and/or
 * software copyright (c) 1990 by Network Computing Devices, Inc., and
 * used with permission.
 *------------------------------------------------------------------
 * $Log: bytestream.c,v $
 * Revision 3.3.10.3  1996/07/23  13:33:24  thille
 * CSCdi63329: Centralize logging for getbuffer() and pak_duplicate()
 * failures
 * Branch: California_branch
 * Shamelessly rip off Mr. Katz' code from CSCdi36037.  Provide
 * centralized logging for getbuffer() and pak_duplicate() failures and
 * then rip out the 9 bazillion different messages that where in the
 * various callers to report the failure.
 *
 * Revision 3.3.10.2  1996/04/19  15:49:50  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commit
 *
 * Revision 3.3.18.2  1996/04/10  23:10:37  lol
 * Branch: IosSec_branch
 *         Incorporate code review comments.
 *
 * Revision 3.3.18.1  1996/04/09  22:22:34  che
 * Branch: IosSec_branch
 * Sync to California_branch (960405)
 *
 * Revision 3.3.10.1  1996/03/18  22:56:34  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.24.3  1996/03/16  08:01:53  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.2.24.2  1996/03/07  11:23:15  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.2.24.1  1996/02/20  22:02:28  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.3  1996/02/26  06:17:43  irfan
 * CSCdi49796:  xremote attempts to install negative timers
 * replace TIMER_START using -ve delta, with GET_TIMESTAMP,
 * TIMER_SUB_DELTA pair.
 *
 * Revision 3.2  1995/11/17  19:26:24  gstovall
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  14:01:24  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/08/25  11:45:59  hampton
 * Miscellaneous changes to eliminate direct references to the system
 * clock and to convert to always using the timer macros to access timers.
 * [CSCdi39258]
 *
 * Revision 2.3  1995/06/30 06:42:34  dstine
 * CSCdi36619:  IDB nits
 *         - remove #include of interface_private.h
 *
 * Revision 2.2  1995/06/09  13:26:30  hampton
 * Rename old scheduler calls to fit the new naming convention.
 * [CSCdi35514]
 *
 * Revision 2.1  1995/06/07 23:31:42  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <ciscolib.h>
#include "packet.h"
#include "../ip/ip.h"
#include "../ts/async_ppp.h"
#include "misc.h"
#include "Xproto.h"
#include "Xdmcp.h"
#include "xremote.h"
#include "../os/compress_lzw.h"
#include "serial_compress.h"
#include "../util/random.h"

extern boolean xrbsflow_debug;
extern boolean xrbsproto_debug;

/* forward declarations */
static void SendBareAck(SerialConnection *sc);
static void PostAck(register SerialConnection *sc);
boolean BSBlock(SerialConnection *sc);
static void SendReset(SerialConnection *sc);
static Bool SendSync(SerialConnection *sc);
static void GetNewPackets(SerialConnection *sc);
static void bs_raw_enqueue(paktype *pak, tt_soc *tty);

#define MS_FRAME_DELAY (1000)	/* premable packets if line is idle longer */
#define MS_ACK_DELAY (250)	/* delayed ack timer */
#define RETRANS_MIN (500)
#define RETRANS_MAX (1000*30)
#define RETRANS_INITIAL RETRANS_MIN
#define RANGESET(loc,val,min,max) (loc) = MIN(MAX((val), (min)), (max))
#define PROBE_INITIAL (1000)
#define PROBE_MAX (1000*30)
#define TRAILER_SIZE PPP_FCS_SIZE /* room for a 16-bit CCITT checksum */
#define MAX_PPP_PACKET_SIZE MAX_PPP_ENCAPSULATED_LENGTH(HEADER_SIZE+PACKET_SIZE)

#define ALLOWCOMPRESSION

/*
 * Enumeration of extended codes (ExtendedHeader.ExtendedCommandCode).
 */
#define BS_EC_SYNC 0
#define BS_EC_SYNC_ACK 1
#define BS_EC_RESET 2
#define BS_EC_CONFIG_REQ 3		/* request set/get a parameter */
#define BS_EC_CONFIG_REP 4		/* reply to request set / get */

/*
 * Connection setup extension to header
 */
#define BS_VERSION 2
#define EqualConnection(pca, pcb) ( \
	((pca)->ConnectionRandom1 == (pcb)->ConnectionRandom1) && \
	((pca)->ConnectionRandom2 == (pcb)->ConnectionRandom2) && \
	((pca)->Seq == (pcb)->Seq) && \
	((pca)->Window == (pcb)->Window) \
    )

/* 
 * Configuration extension to header
 */
typedef struct {
    /* length includes unique, code + length of data */
    unsigned char ConfigurationLength;
    unsigned char ConfigurationUnique;	/* packet 'id' */
    unsigned char ConfigurationCode;	/* see below for possible values */
    unsigned char ConfigurationPad1;
    /* followed by data in packet */
} ConfigurationHeader;

/*
 * Enumeration of configuration request codes
 * (ConfigurationHeader.ConfigurationCode).
 */
#define CRQ_ADD_OPAQUE_CHAR 0	/* extend opaque(escaped) char: data: 1 char */
/*
 * And, for replies, the following tokens appear in the ConfigurationCode
 */
#define CRP_ACK		0	/* no errors */
#define CRP_NAK		1	/* command rejected */

typedef struct {
    ExtendedHeader Header;		/* overlay packet header */
    union {
	ConnectionHeader Sync;
	ConnectionHeader Reset;		/* unused */
	ConfigurationHeader Config;	/* followed by config data */
    } u;
} ExtendedCommand;

/*
 * Receiver Idle:
 *	                      |--- outgoing Ack
 *	                            |--- rcvNext
 *	                            rcvWin : XMT_SEND_WINDOW
 *	-------------------------------------------------------------------
 *	|     |     |     |     |     |     |     |     |     |     |     |
 *	-------------------------------------------------------------------
 *
 * Sender Idle:
 *	                            ^--- xmtNext
 *	                            ^--- xmtUnacked
 *	                            xmtWin : XMT_SEND_WINDOW
 *
 * Sender 1 in flight:
 *	                                  ^--- xmtNext
 *	                            ^--- xmtUnacked
 *	                            xmtWin : XMT_SEND_WINDOW-1
 *
 * Sender all in flight:
 *	                                                    ^--- xmtNext
 *	                            ^--- xmtUnacked
 *	                            xmtWin : 0
 */


/*
 * Add a packet to the end of a Queue
 */
static void
AddToQueue(Packet *packet, PacketQueue *pQueue)
{
    Packet * otail;
    packet->bs_link = NULL;
    otail = pQueue->tail;
    if (otail) {
	otail->bs_link = (uchar *)packet;
    } else {
	pQueue->head = packet;
    }
    pQueue->tail = packet;
}

/*
 * bs_getbuffer - Get a packet for an output buffer
 */

static Packet *
bs_getbuffer (void)
{
    Packet * packet;

    while (TRUE) {
	packet = getbuffer(HEADER_SIZE + PACKET_SIZE - ENCAPBYTES);
	if (packet) {
	    packet->datagramsize = 0;
	    packet->datagramstart = packet->bs_buf;
	    return(packet);
	}	
	process_sleep_for(ONESEC/4);
    }
}

/*
 * Reset a packet to a known state
 */
static void
ClearPacket (Packet *packet)
{
    packet->datagramsize = 0;
    packet->datagramstart = packet->bs_buf;
}

/*
 * Remove and return the head packet of a Queue.
 * Returns null if there is nothing left in the queue.
 */
static Packet *
RemoveHeadOfQueue (PacketQueue *pQueue)
{
    Packet * packet;
    packet = pQueue->head;
    if (packet) {
	pQueue->head = (Packet *)packet->bs_link;
	if (!pQueue->head) {
	    pQueue->tail = NULL;
	}
    }
    return(packet);
}

/*
 * Remove and return a specific packet from a Queue.
 * If the packet is not on the queue (an internal error), this
 * routine returns null, otherwise, it returns the packet.
 */
static Packet *
RemovePacketFromQueue (
    SerialConnection *sc,
    Packet *packet,
    PacketQueue *pQueue)
{
    Packet * prevPacket = NULL;
    Packet * curPacket = pQueue->head;

    while (curPacket && (curPacket != packet)) {
	prevPacket = curPacket;
	curPacket = (Packet *)curPacket->bs_link;
    }
    if (curPacket) {
	if (prevPacket) {
	    prevPacket->bs_link = curPacket->bs_link;
	} else {
	    pQueue->head = (Packet *)curPacket->bs_link;
	}
	if (curPacket == pQueue->tail) {
	    pQueue->tail = prevPacket;
	}
    } else {
	buginf("\nXRemote%t: RHOQ: cannot find packet on queue",
	       sc->tty->ttynum);
    }
    return (curPacket);
}

/*
 * Start up the byte stream driver on a given line.
 */

Bool
BSStartup (SerialConnection *sc)
{
    int timeout = 20;
    ulong map;

    startoutput(stdio);

    while (outputpending(stdio) &&timeout--)
      process_sleep_for(ONESEC/2);

    if (!appp_tty_startup(stdio,
			  bs_raw_enqueue,
			  ENCAPBYTES,
			  HEADER_SIZE + PACKET_SIZE,
			  NULL)) {
	printf(nomemory);
	return(FALSE);
    }

#define CH_CR 0x0D
#define CH_LF 0x0A
#define CH_PLUS 0x2B		/* conflict under FULL_IETF_PPP_ESCAPES */
    map = 0L;
#define MAP(c) map |= (1 << (c))
    appp_add_quoted_char(stdio,CH_PLUS);	/* avoid +++ (hayes modem escapes) */
    appp_add_quoted_char(stdio,CH_PLUS+0x80);
    appp_add_quoted_char(stdio,CH_CR);	/* avoid rlogin escape */
    appp_add_quoted_char(stdio,CH_CR+0x80);
    MAP(CH_CR);
    appp_add_quoted_char(stdio,CH_LF);	/* avoid rlogin escape */
    appp_add_quoted_char(stdio,CH_LF+0x80);
    MAP(CH_LF);
    appp_add_quoted_char(stdio,CH_SUB); /* avoid cisco terminal mux escape */
    appp_add_quoted_char(stdio,CH_SUB+0x80);
    MAP(CH_SUB);
    appp_add_quoted_char(stdio,CH_GS);	/* avoid telnet escape */
    appp_add_quoted_char(stdio,CH_GS+0x80);
    MAP(CH_GS);
    appp_add_quoted_char(stdio,CH_RS);	/* avoid bridge terminal mux escape */
    appp_add_quoted_char(stdio,CH_RS+0x80);
    MAP(CH_RS);
    appp_add_quoted_char(stdio,PPP_XON);	/* avoid flow control chars */
    appp_add_quoted_char(stdio,PPP_XON+0x80);
    MAP(PPP_XON);
    appp_add_quoted_char(stdio,PPP_XOFF);	/* avoid flow control chars */
    appp_add_quoted_char(stdio,PPP_XOFF+0x80);
    MAP(PPP_XOFF);
#undef MAP

    /*
     * Update driver support, if any. Xremote always takes 
     * the receive map as 0 - no characters are discarded.
     */
    tty_pppmap(sc->tty, map, 0L);

    if (!SendSync(sc)) {
	if (xrbsflow_debug)
	  buginf("\nXRemote%t: cant send sync packet", stdio->ttynum);
	return FALSE;
    }
    return TRUE;
}

/*
 * Shut down the byte stream driver
 */
void
BSShutdown (SerialConnection *sc)
{
    paktype *pak;

    if (sc->state != BS_CLOSED) {
	if (xrbsflow_debug)
	    buginf("\nXRemote%t: BSShutdown state %d", 
		   sc->tty->ttynum, sc->state);
	SendReset(sc);
	process_sleep_for(ONESEC*3);		/* Let the reset get out */
	appp_tty_shutdown(sc->tty);
	while ((pak = RemoveHeadOfQueue(&sc->incomingPackets))) {
	    datagram_done(pak);
	}
    }
}

void
BSFree (SerialConnection *sc)
{
    if (sc->state != BS_CLOSED) {
	buginf( "\nXRemote%t: BSFree: State wasn't closed!", sc->tty->ttynum);
	return;
    }	       

    if (sc->incomingPackets.head != NULL) {
	buginf( "\nXRemote%t: BSFree: Incoming packets still on queue",
	       sc->tty->ttynum);
	return;
    }

    if (sc->retransmitPackets.head != NULL) {
	buginf( "\nXRemote%t: BSFree: Retransmit packets still on queue",
	       sc->tty->ttynum);
	return;
    }

    FreeCompression((serialctx *)sc->compression);

    Xfree(sc);
}

#ifndef DEBUG
#define CHECKQUEUES(sc, str)
#else DEBUG
#define CHECKQUEUES(sc, str) CheckQueues(sc,str)
static int
QueueLength (PacketQueue * pq)
{
    int i = 0;
    Packet * p = pq->head;
    while (p) {
	i++;
	p = p->bs_link;
    }
    return (i);
}

static void
CheckQueues (SerialConnection * sc, uchar * str)
{
    int total = 0;

    total += QueueLength(&sc->incomingPackets);
    total += QueueLength(&sc->retransmitPackets);
    if (total != sc->lastTotal)
	buginf("\nXRemote%t: CheckQueues (%s) old=%d, new=%d", 
	       sc->tty->ttynum, str, sc->lastTotal, total);
    sc->lastTotal = total;
}
#endif DEBUG

/*
 * Initialize the data structures used by the byte stream driver
 */
SerialConnection *
BSInit (tt_soc *tty, boolean reverseMode)
{
    register SerialConnection *sc;

    sc = (SerialConnection *)Xalloc(sizeof(SerialConnection));
    if (!sc)
      return(NULL);

    sc->compression = InitCompression();
    if (!sc->compression) {
	free(sc);
	return(NULL);
    }

    sc->tty = tty;
    sc->reverseMode = reverseMode;

    if (xrbsflow_debug)
	buginf("\nXRemote%t: BSInit from state %d to CLOSED", 
	       sc->tty->ttynum, sc->state);
    sc->state = BS_CLOSED;
    GET_TIMESTAMP(sc->sendTime);     /* force frame check */
    TIMER_SUB_DELTA(sc->sendTime, MS_FRAME_DELAY);
    sc->roundTripTime = RETRANS_INITIAL;
    CHECKQUEUES(sc, "Init");

    return(sc);
}

int
BSPollRead (SerialConnection *sc)
{
    return (sc->incomingPackets.head != NULL || sc->rawPackets.head != NULL);
}

int
BSPollWrite (SerialConnection *sc)
{
    return ((sc->state == BS_CONNECTED) && (sc->xmtWin != 0));
}

/*
 * Called when an assembled input packet has been processed.
 * The packet is removed from the input queue, and any
 * outgoing acks and window updates are posted.
 */
static void
DequeueInputPacket (SerialConnection *sc)
{
    Packet  *tempPacketPtr;
    PacketHeader * packHead;

    if (!(tempPacketPtr = RemoveHeadOfQueue(&sc->incomingPackets)))
	return;	/* strange: no head of queue... */

    packHead = (PacketHeader *)(tempPacketPtr->bs_buf);
    sc->rcvWin++;
    if ((SequenceGet(sc->rcvWin) == 0))
      if (xrbsproto_debug)
	buginf( "\nXremote%t: packet dequeued leaves window = 0 seq=%d", 
	       sc->tty->ttynum, packHead->Seq);
    if (
	(sc->rcvWin <= (RCV_WINDOW - (RCV_WINDOW/2)))
	||
	(sc->rcvUnacked > (RCV_WINDOW/2))
	)
	SendBareAck(sc);	/* ack immed since window is now open */
    else
	PostAck(sc);
    if (sc->rcvWin > RCV_WINDOW)
	buginf("\nXRemote%t: rcvWindow too high %d", 
	       sc->tty->ttynum, sc->rcvWin);
    datagram_done(tempPacketPtr);
}

/*
 * Send a packet on the given sc. The packet's Ack and Window fields
 * are filled in.  If there are any problems with getting the bytes
 * out, the value of this routine is FALSE.
 */
static Bool
SendPacket (SerialConnection *sc, Packet *packet)
{
    PacketHeader * packHead = (PacketHeader *)packet->datagramstart;
    Bool emitFrameBegin;

    if (!packHead->ExtendedCommand) {
	packHead->Window = sc->rcvWin;
	packHead->Ack = SequenceGet(sc->rcvNext-1);
    }
    GET_TIMESTAMP(packet->bs_sendTime);
    emitFrameBegin = ELAPSED_TIME(sc->sendTime) >= MS_FRAME_DELAY;
    if (xrbsproto_debug)
      buginf("\nXRemote%t: sending:packHead=0x%02x(%d)[comp=%d, ack=%d, win=%d, seq=%d], len=%d",
	     sc->tty->ttynum,
	     *(unsigned char *)packHead, *(unsigned char *)packHead,
	     packHead->Compressed,
	     packHead->Ack,
	     packHead->Window,
	     packHead->Seq,
	     packet->datagramsize);

    pak_lock(packet);
    appp_oqueue(sc->tty, packet);

    if (xrbsproto_debug)
	buginf("\nXRemote%t: sent packet, size=%d", sc->tty->ttynum,
	       packet->datagramsize);
    sc->xmtPackets++;
    sc->ackNeeded = FALSE;
    sc->rcvUnacked = 0;
    GET_TIMESTAMP(sc->sendTime);
    return TRUE;
}

static void
SendBareAck (SerialConnection *sc)
{
    Packet * ack;
    PacketHeader * h;
    if (!(ack = bs_getbuffer())) {
	return;
    }
    ClearPacket(ack);
    ack->datagramsize = HEADER_SIZE;
    h = (PacketHeader *)ack->datagramstart;
    h->ExtendedCommand = 0;
    h->Compressed = FALSE;
    h->Seq = sc->xmtNext;
    GET_TIMESTAMP(ack->bs_sendTime);
    if(!SendPacket(sc, ack)) {
	if (xrbsflow_debug)
	  buginf("\nXRemote%t: Cannot send bare ack", sc->tty->ttynum);
    }
    datagram_done(ack);
}

/*
 * Cancel an outstanding 'code' packet (eg: Sync) which has yet to be acked.
 */
static void
CancelInitialRetransmit (SerialConnection *sc, unsigned char code)
{
    Packet * retrans;
    if (!(retrans = sc->retransmitPackets.head)) {
	buginf("XRemote%t: no initial retransmit in state %d", 
	       sc->tty->ttynum, sc->state);
    } else {
	if (!(((PacketHeader *)retrans->datagramstart)->ExtendedCommand &&
	    (((ExtendedCommand *)retrans->datagramstart)->Header.ExtendedCommandCode == code))) {
	    buginf("\nXRemote%t: retransmit packet is not %d in state %d",
		sc->tty->ttynum, code, sc->state);
	} else {
	    if (xrbsflow_debug)
		buginf("\nXRemote%t: Cancel retransmit state=%d", 
		       sc->tty->ttynum, sc->state);
	    datagram_done(RemoveHeadOfQueue(&sc->retransmitPackets));
	}
    }
}

/*
 * Fill in the fields of a sync/syncack packet, following the
 * given connection info.
 */
static void
FillSyncPacket (
    SerialConnection *sc,
    Packet *sync,
    ConnectionHeader *pConn,
    int command)
{
    ExtendedCommand * ec;
    ec = (ExtendedCommand *)sync->datagramstart;
    ec->Header.ExtendedCommand = 1;
    ec->Header.ExtendedPad1 = 0;
    ec->Header.ExtendedPad2 = 0;
    ec->Header.ExtendedCommandCode = command;
    sync->datagramsize = ec->Header.ExtendedCommandLength =
	sizeof(ExtendedHeader) + sizeof(ec->u.Sync);
    ec->u.Sync.Version = pConn->Version;
    ec->u.Sync.ConnectionRandom1 = pConn->ConnectionRandom1;
    ec->u.Sync.ConnectionRandom2 = pConn->ConnectionRandom2;
    ec->u.Sync.Seq = pConn->Seq;
    ec->u.Sync.Window = pConn->Window;
    ec->u.Sync.ConnectionHeaderPad = 0;
    if (xrbsproto_debug)
	buginf("\nXRemote%t: FillSyncPacket: cmd=%02x, len=%02x(%d+%d), V=%d, r1=%02x, r2=%02x, seq=%02x, win=%02x",
	       sc->tty->ttynum,
	       ec->Header.ExtendedCommandCode,
	       sync->datagramsize,
	       sizeof(ExtendedHeader),
	       sizeof(ec->u.Sync),
	       ec->u.Sync.Version,
	       ec->u.Sync.ConnectionRandom1,
	       ec->u.Sync.ConnectionRandom2,
	       ec->u.Sync.Seq,
	       ec->u.Sync.Window);
}

/*
 * Send a reset packet on a connection.
 */
static void
SendReset (SerialConnection *sc)
{
    Packet * reset;
    ExtendedCommand * ec;
    paktype *pak;

    switch (sc->state) {
	case BS_CONNECTED:
        case BS_CLOSED:
	    while ((pak = RemoveHeadOfQueue(&sc->retransmitPackets))) {
		datagram_done(pak);
	    }
	    break;
	case SYNCSENT:
	case SYNCRCVD:
	    CancelInitialRetransmit(sc, BS_EC_SYNC);
	    break;
    }
    reset = bs_getbuffer();
    if (!reset) {
	return;
    }
    ClearPacket(reset);

    ec = (ExtendedCommand *)reset->datagramstart;
    ec->Header.ExtendedCommand = 1;
    ec->Header.ExtendedPad1 = 0;
    ec->Header.ExtendedPad2 = 0;
    ec->Header.ExtendedCommandCode = BS_EC_RESET;
    reset->datagramsize = ec->Header.ExtendedCommandLength =
	sizeof(ExtendedHeader);
    GET_TIMESTAMP(reset->bs_sendTime);
    if(!SendPacket(sc, reset)) {
	if (xrbsflow_debug)
	  buginf("\nXRemote%t: Cannot send reset", sc->tty->ttynum);
    }
    datagram_done(reset);
    if (xrbsflow_debug)
	buginf("\nXRemote%t: SendReset from state %d to CLOSED",
	       sc->tty->ttynum, sc->state);
    sc->state = BS_CLOSED;
}

/*
 * Send a sync packet on a connection.
 * Returns FALSE if the send fails.
 * Upon success, the sc is put into SYNCSENT state.
 */
static Bool
SendSync (SerialConnection *sc)
{
    Packet * sync;

    switch (sc->state) {
	case BS_CONNECTED:
	    buginf("\nXRemote%t: send sync in connected state",
		   sc->tty->ttynum);
	    break;
	case SYNCSENT:
	case SYNCRCVD:
	    CancelInitialRetransmit(sc, BS_EC_SYNC);
	    break;
	default:
	case BS_CLOSED:
	    if (sc->retransmitPackets.head)
	      buginf("\nXRemote%t: SendSync with retransmit packets when closed",
		     sc->tty->ttynum);
    }
    if (!(sync = bs_getbuffer())) {
	return FALSE;
    }
    ClearPacket(sync);
    /* Make a new connection number and initial sequence number */
    sc->xmtConnectionData.Version = BS_VERSION;
    sc->xmtConnectionData.ConnectionRandom1 = random_gen();
    sc->xmtConnectionData.ConnectionRandom2 = random_gen();
    sc->xmtNext = sc->xmtConnectionData.Seq = SequenceGet(random_gen());
    sc->xmtWin = sc->xmtConnectionData.Window = XMT_WINDOW;
    sc->xmtMax = SequenceGet(sc->xmtNext + sc->xmtWin);
    sc->xmtUnacked = SequenceGet(sc->xmtNext);
    GET_TIMESTAMP(sc->sendTime);     /* force frame check */
    TIMER_SUB_DELTA(sc->sendTime, MS_FRAME_DELAY);
    sc->retransInterval = sc->roundTripTime = RETRANS_INITIAL;

    FillSyncPacket(sc, sync, &sc->xmtConnectionData, BS_EC_SYNC);

    GET_TIMESTAMP(sync->bs_sendTime);
    AddToQueue(sync, &sc->retransmitPackets);
    if (xrbsflow_debug)
	buginf("\nXRemote%t: SendSync from state %d to SYNCSENT",
	       sc->tty->ttynum, sc->state);
    sc->state = SYNCSENT;
    if (!SendPacket(sc, sync)) {
	if (xrbsflow_debug)
	  buginf("\nXRemote%t: Cannot send sync", sc->tty->ttynum);
	return FALSE;
    }
    return TRUE;
}

static void
printConn (ConnectionHeader *pc, char *str)
{
    buginf("Connection Header %s\n", str);
    buginf(" Version:0x%02x\n", pc->Version);
    buginf(" ConnectionRandom1:0x%02x\n", pc->ConnectionRandom1);
    buginf(" ConnectionRandom2:0x%02x\n", pc->ConnectionRandom2);
    buginf(" Seq:0x%02x\n", pc->Seq);
    buginf(" Window:0x%02x\n", pc->Window);
}

/*
 * Now that the connection is up, haggle some options.
 */

static void
StartOptionTransmission (SerialConnection *sc)
{
    Packet * request;
    ConfigurationHeader * pc;
    ExtendedCommand * ec;
    if (!(request = bs_getbuffer())) {
	return;
    }
    ClearPacket(request);
    ec = (ExtendedCommand *) request->datagramstart;
    pc = (ConfigurationHeader *) (&ec->u.Config);
    pc->ConfigurationLength = sizeof(ec->u.Config) + 1;
    pc->ConfigurationUnique = random_gen();
    pc->ConfigurationCode = CRQ_ADD_OPAQUE_CHAR;
    pc->ConfigurationPad1 = 0;
    *((char *)(pc+1)) = 0x0A;
    ec->Header.ExtendedCommand = 1;
    ec->Header.ExtendedPad1 = 0;
    ec->Header.ExtendedPad2 = 0;
    ec->Header.ExtendedCommandCode = BS_EC_CONFIG_REQ;
    request->datagramsize = ec->Header.ExtendedCommandLength =
	sizeof(ExtendedHeader) + sizeof(ec->u.Config) + 1;
    GET_TIMESTAMP(request->bs_sendTime);
    AddToQueue(request, &sc->retransmitPackets);
}

static void
DoConfigReq (
    SerialConnection *sc,
    ExtendedCommand *ec,
    int length)
{
    ConfigurationHeader * pc = (ConfigurationHeader *) (&ec->u.Config);
    int outlength = 0;
    Packet * reply;
    length -= sizeof(ExtendedHeader);
    outlength += sizeof(ExtendedHeader);
    if (length <= 0) {
	sc->rcvErrors++;
	return;
    }
    if ((length >= sizeof (ConfigurationHeader)) &&
	(length == pc->ConfigurationLength)) {
	length -= sizeof (ConfigurationHeader);
	switch (pc->ConfigurationCode) {
	    case CRQ_ADD_OPAQUE_CHAR:
		if (length) {
		    length -= 1;
		    if (appp_add_quoted_char(sc->tty,*(char *)(pc+1))) {
			pc->ConfigurationCode = CRP_ACK;
		    } else
			pc->ConfigurationCode = CRP_NAK;
		    outlength += sizeof (ConfigurationHeader);
		} else {
		    pc->ConfigurationCode = CRP_NAK;
		    outlength += sizeof (ConfigurationHeader);
		}
		break;
	    /*
	     * send an negative ack so at least
	     * the requestor knows we wont do it.
	     */
	    default:
		pc->ConfigurationCode = CRP_NAK;
		outlength += sizeof (ConfigurationHeader);
	}
    } else {
	if (xrbsproto_debug)
	  buginf("\nXRemote%t: bad config length %d:%d", 
		 sc->tty->ttynum, length, pc->ConfigurationLength);
	sc->rcvErrors++;
	return;
    }
    ec->Header.ExtendedCommandCode = BS_EC_CONFIG_REP;
    ec->Header.ExtendedCommandLength = outlength;
    if (!(reply = bs_getbuffer())) {
	return;
    }
    ClearPacket(reply);
    bcopy((char *)ec, (char *)reply->datagramstart, outlength);
    reply->datagramsize = outlength;
    if(!SendPacket(sc, reply)) {
	if (xrbsflow_debug)
	  buginf("\nXRemote%t: Cannot send config reply", sc->tty->ttynum);
    }
    datagram_done(reply);
}

/*
 * Handle an incoming reply, which usually arrives in response to one
 * of our Set or Get requests.
 */
static void
DoConfigRep (
    SerialConnection *sc,
    ExtendedCommand *ec,
    int length)
{
    Packet * rtpack;
    Packet * nextPacket;
    ExtendedCommand * rtec;
    ConfigurationHeader * pc;
    nextPacket = sc->retransmitPackets.head;
    while ((rtpack = nextPacket)) {
	nextPacket = (Packet *)rtpack->bs_link;
	rtec = (ExtendedCommand *)(rtpack->datagramstart);
	length -= (sizeof(ExtendedHeader) + sizeof(ConfigurationHeader));
	if (length < 0) {
	    sc->rcvErrors++;
	    continue;
	}
	if (rtec->Header.ExtendedCommand &&
	    (rtec->Header.ExtendedCommandCode == BS_EC_CONFIG_REQ) &&
	    (rtec->u.Config.ConfigurationUnique ==
	       ec->u.Config.ConfigurationUnique)) {
	    switch (rtec->u.Config.ConfigurationCode) {
		case CRQ_ADD_OPAQUE_CHAR:
		    pc = (ConfigurationHeader *) (&rtec->u.Config);
		    if (ec->u.Config.ConfigurationCode != CRP_ACK) {
			if (xrbsproto_debug)
			  buginf( "\nXRemote%t: NAK from CRQ_ADD_OPAQUE_CHAR",
				 sc->tty->ttynum);
		    } else {
			if (!appp_add_quoted_char(sc->tty,*(char *)(pc+1))) {
			    buginf(
			      "\nXRemote%t: cannot add requested char to opaque list 0x%x\n",
			      sc->tty->ttynum, *(char *)(pc+1));
			}
		    }
		    break;
	    }
	    (void)RemovePacketFromQueue(sc, rtpack, &sc->retransmitPackets);
	    datagram_done(rtpack);
	}
    }
}

/*
 * Deal with an extended command packet with known good checksum.
 * The packet itself will be returned to the unused queue.
 */
static void
HandleExtendedCommand (SerialConnection *sc, Packet *packet)
{
    ExtendedCommand * ec = (ExtendedCommand *) packet->datagramstart;

    /* Check the length of the packet */
    if ((packet->datagramsize < sizeof(ExtendedHeader)) ||
	(packet->datagramsize != ec->Header.ExtendedCommandLength)) {
	sc->rcvErrors++;
	datagram_done(packet);
	return;
    }
    switch (ec->Header.ExtendedCommandCode) {
	case BS_EC_SYNC:
	case BS_EC_SYNC_ACK:
	    if (packet->datagramsize <
		(sizeof(ExtendedHeader) + sizeof(ec->u.Sync)))
	    {
		sc->rcvErrors++;
		datagram_done(packet);
		return;
	    }
	    break;
	case BS_EC_RESET:
	    if (xrbsflow_debug)
		buginf("\nXRemote%t: Reset packet received", sc->tty->ttynum);
	    datagram_done(packet);
	    xremote_reset((xrcontext *)stdio->xrcb, "Reset by X terminal");
	    BSShutdown(sc);
	    return;
	case BS_EC_CONFIG_REQ:
	    if (packet->datagramsize <
		(sizeof(ExtendedHeader) + sizeof(ec->u.Config)))
	    {
		sc->rcvErrors++;
		datagram_done(packet);
		return;
	    }
	    break;
	case BS_EC_CONFIG_REP:
	    if (packet->datagramsize <
		(sizeof(ExtendedHeader) + sizeof(ec->u.Config)))
	    {
		sc->rcvErrors++;
		datagram_done(packet);
		return;
	    }
	    break;
	default:
	    if (xrbsproto_debug)
	      buginf("\nXRemote%t: unknown ext cmd:%02x\n",
		     sc->tty->ttynum, ec->Header.ExtendedCommandCode);
	    sc->rcvErrors++;
	    datagram_done(packet);
	    return;
    }
    switch (ec->Header.ExtendedCommandCode) {
      case BS_EC_SYNC:
	switch (sc->state) {
	  case BS_CONNECTED:
	  case SYNCRCVD:
	    if (EqualConnection(&ec->u.Sync, &sc->rcvConnectionData)) {
		ec->u.Sync.Version = MIN(BS_VERSION, sc->rcvConnectionData.Version);
		ec->Header.ExtendedCommandCode = BS_EC_SYNC_ACK;
		if (!SendPacket(sc, packet)) {
		    if (xrbsflow_debug)
		      buginf("\nXRemote%t: Cannot send sync ack",
			     sc->tty->ttynum);
		}
	    } else {
		if (xrbsproto_debug)
		  buginf("\nXRemote%t: different sync rcvd", sc->tty->ttynum);
	    }
	    break;

	  case SYNCSENT:
	    /* save the other side's connection data and echo it back */
	    sc->rcvConnectionData = ec->u.Sync;
	    sc->rcvWin = sc->rcvConnectionData.Window;
	    sc->rcvNext = SequenceGet(sc->rcvConnectionData.Seq);
	    sc->rcvUnacked = 0;
	    sc->rcvOutOfSequence = 0;
	    if ((SequenceGet(sc->rcvWin) == 0))
		buginf( "\nXRemote%t: SYNC sets window = 0 seq=%d", 
		       sc->tty->ttynum, sc->rcvNext);
	    ec->u.Sync.Version = MIN(BS_VERSION, sc->rcvConnectionData.Version);
	    ec->Header.ExtendedCommandCode = BS_EC_SYNC_ACK;
	    if (!SendPacket(sc, packet)) {
		if (xrbsflow_debug)
		  buginf("\nXRemote%t: Cannot send sync ack", sc->tty->ttynum);
	    }
	    if (xrbsflow_debug)
	      buginf("\nXRemote%t: HEP:RCV:SYNC from state %d to SYNCRCVD",
		     sc->tty->ttynum, sc->state);
	    sc->state = SYNCRCVD;
	    break;

	  default:
	    if (xrbsproto_debug)
	      buginf("\nXRemote%t: unexpected Sync received state=%d", 
		     sc->tty->ttynum, sc->state);
	}
	break;
      case BS_EC_SYNC_ACK:
	  if (!EqualConnection(&ec->u.Sync, &sc->xmtConnectionData)) {
	      buginf("\nXRemote%t: different SyncAck rcvd state=%d", 
		     sc->tty->ttynum, sc->state);
	      printConn(&ec->u.Sync, "&ec->u.Sync");
	      printConn(&sc->xmtConnectionData, "&sc->xmtConnectionData");
	  } else
	      switch (sc->state) {
	      case BS_CONNECTED:
	      case SYNCSENT:
		  break;

	      case SYNCRCVD:
		  if (xrbsflow_debug)
		      buginf("\nXRemote%t: HEP:RCV:SYNC_ACK from state %d to CONNECTED",
			  sc->tty->ttynum, sc->state);

		  sc->operatingVersion = MIN(BS_VERSION, sc->rcvConnectionData.Version);
		  if (sc->retransmitPackets.head)
		      sc->roundTripTime =
			  ELAPSED_TIME(sc->retransmitPackets.head->bs_sendTime);
		  RANGESET(sc->retransInterval, sc->roundTripTime<<1, RETRANS_MIN, RETRANS_MAX);
		  CancelInitialRetransmit(sc, BS_EC_SYNC);

		  sc->state = BS_CONNECTED;
		  RestartCompression((serialctx *)sc->compression);
		  if ((!sc->reverseMode) && sc->operatingVersion > 1)
		    StartOptionTransmission(sc);
		  break;
	      default:
		  if (xrbsproto_debug)
		    buginf("\nXRemote%t: unexpected SyncAck received state=%d", 
			   sc->tty->ttynum, sc->state);
	      }
	break;
      case BS_EC_CONFIG_REQ:
	DoConfigReq(sc, ec, packet->datagramsize);
	break;
      case BS_EC_CONFIG_REP:
	DoConfigRep(sc, ec, packet->datagramsize);
	break;
    }
    datagram_done(packet);
}

static void
DeliverDelayedAcks (SerialConnection *sc)
{
    if (sc->ackNeeded && ELAPSED_TIME(sc->ackPostTime) >= MS_ACK_DELAY) {
	if (xrbsproto_debug)
	    buginf("\nXRemote%t: Delivering delayed ack", sc->tty->ttynum);
	SendBareAck(sc);
    }
}

static void
PostAck (register SerialConnection *sc)
{
    if (!sc->ackNeeded) {
	sc->ackNeeded = TRUE;
	GET_TIMESTAMP(sc->ackPostTime);
	if (xrbsproto_debug)
	    buginf("\nXRemote%t: posting ack", sc->tty->ttynum);
    }
}

static void
PostProbe (register SerialConnection *sc)
{
    GET_TIMESTAMP(sc->windowCloseTime);
    sc->probeInterval = PROBE_INITIAL;
}

static void
QueueOutputPacket (
    SerialConnection *sc,
    Packet *packet,
    Bool isCompressed)
{
    PacketHeader * h;

    if (!sc->xmtWin)
      buginf("\nXRemote%t: QueueOutputPacket without transmit credits",
	     sc->tty->ttynum);

    h = (PacketHeader *)packet->bs_buf;
    packet->bs_wasRetransmitted = FALSE;
    h->ExtendedCommand = 0;
    h->Compressed = isCompressed;
    h->Seq = sc->xmtNext++;
    if (--sc->xmtWin == 0)
	PostProbe(sc);
    GET_TIMESTAMP(packet->bs_sendTime);
    (void)SendPacket(sc, packet);
    AddToQueue(packet, &sc->retransmitPackets);
}

int 
BSRead (
    SerialConnection *sc,
    unsigned char *buffer,
    int count)
{
    Packet * packet;
    int             total = 0;

    GetNewPackets(sc);

    while (count && (packet = sc->incomingPackets.head)) {
	int             bytes;
	if (count >= packet->datagramsize) {
	    bytes = packet->datagramsize;
	    bcopy(packet->datagramstart, buffer, bytes);
	    DequeueInputPacket(sc);
	} else {
	    bytes = count;
	    bcopy(packet->datagramstart, buffer, bytes);
	    packet->datagramstart += bytes;
	    packet->datagramsize -= bytes;
	}
	total += bytes;
	count -= bytes;
	buffer += bytes;
    }
    return (total);
}

int
BSWrite (
    SerialConnection *sc,
    unsigned char *buffer,
    int count)
{
    int			compressedLen;
    int             	total = 0;
    int             	origcount = count;
    Packet		*packet;

    if (xrbsflow_debug)
	buginf("\nXRemote%t: BSWrite count=%d, window=%d", 
	       sc->tty->ttynum, count, sc->xmtWin);

    /*
     * Process any pending input before we write out more data. This may
     * cause us to get some ACKs and expand our transmit window.
     */

    GetNewPackets(sc);

    while (count && sc->xmtWin) {
	int             bytes;
	Bool		compressed;
	if (count >= MAX_LZW_PLAIN)
	    bytes = MAX_LZW_PLAIN;
	else
	    bytes = count;

	packet = bs_getbuffer();
	if (!packet) {
	    break;
	}
	ClearPacket(packet);
	
#ifdef ALLOWCOMPRESSION
	compressedLen = SerialCompress((serialctx *)sc->compression,
				       buffer, bytes,
				       packet->bs_buf + HEADER_SIZE,
				       PACKET_SIZE);
	if (compressedLen == -1)
#endif ALLOWCOMPRESSION
	{
	    compressed = FALSE;
	    packet->datagramsize = bytes + HEADER_SIZE;
	    bcopy(buffer, packet->bs_buf + HEADER_SIZE, bytes);
	}
#ifdef ALLOWCOMPRESSION
	else {
	    compressed = TRUE;
	    packet->datagramsize = compressedLen + HEADER_SIZE;
	}
#endif ALLOWCOMPRESSION

	QueueOutputPacket(sc, packet, compressed);

	buffer += bytes;
	count -= bytes;
	total += bytes;
    }
    if (xrbsflow_debug)
	buginf("\nXRemote%t: BSWrite req/total=%d/%d", 
	       sc->tty->ttynum, origcount, total);
    return (total);
}

/*
 * If our transmit window is closed, and we haven't heard from the
 * other end in a while, send a bogus packet with no data
 * to which there will be a reply, perhaps with a new window update.
 */
static void
DoWindowProbe (SerialConnection *sc)
{
    if (sc->state == BS_CONNECTED && sc->xmtWin == 0) {
	if (ELAPSED_TIME(sc->windowCloseTime) >= sc->probeInterval) {
	    sc->xmtNext = SequenceGet(sc->xmtNext+1);
	    if (xrbsproto_debug)
		buginf("\nXRemote%t: sending window probe seq=%d", 
		       sc->tty->ttynum, sc->xmtNext);
	    SendBareAck(sc);
	    sc->xmtNext = SequenceGet(sc->xmtNext-1);
	    if (sc->probeInterval == PROBE_MAX) {
		TIMER_UPDATE(sc->windowCloseTime, PROBE_MAX);
	    } else {
		sc->probeInterval = MIN(PROBE_MAX, sc->probeInterval << 1);
	    }
	}
    }
}

/*
 * Retransmit the head of the retransmission queue, or all of the
 * retransmission queue, depending on the sendAll parameter.
 * If any packets are queued, the value of this routine is TRUE.
 */
static Bool
DoRetransmit (
    register SerialConnection *sc,
    Bool sendAll)
{
    Bool sent = FALSE;
    Packet * p = sc->retransmitPackets.head;

    if (!p)
      buginf("\nXRemote%t: DoRetransmit with null retransmission queue",
	     sc->tty->ttynum);

    while (p) {
	if (pak_shared(p))
	  break;		/* Get out if transmit hasn't completed  */

	if(SendPacket(sc, p)) {
	    sc->xmtRetransmissions++;
	    sent = TRUE;
	    if (sendAll)
		p = (Packet *)p->bs_link;
	    else
		break;
	} else
	    break;
    }
    return(sent);
}

/*
 * If the head of the retransmission queue has expired its interval,
 * retransmit the packet.
 */
static void
CheckRetransmissionQueue (SerialConnection *sc)
{
    register Packet * packet;
    if (!(packet = sc->retransmitPackets.head))
	return;
    if (ELAPSED_TIME(packet->bs_sendTime) >= sc->retransInterval) {
	if (xrbsproto_debug)
	    buginf("\nXRemote%t: CRQ:retrans seq:%d(%d) delta=%d, int=%d",
		   sc->tty->ttynum,
		   ((PacketHeader *)sc->retransmitPackets.head->datagramstart)->Seq,
		   sc->retransmitPackets.head->datagramsize,
		   ELAPSED_TIME(packet->bs_sendTime),
		   sc->retransInterval);
	if ((sc->state != BS_CONNECTED) && (sc->retransInterval >= RETRANS_MAX)) {
	    if (xrbsproto_debug)
	      buginf("\nXRemote%t: Cannot establish connection, resetting",
		     sc->tty->ttynum);
	    xremote_reset((xrcontext *)stdio->xrcb,
			  "Unable to establish XRemote connection");
	    BSShutdown(sc);
	    return;
	}
	if (DoRetransmit(sc, FALSE)) {
	    packet->bs_wasRetransmitted = TRUE;
	    RANGESET(sc->retransInterval, sc->retransInterval<<1, RETRANS_MIN, RETRANS_MAX);
	}
    }
}

/*
 * Update the round trip timers for the sc, in light of the
 * given packet's acknowledgement.  If the packet was retransmitted,
 * don't allow the time to decrease.
 */
static void
FigureRoundTrip (
    register SerialConnection *sc,
    register Packet *packet)
{
    unsigned long roundTripTime;

    roundTripTime = ELAPSED_TIME(packet->bs_sendTime);
    if (packet->bs_wasRetransmitted)
	roundTripTime = MAX(roundTripTime, sc->roundTripTime);
    sc->roundTripTime = (roundTripTime >=  sc->roundTripTime) ?
	roundTripTime : ((sc->roundTripTime * 15) + roundTripTime) >> 4;
    RANGESET(sc->retransInterval, sc->roundTripTime<<1, RETRANS_MIN, RETRANS_MAX);
    if (xrbsproto_debug) {
	buginf("\nXRemote%t: seq=%d:rtt=%d, del=%d, srtt=%d, interval=%d",
	       sc->tty->ttynum,
	       ((PacketHeader *)(packet->datagramstart))->Seq,
	       roundTripTime,
	       sc->roundTripTime,
	       sc->retransInterval);
    }
}

/*
 * Given a raw packet with its escaped characters interpreted, verify
 * its checksum, and pass it up the stack.  The given packet
 * will be attached to some queue; the caller must not use this
 * after invocation.
 */
static void
HandleAssembledPacket (
    register SerialConnection *sc,
    register Packet *packet)
{
    PacketHeader packHead;
    int SendAckNow = 0;
    CHECKQUEUES(sc, "HandleAssembledPacket begin");

    if (packet->datagramsize < HEADER_SIZE) {
	datagram_done(packet);
	sc->rcvErrors++;
	return;
    }

    sc->rcvPackets++;
    /* grab the header */
    packHead = *(PacketHeader *)packet->datagramstart;
    if (packHead.ExtendedCommand) {
	HandleExtendedCommand(sc, packet);
	return;
    }
    if (sc->state != BS_CONNECTED) {
	if (xrbsproto_debug)
	    buginf("\nXRemote%t: normal packet received in non-connected state",
		   sc->tty->ttynum);
	datagram_done(packet);
	sc->rcvErrors++;
	return;
    }
    if (xrbsproto_debug)
	buginf("\nXRemote%t: recvd:packHead=0x%02x(%d)[comp=%d, ack=%d, win=%d, seq=%d], len=%d",
	       sc->tty->ttynum,
	       *(unsigned char *)packet->datagramstart, *(unsigned char *)packet->datagramstart,
	       packHead.Compressed,
	       packHead.Ack,
	       packHead.Window,
	       packHead.Seq,
	       packet->datagramsize);

    /* Handle the Ack field */
    if (SequenceGet(sc->xmtUnacked-1) == packHead.Ack) {
	if (xrbsproto_debug)
	    buginf("\nXRemote%t: dup ack %d", sc->tty->ttynum, packHead.Ack);
    } else {
	Packet * rtpack;
	Packet * nextPacket;
	Bool retransmitAck = FALSE;
	if (xrbsproto_debug)
	    buginf("\nXRemote%t: xmtUnacked=%d, ph.ack=%d", 
		   sc->tty->ttynum, sc->xmtUnacked, packHead.Ack);
	nextPacket = sc->retransmitPackets.head;
	while ((rtpack = nextPacket)) {
	    Bool done;
	    nextPacket = (Packet *)rtpack->bs_link;
	    if (((PacketHeader *)(rtpack->datagramstart))->ExtendedCommand)
		continue;
	    if (xrbsproto_debug)
		buginf("\nXRemote%t: rtseq=%d, una=%d",
		       sc->tty->ttynum,
		       ((PacketHeader *)(rtpack->datagramstart))->Seq,
		       sc->xmtUnacked);
	    done = ((PacketHeader *)(rtpack->datagramstart))->Seq ==
		packHead.Ack;
	    sc->xmtUnacked = SequenceGet(((PacketHeader *)(rtpack->datagramstart))->Seq+1);
	    if (rtpack->bs_wasRetransmitted)
		retransmitAck = TRUE;
	    FigureRoundTrip(sc, rtpack);
	    (void)RemovePacketFromQueue(sc, rtpack, &sc->retransmitPackets);
	    datagram_done(rtpack);
	    if (done)
		break;
	}
	/* If we receive an ACK for a packet which we had to retransmit,
	 * then dump the whole retransmission queue (if any outstanding
	 * packets remain).
	 */
	if (retransmitAck && sc->retransmitPackets.head) {
	    (void)DoRetransmit(sc, TRUE);
	}
	if (!rtpack) {
	    if (xrbsproto_debug)
	      buginf("\nXRemote%t: cant find retransmit packet for ack %d!",
		     sc->tty->ttynum, packHead.Ack);
	}
    }

    /* Handle the window field */
    if (SequenceGet(packHead.Ack+packHead.Window+1) != sc->xmtMax) {
	int newXmtWin;
	int newXmtMax;
	newXmtMax = SequenceGet(packHead.Ack + packHead.Window + 1);
	newXmtWin = SequenceGet(newXmtMax - sc->xmtNext);
	if (xrbsproto_debug)
	    buginf("\nXRemote%t: Window update: Ack:%d, Window:%d, xmtMax:%d, xmtWin:%d ==> xmtWin:%d",
		   sc->tty->ttynum,
	      packHead.Ack, packHead.Window, sc->xmtMax, sc->xmtWin, newXmtWin);
	sc->xmtWin = newXmtWin;
	sc->xmtMax = newXmtMax;
	if (sc->xmtWin > XMT_WINDOW) {
	    if (xrbsproto_debug)
	      buginf("\nXRemote%t: xmtWindow too high %d", 
		     sc->tty->ttynum, sc->xmtWin);
	    sc->xmtWin = XMT_WINDOW;
	}
    }

    /* Handle the Seq field */
    if (packHead.Seq != SequenceGet(sc->rcvNext)) {
	if (xrbsproto_debug)
	    buginf("\nXRemote%t: Out of sequence packet received: is:0x%x, expected:0x%x",
		sc->tty->ttynum, packHead.Seq, sc->rcvNext);
	datagram_done(packet);
	/* force an ack / window update as an answer to a window probe */
	SendAckNow++;
	sc->rcvOutOfSequence++;
    } else {
	if (packet->datagramsize > HEADER_SIZE) {
	    /* packet has some data */
	    if (xrbsproto_debug)
	      buginf("\nXRemote%t: packet assembled=%d, %s", 
		     sc->tty->ttynum, packet->datagramsize,
		     packHead.Compressed?"LZW":"plain");
	    if (packHead.Compressed && (packet->datagramsize == HEADER_SIZE)) {
		if (xrbsproto_debug)
		  buginf("\nXRemote%t: Compressed ACK/WIN update rcvd",
			 sc->tty->ttynum);
		packHead.Compressed = FALSE;
	    }
	    if (packHead.Compressed) {
		int		compressedLen;
		int		decompressedLen;
		unsigned char decompBuf[PACKET_SIZE];
		
		compressedLen = packet->datagramsize - HEADER_SIZE;
		decompressedLen =
		  SerialDecompress((serialctx *)sc->compression,
				   packet->datagramstart + HEADER_SIZE,compressedLen,
				   decompBuf, PACKET_SIZE);
		if (decompressedLen == -1) {
		    if (xrbsflow_debug)
		      buginf("\nXRemote%t: decompress Failed, resetting connection",
			       sc->tty->ttynum);
		    datagram_done(packet);
		    sc->rcvErrors++;
			xremote_reset((xrcontext *)stdio->xrcb,
				  "Decompression failed");
		    BSShutdown(sc);
		    return;
		}
		    if (packet->datagramstart + HEADER_SIZE + decompressedLen >=
		  packet->bs_buf + HEADER_SIZE + PACKET_SIZE + TRAILER_SIZE)
		  if (xrbsproto_debug)
		    buginf("\nXRemote%t: Decompressed packet is too big",
			   sc->tty->ttynum);
		bcopy(decompBuf, packet->datagramstart + HEADER_SIZE, decompressedLen);
		packet->datagramsize = decompressedLen + HEADER_SIZE;
	    }
	    packHead.Compressed = FALSE;
	    sc->rcvNext = SequenceGet(packHead.Seq+1);
	    if ((SequenceGet(sc->rcvWin) == 0))
	      if (xrbsproto_debug)
		buginf( "\nXRemote%t: Packet received with window = 0 seq=%d",
		       sc->tty->ttynum, packHead.Seq);
	    sc->rcvWin--;
	    packet->datagramsize -= HEADER_SIZE;
	    packet->datagramstart += HEADER_SIZE;
	    AddToQueue(packet, &sc->incomingPackets);
	    sc->rcvUnacked++;
	    PostAck(sc);
	} else {
	    if (xrbsproto_debug)
		buginf("\nXRemote%t: No data same seq=%d",
		       sc->tty->ttynum, packHead.Seq);
	    datagram_done(packet);
	}
    }
    if (SendAckNow)
      SendBareAck(sc);
    CHECKQUEUES(sc, "HandleAssembledPacket end");
}

static void
GetNewPackets (SerialConnection *sc)
{
    paktype *pak;
    leveltype status;

    while ((sc->state != BS_CLOSED)) {
	status = raise_interrupt_level(TTY_DISABLE);
	pak = RemoveHeadOfQueue(&sc->rawPackets);
	reset_interrupt_level(status);
	if (!pak)
	  break;
	HandleAssembledPacket(sc, pak);
    }
}

static void
bs_raw_enqueue (paktype *pak, tt_soc *tty)
{
    xrcontext *ctx;
    SerialConnection *sc;

    ctx = (xrcontext *)tty->xrcb;

    if (ctx) {
	sc = (SerialConnection *)ctx->serialContext;
	if (sc) {
	    AddToQueue(pak, &sc->rawPackets);
	    return;
	}
    }
    retbuffer(pak);		/* Closing down - dump packet */
}

/*
 * This routine is called when the server is about to snooze.
 * We add any protocol timeout to the wakeup, if our protocol
 * timeout is less than the given timeval wt, and our fd is
 * about to be selected for.
 */

boolean BSBlock (SerialConnection *sc)
{
    register Packet * packet;
    Bool needToWait = FALSE;
    int waitTime = 0;

    if (sc->rawPackets.head != NULL)
      return(FALSE);

    if (sc->ackNeeded) {
	int togo = MS_ACK_DELAY - ELAPSED_TIME(sc->ackPostTime);
	if (togo < 0)
	    togo = 0;
	if (!needToWait || togo < waitTime)
	    waitTime = togo;
	needToWait = TRUE;
    }
    packet = sc->retransmitPackets.head;
    if (packet) {
	int togo =  sc->retransInterval - ELAPSED_TIME(packet->bs_sendTime);
	if (togo < 0)
	    togo = 0;
	if (!needToWait || togo < waitTime)
	    waitTime = togo;
	needToWait = TRUE;
    }
    if (sc->xmtWin == 0) {
	int togo =  sc->probeInterval - ELAPSED_TIME(sc->windowCloseTime);
	if (togo < 0)
	    togo = 0;
	if (!needToWait || togo < waitTime)
	    waitTime = togo;
	needToWait = TRUE;
    }
    if (needToWait) {
	return(waitTime != 0);
    }
    return(TRUE);
}

/*
 * This routine is invoked after the select returns.
 */

void BSWakeup (SerialConnection *sc)
{
    GetNewPackets(sc);
    DoWindowProbe(sc);
    DeliverDelayedAcks(sc);
    CheckRetransmissionQueue(sc);
}


void XremoteStatsDisplay (SerialConnection *sc)
{
    char *stateName;

    switch (sc->state) {
      case BS_CLOSED:
	stateName = "Closed"; break;
      case SYNCSENT:
	stateName = "SYNCSENT"; break;
      case SYNCRCVD:
	stateName = "SYNCRCVD"; break;
      case BS_CONNECTED:
	stateName = "Connected"; break;
      default:
	stateName = "Unknown?";
    }

    printf("\n  Protocol version:    %9d          Line state: %s"
	   "\n  Transmit packets:    %9d          Receive packets: %d"
	   "\n  Transmit errors:     %9d          Receive errors: %d"
	   "\n  Retransmissions:     %9d          Receive out of sequence: %d"
	   "\n  Round trip time:     %9d          Retransmit interval: %d"
	   "\n  Transmit window:     %9d          Receive window: %d"
	   "\n  Transmit next:       %9d          Receive next: %d"
	   "\n  Transmit unacked:    %9d          Receive unacked: %d",
	   sc->state == BS_CONNECTED ? sc->operatingVersion : BS_VERSION,
	   stateName,
	   sc->xmtPackets, sc->rcvPackets,
	   sc->xmtErrors, sc->rcvErrors,
	   sc->xmtRetransmissions, sc->rcvOutOfSequence,
	   sc->roundTripTime, sc->retransInterval,
	   SequenceGet(sc->xmtWin), SequenceGet(sc->rcvWin),
	   SequenceGet(sc->xmtNext), SequenceGet(sc->rcvNext),
	   SequenceGet(sc->xmtUnacked), SequenceGet(sc->rcvUnacked));
}
