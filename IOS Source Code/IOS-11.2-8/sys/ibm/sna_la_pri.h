/* $Id: sna_la_pri.h,v 3.2 1995/11/17 09:24:11 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/ibm/sna_la_pri.h,v $
 *------------------------------------------------------------------
 * sna_la_pri.h   SNA Local LU Address Prioritization 
 *
 * March 1992,  Anson Chen
 *
 * Copyright (c) 1992-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: sna_la_pri.h,v $
 * Revision 3.2  1995/11/17  09:24:11  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:46:22  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:48:34  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __SNA_LA_PRI_H__
#define __SNA_LA_PRI_H__

#define COS_RESEQ_QLEN 32 /* max size of COS resequence queue */

#define SNA_FID    0xF0
#define SNA_FID4   0x40
#define SNA_TPF    0x03
#define SNA_TG_SWEEP 0x08
#define SNA_NET_PRIORITY  0x01
#define TG_SNF     0x0FFF
#define SNA_HDR_OFFSET           2
#define TG_MAX_KEEP_ORDER        9
#define TG_MAX_RESEQ_QSIZE       TG_MAX_KEEP_ORDER
#define TG_MAX_LINKS             8
#define TG_MAX_SEQ_NUM      0x0FFF

#define SNA_LA_PRI_MAXLIST   11
#define SNA_LU_ADDR_LENGTH  2

#define TPF_LOW     0x00
#define TPF_MEDIUM  0x01
#define TPF_HIGH    0x02

/* Format of a part (the part we need...) of SNA FID4 header */
typedef struct _sna_fid4_hdr {
    uchar fid_tg;
    uchar rsrvd;
    uchar ern;
    uchar priority;
    ushort snf;          /* sequence number field */
} sna_fid4_hdr;

/* Type define for local lu address prioritization */
typedef struct la_pritype_ {
    struct la_pritype_ *next;
    uchar linkcode;
    uchar priority;
    idbtype *interface;
    uchar lu_address[SNA_LU_ADDR_LENGTH];
        uchar param_mask;
        uchar dmac[IEEEBYTES];
        uchar smac[IEEEBYTES];
        uchar dsap;
        uchar ssap;
        uchar sdlc;
        uchar sdlc_addr;
} la_pri_type;

#define SAP_PRI_MAXLIST  11
#define SSAP_MASK 0x01
#define DSAP_MASK 0x02
#define SMAC_MASK 0x04
#define DMAC_MASK 0x80

/* Type define for rsrb sap prioritization */
typedef struct sap_pritype_ {
    struct sap_pritype_ *next;
    uchar linkcode;
    uchar priority;
    idbtype *interface;
    uchar param_mask;
    uchar dmac[IEEEBYTES];
    uchar smac[IEEEBYTES];
    uchar dsap;
    uchar ssap;
} sap_pri_type;

/* function defines */
extern int fid2_pri_classify(hwidbtype *, uchar *, uchar *);
extern void locaddr_pri_command(parseinfo *csb);
extern void locaddr_pri_list_command(parseinfo *csb);
extern void snalapri_init(void);
extern int stun_addr_pri_tcp(hwidbtype *, paktype *, int *);
extern boolean prilist_uses_stun_group(uchar);
extern int rsrb_fid2_pri_classify(hwidbtype *, uchar *, tring_hdr *, sap_hdr *);
extern int general_fid2_pri_classify (hwidbtype *, uchar *, uchar *, uchar *,
                                     uchar *, uchar *);
extern int sap_pri_classify(hwidbtype *, tring_hdr *, sap_hdr *);
extern int general_sap_pri_classify (hwidbtype *, uchar *, uchar *,
                                     uchar *, uchar *);
extern void sap_pri_list_command (parseinfo *);
extern void sap_pri_command (parseinfo *);

/*
 * sna_la_pri_chain.c
 */
extern void sna_parser_init(subsystype *subsys);

#endif __SNA_LA_PRI_H__
