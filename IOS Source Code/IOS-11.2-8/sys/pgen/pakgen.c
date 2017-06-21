/* $Id: pakgen.c,v 3.3.18.2 1996/07/23 13:27:49 thille Exp $
 * $Source: /release/112/cvs/Xsys/pgen/pakgen.c,v $
 *------------------------------------------------------------------
 * pakgen.c -- MAC packet generator 
 *
 * February 1992, Anson Chen
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: pakgen.c,v $
 * Revision 3.3.18.2  1996/07/23  13:27:49  thille
 * CSCdi63329: Centralize logging for getbuffer() and pak_duplicate()
 * failures
 * Branch: California_branch
 * Shamelessly rip off Mr. Katz' code from CSCdi36037.  Provide
 * centralized logging for getbuffer() and pak_duplicate() failures and
 * then rip out the 9 bazillion different messages that where in the
 * various callers to report the failure.
 *
 * Revision 3.3.18.1  1996/03/18  21:35:14  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.24.2  1996/03/07  10:41:00  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.2.24.1  1996/02/20  16:58:15  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.3  1996/02/08  20:13:26  thille
 * CSCdi48347: format string in inlines, code size impact
 * Pass 1 - Clean up some literal strings in inlines, duplicate literals,
 * etc.  11,356 bytes saved on gs7-j-m, 9336 bytes saved on igs-j-l
 *
 * Revision 3.2  1995/11/17  18:56:24  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:59:03  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/09/14  01:52:01  biren
 * CSCdi40312:  Move pakgen and pgen (Pagent 2.2) to seperate subsystems
 *
 * Revision 2.2  1995/06/09  13:18:03  hampton
 * Rename old scheduler calls to fit the new naming convention.
 * [CSCdi35514]
 *
 * Revision 2.1  1995/06/07 22:34:06  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <string.h>
#include <ctype.h>
#undef   toupper			/* use library function call */
#include "interface_private.h"
#include "subsys.h"
#include "packet.h"
#include "ttysrv.h"
#include "../if/ether.h"
#include "../if/fddi.h"
#include "../if/tring.h"
#include "../if/network.h"
#include "../os/buffers.h"
#include "pakgen.h"
#include "../ui/common_strings.h"


/*
 * Definitions
 */

#define	MAXLEN	  80			/* command buffer length */
#define PAKGEN_LLC_MIN 3		/* DATA which includes LLC must be
					   at least 3 bytes long */ 

boolean pakgen_ok = FALSE;


/*
 * Pakgen subsystem header
 */

#define PAKGEN_MAJVERSION  1
#define PAKGEN_MINVERSION  0
#define PAKGEN_EDITVERSION 1

SUBSYS_HEADER(pakgen, PAKGEN_MAJVERSION, PAKGEN_MINVERSION, PAKGEN_EDITVERSION,
	      pakgen_init, SUBSYS_CLASS_PROTOCOL,
	      NULL,
	      NULL);


/*
 * pakgen_init
 */

void pakgen_init (subsystype *subsys)
{
    if (!pakgen_ok)
	pakgen_ok = TRUE;
    pakgen_parser_init();
}

/*
 * pakgen_proto 
 * Generate packets on a MAC media interface.
 *
 */

void pakgen_proto (hwidbtype *idb)
{
    uchar mac_address[IEEEBYTES];
    uchar rif[SRB_MAXRIF];
    int   riflen=0;
    char *ptr;
    long size, repeat, seconds;
    long i;
    char buff[MAXLEN];
    char hexbuff[MAXLEN];
    ulong gen_ticks;
    boolean gooddata = FALSE;

    /*
     * Set default parameters.
     */
    size = 100;			/* default data size, including LLC2 hearder */
    repeat = 5;			/* default repeat count */
    seconds = 2;		/* pause between packets in seconds */

    /*
     *  get target MAC address
     */
     printf("Target MAC address: ");
     if(!rdtty(buff,MAXLEN) || !parse_ether_address(buff, mac_address)) {
	 printf("bad MAC address\n");
	 return;
     } 

     if (idb->status & IDB_TR) {
        /*                  
         * get rif-string  
         */
        printf("RIF-string: ");
        if(rdtty(buff, MAXLEN)) { 
	   memset(rif, 0, SRB_MAXRIF);
       	   ptr = buff;
           if (*ptr == '-') {
		ptr++;
	   } else if ((*ptr >= '0' && *ptr <= '9') ||
                        (toupper(*ptr) >= 'A' && toupper(*ptr) <= 'F')) {
                     for(riflen = 0; riflen < SRB_MAXRIF; riflen += 2) {
                        ptr = get_hex_num(ptr, (ushort *)&rif[riflen]);
                        if (term_char(*ptr)) {
                           riflen += 2;
                           break;
                        }
                       ptr++;
                     }
                  }
        }

       /*
        * Sanity check
        */
       if (riflen && ((rif[0] & RC_LTHMASK) != riflen)) {
          printf("length mismatch in RIF or too big");
          return;
       }
     }  

   /*
    * Ask for repeat count, size, and pause interval
    */
    if (!pakgen_common(&repeat,&size, PAKGEN_LLC_MIN,
	MAXDATAGRAMSIZE, &seconds))
    return; 
    /* 
     * Get Data, including LLC2 header
     */
     while(!gooddata) {
         memset(buff, 0, MAXLEN);
         memset(hexbuff, 0, MAXLEN);
         printf("Data (hex): ");
         if (!rdtty(buff, MAXLEN)) {
	     return;
	 }
	 if (null(buff)) {
	     printf("\nNull string entered\n");
  	 }
	 /* Translate the buffer into a series of hex bytes... */
	 else {
             if(pakhexval(buff, hexbuff))
		gooddata = TRUE;
	 }
     }
    /*
     * Go for it.
     */
    psion(stdio);
    printf(type_escape_seq_abort_msg);
    printf("\nSending %d, %d-byte packets to %i, pause is %d seconds:",
	     repeat, size, mac_address, seconds);
    flush();
    gen_ticks = seconds * ONESEC;
    for (i = 0; (i < repeat) && (psipending() == FALSE); i++) {
	printf("!");
	if(idb->status & IDB_TR)
	      send_tr_pak(idb, mac_address, rif, riflen, hexbuff, size);
        else if (idb->status & IDB_ETHER)
	      send_ether_pak(idb, mac_address, hexbuff, size);
        else if (idb->status & IDB_FDDI)
	      send_fddi_pak(idb, mac_address, hexbuff, size);
        else
	      printf("Bad interface specification");
	process_sleep_for(gen_ticks);
	flush();
    }	
    psioff(stdio);

}

/*
 * send_tr_pak
 * build a packet for the token ring interface 
 */

void send_tr_pak (
    hwidbtype *idb,
    uchar *mac_address,
    uchar *rif,
    int riflen,
    char *buff,
    long size)
{
    uchar *frameptr; 
    paktype *pak;
    trrif_t *trh;

    /*
     * get a buffer large encough for MAC header, LLC2 and data
     * Actually, the LLC2 is included as part of data entered by
     * the user.
     */
    pak = getbuffer(TRING_ENCAPBYTES+riflen+size);
/*
 * BOGOSITY ALERT!!!  The below code mungs out test_pak without ever
 * having checked that getbuffer returned a valid packet...
 */
    memset(pak->network_start, 0, TRING_ENCAPBYTES + riflen + size);
    pak->datagramstart = pak->network_start - (TRING_ENCAPBYTES  + riflen);
    frameptr = pak->datagramstart;

    /* insert mac addresses */
    trh = (trrif_t *) pak->datagramstart;
    trh->ac = AC_PRI0;
    trh->fc = FC_TYPE_LLC;
    PUTSHORT(&trh->daddr[0], GETSHORT(&mac_address[0]));
    PUTSHORT(&trh->daddr[2], GETSHORT(&mac_address[2]));
    PUTSHORT(&trh->daddr[4], GETSHORT(&mac_address[4]));
    PUTSHORT(&trh->saddr[0], GETSHORT(&idb->hardware[0]));
    PUTSHORT(&trh->saddr[2], GETSHORT(&idb->hardware[2]));
    PUTSHORT(&trh->saddr[4], GETSHORT(&idb->hardware[4]));
    frameptr += TRING_ENCAPBYTES;

    /* insert rif */
    if(riflen > 0) {
       trh->saddr[0] |= TR_RII;
       bcopy(rif, frameptr, riflen);
       frameptr += riflen;
    }

    /*
     * insert data 
     * the user is allowed to enter the first 40 hex bytes
     * including LLC2 header, the rest if filled with 0s
     */
     
    bcopy(buff, frameptr, MAXLEN);
    pak->datagramsize = TRING_ENCAPBYTES + riflen + size;
    pak->if_output = idb->firstsw;

    datagram_out(pak);
    return;
}



/* 
 * define etherner mac header size 
 */
#define G_ETHERHDRSIZE (2*IEEEBYTES)+2


/*
 * send_ether_pak
 * build a packet for the ethernet interface 
 */

void send_ether_pak (
    hwidbtype *idb,
    uchar *mac_address,
    char *buff,
    long size)
{
    uchar *frameptr; 
    paktype *pak;
    ether_hdr *eth;

    /*
     * get a buffer large encough for MAC header, LLC2 and data
     * Actually, the LLC2 is included as part of data entered by
     * the user.
     */
    pak = getbuffer(G_ETHERHDRSIZE+ size);
/*
 * BOGOSITY ALERT!!!  The below code mungs out test_pak without ever
 * having checked that getbuffer returned a valid packet...
 */
    memset(pak->network_start, 0, (G_ETHERHDRSIZE+size));
    pak->datagramstart = pak->network_start - G_ETHERHDRSIZE;
    frameptr = pak->datagramstart;

    /* insert mac addresses */
    eth = (ether_hdr *) pak->datagramstart;
    PUTSHORT(&eth->daddr[0], GETSHORT(&mac_address[0]));
    PUTSHORT(&eth->daddr[2], GETSHORT(&mac_address[2]));
    PUTSHORT(&eth->daddr[4], GETSHORT(&mac_address[4]));
    PUTSHORT(&eth->saddr[0], GETSHORT(&idb->hardware[0]));
    PUTSHORT(&eth->saddr[2], GETSHORT(&idb->hardware[2]));
    PUTSHORT(&eth->saddr[4], GETSHORT(&idb->hardware[4]));
    frameptr += G_ETHERHDRSIZE;

    /*
     * insert data 
     * the user is allowed to enter the first 40 hex bytes
     * including LLC2 header, the rest if filled with 0s
     */
     
    bcopy(buff, frameptr, MAXLEN);
    pak->datagramsize = G_ETHERHDRSIZE + size;
    pak->if_output = idb->firstsw;

    datagram_out(pak);
    return;
}

/*
 * send_fddi_pak
 * build a packet for the fddi interface 
 */

void send_fddi_pak (
    hwidbtype *idb,
    uchar *mac_address,
    char *buff,
    long size)
{
    uchar *frameptr; 
    paktype *pak;
    fddi_hdr *fth;

    /*
     * get a buffer large encough for MAC header, LLC2 and data
     * Actually, the LLC2 is included as part of data entered by
     * the user.
     */
    pak = getbuffer(FDDI_ENCAPBYTES+size);
/*
 * BOGOSITY ALERT!!!  The below code mungs out test_pak without ever
 * having checked that getbuffer returned a valid packet...
 */
    memset(pak->network_start, 0, (FDDI_ENCAPBYTES+size));
    pak->datagramstart = pak->network_start - FDDI_ENCAPBYTES;
    frameptr = pak->datagramstart;

    /* insert mac addresses */
    fth = (fddi_hdr *) pak->datagramstart;
    fth->fc = FDDI_LLC_FC_BYTE;
    PUTSHORT(&fth->daddr[0], GETSHORT(&mac_address[0]));
    PUTSHORT(&fth->daddr[2], GETSHORT(&mac_address[2]));
    PUTSHORT(&fth->daddr[4], GETSHORT(&mac_address[4]));
    PUTSHORT(&fth->saddr[0], GETSHORT(&idb->hardware[0]));
    PUTSHORT(&fth->saddr[2], GETSHORT(&idb->hardware[2]));
    PUTSHORT(&fth->saddr[4], GETSHORT(&idb->hardware[4]));
    frameptr += FDDI_ENCAPBYTES;

    /*
     * insert data 
     * the user is allowed to enter the first 40 hex bytes
     * including LLC2 header, the rest if filled with 0s
     */
     
    bcopy(buff, frameptr, MAXLEN);
    pak->datagramsize = FDDI_ENCAPBYTES + size;
    pak->if_output = idb->firstsw;

    datagram_out(pak);
    return;
}

/*
 * pakgen_common
 * Common pakgen parameter parsing.
 * Returns TRUE if all successfully parsed, FALSE otherwise.
 */

boolean pakgen_common (
    long *repeat,
    long *size,
    long minsize,
    long maxsize,
    long *seconds)
{
    *repeat = get_num_in_range("Repeat count", 10, 1, 0x7FFFFFFF, *repeat);
    *size = get_num_in_range("Datagram size", 10, minsize, maxsize, *size);
    *seconds = get_num_in_range("Timeout in seconds", 10, 0, 3600, *seconds);
    return(TRUE);
}


boolean pakhexval(char *srcstr, char *retstr)
{
    char *str=srcstr;
    unsigned char c = toupper(*str);    
    unsigned char value = 0;
    int i;
 
    while(*str && *(str+1)) {
          
         i =0;
         while (i < 2) {
	     if (((c >='A') && (c<='F')) || ((c>='0') && (c <= '9'))) {
                value = value*16 + (c <= '9' ? (c - '0') : (c - 'A'+10));    
	        str++;
                c = toupper(*str);
                i++;
             }
	     else {
		 printf("\ninvalid hex character entered\n");
		 return(FALSE);
	     }
         }
         *retstr++ = value;
   }
   return(TRUE);
}

/*
 * test_pakgen
 * The packet generator diagnostics 
 * The user is allowd to generate and output packets on 
 * a MAC media interface.
 */

#define PAKGEN_CHARS 40

void test_pakgen (parseinfo *csb)
{
    hwidbtype *idb = NULL;
    idbtype *swidb;
    char buff[PAKGEN_CHARS];

    while (!idb) {
	printf("\noutput interface: ");
	if (rdtty(buff, PAKGEN_CHARS) == FALSE)
	    return;
   	(void) parse_interface(buff, &swidb);
        if (swidb) {
	    idb = swidb->hwptr;
	    if ((idb->status) & (IDB_TR | IDB_ETHER| IDB_FDDI)) {
		pakgen_proto(idb);
            } else {
		printf("%% %s is not a valid MAC media interface", buff);
	    }
	}  else {
	    printf("%% No such network interface");
	    idb = NULL;
	    continue;
        }
	return;
    }
}
