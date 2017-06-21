/* $Id: qa_hw.h,v 3.2 1995/11/17 18:46:51 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/src-rsp/qa_hw.h,v $
 *------------------------------------------------------------------
 * Queue ASIC for IRSP
 *
 * June 1994 Carl Schaefer
 *
 * Copyright (c) 1994-1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: qa_hw.h,v $
 * Revision 3.2  1995/11/17  18:46:51  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:27:16  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:57:08  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __RSP_QA_HW__
#define __RSP_QA_HW__

typedef volatile ulong	qa_hwq_read_reg_t;



typedef struct qa_hwq_acc_t_ {
    qa_hwq_reg_t	hwq_prim;	/* Primary Hardware Queue Address */
    qa_acc_reg_t	acc_prim;	/* Primary Accumulator Address */
    qa_hwq_reg_t	hwq_alt;	/* Alternate Hardware Queue Address */
    qa_acc_reg_t	acc_alt;	/* Alternate Accumulator Address */
} qa_hwq_acc_t;



/*
 * Structure defining entire primary/secondary QA ASIC register space
 * for the IRSP/1, Bobbie, and for the one processor case for the IRSP/2,
 * Bob.
 *
 * The two IRSP/2 case in the Tokamac chassis unfortunately gets a
 * different register map. Half the qstatus bits at the start of the
 * QA ASIC, and the other half start at the 4 KB midpoint.
 */

typedef union qa_asic_t_ {
    qa_qstatus_reg_t	qstatus[QA_STATUS_WORDS_RSP];	/* Queue status bits */
    qa_hwq_acc_t	qa[QA_NQUEUES];			/* Queues and ACCs*/
} qa_asic_t;


extern qa_asic_t	*qasic_base;

/* constants to specify level for RSP interrupts */
#define QA_ATTN_CMD		0x10	/* qa attn value for RP cmd level */
#define QA_ATTN_NET		0x00	/* qa attn value for RP net level */

extern void	set_qa_ip_attention(int qnum, int slot);
extern void	set_qa_rp_attention(int qnum, ulong attention_level);

#define QA_ATTN_NONE		0	/* clear attn on a queue */

extern void	qa_write_attn(int qnum, ulong attention_value);


#endif /* __RSP_QASIC_H__ */
