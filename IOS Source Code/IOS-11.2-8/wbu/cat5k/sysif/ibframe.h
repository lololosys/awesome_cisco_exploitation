/* $Id: ibframe.h,v 1.1.4.1 1996/05/09 15:01:46 rbadri Exp $
 * $Source: /release/112/cvs/Xwbu/cat5k/sysif/ibframe.h,v $
 *------------------------------------------------------------------
 * ibframe.h
 *
 * February 1995, umahajan
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ibframe.h,v $
 * Revision 1.1.4.1  1996/05/09  15:01:46  rbadri
 * Branch: California_branch
 * Synergy features to the mainline
 *
 * Revision 1.1.40.1  1996/04/27  06:17:42  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.28.1  1996/04/08  01:43:11  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.16.1  1996/03/22  09:13:52  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.1  1996/03/13  07:42:36  rlowe
 * Add file content from Synergy repository.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1  1996/03/11  07:44:50  rlowe
 * Placeholders for California.
 *
 *------------------------------------------------------------------
 * Legacy comments from Synergy repository:
 *
 * Revision 1.2  1995/10/04  01:07:42  kchristi
 * Fix CVS Endlog directives so that cvs-fix-merge script will work.
 *
 * Revision 1.1  1995/05/01  23:56:01  jjchen
 * Moved from syn_core/nmp/nmpflash/ibipc.
 *
 * Revision 1.1  1995/03/29  05:46:09  jjchen
 * Created.
 *
 * Revision 1.2  1995/02/10  02:30:09  umahajan
 * Added pack attribute to the structures for gcc68k to work properly.
 *
*
 *------------------------------------------------------------------
 * $Endlog$
 */



#ifndef PAK
#define PAK __attribute__ ((packed))
#endif

#define TXBUFSIZE  2048
#define RXBUFSIZE  2048
#define FC_PAD_SIZE  3              /*pad to fc field in 1st word of frame*/
#define MAC_ADDR_LEN 6              /*48 bit mac station address*/

/*FDDI Frame definitions, per rfc1103*/

#define ARPHDR_FDDI  6
#define SNAP_SAP     170
#define SNAP_CNTL    3
#define SNAP_ORG     0
#define FDDI_FC      0x51

#define FDDIMTU		4352
#define ETHERMTU	1500
#define SLIPMTU		1006

#define MAX_ETHER_FRAME 1514
#define FC_MASK         0xf0        /* Upper nibble gives class             */
#define LLC_FC          0x50        /* Link level class                     */
#define SMT_FC          0x40        /* SMT class                            */
#define CLAIM_FC        0xC3        /* Claim FC field.                      */
#define BEACON_FC       0xC2        /* Beacon FC field.                     */

typedef struct {
	uInt16 high;
	uInt32 low;
} MAC_ADDRESS;

struct etherhdr {
	MAC_ADDRESS da;
	MAC_ADDRESS sa;
	uInt16 etype;
};
/*
	This llc definition is for the llc driver.
*/
typedef struct llchdr {
    uChar dsap PAK;
    uChar ssap PAK;
    uChar cntl PAK;
} LLCHDR;

struct a8023hdr {
    MAC_ADDRESS da PAK;
    MAC_ADDRESS sa PAK;
    uInt16 len PAK;
    struct llchdr llchdr PAK;
};

typedef struct llcsnap {
	uChar dsap PAK;
	uChar ssap PAK;
	uChar cntl PAK;
    uChar org_code1 PAK;
    uChar org_code2 PAK;
    uChar org_code3 PAK;
    uInt16 etype PAK;
} LLCSNAP;

/*MAC Layer Header*/

typedef struct machdr {
    uChar pad[FC_PAD_SIZE];
    uChar fc;
    uChar da[MAC_ADDR_LEN];
    uChar sa[MAC_ADDR_LEN];
} MACHDR;

typedef struct machdrnopad {
    uChar fc;
    uChar da[MAC_ADDR_LEN];
    uChar sa[MAC_ADDR_LEN];
} MACHDRNOPAD;

/*
	This LLC Layer Header is for the FDDI recv frame code which looks for a SNAP
	header normally.
*/

typedef struct snaphdr {
    uChar org_code1 PAK;
    uChar org_code2 PAK;
    uChar org_code3 PAK;
    uInt16 etype PAK;
} SNAPHDR;
