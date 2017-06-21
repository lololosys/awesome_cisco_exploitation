/* $Id: xremote.h,v 3.2 1995/11/17 19:26:49 gstovall Exp $
 * $Source: /swtal/cherf.111/ios/sys/xremote/xremote.h,v $
 *------------------------------------------------------------------
 * xremote.h - Definitions for Xremote protocol helper process
 *
 * September 1990, Kevin Herbert
 *
 * Copyright (c) 1990-1997 by cisco Systems, Inc.
 *
 * Portions of this module are based on public domain software and/or
 * software copyright (c) 1990 by Network Computing Devices, Inc., and
 * used with permission.
 *------------------------------------------------------------------
 * $Log: xremote.h,v $
 * Revision 3.2  1995/11/17  19:26:49  gstovall
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  14:01:43  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.5  1995/08/25  11:46:03  hampton
 * Miscellaneous changes to eliminate direct references to the system
 * clock and to convert to always using the timer macros to access timers.
 * [CSCdi39258]
 *
 * Revision 2.4  1995/08/08 16:51:45  hampton
 * Miscellaneous changes to eliminate direct references to the system
 * clock and to convert to always using the timer macros to access timers.
 * [CSCdi38391]
 *
 * Revision 2.3  1995/07/21 01:47:34  billw
 * CSCdi36156:  No AAA accounting messages for ARAP or Xremote
 *
 * Revision 2.2  1995/06/20  03:45:28  ahh
 * CSCdi36095:  Connection routines use msclock
 * Remove more msclock references.
 *
 * Revision 2.1  1995/06/07  23:35:23  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __XREMOTE_H__
#define __XREMOTE_H__

#include "packet.h"
#include "../ip/ip.h"
#include "ttysrv.h"
#include "misc.h"
#include "Xdmcp.h"
#include "Xmd.h"

#if defined(mc68000) || defined(sparc)
#define BITSBIGENDIAN
#endif various machines

/*
 * Commonly used states for clients.  Not communicated across
 * the Xremote protocol (ie, private to helper and server)
 */
typedef enum {
  CS_NEW,CS_ACTIVE,CS_HANGUP,CS_AWAIT_HANGUP_REPLY,CS_HANGUP_REPLY
  } ClientState;

/*
 * Xremote helper side states returnable on any output operation.
 */
#define BLOCKED 0
#define NOTREADY 1
#define OUT_READY 2
#define DIAL_ERROR 3
#define SWITCH_CLIENT 4

#define DEBUG_DELTAS xrdelta_debug
#define DEBUG_REQUESTS xrrequest_debug
#define DEBUG_REPLIES xrreply_debug
#define DEBUG_FLOW xrflow_debug
#define DEBUG_FILES xrfiles_debug
#define DEBUG_PACKETS xrpacket_debug

typedef BYTE XRemoteClientId;

struct shortcommand{
  BYTE opcode;
  XRemoteClientId client;
};

/*
 * Request from the server to the helper to read some bytes from somewhere
 * in a file.  Resembles an X-reply packet.
 */
typedef struct _FileReadRequest {
    BYTE opcode;	/* REPLY_READ_FILE */
    BYTE transaction_unique;
    BYTE Pad1;
    BYTE Pad2;
    CARD32 requestlength B32;/* number of 4-byte words in this request */
    CARD32 offset B32;
    CARD32 length B32;	/* number of bytes to read (must be < 2^16, see
			    FileReadData length field) */
    CARD32 fileNameLength B32;	/* #bytes in following string */
    /* followed by the string of the file name, padded to 4 byte boundary */
} FileReadRequest;
#define MAXFileReadRequest (65535)

/*
 * Request from the server to the helper to indicate that
 * normal X transactions may proceed.
 * Resembles an X-reply packet.
 */
typedef struct _FileReadDone {
    BYTE opcode;	/* REPLY_READ_FILE_DONE */
    BYTE transaction_unique;
    BYTE Pad1;
    BYTE Pad2;
    CARD32 replyLength B32;/* number of 4-byte words in this reply (2) */
} FileReadDone;

/*
 * Request from the helper to the server in reply to a FileReadRequest
 * Contains the data from the file.
 * Resembles an X-request packet.
 */
typedef struct _FileReadData {
    BYTE opcode;	/* REQ_FILE_DATA */
    BYTE transaction_unique;	/* from previous READ_FILE */
    CARD16 xlength B16;	/* number of 4-byte words in this packet */
    CARD32 error B32;	/* error indicator */
    CARD32 offset B32;	/* offset in file of this reply */
    CARD32 lengthOfData B32;/* number of bytes which follow */
} FileReadData;

#define MAXDELTASIZE 64
#define NDELTAS 16
#define MAXBYTESDIFF 8

/* 
 * A DeltaRec holds a previously transferred byte sequence, and its length.
 */
typedef struct _DeltaRec {
    int length;
    unsigned char buf[MAXDELTASIZE];
} DeltaRec;

/*
 * A two-byte sequence embedded in the delta packet describing
 * byte # and new datum.
 */
struct delta{
  unsigned char byte;
  unsigned char data;
};

/*
 * A whole REQ/REP_DELTA packet has a shortcommand header, and
 * a set of delta pairs.
 */
struct deltacommand{
  struct shortcommand sc;
  struct delta deltas[MAXBYTESDIFF];
};

#define REQ_FILE_DATA		0xFB
#define REQ_DIAL		0xFC
#define REQ_HANGUP		0xFD
#define REQ_CHANGE_CLIENT	0xFE
#define REQ_DELTA		0xFF

#define REPLY_READ_FILE_DONE	0xFA
#define REPLY_READ_FILE		0xFB
#define REPLY_CHANGE_CLIENT	0xFC
#define REPLY_CLOSE_CLIENT	0xFD
#define REPLY_DELTA		0xFE
#define REPLY_HANGUP_REPLY	0xFF

#define XREMOTE_BUFFERSIZE 1024
#define MAXBYTESPERCLIENT (2*MAXDELTASIZE)

#define PACKET_SIZE 120
#define MAX_LZW_PLAIN ((PACKET_SIZE * 9) / 10)
#define cb ((xrcontext *)(stdio->xrcb))

#define XR_TFTP_BUFFERSIZE	70000
#define XR_TFTP_RETRIES		3

typedef struct xrfonthost_ {
    struct xrfonthost_ *next;		/* ptr to next entry */
    ipaddrtype addr;			/* Internet address */
    int bytesread;			/* Bytes read from host */
    int filesread;			/* Files successfully read */
    int neterrors;			/* Number of network errors */
    int fileerrors;			/* Number of file-oriented entries */
} xrfonthost;

#define MAXSOCKS 128

enum LineProtocol {XR_XWIRE, XR_FILE_ACCESS};

struct xrcontext_ {
    tt_soc *tty;
    struct buffer *bufferList;
    struct buffer *lineBuf;
    struct client *clientTable[MAXSOCKS];
    struct client *anyClient;
    struct client *lastRequestClient;
    struct client *firstRequestClient;
    XRemoteClientId selectedClient;
    int requestBytesLeft;
    int replyBytesLeft;
    struct client *grabbingClient;
    DeltaRec ReqDeltas[NDELTAS];
    int activeReqDeltas;
    int nextReqDelta;
    DeltaRec ReplyDeltas[NDELTAS];
    int nextReplyDelta;
    enum LineProtocol lineProtocol;
    int numClients;
    int someClients;
    char *stopReason;
    struct _connection *serialContext;
    char *currentFileName;
    char *currentFileData;
    ulong currentFileDataLen;
    int fileErrorCode;
    FileReadData *fileReplyBuffer;
    BYTE file_transaction_unique;
    unsigned int fileRequestedOffset;
    unsigned int fileReplyLength;
    char *fileRequestedName;
    sys_timestamp idletime;

    /* Context for XDM */

    xdmcp_states xdmState;
    sys_timestamp xdmTimeOutTime;
    int xdmTimeOutRtx;
    ipsocktype *xdmSocket;
    ulong xdmKeepaliveDormancy;
    struct client *xdmSessionClient;
    ulong xdmSessionID;

    /* Context for local client */

    queuetype localReqQ;	/* Pending request queue */
    int localReadThresh;	/* Count we want to read from terminal */
    int localWriteThresh;	/* Space we need to write to terminal */
    boolean localConnected;	/* True if connection established */

    /* Context for LAT session startup */

    char *lat_error;		/* Pointer to error string, if any */

    /* Context for Reverse XRemote */

    boolean reverseMode;	/* TRUE if using reverse XRemote */
    ipaddrtype reverse_ipaddr;	/* X server IP address */
    ushort reverse_display;	/* X server display TCP port number */

    /* Conect for Reverse XRemote input scanner */

    boolean reverse_inescape;	/* TRUE if escape just seen */
    int reverse_scanptr;	/* Count of valid characters seen */
    ushort reverse_crc;		/* CRC calculated so far */

    void *acctinfo;		/* Accounting info */
};

struct halfbuf{
  unsigned char *buf; /* buffer pointer */
  unsigned char *buftop; /* top of buffer */
  int bufsize; /* size of buffer */
  int count; /* number of bytes in buffer */
  unsigned char *head,*tail; /* head and tail pointers for buffer */
};

struct buffer{
  struct halfbuf inbuf,outbuf;
  struct buffer *link; /* link to next buffer */
};

#define BufferSlack(buf) ((buf)->bufsize - (buf)->count)

enum transportType { TRANSPORT_TCP, TRANSPORT_LAT, TRANSPORT_LOCAL } ;

struct client {
  xrcontext *ctx;		/* Pointer back to context block */
  struct buffer *buf;  		/* client's i/o buffer */
  ClientState state; 		/* client's state */
  int bytesSent;		/* fairness considerations/scheduling */
  Bool NeedConnectionInfo;	/* aux state: client instantiated */
  struct client *blink,*flink; 	/* pointers to create ring */
  XRemoteClientId XRemId;	/* the small int for comm with the Xremote server */
  Bool LSBfirst;
  enum transportType transport;	/* Transport to use */
  void *tc;			/* Transport connection pointer */
  Bool newdata;			/* New data may be readable */
  CARD16 seqnum;		/* Current request sequence number */
  Bool spoofreq;		/* TRUE if spoofing a request */
  CARD16 spoofnum;		/* Sequence number we are spoofing */
};

typedef unsigned int Sequence;
#define SequenceWidth 3
#define SequenceDEFN(name) Sequence name : SequenceWidth
#define SequenceGet(var) ((var) & ((1<<SequenceWidth)-1))
#define XMT_WINDOW ((1<<SequenceWidth)-1)
#define RCV_WINDOW ((1<<SequenceWidth)-1)

typedef struct _packetHeader {
#ifdef BITSBIGENDIAN
    unsigned int ExtendedCommand : 1;	/* overlaid */
    unsigned int Compressed  : 1;
#if SequenceWidth > 2
    unsigned int filler : (8*((2+(3*SequenceWidth)+7)/8))-(2+(3*SequenceWidth));
#endif SequenceWidth > 2
    SequenceDEFN(Seq);
    SequenceDEFN(Window);
    SequenceDEFN(Ack);
#else BITSBIGENDIAN
    SequenceDEFN(Ack);
    SequenceDEFN(Window);
    SequenceDEFN(Seq);
#if SequenceWidth > 2
    unsigned int filler : (8*((2+(3*SequenceWidth)+7)/8))-(2+(3*SequenceWidth));
#endif SequenceWidth > 2
    unsigned int Compressed  : 1;
    unsigned int ExtendedCommand : 1;
#endif BITSBIGENDIAN
} PacketHeader;
#define HEADER_SIZE ((1+1+(3*SequenceWidth)+7)/8)

typedef struct {
    unsigned char Version;
    unsigned char ConnectionRandom1;
    unsigned char ConnectionRandom2;
    unsigned char Seq;
    unsigned char Window;
    unsigned char ConnectionHeaderPad;
} ConnectionHeader;


/*
 * Overlay of a PacketHeader for extended commands
 */
typedef struct {
#ifdef BITSBIGENDIAN
    unsigned int ExtendedCommand : 1;
    unsigned int ExtendedPad1 : 7;
#else BITSBIGENDIAN
    unsigned int ExtendedPad1 : 7;
    unsigned int ExtendedCommand : 1;
#endif BITSBIGENDIAN
    unsigned char ExtendedCommandCode;
    unsigned char ExtendedCommandLength;/* including Command, code and length */
    unsigned char ExtendedPad2;
} ExtendedHeader;

typedef unsigned int Counter;

#define bs_link clns_nexthopaddr
#define bs_buf data_area
#define bs_sendTime gp_timer
#define bs_wasRetransmitted unspecified[1]
typedef paktype Packet;

typedef struct _queue {
    Packet * head;
    Packet * tail;
} PacketQueue;

typedef enum {BS_CLOSED, SYNCSENT, SYNCRCVD, BS_CONNECTED} ConnectionState;

typedef struct _connection {
    ConnectionState state;
    PacketQueue rawPackets;
    PacketQueue	incomingPackets;
    PacketQueue	retransmitPackets;
    unsigned char operatingVersion;	/* lowest common version number */
    Bool	ackNeeded;
    sys_timestamp sendTime;		/* time last transmit took place */
    sys_timestamp ackPostTime;
    sys_timestamp windowCloseTime;	/* time at which xmtWin hit 0 */
    unsigned long probeInterval;	/* time till probe is made */
    unsigned long roundTripTime;	/* smoothed turnaround time */
    unsigned long retransInterval;	/* retransmission interval */
    SequenceDEFN(xmtNext);
    SequenceDEFN(xmtWin);
    SequenceDEFN(xmtMax);
    SequenceDEFN(xmtUnacked);
    SequenceDEFN(rcvNext);
    SequenceDEFN(rcvWin);
    SequenceDEFN(rcvUnacked);
    Counter	xmtRetransmissions;
    Counter	xmtPackets;
    Counter	xmtErrors;
    Counter	rcvPackets;
    Counter	rcvErrors;
    Counter	rcvOutOfSequence;
    ConnectionHeader xmtConnectionData;
    ConnectionHeader rcvConnectionData;
    char	*compression;
    int		lastTotal;
    tt_soc	*tty;
    Bool	reverseMode;
} SerialConnection;

extern boolean xremote_enable;

/*
 * Function prototypes
 */

extern xrcontext *xremote_new_context(tt_soc *, boolean);
extern struct client *xremote_connect(tt_soc *);
extern struct client *xremote_create_client(xrcontext *, int);
extern struct buffer *xremote_io_buffer(int, int);
extern void xremote_reset(xrcontext *, char *);
extern void xremote_shutdown(tt_soc *);
extern void xremote_show_tcp(tt_soc *);
extern boolean xremote_tcp_connect(tcbtype *, tt_soc *);
extern boolean xremote_tcp_check_access(tt_soc *, ipaddrtype, ulong, ulong);
extern int SerialWrite(struct buffer *);
extern int BufferMove(int, struct halfbuf *, struct halfbuf *);
extern int ClientGetData(struct client *, unsigned char **, int);
extern int BufferStuff(int, void *, struct halfbuf *);
extern char *client_state(struct client *);
extern struct client *NextClient(struct client *);
extern void DoIO(void);
extern boolean xremoteBLOCK(tt_soc *);
extern void CloseClient(xrcontext *, struct client *);
extern int BufferPreview(int, struct halfbuf *, void *);
extern int BufferYank(int, struct halfbuf *, void *);
extern int HostUnswapWord(struct client *, int);
extern int HostUnswapLong(struct client *, int);
extern void ClientRead(struct client *, int);
extern int pad(int);
extern int BytesDiff(unsigned char *, unsigned char *, int);

extern SerialConnection *BSInit(tt_soc *, boolean);
extern Bool BSStartup(SerialConnection *);
extern int BSPollWrite(SerialConnection *);
extern int BSWrite(SerialConnection *, unsigned char *, int);
extern void BSWakeup(SerialConnection *);
extern void BSShutdown(SerialConnection *);
extern void BSFree(SerialConnection *);
extern int BSPollRead(SerialConnection *);
extern boolean BSBlock(SerialConnection *);
extern int BSRead(SerialConnection *, unsigned char *, int);
extern void XremoteStatsDisplay(SerialConnection *);

extern void xdm_shutdown(xrcontext *);
extern boolean xdm_init(xrcontext *, ipaddrtype, xdmcp_states);
extern void xdm_wakeup(xrcontext *);
extern void xdm_tcp_close(xrcontext *, struct client *);
extern void xdm_tcp_open(xrcontext *, struct client *);

extern boolean create_xremote_tty_process(tt_soc *);

extern void xrlatfont_show_server(void);
extern int xremote_clients(tt_soc *);

extern int xr_font_memerr;
extern int xr_tftp_buffersize;

extern struct delta tempDeltaBuf[MAXBYTESDIFF];
extern unsigned char tempReqBuf[MAXDELTASIZE];
extern unsigned char tempReplyBuf[MAXDELTASIZE];
extern boolean deltaEnable;

extern void xremote_init(subsystype *);
extern void xremote_parser_init(void);
extern void xremote_lat_init(void);

#define X_PROTOCOL 11
#define X_PROTOCOL_REVISION 0

/*
 * xremote_setup.c
 */
extern void xremote_setup_global(setup_struct *);

#endif __XREMOTE_H__
