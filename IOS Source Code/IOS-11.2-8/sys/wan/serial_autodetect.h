/* $Id: serial_autodetect.h,v 3.1.2.1 1996/07/28 07:33:19 irfan Exp $
 * $Source: /release/112/cvs/Xsys/wan/serial_autodetect.h,v $
 *------------------------------------------------------------------
 * serial_autodetect.h   header file for serial encaps autodetect
 *                       feature
 *
 * July 1996, Syed Mohammad Irfan Ashraf
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: serial_autodetect.h,v $
 * Revision 3.1.2.1  1996/07/28  07:33:19  irfan
 * CSCdi60568:  v120 autodetect not working
 * Branch: California_branch
 * allow serial interfaces to autodetect encaps on the wire for incoming
 * connections.
 *
 * Revision 3.1  1996/07/28  07:22:12  irfan
 * files needed for serial autodetect.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Serial Autodetect constants
 */
#define SERIAL_AUTODETECT_MAX_ATTEMPTS 5   /* max paks to check at a time */
#define SERIAL_AUTODETECT_MAX_TIME (10*ONESEC) /*  until assuming failure */


/*
 * Bits for Autodetect parsing. These are kept in
 * allowed_encaps field of ser_autodetect_info struct.
 */
#define AUTODETECT_PARSE_PPP          0x0001
#define AUTODETECT_PARSE_V120         0x0002
#define AUTODETECT_PARSE_FR           0x0004
#define AUTODETECT_PARSE_HDLC         0x0008
#define AUTODETECT_PARSE_X25          0x0010
#define AUTODETECT_PARSE_SMDS         0x0020

#define MAX_AUTODETECT_ENC_TYPES 6

typedef struct autodetect_type {

    encapstype enctype;        /* ET_xx for this encaps from h/encapstype.h */
    uint parse_bit;            /* AUTODETECT_PARSE_xx  from above  */
    char *name;                /* encaps name for parsing. may be  */
		               /*	different from encstring() */
                               /* vector to test this encaps       */
    boolean (*test_this_encaps_pak)(hwidbtype *, paktype *);
                               /* encaps start vector              */
    boolean (*switch_to_this_encaps)(hwidbtype *);

} autodetect_type;

/*
 * serial autodetect information
 * each serial interface doing autodetect
 * will keep a pointer
 */
struct ser_autodetect_info_ {

    /*
     * saved vectors & config information
     */
    iqueue_t       save_iqueue;       /* saved default iqueue vector */
    parse_packet_t save_parse_packet; /* saved parse_packet vector */
    encapstype     saved_enctype;     /* saved encapsulation */
    linestate_t    saved_linestate;   /* saved linestate vector */
    reset_t        saved_reset;       /* saved reset vector */
    ulong          allowed_encaps;    /* AUTODETECT_PARSE_xx bits configured */
    short          pak_count;         /* count of paks tested */
    mgd_timer      fail_timer;	      /* failure timer */
};

/*
 * get_ser_autodetect_info
 *
 * return the serial autodetect info block for the
 * given hwidb
 */
static inline ser_autodetect_info 
  *get_ser_autodetect_info (hwidbtype *hwidb)

{
    serialsb *ssb;

    ssb = idb_use_hwsb_inline(hwidb, HWIDB_SB_SERIAL);
    if (ssb) {
	return (ssb->ser_autodetect);
    }
    return (NULL);
}
