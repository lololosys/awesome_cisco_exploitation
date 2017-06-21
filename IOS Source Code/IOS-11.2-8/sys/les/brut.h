/* $Id: brut.h,v 3.2.62.2 1996/06/16 21:13:58 vnguyen Exp $
 * $Source: /release/112/cvs/Xsys/les/brut.h,v $
 *------------------------------------------------------------------
 * Header file for Brut Aux line driver
 *
 * 1993, Lele Nardin
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: brut.h,v $
 * Revision 3.2.62.2  1996/06/16  21:13:58  vnguyen
 * Finally committing Brasil to California_branch
 *
 * Revision 3.2.62.1  1996/03/18  20:40:18  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.1  1996/01/24  22:22:00  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.2  1995/11/17  17:36:29  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:29:24  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:30:27  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * trace table for debug command
 */
typedef struct {
    uchar code;
    uchar isr;
    ushort status;
    int c;
} trace_table;

/*
 * External declarations:
 */
extern tt_soc *bruttty;

#if defined(PAN) | defined(BRASIL) & !defined(XX) & !defined(SIERRA)
#define IS_BRUT(cookie)	    ( cookie->processor == PROCESSOR_BRUT )
#else
#define IS_BRUT(cookie)	    FALSE
#endif

#define TRACE_TABLE_MAX     2000

/*
 * Back Plane Protocol
 */

#define REQ_START_MSG                   '{'
#define RESP_START_MSG                  '}'
#define END_MSG1                         '\r'
#define END_MSG2                         '\n'
/*
 * command codes
 */

#define CMD_CD_WHO                      '?'
/*
 * Who are you ; BRUT will answer this query in slave mode
 */

#define CMD_CD_RESET                    '='
/*
 * Reset unit
 */

#define CMD_CD_STATUS                   'S'
/*
 * Query Status of port
 */

#define CMD_CD_DISC                     'D'
/*
 * Disconnect port & report status
 */

#define CMD_CD_CONN                     'C'
/*
 * Connect port & report status
 */

#define CMD_CD_LOCATE                   'L'
/*
 * Locate port for node address
 */

#define CMD_CD_STAT_LOC                 'Y'
/*
 * Report Status of locate
 */

#define CMD_CD_STAT_MASTER              'P'
/*
 * Master presnt sequence message for arbitration
 */
#define CMD_CD_QUIET              	'Q'
/*
 * Master contention message for arbitration
 */
/*
 * Message Structure
 */

/*
 * Who are you
 */
#define REQMSGSTRUCTW 		struct ReqMsgStructW_
#define RESPMSGSTRUCTW1		struct RespMsgStructW1_
#define RESPMSGSTRUCTW2 	struct RespMsgStructW2_


REQMSGSTRUCTW
{
    uchar           ReqStartMsg;
    uchar           SlotNumber;
    uchar           CmdCode;
    uchar           endmsg1;
    uchar           endmsg2;
};

RESPMSGSTRUCTW1
{
    uchar           RespStartMsg;
    uchar           SlotNumber;
    uchar           CmdCode;
    uchar           RepType;
    uchar           Revision;
    uchar           NumPorts;
    uchar           endmsg1;
    uchar           endmsg2;

};

RESPMSGSTRUCTW2
{
    uchar           RespStartMsg;
    uchar           SlotNumber;
    uchar           CmdCode;
    uchar           RepType;	/* the type code has to be resolved */
    uchar           stationAdd[12];	/* station address */
    uchar           endmsg1;
    uchar           endmsg2;
};

/*
 * Reset unit
 */
#define REQMSGSTRUCTR 		struct ReqMsgStructR_
REQMSGSTRUCTR
{
    uchar           ReqStartMsg;
    uchar           SlotNumber;
    uchar           CmdCode;
    uchar           endmsg1;
    uchar           endmsg2;
};

/*
 * Status of port
 */
#define REQMSGSTRUCTS 		struct ReqMsgStructS_
#define RESPMSGSTRUCTS 		struct RespMsgStructS_
REQMSGSTRUCTS
{
    uchar           ReqStartMsg;
    uchar           SlotNumber;
    uchar           CmdCode;
    uchar           PortNumber;
    uchar           endmsg1;
    uchar           endmsg2;
};

RESPMSGSTRUCTS
{
    uchar           RespStartMsg;
    uchar           SlotNumber;
    uchar           CmdCode;
    uchar           PortNumber;
    uchar           PartCount;
    uchar           PartStatus;
    uchar           endmsg1;
    uchar           endmsg2;
};

/*
 * Disconnect port & report status
 * Connect port & report status
 * Locate port for node address
 *     All the above three commanad req and resp
 *     follow the structure of query status of port
 */

#define REQMSGSTRUCTY 		struct ReqMsgStructY_

/*
 * Report Status of locate
 */
REQMSGSTRUCTY
{
    uchar           ReqStartMsg;
    uchar           SlotNumber;
    uchar           CmdCode;
    uchar           endmsg1;
    uchar           endmsg2;
};

#define RESPMSGSTRUCTY 		struct RespMsgStructY_

RESPMSGSTRUCTY
{
    uchar           RespStartMsg;
    uchar           SlotNumber;
    uchar           CmdCode;
    uchar           PortNumber;
    uchar           MacAddr[12];
    uchar           endmsg1;
    uchar           endmsg2;
};

/*
 * If Port Number = "?"
 * or MacAddr[0]  = "?"  Data invalid
 */

/*
 * Master present sequence message
 */

#define REQMSGSTRUCTP 		struct ReqMsgStructP_

REQMSGSTRUCTP
{
    uchar           ReqStartMsg;
    uchar           SlotNumber;
    uchar           CmdCode;
    uchar           RcvHub;
    uchar           BrutSlot;
    uchar           BrutMac[12];
    uchar           endmsg1;
    uchar           endmsg2;
};
#define RESPMSGSTRUCTP 		struct RespMsgStructP_

RESPMSGSTRUCTP
{
    uchar           RespStartMsg;
    uchar           SlotNumber;
    uchar           CmdCode;
    uchar           RcvHub;
    uchar           BrutSlot;
    uchar           BrutMac[12];
    uchar           endmsg1;
    uchar           endmsg2;
};

/*
 * There is no response to this master present message
 */
#define RESPMSGSTRUCTQ struct RespMsgStructQ_ 

RESPMSGSTRUCTQ {
    uchar           RespStartMsg;
    uchar           SlotNumber;
    uchar           CmdCode;
    uchar           endmsg1;
    uchar           endmsg2;
};

#define TXMSGENTRY 		struct txmsgentry_

TXMSGENTRY
{
    TXMSGENTRY     *next;
    uchar           TxState;
    uchar           hubid;
/*
 * keep the above 3 members in sync with RXMSGENTRY
 */
    union {
	REQMSGSTRUCTW   ReqMsgStructW;
	REQMSGSTRUCTS   ReqMsgStructS;
	REQMSGSTRUCTY   ReqMsgStructY;
	REQMSGSTRUCTP   ReqMsgStructP;
    }               req_msg;
};

#define RXMSGENTRY 		struct rxmsgentry_

RXMSGENTRY
{
    RXMSGENTRY     *next;
    uchar           RxState;
    uchar           hubid;
/*
 * keep the above 3 members in sync with TXMSGENTRY
 */
    union {
	RESPMSGSTRUCTY  RespMsgStructY;
	RESPMSGSTRUCTW1 RespMsgStructW1;
	RESPMSGSTRUCTW2 RespMsgStructW2;
	RESPMSGSTRUCTS  RespMsgStructS;
	RESPMSGSTRUCTQ  RespMsgStructQ;
	RESPMSGSTRUCTP  RespMsgStructP;
    }               resp_msg;
};


#define BRUTMSGSTRUCT   struct brutmsgstruct_

BRUTMSGSTRUCT
{
  union {
	RESPMSGSTRUCTY  RespMsgStructY;
	RESPMSGSTRUCTW1 RespMsgStructW1;
	RESPMSGSTRUCTW2 RespMsgStructW2;
	RESPMSGSTRUCTS  RespMsgStructS;
	RESPMSGSTRUCTQ  RespMsgStructQ;
	RESPMSGSTRUCTP  RespMsgStructP;

	REQMSGSTRUCTW   ReqMsgStructW;
	REQMSGSTRUCTS   ReqMsgStructS;
	REQMSGSTRUCTY   ReqMsgStructY;
	REQMSGSTRUCTP   ReqMsgStructP;
  }brut_msgstruct;
};


/*
 * as defined in TXMSGENTRY hubid
 */
/* same for req and resp */

#define RQ_start	           req_msg.ReqMsgStructW.ReqStartMsg
#define RQ_cmd_cd               req_msg.ReqMsgStructW.CmdCode
#define RQ_slot_no              req_msg.ReqMsgStructW.SlotNumber

#define RS_start        	   resp_msg.RespMsgStructW1.RespStartMsg
#define RS_cmd_cd               resp_msg.RespMsgStructW1.CmdCode
#define RS_slot_no              resp_msg.RespMsgStructW1.SlotNumber


#define BRUTGLOBENTRY  		struct BrutGlobEntry_

BRUTGLOBENTRY
{
    uchar           GS_brut_slot;
    uchar           GS_brut_mac[6];
    uchar           GS_BRUT_driver_state;
};

#define    brut_slot      	brutglobentry.GS_brut_slot
#define    brut_mac      	brutglobentry.GS_brut_mac
#define	   brut_driver_state 	brutglobentry.GS_BRUT_driver_state

/* brut driver_state definitions */
#define    BRUT_DRIVER_STATE_NORMAL         1
#define    BRUT_DRIVER_STATE_RX_START       2
#define    BRUT_DRIVER_STATE_RX_MY_SLOT     3
#define    BRUT_DRIVER_STATE_RX_WHO_ARE_YOU 4
#define    BRUT_DRIVER_STATE_RESPONDING     5

#define BRUT_ID                         'T'

/*
 * Prototypes
 */
extern void brut_parser_init(void);
