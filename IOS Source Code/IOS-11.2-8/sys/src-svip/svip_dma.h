/* $Id: svip_dma.h,v 3.1.62.1 1996/03/21 23:51:28 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/src-svip/svip_dma.h,v $
 *------------------------------------------------------------------
 * svip_dma.h - SVIP DMA definitions
 *
 * 10/95, David Getchell
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: svip_dma.h,v $
 * Revision 3.1.62.1  1996/03/21  23:51:28  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.1.10.4  1996/03/07  17:29:45  getchell
 * CSCdi50953:  viper dfs mods
 * Branch: ELC_branch
 *
 * Revision 3.1.10.3  1996/02/21  20:37:23  getchell
 * CSCdi49474:  Time to turn on IPC/DFS on viper
 * Do some cleanup while at it...
 * Branch: ELC_branch
 *
 * Revision 3.1.10.2  1996/02/16  22:33:57  getchell
 * CSCdi49237:  svip kernel and asm files out of whack
 * cache invalidate before reading instead of after
 * add some accounting useful for debugging
 * change function definitions for drq/dtq consumer (no longer static)
 * Branch: ELC_branch
 *
 * Revision 3.1.10.1  1996/02/10  00:22:03  mbeesley
 * CSCdi48581:  Add VIP2 support to ELC_branch
 * Branch: ELC_branch
 *
 * Revision 3.1  1995/12/26  21:18:39  getchell
 * Placeholder files for Viper development
 *
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Definitions
 */

 /* The number of SVIP_DMA_BUFFERS does not need to be a power of two */
#define SVIP_DMA_BUFFERS		450		
#define SVIP_POLL_SLOP                  8
#define SVIP_MIN_DRQ_CLEAR              16

#ifndef ASMINCLUDE

typedef struct drq_entry_t_ {
    volatile ulong    drq_word0;
    volatile ulong    drq_word1;
} drq_entry_t;

#define DRQ_DMA_OWN	 0x80000000	      /* Entry owned by dma */
#define DRQ_DMA_EOP	 0x40000000	      /* EOP entry */
#define DRQ_DMA_TRANSFER 0x01000000           /* Form of entry */
#define DRQ_STATUS_MASK  0x00FF0000           /* Status Mask */
#define DRQ_QUEUE_SHIFT  16                   /* Located in leftmost 16 bits */

#define DRQ_SIZE         128                  /* Must be power of two */

#define DRQ_ADVANCE_RING_INDEX(a) (((a) + 1) & (DRQ_SIZE-1))

typedef struct dtq_struct0_ {
    uchar  control;                  /* OWN, EOP, etc */
    uchar  ifc;                      /* interface */
    ushort length;                   /* length of transfer */
} dtq_struct0_t;

typedef struct dtq_entry_t_ {
    union {
	ulong    word0;
	dtq_struct0_t     s0;
    }u0;
    ulong    word1;
} dtq_entry_t;
#define dtq_word0     u0.word0
#define dtq_struct0   u0.s0
#define dtq_word1     word1

#define DTQ_DMA_OWN	 0x80000000	      /* Entry owned by dma */
#define DTQ_DMA_EOP	 0x40000000	      /* EOP entry */
#define DTQ_INTFC_MASK   0x00FF0000           /* interface Mask */
#define DTQ_INTFC_SHIFT  16                   /* Shift right 16 places */
#define DTQ_INTFC_STW_SHIFT 4                 /* Status word shift 4 places */
#define DTQ_LENGTH_MASK  0x0000FFFF           /* length mask */

#define DTQ_SIZE         128                  /* Must be power of two */
#define DTQ_ADVANCE_RING_INDEX(a) (((a) + 1) & (DTQ_SIZE-1))

#define VIPER2QE_PCIADDR(a) (((ulong)(a) & 0xFFFFFF))

/*
 * Prototypes
 */
extern void		svip_init_dma_pools(void);
extern boolean		vip_pak_to_host(hwidbtype *, pas_cbtype *, paktype *);
extern boolean		vip_pak_switch(vip_memd_ifcntl_t *, paktype *,
					vip_bhdr_t *, volatile ulong *);
	
/**************************************************************************
 * Static inlines
 **************************************************************************/

/*
 * vip_acquire_global_lock:
 * Get the global lock
 */
static inline boolean vip_acquire_global_lock (void)
{
    return(TRUE);
}

/*
 * vip_clear_global_lock_request:
 * Give up the global lock
 */
static inline void vip_clear_global_lock_request (void)
{
}

#endif /* ASMINCLUDE */

/* end of file */

