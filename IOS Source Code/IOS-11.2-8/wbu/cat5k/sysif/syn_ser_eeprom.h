/* $Id: syn_ser_eeprom.h,v 1.1.4.1 1996/05/09 15:01:53 rbadri Exp $
 * $Source: /release/112/cvs/Xwbu/cat5k/sysif/syn_ser_eeprom.h,v $
 *-----------------------------------------------------------------
 * syn_ser_eeprom.h -- Definition of serial eeprom format.  Applies
 *                     to all synergy cards.
 *
 * January 1995, Steve Garcia
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: syn_ser_eeprom.h,v $
 * Revision 1.1.4.1  1996/05/09  15:01:53  rbadri
 * Branch: California_branch
 * Synergy features to the mainline
 *
 * Revision 1.1.40.1  1996/04/27  06:17:48  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.28.1  1996/04/08  01:43:19  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.16.1  1996/03/22  09:13:59  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.1  1996/03/13  07:42:42  rlowe
 * Add file content from Synergy repository.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1  1996/03/11  07:44:56  rlowe
 * Placeholders for California.
 *
 *------------------------------------------------------------------
 * Legacy comments from Synergy repository:
 *
 * Revision 1.19  1995/11/03  17:03:43  iris
 * define earl trap feature bit (h/w changes)
 *
 * Revision 1.18  1995/10/04  01:07:30  kchristi
 * Fix CVS Endlog directives so that cvs-fix-merge script will work.
 *
 * Revision 1.17  1995/08/31  00:16:21  sgarcia
 * Define feature bit to indicate packet buffer size.
 *
 * Revision 1.16  1995/08/28  22:55:49  iris
 * define pmd feature bits - vendor type
 *
 * Revision 1.15  1995/08/04  17:25:38  sgarcia
 * Clarify things a bit.  No content mods.
 *
 * Revision 1.14  1995/08/03  20:57:42  iris
 * rearrange file on request
 *
 * Revision 1.13  1995/08/02  23:39:42  iris
 * add pmd serial eeprom struct for reset ack and future set/show pmd sprom msg
 *
 * Revision 1.12  1995/02/18  19:56:28  iris
 * rename bridge_data to eeprom_data; remove one layer of eeprom struct
 *
 * Revision 1.11  1995/02/18  05:31:58  iris
 * add macro for SER_EEPROM_BSIZE
 *
 * Revision 1.10  1995/02/07  22:10:53  sgarcia
 * Removed media_type.
 *
 * Revision 1.9  1995/02/01  23:04:22  palmore
 * Added warning about changing eeprom version.
 *
 * Revision 1.8  1995/02/01  22:34:46  vinod
 * Moved some defintions from utypes.h to syn_ser_eeprom.h, added
 * a few duplicate constants.
 *
 * Revision 1.7  1995/01/30  18:15:36  sgarcia
 * Added card_media_type.
 *
 * Revision 1.6  1995/01/27  20:42:59  sgarcia
 * Needed SER_EEPROM_DATA_VALID.
 *
 * Revision 1.5  1995/01/23  22:04:55  sgarcia
 * Removed some defines already appearing elsewhere.
 *
 * Revision 1.4  1995/01/19  23:19:09  sgarcia
 * Learn to subtract 34 from 64.  :-(
 *
 * Revision 1.3  1995/01/19  23:10:23  sgarcia
 * Removed snmpobjid3.  Forgot earlier.
 *
 * Revision 1.2  1995/01/19  21:56:33  sgarcia
 * Updated to reflect new serial eeprom format and new design for accessing
 * feature data.
 *
 * Revision 1.1  1995/01/12  21:33:36  sgarcia
 * Initial version
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __syn_ser_eeprom_H__
#define __syn_ser_eeprom_H__

#ifndef	flag
#define	flag(x)		(1 << (x))
#endif

/* general */
#define SER_EEPROM_VERSION         0
#define SER_EEPROM_SIZE            64       /* words */
#define SER_EEPROM_BSIZE           128       /* byte */
#define E_MAX_LEN                  64
#define SER_EEPROM_DATA_VALID      0x55AA
#define SER_EEPROM_CHECKSUM_TOTAL  0x5555
#define START_PROT_AREA            0
#define PROT_AREA_SIZE             64


/* NMC93CS46 eeprom access */
#define SER_EEPROM_ADDR_MASK       0x003F
#define E_ADDR_MASK                0x003F
#define SER_EEPROM_RD_CODE         0x0180
#define E_RD_CODE                  0x0180
#define SER_EEPROM_WR_CODE         0x0140
#define E_WR_CODE                  0x0140
#define SER_EEPROM_WEN_CODE        0x0130
#define E_WEN_CODE                 0x0130
#define	SER_EEPROM_WDS_CODE	   0x0100
#define E_WDS_CODE                 0x0100


/* Read Protected area return values */
#define SER_EEPROM_OK         	0
#define SER_EEPROM_UNINIT     	1
#define	SER_EEPROM_CHKSUM_ERROR	2


/* misc */
#define OEM_STR_SIZE               28
#define MODEL_STR_SIZE             10


/**************
 * Structures *
 **************/

/* Some structs have already been defined in syn_common/utypes.h.
 * They are: spint, BYT_ACC, splint, and WRD_ACC.
 * Perhaps MAC_ACC, STR_ACC, MOD_ACC could be moved there as well,
 * and syn_common.h included here.
 * It will not affect the 8051 code to move these.
 */

/* define split uInt16 for indivdual byte access */
struct spint {
  uChar h;
  uChar l;
};

typedef union {
  uInt16	word;
  struct spint	byte;
} BYT_ACC;

/* define split long word for individual word access */
struct splint {
  uInt16 h;
  uInt16 l;
};

typedef union {
  uInt32	lword;
  struct splint	word;
} WRD_ACC;

typedef union
{
  uInt16	word[3];
  uChar		byte[6];
} MAC_ACC;

typedef union
{
  uInt16	word[(OEM_STR_SIZE >> 1)];
  uChar		str[OEM_STR_SIZE];
} STR_ACC;

typedef union
{
  uInt16	word[(MODEL_STR_SIZE >> 1)];
  uChar		str[MODEL_STR_SIZE];
} MOD_ACC;



/* NMP, LCP Serial EEPROM contents 
 * -------------------------------
 *
 * Following is the defines for the contents of the serial EEPROM as
 * expected by synergy software.  The size of the serial EEPROM used
 * (NMC93CS46) is 64 x 16 bit.
 *
 * The first word in the EEPROM should contain a well known pattern.
 * The presence of this pattern allows the software to determine
 * whether EEPROM has been programmed.
 *
 * EEPROM data is further protected by a 16 bit checksum.  The
 * checksum value is written in the last word of the EEPROM.  This
 * value is such that when each word of the EEPROM is summed, the
 * computed value should equal 0x5555.
 *
 * The actual format of the EEPROM data is given below by both the
 * structure and the enumeration EEPROM_ADDRESSES.  Both of these
 * data types must match with respect to eeprom format.
 * 
 * Adding fields:
 * New eeprom fields are added after the currently last used field.
 * Do not insert fields in the midst of already allocated fields.
 * When adding a field, add it to the structure and to the enumerated
 * list.  Do not forget to decrement "empty" by the number of 16 bit
 * words you are adding.  Remember, the idea is to make sure the
 * format for the older items remains unchanged even though a new
 * item is added at the end.
 *
 * Should the eeprom format ever change, i.e. the old items in the
 * structure move around, the eeprom_version contains a value 
 * representing the format.  Future software can use this field
 * to distinguish between various formats.  BUT CHANGING THE FORMAT
 * IS TO BE AVOIDED AT ALL COSTS since it will make cards supporting
 * the new version of the eeprom incompatible with earlier versions
 * of the supervisor software.  The current eeprom_version is zero.
 *
 * Descriptions of selected fields:
 *   feature:
 *     represents data that does not require changes in the nmp software.
 *   snmp_card_type:
 *     An snmp specific value uniquely identifying a module.  This value
 *     will only change if the new version of the module has a new
 *     customer visible model number.
 *   card_feature_index:
 *     A value internal to synergy software.  It can change when software
 *     must be able to uniquely identify this card.  This value is typically
 *     used as an index into the CardFeatureTable, a feature lookup table.
 *     Note that adding new values to card_feature_index will most likely
 *     require modification/update of the nmp/mcp software.  If updated
 *     nmp/mcp software is not required, the eeprom's "feature" field
 *     should be changed instead.
 *     Bits within the field are defined via the #define for feature.
 *   checksum:
 *     checksum with covers the entire eeprom.  The value stored here is
 *     such that a 16 bit addition of the entire eeprom will sum to
 *     SER_EEPROM_CHECKSUM_TOTAL.  Should always be the last word of the
 *     eeprom.
 *
 */

/* Structure definition for the NMP, LCP general EEPROM. */

struct eeprom_data {
	uInt16	data_valid;     /* should be 0x55AA */
	WRD_ACC	serial_number;
	uInt16	hw_version_high;
	uInt16	hw_version_low;
	MAC_ACC	mac_addr;
   	STR_ACC OEMString; 
	MOD_ACC model;
	uInt16  snmpobjid1;
	uInt16  snmpobjid2;
	uInt16	feature;        /* individual bit masks defined below */
	uInt16  eeprom_version;
	uInt16  snmp_card_type;      
	uInt16  card_feature_index;
	                        /* defined in syn_common/card_features.h */
	/* Insert new stuff here */
	                        /* When inserting, reduce the size of
				 * "empty" by the size of inserted stuff.
				 * Add new stuff to EEPROM_ADDRESSES
				 * enumeration as well. */
	uInt16  empty[30];
	uInt16	checksum;	/* Must always be the last word */
};

/*
 * Enumertion to allow 8051 code to access the eeprom without need to use
 * structure definitions above.  Each entry produces the corresponding
 * eeprom 16 bit word address.
 */
enum EEPROM_ADDRESSES {
  DATA_VALID_ADDR = 0,
  SERIAL_NUMBER_HI_ADDR,
  SERIAL_NUMBER_LO_ADDR,
  HW_VERSION_HI_ADDR,
  HW_VERSION_LO_ADDR,
  MAC_ADDR_HI_ADDR,
  MAC_ADDR_MID_ADDR,
  MAC_ADDR_LO_ADDR,
  OEMSTRING_1_ADDR,
  OEMSTRING_2_ADDR,
  OEMSTRING_3_ADDR,
  OEMSTRING_4_ADDR,
  OEMSTRING_5_ADDR,
  OEMSTRING_6_ADDR,
  OEMSTRING_7_ADDR,
  OEMSTRING_8_ADDR,
  OEMSTRING_9_ADDR,
  OEMSTRING_10_ADDR,
  OEMSTRING_11_ADDR,
  OEMSTRING_12_ADDR,
  OEMSTRING_13_ADDR,
  OEMSTRING_14_ADDR,
  MODEL_1_ADDR,
  MODEL_2_ADDR,
  MODEL_3_ADDR,
  MODEL_4_ADDR,
  MODEL_5_ADDR,
  SNMPOBJID1_ADDR,
  SNMPOBJID2_ADDR,
  FEATURE_ADDR,
  EEPROM_VERSION_ADDR,
  SNMP_CARD_TYPE_ADDR,
  CARD_FEATURE_INDEX_ADDR,
  /* empty up to checksum */
  CHECKSUM_ADDR = (SER_EEPROM_SIZE - 1)
};

/*
 * Synergy feature bit definitions/masks.
 * Define bits with the "feature" byte of the serial eeprom.
 */

#define SYN_FEATURE_UNUSED_BIT_15  0x8000   /* unused */
#define SYN_FEATURE_UNUSED_BIT_14  0x4000   /* unused */
#define SYN_FEATURE_UNUSED_BIT_13  0x2000   /* unused */
#define SYN_FEATURE_UNUSED_BIT_12  0x1000   /* unused */
#define SYN_FEATURE_UNUSED_BIT_11  0x0800   /* unused */
#define SYN_FEATURE_UNUSED_BIT_10  0x0400   /* unused */
#define SYN_FEATURE_UNUSED_BIT_9   0x0200   /* unused */
#define SYN_FEATURE_UNUSED_BIT_8   0x0100   /* unused */
#define SYN_FEATURE_UNUSED_BIT_7   0x0080   /* unused */
#define SYN_FEATURE_UNUSED_BIT_6   0x0040   /* unused */
#define SYN_FEATURE_UNUSED_BIT_5   0x0020   /* unused */
#define SYN_FEATURE_UNUSED_BIT_4   0x0010   /* unused */
#define SYN_FEATURE_UNUSED_BIT_3   0x0008   /* unused */
#define SYN_FEATURE_UNUSED_BIT_2   0x0004   /* unused */
#define SYN_FEATURE_EARL_TRAP      0x0002   /* 0=NotSupported, 1=Supported Supervisor Card Earl Trap Feature */
#define SYN_FEATURE_PKT_BUF_SIZE   0x0001   /* 0=32K, 1=128K packet buffers */



/* ATM's PMD Serial EEPROM contents 
 * --------------------------------
 *
 * Following is the defines for the contents of the ATM's PMD serial
 * EEPROM.  The size of the serial EEPROM used (NMC93CS46) is 64 x 16 bit.
 *
 */

/* Structure definition (without checksum) for the ATM PMD EEPROM. */

typedef struct _PMD_SERPROM	
{
	uInt16  data_valid;
	WRD_ACC	serial_number;
	uInt16	hw_version_high;
	uInt16	hw_version_low;
	MOD_ACC model;
	uInt16	feature;
	uInt16  snmp_card_type;      

	/* size must be less than additional_info in scp_msgs.h */

} PMDSERPROM;

/* PMD feature index mask define */
#define	PMD_FEATURE_VENDOR (flag(0) | flag(1))

/* vendor (LS2b)*/
#define VENDOR_FUJITSU_NTC  0x0
#define VENDOR_FUJITSU_NTC2 0x1
#define VENDOR_SIERRA_PMC   0x2

#endif  /* #ifndef __syn_ser_eeprom_H__ */
