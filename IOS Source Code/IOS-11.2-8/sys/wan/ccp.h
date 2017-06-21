/* $Id: ccp.h,v 3.3.56.2 1996/05/01 14:33:21 fox Exp $
 * $Source: /release/112/cvs/Xsys/wan/ccp.h,v $
 *------------------------------------------------------------------
 * ccp.h - PPP Compression Control Protocol.
 *
 * October 1994, George M. Taylor
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ccp.h,v $
 * Revision 3.3.56.2  1996/05/01  14:33:21  fox
 * CSCdi55358:  PPP subsystem needs to be more modular
 * Branch: California_branch
 * Make PPP NCPs into real subsystems.  Add them to all images where
 * PPP and the related network protocol are located.  NCPs are dependant
 * on PPP and the related network protocol subsystem but not vice versa.
 * Remove dependancies between PPP and the compression code.
 *
 * Revision 3.3.56.1  1996/04/23  20:26:51  gtaylor
 * CSCdi41791:  Memory corruption crash. Workspace for PPP compression is
 * now based on current link max MTU size.
 * Branch: California_branch
 *
 * Revision 3.3  1995/11/23  00:54:28  gtaylor
 * CSCdi43498:  PPP Stacker compression requires support for LCB check
 * method. This fix should guarantee that we will be able to work with
 * Combinets PPP Stac compression and other vendors supporting the LCB
 * check method.
 *
 * Revision 3.2  1995/11/17  18:00:44  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:47:47  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:16:58  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * ppp.c - Point-to-Point Protocol.
 *
 * Copyright (c) 1989 Carnegie Mellon University.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms are permitted
 * provided that the above copyright notice and this paragraph are
 * duplicated in all such forms and that any documentation,
 * advertising materials, and other materials related to such
 * distribution and use acknowledge that the software was developed
 * by Carnegie Mellon University.  The name of the
 * University may not be used to endorse or promote products derived
 * from this software without specific prior written permission.
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 */

typedef struct stacker
{
    int          negotiate;
    uchar        type;
    uchar        length;
    ushort       history_count;
    uchar        check_mode;
} stacker;

typedef struct stacker_dcp
{
    int          negotiate;
    uchar        type;
    uchar        length;
    ushort       history_count;
    uchar        check_mode;
    uchar        process_mode;
} stacker_dcp;

typedef struct predictor1
{
    int      negotiate;
    uchar    type;
    uchar    length;
} predictor1;

typedef struct ccp_options
{
    int          negotiate_compression;
    uchar        user_preference;
    uchar        negotiated_type;
    stacker      lzs;
    stacker_dcp  lzsdcp;
    predictor1   pred1;
} ccp_options;

#define CCP_NEGOTIABLE_COMPRESSION        1
#define CCP_NON_NEGOTIABLE_COMPRESSION    0

/*
 * Compression Types
 */
#define CI_OUI                   0
#define CI_PREDICTOR1            1
#define CI_PREDICTOR2            2
#define CI_PUDDLEJMP             3
#define CI_UNASSIGNED4           4
#define CI_UNASSIGNED5           5
#define CI_UNASSIGNED6           6
#define CI_UNASSIGNED7           7
#define CI_UNASSIGNED8           8
#define CI_UNASSIGNED9           9
#define CI_UNASSIGNED10          10
#define CI_UNASSIGNED11          11
#define CI_UNASSIGNED12          12
#define CI_UNASSIGNED13          13
#define CI_UNASSIGNED14          14
#define CI_UNASSIGNED15          15
#define CI_UNASSIGNED16          16
#define CI_STACKER               17
#define CI_MIRCOSOFTPPC          18
#define CI_GANDALFFZA            19
#define CI_V42BIS                20
#define CI_BSDLZW                21
#define CI_UNASSIGNED22          22
#define CI_LZSDCP                23
#define CI_MAX_COMPRESSION_TYPES 24
#define CI_COMPRESSION_NONE    CI_MAX_COMPRESSION_TYPES

/*
 * Stacker LZS
 */
#define LEN_CI_LZS            5

#define LZS_MAX_HISTORIES     1

#define LZS_CHECK_MODE_NONE  0
#define LZS_CHECK_MODE_LCB   1
#define LZS_CHECK_MODE_CRC   2
#define LZS_CHECK_MODE_SEQ   3
#define LZS_CHECK_MODE_EXTND 4
#define LZS_CHECK_MODE_EXTND  4

#define LZS_SEQ_NUM_INIT      1
#define LZS_LCB_NUM_INIT      0xFF
#define LZS_ID_NUM_INIT       0

typedef struct stacHistory
{
    uchar   check_mode;
    uchar   seq_num;
    uchar   reset_id;
    boolean reset_requested;
    boolean reset_acked;
    boolean legacy_support;
    int     wrk_spc_size;
    uchar   *history;
} stacHistory;

/*
 * Stacker compress stops before reaching zero workspace bytes remaining.
 * Adding this constant to the true length makes it use every last byte.
 */
#define CCP_LZS_COMPRESS_FUDGE    (LZS_DEST_MIN + 1)

#define LENCISTACKER(neg)     (neg ? LEN_CI_LZS : 0)

#define ADDCISTACKER(options) \
    if (options.negotiate)\
    {\
    PUTCHAR(options.type, ucp); \
    PUTCHAR(options.length, ucp); \
    PPP_PUTSHORT(options.history_count, ucp); \
    PUTCHAR(options.check_mode, ucp); \
    if (ppp_negotiation_debug) \
      buginf("\n%s:CCP: sending CONFREQ, type = %d (LZS), value = %d/%d", \
             f->idb->hw_namestring, \
             options.type, options.history_count, options.check_mode); \
    }


#define ACKCISTACKER(options,result) \
    if (options.negotiate) \
    { \
    if ((len -= options.length) < 0) \
        goto bad; \
    GETCHAR(citype, p); \
    GETCHAR(cilen, p); \
    if (cilen != options.length || citype != options.type) \
        goto bad; \
    PPP_GETSHORT(cishort, p); \
    if (options.history_count != cishort) \
      goto bad; \
    GETCHAR(cichar, p); \
    if (options.check_mode != cichar) \
      goto bad; \
    result = CI_STACKER; \
    if (ppp_negotiation_debug) \
      buginf("\n%s:CCP: config ACK received, type = %d (LZS), value = %d/%d", \
             f->idb->hw_namestring, \
             options.type, options.history_count, options.check_mode);\
    }


#define NAKCISTACKER(options, code) \
    if (options.negotiate && \
    len >= options.length && \
    p[1] == options.length && \
    p[0] == options.type) { \
    len -= options.length; \
    INCPTR(2, p); \
    PPP_GETSHORT(history_count, p); \
    GETCHAR(check_mode, p); \
    code \
    }


#define REJCISTACKER(options, reject) \
    if (options.negotiate && \
    len >= options.length && \
    p[1] == options.length && \
    p[0] == options.type) { \
    len -= options.length; \
    INCPTR(2, p); \
    PPP_GETSHORT(cishort, p); \
    /* Check rejected value. */ \
    if (cishort != options.history_count) \
        goto bad; \
    GETCHAR(cichar, p); \
    /* Check rejected value. */ \
    if (cichar != options.check_mode) \
        goto bad; \
    if (ppp_negotiation_debug) \
        buginf("\n%s:CCP: config REJ received, type = %d (LZS), value = %d/%d", \
               f->idb->hw_namestring, \
               options.type, cishort, cichar); \
    options.negotiate = CCP_NON_NEGOTIABLE_COMPRESSION; \
    reject = CI_STACKER; \
    }


/*
 * LZS DCP - Stac Electronics LZS compression with Motorola's DCP header.
 */
#define LEN_CI_LZSDCP             6

#define LZSDCP_MAX_HISTORIES      1

#define LZSDCP_CHECK_MODE_NONE    0
#define LZSDCP_CHECK_MODE_LCB     1
#define LZSDCP_CHECK_MODE_SEQ     2
#define LZSDCP_CHECK_MODE_SEQ_LCB 3  /* Default checkmode */

#define LZSDCP_PROCESS_MODE_NONE  0
#define LZSDCP_PROCESS_MODE_PROC  1

#define LZSDCP_SEQ_NUM_INIT       1

#define LZS_DCP_HEADER_FIXED      0x80
#define LZS_DCP_COMPRESSED        0x40
#define LZS_DCP_RESET_REQ         0x20
#define LZS_DCP_RESET_ACK         0x10

#define LZS_DCP_BLOCK_RESET       0x01

typedef struct lzsdcpHistory
{
    uchar            check_mode;
    uchar           seq_num;
    uchar            flags;
    sys_timestamp    ResetAckTimer;
    int             wrk_spc_size;
    uchar           *history;
} lzsdcpHistory;

#define LENCILZSDCP(neg)     (neg ? LEN_CI_LZSDCP : 0)

#define ADDCILZSDCP(options) \
    if (options.negotiate)\
    {\
    PUTCHAR(options.type, ucp); \
    PUTCHAR(options.length, ucp); \
    PPP_PUTSHORT(options.history_count, ucp); \
    PUTCHAR(options.check_mode, ucp); \
    PUTCHAR(options.process_mode, ucp); \
    if (ppp_negotiation_debug) \
     buginf("\n%s:CCP: sending CONFREQ, type = %d (LZS DCP), value = %d/%d/%d", \
            f->idb->hw_namestring, \
            options.type, options.history_count, options.check_mode, \
            options.process_mode); \
    }


#define ACKCILZSDCP(options,result) \
    if (options.negotiate){ \
    if ((len -= options.length) < 0) \
        goto bad; \
    GETCHAR(citype, p); \
    GETCHAR(cilen, p); \
    if (cilen != options.length || citype != options.type) \
        goto bad; \
    PPP_GETSHORT(cishort, p); \
    if (options.history_count != cishort) \
      goto bad; \
    GETCHAR(cichar, p); \
    if (options.check_mode != cichar) \
      goto bad; \
    GETCHAR(cichar, p); \
    if (options.process_mode != cichar) \
      goto bad; \
    result = CI_LZSDCP; \
    if (ppp_negotiation_debug) \
      buginf("\n%s:CCP: config ACK received, type = %d (LZSDCP), value = %d/%d/%d", \
               f->idb->hw_namestring, \
             options.type, options.history_count, options.check_mode, \
             options.process_mode); \
    }


#define NAKCILZSDCP(options, code) \
    if (options.negotiate && \
    len >= options.length && \
    p[1] == options.length && \
    p[0] == options.type) { \
    len -= options.length; \
    INCPTR(2, p); \
    PPP_GETSHORT(history_count, p); \
    GETCHAR(check_mode, p); \
    GETCHAR(process_mode, p); \
    code \
    }


#define REJCILZSDCP(options, reject) \
    if (options.negotiate && \
    len >= options.length && \
    p[1] == options.length && \
    p[0] == options.type) { \
    len -= options.length; \
    INCPTR(2, p); \
    PPP_GETSHORT(cishort, p); \
    /* Check rejected value. */ \
    if (cishort != options.history_count) \
        goto bad; \
    GETCHAR(cichar, p); \
    /* Check rejected value. */ \
    if (cichar != options.check_mode) \
        goto bad; \
    GETCHAR(cichar, p); \
    /* Check rejected value. */ \
    if (cichar != options.process_mode) \
        goto bad; \
    if (ppp_negotiation_debug) \
        buginf("\n%s:CCP: config REJ received, type = %d (LZSDCP)", \
                f->idb->hw_namestring, \
                options.type); \
    options.negotiate = CCP_NON_NEGOTIABLE_COMPRESSION; \
    reject = CI_LZSDCP; \
    }



/*
 * Predictor 1
 */
#define LEN_CI_PRED1    2

#define PRED1_ID_NUM_INIT    0x0

#define PRED1_LEN_MASK         0x7FFF
#define PRED1_COMP_BIT_MASK    0x8000

#define CCP_PRED1_HEADERBYTES   2
#define CCP_PRED1_TAILBYTES     2

#define PRED1_HASH_MASK_INIT    0x0

#define PRED1_PUTCRC16(s, cp) { \
    *(cp)++ = (s) & 0xff; \
    *(cp)++ = (s) >> 8; \
}

#define PRED1_GETCRC16(s, cp) { \
    (s) = *(cp)++; \
    (s) |= *(cp)++ << 8; \
}

typedef struct pred1History
{
    ushort  hash;
    uchar   reset_id;
    boolean reset_requested;
    boolean legacy_support;
    int     wrk_spc_size;
    uchar   *guess_table;
} pred1History;

#define LENCIPRED1(neg)    (neg ? LEN_CI_PRED1 : 0)

#define ADDCIPRED1(options) \
    if (options.negotiate)\
    {\
    PUTCHAR(options.type, ucp); \
    PUTCHAR(options.length, ucp); \
    if (ppp_negotiation_debug) \
      buginf("\n%s:CCP: sending CONFREQ, type = %d (PRED1)", \
               f->idb->hw_namestring, \
               options.type); \
    }


#define ACKCIPRED1(options,result) \
    if (options.negotiate) \
    { \
    if ((len -= options.length) < 0) \
        goto bad; \
    GETCHAR(citype, p); \
    GETCHAR(cilen, p); \
    if (cilen != options.length || citype != options.type) \
        goto bad; \
    result = CI_PREDICTOR1; \
    if (ppp_negotiation_debug) \
      buginf("\n%s:CCP: config ACK received, type = %d (PRED1)", \
               f->idb->hw_namestring, \
               options.type); \
    }


#define NAKCIPRED1(options, code) \
    if (options.negotiate && \
    len >= options.length && \
    p[1] == options.length && \
    p[0] == options.type) { \
    len -= options.length; \
    INCPTR(2, p); \
    code \
    }

#define REJCIPRED1(options, reject) \
    if (options.negotiate && \
    len >= options.length && \
    p[1] == options.length && \
    p[0] == options.type) { \
    len -= options.length; \
    INCPTR(2, p); \
    if (ppp_negotiation_debug) \
        buginf("\n%s:CCP: config REJ received, type = %d (PRED1)", \
               f->idb->hw_namestring, \
               options.type); \
    options.negotiate = CCP_NON_NEGOTIABLE_COMPRESSION; \
    reject = CI_PREDICTOR1; \
    }
