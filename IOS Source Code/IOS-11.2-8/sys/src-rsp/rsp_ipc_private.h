/* $Id: rsp_ipc_private.h,v 3.2.6.1 1996/08/15 15:32:11 skufer Exp $
 * $Source: /release/112/cvs/Xsys/src-rsp/rsp_ipc_private.h,v $
 *------------------------------------------------------------------
 * IPC RSP Common CBus driver constants and structures
 *
 * June 1995, Yan Ke
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: rsp_ipc_private.h,v $
 * Revision 3.2.6.1  1996/08/15  15:32:11  skufer
 * CSCdi60891:  HSA needs to work with SVIP and RVIP
 * Branch: California_branch
 *         HSA (fail-over dual RSP) now works with VIP.
 *
 * Revision 3.2  1996/03/05  04:01:52  kao
 * CSCdi50679:  IPC fast message is broadcasted to all IPC-capable boards
 *
 * Revision 3.1  1995/11/09  13:28:19  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/09/17  20:55:37  motto
 * CSCdi40168:  IPC does not work with a CIP in a RSP (c75xx) router
 *
 * Revision 2.1  1995/08/04  01:44:56  yanke
 * Initial commit of placeholder
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Init command code value.
 */
#define	IPC_CBUS_INIT_CMD_CODE		0x80

/*
 * Name of the master init port
 */
#define	IPC_CBUS_INIT_PORT_NAME		"IPC Master:Init"

/*
 * Max number of ipc messages dequeued per poll.
 */
#define NETINT_IPC_MSG_PER_POLL		16

/*
 * Maximum name length
 */
#define	IPC_CARD_NAME_LEN		32

/*
 * Bits in control field in card structure.
 */
#define IPC_CARD_PRESENT        0x1     /* ipc card detected */
#define IPC_CARD_REGISTERED     0X2     /* ipc card registered */
#define IPC_CARD_PERMANENT_PORT 0X4     /* ipc card supports permanent port */

/*
 * Per card structure.
 */
typedef struct ipc_card_ {
    ulong control;			/* platform dependent flag */
    ipc_seat seat_id;			/* assigned seat id */
    qa_hwq_t *hwq;			/* associated h/w queue */
    int slot_num;			/* slot number of the IPC card */
    char name[IPC_CARD_NAME_LEN + 1];	/* the name of the ipc card */
} ipc_card_t;

/*
 * Init Command Block.
 */
typedef struct init_cmd_block_ {
    ulong slave_seat_id;		/* slave seat id */
    ulong master_ctrl_port_id;		/* master control port id */
    ushort master_rcv_hwq;		/* master receive h/w queue */
    ushort slave_rcv_hwq;		/* slave receive h/w queue */
    ushort ipc_free_hwq;		/* ipc free buffer queue */
} init_cmd_block_t;

/*
 * IPC registration message.
 */
typedef struct ipc_cbus_reg_msg_ {
    ipc_seat seat_id;
} ipc_cbus_reg_msg_t;

/*
 * Inline functions
 */
static inline ipc_seat ipc_make_class_a_seatid (int seat_num)
{
    return ((seat_num << 10) & IPC_CLASS_A_SEAT_MASK);
}

static inline ipc_seat ipc_make_class_b_seatid (int seat_num)
{
    return ((seat_num << 16) & IPC_CLASS_B_SEAT_MASK);
}

extern boolean rsp_ipc_debug;

/*
 * Function Prototypes.
 */
extern boolean ipc_cbus_transmit(ipc_message *, ipc_seat_data *);
extern void ipc_cbus_poll(qa_hwq_t *, queuetype *);
extern paktype *ipc_cbus_get_pak(qa_hwq_t *);
extern bufhdr_t *alloc_ipc_memd_buffer(ulong);
extern ipc_card_t *get_own_transport(void);
extern void ipc_print_message_header(ipc_message *, char *);
