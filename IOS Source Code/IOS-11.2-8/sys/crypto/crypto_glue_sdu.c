/* $Id: crypto_glue_sdu.c,v 3.1.70.13 1996/09/11 23:25:01 bew Exp $
 * $Source: /release/112/cvs/Xsys/crypto/crypto_glue_sdu.c,v $
 *------------------------------------------------------------------
 * crypto_glue_sdu.c - IOS glue routines called from Cylink SDU code.
 *
 * November 1995, Brian Weis
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: crypto_glue_sdu.c,v $
 * Revision 3.1.70.13  1996/09/11  23:25:01  bew
 * CSCdi68438:  crypto determination of network address should be done
 * earlier
 * Branch: California_branch
 * Run the routing tables during connection message setup to determine
 * which interface the packet will go out.
 *
 * Revision 3.1.70.12  1996/09/10  23:28:35  bew
 * CSCdi64360:  Bad and pending connection does not timeout
 * Branch: California_branch
 * Clear out the pending connection table entry as well
 * as connection table entry when manually clearing a connection.
 *
 * Revision 3.1.70.11  1996/08/28  19:29:05  bew
 * CSCdi66328:  crypto connection set too early on CRM side
 * Branch: California_branch
 * Use the pending connection id on the CRM side to tag crypto maps. The
 * pending conn id is replaced by connection id when connection
 * established as on initiating side.
 *
 * Revision 3.1.70.10  1996/08/16  07:30:48  bew
 * CSCdi65769:  pregen does not work on 7500
 * Branch: California_branch
 * Pregeneration for slots other than zero didn't work due to premature
 * exitting.
 *
 * Revision 3.1.70.9  1996/07/26  07:40:11  bew
 * CSCdi64136:  show crypto connections shows unneeded status bits
 * Branch: California_branch
 * Only show some Cylink status bits. The others are not
 * meaningful to users.
 *
 * Revision 3.1.70.8  1996/07/06  19:44:27  xliu
 * CSCdi61999:  decryption packet lost after connection already establish
 * Branch: California_branch
 *
 * Revision 3.1.70.7  1996/07/01  09:05:53  xliu
 * CSCdi61744:  fail to select correct crypto maps when multiple are
 * created
 * Branch: California_branch
 *
 * Revision 3.1.70.6  1996/06/28  23:37:07  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.1.70.5  1996/06/18  00:21:52  bew
 * CSCdi60362:  Crypto router choose 40-des over 56-des
 * Branch: California_branch
 * The strongest DES possible is now negotiated during connection setup.
 *
 * Revision 3.1.70.4  1996/06/07  00:24:07  bew
 * CSCdi59169:  Crypto cleanup work
 * Branch: California_branch
 * Various cleanups & addition of "crypto ping".
 *
 * Revision 3.1.70.3  1996/06/05  23:11:23  bew
 * CSCdi59169:  Crypto cleanup work
 * Branch: California_branch
 * Fix crypto zeroize & key generation for 7500 with VIPs.
 * Fix crypto connection setup with traffic in both directions.
 * Fix encryption bugs.
 *
 * Revision 3.1.70.2  1996/05/12  23:12:16  xliu
 * CSCdi56023:  Fixes made after integrating with turbo IPC, and cached
 * packet mem.
 * Branch: California_branch
 *
 * Revision 3.1.70.1  1996/04/19  14:55:45  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commits
 *
 * Revision 3.1.2.11  1996/04/16  21:52:11  bew
 * Branch: IosSec_branch
 * General code cleanup. Implemented checking of crypto map peers. CIM now
 * addressed to the src/dst rather than pe/upe.
 *
 * Revision 3.1.2.10  1996/04/09  20:02:31  che
 * Branch: IosSec_branch
 * Sync to California_branch (960405)
 *
 * Revision 3.1.2.9  1996/03/25  18:43:39  bew
 * Branch: IosSec_branch
 * Added 40-bit DES and DES 8-bit CFB support. Implemented partial
 * challenge processing support, not yet compatible with SDU.
 * Updated the crypto methods vector per Cylink's new definitions.
 *
 * Revision 3.1.2.8  1996/03/13  22:45:06  bew
 * Branch: IosSec_branch
 * The RSP/VIP environment (multiple crypto engines) are now supported
 * in crypto parser routines and crypto session mgmt code.
 *
 * Revision 3.1.2.7  1996/03/01  01:38:59  carrel
 * Branch: IosSec_branch
 * Crypto map enhancements, parser clarification and LOTS of code cleanup
 * Fixed a bug in access list processing and one in crypto maps.
 *
 * Revision 3.1.2.6  1996/01/25  18:19:51  bew
 * Branch: IosSec_branch
 * Implemented a config option for automatic pre-genereration of
 * Diffie-Hellman key pairs. Also changed the use of timers so
 * that the the STUCKTMR message doesn't show up any more.
 *
 * Revision 3.1.2.5  1996/01/18  23:23:15  bew
 * Branch: IosSec_branch
 * The crypto session manager no longer forks off connection messages but
 * handles them itself serially. This is due to SDU connection setup code
 * not handling the multitasking gracefully.
 *
 * Revision 3.1.2.4  1996/01/13  05:46:42  bew
 * Branch: IosSec_branch
 * Cleanup. Lots of cleanup in the IosSec_branch crypto directory.
 *
 * Revision 3.1.2.3  1995/12/20  23:30:44  bew
 * Branch: IosSec_branch
 * Public key exchange can now be two-way as well as one-way. Context is
 * now freed after a sign operation. Wrap-around of connection ids now
 * supported.  General cleanup.
 *
 * Revision 3.1.2.2  1995/12/12  23:09:29  bew
 * Branch: IosSec_branch
 * Fixes for CS & better formatting of debug messages.
 *
 * Revision 3.1.2.1  1995/11/28  01:46:31  bew
 * Port of newest Cylink SDU code, plus CS cleanup. Cylink files
 * are moved to match their original tree structure.
 * Branch: IosSec_branch
 *
 * Revision 3.1  1995/11/22  23:17:40  bew
 * Placeholders for new locations of crypto parser files
 *
 * Revision 1.7  1995/06/07 12:03:05  smackie
 * Fold back Arizona_branch into mainline.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <ciscolib.h>
#include "packet.h"
#include "ttysrv.h"
#include "interface_private.h"
#include "globs.h"
#include "string.h"
#include "../ip/ip.h"
#include "../ipc/ipc_util.h"
#include "../ipc/ipc.h"
#include "crypto_public.h"
#include "crypto.h"
#include "crypto_engine_api.h"
#include "crypto_map.h"
#include "crypto_sesmgmt.h"
#include "crypto_ip.h"
#include "crypto_conntable.h"
#include "crypto_glue_private.h"
#include "crypto_glue_structs.h"
#include "crypto_glue.h"
#include "crypto_pubkey.h"
#include "../ui/debug.h"
#include "crypto_debug.h"
#include "../os/clock.h"
#include "crypto_glue_sdu.h"

/* 
 * Pointer to the beginning of conn id table 
 */
ulong *cs_unique_conn_id_array = NULL; 

/*
 * The following are pointers to arrays which will get malloc'ed memory. The 
 * size of the array is not known until runtime.
 */
crypto_dh_pregen **crypto_dh_pregen_head_array = NULL;
ushort *crypto_dh_pregen_count_array = NULL;

/*****************************************************************************
 *  Cylink defines from places we don't include yet.                         *
 *****************************************************************************/
 int sduState = SDU_OFFLINE ; 	/* From task/mg/evntmgr.c */

 ushort dssPkeyLen = 64 ;	/* From task/auth/authcy.c */
 ushort dhPublicLen = 64 ;	/* From task/auth/authcy.c */

/* From task/mg/mgdata.c */
uchar sys_sdu_nw_addr_ipx[PDU_NW_ADDR_SIZE] = {0, 0, 0, 0}; /*4 bytes */
uchar sys_sdu_nw_addr_idp[PDU_NW_ADDR_SIZE] = {0, 0, 0, 0}; /*4 bytes */
uchar sys_sdu_nw_addr_ddp[DDP_NW_ADDR_SIZE] = {0, 0}; 	   /*2 bytes */

/*****************************************************************************
 *  Cylink defines from cy_frm.c (which is not yet used)		             *
 *****************************************************************************/

/* types of encryption (frame type I only) */

#ifndef CISCO
#define UNDEFINED	0					/* currently no encryption defined  */
#define FLIPBIT		1					/* flip bit algorithm - for test only  */
#define DES_56      10                  /* 56-bit key, DES encryption, Type I */
#define DESx2_56	15					/* 56-bit key, Dual DES encrypt, Type I */
#define KAPPA_56	20					/* 56-bit key, KAPPA, Type I  */
#else 
/*
 * From the Chuck Williams (Cylink) mail, with additions of cfb-64.
 *
 *     Algorithm      CY_Method Bit       CY_Method Code (hex)
 *     DES-56; 8CFB        8,0                 0A
 *     DES-56; 64CFB       9                   0B
 *     DES-56; 64CBC       --                  0C
 *     DES-56; ECB         --                  0D
 *     DES-40; 8CFB        4                   01
 *     DES-40; 64CFB       5                   02
 *     DES-40; 64CBC       --                  03
 *     DES-40; ECB         --                  04
 *     TDES; 2key; 8CFB    14                  07
 *     CEPA-128            12                  14
 *     CEPA-64             6                   15
 *     IDEA-128            10                  20 
 *     Null                --                  FF
 */
#define UNDEFINED		0x0
#define DES_56_8CFB		0xA
#define DES_56_64CFB		0xB	
#define DES_56_64CBC		0xC
#define DES_56_ECB		0xD
#define DES_40_8CFB		0x1
#define DES_40_64CFB		0x2
#define DES_40_64CBC		0x3
#define DES_40_ECB		0x4
#define DESx3_56		0x7
#define CEPA_128		0x14
#define CEPA_64			0x15
#define IDEA_128		0x20
#define NULL_ENC		0xFF
#endif


/* IV generation macros */

#define IV_SIZE		8					/* byte size of iv generation value */

/******************************************************************************
								PUBLIC GLOBALS
******************************************************************************/

/*=============================== MIB GLOBALS ================================= */

/*------------------------------ CY Status Info ------------------------------- */

ulong	cyReject	= 0L;				/* # frames rejected */
ulong	cyPass		= 0L;				/* # frames passed in clear */
ulong	cyDecrypt	= 0L;	 			/* # frames decrypted */
ulong	cyEncrypt	= 0L;				/* # frames encrypted */

/*---------------------------- CY Error Counters ------------------------------ */

ulong	cyErrBadMode	= 0L;			/* # frames with unknown frameModes */
ulong	cyErrNoMethod	= 0L;			/* # no Method defined */
ulong	cyErrBadMethod	= 0L;			/* Method not supported by this s/w vers */
ulong	cyErrNoFrame 	= 0L;			/* cy_isr() awoken with no frame ptr */
ulong	cyErrOverFlowQ	= 0L;			/* frame queue overflowed */
ulong	cyErrZeroIVgen	= 0L;			/* zero iv generator datum size */
ulong	cyErrNegSize	= 0L;			/* negative encryption size computed */

/*------------------------- Cryptographic Definitions  ------------------------ */

ushort		cyMode = 0x0000;			/* cryption mode (ECB:1-bit,CFB:8-bit) */
/*
 * WARNING! Changes to this bitmap must be reflected in the #defines in
 * crypto.h (e.g., CRYPTO_DES_56).
 */
cyMethod	cyMethodTbl =				/* Method definition table */
{
	/* Static Methods (cannot be modified via MIB - identical for all SDUs) */

	DES_56_8CFB,						/* Method #0, DES 56, 8bit CFB (SDU) */ 
	UNDEFINED,							/* Method #1, undefined */
	UNDEFINED,							/* Method #2, undefined */
	UNDEFINED,							/* Method #3, undefined */
	DES_40_8CFB,						/* Method #4, DES 40 key, 8bit CFB */
	DES_40_64CFB,						/* Method #5, DES 40 key, 64bit CFB */
	CEPA_64,							/* Method #6, CEPA-64 */
	UNDEFINED,							/* Method #7, undefined */
	DES_56_8CFB,						/* Method #8, DES, 56-bit key */
	DES_56_64CFB,						/* Method #9, DES 56 key, 64bit CFB */
	IDEA_128,							/* Method #10, IDEA-128 */
	UNDEFINED,							/* Method #11, undefined */
	CEPA_128,							/* Method #12, CEPA-128 */
	UNDEFINED,							/* Method #13, undefined */
	DESx3_56,							/* Method #14, TDES; 2key; 8CFB */
	UNDEFINED,							/* Method #15, undefined */

	/* Dynamic Methods (defined via the MIB interface - Customer specific) */

	UNDEFINED,							/* Method #16, undefined */
	UNDEFINED,							/* Method #17, undefined */
	UNDEFINED,							/* Method #18, undefined */
	UNDEFINED,							/* Method #19, undefined */
	UNDEFINED,							/* Method #20, undefined */
	UNDEFINED,							/* Method #21, undefined */
	UNDEFINED,							/* Method #22, undefined */
	UNDEFINED,							/* Method #23, undefined */
	UNDEFINED,							/* Method #24, undefined */
	UNDEFINED,							/* Method #25, undefined */
	UNDEFINED,							/* Method #26, undefined */
	UNDEFINED,							/* Method #27, undefined */
	UNDEFINED,							/* Method #28, undefined */
	UNDEFINED,							/* Method #29, undefined */
	UNDEFINED,							/* Method #30, undefined */
	UNDEFINED,							/* Method #31, undefined */
};

/* Cryptographic initialization values.  Note that the offsets (1st and 3rd */
/* columns) are relative to the Layer 3 header.  ALSO, A RESTRICTION ON THIS */
/* TABLE is that the size be a root value of 8 (e.g., 1,2,4,8). */

cyConfig  cyConfigTbl  =		/* init values per protocol stack */
{
	{	0,	0,	 0	},	  		/* #0	undefined */
	{	0,	0,	 0	},	  		/* #1	undefined */
	{	0,	0,	 0 	},	  		/* #2	undefined */
	{	0,	0,	 0	},	  		/* #3	undefined */
	{	0,	0,	 0	},	  		/* #4	undefined */
	{	0,	0,	 0	},	  		/* #5	undefined */
	{	0,	0,	 0	},	  		/* #6	undefined */
	{	0,	0,	 0	},	  		/* #7	undefined */
	{	0,	0,	 0	},	  		/* #8	undefined */
	{	0,	0,	 0	},	  		/* #9	undefined */
	{	0,	0,	 0	},	  		/* #10	undefined */
	{	0,	0,	 0	},	  		/* #11	undefined */
	{	0,	0,	 0	},	  		/* #12	undefined */
	{	0,	0,	 0	},	  		/* #13	NTBS */
	{	0,	0,	 0	},	  		/* #14	VIP */
	{	0,	0,	 0	},	  		/* #15	VIP:VSPP */
	{	0,	0,	 0	},	  		/* #16	VIP:VIPC */
	{	0,	0,	 0	},	  		/* #17	DRP */
	{	0,	0,	 0	},	  		/* #18	DRP:NSP */
	{	6,	1,	 7	},	  		/* #19	DDP */
	{	7,	4,	 11	},	  		/* #20	DDP:ADSP */
	{	6,	1,	 7	},	  		/* #21	DDP:ATP */
	{	6,	2,	 8	},	  		/* #22	IDP */
	{	6,	2,	 8	},	  		/* #23	IDP:PEP */
	{	6,	4,	 10	},	  		/* #24	IDP:SPP */
	{	0,	1,	 1	},	  		/* #25	IPX */
	{	3,	1,	 4	},	  		/* #26	IPX:NCP */
	{	0,	4,	 4	},	  		/* #27	IPX:PEP */
	{	6,	4,	 10	},	  		/* #28	IPX:SPX */
	{	6,	2,	 8	},	  		/* #29	IP */
	{	6,	2,	 8	},	  		/* #30	IP:UDP */
	{	4,	8,	 12	}	  		/* #31	IP:TCP */
};
/*=============================== SYSTEM GLOBALS ============================== */


/*****************************************************************************
 *  Cisco Functions called from SDU code to provide a Cisco-specific service *
 *****************************************************************************/

uchar *
crypto_glue_umSduGetDssPublic(uchar *sduNetAddr, uchar *sduSerialNum, 
							  CIBptr cib)
{
    crypto_pubkey *k;
    uchar *key;
    uint serial;
    boolean found;
    int i;
   
    /*
     * Get an integer representation of the serial number.
     */
    memcpy(&serial, sduSerialNum, 4);

    /*
     * Get the name associated with the serial number.
     */
    k = crypto_pubkey_get_by_serial(serial);
    if (k == NULL) {
	return 0;
    }
    /* 
     * Verify that the peer is an acceptable peer by comparing the peer name
     * with those stored in the cib. Our pointer in the cib is actually a
     * pointer to the crypto map structure which may have changed or been
     * freed so we need to check it to ensure it is valid.
     */
    found = FALSE;
    if (cib->crypto.cisco.peers_allowed) {
    	for (i=0; cib->crypto.cisco.peers_allowed[i]; i++) {
	    if (!strcmp(cib->crypto.cisco.peers_allowed[i], k->name)) {
		found = TRUE;
		break;
	    }
	}
    }
    cib->crypto.cisco.peers_allowed = NULL;

    if (!found) {
	return(0);
    }

    /*
     * Copy it into memory which the caller can safely keep around for as 
     * long as they like.
     */
    key=malloc(CRYPTO_KEYSIZE);
    if (key == NULL) {
        return(0);
    }
    memcpy(key, k->pubkey.key, CRYPTO_KEYSIZE);

    return (key);
}

uchar
crypto_glue_good_signature( uchar *start, uchar *sig, short index, 
			    uchar *dss_pub_num)
{
    ushort slot = 0;

    /*
     * Find the slot, if RSP/VIP
     */
    if (crypto_number_of_slots > 1) {
	slot = pconn_list[index].cib->crypto.cisco.slot;
    }
    return((uchar) crypto_verify_me_sdu(start, sig, slot, dss_pub_num));
}

/*
 * GetRTClock wants time in seconds. 
 */
int GetRTClock( ulong *time)
{
    secs_and_nsecs secs;

    secs_and_nsecs_since_jan_1_1970(&secs);
    *time = secs.seconds;
    return(0);
}

/*
 * Search the pending connection list to see if a given cib is linked to
 * any pending connections. A linkage is only true if the timestamp in the
 * pending connection has a time value.
 */
int crypto_glue_find_pending_index(CIBptr cib)
{
    short i = pconn_used;

    while (i != NULL_INDEX) {
	if ((pconn_list[i].cib == cib) && (pconn_list[i].timestamp != 0L)) {
	    break;
	}
	i = pconn_list[i].next;
    }

    return(i);
}

/*
 * Find any pending index referring to the given cib
 * and clear it.
 */
void crypto_glue_clear_pending_index(CIBptr cib)
{
    short i;

    while ((i=crypto_glue_find_pending_index(cib)) != NULL_INDEX) {
	free_pconn(i);
    }
    return;
}

void crypto_glue_timeout_keys(CIBptr cib)
{
    /*
     * Make sure the new connection is "unset" to
     * signal session timeout. It should have been 
     * already, but this increases reliability since we
     * are depending upon it being "unset". Also, don't
     * bother to call if the connection ids are both CRYPTO_CONN_UNSET
     * already -- this can happen on a connection setup failure.
     */
    if ((cib->crypto.cisco.connection_id     != CRYPTO_CONN_UNSET) ||
	(cib->crypto.cisco.new_connection_id != CRYPTO_CONN_UNSET)) {
    	    crypto_glue_xchg_connection_ids(cib, TRUE, CRYPTO_CONN_UNSET);
    }
    /*
     * For reliability, clear the TIME_KEYS & PEND_CONN bits regardless of 
     * circumstances.
     */
    (cib->status) &= (~TIME_KEYS);
    (cib->status) &= (~PEND_CONN);
    (cib->status) |= XCHG_KEYS;
    crypto_glue_clear_pending_index(cib);

    return;
}

void crypto_glue_init_connection_ids (void)
{
    int i;

    crypto_number_of_slots = crypto_get_number_of_slots();
    cs_unique_conn_id_array = malloc(crypto_number_of_slots*sizeof(ulong));
    crypto_dh_pregen_head_array = 
        malloc(crypto_number_of_slots*sizeof(crypto_dh_pregen_head_array));

    if (cs_unique_conn_id_array && crypto_dh_pregen_head_array) {
        for (i=0; i<crypto_number_of_slots; i++) {
	    cs_unique_conn_id_array[i] = 0L;
	    crypto_dh_pregen_head_array[i] = (crypto_dh_pregen *)NULL;
        }
    }
}

/*
 * Determine if a connection_id is currently being used. Used means that it
 * is currently found in a Connection Information Block. 
 */
boolean crypto_is_connid_used (ulong conn_id, ushort slot, CIBdef **cib)
{
    ushort    i,j;                /* pet and cot node indeces respectively */
    short    c;                    /* keep count of # COT entries traversed */
    ushort    p;                    /* page index */

    for (i=1; i<pet_nodes; i++) {
        if ((p = pet[i].page) != 0) {
            j = cot[p].used;                /* get last node in free list */
            c = cot_nodes;                        /* load number of cot nodes */

            while (j) {
		if ((cot[p].node[j].cib.crypto.cisco.connection_id == conn_id) 
		        && (cot[p].node[j].cib.crypto.cisco.slot == slot)) {
		    *cib = &cot[p].node[j].cib;
		    return(TRUE);
                }
                j = cot[p].node[j].free.prev;     /* get index of next node */
            }
	}
    }

    cib = 0;
    return(FALSE);
}

inline ulong crypto_glue_next_conn_id (ulong conn_id)
{
    if (conn_id == CRYPTO_CONN_MAXIMUM) {
	return (1);
    } else {
        return (++conn_id);
    }
}

ulong crypto_glue_find_cs_unique_conn_id_array (ulong start_conn_id, 
						ushort slot,
					  	boolean *is_conn_oldest, 
					  	boolean nuke_if_oldest)
{
    ulong try_this_one;
    CIBdef *cib, *oldest_cib;
    boolean used;

   /*
    * Initialize 
    */
    oldest_cib = NULL;
    try_this_one = start_conn_id;
    *is_conn_oldest = FALSE; 

    /*
     * Search through the entire connection id namespace but stop at the
     * first free connection id. The EPA list goes from 0 to 
     * CRYPTO_CONN_TABLE_MAX, which is set to CRYPTO_SWEPA_CONNTBL_SIZE-1.
     * NOTE: We actually are only using from 1 to CRYPTO_SWEPA_CONNTBL_SIZE-1
     *       since 0 means "unassigned" in the ACL index.
     */

    do {
	try_this_one = crypto_glue_next_conn_id(try_this_one);
        used = crypto_is_connid_used(try_this_one, slot, &cib);
        if (used) {
	    if (crypto_sm_debug) {
	        buginf("\nCRYPTO: Connection_id %d, slot %d is already used.",
			try_this_one, slot);
	    }
	    /* 
	     * If this conn_id is being used, and if it has the oldest time,
	     * then we note the cib for later use. Of course we have to ensure
	     * the pointer is valid first....
	     */
	    if (oldest_cib != NULL) {
	        if (oldest_cib->crypto.time > cib->crypto.time) { 
		    oldest_cib = cib;
		}
	    } else {
	        oldest_cib = cib;
	    }
	}
    } while (used && (try_this_one != start_conn_id));

    /* 
     * If we stopped where we started the entire table is full of
     * valid connections. Otherwise we found a free one so we're done.
     */
    if (try_this_one != start_conn_id) {
	return(try_this_one);
    }

    /*
     * Return the oldest one to the caller, who decides what to do with it.
     * Also nuke it if desired -- we have the cib info, the caller doesn't.
     */
    *is_conn_oldest = TRUE;
    if (nuke_if_oldest) {
        crypto_glue_timeout_keys(oldest_cib);
    }
    return(oldest_cib->crypto.cisco.connection_id);
}

/*
 * Tell the RP/RSP crypto engine to pregenerate the configured number of DH 
 * pairs. This is done strictly for preformance reasons; the time taken by the 
 * exponentiation takes so long that doing it ahead of time speeds up 
 * connection setup by a significant amount of time on slow routers.
 *
 * Session Manager keeps a list of connections which have pregenerated DH
 * pairs which obviously take priority when choosing a connection id at 
 * connection time.
 *
 * This function doesn't explicitely yield the processor, but needn't since
 * each call to crypto_pregen_dh_pair() causes a yield anyway while waiting
 * for the crypto engine to create the pair.
 */
void crypto_glue_pregen_dh (void)
{
    short how_many_in_list, how_many_we_need;
    short conn_id;
    boolean in_use;
    crypto_dh_pregen *item, *new_item, *last_in_list;
    ulong start_conn_id;
    int this_slot;

    /*
     * Continue only if we are configured to pregenerate DH pairs, and
     * if there are no pending connections.
     */
    if (!crypto_dh_pregen_count_array || !crypto_dh_pregen_head_array  ||
	(pconn_used != NULL_INDEX)) {
	return;
    }

    for (this_slot=0; this_slot<crypto_number_of_slots; this_slot++) {
	/*
     	 * Continue only if this slot is configured to pregenerate DH pairs.
         */
    	if (crypto_dh_pregen_count_array[this_slot] == 0) {
	    continue;
        }

	/*
	 * See how many have already been pregenerated by running through the 
	 * list & see if we need any more.
	 */
	how_many_in_list = 0;
	item =  crypto_dh_pregen_head_array[this_slot];
	while (item) {
	    how_many_in_list++;
	    item=item->next;
	}
	how_many_we_need = crypto_dh_pregen_count_array[this_slot] - 
				how_many_in_list;
	if (how_many_we_need <= 0) {
	    continue;
	}

	/*
	 * OK, we've got work to do.
	 */
	if (crypto_sm_debug) {
	    buginf("\nCRYPTO: Need to pregenerate %d pairs for slot %d.",
		    how_many_we_need, this_slot);
	}

	/*
	 * Start with the current connection id. Well, actualy we'll increment 
	 * this inside crypto_glue_find_cs_unique_conn_id_array() first, so 
	 * we'll really start with the next one.
	 */
	start_conn_id = cs_unique_conn_id_array[this_slot];
	/*
	 * We do nasty things to the router if we do a lot of them at once, even
	 * though the processor is yielded between calls to the crypto engine.
	 * The hack is to just do one per wakeup, but there isn't any 
	 * real hurry to pre-generate anyway. Note: The whole business of
	 * figuring out how many is needed above is left in place for the
	 * future when we might do a better job of deciding how many to do each
	 * time.
	 */
	how_many_we_need = 1;
	while (how_many_we_need-- > 0) {
	    /*
	     * Loop until either we find a good connection id or we run out of
	     * connection ids to try.
	     */
	    while (TRUE) {
		/*
		 * Find a connection id
		 */
		conn_id = 
		    crypto_glue_find_cs_unique_conn_id_array(start_conn_id,
							     this_slot,
							     &in_use, FALSE);
		if (in_use) {
		    /*
		     * If the connection returned is in use then all 
		     * connections are currently in use, so we cannot 
		     * precompute anything at all. Give it up, mate.
		     */
		    return;
	        }
	        /*
	         * Found an unused conn_id, now see if it already has a DH pair 
	         * pregenerated for it. Also, save a pointer to the last item in
	         * the list for later use.
	         */ 
	        last_in_list = crypto_dh_pregen_head_array[this_slot];
	        if (crypto_dh_pregen_head_array[this_slot]) {
	    	    item = crypto_dh_pregen_head_array[this_slot];
		    while (item && (item->conn_id != conn_id)) {
		        last_in_list = item;
		        item=item->next;
	    	    }
	    	    if (item == NULL) { /* no DH pair yet generated */
		    	break;
	    	    } else {
		    	start_conn_id = conn_id;
		    }
	        } else { /* empty list */
		    break;
	        }
	    }
	}

	/* 
	 * Call the crypto engine to pregenulate a dh pair for the 
	 * connection.
	 */
	if (crypto_sm_debug) {
	    buginf("\nCRYPTO: Pregenerating DH for conn_id %d slot %d", 
		   conn_id, this_slot);
	}
	if (crypto_pregen_dh_pair(conn_id, this_slot) == FALSE) {
	    continue;
	}

	/*
	 * Successful pregeneration, so create a new list item.
	 */
	new_item = malloc(sizeof(crypto_dh_pregen));
	if (!new_item) {
		return;
	}
	new_item->conn_id = conn_id;
	new_item->next = NULL;
	if (crypto_dh_pregen_head_array[this_slot]) {
	    /*
	     * Add to the list. last_in_list should always be non-NULL, but just
	     * in case, fail silently.
	     */
	    if (last_in_list && (last_in_list->next == NULL)) {
	        last_in_list->next = new_item;
	    } else {
		free(new_item);
	    }
	} else {
	    /*
	     * Begin the list
	     */
	    crypto_dh_pregen_head_array[this_slot] = new_item;
	}
    }
    return;
}

/*
 * The Cylink code uses the "cs_unique_conn_id_array" variable to determine a
 * connection id. We use it as well, although it is really implemented as an
 * array because of machines with multiple crypto engines (e.g., RSP/VIP). 
 * However, Cylink does not have an upper bound on the variable whereas we do. 
 * (The EPA has a 300 hard-coded limit.)
 *
 * The Cylink code simply increments cs_unique_conn_id_array; this call replaces
 * those increment statements.
 * 
 * We attempt to find an unused conn_id, but if none then it determines which
 * to reuse. If set in the configuration, we may pregenulate some DH pairs
 * for performance reasons. If so, we have a list of those connection ids which
 * we should use first.
 */
ulong crypto_glue_set_cs_unique_conn_id (ushort slot)
{
    boolean dont_care, pregen_found;
    crypto_dh_pregen *item;
    CIBdef *dont_use;

    /*
     * Check the pregenerated list first.
     */
    pregen_found = FALSE;
    while (crypto_dh_pregen_head_array && crypto_dh_pregen_head_array[slot]) {
	item=crypto_dh_pregen_head_array[slot];
        /*
	 * Check to make sure the connection id is still unused.
	 */
	if (crypto_is_connid_used(item->conn_id, slot, &dont_use)) {
	    /*
	     * Oops. It's been used. Dequeue and try the next one.
	     */
	    crypto_dh_pregen_head_array[slot] = item->next;
	    free(item);
	    continue;
	} else {
	    crypto_dh_pregen_head_array[slot] = item->next;
	    cs_unique_conn_id_array[slot] = item->conn_id;
	    free(item);
	    pregen_found = TRUE;
	    break;
	}
    }
    if (!pregen_found) {
	/*
     	 * We've exhausted the pregen list. Look for a new one. We are
     	 * guarenteed to have a conn_id returned to us, either a free one or
     	 * a recycled one.
     	 */
    	cs_unique_conn_id_array[slot] = 
	 crypto_glue_find_cs_unique_conn_id_array(cs_unique_conn_id_array[slot],
	 slot, 
	 &dont_care, TRUE);
    }
    return(cs_unique_conn_id_array[slot]);
}

inline boolean
crypto_glue_generate_DH_public_number (uchar *public_number, short conn_id,
				       ushort slot)
{
    return crypto_generate_DH_parameters((short) DH_ALG_PHASE1, public_number, 
					 conn_id, slot);
}

inline boolean
crypto_glue_generate_DH_shared_number (uchar *upe_public_number, short conn_id,
				       ushort slot)
{
    return crypto_generate_DH_parameters((short) DH_ALG_PHASE2, 
					 upe_public_number, conn_id, slot);
}

inline boolean
crypto_glue_generate_syndrome (short conn_id, ushort slot, short offset,
			       short challenge_length, uchar *syndrome)
{
    return crypto_generate_syndrome(conn_id, slot, offset, challenge_length, 
				    syndrome);
}

inline boolean
crypto_glue_crypto_sign (ushort slot, uchar *data_start, ushort data_length,
			 uchar *signature_start)
{
    return crypto_sign_me(slot, (char *)data_start, data_length, 
			  (char *)signature_start);
}

/*
 * The Cisco NNC is unsigned and built on the fly. This function adds
 * the Node id to the NNC.
 */
void
crypto_glue_insert_node_id(uchar *nnc_location, EntityID *pe)
{
    uchar *node_addr = nnc_location + CRYPTO_NODE_ID_OFFSET;

    /*
     * Insert the node id
     */
    switch (pe->pid) {
	case INTERNET_IP:
    	    memcpy(node_addr, &pe->adr, 4); /* Copy the IP address */
	    node_addr += sizeof(ulong);     /* Skip over IP address */
	    *node_addr++ = INTERNET_IP;	  /* Define protocol type */
	    *node_addr = 0;		  /* Be clean */
	    break;

	default:
	    /*
	     * Invalid PID: Clear out the space.
	     */
	    memset(node_addr, 0, 6);
	    if (crypto_sm_debug) {
		buginf("\nCRYPTO: Invalid PID found for NNC.");
	    }
	    break;
    }
    return;
}

/**************************************************************************
* Replacement routines for SDU ones
**************************************************************************/

/*
 * The following replace functions defined in pa/phybuf.c
 */


/*
 * Acquire memory for a conn. msg. ("frame"), and the control structure
 * holding it. This is really gross, but an artifact of trying to use SDU
 * code as much as possible.
 */
void *taskAcquireBuffer( void)
{
    FrameControlHeader *header;

    /*
     * Need to malloc the bigger of Pa2EpFrameControl and FrameControlHeader,
     * and since the former includes the latter this is an easy choice.
     */
    header = malloc(sizeof(Pa2EpFrameControl));
    if (!header) {
	return(NULL);
    }
    header->frameP=malloc(CRYPTO_MAX_CONNECTION_MESSAGE_SIZE);
    if (!header->frameP) {
	free(header);
	return(NULL);
    }
    /* 
     * Yuk, a self-reference. This is the pointer given to taskReleaseBuffer
     * by the SDU code. We put a self-reference here so we free() the pointer
     * in an anologous way to the SDU code.
     */
    header->descLogP = header; 

    /*
     * Malloc space for the Cisco-specific stuff
     */
     header->otherP=malloc(sizeof(cisco_FrameControlHeader));
     if (!header->otherP) {
	free(header->frameP);
	free(header);
	return(NULL);
     }

    return(header);

}

/*
 * Free the "frame" memory & the structure holding the pointer.
 */
void taskReleaseBuffer( Cy2TxFrameControl *p)
{
    if (p == NULL) {
	return;
    }
    free(p->frameP);
    free(p->otherP);
    free(p);

    return;
}

int taskTransmitReq (Cy2TxFrameControl *fch, int size, int action)
{
    crypto_sesmgmt_block blk;
    crypto_sesmgmt_message_types message_type_sent;
    cisco_FrameControlHeader *cisco;

    cisco = (cisco_FrameControlHeader *)fch->otherP;

    /*
     * We really don't know what kind of message at this point so we let
     * the IOS routine figure it out. We do need to tell them whether this 
     * message is a re-transmitted message or not, however.
     */
    if (cisco->retransmit == FALSE) {
    	blk.message_type = Undeclared_Message;
    } else {
    	if (crypto_sm_debug) {
    	    buginf("\nCRYPTO: Retransmitting a connection message");
    	}
    	blk.message_type = Retransmitted_Message;
    }
    blk.message = fch->frameP;
    blk.message_length = size;

    /*
     * The following fields are used to determine addressing of the message. 
     */
    blk.pe.type = blk.upe.type = ADDR_IP;
    blk.src.ip_addr = cisco->epEntry.cisco.src;
    blk.dst.ip_addr = cisco->epEntry.cisco.dst;

    /*
     * Send out the connection message via the IOS.
     */
    message_type_sent = crypto_ip_send_echo_based_msg(&blk);

    if (message_type_sent == Unsuccessful_Message) {
	return(REJECT_TXREQ);
    } 

    return(ACCEPT_TXREQ);
}
/* 
 * Extracted and modified from isr/pa/paisr.c 
 */
/*-------------------------------------------------------------------------
*	Name: ushort computeHashValue( ulong address, int protocol)
*		computes a Hash value for EP module for a given Protocol and protocol
*		dependent address.
*		returns the computed hash value that corresponds to the input;
*-------------------------------------------------------------------------*/
ushort computeHashValue( ulong address, int protocol)
{
  EntityID eids ;
  ulong ipnetType ;
  ulong netMask ;
  int result ;
  
  if ( protocol != INTERNET_IP)
		return (ushort)address ;
#ifndef CISCO		
  result = normalizeIpEntityInfo( address, &eids) ;
#else
  /*
   * The above function returns 0 anyway if the IP subnet table isn't
   * populated. Currently, we don't use the IP subnet table.
   */
  result = 0;
#endif /* CISCO */
  if (result)
		return eids.hash ;
  ipnetType = address & 0x00FF ;
  netMask = (ipnetType < 127) ? 0xFFFFFF00ul :  /* class A  */
				(ipnetType < 192) ? 0xFFFF0000ul :  /* class B */
										  0xFF000000ul ;  /* class C */
  return (ushort)(address & ~netMask) ;
}

/*
 * The real getAuthTimers and getNCtimeStamp routines are in task/auth/authfun.c
 */

/* -------------------------------------------------------------------------
*		AuthNetControl	*getAuthTimers( void) 
*	 Returns the current active ASRE record;
*	 Users: Auth. Manager to know R & E expirations, CS task to select NNC
*	 Output: NULL if there is no NC in the system
* -------------------------------------------------------------------------*/
inline AuthNetControl	*getAuthTimers( void) 
{
    return NULL ;
}

/* -------------------------------------------------------------------------
*		ulong getNCtimeStamp( void) 
*		Returns the time stamp of the active Net Cert; called by Auth. Mgr 
*		or CS task
* -------------------------------------------------------------------------*/
inline ulong getNCtimeStamp( void) 
{
    return 0uL ;
}

/*
 * For use by crypto_glue_set_crypto_method():
 * Mapping of Cylink crypto method values to EPA enum values
 */

typedef struct crypto_method_map_type_ {
	uchar cylink;
	alg_types cisco;
} crypto_method_map_type;

/* 
 * The SDU code uses a list of algorithms which is distinct from the
 * list kept by the crypto engine. This table reconciles the relevant
 * algorithms in the two lists.
 */
crypto_method_map_type crypto_method_map[] = {
    { DES_56_64CFB, 	DES_CFB64_IV1},
    { DES_56_8CFB,	DES_CFB8_IV1},
    { DES_40_64CFB,	DES_40_CFB64_IV1},
    { DES_40_8CFB,	DES_40_CFB8_IV1}
};


boolean crypto_glue_set_crypto_method(CIBptr cib)
{
    int i;
    alg_types alg;
    int conn_id, new_conn_id;

    /*
     * Use the local variables for clarity.
     */
    conn_id     = cib->crypto.cisco.connection_id;
    new_conn_id = cib->crypto.cisco.new_connection_id;
    /*
     * The new connection id must be a valid connection id, and the current
     * must be either (an older) valid connection id or a pending id.
     * We don't want to be replacing CRYPTO_CONN_UNSET with a connection id!
     */
    if (!crypto_conntable_conn_is_valid(new_conn_id) ||
 	!(crypto_conntable_conn_is_valid(conn_id) ||
	  crypto_conntable_conn_is_pending(conn_id))) {
        if (crypto_sm_debug) {
        	buginf("\nCRYPTO: Invalid connection id pair: old=%d, new=%d\n",
			cib->crypto.cisco.connection_id,
			cib->crypto.cisco.new_connection_id);
        }
	return(FALSE);
    }

    /*
     * Translate the Cylink method to Cisco method.
     */
    alg = DH_ALG_PHASE1; /* invalid algorithm */
    for (i=0; i< sizeof(crypto_method_map)/sizeof(crypto_method_map_type);
		i++) {
	if (cib->crypto.method == crypto_method_map[i].cylink) {
	    /*
	     * Found a match. Call the Cisco code & return.
	     */
	    alg = crypto_method_map[i].cisco;
	    break;
	}
    }

    if (alg != DH_ALG_PHASE1) { /* Found a match */
	/*
	 * Set the encryption algorithm for the connection. This marks
	 * the completion of the connection setup; the connection is now
	 * usable if the policy puts it (or has put it) in the policy.
	 */
	if (crypto_create_session_key(new_conn_id, 
				      cib->crypto.cisco.slot,
				       (short)alg) == TRUE) {
	}
    	return(TRUE); 
    }

    if (crypto_sm_debug) {
    	buginf("\nCRYPTO: Algorithm not found: %d", cib->crypto.method);
    }

    return(FALSE);
}

/*
 * Cleanup the CIB so that it is obvious that there is no connection between
 * this PE and UPE.
 */
void crypto_glue_cleanup_cib(CIBptr cib)
{
    cib->crypto.cisco.connection_id = CRYPTO_CONN_UNSET;
    cib->crypto.cisco.new_connection_id = CRYPTO_CONN_UNSET;
    cib->crypto.method = UNDEFINED;
    cib->crypto.time = 0;
    cib->status = BAD_CONN|XCHG_KEYS ;
}

void crypto_glue_setup_cib (CIBptr cib, void *ptr)
{
    crypto_sesmgmt_block *blk;

    if (!cib) {
        buginf("\nEMPTRY CIB>");
        return;
    }

    blk = (crypto_sesmgmt_block *) ptr;
    /*
     * Update the protocol vector in the CIB. We remove TIME_KEYS from the 
     * status to indicate that the connection is no longer complete. Also,
     * add the standard PEND_CONN & USED_NODE to make it as pending.
     */
    cib->crypto.time = 0;
    /*
     * Set the current method to undefined because we haven't negotiated
     * a method yet. Record the methods we will allow in methods_allowed.
     */
    cib->crypto.method = CRYPTO_UNDEFINED_METHOD;
    cib->crypto.cisco.methods_allowed = blk->method;
    cib->crypto.cisco.peers_allowed = blk->peer;
    cib->crypto.cisco.mode_flags = blk->mode_flags;
    (cib->status) &= (~TIME_KEYS);
    cib->status |= PEND_CONN|USED_NODE;
    /* 
     * Assign a pending value in case an initiate connection attempt 
     * happens before this connection is setup. But if there was an old
     * connection id which is being replaced we need to clear it out of
     * the policy & EPA first.
     *
     * First we must clean any old connection id out of the policy & crypto
     * engine. Then we can insert a new pending identifier in the policy.
     */
    if (cib->crypto.cisco.connection_id != CRYPTO_CONN_UNSET) {
        crypto_glue_clear_conn_id(cib->crypto.cisco.connection_id,
    			      blk->pending_conn_id,
    			      cib->crypto.cisco.slot);
    }
    cib->crypto.cisco.connection_id = blk->pending_conn_id;
    cib->crypto.cisco.slot = blk->slot;
}

/*
 * Returns a pointer to a string associated with a connection status.
 */
char *
crypto_xlate_cib_status (ulong type)
{
    static char str[12];
    switch (type) {
	case PEND_CONN:
	    sstrncpy(str, "PEND_CONN", 10);
	    break;
	case OPEN_NODE:
	    /* not relevant to unmanaged case */
	    str[0]=0x0;
	    break;
	case LOCK_NODE:
	    /* not relevant to unmanaged case */
	    str[0]=0x0;
	    break;
	case USED_NODE:
	    /* not relevant to users */
	    str[0]=0x0;
	    break;
	case XCHG_KEYS:
	    sstrncpy(str, "XCHG_KEYS", 10);
	    break;
	case TIME_KEYS:
	    sstrncpy(str, "TIME_KEYS", 10);
	    break;
	case KEY_ALLOC:
	    /* not relevant to unmanaged case */
	    str[0]=0x0;
	    break;
	case BAD_CONN:
	    sstrncpy(str, "BAD_CONN", 9);
	    break;
	default:
	    sstrncpy(str, "UNK_STATUS", 11);
	    break;
    }
    return(str);
}

void
crypto_glue_get_network_address (ulong *addr_buffer, long destination)
{
    ipaddrtype source;

    /*
     * Correct the sourceadr. 
     * Choose the best source address to get to the signed destination given
     * within the original connection message.
     */
    source = ip_best_local_address(destination, TRUE);
    if (source == 0) {
	if (crypto_sm_debug) {
	    buginf("\nCRYPTO: No source found for dst %i", destination);
	}
	return;
    }

	*addr_buffer = (ulong) source;
}

/*
 * Returns true if the specified serial number is associated with
 * "this" particular box.
 * Else returns false.
 */
boolean crypto_glue_is_my_snum (ulong serial_num)
{
    extern crypto_mykey *crypto_my_key_list_head ;
    crypto_mykey  *key_entry = crypto_my_key_list_head;
    while ( key_entry != NULL ) {
        if ( key_entry->serial == serial_num ) {
	     return TRUE;
	}
	key_entry = key_entry->next;
    }
    return FALSE;
}

/* 
 * Returns serial number for the crypto engine associated with specified
 * slot number. 
 *
 * If no serial number was found and this system has extended slots, assume
 * the main processor's serial number is correct. Otherwise, no keys have
 * been defined and 0x0 should be returned.
 */
ulong crypto_glue_get_sn_by_slot (ushort slot)
{
    extern crypto_mykey *crypto_my_key_list_head ;
    crypto_mykey  *key_entry = crypto_my_key_list_head;
    while ( key_entry != NULL ) {
        if ( key_entry->slot == slot ) {
	     return key_entry->serial;
	 }
	key_entry = key_entry->next;
    }
    /*
     * No keys found by slot number.
     */
    if (crypto_number_of_slots) {
	return crypto_processor_serial;
    } else {
        return 0;
    }
}
