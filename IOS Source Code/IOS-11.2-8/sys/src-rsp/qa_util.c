/* $Id: qa_util.c,v 3.2 1995/11/17 18:46:56 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/src-rsp/qa_util.c,v $
 *------------------------------------------------------------------
 * Queue/Accumulator utility routines
 *
 * September 1994, Bob O'Grady
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: qa_util.c,v $
 * Revision 3.2  1995/11/17  18:46:56  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:27:19  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:57:11  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "rsp_bufhdr.h"
#include "rsp_qasic.h"
#include "qa_hw.h"

/********************************************************************/

int	is_qnum (ulong q_num)
{
	return (q_num < QA_QNUM_LIMIT  &&  q_num >= QA_FIRST_QNUM);
}

/********************************************************************/

int	is_qptr (qa_hwq_t *q_ptr)
{
	ulong qo;

	qo = qaptr2boffset(q_ptr);
	return ((qo&7) == 0  &&  qo < QA_QNUM_LIMIT*QA_QSIZE &&
		   qo >= QA_FIRST_QNUM*QA_QSIZE );
}

/********************************************************************/

int	is_bufhdr_off (ulong qvalue)
{
		/*
		 * allow ccbs which are > 
		 *  MEMD_BUFHDR_OFFSET + (bufhdr_count*sizeof(bufhdr_t))
		 */
	return ((qvalue&0xf) == 0 && qvalue >= MEMD_BUFHDR_OFFSET &&
	                qvalue < MEMD_BUFDATA_OFFSET);
}

/********************************************************************/

int	is_accptr (qa_acc_t *acc_ptr)
{
	ulong ao, qa_acc_offset;

	ao = qaptr2boffset(acc_ptr);
	qa_acc_offset = QA_ACC_OFFSET;
	return ((ao&ACC_LOWMASK) == ACC_LOWBITS  && ao >= qa_acc_offset &&
		ao < (QA_NACCS*QA_ACCSIZE)+QA_ACC_OFFSET);
}

/********************************************************************/

int	is_accnum (ulong acc_num)
{
	return (acc_num < QA_ACCNUM_LIMIT && acc_num >= QA_FIRST_ACCNUM);
}

/********************************************************************/
/*
 * note: return -1 if ptr is invalid
 */
int get_acc_number (qa_acc_t *a_ptr)
{
	if(is_accptr(a_ptr))
		return(ANUM(a_ptr)); 
	return(-1);
}

/********************************************************************/
/*
 * note: return -1 if ptr is invalid
 */
int get_queue_number (qa_hwq_t *q_ptr)
{
	if(is_qptr(q_ptr))
		return(QNUM(q_ptr)); 
	return(-1);
}

/****************************************************************************/

/*
 * QA attention routines
 */

#define QA_ATTN_MASK		0xff	/* qa attention field mask */
#define QA_ATTN_SLOT_MASK	0xf	/* qa attention slot field mask */
#define QA_ATTN_IP		0x20	/* qa attention value for IP attn */
#define QA_ATTN_SP		0x40	/* qa attention value for SP attn */
#define QA_ATTN_RP		0x60	/* qa attention value for RP attn */

/*
 * write attention_value to qa attention field for given q
 */
void qa_write_attn (int qnum, ulong attention_value)
{
    ulong current_value;
    ulong volatile *qa_diag_ptr;
    rsp_cbus_state_t	old_cbus_state;
    leveltype		level;

    qa_diag_ptr = &qasic_diag_base->qd_u.qd_data.stat[qnum].data;

    /*
     * We must save the state of the CyBus, and stop the Cybus before
     * accessing QA diag space. When we have done our access, we must
     * restore the state of the CyBus.
     */
    level = raise_interrupt_level(NETS_DISABLE);

    old_cbus_state = rsp_disable_cbuses();

    current_value = read_qa_diag_unprotected(qa_diag_ptr);
    current_value &= ~QA_ATTN_MASK;
    current_value |= (attention_value & QA_ATTN_MASK);

    write_qa_diag_unprotected(qa_diag_ptr, current_value);

    rsp_restore_cbuses(old_cbus_state);

    reset_interrupt_level(level);
}

/*
 * enable attentions to IP in "slot" for given queue
 */
void set_qa_ip_attention (int qnum, int slot)
{
	ulong attention_value;

	attention_value = QA_ATTN_IP + (slot & QA_ATTN_SLOT_MASK);
	qa_write_attn(qnum, attention_value);
}


/*
 * enable attentions to RP at specified level for specified queue
 * attention_level should be:
 * QA_ATTN_CMD or QA_ATTN_NET;
 */
void set_qa_rp_attention (int qnum, ulong attention_level)
{
	attention_level &= QA_ATTN_CMD;	/* use only cmd/net bit */
	attention_level |= QA_ATTN_RP; 
	qa_write_attn(qnum, attention_level);
}
