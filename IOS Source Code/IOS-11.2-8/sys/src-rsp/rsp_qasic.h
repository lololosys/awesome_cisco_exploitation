/* $Id: rsp_qasic.h,v 3.3.4.2 1996/07/08 22:52:54 mwu Exp $
 * $Source: /release/112/cvs/Xsys/src-rsp/rsp_qasic.h,v $
 *------------------------------------------------------------------
 * Queue ASIC for IRSP
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: rsp_qasic.h,v $
 * Revision 3.3.4.2  1996/07/08  22:52:54  mwu
 * CSCdi60922:  11.2 Core Features Commit
 * Branch: California_branch
 *
 * Revision 3.3.4.1  1996/04/06  08:43:56  ondiaye
 * CSCdi49252:  VIPER needs 16 tx queues in one status word: This is fixed
 *              by breaking the QAasic into two pools of txq and
 *              accumulators
 *              (poo1 and pool2). VIP2 txq's get allocated from pool2.
 *              VIP2
 *              txq's need to be continuous and their accumulators must at
 *              2 byte offset from their queues.
 *              This is temporary in a sense that it won't work well for
 *              the
 *              master/slave rsp model. Marty is working on that version.
 * Branch: California_branch
 *
 * Revision 3.3  1996/03/11  11:09:54  ogrady
 * CSCdi51224:  RSP emulation code is obsolete
 *              Expunge code.
 *
 * Revision 3.2  1995/11/17  18:49:17  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:28:34  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:28:41  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __RSP_QASIC_H__
#define __RSP_QASIC_H__

#define	QA_DIAGSIZE		(sizeof(qa_diag_t))
#define	QA_QSIZE		8		/* byte offset btwn qs */
#define	QA_ACCSIZE		8		/* byte offset btwn accs */
#define	QA_NQ_ELEMENTS		4096

#define	DIAG_ERROR_LOG_BASE	0x8000		/* byte offset in diag space */
#define	DIAG_MEMD_BASE_REG	0x8018		/* byte offset in diag space */
#define	QA_LSA_BIT		(1<<30)

#define ACC_LOWBITS	0x0002
#define ACC_LOWMASK	0x0007

#define	qaptr2boffset(_p)	((ulong)((uchar *)(_p) - (uchar *)qasic_base))
#define	QNUM(q_ptr)		(qaptr2boffset(q_ptr)/QA_QSIZE)
#define	ANUM(a_ptr)	((qaptr2boffset(a_ptr)-QA_ACC_OFFSET-ACC_LOWBITS) / \
				QA_ACCSIZE)




#ifdef RSP_EMULATION

#define QA_BASE_OFFSET		0x40		/* start of qa byte offset */
#define	QA_Q_OFFSET		QA_BASE_OFFSET	/* start of qs byte offset */
#define	QA_QEND_OFFSET		0x300		/* end of qs byte offset */
#define	QA_ACC_OFFSET		QA_QEND_OFFSET	/* start of accs byte off */
#define	QA_ACCEND_OFFSET	0x400		/* end of accs byte off */
#define	QA_END_OFFSET		0x400		/* end of qa byte off */

#define	QA_NQUEUES		((QA_QEND_OFFSET-QA_Q_OFFSET)/QA_QSIZE)
#define	QA_NACCS		((QA_ACCEND_OFFSET-QA_ACC_OFFSET)/QA_ACCSIZE)

#define	QA_FIRST_QNUM		(QA_Q_OFFSET/QA_QSIZE)
#define	QA_QNUM_LIMIT		(QA_FIRST_QNUM+QA_NQUEUES)
#define QA_LAST_QNUM		(QA_QNUM_LIMIT-1)

#define	QA_FIRST_ACCNUM		QA_FIRST_QNUM /* arbitrary */
#define	QA_ACCNUM_LIMIT		QA_NACCS
#define	QA_LAST_ACCNUM		(QA_ACCNUM_LIMIT-1)
/* ondiaye starts */
#define POOL2_NUM_ACC           192           /* 17 (svip 16 txq + 1 ipc_q) * 11 (slots) 
						 rounded to a multiple of 16 */
#define POOL2_FIRST_ACCNUM      (QA_ACCNUM_LIMIT - POOL2_NUM_ACC)
#define POOL2_FIRST_TXQNUM      POOL2_FIRST_ACCNUM
#define POOL2_LAST_ACCNUM       (POOL2_FIRST_ACCNUM + POOL2_NUM_ACC - 1)
#define POOL2_LAST_TXQNUM       POOL2_LAST_ACCNUM 
#define POOL1_NUM_ACC           (QA_NACCS - POOL2_NUM_ACC) /* the rest */
#define POOL1_FIRST_ACCNUM      QA_FIRST_ACCNUM        
#define POOL1_LAST_ACCNUM       (POOL2_FIRST_ACCNUM - 1)
#define POOL1_LAST_TXQNUM       POOL1_LAST_ACCNUM
/* ondiaye ends */
					/* # of 16 bit Q status words */
#define QA_STATUS_WORDS_RSP	(QA_Q_OFFSET/sizeof(ushort))


#else /* not RSP_EMULATION */



/*
 * This is a pain, because the single RSP case is different
 * from the dual RSP case.
 *
 * In the RSP case, the QA ASIC address space starts with 64 16 bit words
 * of queue status. The rest of the QA address space is filled with queues:
 */
#define QA_STATUS_WORDS_RSP	64	/* # of 16 bit Q status words */

#define QA_ACC_OFFSET		0


#define QA_NQUEUES              (QA_QNUM_LIMIT - QA_FIRST_QNUM)
#define QA_NACCS                (QA_ACCNUM_LIMIT - QA_FIRST_ACCNUM)

#define	QA_FIRST_QNUM		(QA_STATUS_WORDS_RSP / \
				 btow(sizeof(qa_hwq_acc_t)))
#define QA_LAST_QNUM		1023
#define QA_QNUM_LIMIT           (QA_LAST_QNUM+1)

#define	QA_FIRST_ACCNUM		(QA_STATUS_WORDS_RSP / \
				 btow(sizeof(qa_hwq_acc_t)))
#define	QA_LAST_ACCNUM		1023
#define QA_ACCNUM_LIMIT         (QA_LAST_ACCNUM+1)
/* ondiaye starts */
#define POOL2_NUM_ACC           192           /* 17 (svip 16 txq + 1 ipc_q) * 11 (slots) 
						 rounded to a multiple of 16 */
#define POOL2_FIRST_ACCNUM      (QA_ACCNUM_LIMIT - POOL2_NUM_ACC)
#define POOL2_FIRST_TXQNUM      POOL2_FIRST_ACCNUM
#define POOL2_LAST_ACCNUM       (POOL2_FIRST_ACCNUM + POOL2_NUM_ACC - 1)
#define POOL2_LAST_TXQNUM       POOL2_LAST_ACCNUM 
#define POOL1_NUM_ACC           (QA_NACCS - POOL2_NUM_ACC) /* the rest */
#define POOL1_FIRST_ACCNUM      QA_FIRST_ACCNUM        
#define POOL1_LAST_ACCNUM       (POOL2_FIRST_ACCNUM - 1)
#define POOL1_LAST_TXQNUM       POOL1_LAST_ACCNUM
/* ondiaye ends */

#endif /* RSP_EMULATION */





/*
 * QA ASIC register types:
 */

typedef volatile ushort	qa_hwq_reg_t;
typedef volatile short	qa_acc_reg_t;
typedef volatile ushort qa_qstatus_reg_t; /* Queue status bits read 16 bits */

struct qa_hwq_t_ {
    qa_hwq_reg_t	primary;	/* Primary Hardware Queue reg */
    qa_acc_reg_t	DO_NOT_USE;	/* Accumulator -- do not use */
    qa_hwq_reg_t	alternate;	/* Secondary Hardware Queue reg */
};

struct qa_acc_t_ {
    qa_acc_reg_t	primary;	/* Primary Accumulator reg */
    qa_hwq_reg_t	DO_NOT_USE;	/* Queue -- do not use*/
    qa_acc_reg_t	alternate;	/* Alternate Accumulator reg */
};

/* These definitions are for error0_3 */
#define QA_DIAG_INPUT_PARITY_ERROR	0x40
#define QA_DIAG_REUSE0_LINK_ERROR	0x20
#define QA_DIAG_MEM_PARITY_ERROR	0x10
#define QA_DIAG_ALT_QWRITE_ERROR	0x08
#define QA_DIAG_BAD_ADD_ERROR   	0x04


#define QA_DIAG_SELECT_MASK             0x03	
#define QA_DIAG_ADDRESS_MASK            0xFFFE
#define QA_DIAG_DATA_PARITY_MASK        0xFF

#define QA_DIAG_NQUEUES		1024	/* != QA_NQUEUES on emulation */
#define QA_DIAG_NACCS		1024	/* != QA_NQUEUES on emulation */

/* These definitions are for error4_7 after a QA_DIAG_REUSE0_LINK_ERROR */
#define QA_DIAG_BUF_MASK		0xFFFF
#define QA_DIAG_BUF_SHIFT		16



/*
 * Structures defining the entire diagnostic address space for the QA ASIC:
 * This area has to be read or written by 32 bit writes and reads on 64 bit
 * boundaries!
 */

typedef struct qa_diag_data_t_ {
    volatile ulong	data;
    ulong	:32;
} qa_diag_data_t;


typedef struct qa_diag_t_ {
    union {
	qa_diag_data_t	body[QA_NQ_ELEMENTS];
	struct {
	    qa_diag_data_t head[QA_DIAG_NQUEUES];
	    qa_diag_data_t tail[QA_DIAG_NQUEUES];
	    qa_diag_data_t stat[QA_DIAG_NQUEUES];
	    qa_diag_data_t acc[QA_DIAG_NACCS];
	} qd_data;
    } qd_u;

    volatile ulong error0_3;
    ulong :32;
    volatile ulong error4_7;
    ulong :32;
    volatile ulong error8_11;
    ulong :32;
    volatile ulong base_addr;
    ulong :32;
    volatile ulong control;
    ulong :32;
} qa_diag_t;


extern ulong read_qa_diag(volatile ulong *qa_diag_ptr);
extern ulong read_qa_diag_unprotected(volatile ulong *qa_diag_ptr);
extern void write_qa_diag(volatile ulong *qa_diag_ptr, ulong value);
extern void write_qa_diag_unprotected(volatile ulong *qa_diag_ptr, ulong value);

extern qa_diag_t	*qasic_diag_base;
extern qa_diag_t	qasic_diag_copy;

/*
 * macros to read qa diag array
 */

#define	QA_READ_HEAD(qnum, read_qa_fun, qd_base)	\
    ((ushort)				\
     ( (read_qa_fun((ulong *)&((qd_base)->qd_u.qd_data.head[qnum].data))) \
       & 0xffff))

#define	QA_READ_TAIL(qnum, read_qa_fun, qd_base)		\
    ((ushort)				\
    ((read_qa_fun((ulong *)&((qd_base)->qd_u.qd_data.tail[qnum].data))) \
     & 0xffff))

/*
 * returns body value (w/o lsa bit) shifted to be a buffer header byte offset
 */
#define	QA_READ_BODY(bufhdr_off, read_qa_fun, qd_base)	\
    ((ushort)				\
    (((read_qa_fun((ulong *)&((qd_base)->qd_u.body[(bufhdr_off)>>4].data))) \
     >> 12) & 0xfff0))

#define	QA_READ_LSA(bufhdr_off, read_qa_fun, qd_base)			\
    (((read_qa_fun((ulong *)&((qd_base)->qd_u.body[(bufhdr_off)>>4].data))) \
     & QA_LSA_BIT) ? 1 : 0)

#define	QA_READ_STATUS(qnum, read_qa_fun, qd_base)			  \
    (((read_qa_fun((ulong *)&((qd_base)->qd_u.qd_data.stat[qnum].data))) \
     & 0x8000) ? 1 : 0)

#define	QA_READ_ATTN(qnum, read_qa_fun, qd_base)			\
    ((read_qa_fun((ulong *)&((qd_base)->qd_u.qd_data.stat[qnum].data))) & 0xff)


#define	QA_READ_DIAG_ACC(_acc, read_qa_fun, qd_base) 		((short) \
    ((read_qa_fun((ulong*) &((qd_base)->qd_u.qd_data.acc[_acc])))&0xffff))


/*
 * inlines to get from hw qa diag space
 */
static inline ushort qa_read_head(ushort qnum)
{
	return(QA_READ_HEAD(qnum, read_qa_diag, qasic_diag_base));
}

static inline ushort qa_read_tail(ushort qnum)
{
	return(QA_READ_TAIL(qnum, read_qa_diag, qasic_diag_base));
}

static inline ushort qa_read_body(ushort bufhdr_off)
{
	return(QA_READ_BODY(bufhdr_off, read_qa_diag, qasic_diag_base));
}

static inline ushort qa_read_lsa(ushort bufhdr_off)
{
	return(QA_READ_LSA(bufhdr_off, read_qa_diag, qasic_diag_base));
}

static inline ushort qa_read_status(ushort qnum)
{
	return(QA_READ_STATUS(qnum, read_qa_diag, qasic_diag_base));
}

static inline ushort qa_read_attn(ushort qnum)
{
	return(QA_READ_ATTN(qnum, read_qa_diag, qasic_diag_base));
}

static inline ushort qa_read_diag_acc(ushort acc)
{
	return(QA_READ_DIAG_ACC(acc, read_qa_diag, qasic_diag_base));
}



#define	QA_CLEAR_ERROR()	write_qa_diag(&qasic_diag_base->control,1)

#define QA_ALLOC_ERROR	(-1)

/* Prototypes for hw queue and hw accumulator access routines */

typedef enum {
    QT_CMD,			/* command queue */
    QT_LFREE,			/* local free queue */
    QT_GFREE,			/* global free queue */
    QT_TX,			/* transmit queue */
    QT_RAW,			/* raw queue (from IPs) */
    QT_RETURN,			/* return queue (for multicast packets) */
    QT_EVENT,			/* love missives */
} queue_t;

extern void	qa_mgr_init(void);
extern int	get_hwq(queue_t, int);
extern qa_acc_t	*get_hwacc(void);
extern void	ret_hwacc(qa_acc_t *);
extern int      reserve_txhwq(int);

#define SLOT2CMDQ(slotnum)	(16+(slotnum))

typedef struct {
    uchar cbus0;
    uchar cbus1;
    uchar memd_rdbypass;
} rsp_cbus_state_t;


extern qa_hwq_t *queue_ptr(int q_num);
extern qa_acc_t *acc_ptr(int acc_num);

extern bufhdr_t *hwq_dequeue(qa_hwq_t *q_ptr);
extern bufhdr_t *hwq_peek(qa_hwq_t *q_ptr);
extern void hwq_enqueue(qa_hwq_t *q_ptr, bufhdr_t *b);

extern ushort read_ccb_done(struct ccbtype_ *ccb);
extern short hwacc_read_dec(qa_acc_t *a_ptr);
extern short hwacc_peek(qa_acc_t *a_ptr);
extern void hwacc_sub(qa_acc_t *a_ptr, short value);
extern void hwacc_set(qa_acc_t *a_ptr, short value);

extern void rsp_qa_init(void);
extern void rsp_snap_qadiag(void);
extern void rsp_save_qadiag(qa_diag_t *qasic_diag_copy);
extern qa_diag_t qasic_diag_copy;

extern rsp_cbus_state_t rsp_disable_cbuses(void);
extern void rsp_restore_cbuses(rsp_cbus_state_t old);

extern int is_qnum(ulong q_num);
extern int is_qptr(qa_hwq_t *q_ptr);
extern int is_accnum(ulong acc_num);
extern int is_accptr(qa_acc_t *acc_ptr);
extern int is_diag_off(ulong qa_diag_offset);
extern int is_memd_off(ulong memd_off);
extern int is_bufhdr_off(ulong qvalue);
extern int is_bufhdr(bufhdr_t *b);

extern int get_acc_number(qa_acc_t *a_ptr);
extern int get_queue_number(qa_hwq_t *q_ptr);



#endif /* __RSP_QASIC_H__ */
