/* $Id: dlc_public.h,v 3.2 1995/11/17 08:55:34 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/cls/dlc_public.h,v $
 *------------------------------------------------------------------
 * dlc_public.h - public interface for Cep's
 *
 * May,		1995, 	Phil Mortn
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: dlc_public.h,v $
 * Revision 3.2  1995/11/17  08:55:34  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:11:37  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:20:14  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __DLC_PUBLIC_H__
#define __DLC_PUBLIC_H__

STRUCTURE_REF(CepT);

/*
 * Congestion level of the DLC OutputQ.
 * Warning: DLC code depends on the order of the levels.
 *          (eg: arithmetically, DLC_OQUEUE_LOW < DLC_OQUEUE_NORMAL)
 */
typedef enum dlc_oqueue_status_ {
      DLC_OQUEUE_EMPTY,
      DLC_OQUEUE_LOW,
      DLC_OQUEUE_NORMAL,
      DLC_OQUEUE_HIGH,
      DLC_OQUEUE_FULL
} dlc_oqueue_status_t;

/*
 * Values used to control the pong window sizes
 *
 * CLS_MIN_PONG_Q_DEPTH is the smallest that the floating 
 * optimum queue depth is allowed to get.
 * CLS_MIN_PONG_WINDOW is the smallest pong window
 * size allowed, used to avoid sending too many pongs.
 *
 * CLS_MIN_PONG_Q_DEPTH must be greater than or equal to
 * CLS_MIN_PONG_WINDOW for the algorithm to work.
 */

#define CLS_MIN_PONG_Q_DEPTH 3                                 
#define CLS_MIN_PONG_WINDOW  2


/****************************************************************
 *								*
 *			Common CEP OJECT			*
 *								*
 * This data structure is avilable for all CLS compliant DLC's.	*
 * If anyone has a mind to mess with this they had better be	*
 * limit this structure to common fields			*
 *								*
 ****************************************************************
*/

struct CepT_ {
/*    void	*fUCepId;*/
    queuetype 	*fOqueue;
    ulong 	fOqueueMax;		/* depth of the transmit queue */

    /* 
     * The following fields are used to create damping zones around the
     * low and high water marks to avoid sending excessive flow control
     * indications when the txQ is oscillating around either a low or
     * high water mark.
     *
     * txQ low damp zone (similar for the hihg water damp zone): 
     *
     *                     low
     *    empty           water                               full
     *      |               |                                  |
     *      |----------|----|----|------------........---------|
     *                 |         |
     *                 |         |
     *    lowater_lodamp         lowater_hidamp
     */

    uint	fLowWater;
    uint	fHighWater;

    ushort	fDamping;
    ushort 	fLowWaterLowDamp;
    ushort 	fLowWaterHighDamp;
    ushort 	fHighWaterLowDamp;
    ushort 	fHighWaterHighDamp;

    dlc_oqueue_status_t fCurrCongLevel;  /* current txQ cong level      */
    dlc_oqueue_status_t fPrevCongLevel;  /* previous txQ cong level     */
    boolean 	fPongPending;
    ushort      fPongOptQueueDepth;
} ;

/* inlines */

#ifdef UCEPIDS
static inline void *CepGetUCepId (CepT *cep)
{	return(cep->fUCepId);			}

static inline void CepSetUCepId (Cep *cep, void *uCepId)
{	cep->fUCepId = uCepId;			}

#endif

static inline dlc_oqueue_status_t CepGetCurrCongLevel (CepT *cep)
{	return(cep->fCurrCongLevel);		}

static inline void CepSetCurrCongLevel (CepT *cep, dlc_oqueue_status_t level)
{	cep->fCurrCongLevel = level;		}

static inline dlc_oqueue_status_t CepGetPrevCongLevel (CepT *cep)
{	return(cep->fPrevCongLevel);		}

static inline void CepSetPrevCongLevel (CepT *cep, dlc_oqueue_status_t level)
{	cep->fPrevCongLevel = level;		}

/* prototypes */

void CepInit(CepT *cep, queuetype *queue, ulong queueMax, 
	 uint lowWaterPct, uint highWaterPct, uint damping);
void CepUpdate(CepT *cep, queuetype *queue, ulong queueMax, 
		uint lowWaterPct, uint highWaterPct, uint damping);
dlc_oqueue_status_t CepGetOqueueStatus (CepT *cep);

#endif /* __DLC_PUBLIC_H__ */
