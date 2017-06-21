/* $Id: qem_spif.c,v 3.3.12.1 1996/03/18 22:01:34 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/src-rsp/qem_spif.c,v $
 *------------------------------------------------------------------
 * Low-level interface to SP emulation of hardware queues and
 * accumulators
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: qem_spif.c,v $
 * Revision 3.3.12.1  1996/03/18  22:01:34  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.2  1996/03/07  10:44:27  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.2.26.1  1996/02/20  17:59:34  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.3  1996/02/13  20:49:52  hampton
 * Clean up includes of gdb.h and references to breakpoint.  [CSCdi48844]
 *
 * Revision 3.2  1995/11/17  18:47:02  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:27:22  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:27:52  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/06/07  22:57:15  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * irsp qa emulation layer
 *
 *
 * RP must:
 *	initialize mciregtype *qa_regptr with ptr to SP's multibus
 * 		registers before doing anything.
 *
 *	call init_bufhdr() for every buffer in system before using
 *		q functions.
 * 	should use 
 *            qa_acc_t  *acc_ptr (int acc_number)
 *            qa_hwq_t  *queue_ptr (int queue_number)
 * 	to get ptrs to Qs and As.
 *
 * ASSUMPTION: buffer sizes are lword multiples and lword aligned
 *
 */

#include "master.h"
#include <string.h>
#include "interface_private.h"
#include "exception.h"
#include "../hes/if_mci.h"

#include "rsp_commands.h"
#include "rsp_bufhdr.h"
#include "rsp_qasic.h"
#include "rsp_if.h"
#include "qa_hw.h"
 

/*
 * SP QA commands
 */
#define	CLRQAINTR	0x8035
#define	GETSPVERSION	0x8036		/* NOTE: copied to qem.c */
#define	WRITEMEMDLONG	0x8037
#define	READMEMDLONG	0x8038
#define	SETCOPYADDR	0x8039
#define	WRITEDIAG	0x803A
#define	READDIAG	0x803B
#define	WRITEACC	0x803C
#define	READACC		0x803D
#define	WRITEQ		0x803E
#define	READQ		0x803F

#define	HDRLWORDS		4
#define	MAXRXOFFSET		127

#define QA_INTLEVEL	3


/*
 * byte, short, offset, ptr, buffer num conversions
 */
#define	boffset2mdptr(_o)	((void *)((uchar *)memd_base + (ulong)(_o)))
#define	mdptr2boffset(_p)	((ulong)((uchar *)(_p) - (uchar *)memd_base))

#define	boffset2qaptr(_o)	((void *)((uchar *)qasic_base + (ulong)(_o)))
#define	qaptr2boffset(_p)	((ulong)((uchar *)(_p) - (uchar *)qasic_base))
#define	qaptr2soffset(_p)	(qaptr2boffset(_p)/2)

#define	boffset2bufnum(_b)	((_b)/16)
#define	bufptr2bufnum(_b)	(boffset2bufnum(mdptr2boffset(_b)))

#define	QNUM(q_ptr)		(qaptr2boffset(q_ptr)/QA_QSIZE)
#define	ANUM(a_ptr)	((qaptr2boffset(a_ptr)-QA_ACC_OFFSET-ACC_LOWBITS) / \
				QA_ACCSIZE)


static mciregtype	*qa_regptr;


static int	qtype[QA_QNUM_LIMIT];
static int	buffer_sizes[MEMD_BUFDATA_OFFSET / sizeof(bufhdr_t)];


/********************************************************************/
/*
 * Sanity checks
 */
/********************************************************************/

static int qa_error (char *s, ulong n)
{
	buginf(s, n);
	if (!breakpoint())
	    crashdump(0);
	return(1);
}

/********************************************************************/

int	is_diag_off (ulong qa_diag_offset)
{
	return(qa_diag_offset < QA_DIAGSIZE && (qa_diag_offset&7) == 0);
}

/********************************************************************/

int	is_memd_off (ulong memd_off)
{
	return ((memd_off&3) == 0  &&  memd_off >= MEMD_BUFHDR_OFFSET &&
		memd_off < MEMDSIZE);
}

/********************************************************************/

int	is_bufhdr (bufhdr_t *b)
{
	return(is_bufhdr_off(mdptr2boffset(b)) &&
	      (b->bf_buffer_ptr == 0 || is_memd_off(b->bf_buffer_ptr)));
}


/********************************************************************/

static int	chk_qnum (ulong q_num)
{
	if (!is_qnum(q_num))
		return qa_error( "\nQEM: Invalid q number: 0x%x", q_num);
	return 0;
}

/********************************************************************/

static int	chk_qptr (qa_hwq_t *q_ptr)
{
	if (!is_qptr(q_ptr))
		return qa_error( "\nQEM: Invalid q ptr: 0x%x", (ulong)q_ptr);
	return 0;
}

/********************************************************************/

static int	chk_anum (ulong acc_num)
{
	if (!is_accnum(acc_num))
		return qa_error( "\nQEM: Invalid acc number: 0x%x", acc_num);
	return 0;
}

/********************************************************************/

static int	chk_aptr( qa_acc_t *acc_ptr)
{
	if (!is_accptr(acc_ptr))
		return qa_error("\nQEM: Invalid acc ptr: 0x%x", (ulong)acc_ptr);
	return 0;
}

/********************************************************************/

static int	chk_diag_off (ulong qa_diag_offset)
{
	if (!is_diag_off(qa_diag_offset))
		return qa_error( "\nQEM: Invalid diag offset: 0x%x",
				qa_diag_offset);
	return 0;
}

/********************************************************************/

static int	chk_memd_off (ulong memd_off)
{
	if (!is_memd_off(memd_off))
		return qa_error("\nQEM: Invalid memd offset: 0x%x", memd_off);
	return 0;
}

/********************************************************************/

static int	chk_hdroff (ulong qvalue)
{
	if (!is_bufhdr_off(qvalue))
		return qa_error( "\nQEM: Invalid buf hdr offset: 0x%x", qvalue);
	return 0;
}

/********************************************************************/

static int	chk_hdr (bufhdr_t *b)
{
	if (!is_bufhdr(b))
		return qa_error( "\nQEM: Invalid buf hdr: 0x%x", (ulong)b);
	return 0;
}

/********************************************************************/
/*
 * Utilities
 */
/********************************************************************/

/*
 * read hw memd into buffer
 * ASSUMES caller has done addr/offset checks
 */
static void read_hw_memd (ulong memd_boffset, ulong *dest, int nlwords)
{
	int	i;
	leveltype level;

	level = raise_interrupt_level(QA_INTLEVEL);
	qa_regptr->write0long = memd_boffset/2;
	qa_regptr->cmdreg = SETCOPYADDR;

	for( i = 0; i < nlwords; i++)
	{
		qa_regptr->cmdreg = READMEMDLONG;
		*dest++ = qa_regptr->readlong;
	}
	reset_interrupt_level(level);
}
/********************************************************************/

static void read_memd (ulong memd_boffset, ulong *dest, int nlwords)
{
	if (!nlwords ||
	    chk_memd_off(memd_boffset) || 
	    chk_memd_off(memd_boffset+((nlwords-1)*sizeof(ulong))) ||
	    chk_memd_off(mdptr2boffset(dest)) ||
	    chk_memd_off(mdptr2boffset(dest+(nlwords-1)))
	   )
		return;

	read_hw_memd(memd_boffset, dest, nlwords);
}
/********************************************************************/

/*
 * write to hardware memd only not psuedo memd
 * ASSUMES caller checked addrs, offsets
 */
static void write_hw_memd (ulong *src, ulong memd_boffset, int nlwords)
{
	int	i;
	leveltype level;

	level = raise_interrupt_level(QA_INTLEVEL);
	qa_regptr->write0long = memd_boffset/2;
	qa_regptr->cmdreg = SETCOPYADDR;

	for( i = 0; i < nlwords; i++)
	{
		qa_regptr->write0long = *src++;
		qa_regptr->cmdreg = WRITEMEMDLONG;
	}
	reset_interrupt_level(level);
}
/********************************************************************/

void write_memd (ulong *src, ulong memd_boffset, int nlwords)
{
	if (!nlwords ||
	    chk_memd_off(memd_boffset) || 
	    chk_memd_off(memd_boffset+((nlwords-1)*sizeof(ulong))) ||
	    chk_memd_off(mdptr2boffset(src)) ||
	    chk_memd_off(mdptr2boffset(src+(nlwords-1)))
	   )
		return;

	write_hw_memd(src, memd_boffset, nlwords);
}

/********************************************************************/

static ulong	read_qval_and_hdr (qa_hwq_t *q_ptr)
{
	ulong	qvalue;
	leveltype level;

	level = raise_interrupt_level(QA_INTLEVEL);
	qa_regptr->argreg = qaptr2soffset(q_ptr);
	qa_regptr->cmdreg = READQ;
	qvalue = qa_regptr->cmdreg;

	if (qvalue != 0) {
	    if(!chk_hdroff(qvalue)) {
	        read_memd( qvalue, (ulong *)boffset2mdptr(qvalue), HDRLWORDS);

	        if(chk_hdr(boffset2mdptr(qvalue))) {
			reset_interrupt_level(level);
			return 0;
		}
	    }
	}
	reset_interrupt_level(level);

	return(qvalue);
}

/********************************************************************/

static bufhdr_t *read_q_and_buf (qa_hwq_t *q_ptr)
{
	ulong	qvalue;
	bufhdr_t *b;
	int	bnum;

	qvalue = read_qval_and_hdr(q_ptr);

	if (qvalue != 0) {
	    b = (bufhdr_t*)boffset2mdptr(qvalue);
	    bnum = boffset2bufnum(qvalue);

	    read_memd(b->bf_buffer_ptr,
		      (ulong *)boffset2mdptr(b->bf_buffer_ptr), 
		      buffer_sizes[bnum] );
	} else {
	    b = NULL;
	}
	return(b);
}

/********************************************************************/

static void     write_q_and_hdr (qa_hwq_t *q_ptr, bufhdr_t *b)
{
	leveltype level;

	if(chk_hdr(b))
		return;

	write_memd( (ulong *)b, mdptr2boffset(b), HDRLWORDS);

	level = raise_interrupt_level(QA_INTLEVEL);
	qa_regptr->rxlength = mdptr2boffset(b);
	qa_regptr->argreg = qaptr2soffset(q_ptr);
	qa_regptr->cmdreg = WRITEQ;
	reset_interrupt_level(level);
}

/********************************************************************/

static void      write_q_and_buf (qa_hwq_t *q_ptr, bufhdr_t *b, int nlwords)
{

	write_memd( (ulong *)boffset2mdptr(b->bf_buffer_ptr),
			b->bf_buffer_ptr, nlwords );

	write_q_and_hdr (q_ptr, b);
}

/********************************************************************/

static bufhdr_t *read_raw_q (qa_hwq_t *q_ptr)
{
	ulong	qvalue;
	bufhdr_t *b;
	int	nlwords, rx_bytes, n;

	qvalue = read_qval_and_hdr(q_ptr);

	if (qvalue != 0) {
	    b = (bufhdr_t*)boffset2mdptr(qvalue);

	    rx_bytes = b->bf_rx_bytes & 0x1FFF;	 /* fip uses upper bits*/
	    nlwords = (rx_bytes+MAXRXOFFSET)/sizeof(ulong);
	    n = buffer_sizes[bufptr2bufnum(b)];
	    if( n < nlwords)
	    {
		if( rx_bytes > (n*sizeof(ulong)) )
			buginf("\nQEM: Warning on buffer 0x%x, "
			       "rx_bytes 0x%x > buffer size 0x%x",
			       b, rx_bytes, n*sizeof(ulong) );
		nlwords = n;
	    }

	    read_memd( b->bf_buffer_ptr,
		      (ulong *)boffset2mdptr(b->bf_buffer_ptr), nlwords);
	} else {
	    b = NULL;
	}

	return(b);
}

/********************************************************************/

static bufhdr_t *read_q_and_hdr (qa_hwq_t *q_ptr)
{
	ulong	qvalue;
	bufhdr_t *b;

	qvalue = read_qval_and_hdr(q_ptr);

	if (qvalue != 0) {
	    b = (bufhdr_t *)boffset2mdptr(qvalue);
	} else {
	    b = NULL;
	}

	return(b);
}

/********************************************************************/

static void     write_tx_q (qa_hwq_t *q_ptr, bufhdr_t *b)
{
	int	nlwords, n;

	nlwords = (b->bf_tx_bytes+MAXRXOFFSET)/sizeof(ulong);
	n = buffer_sizes[bufptr2bufnum(b)];
	if( n < nlwords)
	{
	    if( b->bf_tx_bytes > n*sizeof(ulong) )
		buginf("\nQEM: Warning on buffer 0x%x, "
		       "tx_bytes 0x%x > buffer size 0x%x",
		       b, b->bf_tx_bytes, n*sizeof(ulong));

		nlwords = n;
	}

	write_q_and_buf(q_ptr, b, nlwords);
}

/********************************************************************/

static bufhdr_t *hwq_read (qa_hwq_t *q_ptr)
{
	/*
	 * don't do chk_qptr() here - could be alt q_ptr
	 */
	switch( qtype[QNUM(q_ptr)])
	{
		case QT_RAW:
			return(read_raw_q(q_ptr));
		case QT_GFREE:
		case QT_LFREE:
			return(read_q_and_hdr(q_ptr));
		default:
			return(read_q_and_buf(q_ptr));
	}
}

/********************************************************************/
/********************************************************************/
/*
 * interface functions
 * NOTE: most of these functions die if parameters are invalid
 */
/********************************************************************/
/********************************************************************/

/*
 * initialize RP layer of emulator code ONLY
 */
void	emulator_init (void* regptr)
{
	qa_regptr = regptr;
	memset(qtype, 0, sizeof(qtype));
	memset(buffer_sizes, 0, sizeof(buffer_sizes));
}

/********************************************************************/

int	init_bufhdr (bufhdr_t *bufhdr, int buffer_size)
{
	int current_size;

	current_size = buffer_sizes[bufptr2bufnum(bufhdr)] * sizeof(ulong);

	if(buffer_size >= 0)
	    buffer_sizes[bufptr2bufnum(bufhdr)] = (buffer_size+3)/sizeof(ulong);

	return current_size;
}

/********************************************************************/

void	hwq_init (int q_num)
{
	return;
}

/********************************************************************/

int emulate_queue_type (int type, int q_num)
{
	int current_type = 0;

	if(!chk_qnum(q_num))
	{
		current_type = qtype[q_num];

		if( type >= 0)
			qtype[q_num] = type;
	}
	return current_type;
}

/********************************************************************/

qa_hwq_t  *queue_ptr (int q_num)
{
	if(chk_qnum(q_num))
		return(NULL);

	return( (qa_hwq_t *)boffset2qaptr(q_num*QA_QSIZE) );
}

/********************************************************************/

qa_acc_t  *acc_ptr (int acc_num)
{
	if(chk_anum(acc_num))
		return(NULL);

	return (qa_acc_t*) ((char*) boffset2qaptr(acc_num*QA_ACCSIZE) +
			    QA_ACC_OFFSET + ACC_LOWBITS);
}

/********************************************************************/

ulong    read_memd_long (ulong *memd_addr)
{
	read_memd(mdptr2boffset(memd_addr), memd_addr, 1);
	return(*memd_addr);
}

/********************************************************************/

void      write_memd_long (ulong *memd_addr, ulong value)
{
	*memd_addr = value;
	write_memd(memd_addr, mdptr2boffset(memd_addr), 1);
}

/********************************************************************/

/*
 * read hardware memd only NOT psuedo memd
 * allow access below MEMD_BUFHDR_OFFSET
 */
ulong peek_hw_memd (ulong *memd_addr)
{
	ulong	value = 0;
	ulong	memd_boffset;

	memd_boffset = mdptr2boffset(memd_addr);

	if( (memd_boffset&3) || memd_boffset >= MEMDSIZE)
		return qa_error("\nQEM: Invalid memd offset: 0x%x",
			memd_boffset);
	else
		read_hw_memd(memd_boffset, &value, 1);

	return value;
}

/********************************************************************/

/*
 * write to memd and psuedo memd
 * allow access below MEMD_BUFHDR_OFFSET
 */
void poke_hw_memd (ulong *memd_addr, ulong value)
{
	ulong	memd_boffset;

	memd_boffset = mdptr2boffset(memd_addr);

	if( (memd_boffset&3) || memd_boffset >= MEMDSIZE)
		(void)qa_error("\nQEM: Invalid memd offset: 0x%x",
			memd_boffset);
	else
	{
		*memd_addr = value;
		write_hw_memd(&value, memd_boffset, 1);
	}
}

/********************************************************************/

bufhdr_t *hwq_dequeue (qa_hwq_t *q_ptr)
{
	if(chk_qptr(q_ptr))
		return(NULL);

	return(hwq_read(q_ptr));
}

/********************************************************************/

void      hwq_enqueue (qa_hwq_t *q_ptr, bufhdr_t *b )
{
	if(chk_qptr(q_ptr))
		return;

	switch( qtype[ QNUM( (qa_hwq_t *)q_ptr)] )
	{
		case QT_TX:
			write_tx_q(q_ptr, b);
			break;
		case QT_GFREE:
		case QT_LFREE:
			write_q_and_hdr(q_ptr, b);
			break;
		default:
			write_q_and_buf(q_ptr, b,
				buffer_sizes[bufptr2bufnum(b)] );
			break;
	}
}
/********************************************************************/

bufhdr_t *hwq_peek (qa_hwq_t *q_ptr)
{
	if(chk_qptr(q_ptr))
		return(NULL);

	(uchar *)q_ptr += 4;
	return(hwq_read(q_ptr));
}

/********************************************************************/

short   hwacc_read_dec (qa_acc_t *a_ptr)
{
	ushort	value;
	leveltype level;

	if(chk_aptr(a_ptr))
		return(0);

	level = raise_interrupt_level(QA_INTLEVEL);
	qa_regptr->argreg = qaptr2soffset( a_ptr);
	qa_regptr->cmdreg = READACC;
	value = qa_regptr->cmdreg;
	reset_interrupt_level(level);
	return(value);
}

/********************************************************************/

short	hwacc_peek (qa_acc_t *a_ptr)
{
	ushort	value;
	leveltype level;

	if(chk_aptr(a_ptr))
		return(0);

	level = raise_interrupt_level(QA_INTLEVEL);
	qa_regptr->argreg = qaptr2soffset(&a_ptr->alternate);
	qa_regptr->cmdreg = READACC;
	value = qa_regptr->cmdreg;
	reset_interrupt_level(level);
	return(value);
}

/********************************************************************/

void	hwacc_sub (qa_acc_t *a_ptr, short value)
{
	leveltype level;

	if(chk_aptr(a_ptr))
		return;

	level = raise_interrupt_level(QA_INTLEVEL);
	qa_regptr->rxlength = value;
	qa_regptr->argreg = qaptr2soffset(a_ptr);
	qa_regptr->cmdreg = WRITEACC;
	reset_interrupt_level(level);
}

/********************************************************************/

void	hwacc_set (qa_acc_t *a_ptr, short value)
{
	leveltype level;

	if(chk_aptr(a_ptr))
		return;

	level = raise_interrupt_level(QA_INTLEVEL);
	qa_regptr->rxlength = value;
	qa_regptr->argreg = qaptr2soffset(&a_ptr->alternate);
	qa_regptr->cmdreg = WRITEACC;
	reset_interrupt_level(level);
}

/********************************************************************/

ulong read_qa_diag (volatile ulong *qa_diag_ptr)
{
	leveltype level;
	ulong	qa_diag_offset, value, lsa, data;

	qa_diag_offset = (uchar *)qa_diag_ptr - (uchar *)qasic_diag_base;

	if(chk_diag_off( qa_diag_offset))
		return(0);

	level = raise_interrupt_level(QA_INTLEVEL);
	qa_regptr->argreg = qa_diag_offset/2;
	qa_regptr->cmdreg = READDIAG;
	value = qa_regptr->readlong;

		/*
		 * adjust for sp's way of storing diag values
		 */
	if( qa_diag_offset < DIAG_ERROR_LOG_BASE)
	{
		lsa = (value & 0x10000) ? QA_LSA_BIT : 0;
		data = value >> 4;
		data &= ~0xffff;
		value &= 0xffff;
		value |= (lsa|data);
	}

	reset_interrupt_level(level);
	return( value);
}

/********************************************************************/

void write_qa_diag (volatile ulong *qa_diag_ptr, ulong value)
{
	leveltype level;
	ulong qa_diag_offset;

	qa_diag_offset = (uchar *)qa_diag_ptr - (uchar *)qasic_diag_base;

	if(chk_diag_off( qa_diag_offset))
		return;

		/*
		 * adjust for sp's way of storing diag values
		 */
	if( qa_diag_offset < DIAG_ERROR_LOG_BASE)
	{
		ulong	lsa, data;

		lsa = (value & QA_LSA_BIT) ? 0x10000 : 0;
		data = (value&~0xffff);
		data <<= 4;
		value &= 0xffff;
		value |= (lsa|data);
	}

	if( qa_diag_ptr < (ulong *)&qasic_diag_base->qd_u.qd_data.tail[0])
	{
		/*
		 * its a write to head array - update memd q head and altq
		 */
		int qnum;
		ulong head_val;

		qnum = qa_diag_offset/sizeof(qa_diag_data_t);
		if (is_qnum(qnum)) {
		    head_val = value & 0xffff;
		    if (head_val)
			head_val = (ulong)BUFHDR_OFFSET_TO_ADDR(head_val);

		    poke_hw_memd((ulong*)queue_ptr(qnum), head_val);
		    poke_hw_memd((ulong*)&queue_ptr(qnum)->alternate, head_val);
		}
	}

	level = raise_interrupt_level(QA_INTLEVEL);
	qa_regptr->write0long = value;
	qa_regptr->argreg = qa_diag_offset/2;
	qa_regptr->cmdreg = WRITEDIAG;

	if( qa_diag_offset == DIAG_MEMD_BASE_REG)
	{
		memd_base = (uchar *)(value & ~0xffff);
	}
	reset_interrupt_level(level);
}

/********************************************************************/

ulong get_sp_version (void)
{
    ushort val;
    leveltype level;

    level = raise_interrupt_level(QA_INTLEVEL);
    qa_regptr->cmdreg = GETSPVERSION;
    val = qa_regptr->cmdreg;
    reset_interrupt_level(level);
    return val;
}

/********************************************************************/

void clr_qa_intr (void)
{
	qa_regptr->cmdreg = CLRQAINTR;
}

/********************************************************************/

ushort	read_ccb_done (struct ccbtype_ *ccb)
{
	(void) read_memd_long( (ulong *)&ccb->ccb_res0done);
	if( ccb->ccb_done)
	{
		(void) read_memd_long( (ulong *)&ccb->ccb_cmdarg0);
		(void) read_memd_long( (ulong *)&ccb->ccb_arg1res1);
	}
	return(ccb->ccb_done);
}

/******************************* END *********************************/

