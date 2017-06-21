/* $Id: biga.h,v 1.1.4.1 1996/05/09 14:54:29 rbadri Exp $
 * $Source: /release/112/cvs/Xwbu/cat5k/alc_cominc/biga.h,v $
 *-----------------------------------------------------------------
 * biga.h
 *
 * October 1994, husain
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: biga.h,v $
 * Revision 1.1.4.1  1996/05/09  14:54:29  rbadri
 * Branch: California_branch
 * Synergy features to the mainline
 *
 * Revision 1.1.40.1  1996/04/27  06:13:54  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.28.1  1996/04/08  01:38:36  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.16.2  1996/04/03  21:18:23  sakumar
 * LE-ARP indication from TSAR sends SRC address (as well as DEST address).
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.16.1  1996/03/22  09:09:56  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.1  1996/03/13  07:40:54  rlowe
 * Add file content from Synergy repository.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1  1996/03/11  07:39:25  rlowe
 * Placeholders for California.
 *
 *------------------------------------------------------------------
 * Legacy comments from Synergy repository:
 *
 * Revision 1.16  1995/10/04  01:32:09  kchristi
 * Fix CVS Endlog directives so that cvs-fix-merge script will work.
 *
 * Revision 1.15  1995/08/26  18:47:23  sakumar
 * Added field bigatd_size_s (BIGA Transmit Buffer Descriptor Size) for use
 * by RXSAR.
 *
 * Revision 1.14  1995/07/11  00:07:28  sakumar
 * Increased buffer size for transmit buffers to fix more bit being set
 * on BIGA transmit buffers due to size round-up.
 *
 * Revision 1.13  1995/07/05  21:03:13  sakumar
 * Index mask now discards bits 13 .. 15 (card instance number) when looking
 * for index/vlan.
 *
 * Revision 1.12  1995/05/25  23:32:03  rahuja
 * Fixed #define for The I/G bit
 *
 * Revision 1.11  1995/05/05  00:25:48  sakumar
 * Increased size of RX Biga Buffers used by TXSAR.
 *
 * Revision 1.10  1995/04/07  19:58:27  rahuja
 * 	Added support for inband ipc
 *
 * Revision 1.9  1995/03/18  01:47:33  sakumar
 * Merged txsar and atmdriver with IOS environment
 *
 * Revision 1.8  1995/02/06  18:27:38  rahuja
 * Added cam defines
 *
 * Revision 1.7  1994/12/13  16:48:06  sakumar
 * If ATMIZER_EMULATOR is not defined, then the start of shared memory is
 * different between the ACP and the SAR.
 *
 * Revision 1.6  1994/12/09  23:28:04  sakumar
 * Added start of BIGA REG from SAR point of view.
 *
 * Revision 1.5  1994/12/08  22:52:24  rahuja
 * *** empty log message ***
 *
 * Revision 1.4  1994/12/05  02:29:35  rahuja
 * Integrated with biga.h and memory mapping for the buffer manager
 *
 * Revision 1.3  1994/12/02  00:34:22  rahuja
 * Moved biga descriptors from queue_if.h
 *
 * Revision 1.2  1994/11/09  17:54:19  husain
 * INT32ermediate checkin
 *
 * Revision 1.1  1994/10/24  18:38:38  husain
 * Moved biga.h to here
 *
* Revision 1.1  1994/10/20  15:37:36  husain
* file created
*
*
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef  _bigah_
#define  _bigah_

#include	"basic_types.h"
#include	"param.h"

/*
**                  B I G A _ T D
**
**  Biga_TDs are used to enqueu data to be sent out on the synergy bus.
**
*/
 
#ifdef LANGUAGE_C
 
typedef struct biga_td {
 
    void    *bigatd_pData;                   /* data to be transmitted       */
 
    union {
 
        struct {
            UINT32  more    : 1;
            UINT32  disable : 1;
            UINT32  xdone   : 1;
            UINT32  unused  : 1;
            UINT32  abort   : 1;
            UINT32  qcode   : 3;
            UINT32  ubits   : 8;
            UINT32  length  : 16;
        } bigatd_control_bits;
 
        UINT32  bigatd_control;
 
    } control_u;
 
    struct biga_td  *bigatd_pNext;
 
} tBIGATD, *tpBIGATD;
 
#define TBD_NEXTP_OFFSET	12
 
/*
**  BIGA TD QUEUE
**  An array of BIGA Transmit discriptors is maINT32ained.  The RX sar updates
**  biga tds at the tail, the BIGA services queued packets from the head
*/
 
extern tpBIGATD pBigaTdHead;
extern tpBIGATD pBigaTdTail;
 
extern tBIGATD  bigatd[];                   /* array of BIGA TDs            */
extern UINT16   bigatdp_size;               /* number of biga tds           */
 
 
#else /* assembly */
 
        .data
        .struct 0
bigatd_pData_s:     .word 0
bigatd_control_s:   .word 0
bigatd_pNext_s:     .word 0
bigatd_size_s:
 
        .text                           /* reestablish the default      */
#endif /* language */

 

/*
**	Definitions of  bigatd_control_bits 
**
*/

#define CTL_MORE             0x80000000
#define CTL_DISABLE          0x40000000
#define CTL_XDONE            0x20000000
#define CTL_NOFCS            0x10000000
#define CTL_XMTABT           0x08000000
#define CTL_QC_SPECIAL       0x02000000
#define CTL_QC_RX            0x03000000
#define CTL_QC_SYNC          0x04000000
#define CTL_QC_ASYNC0        0x05000000
#define CTL_QC_ASYNC1        0x06000000
#define CTL_QC_ASYNC2        0x07000000
#define CTL_UD_0             0x00010000
#define CTL_UD_1             0x00020000
#define CTL_UD_2             0x00040000
#define CTL_UD_3             0x00080000
#define CTL_UD_4             0x00100000
#define CTL_UD_5             0x00200000
#define CTL_UD_6             0x00400000
#define CTL_UD_7             0x00800000
#define CTL_LEN              0x0000ffff

#define	CTL_UD_MASK			 0x00ff0000




/*
**                          B I G A _ R D
**
**  BIGA Receive descriptor.  Frames recvd off the synergy bus are placed
**  in buffers poINT32ed to by BIGA recv descriptors
*/
 
 
#ifdef LANGUAGE_C
 
typedef struct biga_rd {
 
    void    *bigard_pData;              /* recv buffer address          */
    UINT32  bigard_usable_length;       /* size of recv buffer          */
 
    union {
 
        struct {
            UINT32  more        : 1;
            UINT32  used_length : 15;
            UINT32  mored       : 1;            /* dupliacted fields    */
            UINT32  used_lengthd: 15;
        } control_bits;
 
        UINT32  bigard_control;
 
    } control_u;

    union {
 
        struct {
            UINT32  sage_status : 8;
            UINT32  reserved_1  : 5;
            UINT32  empty       : 1;
            UINT32  trap        : 1;
            UINT32  flood       : 1;            /*if 0(flood), index is color*/
            UINT32  card_id     : 3;
            UINT32  reserved_2  : 3;            /* index retrieved from the */
            UINT32  index       : 10;           /* synergy header           */
        } index_bits;
 
        struct {                                /* if FLOOD bit is set      */
                                                /* use this description     */
            UINT32  sage_status : 8;
            UINT32  reserved_1  : 5;
            UINT32  empty       : 1;
            UINT32  trap        : 1;
            UINT32  flood       : 1;            /* if set, index is color   */
            UINT32  card_id     : 3;
            UINT32  reserved_2  : 2;
            UINT32  i_g_bit     : 1;            /*individual/group bit      */
            UINT32  color       : 10;           /* color id                 */
        } color_bits;
 
        UINT32  bigard_dest_index;
 
    } dest_index_u;
 
    struct biga_rd  *bigard_pNext;
    struct biga_rd  *bigard_pPrev;				/* previous descriptor		*/
    UINT16  bigard_pad;                         /* 2 byte pad for alignment */
                    /* bigard_pData field is initialised */
                    /* to point to bigard_syhdr field   */
    UINT8   bigard_syhdr[SYHDR_SIZE];           /* space for synergy header */
    UINT8   bigard_da[6];                       /* 6 byte destination addr  */
    UINT8   bigard_sa[6];                       /* 6 byte source addr       */
 
} tBIGARD, *tpBIGARD;
 
 
typedef struct bigard_buf {                     /* overlay describing the rd*/
                                                /* contigous with sy hdr,   */
                                                /* ethernet header + data   */
 
    tBIGARD bigard;                             /* biga recv descriptor     */
    UINT8   bigard_data[MAX_ENENT_PACKET];   	/* max size ethernet frame  */
    UINT32  bigard_crc;
	UINT32	bigard_pad;
 
} tBIGARDBUF, *tpBIGARDBUF;


/*
**
**  BIGA RD QUEUE
**  An array of BIGA Recv discriptors is maintained.  Biga enques recvd
**  data packets at head, tx sar services from tail
**
*/
 
extern tpBIGARD pBigaRdHead;
extern tpBIGARD pBigaRdTail;
 
 
 
#else /* assembly */
 
	/*
	**
	**	NOTE:  the bigard size is used in txsar.h.  If the biga rd
	**	structure is changed, be sure to change the size in txsar.h
	**	structure tVCRSTRUCT accordingly
	**	 
	*/
        .data
        .struct 0
bigard_pData_s:             .word 0
bigard_usable_length_s:     .word 0
bigard_control_s:           .word 0
										/* the Index field has:			*/
bigard_index_s:
bigard_dindex_status_8s:    .byte 0		/* 8 bit status					*/
bigard_dindex_retf_8s:    	.byte 0		/* 5 bits resvd					*/
										/* Empty, trap, flood bits		*/
bigard_dindex_value_16s:	.half 0		/* 16 bits of index				*/
			

bigard_pNext_s:             .word 0
bigard_pPrev_s:             .word 0
bigard_pad_16s:             .half 0
bigard_syhdr_s:             .space 12
bigard_da_s:                .space 6
bigard_sa_s:                .space 6
bigard_dmasz_s:                         /* dma recv desc upto here to vcr   */

        .text                           /* reestablish the default      */

#endif /* language */

#define	BIGARD_PNEXT_OFFSET		16

#define	BIGA_RX_BUFSZ_REAL (SYHDR_SIZE+MAX_ENENT_PACKET+sizeof(UINT32))
#define	BIGA_RX_BUFSZ	(BIGA_RX_BUFSZ_REAL+ (4 - (BIGA_RX_BUFSZ_REAL & 0x3)))


#define LEN_MASK	0x7fff
#define QC_SHIFT        24
#define MAXBURST	64
#define MAXBURSTMASK	(64-1)

#define ROUNDUP(a, n)   (((a) + ((n) - 1)) & ~((n) - 1))
#define BURSTMASK	(MAXBURST - 1)
#define RXDATASIZE      ROUNDUP(RXBUFSIZE,MAXBURST)

/*
**
**	Defines for INDEX fields
**
*/

#define	INDEX_STATUS_MASK_C				0xff000000
#define	INDEX_STATUS_SHIFT_C			24
#define	INDEX_EMPTY_BIT_C				0x00040000
#define	INDEX_TRAP_BIT_C				0x00020000
#define	INDEX_FLOOD_BIT_C				0x00010000
#define	INDEX_MASK_C					0x00001fff
#define	INDEX_FLOOD_COLOR_MASK_C		0x000003ff	/* i0-i9: 10 bit color	*/
#define	INDEX_IG_BIT_C					0x00000400	/* bit i10:	i/g bit		*/


/*
**
**	B I G A   device registers
**
*/

#ifdef LANGUAGE_C

typedef struct biga_reg {

    UINT16 npctl;    	/* R/W  8 bit  NP Control register 			*/
    UINT8 filler1;
    UINT8 filler2;
    UINT16 com;      	/* R/W 16 bit  COM register 				*/
    UINT16 filler3;
    UINT8 INT320;      	/*   W  8 bit  Interrupt 0 					*/
    UINT8 filler4;
    UINT8 filler5;
    UINT8 filler6;
    UINT8 INT321;      	/*   W  8 bit  Interrupt 1 					*/
    UINT8 filler7;
    UINT8 filler8;
    UINT8 filler9;
    UINT16 npim;     	/* R/W 16 bit  NP interrupt mask 			*/
    UINT16 filler10;
    UINT16 nist;     	/* R/W 16 bit  NP interrupt status 			*/
    UINT16 filler11;
    UINT8 i0v;       	/* R/W 8 bit sbus interrupt 0 vector		*/
    UINT8 filler12;
    UINT8 filler13;
    UINT8 filler14;
    UINT8 i1v;       	/* R/W 8 bit sbus interrupt 1 vector		*/
    UINT8 filler15;
    UINT8 filler16;
    UINT8 filler17;
    UINT8 rsrcv;     	/* R/W  8 bit  resource not available vector*/
    UINT8 filler18;
    UINT8 filler19;
    UINT8 filler20;
    UINT8 rcvev;     	/* R/W  8 bit  dma receive done for NP vector*/
    UINT8 filler21;
    UINT8 filler22;
    UINT8 filler23;
    UINT8 xmtev;     	/* R/W  8 bit  dma transmit error vector	*/
    UINT8 filler24;
    UINT8 filler25;
    UINT8 filler26;
    UINT8 rdnpv;     	/* R/W  8 bit  receive done for NP vector	*/
    UINT8 filler27;
    UINT8 filler28;
    UINT8 filler29;
    UINT8 rdsbv;     	/* R/W  8 bit  receive done for host vector	*/
    UINT8 filler30;
    UINT8 filler31;
    UINT8 filler32;
    UINT8 xmtdv;     	/* R/W  8 bit  transmit done vector			*/
    UINT8 filler33;
    UINT8 filler34;
    UINT8 filler35;
    UINT32  filler80;
    UINT8 lev;       	/* R/W  8 bit  LateError vector				*/
    UINT8 filler36;
    UINT8 filler37;
    UINT8 filler38;
    UINT8 aev;       	/* R/W  8 bit  Ack Error Vector				*/
    UINT8 filler39;
    UINT8 filler40;
    UINT8 filler41;
    UINT8 cstat;     	/* R    8 bit  COM status register			*/
    UINT8 filler42;
    UINT8 filler43;
    UINT8 filler44;
    UINT8 parv;	    	/* R/W 8 bit parity error vector 			*/
    UINT8 filler45;
    UINT8 filler46;
    UINT8 filler47;
    UINT16 sist;     	/* R/W 16 bit  host interrupt status		*/
    UINT16 filler48;
    UINT16 hica;     	/* R/W 16 bit  host interrupt config reg a	*/
    UINT16 filler49;
    UINT16 hicb;     	/* R/W 16 bit  host interrupt config reg b	*/
    UINT16 filler50;
    UINT8 hicc;      	/* R/W  8 bit  host interrupt config reg c	*/
    UINT8 filler51;
    UINT8 filler52;
    UINT8 filler53;
    UINT16 upad;     	/* R/W 16 bit  upper DVMA address			*/
    UINT16 filler54;
    UINT16 dctrl;    	/* R/W 16 bit  dma control					*/
    UINT16 filler55;
    UINT8 dstat;     	/* R/W  8 bit  dma status					*/
    UINT8 filler56;
    UINT8 filler57;
    UINT8 filler58;
    UINT8 dctrl2;    	/* R/W 8 bit DMA control 2 reg 				*/
    UINT8 filler59;
    UINT8 filler60;
    UINT8 filler61;
    UINT16 pctrl;    	/* R/W 16 bit parity control reg 			*/
    UINT16 filler;
    UINT32 thead;      	/* R/W 32 bit  dma transmit list head		*/
    UINT32 ttail;      	/* R/W 32 bit  dma transmit list tail		*/
    UINT32 ttmph;      	/* R/W 32 bit  dma transmit list temp head	*/
    UINT32 tptr;       	/* R/W 32 bit  dma transmit pointer			*/
    UINT8 tqsel;     	/* R/W  8 bit  dma transmit queue select	*/
    UINT8 filler62;
    UINT8 filler63;
    UINT8 filler64;
    UINT16 tlen;     	/* R/W 16 bit  dma transmit buffer length	*/
    UINT16 filler65;
    UINT16 tdsc;       	/* R/W 16 bit  dma transmit pkt desc		*/
    UINT16 filler66;
    UINT16 pstat;    	/* R/W 16 bit parity status 				*/
    UINT16 filler67;
    UINT32 rdsc;       	/* R/W 32 bit  dma receive pkt desc			*/
    UINT32 rhead;      	/* R/W 32 bit  dma receive list head		*/
    UINT32 rtail;      	/* R/W 32 bit  dma receive list tail		*/
    UINT32 rtmph;      	/* R/W 32 bit  dma receive list temp head	*/
    UINT32 rplen;      	/* R/W 32 bit  dma total length pointer		*/
    UINT16 rtlen;    	/* R/W 16 bit  dma receive total length		*/
    UINT16 filler68;
    UINT32 rptr;       	/* R/W 32 bit  dma receive pointer			*/
    UINT16 rlen;     	/* R/W 16 bit  dma receive buffer length	*/
    UINT16 filler69;
    UINT8 fltr;     	/* R/W 8 bit  dma filter control			*/
    UINT8 filler70;
    UINT8 filler71;
    UINT8 filler72;
    UINT8 fc;        	/* R    8 bit  packet FC field				*/
    UINT8 filler73;
    UINT8 filler74;
    UINT8 filler75;
    UINT32 sfq;        	/* R/W 32 bit  special pkt queue w/o tag	*/
    UINT32 sfqt;       	/* R/W 32 bit  special pkt queue w/ tag		*/
    UINT32 rq;         	/* R   32 bit  receive queue w/o tag		*/
    UINT32 stq;        	/*   W 32 bit  sync transmit queue w/o tag	*/
    UINT32 stqt;       	/*   W 32 bit  sync transmit queue w/ tag	*/
    UINT32 atq0;       	/*   W 32 bit async transmit queue 0 w/o tag*/
    UINT32 atq0t;      	/*   W 32 bit async transmit queue 0 w/ tag	*/
    UINT32 atq1;       	/*   W 32 bit async transmit queue 1 w/o tag*/
    UINT32 atq1t;      	/*   W 32 bit  async transmit queue 1 w/ tag*/
    UINT32 atq2;       	/*   W 32 bit async transmit queue 2 w/o tag*/
    UINT32 atq2t;      	/*   W 32 bit  async transmit queue 2 w/ tag*/
    UINT32 filler81;
    UINT32 filler82;
    UINT8 rev;	     	/* R 8 bit revision reg 					*/
    UINT8 filler76;
    UINT8 filler77;
    UINT8 filler78;
    UINT32  cfg;	   	/* R/W 32 bit bus config reg 				*/

} tBIGA_DEVICE, *tpBIGA_DEVICE;

#endif	/* language_c */

#ifndef _bigaregh_
#define _bigaregh_

#define BIGA32_REGBASE		BIGA_REGBASE_ADDRESS	/* for 32 bit access	*/
#define BIGA16_REGBASE		BIGA_H_BASE				/* for 16 bit accesses	*/

#define	SAR_BIGA32_REGBASE	0xa0e00000				/* as viewed from SARs	*/

#define RBD_NEXTP_OFFSET	16


#define SS2L(h,l)	((h << 16) | l)	/*take 2 shorts and make a long*/
	
/*
**
**	BIGA Gate Array Register Memory Map Structure
**
*/

#define	NPCTL	0	/* R/W	8 bit	NP Control register */
#define	COM		1	/* R/W 16 bit	COM register */
#define	INT0	2	/*	W	8 bit	Interrupt 0 */
#define	INT1	3	/*	W	8 bit	Interrupt 1 */
#define	NPIM	4	/* R/W 16 bit	NP interrupt mask */
#define	NIST	5	/* R/W 16 bit	NP interrupt status */
#define	I0V		6	/* R/W	8 bit	sbus interrupt 0 vector*/
#define	I1V		7	/* R/W	8 bit	sbus interrupt 1 vector*/
#define	RSRCV	8	/* R/W	8 bit	resource not available vector*/
#define	RCVEV	9	/* R/W	8 bit	dma receive done for NP vector*/
#define	XMTEV	10	/* R/W	8 bit	dma transmit error vector*/
#define	RDNPV	11	/* R/W	8 bit	receive done for NP vector*/
#define	RDSBV	12	/* R/W	8 bit	receive done for host vector*/
#define	XMTDV	13	/* R/W	8 bit	transmit done vector*/
#define	LEV		15	/* R/W	8 bit	LateError vector*/
#define	AEV		16	/* R/W	8 bit	Ack Error Vector*/
#define	CSTAT	17	/* R	8 bit	COM status register*/
#define	PARV	18	/* R/W	8 bit	parity error vector */
#define	SIST	19	/* R/W 16 bit	host interrupt status*/
#define	HICA	20	/* R/W 16 bit	host interrupt config reg a*/
#define	HICB	21	/* R/W 16 bit	host interrupt config reg b*/
#define	HICC	22	/* R/W	8 bit	host interrupt config reg c*/
#define	UPAD	23	/* R/W 16 bit	upper DVMA address*/
#define	DCTRL	24	/* R/W 16 bit	dma control*/
#define	DSTAT	25	/* R/W	8 bit	dma status*/
#define	DCTRL2	26	/* r/w 8 bit dma control 2 */
#define	PCTRL	27	/* r/w 16 bit parity control */
#define	THEAD	28	/* R/W 32 bit	dma transmit list head*/
#define	TTAIL	29	/* R/W 32 bit	dma transmit list tail*/
#define	TTMPH	30	/* R/W 32 bit	dma transmit list temp head*/
#define	TPTR	31	/* R/W 32 bit	dma transmit pointer*/
#define	TQSEL	32	/* R/W	8 bit	dma transmit queue select*/
#define	TLEN	33	/* R/W 16 bit	dma transmit buffer length*/
#define	TDSC	34	/* R/W 16 bit	dma transmit pkt desc*/
#define	PSTAT	35	/* R/W 16 bit parity status */
#define	RDESC	36	/* R/W 32 bit receive descriptor */
#define	RHEAD	37	/* R/W 32 bit	dma receive list head*/
#define	RTAIL	38	/* R/W 32 bit	dma receive list tail*/
#define	RTMPH	39	/* R/W 32 bit	dma receive list temp head*/
#define	RPLEN	40	/* R/W 32 bit	dma total length pointer*/
#define	RTLEN	41	/* R/W 16 bit	dma receive total length*/
#define	RPTR	42	/* R/W 32 bit	dma receive pointer*/
#define	RLEN	43	/* R/W 16 bit	dma receive buffer length*/
#define	FLTR	44	/* R/W 16 bit	dma filter control*/
#define	FC		45	/* R	8 bit	packet FC field*/
#define	SFQ		46	/* R/W 32 bit	special pkt queue w/o tag*/
#define	SFQT	47	/* R/W 32 bit	special pkt queue w/ tag*/
#define	RQ		48	/* R	32 bit	receive queue w/o tag*/
#define	STQ		49	/*	W 32 bit	sync transmit queue w/o tag*/
#define	STQT	50	/*	W 32 bit	sync transmit queue w/ tag*/
#define	ATQ0	51	/*	W 32 bit	async transmit queue 0 w/o tag*/
#define	ATQ0T	52	/*	W 32 bit	async transmit queue 0 w/ tag*/
#define	ATQ1	53	/*	W 32 bit	async transmit queue 1 w/o tag*/
#define	ATQ1T	54	/*	W 32 bit	async transmit queue 1 w/ tag*/
#define	ATQ2	55	/*	W 32 bit	async transmit queue 2 w/o tag*/
#define	ATQ2T	56	/*	W 32 bit	async transmit queue 2 w/ tag*/
#define	CLRDMA0	57	/* W 8 bit clear NP DMA 0 */
#define	CLRDMA1	58	/* W 8 bit clear NP DMA 1 */
#define	BIGAREV	59	/* R 8 bit revision register */
#define	BIGACFG	60	/* RW 8/32 bit bus configuration register */

#define B2W(x)			((x)/sizeof(int))		/*convert bytes to words*/
#define W2B(x)			((x)*sizeof(int))		/*convert words to bytes*/

/*Formac Queue Codes*/

#define QC_ASYNC0		(5 << 24)
#define QC_ASYNC1		(6 << 24)
#define QC_ASYNC2		(7 << 24)
#define QC_SPECIAL		(2 << 24)
#define QC_RX			(3 << 24)
#define QC_SYNC			(4 << 24)

/*	NPCTL NP Control register bit definitions */
#define RESET	0x01
#define BYPN	0x02
#define BACKEN	0x04
#define SBCI	0x08
#define IMOD	0x10
#define NPCI	0x20
#define THHLC	0x40
#define RHHLC	0x80
#define DMA1EN	0x100
#define XRESET	0x200

/* NPIM NP Interrupt Mask register bit definitions */
#define MS0I	0x0001
#define MS1I	0x0002
#define MRDNP	0x0004
#define MRDSB	0x0008
#define MXMTD	0x0010
#define MRCVE	0x0020
#define MXMTE	0x0040
#define MRSRC	0x0080
#define MPAR	0x0100
#define MAERR	0x0200
#define MLERR	0x0400

/* NIST NP Interrupt Status register bits masks*/

#define NINTS		11		/*Total number of interrupts*/
#define N_S0INT		0x0001	/*np interrupt 0*/
#define N_S1INT		0x0002	/*np interrupt 1*/
#define N_RDNP		0x0004	/*receive done for np*/
#define N_RDSB		0x0008	/*receive done for host*/
#define N_XMTD		0x0010	/*transmit done*/
#define N_RCVE		0x0020	/*receive error*/
#define N_XMTE		0x0040	/*transmit error*/
#define N_RSRC		0x0080	/*resource error*/
#define N_PAR		0x0100	
#define N_AERR		0x0200	
#define N_LERR		0x0400	
#define N_DMA0		0x0800	
#define N_DMA1		0x1000	

/*DMA Control register*/

#define DMA_DUMP		0x8000		/*dump error pkt enable*/
#define DMA_FIXED		0x4000		/*dump error pkt enable*/
#define DMA_FCP1		0x2000		/*fc receive offset*/
#define DMA_FCP0		0x1000		/*fc receive offset*/
#define DMA_RSTRT		0x0800		/*transmit enable*/
#define DMA_RXA			0x0400		/*receive enable*/
#define DMA_TSTRT		0x0200		/*transmit enable*/
#define DMA_TXA			0x0100		/*transmit enable*/
#define DMA_RXEN		0x0080		/*receive enable*/
#define DMA_TXEN		0x0040		/*transmit enable*/
#define DMA_BSIZ16		0x0020		/*burst size 16 words*/
#define DMA_BSIZ8		0x0010		/*burst size 8 words*/
#define DMA_BSIZ4		0x0008		/*burst size 4 words*/
#define DMA_BSIZ2		0x0004		/*burst size 2 words*/
#define DMA_BSIZ1		0x0002		/*burst size 2 words*/
#define DMA_BSIZH		0x0001		/*burst size 2 words*/
#define DMA_BSIZ0		0x0000		/*burst size 1 word default*/
#define DMA_NBSIZ		4			/*number of burst sizes*/
#define	DMA_INIT		(DMA_DUMP|DMA_FIXED|DMA_FCP1|DMA_FCP0|DMA_BSIZ16|DMA_BSIZ8|DMA_BSIZ4|DMA_BSIZ2|DMA_BSIZ1|DMA_BSIZH|DMA_BSIZ0)

/*DMA Control register 2*/

#define DMA_ABTVLD	0x80
#define DMA_ABTABT	0x40
#define DMA_ABTEAC	0x20
#define DMA_ABTERR	0x10
#define DMA_ABTQ	0x08
#define DMA_LSBSEL	0x04
#define DMA_TCAN	0x02
#define DMA_RCAN	0x01

/*DMA operation macros*/

#define INITDMA(cd)	WRITE_BIGA16(DCTRL, (UINT16)(DMA_INIT))
#define INITDMA_DUMP(cd)	WRITE_BIGA16(DCTRL, (UINT16)(DMA_INIT_DUMP))

#define STARTDMA(cd) \
	WRITE_BIGA16(DCTRL, (UINT16)(DMA_INIT|DMA_TXA|DMA_TXEN|DMA_RXA|DMA_RXEN)); 

#define STARTTXDMA(cd) \
{ UINT16 d; \
	d = READ_BIGA(DCTRL); \
	d |= (UINT16)(DMA_INIT|DMA_TXA|DMA_TXEN); \
	WRITE_BIGA16(DCTRL, d); \
}

#define STARTRXDMA(cd) \
{ UINT16 d; \
	d = READ_BIGA(DCTRL); \
	d |= (UINT16)(DMA_INIT|DMA_RXA|DMA_RXEN); \
	WRITE_BIGA16(DCTRL, d); \
}

#define STOPTXDMA(cd)	\
{ UINT16 d; \
	d = READ_BIGA(DCTRL); \
	d &= ~(DMA_TXA|DMA_TXEN)); \
	WRITE_BIGA16(DCTRL, d); \
}
#define STOPRXDMA(cd)	\
{ UINT16 d; \
	d = READ_BIGA(DCTRL); \
	d &= ~(DMA_RXA|DMA_RXEN)); \
	WRITE_BIGA16(DCTRL, d); \
}

/* Burst sizing states */

#define S_OFFLINE		0			/*Have not attempted burst sizing*/
#define S_SIZING		1			/*Trying burst sizes*/
#define S_ONLINE		2			/*Determined burst sizes*/

#define BURSTBUFSIZE	96			/*amount of data to test burst size on sbus*/

/*DMA Status register*/

#define DS_QERR		0x01			/*queue select error*/
#define DS_AERR		0x02			/*acknowledge error*/
#define DS_SEAC2	0x04			/* SEAC2 set */
#define DS_FRMER	0x08			/*	Frame error */
#define DS_MSABT	0x10			/* abort */
#define DS_MSVLD	0x20			/* invalid frame */

/*DMA Filter Control register*/

#define DF_VOID		0x01		/*void frames*/
#define DF_LLCS		0x02		/*sync llc frames*/
#define DF_IMPS		0x04		/*sync implementor frames*/
#define DF_SMT		0x08		/*smt frames*/
#define DF_MAC		0x10		/*mac frames*/
#define DF_LLCA		0x20		/*async llc frames*/
#define DF_IMPA		0x40		/*async implementor frames*/
#define DF_FUT		0x80		/*destination future frames*/

/* CFG Bus Configuration register */

#define CFG_S16		0x01
#define CFG_S32		0x02
#define CFG_RISC	0x04
#define CFG_SSWAP	0x08
#define CFG_MSWAP	0x10
#define CFG_MCAA	0x20
#define CFG_STRM	0x40


/*Formac Descripter Bit Masks*/

#define FD_MORE			0x80000000
#define FD_RESV			0x40000000
#define FD_TXFBB1		0x10000000
#define FD_TXFBB0		0x08000000
#define FD_TXFBBSHIFT	27
#define F_LSB			0			/*Assume Formac LSB Bit is OFF*/
#define FD_XDONE		0x04000000
#define FD_NFCS			0x02000000
#define FD_XMTABT		0x01000000


/*Formac Receive frame status word definitions*/

#define FR_MSVALID		0x8000		/*valid frame*/
#define FR_MSRABT		0x4000		/*receive aborted*/
#define FR_SSRCRTG		0x1000		/*source routing, MSB of SA set*/
#define FR_SEAC_E		0x0800		/* E indicator received*/
#define FR_SEAC_A		0x0400		/* A indicator received*/
#define FR_SEAC_C		0x0200		/* C indicator received*/
#define FR_SFRMERR		0x0100		/* frame error occurred*/
#define FR_SADRRG		0x0080		/* mac address match*/
#define FR_SFRMTY2		0x0040		/* frame type bit 2*/
#define FR_SFRMTY1		0x0020		/* frame type bit 1*/
#define FR_SFRMTY0		0x0010		/* frame type bit 0*/
#define FR_ERFBB1		0x0002		/* end of frame byte boundary*/
#define FR_ERFBB0		0x0001		/* end of frame byte boundary*/

#define FR_LENMASK		0x0000ffff	/*length mask*/
#define FR_STATMASK		0xffff0000	/*status mask*/
#define FR_STATSHIFT	16			/*status shift count*/

/*Frame Type Bits*/

#define FT_MASK			(FR_SFRMTY0 | FR_SFRMTY1 | FR_SFRMTY2)
#define ASYNC_SMT_VOID	(0)
#define ASYNC_LLC		(FR_SFRMTY0)
#define ASYNC_IMP		(FR_SFRMTY1)
#define SYNC_MAC		(FR_SFRMTY2)
#define SYNC_LLC		(FR_SFRMTY0 | FR_SFRMTY2)
#define SYNC_IMP		(FR_SFRMTY1 | FR_SFRMTY2)

#define EAC_MASK		(FR_SEAC_E | FR_SEAC_A | FR_SEAC_C)


/*
	SBUGA MACROs
*/

#define BIGA_PUTSOFT_RBD() \
		(softrhead = softrhead->next)

#define BIGA_INSTALL_RBD(fp) \
{ \
	register RBD *nextrbd; \
	nextrbd =((RBD *)(fd->sbuga->rtail_lo | (fd->sbuga->rtail_hi<<16)))->next; \
	nextrbd->fp = (UINT32 *)fp; \
	fd->sbuga->rtail_lo = (UINT16)(((UINT32)nextrbd) & 0xffff); \
	fd->sbuga->rtail_hi = (UINT16)((((UINT32)nextrbd) & 0xffff0000) >> 16); \
}


/*
**
**	Macros for reading/writing the BIGA.
**
*/

#define READ_BIGA8(offset)			\
		(*((volatile UINT8 *)(BIGA16_REGBASE + ((offset) << 4))))

#define READ_BIGA16(offset)			\
		(*((volatile UINT16 *)(BIGA16_REGBASE + ((offset) << 4))))

#define READ_BIGA(offset)			\
		(*((volatile UINT32 *)(BIGA32_REGBASE + ((offset) << 4))))

#define WRITE_BIGA16(offset,value)	\
*(volatile UINT16 *)(BIGA16_REGBASE + ((offset) << 4)) = ((UINT16)(value));


/*
#define READ_BIGA32_RHEAD(var)	{\
var = 0xA9020000 | (UINT32)(*(volatile UINT16 *)(BIGA32_REGBASE + (RHEAD << 4)));\
		}

#define READ_BIGA32_THEAD(var)	{\
var = 0xA9030000 | (UINT32)(*(volatile UINT16 *)(BIGA32_REGBASE + (THEAD << 4)));\
		}
*/


#define READ_BIGA32(offset,var)	{\
		(var) =  (*(volatile UINT32 *)(BIGA32_REGBASE + ((offset) << 4)));\
		}

#define WRITE_BIGA32(offset,value)	{\
		*(volatile UINT32 *)(BIGA32_REGBASE + ((offset) << 4)) = (value);\
		}


#define WRITETTAIL(value)	{*(volatile UINT32 *)\
		(BIGA32_REGBASE + (TTAIL << 4)) = ((UINT32)(value));}

#define WRITERTAIL(value)	{*(volatile UINT32 *)\
		(BIGA32_REGBASE + (RTAIL << 4)) = ((UINT32)(value));}

#define GET_TBD(bd, new) {\
    register UINT32 nexttail, ttail; \
	if (new) \
    	ttail = (UINT32)(new); \
	else \
    	ttail = (UINT32)(softttail - 8); \
    nexttail = (UINT32)(((tBIGATD *)ttail)->bigatd_pNext); \
    if ((UINT32)(&(((tBIGATD *)nexttail)->bigatd_pNext)) == softthead) \
        (bd) = NULLP;\
    else \
    (bd) = (tBIGATD *)nexttail;\
}

#define GET_TBD3(bd, new, th) {\
    register UINT32 nexttail, ttail; \
	if (new) \
    	ttail = (UINT32)(new); \
	else \
    	ttail = (UINT32)(softttail - 8); \
    nexttail = (UINT32)(((tBIGATD *)ttail)->bigatd_pNext); \
    if ((UINT32)(&(((tBIGATD *)nexttail)->bigatd_pNext)) == (th)) \
        (bd) = NULLP;\
	else \
    (bd) = (tBIGATD *)nexttail;\
}


#define GET_TBD2X(bd, bd2) {\
    	bd = (tBIGATD *) (*((UINT32 *) softttail)); \
    	bd2 = bd->nextp; \
}

#define GET_TBD3X(bd, bd2, bd3) {\
    	bd = (tBIGATD *) (*((UINT32 *) softttail)); \
    	bd2 = bd->nextp; \
    	bd3 = bd2->nextp; \
}

#define INSTALL_TBD(n) {\
    register tBIGATD *ttail;\
    register INT32 cnt = n; \
    ttail = (tBIGATD *)(softttail - 8); \
    while ( cnt-- ) \
        ttail = ttail->bigatd_pNext; \
    softttail = (UINT32)&ttail->bigatd_pNext; \
    *(volatile UINT32 *)(BIGA16_REGBASE + (TTAIL << 4)) = ((UINT32)&ttail->bigatd_pNext); \
}

#define INSTALL_RBD(bb) {\
    register RBD *nextrbd; \
    nextrbd = *((RBD **)softrtail); \
    nextrbd->fp = (UINT32 *)(bb); \
    *(UINT32 *)(BIGA16_REGBASE + (RTAIL << 4)) = ((UINT32)&nextrbd->nextp); \
    softrtail = (UINT32)&nextrbd->nextp; \
}


#define PUT_RBD(n) { \
    register RBD *rbdp; \
	register int cnt = n; \
    extern RBD *RBDstart, *RBDend; \
    rbdp = (RBD *)(srhead+4); \
    if (rbdp == RBDend) \
       rbdp = RBDstart; \
    while (--cnt) { \
        rbdp++; \
        if (rbdp == RBDend) \
           rbdp = RBDstart; \
    } \
    srhead = (UINT32)&rbdp->nextp; \
}

#endif /*_bigaregh_*/


/*
**
**	PROTOTYPES
**
*/
#ifdef LANGUAGE_C

extern UINT32 softthead, softrhead, softrtail, softttail;
extern	tBIGARD *RBDstart, *RBDend;
extern	tBIGATD *TBDstart, *TBDend;

extern UINT32 biga_init(void);

#endif /* language_c */

#endif	/* bigah */
