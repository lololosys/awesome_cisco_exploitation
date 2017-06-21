/* $Id: transmit_test.c,v 3.6.20.5 1996/08/26 15:09:16 ppearce Exp $
 * $Source: /release/112/cvs/Xsys/if/transmit_test.c,v $
 *------------------------------------------------------------------
 * transmit_test.c -- Packet generator for testing purposes
 *
 * December 1991 - David R. Hampton
 * 
 * Copyright (c) 1991-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: transmit_test.c,v $
 * Revision 3.6.20.5  1996/08/26  15:09:16  ppearce
 * CSCdi66582:  Implement SRB vector table to cut size of SRB_CORE subsys
 * Branch: California_branch
 *
 * Revision 3.6.20.4  1996/07/09  05:58:58  ppearce
 * CSCdi59527:  inlines waste code space
 * Branch: California_branch
 *   Just say "no" to inline abuse - SRB subblock inlines removed
 *
 * Revision 3.6.20.3  1996/05/21  09:50:13  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.6.20.2  1996/05/17  11:23:48  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.6.6.2  1996/04/26  15:20:37  ppearce
 * IBU modularity - SRB subblock (part 1)
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.6.6.1  1996/04/03  14:39:13  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.6.20.1  1996/03/18  20:14:54  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.4.14.2  1996/03/07  09:44:41  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.4.14.1  1996/02/20  00:57:40  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.6  1996/02/01  06:04:05  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.5  1996/01/29  07:28:21  hampton
 * Remove unnecessary includes of parser definition files.  [CSCdi47717]
 *
 * Revision 3.4  1995/12/17  18:28:54  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.3  1995/11/17  09:31:17  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:39:11  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:52:51  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:55:12  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include <string.h>
#include <ctype.h>
#undef   toupper			/* use library function call */
#include <ciscolib.h>
#include "interface_private.h"
#include "packet.h"
#include "ttysrv.h"
#include "config.h"
#include "../if/ether.h"
#include "../if/tring.h"
#include "../if/fddi.h"
#include "../if/network.h"
#include "../if/transmit_test.h"
#include "sys_registry.h"

#include "parser.h"

#include "../parser/parser_actions.h"
#include "../parser/macros.h"

#include "../srt/srb_core.h"

/*
 * test transmit
 */
#define	ALTERNATE NONE
#include "exec_test_transmit.h"
LINK_POINT(transmit_test_commands, ALTERNATE);
#undef	ALTERNATE


static ulong testframesize = 100;
static uchar def_tr_dst[IEEEBYTES]    = {0x10, 0x00, 0x5a, 0x01, 0x00, 0x0c};
static uchar def_fddi_dst[IEEEBYTES]  = {0x00, 0x00, 0x0c, 0x99, 0x99, 0x99};
static uchar def_ether_dst[IEEEBYTES] = {0x00, 0x00, 0x0c, 0x99, 0x99, 0x99};


/**********************************************************************
 *
 * 			       ETHERNET
 *
 **********************************************************************/

/*
 *
 */
static void build_ether_header_by_hand (hwidbtype *idb, paktype **pakp)
{
    paktype *pak = *pakp;
    char *args, buf[BUFLEN];
    int c;

    /*
     * Get Destination Address
     */
    printf("Specify destination MAC: ");
    READ_OR_EXIT;
    if (c == 'Y') {
	printf("48 bit MAC address: ");
	READ_OR_EXIT;
	if (!parse_ether_address(args, pak->datagramstart)) {
	    printf("bad address\n");
	    retbuffer(pak);
	    *pakp = NULL;
	    return;
	}
    } else {
	ieee_copy(def_ether_dst, pak->datagramstart);
    }
    
    /*
     * Get Source Address
     */
    printf("Specify source MAC: ");
    READ_OR_EXIT;
    if (c == 'Y') {
	printf("48 bit MAC address: ");
	READ_OR_EXIT;
	if (!parse_ether_address(args, pak->datagramstart + IEEEBYTES)) {
	    printf("bad address\n");
	    retbuffer(pak);
	    *pakp = NULL;
	    return;
	}
    } else {
	ieee_copy(idb->hardware, pak->datagramstart + IEEEBYTES);
    }
    
    pak->datagramsize = 2 * IEEEBYTES;
    *pakp = pak;
}

static void build_ether_header (hwidbtype *idb, paktype **pakp)
{
    paktype *pak;
    char *args, buf[BUFLEN];
    int c;

    pak = getbuffer(testframesize);
    if (!pak) {
	*pakp = NULL;
	return;
    }
    pak->datagramstart = pak->network_start;
    memset(pak->datagramstart, 0, testframesize);
    pak->flags |= PAK_HEARSELF;
    pak->if_output = idb->firstsw;

    printf("1) To this interface\n");
    printf("2) To another interface\n");
    printf("9) Ask for everything\n");
    printf("Choice: ");
    READ_OR_EXIT;
    switch (c) {
      case '1':
	ieee_copy(idb->hardware, pak->datagramstart);
	ieee_copy(def_ether_dst, pak->datagramstart + IEEEBYTES);
	pak->datagramsize = 2 * IEEEBYTES;
	*pakp = pak;
	return;
	
      case '2':
	ieee_copy(def_ether_dst, pak->datagramstart);
	ieee_copy(idb->hardware, pak->datagramstart + IEEEBYTES);
	pak->datagramsize = 2 * IEEEBYTES;
	*pakp = pak;
	return;
	
      case '9':
	build_ether_header_by_hand(idb, &pak);
	*pakp = pak;
	return;
    }
    *pakp = NULL;
}

/**********************************************************************
 *
 * 			      TOKEN RING
 *
 **********************************************************************/

/*
 *
 */
static void build_token_header_by_hand (hwidbtype *idb, paktype **pakp)
{
    paktype *pak = *pakp;
    char *args, buf[BUFLEN];
    int c;
    ushort us, *sptr;
    trrif_t *trh;
    trh = (trrif_t *)pak->datagramstart;

    /*
     * Get Access Control byte.
     */
    printf("Specify AC byte: ");
    READ_OR_EXIT;
    if (c == 'Y') {
	printf("ac byte: ");
	READ_OR_EXIT;
	get_hex_num(args, &us);
	trh->ac = (uchar) us;
    } else {
    	trh->ac = AC_PRI0;
    }

    /*
     * Get Frame Control byte.
     */
    printf("Specify FC byte: ");
    READ_OR_EXIT;
    if (c == 'Y') {
	printf("fc byte: ");
	READ_OR_EXIT;
	get_hex_num(args, &us);
	trh->fc = (uchar) us;
    } else {
	trh->fc = FC_TYPE_LLC;
    }
    
    /*
     * Get Destination Address
     */
    printf("Specify destination MAC: ");
    READ_OR_EXIT;
    if (c == 'Y') {
	printf("48 bit MAC address: ");
	READ_OR_EXIT;
	if (!parse_ether_address(args, trh->daddr)) {
	    printf("bad address\n");
	    retbuffer(pak);
	    *pakp = NULL;
	    return;
	}
    } else {
	ieee_copy(def_tr_dst, trh->daddr);
    }
    
    /*
     * Get Source Address
     */
    printf("Specify source MAC: ");
    READ_OR_EXIT;
    if (c == 'Y') {
	printf("48 bit MAC address: ");
	READ_OR_EXIT;
	if (!parse_ether_address(args, trh->daddr)) {
	    printf("bad address\n");
	    retbuffer(pak);
	    *pakp = NULL;
	    return;
	}
    } else {
	ieee_copy(idb->hardware, trh->saddr);
    }
    
    /*
     * Get RIF field
     */
    sptr = (ushort *)&trh->rc_blth;
    printf("Include RIF field: ");
    READ_OR_EXIT;
    if (c == 'Y') {
	printf("Enter RIF field: ");
	READ_OR_EXIT;
	
	sptr = (ushort *)&trh->rc_blth;
	while (args && !term_char(*args)) {
	    args = get_hex_num(args, sptr);
	    sptr++;
	    if (term_char(*args)) {
		break;
	    }
	    args++; /* skip delimiter */
	}
	if (sptr != (ushort *)&trh->rc_blth) {
	    trh->saddr[0] |= TR_RII;
	    sptr = (ushort *)((uchar *)&trh->rc_blth +
			      (trh->rc_blth & RC_LTHMASK));
	}
    }
    
    pak->datagramsize = (uchar *)sptr - pak->datagramstart;
    *pakp = pak;
}

static void build_token_header (hwidbtype *idb, paktype **pakp)
{
    paktype *pak = *pakp;
    int c;
    char *args, buf[BUFLEN];
    trrif_t  *trh;
    srb_triplet_t srb_triplet;

    pak = getbuffer(testframesize);
    if (!pak) {
	*pakp = NULL;
	return;
    }
    pak->datagramstart = pak->network_start;
    memset(pak->datagramstart, 0, testframesize);
    pak->if_output = idb->firstsw;
    pak->flags |= PAK_SRB_EXPLORER;
    trh = (trrif_t *) pak->datagramstart;

    printf("1) To this interface,    RIF not present \n");
    printf("2) To this interface,    RIF, no match   \n");
    printf("3) To this interface,    RIF, match      \n");
    printf("4) To another interface, RIF not present \n");
    printf("5) To another interface, RIF (e), no match   \n");
    printf("6) To another interface, RIF (e), match      \n");
    printf("7) To another interface, RIF (o), no match   \n");
    printf("8) To another interface, RIF (o), match      \n");
    printf("9) Ask for everything\n");
    printf("Choice: ");
    READ_OR_EXIT;
    switch (c) {
      case '1':
	trh->ac = AC_PRI0;
	trh->fc = FC_TYPE_LLC;
	ieee_copy(idb->hardware, trh->daddr);
	ieee_copy(def_tr_dst,    trh->saddr);
	pak->datagramsize = TRING_ENCAPBYTES;
	*pakp = pak;
	return;
	
      case '2':
	trh->ac = AC_PRI0;
	trh->fc = FC_TYPE_LLC;
	ieee_copy(idb->hardware, trh->daddr);
	ieee_copy(def_tr_dst,    trh->saddr);
	trh->saddr[0] |= TR_RII;
	trh->rc_blth = 0x0A;
	trh->rc_dlf = RC_LF8144;
	trh->rd[0] = 0xAAA1;
	trh->rd[1] = 0xBBB2;
	trh->rd[2] = 0xCCC3;
	trh->rd[3] = 0xDDD0;
	pak->datagramsize = TRING_ENCAPBYTES + 0x0A;
	*pakp = pak;
	return;
	
      case '3':
	trh->ac = AC_PRI0;
	trh->fc = FC_TYPE_LLC;
	ieee_copy(idb->hardware, trh->daddr);
	ieee_copy(def_tr_dst,    trh->saddr);
	trh->saddr[0] |= TR_RII;
	trh->rc_blth = 0x0A;
	trh->rc_dlf = RC_LF8144;
	trh->rd[0] = 0xAAA1;
        srbV.srb_sb_get_triplet(idb->firstsw, &srb_triplet);
	trh->rd[1] = ((srb_triplet.localRing & 0x0FFF) << 4) |
	                    (srb_triplet.bridgeNum & 0x000F);
	trh->rd[2] = ((srb_triplet.remoteRing & 0x0FFF) << 4) | 0x0002;
	trh->rd[3] = 0xDDD0;
	pak->datagramsize = TRING_ENCAPBYTES + 0x0A;
	*pakp = pak;
	return;
	
      case '4':
	trh->ac = AC_PRI0;
	trh->fc = FC_TYPE_LLC;
	ieee_copy(def_tr_dst,    trh->daddr);
	ieee_copy(idb->hardware, trh->saddr);
	pak->datagramsize = TRING_ENCAPBYTES;
	*pakp = pak;
	return;
	
      case '5':
	trh->ac = AC_PRI0;
	trh->fc = FC_TYPE_LLC;
	ieee_copy(def_tr_dst,    trh->daddr);
	ieee_copy(idb->hardware, trh->saddr);
	trh->saddr[0] |= TR_RII;
	trh->rc_blth = 0x08;
	trh->rc_dlf = RC_LF8144;
	trh->rd[0] = 0xAAA1;
	trh->rd[1] = 0xBBB2;
	trh->rd[2] = 0xCCC0;
	pak->datagramsize = TRING_ENCAPBYTES + 0x0A;
	*pakp = pak;
	return;
	
      case '6':
	trh->ac = AC_PRI0;
	trh->fc = FC_TYPE_LLC;
	ieee_copy(def_tr_dst,    trh->daddr);
	ieee_copy(idb->hardware, trh->saddr);
	trh->saddr[0] |= TR_RII;
	trh->rc_blth = 0x08;
	trh->rc_dlf = RC_LF8144;
	trh->rd[0] = 0xAAA1;
        srbV.srb_sb_get_triplet(idb->firstsw, &srb_triplet);
	trh->rd[1] = ((srb_triplet.localRing & 0x0FFF) << 4) |
   	                    (srb_triplet.bridgeNum & 0x000F);
        trh->rd[2] = ((srb_triplet.remoteRing & 0x0FFF) << 4) | 0x0000;
	pak->datagramsize = TRING_ENCAPBYTES + 0x0A;
	*pakp = pak;
	return;
	
      case '7':
	trh->ac = AC_PRI0;
	trh->fc = FC_TYPE_LLC;
	ieee_copy(def_tr_dst,    trh->daddr);
	ieee_copy(idb->hardware, trh->saddr);
	trh->saddr[0] |= TR_RII;
	trh->rc_blth = 0x0A;
	trh->rc_dlf = RC_LF8144;
	trh->rd[0] = 0xAAA1;
	trh->rd[1] = 0xBBB2;
	trh->rd[2] = 0xCCC3;
	trh->rd[3] = 0xDDD0;
	pak->datagramsize = TRING_ENCAPBYTES + 0x0A;
	*pakp = pak;
	return;
	
      case '8':
	trh->ac = AC_PRI0;
	trh->fc = FC_TYPE_LLC;
	ieee_copy(def_tr_dst,    trh->daddr);
	ieee_copy(idb->hardware, trh->saddr);
	trh->saddr[0] |= TR_RII;
	trh->rc_blth = 0x0A;
	trh->rc_dlf = RC_LF8144;
	trh->rd[0] = 0xAAA1;
        srbV.srb_sb_get_triplet(idb->firstsw, &srb_triplet);
	trh->rd[1] = ((srb_triplet.localRing & 0x0FFF) << 4) |
	                    (srb_triplet.bridgeNum & 0x000F);
	trh->rd[2] = ((srb_triplet.remoteRing & 0x0FFF) << 4) | 0x0002;
	trh->rd[3] = 0xDDD0;
	pak->datagramsize = TRING_ENCAPBYTES + 0x0A;
	*pakp = pak;
	return;
	
      case '9':
	build_token_header_by_hand(idb, &pak);
	*pakp = pak;
	return;
    }
    *pakp = NULL;
}

/**********************************************************************
 *
 * 				 FDDI
 *
 **********************************************************************/

static void build_fddi_header_by_hand (hwidbtype *idb, paktype **pakp)
{
    printf("Not written yet.\n");
    datagram_done(*pakp);
    *pakp = NULL;
}

static void build_fddi_header (hwidbtype *idb, paktype **pakp)
{
    paktype *pak;
    int c;
    char *args, buf[BUFLEN];
    trrif_t *trh;

    pak = getbuffer(testframesize);
    if (!pak) {
	*pakp = NULL;
	return;
    }
    pak->datagramstart = pak->network_start;
    memset(pak->datagramstart, 0, testframesize);
    pak->if_output = idb->firstsw;
    pak->flags |= PAK_SRB_EXPLORER;
    trh = (trrif_t *) pak->datagramstart;
    trh->fc = FDDI_LLC_FC_BYTE;

    printf("1) To this interface\n");
    printf("2) To another interface \n");
    printf("9) Ask for everything\n");
    printf("Choice: ");
    READ_OR_EXIT;
    switch (c) {
      case '1':
	ieee_copy(idb->hardware, trh->daddr);
	ieee_copy(def_fddi_dst,    trh->saddr);
	pak->datagramsize = TRING_ENCAPBYTES;
	*pakp = pak;
	return;
	
      case '2':
	ieee_copy(def_fddi_dst,    trh->daddr);
	ieee_copy(idb->hardware, trh->saddr);
	pak->datagramsize = TRING_ENCAPBYTES;
	*pakp = pak;
	return;
	
      case '9':
	build_fddi_header_by_hand(idb, &pak);
	*pakp = pak;
	return;
    }
    *pakp = NULL;
}

static void build_serial_header (hwidbtype *idb, paktype **pakp)
{
    paktype *pak;
/*
    int c;
    char *args, buf[BUFLEN];
*/

    pak = getbuffer(testframesize);
    if (!pak) {
	*pakp = NULL;
	return;
    }
    pak->datagramstart = pak->network_start;
    memset(pak->datagramstart, 0, testframesize);
    pak->if_output = idb->firstsw;

    printf("Not written yet.\n");
    datagram_done(pak);
    *pakp = NULL;
    return;
}

/**********************************************************************
 *
 *			    Encapsulation
 *
 **********************************************************************/

static int get_encap_type (hwidbtype *idb)
{
    char *args, buf[BUFLEN];
    int c;

    do {
	printf("\nEncapsulation Type:\n");
	if (idb->status & IDB_ETHER)
	    printf("1) Ethertype\n");
	if (!(idb->status & IDB_FDDI))
	    printf("2) SAP\n");
	printf("3) SNAP\n");
	printf("4) SNAP (Cisco OUI)\n");
	printf("5) SNAP (EtherV2 OUI)\n");
	if (idb->status & IDB_ETHER)
	    printf("6) Novell 802.3\n");
	printf("Choice: ");
	READ_OR_EXIT3;

	
	if (((c == '1') || (c == '6')) && !(idb->status & IDB_ETHER)) {
	    printf("Only valid on Ethernet interfaces.");
	    continue;
	}
	if ((c == '2') && (idb->status & IDB_FDDI)) {
	    printf("Not valid on FDDI interfaces.");
	    continue;
	}
    } while ((c < '1') || (c > '6'));
    return(c);
}

/**********************************************************************
 *
 * 				Generic
 *
 **********************************************************************/

/*
 *
 */
void transmit_frames (parseinfo *csb)
{
    idbtype *idb;
    hwidbtype *hwidb;
    paktype *pak;
    char *args, buf[BUFLEN];
    int c, encap;
    ulong i;
    boolean parsed = FALSE;

    idb = NULL;
    hwidb = NULL;
    pak = NULL;
    while (TRUE) {
	while (!idb) {
	    printf("interface: ");
	    READ_OR_EXIT2;
	    parse_interface(args, &idb);
	    if (idb) {
		hwidb = idb->hwptr;
		if (!(hwidb->status & (IDB_ETHER | IDB_TR |
				       IDB_FDDI | IDB_SERIAL))) {
		    printf("\nDon't know the format for that interface.\n");
		    idb = NULL;
		}
	    } else {
		printf("\nMust specify interface.\n");
	    }
	}

	printf("total frame size [%d]: ", testframesize);
	READ_OR_EXIT2;
	i = parse_unsigned(args, &parsed);
	if (parsed)
	    testframesize = i;

	while (TRUE) {
	    if (pak) {
		printf("repeat frame [y]: ");
		READ_OR_EXIT2;
		if (c == 'N') {
		    datagram_done(pak);
		    pak = NULL;
		    break;
		}
		pak_lock(pak);
		datagram_out(pak);
		continue;
	    }

	    /*
	     * Get MAC header (sans protocol type).
	     */
	    if (hwidb->status & IDB_ETHER)
		build_ether_header(hwidb, &pak);
	    else if (hwidb->status & IDB_TR)
		build_token_header(hwidb, &pak);
	    else if (hwidb->status & IDB_FDDI)
		 build_fddi_header(hwidb, &pak);
	    else
		build_serial_header(hwidb, &pak);
	    if (!pak) {
		break;
	    }

	    /*
	     * get encapsulation type
	     */
	    encap = get_encap_type(hwidb);

	    /*
	     * Get protocol type and data
	     */
	    printf("\nProtocol type:");
	    for (c='0';  c<= '9'; c++) {
		reg_invoke_transmit_frames_protocol_print(c);
	    }
	    printf("\nChoice: ");
	    READ_POP;
	    reg_invoke_transmit_frames_protocol(c, idb, &pak,
						encap, testframesize);

	    if (pak) {
		if ((hwidb->status & IDB_ETHER) &&
		    (pak->datagramsize < MINETHERSIZE)) {
		    pak->datagramsize = MINETHERSIZE;
		}
		pak->if_output = idb;
		pak_lock(pak);
		datagram_out(pak);
	    }
	}
    }
}

static void default_transmit_frames_protocol (idbtype *idb, paktype **pak,
					      int encap, ulong testframesize)
{
    printf("Unknown protocol\n");
    datagram_done(*pak);
    *pak = NULL;
}

static void default_transmit_frames_protocol_print (void)
{
    printf("\n9) Ask for everything");
}

void transmit_test_add_commands (void)
{
    reg_add_transmit_frames_protocol('9', default_transmit_frames_protocol,
				     "default_transmit_frames_protocol");
    reg_add_transmit_frames_protocol_print('9',
				default_transmit_frames_protocol_print,
			       	"default_transmit_frames_protocol_print");
    parser_add_commands(PARSE_ADD_TEST_CMD,&pname(transmit_test_commands),
			"transmit");
}
