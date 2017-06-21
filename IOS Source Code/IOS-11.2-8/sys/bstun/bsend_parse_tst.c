/* $Id: bsend_parse_tst.c,v 3.4.18.5 1996/09/05 05:36:18 jbalestr Exp $
 * $Source: /release/112/cvs/Xsys/bstun/bsend_parse_tst.c,v $
 *------------------------------------------------------------------
 * Parser for the BSEND test BSC commands
 *
 * Oct 95. Saritha Roach
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: bsend_parse_tst.c,v $
 * Revision 3.4.18.5  1996/09/05  05:36:18  jbalestr
 * CSCdi68144:  bsend size larger than mtu causes memory corruption
 * Branch: California_branch
 *
 * Revision 3.4.18.4  1996/08/13  02:15:46  gstovall
 * CSCdi39484:  Giant packets output on ethernet interface
 * Branch: California_branch
 * Make sure we drop giants, even it they are just a wee bit too big.
 *
 * Revision 3.4.18.3  1996/06/17  08:30:09  jhernand
 * Commit Modular Cobra to California_branch
 *
 * Revision 3.4.18.2  1996/05/17  10:40:47  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.4.4.2  1996/04/11  08:19:39  jbalestr
 * CSCdi51807:  need a test command to transmit raw bisync frames
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.4.4.1  1996/04/03  13:50:26  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.4.18.1  1996/03/18  19:01:19  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.1.32.2  1996/03/13  01:12:08  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.1.32.1  1996/02/20  00:19:45  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.4  1996/02/07  16:10:16  widmer
 * CSCdi48285:  Remove function name from bad_parser_subcommand
 *
 * Revision 3.3  1996/02/01  06:00:11  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.2  1996/01/29  07:26:33  hampton
 * Remove unnecessary includes of parser definition files.  [CSCdi47717]
 *
 * Revision 3.1  1995/11/09  11:04:08  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/10/10  07:55:52  sroach
 * CSCdi41879:  Add BSC test command support
 *
 * Revision 2.1  1995/10/10  05:43:16  sroach
 * *** empty log message ***
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <ctype.h>
#undef   toupper			/* use library function call */
#include <string.h>
#include <ciscolib.h>
#include "sys_registry.h"
#include "media_registry.h"
#include "interface_private.h"
#include "../if/network.h"
#include "packet.h"
#include "logger.h"
#include "config.h"
#include "parser.h"
#include "parser_defs_bsend_tst.h"

#include "bstun_externs.h"
#include "bstun.h"
#include "bsc_lcb.h"
#include "bsc_cmd_set.h"
#include "bsc_conv_table.h"
#include "bsc_lack_support.h"
#include "bsc.h"

/*---- Constants. */

#define BSEND_CMD_PAK_SIZE             50
#define BSEND_DATA_CNTL_SIZE           6


/*---- global declared in bstun_tst.c - this peer's name */
extern ipaddrtype bstun_name;

extern int
get_bcc(uchar *data, uint data_len, uint bcc_length);

/*----------------------------------------------------------------------------*/
/* bsend_queue_network_event                                                  */
/*----------------------------------------------------------------------------*/

static void
bsend_queue_network_event (lcb_t* lcb,
                           uchar cu_addr,
                           uchar cu_cmd,
                           uchar cu_devaddr)
{
   paktype* pak;

   pak = getbuffer(BSEND_CMD_PAK_SIZE);
   if (pak) {
       pak->if_output = lcb->idb->firstsw;
       pak->datagramsize = BSC_HEADER_SIZE;
       pak->datagramstart -= BSC_HEADER_SIZE;
       *(pak->datagramstart+2) = cu_devaddr; /* Device address. */
       *(pak->datagramstart+1) = cu_cmd;     /* Command. */
       *pak->datagramstart = cu_addr;        /* CU address. */

       bevent_network(lcb->idb, pak); 
   }
}

/*---------------------------------------------------------------------------*/
/* bsend_queue_network_data                                                  */
/*---------------------------------------------------------------------------*/

static void
bsend_queue_network_data (lcb_t* lcb,
                          uchar cu_addr,
                          uchar cu_devaddr,
                          uchar *data)
{
   paktype* pak;
   uchar *pakdata, *startdata;
   uint datalen = strlen(data);
   int crc;

   pak = getbuffer(BSC_HEADER_SIZE + BSEND_DATA_CNTL_SIZE +
                           2 + datalen);
   if (pak) {
       pak->if_output = lcb->idb->firstsw;
       pak->datagramstart -= BSC_HEADER_SIZE;

       pakdata = pak->datagramstart;
       *pakdata++ = cu_addr;             /* CU address. */
       *pakdata++ = ((lcb->poll_role == PR_POLLEE)
                               ? POLL_DATA_ETX : SELECT_DATA_ETX);
       *pakdata++ = cu_devaddr;      /* Device address. */
       startdata = pakdata;
       *pakdata++ = lcb->bsc_cs_table[CS_DLE];
       *pakdata++ = lcb->bsc_cs_table[CS_STX];
       *pakdata++ = cu_addr;
       *pakdata++ = cu_devaddr;
       bcopy(data, pakdata, datalen);
       pakdata += datalen;
       *pakdata++ = lcb->bsc_cs_table[CS_DLE];
       *pakdata++ = lcb->bsc_cs_table[CS_ETX];
       crc = get_bcc(startdata, pakdata - startdata, 2);
       *pakdata++ = crc % 256;
       *pakdata++ = crc/256;
       pak->datagramsize = pakdata - pak->datagramstart;
       bevent_network(lcb->idb, pak); 
   }
}


/*--------------------------------------------------------------------------*/
/* bsend_queue_network_raw                                                  */
/*--------------------------------------------------------------------------*/

static void
bsend_queue_network_raw (lcb_t* lcb, 
                         uchar cu_addr, 
                         uchar *data)
{
   paktype* pak;
   int i , datalen = (strlen(data)/2);

   pak = getbuffer(BSC_HEADER_SIZE + datalen);
   if (pak) {
       pak->if_output = lcb->idb->firstsw;
       pak->datagramsize = BSC_HEADER_SIZE + datalen;
       for( i=0; i<datalen; i++){
          pak->datagramstart[i] = atohex(data);
          data += 2;
       }
       pak->datagramstart -= BSC_HEADER_SIZE;

       /* write in the NSC control header */
       pak->datagramstart[2] = cu_addr;      /* Device address. */
       pak->datagramstart[1] = (lcb->poll_role == PR_POLLEE) 
                             ? POLL_DATA_ETX : SELECT_DATA_ETX;  /* Command. */
       pak->datagramstart[0] = cu_addr;             /* CU address. */
       bevent_network(lcb->idb, pak); 
   }
}

/*---------------------------------------------------------------------------*/
/* bsend_queue_network_size                                                  */
/*---------------------------------------------------------------------------*/

static void
bsend_queue_network_size (lcb_t* lcb,
                          uchar cu_addr,
                          uint size)
{
   paktype* pak;
   uchar *pakdata, *startdata;
   int crc , i , datasize, overhead;

   /*
    * frame size includes control bytes and BCC characters
    */
   overhead = BSEND_DATA_CNTL_SIZE + 2;

   if (size < overhead) {
       size = overhead;
   }
   datasize = size - overhead;

   pak = getbuffer(BSC_HEADER_SIZE + size);
   if (pak) {
       pak->if_output = lcb->idb->firstsw;
       pak->datagramstart -= BSC_HEADER_SIZE;

       pakdata = pak->datagramstart;
       *pakdata++ = cu_addr;             /* CU address. */
       *pakdata++ = ((lcb->poll_role == PR_POLLEE)
                  ? POLL_DATA_ETX : SELECT_DATA_ETX);
       *pakdata++ = cu_addr;             /* Device address. */
       startdata = pakdata;
       *pakdata++ = lcb->bsc_cs_table[CS_DLE];
       *pakdata++ = lcb->bsc_cs_table[CS_STX];
       *pakdata++ = cu_addr;
       *pakdata++ = cu_addr;
       for(i=0;i<datasize;i++) {
           *pakdata++ = 0x5A;
       }
       *pakdata++ = lcb->bsc_cs_table[CS_DLE];
       *pakdata++ = lcb->bsc_cs_table[CS_ETX];
       crc = get_bcc(startdata, pakdata-startdata, 2);
       *pakdata++ = crc % 256;
       *pakdata++ = crc/256;
       pak->datagramsize = pakdata - pak->datagramstart;
       bevent_network(lcb->idb, pak); 
   }
}

/*
 * bsend_command
 * 
 */
void
bsend_command (parseinfo *csb)
{
    hwidbtype *idb = hwidb_or_null(csb->interface);
    bstun_protocol_info* bpi;
    lcb_t* lcb = idb->lcb;
    uchar cu_addr,cu_devaddr;
 
    if (!bstun_name) {
	if (!csb->nvgen) {
	    printf("\nBSTUN not enabled - give a bstun peer-name first.");
	}
	return;
    }
    if ((idb->status & IDB_SERIAL) == 0) {
	if (!csb->nvgen) {
	    printf("\nBSEND only allowed on Serial interfaces");
	}
	return;
    }
    if (idb->enctype != ET_BSTUN && csb->sense) {
	if (!csb->nvgen) {
	    printf("\nBSEND requires BSTUN encapsulation on interface");
	}
	return;
    }

    if (idb->bstun_group == 0) {
	printf("\nA bstun group must be defined for this interface before using this command.");
	return;
    }

    bpi = bstun_pinfo[idb->bstun_group];
    if (bpi == NULL) {
	printf("\nNo protocol-group defined for group %d",
	           idb->bstun_group);
	return;
    }

    if (!bsc_query_local_ack_capable(idb)) {
        printf("\nBSEND command is only valid if a bsc-local-ack capable protocol is configured");
        return;
    }

   if (bpi->validate_address) {
      if (!(*bpi->validate_address)(idb, GETOBJ(paddr,1)->bstun_addr,
				    GETOBJ(paddr,1)->length)) {
          printf("\nInvalid control unit address on BSEND command");
	  return;
       }
    }
    bcopy((char *)GETOBJ(paddr,1)->bstun_addr, (char *)&cu_addr, 1);

    switch (csb->which) {

      case BSEND_CMD_ADD:
        if (lcb->poll_role == PR_POLLEE) {
	    printf("\nBSEND ADD command is not valid to BSC SECONDARY interface");
	    return;
        }
        bsend_queue_network_event(lcb, cu_addr, POLL_ADDR_ADD, 0);
        break;

      case BSEND_CMD_DELETE:
        if (lcb->poll_role == PR_POLLEE) {
	  printf("\nBSEND DELETE command is not valid to BSC SECONDARY interface");
	  return;
        }
        bsend_queue_network_event(lcb, cu_addr, POLL_ADDR_DELETE, 0);
        break;

      case BSEND_CMD_ACTIVE:
        bsend_queue_network_event(lcb, cu_addr, POLL_ADDR_ACTIVE, 0);
        break;

      case BSEND_CMD_INACTIVE:
        bsend_queue_network_event(lcb, cu_addr, POLL_ADDR_INACTIVE, 0);
        break;

      case BSEND_CMD_DATA:
        if (!bsc_cucb_find(lcb, cu_addr)) {
            printf("\ncontrol unit address not found on this interface - cannot send data");
	    return;
        }

        cu_devaddr = GETOBJ(int,2);
        if (bpi->validate_address) {
            if (!(*bpi->validate_address)(idb, GETOBJ(paddr,2)->bstun_addr,
				          GETOBJ(paddr,2)->length)) {
                printf("\nInvalid device address on BSEND command");
	        return;
            }
        }
        bcopy((char *)GETOBJ(paddr,2)->bstun_addr, (char *)&cu_devaddr, 1);
        
        bsend_queue_network_data(lcb, cu_addr, cu_devaddr, GETOBJ(string,1));
        break;

      case BSEND_CMD_RAW:
        
        if (!bsc_cucb_find(lcb, cu_addr)) {
            printf("\ncontrol unit address not found on this interface - cannot send data");
	    return;
        }
        bsend_queue_network_raw(lcb, cu_addr, GETOBJ(string,1));
        break;

      case BSEND_CMD_SIZE:
        
        if (!bsc_cucb_find(lcb, cu_addr)) {
            printf("\ncontrol unit address not found on this interface - cannot send data");
	    return;
        }
        bsend_queue_network_size(lcb, cu_addr, GETOBJ(int,1));
        break;

      default:
	bad_parser_subcommand(csb, csb->which);
	break;
    }

}  /* bsend_command. */
