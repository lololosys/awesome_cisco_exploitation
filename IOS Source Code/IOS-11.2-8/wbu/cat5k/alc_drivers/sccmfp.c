/* $Id: sccmfp.c,v 1.1.4.1 1996/05/09 14:55:08 rbadri Exp $
 * $Source: /release/112/cvs/Xwbu/cat5k/alc_drivers/sccmfp.c,v $
 *------------------------------------------------------------------
 * pmd_eeprom.c
 * sccmfp.c -- SCC driver using UART for handling IO to/from console
 *             for the Ethernet board
 *
 * ?
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * copyright 1993 Xlnt Designs Inc
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: sccmfp.c,v $
 * Revision 1.1.4.1  1996/05/09  14:55:08  rbadri
 * Branch: California_branch
 * Synergy features to the mainline
 *
 * Revision 1.1.40.1  1996/04/27  06:14:35  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.28.1  1996/04/08  01:39:24  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.16.1  1996/03/22  09:10:42  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.1  1996/03/13  07:41:35  rlowe
 * Add file content from Synergy repository.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1  1996/03/11  07:40:03  rlowe
 * Placeholders for California.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef lint
static char sccsid[]="@(#)scc.c	2.1 10/27/93 10/27/93 Copyright (c) 1993 Xlnt Designs Inc";
#endif
/*
						scc.c
		SCC driver using UART for handling IO to/from console for
		the Ethernet board

		c_scc1_intr	- entry point for scc port 1 interrupts
		scc_intr	- scc interrupt handler
		scc_init	- scc intialization
		scc_output	- console output routine
		scc_input	- console input routine

	8-1-90  MA
	03-apr-91 ahs	Cleaned up interrupt handling for VECTORED case
	11-jul-91 ahs   Fixed to deal with 'cnt' greater than 1 for scc_output
	10-aug-91 ahs   Macro defs for spl: completely disable for serial interrupts
*/


#include "acp_basic_types.h"
#include "acp_hw_defs.h"
#include "mfp_hw_defs.h"
#include "mfp_acp_defs.h"
#include "acp_c_defs.h"
#include "scc_if.h"


#define CTL_C   0x03
#define CTL_D   0x04
#define CTL_Z   0x1A
#define ESC	0x1B
#define XOFF    0x13
#define XON     0x11
/*
	Global UART error status interrupt counters
*/
u_int32 xerr = 0;
u_int32 rerr = 0;

/*
	1 character buffers for io to/from scc
*/

u_int32 _port_stopped [2];

/* buffers for rx/tx data */
#define RXBUFSIZE   256        /* must handle large packets */
#define TXBUFSIZE   256

u_int8 rxbufs[1][RXBUFSIZE];
u_int8 txbufs[1][RXBUFSIZE];
/*
	input queue for console
*/
struct sccinqueue conqueue;

/*
	output queue for console
*/
struct sccinqueue conoqueue;

u_int8 * uart_cntl_reg;
u_int8 * recv_status_reg;
u_int8 * xmit_status_reg;
u_int8 * uart_data_reg;
u_int8 * intr_mask;

/*
	Setup the SCC params to use UART
*/

int 
scc_init( port, slip )
int port;
int slip;
{
	int i;


	/* 
		Get the address of the UART registers
	*/
	uart_cntl_reg   = (u_int8 *)(ACP_HW_MFP_BASE + MFP_RO_UCR);
	recv_status_reg = (u_int8 *)(ACP_HW_MFP_BASE + MFP_RO_RSR);
	xmit_status_reg = (u_int8 *)(ACP_HW_MFP_BASE + MFP_RO_TSR);
	uart_data_reg   = (u_int8 *)(ACP_HW_MFP_BASE + MFP_RO_UDR);
	/*
		Get the addess of the Interrupt registers
	*/
	intr_mask = (u_int8 *)(ACP_HW_MFP_BASE + MFP_RO_IMRA);

	if (port == CONSOLE) {
        conqueue.max = RXBUFSIZE;
        conqueue.head = 0;
        conqueue.tail = 0;
        conqueue.drops = 0;
		conqueue.mask = RXBUFSIZE - 1;
        conqueue.buf = rxbufs[0];
        conoqueue.max = TXBUFSIZE;
        conoqueue.head = (-1);
        conoqueue.tail = 0;
        conoqueue.drops = 0;
        conoqueue.buf = txbufs[0];
	}

	/*
		Set UART Control register to 8 bit, 1 stop bit, No parity, 9600 baud
	*/
	*uart_cntl_reg =  CLK_16 | CLK_8_BIT_CHARS | ASYNC_1_STOP_BIT;


	/*
		Set intr mask register for SCC
	*/
		/* No longer use separate error interrupts for Tx/Rx. */
	*intr_mask |= (EN_RCV_BUF_FULL | EN_XMIT_BUF_EMPTY);  


    /* mark the port as not stopped */
    _port_stopped[port] = 0;
	/*
		Enable transmitter and receiver
	*/
	*recv_status_reg = RECEIVER_ENABLE;
	*xmit_status_reg = TRANSMITTER_ENABLE | XMIT_HIGH;
}


#if	0
int
scc_output(port, buf, cnt)
int port;
char *buf;
int cnt;
{
	register u_int8 *	pReg = (u_int8 *)(ACP_HW_IOREG_BASE | ACP_WRRG_DEBUG) ;
    int s;
    register int nbytes;
    register struct sccinqueue *ifq;

    /* queue as many bytes as possible */
    for(nbytes = 0; nbytes < cnt; nbytes++) {
    	PUNCH_WD ;

		/* if transmitter is busy wait for it to get free */
	while (! (*xmit_status_reg & XMIT_BUFFER_EMPTY))
		*pReg = 0x0b ;

	*uart_data_reg = *buf++ ;
    }

    /* reenable ints */
    *pReg = 0x07 ;
    PUNCH_WD ;
    return(1);
}
#else
int
scc_output(port, buf, cnt)
int port;
char *buf;
int cnt;
{
    int s;
    register int nbytes;
    register struct sccinqueue *ifq;

    /* get a pointer to the queue */
    ifq = ((port == CONSOLE) ? &conoqueue : 0);

    /* disable interrupts */
    s = spl7();

    /* queue as many bytes as possible */
    for(nbytes = 0; nbytes < cnt; nbytes++) {
        if(ifq->tail != ifq->head) {
            if(ifq->head < 0) ifq->head = ifq->tail;
            ifq->buf[ifq->tail++] = *buf++;
            if(ifq->tail == ifq->max) ifq->tail = 0;
        } else {
            ifq->drops++;
        }
    }

    /* start the interrupt handler */
    scc_start(port);

    /* reenable ints */
    splx(s);
    return(1);
}
#endif

scc_start(port)
int port;
{
	register u_int8 *	pReg = (u_int8 *)(ACP_HW_IOREG_BASE | ACP_WRRG_DEBUG) ;
	register struct sccinqueue *ifq = &conoqueue;

	if (_port_stopped[port])
		return;

	/*
		if transmitter is busy wait for it to get free
	*/
	while (ifq->head >= 0) {
    		PUNCH_WD ;

		while ((*xmit_status_reg & XMIT_BUFFER_EMPTY) != XMIT_BUFFER_EMPTY) 
			*pReg = 0x0b ;

		*uart_data_reg = ifq->buf[ifq->head++];
		if(ifq->head == ifq->max) ifq->head = 0;
		if(ifq->head == ifq->tail) ifq->head = (-1);
	}
	*intr_mask |= EN_XMIT_BUF_EMPTY; 
	*pReg = 0x07 ;
}

#if	0
long
scc_input(port, buf, cnt)
int port;
uChar *buf;
int cnt;
{
	if (*recv_status_reg & RCV_BUFFER_FULL)
	{
		return (* (u_int8 *) (uart_data_reg)) ;
	}
	else
	{
		return (-1) ;
	}
}
#else
int
scc_input(port, buf, cnt)
int port;
uChar *buf;
int cnt;
{
	register struct sccinqueue *ifq;
	register int i;

	ifq = &conqueue;
	/*
		get the buf pointer
	*/

	i = (ifq->tail + 1) & ifq->mask;
	if(ifq->head != i) {
		ifq->buf[i] = *buf;
		ifq->tail = i;
	} else {
		ifq->drops++;
	}

#if	0
    /* Wake up anybody waiting on this character.  */
    wakeup(ifq);
#endif
}
#endif

void
c_tbe_hdlr()
{
	register u_int8			Temp ;
	register struct sccinqueue *	que;

		/* This routine now also checks for errors, in case */
		/* we are not using the separate error interrupt.   */

	Temp = *xmit_status_reg ;

	if (Temp & XMIT_EXCEPTION)
	{
		++ xerr ;
	}

	if (Temp & XMIT_BUFFER_EMPTY)
	{
		que = &conoqueue;
		if(que->head >= 0) 
			scc_start(0);
		  else
			*intr_mask &= ~EN_XMIT_BUF_EMPTY; 
	}
}

void
c_rbf_hdlr()
{
	register u_int8	Temp ;

		/* This routine now also checks for errors, in case */
		/* we are not using the separate error interrupt.   */

	Temp = ((*recv_status_reg) & (RCV_BUFFER_FULL | RCV_ERROR)) ;

	if (Temp == RCV_BUFFER_FULL)		/* Good Rx. */
	{
		scc_input (0, uart_data_reg, 1);
	}
	else  if (Temp)				/* Rx error case. */
	{
		Temp = *uart_data_reg;		/* Drain UART Rx. */
		++ rerr ;
	}
}

void
c_te_hdlr()
{
	register u_int8	Temp ;


	Temp = *xmit_status_reg ;
	++ xerr ;
}

void
c_re_hdlr()
{
	register u_int8	Temp ;


	Temp = *recv_status_reg ;
	Temp = *uart_data_reg;
	++ rerr ;
}

/****************************************************************
 *
 *    Procedure: scc_change
 *
 *    Description:
 *    Changes the port speed. Change the SCC params for the UART
 *
 *    Inputs: Port number to change.
 *
 *    Outputs: None.
 *
 *    Side Effects:
 *
 ****************************************************************/
void
scc_change(int port)
{
} /* scc_change */

