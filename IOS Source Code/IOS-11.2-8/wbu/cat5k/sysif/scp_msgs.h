/* $Id: scp_msgs.h,v 1.1.4.1 1996/05/09 15:01:50 rbadri Exp $
 * $Source: /release/112/cvs/Xwbu/cat5k/sysif/scp_msgs.h,v $
 *-----------------------------------------------------------------
 * scp_msgs.h -- SCP message descriptions
 *
 * December 1994, jlockhar
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: scp_msgs.h,v $
 * Revision 1.1.4.1  1996/05/09  15:01:50  rbadri
 * Branch: California_branch
 * Synergy features to the mainline
 *
 * Revision 1.1.40.1  1996/04/27  06:17:46  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.28.2  1996/04/11  01:50:26  sakumar
 * Header file changes to support LEC creation via VTP messages.
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 1.1.28.1  1996/04/08  01:43:16  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.16.2  1996/04/02  05:08:17  rlowe
 * Fix up include file paths.  Cleanup main ALC makefile regarding
 * sar images and tools.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.16.1  1996/03/22  09:13:57  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.1  1996/03/13  07:42:40  rlowe
 * Add file content from Synergy repository.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1  1996/03/11  07:44:54  rlowe
 * Placeholders for California.
 *
 *------------------------------------------------------------------
 * Legacy comments from Synergy repository:
 *
* Revision 1.106.2.2  1995/12/07  23:00:36  ahhabib
* 	Add define for ACP_InDiagMode. When ATM card is in the Offline DIAG mode,
* 	this bit needs to be set in the module ready message.
*
* Revision 1.106.2.1  1995/11/17  01:40:38  jkuo
* Added SPAN monitor and MII register set/dump messages.
*
* Revision 1.106  1995/10/05  21:23:08  sgarcia
* Mainline post-sync fixup:
* - remove from comments test strings (< or >) which confuse sync scripts.
*
* Revision 1.105  1995/10/04  01:07:28  kchristi
* Fix CVS Endlog directives so that cvs-fix-merge script will work.
*
* Revision 1.104  1995/09/25  02:03:48  iris
* recheckin
*
* Revision 1.103  1995/09/24  19:23:34  iris
* bldVersion's
*
* Revision 1.102  1995/09/22  23:52:51  ralkass
* Add build version for FDDI
*
* Revision 1.101  1995/09/13  23:58:54  ralkass
* Add unsolicited message for intelligent linecard to report to NMP the status of flash download
*
* Revision 1.100  1995/09/07  00:28:20  jkuo
* Added comment for new set_port_features return status.
*
* Revision 1.99  1995/08/31  21:27:46  jkuo
* Added SET_MCP_MAC_ADDR & SET_MCP_MAC_ADDR_RESP structs.
*
* Revision 1.98  1995/08/28  22:55:48  iris
* define pmd feature bits - vendor type
*
* Revision 1.97  1995/08/25  17:39:29  ralkass
* Add SCPSTAT_FDDI_CAM_TBL_FULL for monterey
*
* Revision 1.96  1995/08/10  18:27:10  ralkass
* Define cam entries types for clear cam command
*
* Revision 1.95  1995/08/02  23:40:54  iris
* modify alc status, reset ack, increase xtra diag size, add warnings
*
* Revision 1.94  1995/08/02  07:03:57  ssankara
* Changed cufoff_value to cutoff_value in the message SET_CUTOFF(spelling mistake)
*
* Revision 1.93  1995/08/01  23:38:59  ralkass
* Expand struct GET_FDDI_RESP for monterey
*
* Revision 1.92  1995/08/01  16:58:57  rbau
* added ATM line card PMD info to ALCSTATUS.
*
* Revision 1.91  1995/07/31  23:46:40  ralkass
* Added structs SET_LER_CUTOFF & SET_LER_ALARM. Also, SCP_PSTAT_LER_ALARM to flag FDDI port Ler_alarm condition to the NMP
*
* Revision 1.90  1995/07/28  12:36:30  rsmcmaho
* added InLost and OutLost counters to GET_MAC_RESP
*
* Revision 1.89  1995/07/27  16:16:11  ralkass
* Renamed FDDI SCPSTAT defines
*
* Revision 1.88  1995/07/26  01:56:50  ralkass
* Change TReq type to uInt32 in struct GET_FDDI_RESP for montereyscp_msgs.h
*
* Revision 1.87  1995/07/22  00:02:13  ralkass
* Add status code for FDDI clear cam
*
* Revision 1.86  1995/07/20  23:15:36  ralkass
* Added struct get_FDDI_resp for monterey
*
* Revision 1.85  1995/07/20  17:00:17  ralkass
* Add mib counters to struct GET_BRIDGE_RESP
*
* Revision 1.84  1995/07/19  22:13:11  ralkass
* Add Unreachable field to struct GET_BRIDGE_RESP for FDDI
*
* Revision 1.83  1995/07/07  01:06:18  iris
* Add new struct for FDDIDIAG and ALCDIAG.  Add comments for diag_global bytes
*
* Revision 1.82  1995/06/29  16:53:29  jkuo
* Fix PPD_DEFAULT_LARGE from 0x12 to 0x1C.
*
* Revision 1.81  1995/06/28  02:36:54  rbau
* added  struct ALCSTATUS for ATM diag results
*
* Revision 1.80  1995/06/26  05:13:56  kiett
* PAKing the FDDI and FDDI_NVRAM structs
*
* Revision 1.79  1995/06/20  08:35:14  rlowe
* Add new fields to ATM_INSTANCE_MSG:
*
* 	uInt32  IfStartIndex PAK ;
* 	uInt32  IfIndexRange PAK ;
*
* For SNMP/MIB ATM interface use.
*
* Revision 1.78  1995/06/14  00:16:02  sgarcia
* For 8051:
* card_features.h: added MAC_MODE_MASK
* scp_msgs.h: added SCP_HDR_DATA_START_OFFSET
*
* Revision 1.77  1995/06/13  23:15:15  vinod
* ATMCardInstance and ATMMACAddresses is now sent in its own message,
* not a part of SUPV_MAC_IP_ADDR anymore.
*
* Revision 1.76  1995/06/13  17:32:08  ralkass
*  Added missing member to monterey struct GET_MAC_DBG_RESP
*
* Revision 1.75  1995/06/09  23:57:53  ralkass
* Modified monterey structs
*
* Revision 1.74  1995/06/09  23:42:30  vinod
* Added ATMCardInstance Number to the SUPV_MAC_IP_ADDR structure.
*
* Revision 1.73  1995/06/09  23:28:10  ssankara
* Removed #ifdef MONTEREY flag.
*
* Revision 1.72  1995/06/09  08:46:20  rlowe
* Add field "SCPHDR hdr" to union msg_space.  This gives access to a
* SCP header with src_addr and dest_addr fields broken out.
* For ALC integration to latest LCP driver requirements
* (message model, startup & HW handshakes).
*
* Revision 1.71  1995/06/08  21:28:58  rfa
* New FDDI ILC download messages; new flash header
*
* Revision 1.70  1995/05/31  16:54:45  ralkass
* Corrected comments for Monterey status bits definition
*
* Revision 1.69  1995/05/31  03:07:29  ralkass
* Expanded and made public struct FDDISTATUS, also defined FDDI status flags
*
* Revision 1.68  1995/05/26  18:24:35  jkuo
* Added comments regarding side effects for setting LOOPBACK in SET_PORT_FEATURE.
*
* Revision 1.67  1995/05/25  02:44:03  jkuo
* Added DIAG_ON_LINE & DIAG_OFF_LINE commands.
*
* Revision 1.66  1995/05/24  18:12:37  jkuo
* Added define for CDP_PORT_ADDR_SIZE (7).
*
* Revision 1.65  1995/05/24  02:35:34  jkuo
* Added comments about PORT FEATURES bits.
*
* Revision 1.64  1995/05/24  00:38:37  aannaama
* Added cdp message structure
*
* Revision 1.63  1995/05/23  18:43:10  vinod
* Added ATMMACAddresses field to SUPV_MAC_IP_ADDR structure, added big note
* to ENABLE/DISABLE MODULE/PORT message structure (please read all
* concerned people), added define PE_ENABLE = PE_DEFAULT
*
* Revision 1.62  1995/05/22  21:34:07  ralkass
* Modified struct SYSTEM_DIAG for monterey
*
* Revision 1.61  1995/05/19  05:15:58  vinod
* Moved structure SUPV_MAC_IP_ADDR out of ifdef MONTEREY and added
* a SCPHDR to it.
*
* Revision 1.60  1995/05/19  00:08:07  sgarcia
* scp_defs.h: Added comments to those opcodes that are also defined elsewhere.
*             Removed unused CP_SAP for 8051s.
* scp_msgs.h: added length define for COLL_COUNTS.
*
* Revision 1.59  1995/05/16  16:19:25  iris
* *** empty log message ***
*
* Revision 1.58  1995/05/16  15:54:44  rfa
* iris added SAINT_ERROR_COUNT_MSG, I checked it in for her.
*
* Revision 1.57  1995/05/10  20:15:40  rsmcmaho
* Put ifndef PAK
*
* Revision 1.56  1995/05/09  21:11:37  ralkass
* Added MONTEREY SCP messages
*
* Revision 1.55  1995/05/03  04:30:54  jkuo
* Removed conditional for new GO-ONLINE command to take effect.
* Also changed response code for module/port enable/disbale.
*
* Revision 1.54  1995/04/29  04:26:12  jkuo
* Changed SCP_MSG_STATUS_OFFSET from 4 to 5.  This might have been the reason
* why simple_scp_reply() in scp.c kept sending NMP wrong status in the
* opcode byte location.  I think that's why we've been seeing the spurious
* Unimplemented Opcode messages from time to time.
*
* Revision 1.53  1995/04/29  01:27:57  jlockhar
* Changed CARD_ON_LINE to MODULE_ON_LINE.
*
* Revision 1.52  1995/04/28  17:22:10  jlockhar
* Port Features Message.
*
* Revision 1.51  1995/04/25  00:19:27  sgarcia
* Mother of all Merges.
* Within syn_common, merely added several defines so message handlers no
* longer need to use "magic numbers" as offsets into the message buffers.
*
* Revision 1.50  1995/04/05  00:00:23  jkuo
* Added port_mask (U32) to SET_PORT_FEATURE msg
*
* Revision 1.49  1995/04/04  19:48:56  jlockhar
* Added SET_PORT_FEATURES msg.
*
* Revision 1.48  1995/03/15  20:40:09  jlockhar
* Changed STATUS_PORT_DISBALED to STATUS_PORT_DISABLED in enable/disable msg.
*
* Revision 1.47  1995/03/11  20:56:58  umahajan
* Modified SET_PORT_STATE_CMD to handle multiple tuples in same message.
*
* Revision 1.46  1995/03/09  07:51:41  jkuo
* Renamed _MODULE_RESET to MODULE_RESTARTED
*
* Revision 1.45  1995/03/02  22:06:50  sgarcia
* Renamed SUBCOMMAND_ENABLE_xxxx to SUBCOMMAND_ENDISABLE_xxxx for port/module
* enable/disable SCP command.
*
* Revision 1.44  1995/03/02  02:44:09  jkuo
* Corrected spelling STATUS_MODULE_DISABLED
*
* Revision 1.43  1995/03/02  01:39:45  sgarcia
* Added SCP_GET_COLL_COUNTS and its associated struct.
*
* Revision 1.42  1995/03/01  23:27:12  sgarcia
* Removed SER_PROM_SIZE and SER_PROM_BSIZE.  Code should be using
* similar values from syn_common/syn_ser_eeprom.h.
*
* Revision 1.41  1995/03/01  00:46:07  sgarcia
* Implement scp get status, and link (LED and status).
*
* Revision 1.40  1995/03/01  00:20:24  rbatra
* Added new structures for scp statistics msg
*
* Revision 1.39  1995/02/28  18:22:28  jlockhar
* Updated GETSTAT msg definition.
*
* Revision 1.38  1995/02/28  00:11:08  sgarcia
* Added another port status define for get status message.
*
* Revision 1.37  1995/02/27  19:19:07  sgarcia
* Add defines for GET_STATUS message.
*
* Revision 1.36  1995/02/27  01:17:48  vinod
* After some discussion changed the Enable/Disable module/port
* message structure and some defines.
*
* Revision 1.35  1995/02/25  23:14:02  rbatra
* Added ENDISMSG (Enable/Disable) and changed format of Saint Counters Msg.
*
* Revision 1.34  1995/02/25  20:17:12  rbatra
* Fixed SAINT_COUNTERS msg struct
*
* Revision 1.33  1995/02/25  06:10:24  vinod
* Corrected the comment character
*
* Revision 1.32  1995/02/24  21:22:16  jkuo
* Added support of mem peek into CODE (ROM, type 2)
*
* Revision 1.31  1995/02/24  16:52:23  umahajan
* Added Port States defines for SET_PORT_STATE_CMD.
*
* Revision 1.30  1995/02/24  04:47:19  jkuo
* Added mem peek/poke msgs
*
* Revision 1.29  1995/02/23  22:16:53  umahajan
* Added SCP_SET_PORT_STATE_CMD and SCP_SET_PORT_STATE_RESP typedefs.
*
* Revision 1.28  1995/02/18  19:56:26  iris
* rename bridge_data to eeprom_data; remove one layer of eeprom struct
*
* Revision 1.27  1995/02/17  17:34:25  sgarcia
* Just a nit comment.
*
* Revision 1.26  1995/02/13  18:35:50  jlockhar
* Removed old RSACK message (replaced by RESET_ACK).
* Changed format of the Counters msg.
*
* Revision 1.25  1995/02/11  23:20:34  husain
* Changes for send_pkt & rcv_pkt routines; changed SCP_HEADER_LEN to
* sizeof scp header struct
*
* Revision 1.24  1995/02/11  06:48:49  jkuo
* Added BootROM vers/date/time to ResetAckMsg, added sub cmdStatus to SAINT_REG msg
*
* Revision 1.23  1995/02/07  05:16:54  jkuo
* Moved Error Status bits from regs.h
*
* Revision 1.22  1995/02/07  05:06:44  jkuo
* Added Status field for handling Bad Saint#
*
* Revision 1.21  1995/02/07  03:37:53  jkuo
* Added SCP_DUMP_SAINT_REGS & SCP_SET_SAINT_REGS_BY_ID msgs
*
* Revision 1.20  1995/02/03  18:31:35  sgarcia
* Minor changes for get/clear saint counters, and read/write eeprom.
*
* Revision 1.19  1995/02/02  22:04:06  jlockhar
* Removed Counters Message to let 8051 build.
*
* Revision 1.18  1995/02/02  19:56:53  jlockhar
* Added Counters Msgs, modified CBL and LTL (a little).
* Added ifdef for GCC68K to generate either:
* 	#define PAK __attribute__ ((packed)) (if GCC68K defined)
* 	or
* 	#define PAK				(if not).
*
* Revision 1.17  1995/02/01  01:07:06  vinod
* Added READ_WRITE_SEEPROM strucutre and some related defines
*
* Revision 1.16  1995/01/26  18:26:06  jlockhar
* Added MAXPORTS define.
*
* Revision 1.15  1995/01/26  02:05:35  jlockhar
* Added GETSTAT & BRDPRES messages.
*
* Revision 1.14  1995/01/17  23:55:28  rbatra
* Added union for the LTL msg so that the data body can be treated as
* chars.
*
* Revision 1.13  1994/12/29  05:05:07  blouie
* added data structure for Reset Ack msg.
*
* Revision 1.12  1994/12/24  03:22:31  husain
* merging
*
* Revision 1.11  1994/12/24  02:47:16  husain
* merging
*
* Revision 1.10  1994/12/23  02:48:38  vinod
* Changed the SCP send packet structure name to TRANSMIT_PACKET since
* it conflicted with the SCP opcode.
*
* Revision 1.9  1994/12/22  04:58:32  vinod
* Added msgs for Send/receive packet and for setting and getting
* SAINT registers.
*
* Revision 1.8  1994/12/14  19:33:23  blouie
* Deleted inclusion of utypes.h. It breaks the 68k NMP build.
* Do NOT put 8051 specific include files within these two header files.
*
* Revision 1.7  1994/12/13  23:33:57  husain
* Added include for utypes.h and scp_msgs.h
*
* Revision 1.6  1994/12/09  18:38:41  umahajan
* Reduced the MAX_SCP_PKT_DATA_SIZE to 249 and MAX_SCP_PKT_SIZE to 255.
*
* Revision 1.5  1994/12/08  23:39:18  umahajan
* Flipped src_addr and dst_addr declaration in order to get info into proper
* nibble. SCHDR typedef modified for that.
*
* Revision 1.4  1994/12/08  23:21:05  iris
* Changed uchar to uChar.
*
* Revision 1.3  1994/12/08  21:51:57  jkuo
* Changed the free format definition of the SCP msg, the original definiton
* was breaking my code.
*
* Revision 1.2  1994/12/08  00:47:20  vinod
* changed to comply with C coding standards. Added some more
* message structures. cleaned up some existing structures.
* renamed some defines and structure to be SCP specific
*
* Revision 1.1  1994/12/07  22:47:31  husain
* Moved to syn_common
*
* ---------------------------------------------------------------
 * Revision 1.3  1994/12/05  17:57:01  rbatra
 * fixed a couple of structs with missing typedef so was allocating struct in
 * all the files
 *
 * Revision 1.2  1994/12/03  00:33:52  husain
 * update
 *
 * Revision 1.1  1994/12/01  04:06:07  jlockhar
 * SCP Message formats, starter file.
 *
*
 *------------------------------------------------------------------
 * $Endlog$
 */
#ifndef _SCP_MSGS_H
#define _SCP_MSGS_H

#include "../sysif/scp_defs.h"
#include "../sysif/st_counters.h" 
#include "../sysif/syn_ser_eeprom.h"
#include "../sysif/card_features.h"

#ifdef	GCC68K
#define	PAK __attribute__ ((packed))
#else	/* GCC68K */	
#ifndef PAK
#define PAK 
#endif
#endif	/* GCC68K */

/* defintion of the message body size	*/
/* NOTE : 1 byte is being wasted since 8051 can only count to 255 using
   uChar for the length field */
#define MAX_SCP_PKT_DATA_SIZE		249
#define	MAX_SCP_PKT_SIZE		255

#define MAXPORTS 24	/* remove later when common define agreed JimL */

/* 
 *--------------------------------------------------------------------------
 *      SCP Header Format 
 *--------------------------------------------------------------------------
 */
typedef struct	scphdr {	
	uChar	src_addr:4;  /* Source Slot/Card address in 4 msb */
 	uChar	dest_addr:4; /* Destination Slot/Card address in 4 lsb */
	uChar	length;      /* Length of data portion only */  
	uChar	dsap;
	uChar	ssap;
	uChar	opcode;
} SCPHDR, *PSCP_HDR;
#define	SCP_HEADER_LENGTH         sizeof(SCPHDR)
#define SCP_HDR_LENGTH_OFFSET     1
#define SCP_HDR_DSAP_OFFSET       2
#define SCP_HDR_SSAP_OFFSET       3
#define SCP_HDR_OPCODE_OFFSET     4    /* also exists in common/msg_proc.s */
#define SCP_HDR_DATA_START_OFFSET sizeof(SCPHDR)

/*
 * Comment here.
 */
typedef struct scpilc {
	SCPHDR	hdr PAK;		/* Download command header */
	union {
		uChar	state;		/* Download command state */
		uChar	status;		/* Download command status */
	} cmd PAK;
} SCPILC, *PSCP_ILC;
#define ILC_DNLD_START		0	/* Initiate download command */
#define ILC_DNLD_CONT		1	/* Continue receiving data command */
#define ILC_DNLD_DONE		2	/* Data download done, compute csum */
#define ILC_DNLD_PROG		3	/* Begin extract or explode phase */

/*
 *-------------------------------------------------------------------------- 
 *	SCP free format Message  
 *--------------------------------------------------------------------------
 */
typedef struct	msg {
	uChar   addr; /* Destination Slot/Card address */
        uChar   length;      /* Length of data portion only */
        uChar   dsap;
        uChar   ssap;
        uChar   opcode;
	uChar	mdata[MAX_SCP_PKT_DATA_SIZE];

	/*
         * Checksum follows data immediately. This field is 
	 * filled in/checked  by the driver, applications do 
	 * not need to touch this.
         */
	uChar	checksum;
} SCPMSG, *PSCP_MSG;
#define SCP_MSG_STATUS_OFFSET 5

union msg_space {
	SCPHDR	hdr;
	SCPMSG	msg;
	uChar	buffer[MAX_SCP_PKT_SIZE];
};

/*
 *--------------------------------------------------------------------------
 *	Error Status for Get Diag Status msg (which uses free format mag)
 *--------------------------------------------------------------------------
 */
#ifndef	flag
#define	flag(x)		(1 << (x))
#endif

#ifndef	flagL
#define	flagL(x)	(1L << (x))	/* long word flag */
#endif

/*
 * For error status diag results.
 *
 * Note that several of these values are also defined in 8051 assembler code.
 * Do not change them here with out changing all other occurrences.
 * (There is currently no good method of incorporating C defined values
 * into the 8051 Archimedes assembler.)
 */
/* ErrorStat1 - low byte of diag_global */
#define	ES_CPU		flag(0)		/* CPU, internal Ram/Timer */
#define	ES_XB0		flag(1)		/* External RAM Bank 0	   */
#define	ES_XB1		flag(2)		/* External RAM Bank 1	   */
#define	ES_XB2		flag(3)		/* External RAM Bank 2	   */
#define	ES_DP	        flag(4)         /* dualport RAM		   */
#define	ES_LTL0         flag(5)		/* LTL RAM Bank 0	   */
#define	ES_LTL1         flag(6)		/* LTL RAM Bank 1	   */
#define	ES_LTL2         flag(7)		/* LTL RAM Bank 2	   */

/* ErrorStat2 - high byte of diag_global */
#define	ES_CBL0         flag(0)		/* CBL RAM Bank 0	   */
#define	ES_CBL1         flag(1)		/* CBL RAM Bank 1	   */
#define	ES_CBL2         flag(2)		/* CBL RAM Bank 2	   */
#define	ES_SEEPROM	flag(3)		/* Serial EEPRom checksum  */
#define	ES_BOOTROM	flag(4)		/* Boot Rom checksum	   */
#define	ES_SAINT	flag(5)         /* saint test		   */
#define	ES_PKTBUF	flag(6)         /* Packet Buffer test	   */
#define	ES_REPEATER	flag(7)		/* Repeater test	   */


/*
 *--------------------------------------------------------------------------
 *	Reset Ack msg
 *--------------------------------------------------------------------------
 */ 

#define PROM_VERS_SIZE		4	/* xxxx.xxxx */
#define DATETIME_STAMP_SIZE	6	/* YY-MM-DD hh:mm:ss */
#define	RESERVE_8051_BYTES	8
#define ADDITIONAL_INFO_SIZE    28      /* ATM PMD SPROM, */
#define MPU_XTRA_DIAG_BYTES	36	/* 68030 (ATM/FDDI) diag 
					 * DO NOT change size 
					 * will affect ATM card - iris
					 */

typedef struct _RESET_ACK {
	SCPHDR	hdr PAK;
	uChar	mlcpBldVersion PAK;
	uChar   serEProm [SER_EEPROM_BSIZE] PAK;
	uInt32  PromVers PAK;
	uChar	PromDateTimeStamp [DATETIME_STAMP_SIZE] PAK;
	uChar	cardFeatureData[CARD_FEATURE_TABLE_BYTES] PAK;
	uInt16	cardPresentMask PAK;	/* set by MCP only */
	uInt16	diag_global PAK;
	uInt32	saint_detail PAK;
	uInt32	pkt_buf_detail PAK;
	uInt32	repeater_detail PAK;
	uInt32	big_pkt_buf_flags PAK;	/* large PkyBuf flags (bit/port) */
	uChar	reserved_8051[RESERVE_8051_BYTES] PAK;
	/* DO NOT CHANGE size of     mpu_xtra 
	 * DO NOT ADD fields between mpu_xtra and additional_info 
	 * - will affect ATM card - iris
	 */
	uChar	mpu_xtra_diag_result[MPU_XTRA_DIAG_BYTES] PAK; /* union FDDISTATUS,ALCSTATUS */
	uChar   additional_info[ADDITIONAL_INFO_SIZE] PAK; /* union PMDSERPROM, ... */
				/* 244 bytes so far 8/2/95 */
		/* ATM/FDDI expansion - add more fields here */
} RESET_ACK;

/*--------------------------------------------------------------------------
 *	Set Port Features 	
 *------------------------------------------------------------------------*/

/***	Feature Numbers: these must match the Feature Bits below */
/***	NOTE: port_enable must be located at bit 0.		 */

enum	port_features		/* Numbers for bit positions	*/
	{
	  port_enable=0,	/* enable, default config       */
	  port_speed,		/* 10, 100, or MII auto sensing */ /*Saint*/
	  port_duplex,		/* Half/full duplex		*/ /*Saint*/
	  port_huge,		/* Huge frame enable/disable	*/
	  port_isl,	        /* ISL on/off			*/ /*Saint*/
	  port_loopback,        /* loopback enable/disable	*/ /*Saint*/
	  port_brq_priority,    /* Bus Req Priority		*/
	  port_high_water,      /* High-water mark		*/
	  port_pb_divider,	/* Pkt Buffer divider		*/

	  MAX_PORT_FEATURES	/* auto define for total	*/
	};


/***	Feature bits	*/

#define	PORT_ENABLE		flagL(port_enable)
#define   PE_UNRESET			0x01 /* 1: enable port  */
#define   PE_DEFAULT			0x02 /* default config  */
#define   PE_ENABLE                     PE_DEFAULT /* For clarity */
	/*
	 * Release port from reset and configure port with
	 * default settings.  (See 8051/common/st_init.h
	 * for deatils.  See also SCP_ENADIS_MODPORT msg
	 * struct ENABLE_DISABLE_MSG for similar features.)
	 * Use ENABLE_DISABLE_MSG or SET_PORT_STATE to
	 * disable a port.
	 */

#define PORT_SPEED		flagL(port_speed) /* Saint only */
#define   PS_10_MBPS			0x00 /* 10 Mbps		*/
#define   PS_100_MBPS			0x01 /* 100 Mbps	*/
#define   PS_MII			0x02 /* MII auto sense  */

#define	PORT_DUPLEX		flagL(port_duplex) /* Saint only*/
#define   PD_HALF			0x00 /* half duplex	*/
#define   PD_FULL			0x01 /* full duplex	*/

#define PORT_HUGE		flagL(port_huge)
#define   PH_HUGE_OFF			0x00 /* Huge pkt OFF	*/
#define   PH_HUGE_ON			0x01 /* Huge Pkt ON	*/

#define PORT_ISL		flagL(port_isl) /* Saint only 	*/
#define   PI_ISL_OFF			0x00 /* ISL port OFF	*/
#define   PI_ISL_ON			0x01 /* ISL port ON	*/

#define PORT_LOOPBACK		flagL(port_loopback) /* Saint only */
#define   PL_EXT_LOOPBACK_ON		0x01 /* 1 = ON, 0 = off */
#define   PL_INT_LOOPBACK_ON		0x02 /* 2 = ON, 0 = off */

	/*
	 * Notes:
	 * (1) Turning external	loopback ON  changes DUPLEX to FULL.
	 * (2) Turning external	loopback OFF changes DUPLEX to HALF.
	 * (3) Internal loopback does not work on rev-1 Saint.
	 */

#define PORT_BRQ_PRIORITY	flagL(port_brq_priority)
#define   PBP_LOW			0x00 /* priority 1	*/
#define   PBP_HIGH			0x01 /* priority 2	*/

#define PORT_HIGH_WATER		flagL(port_high_water)
#define   PHW_DEFAULT_SMALL		0x03 /* small (32K x48) */
#define   PHW_DEFAULT_LARGE		0x03 /* large (128Kx48) */
	/*
	   upper 8 bits of PB_ADDR(17)
	   threshold for priority 3.
	*/

#define PORT_PB_DIVIDER		flagL(port_pb_divider)
					     /* small (32K x48) */
#define   PPD_DEFAULT_SMALL		0x07 /* 28K for Tx	*/
					     /* large (128Kx48) */
#define   PPD_DEFAULT_LARGE		0x1C /* 112K for Tx	*/
	/*
	   No. of 4K (0x100) PB blocks slated for Tx,
	   the rest is for Rx.  The range is from 01
	   to 07, inclusive.  Default is 28K/4K for
	   Tx/Rx for small PB (32Kx48), 112K/16K for
	   Tx/Rx for large PB (128Kx48).
	*/

typedef struct _SET_PORT_FEATURES {
	SCPHDR	hdr;
	uChar	cmdStatus;
	uInt32	port_mask;
	uInt32  feature_mask;

       	/*
	   Each feature_data byte below corresponds to a bit "set"
	   in feature_mask above.  If the bit is not set, its
	   corresponding byte does not show up in the message.
	*/

	uChar	feature_data[32]; /* variable length depending on feature_mask */

	/*
	   In the case of bad SCP return status, e.g., SCP_NOTSUPPORTED,
	   The variable-length feature_data array is followed by equal
	   no. of bytes for status, one byte per feature.
	*/

} SET_PORT_FEATURES;
 
 
/*--------------------------------------------------------------------------
 *     	Module/Port  Enable/Disable message 
 *      NOTE: This message henceforth has to be understood as the 
 *      MODULE/CHANNEL ENABLE/DISABLE message. This message will and 
 *      should be used for ENABLE/DISABLE of Modules only. The only 
 *      PORT ENABLE/DISABLE command that will be supported in future 
 *      8051 code will be the ENABLE/DISABLE of the port DMA channels
 *      only. Everybody is henceforth warned.
 *------------------------------------------------------------------------*/
typedef struct _ENABLE_DISABLE_MSG {
        /*
        // SCP header
        */
	struct  scphdr  hdr; 

        /*
        // Command (68K to M/LCP), Status the otherway
        // See below for type of Commands and Statuses
        */
	uChar	cmdStatus PAK;

	/*
        // SubCommand, bit mask
        */
	uInt16 SubCommandBits PAK;

	/*
        // Module or Port mask, For messages related to module this 
        // is a port mask and for port related messages this is a 
        // port mask. 
        */
	uInt32	ModuleOrPortMask PAK;

} ENABLE_DISABLE_MSG, *PENABLE_DISABLE_MSG;  

/*
// Command & Status defines
*/
#define COMMAND_PORT_ENABLE	0x01 /* Enables, SAINT */
#define COMMAND_PORT_DISABLE	0x02 /* Disable, SAINT */
#define COMMAND_MODULE_ENABLE	0x03 /* Enables, LTL, CBL, Card to SwBus */
#define COMMAND_MODULE_DISABLE	0x04 /* Disables LTL, CBL, Card to SwBus */
#define COMMAND_MODULE_RESTART  0x05 /* for MCP to hard reset other LCPs */
				     /* for LCP to soft reset themselves */
#define COMMAND_MODULE_ON_LINE	0x06 /* Tell card to go on line		 */
				     /* MCP: start polling LCPs; LCP: nop*/
#define COMMAND_MODULE_OFF_LINE	0x07 /* Tell card to go off line	 */
				     /* MCP: stop polling LCPs; LCP: nop */
#define COMMAND_DIAG_ON_LINE	0x08 /* Go online w/o touching big LED	 */
#define COMMAND_DIAG_OFF_LINE	0x09 /* Go offline w/o touching big LED	 */

/* response code to above commands */

#define STATUS_PORT_ENABLED     0x21
#define STATUS_PORT_DISABLED    0x22
#define STATUS_MODULE_ENABLED   0x23
#define STATUS_MODULE_DISABLED  0x24
#define STATUS_MODULE_RESTARTED 0x25
#define STATUS_MODULE_ON_LINE	0x26
#define STATUS_MODULE_OFF_LINE	0x27
#define STATUS_DIAG_ON_LINE	0x28
#define STATUS_DIAG_OFF_LINE	0x29
/*
// Sub Commands bits
*/
#define SUBCOMMAND_ENDISABLE_SB2PB     0x0001
#define SUBCOMMAND_ENDISABLE_PB2SB     0x0002
#define SUBCOMMAND_ENDISABLE_MC2PB     0x0004
#define SUBCOMMAND_ENDISABLE_PB2MC     0x0008
#define SUBCOMMAND_SET_SAINT_DEFAULTS  0x0010
#define SUBCOMMAND_DISABLE_WHOLE_SAINT 0x0010


/*
 *--------------------------------------------------------------------------
 *      LTL Config msg
 *--------------------------------------------------------------------------
 */
typedef struct  ltlndx {
        uInt16  index;
        uInt32  ports;
} LTLNDX;

typedef union ltlmsg
{
        LTLNDX  ltl[MAX_SCP_PKT_DATA_SIZE / sizeof(LTLNDX)];
        uChar   buffer[MAX_SCP_PKT_DATA_SIZE];
}LTLMSG;

typedef struct ltlcfg {
        SCPHDR	hdr;
        LTLNDX  ltl[MAX_SCP_PKT_DATA_SIZE / sizeof(LTLNDX)] PAK;
} LTLCFG;


/*--------------------------------------------------------------------------
 *	CBL Config msg
 *	Note. Ports is currently 32 bits and there is 2bits per port,
 *	for now this works fine and is convenient, because CBL will 
 *	only be present on 100Mbit cards and no more than 12 ports 
 *	will be on one card, so 32 bits will work fine.
 *	The RAM layout is:
 *		VLANS(1024) * stat (2 bits) * # of ports (12).	(JimL)
 *------------------------------------------------------------------------*/
    
typedef struct  cblvlan {
        uInt16	vlan;
        uInt32	ports;
} CBLVLAN;
 
typedef struct	cblcfg {
        SCPHDR  hdr;
       	CBLVLAN	cblv[MAX_SCP_PKT_DATA_SIZE / sizeof(CBLVLAN)] PAK;
} CBLCFG;
 
 
/*
 *--------------------------------------------------------------------------
 *      Get Status Message
 *--------------------------------------------------------------------------
 */ 
  
typedef struct	getstat { 
        struct  scphdr  hdr; 
        uChar	modStat PAK;
        uChar	portStat[MAXPORTS] PAK;
} GETSTAT;  



/*
 *--------------------------------------------------------------------------
 *      Peek/Poke Memory Message
 *--------------------------------------------------------------------------
 */ 
  
#define	MAXMEMDIAGSIZE	243

typedef struct	mem_diag { 
        struct  scphdr  hdr;
	uChar	cmdStatus;
        uChar	byteCount;	/* no. of bytes to get/set */
	uChar	startAddr[3];	/* starting address:
				   Byte#0 = Memory Type 1 - XRAM, 2 - CODE
				   Byte#1 = Memory Addr Hi
				   Byte#2 = Memory Addr Lo   */
	uChar	dataBuf[MAXMEMDIAGSIZE];  /* data buffer for get/set */
} MEM_DIAG;  

/*
 *--------------------------------------------------------------------------
 *      SCP_GET_COLL_COUNTS Message
 *--------------------------------------------------------------------------
 */ 

struct saint_error { 
	uChar	rxl_err;
	uChar	txl_err;
	uChar	tx_overflow;
	uChar	sbifr_underflow;
};

typedef struct	saint_error_count_msg { 
        struct  scphdr  hdr;
	uChar	cmd_status;
	uChar   padding;
	uChar	txl_err_h;
	uChar	txl_err_m;
	uChar	txl_err_l;
	uChar	txn_err_l;
	uChar	txf_err_l;
	struct  saint_error saint[24];
} SAINT_ERROR_COUNT_MSG;

#define CLEAR_COUNT_GENERIC_LENGTH  0x07 /* without scphdr and saint_error */
#define CLEAR_COUNT_MASK  0x01   /* if cmd_status is bit 1, read-clear counts */

/*
 *--------------------------------------------------------------------------
 *      Board Present msg
 *--------------------------------------------------------------------------
 */ 
  
typedef struct 
{ 
        SCPHDR  hdr; 
        uInt32	boards PAK;	/* bit string of boards present	*/
				/*  bit 0 - board 1, bit 1 board 2 etc 	*/
} BRDPRES;  

/*
 *--------------------------------------------------------------------------
 *     	Download Messages 
 *--------------------------------------------------------------------------
 */
/*---	There is no specific format for MCP/LCP download messages, 
 *	the control messages (setpage, dochksum, etc) use only the SCPHDR with 
 *	zero data length and the 
 *	download code is put the FMSG format, see the download OPCODES for more
 *	info.
*/  

/**************************************************************************
*
*	SCP Statistics message format
*
***************************************************************************/
typedef struct lcp_scp_stats
{
    struct  scphdr  hdr;
    uChar   good_cksum_pkts_rcvd;
    uChar   bad_cksum_pkts_rcvd;
    uChar   tot_pkts_xmitted;
    uChar   bad_addr_pkts_rcvd;
    uChar   tot_cts_rcvd; /* No of times the lcp got a chance to xmit a pkt*/
    uChar   tot_cts_reqs; /* total no of times the lcp requested the MCP */
			  /* to xmit a pkt.				*/
    uChar   tot_timeouts;
    uChar   tot_overflow_pkts;
}LCP_STATS;

typedef struct mcp_scp_stats
{
    struct  scphdr  hdr;
    uChar   good_pkts_rcv_from_lc;
    uChar   bad_cksum_pkts_rcvd;
    uChar   tot_pkts_xmit_to_lc;
    uChar   tot_cts_sent;
    uChar   tot_msgs_rcv_frm_68k;
    uChar   tot_msgs_sent_to_68k;
    uChar   tot_timeouts;
}MCP_STATS;


/*--------------------------------------------------------------------------
 *
 *     SAINT related messages
 *
 *--------------------------------------------------------------------------
 */

/*
 *--------------------------------------------------------------------------
 *	Dump Saint Register Message
 *	Dump MII PHY Registers
 *--------------------------------------------------------------------------
 */
#define	SAINT_REG_DUMP_START	0x48

typedef struct _SAINT_DUMP_REGS {

	SCPHDR	scphdr;
	uChar	Status;		/* sub-cmd: 0x01=Get Saint Regs */
				/*          0x11=Get MII Regs   */
				/*          0x12=Set MII Regs   */
	uChar	SaintId;
	uChar	regData [0xFF - SAINT_REG_DUMP_START + 1];
		/* sub-cmd: (0x01) output: Saint Regs Dump	*/
		/*          (0x11) output: MII Regs Dump (u16)	*/
		/*          (0x12) input: Byte #0 = MII Reg No.	*/
		/*		          Byte #1,#2 = Reg (u16)*/

} SAINT_DUMP_REGS, *PSAINT_DUMP_REGS;

#define	SUBCMD_SAINT_REGS_DUMP	0x01
#define	SUBCMD_MII_REGS_DUMP	0x11
#define	SUBCMD_SET_MII_REG	0x12

#define	MAX_MII_REG		0x1C /* MII Reg range 00-1C */


/*
// ========================================================
// This structure is used by the GET_SAINT_MODE_BY_MASK & 
// the SET_SAINT_MODE_BY_MASK messages to get/set multiple 
// SAINTs in the same Mode.
// NOTE: The Get SAINT mode by MASK request in this format 
// will cause a response in the SAINT_MODE_BY_ID format.
// ========================================================
*/

typedef struct _SAINT_MODE_BY_MASK {

     SCPHDR scphdr;

     /*
     // Status of request, Should be set to 0xFF at the time 
     // of request and will be explicitly written by LCP/MCP
     // after the request is completed.
     */
     uChar Status;

     /*
     // Define the SAINTS that need to be set in the 
     // Mode defined by the Mode Bits
     // 1 bit per SAINT upto a MAX of 32 SAINTS. If we have 
     // more in the future use the SAINT_MODE_BY_ID msg
     */
     uInt32 SaintsMask;

     /*
     // Mode Bits 
     */
     uInt32 ModeBits;

} SAINT_MODE_BY_MASK, *PSAINT_MODE_BY_MASK;




typedef struct _SAINT_ID_AND_MODE {
     /*
     // SAINT ID
     */
     uChar SaintId;

     /*
     // Mode Bits 
     */
     uInt32 ModeBits;

} SAINT_ID_AND_MODE, *PSAINT_ID_AND_MODE;


/*
// SAINT Mode bit definitions
*/
#define SAINT_MODE_DEFAULT             0x00000001  /*init SAINT regs, but diabled port*/
#define SAINT_MODE_EXTERNAL_LOOPBACK   0x00000002  /* LP through PMD*/
#define SAINT_MODE_INTERNAL_LOOPBACK   0x00000004  /* LP before PMD*/
#define SAINT_MODE_PKTBUF_LOOPBACK     0x00000008  /* LP through PKT buf (S/W)*/


/*
// This structure is used by the GET_SAINT_MODE_BY_ID, 
// SET_SAINT_MODE_BY_ID request messages and also the 
// GET_SAINT_MODE_BY_MASK response message to get/set 
// multiple SAINTs in different Modes.
*/

typedef struct _SAINT_MODE_BY_ID {

     SCPHDR scphdr;

     /*
     // Status of request, Should be set to 0xFF at the time 
     // of request and will be explicitly written by LCP/MCP
     // after the request is completed.
     */
     uChar Status;

     /*
     // Number of SAINT ID & MODE combinations being 
     // specified below
     */

     uChar NoOfSaintsSpecified;

     /*
     // Id and Mode combinations
     */
     SAINT_ID_AND_MODE IdAndMode[(MAX_SCP_PKT_DATA_SIZE - 2)/sizeof(SAINT_ID_AND_MODE)];

} SAINT_MODE_BY_ID, *PSAINT_MODE_BY_ID;




typedef struct _SAINT_REG_AND_VALUE {

     /*
     // Register Offset
     */
     uChar RegOffset;

     /*
     // Register Value
     */
     uChar RegValue;

} SAINT_REG_AND_VALUE, *PSAINT_REG_AND_VALUE;


/*
// This structure is used by the GET_SAINT_REGS_BY_MASK & 
// the SET_SAINT_REGS_BY_MASK messages to get/set multiple 
// SAINTs and the corresponding registers to the same value.
*/

typedef struct _SAINT_REGS_BY_MASK {

     SCPHDR scphdr;

     /*
     // Status of request, Should be set to 0xFF at the time 
     // of request and will be explicitly written by LCP/MCP
     // after the request is completed.
     */
     uChar Status;

     /*
     // Define the SAINTs that need to have their registers set 
     //to the same values
     // 1 bit per SAINT upto a MAX of 32 SAINTS. If we have 
     // more in the future use the SAINT_MODE_BY_ID msg
     */
     uInt32 SaintsMask;

     /*
     // Number of REG_OFFSET & REG_VALUE combinations values 
     // being specified below.
     */
     uChar NoOfRegistersSpecified;

     /*
     // Register offset and Value combinations
     */
     SAINT_REG_AND_VALUE RegAndvalue[(MAX_SCP_PKT_DATA_SIZE - 6)/sizeof(SAINT_REG_AND_VALUE)];

} SAINT_REGS_BY_MASK, *PSAINT_REGS_BY_MASK;



/*
// This structure is used by the GET_SAINT_REGS_BY_ID & 
// the SET_SAINT_REGS_BY_ID messages to get/set multiple 
// SAINTs and its registers with different values.
*/

/* sub command definitions */
#define	SAINT_SETBITS		1
#define SAINT_CLRBITS		2
#define SAINT_SETREGS		3
#define SAINT_GETREGS		4

typedef struct _SAINT_REGS_BY_ID {

   SCPHDR scphdr;

   /*
   // sub cpmmand or return status
   */
   uChar cmdStatus;

   /*
   // Number of SAINT REGS & VALUE combinations being 
   // specified below
   */
   uChar NoOfSaintsSpecified;

   /*
   // Specify which SAINT and which registers within the SAINT
   // There could be multiple of these
   */   
   struct WHICH_SAINT {
     
       /*
       // SAINT ID
       */
       uChar SaintId;

       /*
       // Number of REG_OFFSET & REG_VALUE combinations values 
       // being specified below.
       */
       uChar NoOfRegistersSpecified;

       /*
       // Id and Mode combinations
       // Could be more than one of these
       */
       SAINT_REG_AND_VALUE RegAndValue[1];

   } WhichSaint;

} SAINT_REGS_BY_ID, *PSAINT_REGS_BY_ID;



/*
// SAINT/SAGE Send/Receive packet structures and defines
*/
#define PACKET_ON_NETWORK      0x00
#define PACKET_ON_SW_BUS       0x01

/*
// Status defines for Transmit/Receive packet
*/
#define PACKET_TRASNMIT_DONE       0x00
#define PACKET_TRANSMIT_FAIL       0x01


/*
//
// SCP Transmit packet structure
//
*/
typedef struct _TRANSMIT_PACKET {

   SCPHDR scphdr;
   
   /*
   // Status of Transmit packet
   */
/*   uChar Status;*/
      
   /*
   // Port (SAINT/SAGE) to transmit on, maybe be redundant in the 
   // case of ATM/FDDI card but is needed for MCP/LCPs.
	 // This MUST be a valid port # (0 - > max saint#)
   */
   uChar PortId;
   
   /*
   // Direction to transmit packet (i.e on Network or Switching bus)
   */
   uChar Direction;
   
   /*
   // The packet to be transmitted. 
   // NOTE: This packet data is the Synergy Packet (i.e regular 
   // Ethernet packet + Synergy Header.
   */
   uChar PacketData[MAX_SCP_PKT_DATA_SIZE - 2];
   
} TRANSMIT_PACKET, *PTRANSMIT_PACKET;


/*
//
// SCP Receive packet structure
//
*/
typedef struct _RECEIVE_PACKET {

   SCPHDR scphdr;
   
   /*
   // Status of Receive packet
   */
   uChar Status;
      
   /*
   // Port (SAINT/SAGE) to receive on, maybe be redundant in the 
   // case of ATM/FDDI card but is needed for MCP/LCPs
   */
   uChar PortId;
   
   /*
   // Direction to receive packet (i.e on Network or Switching bus
   */
   uChar Direction;
   
   /*
   // The packet to be received. 
   // NOTE: This packet data is the Synergy Packet (i.e regular 
   // Ethernet packet + Synergy Header.
   */
   uChar PacketData[MAX_SCP_PKT_DATA_SIZE - 3];
   
} RECEIVE_PACKET, *PRECEIVE_PACKET;



/*--------------------------------------------------------------------------
 *	SAINT Counters msgs
 *------------------------------------------------------------------------*/
    
typedef struct
{
    SCPHDR  hdr;
	uChar	status;
	uChar	SaintNo;	
	uChar	reserved;
    SAINT_COUNTERS	scnts PAK;
} SAINTCNT;
    
typedef struct
{
        SCPHDR  hdr;
} SAINTCLR;


/*
//
// SCP defines & structure to Read/Write EEPROM data
// Used with SCP_READ_SEEPROM and SCP_WRITE_SEEPROM 
// opcodes
//
*/
typedef struct _READ_WRITE_SEEPROM {

   SCPHDR scphdr;
   
   /*
   // Status of Read/Write operation (see below for 
   // the defines to use)
   */
   uChar Status;
      
   /*
   // The entire SEEPROM data structure
   */
   struct eeprom_data seeprom_data;
   
} READ_WRITE_SEEPROM, *PREAD_WRITE_SEEPROM;

/*
// READ or WRITE of SEEPROM was good, i.e. data valid 
// was good AND checksum was good
*/
#define STATUS_SEEPROM_GOOD    0 

/*
// READ or WRITE of SEEPROM was bad, i.e. data valid 
// was bad OR checksum was bad.
//
// NOTE: even if the status is bad the entire structure 
// read from the SEEPROM should be returned in the message
*/
#define STATUS_SEEPROM_BAD     1  /* bad data_valid or checksum in eeprom*/
#define STATUS_SEEPROM_BAD_LEN 2  /* incorrect write length requested in msg */

#define SEEPROM_STATUS_OFFSET  (SCP_HEADER_LENGTH + 0)
#define SEEPROM_DATA_OFFSET    (SCP_HEADER_LENGTH + 1)

/*
   _SET_PORT_STATE_CMD and _SET_PORT_STATE_RESP 
*/
/* PORT STATE's  as per Spanning Tree */
#define    PORT_DISABLED          0
#define    PORT_LISTENING         1
#define    PORT_LEARNING          2
#define    PORT_FORWARDING        3
#define    PORT_BLOCKING          4

typedef struct  portstvlan {
	uChar   port_num PAK; 	/* 0 - 23 */
        uInt16  vlan_num PAK;	/* 0 - 1023 */ 
        uChar   port_state PAK; /* values as defined above */
} PORT_ST_VLAN;

typedef struct _SET_PORT_STATE {
        SCPHDR  hdr PAK;
       	PORT_ST_VLAN portst[MAX_SCP_PKT_DATA_SIZE / sizeof(PORT_ST_VLAN)] PAK;
} SET_PORT_STATE;

#define STATUS_SET_PORT_STATE_GOOD	0
#define STATUS_SET_PORT_STATE_BAD 	1
#define SET_PORT_STATE_RESP_LEN  	1 /* length of response from 8051 */

typedef struct _SET_PORT_STATE_RESP {
        SCPHDR  hdr;
        uChar   status PAK;
} SET_PORT_STATE_RESP;

/*
 * SCP_SET_VLAN_PARAMETERS
 *
 */
typedef struct  {
    uInt16  vlan_num PAK;
#define VTP_ADD        1
#define VTP_DELETE     2
#define VTP_MODIFY     3
    uChar   vlan_info_mode PAK;     /* add= 1, delete = 2, modify =3 */
    uChar   name_len PAK;
    uChar   name[32] PAK;
    uInt16  mtu PAK;
/* VLAN TYPE definition as per VTP */
#define VTP_VLAN_ETH_TYPE        1
#define VTP_VLAN_FDDI_TYPE       2
#define VTP_VLAN_TR_TYPE         3
#define VTP_VLAN_FDDI_NET_TYPE   4
#define VTP_VLAN_TR_NET_TYPE     5
    uChar   type PAK;
    uInt32  said PAK;
#define VTP_VLAN_OPERATIONAL_STATE        1
#define VTP_VLAN_SUSPENDED_STATE          2
    uChar   vlan_state PAK; /* 1 = active, 2 = suspended */
    uInt16  ring_number PAK;
    uInt16  bridge_number PAK;
    uInt16  parent_vlan PAK;
#define VTP_VLAN_STP_SRT         1
#define VTP_VLAN_STP_SRB         2
    uInt16  stp_type PAK;
    uInt16  translation_vlan1 PAK;   /* vlan id of other vlan */
    uInt16  translation_vlan2 PAK;    /* vlan id of other vlan */
} VLAN_PARAMS;
 
typedef struct {
        SCPHDR  hdr PAK;
#define SET_VLAN_PARAMS_MSG_REQUEST 0
    uChar   msg_status PAK; /* 0 = Request */
#define SET_VLAN_PARAMS_MSG_VERSION 1
    uChar   msg_version PAK; /* initial version of message will be 1 */
    VLAN_PARAMS vlanparams PAK;
} SET_VLAN_PARAMETERS_CMD;
 
 
typedef struct {
        SCPHDR  hdr PAK;
#define SET_VLAN_PARAMS_MSG_REPLY   1
#define SET_VLAN_PARAMS_MSG_INC_VER 2
    uChar   msg_status PAK; /* 0 = Request, 1 = Reply, 2 = incompatible version,
 
                        0xAA = NOT_SUPPORTED*/
#define SET_VLAN_PARAMS_SUCCESS     0
#define SET_VLAN_PARAMS_FAILURE     1
    uChar   status PAK; /* 0 = Success, 1 = Failure */
} SET_VLAN_PARAMETERS_RESP;


/*
  Supervisor MAC Address 
*/
typedef struct _SUPV_MAC_IP_ADDR  {
        SCPHDR  hdr PAK;
	uChar	MAC_addr[6] PAK;
	uChar	IP_addr[4] PAK;
} SUPV_MAC_IP_ADDR;

/*
 * ATM specific startup message
 */
typedef struct _ATM_INSTANCE {
  SCPHDR hdr PAK;
  uChar   ATMCardInstanceNumber PAK;
  uChar   ATMMACAddresses[6] PAK; /* First of 16 MAC addr for ATM */
  uInt32  IfStartIndex PAK ;
  uInt32  IfIndexRange PAK ;
} ATM_INSTANCE_MSG, *PATM_INSTANCE_MSG;

/*  ALC status 
 *
 * struct ALCSTATUS to overlay the mpu_xtra_diag_results[10] of the RESET_ACK
 * and define specific ATM data:
 * Note: RESTART_ACK shares the same structure as RESET_ACK
 */


typedef struct _ALC_STATUS
{
	uInt16  ACPBootHiVersion; /* Fw */
	uInt16  ACPBootLoVersion;
	uInt16  ACPSwHiVersion;	/* Sw */
	uInt16  ACPSwLoVersion;
	uInt16  ACPBoot2HiVersion; /* Fw2 */
	uInt16  ACPBoot2LoVersion;
	uInt16  ACPIosBootHiVersion; /* Fw3 */
	uInt16  ACPIosBootLoVersion;
	uInt16  RxSARSwHiVersion; /* Sw2 */
	uInt16  RxSARSwLoVersion;
	uInt16  TxSARSwHiVersion; /* Sw3 */
	uInt16  TxSARSwLoVersion;
	uChar   AcpErrStat1;
	uChar   AcpErrStat2;
	uChar   AcpErrStat3;
	uChar   AcpErrStat4;
	uChar   RxSARErrStat1;
	uChar   RxSARErrStat2;
	uChar   TxSARErrStat1;
	uChar   TxSARErrStat2;
        uChar	ACPBldVersion;
} ALCSTATUS;

typedef struct _ALC_DIAG	/* make sure this matches above */
{
	uChar   AcpErrStat1;
	uChar   AcpErrStat2;
	uChar   AcpErrStat3;
	uChar   AcpErrStat4;
	uChar   RxSARErrStat1;
	uChar   RxSARErrStat2;
	uChar   TxSARErrStat1;
	uChar   TxSARErrStat2;
        uChar	ACPBldVersion;
} ALCDIAG;

/*--- ACPErrorStat1 ---*/
#define  ACP_ROM				flag(0)		/* ROM checksum */
#define  ACP_DRAM				flag(1)		/* DRAM         */
#define  ACP_FLASH_OK		flag(2)		/* flash programability */
#define  ACP_FLASH_CODE		flag(3)		/* flash code validity   */
#define  ACP_DPRAM			flag(4)		/* ACP/LCP dual-port RAM */
#define  ACP_NVRAM			flag(5)		/* NVRAM  */
#define  ACP_TXSAR_SHRAM	flag(6)		/* ACP/TxSAR shared RAM */
#define  ACP_RXSAR_SHRAM	flag(7)		/* ACP/RxSAR shared RAM */

/*--- ACPErrorStat2 ---*/
#define  ACP_FRAME_BUF		flag(0)     /* ACP/SAR frame buffer  */
#define  ACP_PMD_EEPROM		flag(1)     /* ACP PMD serial EEPROM */
#if	1
#define	ACP_InDiagMode		flag(2)		/* 1: ACP in offline diag mode */
#else
#define  ACPErrSt2_2			flag(2)     /* not used */
#endif	/* ZERO */
#define  ACPErrSt2_3			flag(3)     /* not used */
#define  ACPErrSt2_4			flag(4)     /* not used */
#define  ACPErrSt2_5			flag(5)     /* not used */
#define  ACPErrSt2_6			flag(6)     /* not used */
#define  ACPErrSt2_7			flag(7)     /* not used */

/*--- ACPErrorStat3 ---*/
#define  ACP_RxSAR_COMM		flag(0)     /* ACP/RxSAR comm channel failed */
#define  ACP_TxSAR_COMM		flag(1)     /* ACP/TxSAR comm channel failed */

/*
 * Loopback Test 1:
 * 	ACP->TSAR->NTC->RSAR->ACP
 */
#define  ACP_SAR_LOOPBK1	flag(2)     

/*
 * Loopback Test 2:
 * 	ACP -> TSAR-> PMD -> RSAR -> ACP
 */
#define  ACP_SAR_LOOPBK2	flag(3)     

/*
 * Loopback Test 3:
 * 	ACP-> RxSAR -> BIGA -> SAGE -> LCP -> ACP
 */
#define  ACP_SAR_LOOPBK3	flag(4)     

/*
 * Loopback Test 4:
 * 	ACP -> LCP -> SAGE -> BIGA -> TxSAR -> PMD -> RxSAR -> ACP
 */
#define  ACP_SAR_LOOPBK4	flag(5)     

#define  ACPErrSt3_6			flag(6)     /* NOT DEFINED  */
#define  ACPErrSt3_7			flag(7)     /* NOT DEFINED  */

/*--- ACPErrorStat4 ---*/  				/* NOT DEFINED !! */

/*--- RxSARErrorStat1 ---*/
#define  RxSAR_SPRAM			flag(0)     /* RxSAR Secondary Port RAM  */
#define  RxSAR_FRAME_BUF	flag(1)     /* RxSAR Frame Buffer        */
#define  RxSAR_VCR_DMA		flag(2)     /* RxSAR VCR DMA             */
#define  RxSAR_BIGAACC		flag(3)     /* */
#define  RxSAR_ErrSt1_4		flag(4)     /*  */
#define  RxSAR_ErrSt1_5		flag(5)     /*  */
#define  RxSAR_ErrSt1_6		flag(6)     /*  */
#define  RxSAR_ErrSt1_7		flag(7)     /*  */

/*--- RxSARErrorStat2 ---*/				/* NOT DEFINED !! */


/*--- TxSARErrorStat1 ---*/
#define  TxSAR_SPRAM       flag(0)     /* TxSAR Secondary Port RAM  */
#define  TxSAR_FRAME_BUF   flag(1)     /* TxSAR Frame Buffer        */
#define  TxSAR_VCR_DMA     flag(2)     /* TxSAR VCR DMA             */
#define  TxSAR_BIGAACC     flag(3)     /*  */
#define  TxSAR_ErrSt1_4    flag(4)     /*  */
#define  TxSAR_ErrSt1_5    flag(5)     /*  */
#define  TxSAR_ErrSt1_6    flag(6)     /*  */
#define  TxSAR_ErrSt1_7    flag(7)     /*  */

/*--- TxSARErrorStat2 ---*/
#define  TxSAR_CAM0        flag(0)     /* TxSAR CAM chip 0 */
#define  TxSAR_CAM1        flag(1)     /* TxSAR CAM chip 1 */
#define  TxSAR_CAM2        flag(2)     /* TxSAR CAM chip 2 */
#define  TxSAR_CAM3        flag(3)     /* TxSAR CAM chip 3 */
#define  TxSAR_ErrSt2_4    flag(4)     /*  */
#define  TxSAR_ErrSt2_5    flag(5)     /*  */
#define  TxSAR_ErrSt2_6    flag(6)     /*  */
#define  TxSAR_ErrSt2_7    flag(7)     /*  */

/* Look for PMDSERPROM in syn_ser_eeprom.h */

/* -------------- * CDP MESSAGE STRUCTURE * --------------- */
 
#define STATUS_SET_CDP_MATCH_ADDR_GOOD	0
#define STATUS_SET_CDP_MATCH_ADDR_BAD 	0xff
#define SET_CDP_MATCH_ADDR_RESP_LEN  	1 /* length of response from 8051 */

typedef struct _SET_CDP_MATCH_ADDR {
        SCPHDR  hdr PAK;
        uChar   pNo PAK;
        uChar   macAddr[6] PAK;
	/* Repeat the bove two fields for more ports */
} SET_CDP_MATCH_ADDR;

#define	CDP_PORT_ADDR_SIZE	(sizeof(SET_CDP_MATCH_ADDR) - sizeof(SCPHDR))

typedef struct _SET_CDP_MATCH_ADDR_RESP {
        SCPHDR  hdr PAK;
        uChar   status PAK;
} SET_CDP_MATCH_ADDR_RESP;
 

/* -------------- * MCP MAC Address for SSA'a * --------------- */

typedef struct _SET_MCP_MAC_ADDR {
        SCPHDR  hdr PAK;
        uChar   filler;
        uChar   macAddr[6] PAK;	    /* SSA5 (MSB) - SSA0 (LSB) */
} SET_MCP_MAC_ADDR;

typedef struct _SET_MCP_MAC_ADDR_RESP {
        SCPHDR  hdr PAK;
        uChar   status PAK;
} SET_MCP_MAC_ADDR_RESP;
 

/* -------------- * Set SPAN Monitor Command * --------------- */

typedef struct _SPAN_MONITOR {
	SCPHDR	scphdr;
	U8	cmd_status;		/* 0=enable, 1=disable */
	U8	direction;		/* flag(0)=Tx, flag(1)=Rx */
	U8	monitored_module;	/* slot 0 - 4 */
	U8	monitored_port;		/* port 0 - 23 (segment) */
	U8	monitoring_module;	/* slot 0 - 4 */
	U8	monitoring_port;	/* port 0 - 23 (segment) */
} SPAN_MONITOR;

#define	SET_SPAN_ENABLE		0x01	/* Enable sourc/dest port */
#define	SET_SPAN_DISABLE       	0x02	/* Disable sourc/dest port */

typedef struct _SPAN_MONITOR_RESP {
        SCPHDR  hdr PAK;
        uChar   status PAK;		/* 0 = OK */
} SPAN_MONITOR_RESP;
 
/* -------------- * Set SPAN Monitor Command * --------------- */



/* ----------- Intelligent linecard flash download status ---------- */
/* Unsolicited message to report to NMP the status of flash download */
typedef struct FlashDownLoadStatus {
        SCPHDR  hdr;
        uChar   DnLdStatus;     /* SCPSTAT_OK / SCPSTAT_FAILED */
} FLASH_DNLD_STATUS;


/*
 *-------------------------------------------------------------------------- 
 *	*****   MONTEREY MESSAGES   *****
 *--------------------------------------------------------------------------
 */

typedef struct _MONTEREY_STATUS {
        uInt16  FCPBootHiVersion;
        uInt16  FCPBootLoVersion;
        uInt16  DMPBootHiVersion;
        uInt16  DMPBootLoVersion;
        uInt16  FCPSwHiVersion;
        uInt16  FCPSwLoVersion;
        uInt16  DMPSwHiVersion;
        uInt16  DMPSwLoVersion;
        uChar   FcpErrStat1;
        uChar   FcpErrStat2;
        uChar   FcpErrStat3;
        uChar   FcpErrStat4;
        uChar   DmpErrStat1;
        uChar   DmpErrStat2;
        uChar   DmpErrStat3;
        uChar   DmpErrStat4;
        uChar   ExtraErrStat1;
        uChar	FCPBldVersion;
} FDDISTATUS;

typedef struct _MONTEREY_DIAG {	/* make sure this matches above */
        uChar   FcpErrStat1;
        uChar   FcpErrStat2;
        uChar   FcpErrStat3;
        uChar   FcpErrStat4;
        uChar   DmpErrStat1;
        uChar   DmpErrStat2;
        uChar   DmpErrStat3;
        uChar   DmpErrStat4;
        uChar   ExtraErrStat1;
        uChar	FCPBldVersion;
} FDDIDIAG;
  


/*
 *-------------------------------------------------------------------------- 
 *	FDDI format Message  
 *--------------------------------------------------------------------------
 */

typedef struct _FDDIHDR {
	SCPHDR	hdr PAK;
	uChar	cmd_status PAK;		/* for status */
	uChar	SubOpcode PAK;	/* i.e. Set_FDDI_Req = xx */
} FDDI_HDR;

typedef struct _FDDI_MESSAGE {
	FDDI_HDR hdr PAK;	/* opcode = set command, SubOpcode = SET_FDDI */
	uChar	mData[MAX_SCP_PKT_DATA_SIZE - 2] PAK;
				/* 1 byte cmd_status, 1 byte SubOpcode */
} FDDI_MESSAGE;

/*
 *-------------------------------------------------------------------------
 *	SCP command status
 */
#define SCPSTAT_OK			0x00
#define SCPSTAT_FAILED			0xFF
#define SCPSTAT_UNRECOGNIZED		0xFE  /* command not recognized */
#define SCPSTAT_NOTAPPLICABLE		0xFD  /* command not applicable */
#define SCPSTAT_FDDI_CAMNOTFOUND	0xFC  /* CAM entry not found */
#define SCPSTAT_FDDI_SYSTEMCAM		0xFB  /* permanent CAM entry */
#define SCPSTAT_FDDI_CAM_TBL_FULL      0xFA  /* Full CAM table */


/*
 *--------------------------------------------------------------------------
 *	Error Status for FCP/DMP Module_Ready msg
 *--------------------------------------------------------------------------
 */
#ifndef	flag
#define	flag(x)		(1 << (x))
#endif
/* For error status diag results */
/* FCPErrorStat1 */
#define	FCP_ROM		flag(0)		/*  */
#define	FCP_RAM		flag(1)		/*  */
#define	FCP_FLASH	flag(2)		/*  */
#define	FCP_DPRAM	flag(3)		/*  */
#define	FCP_SWRAM       flag(4)         /*  */ 
#define	FCP_CACHE       flag(5)		/*  */
#define	FCP_DMP_COMM    flag(6)		/* if failed, all DMP status is Unknown */
#define	FCP_LOAD_GEN    flag(7)		/*  */
/* FCPErrorStat2 */
#define	FCP_PORTAACC	flag(0)		/*  */
#define	FCP_PORTBACC	flag(1)		/*  */
#define	FCP_PORTALOOP	flag(2)		/*  */
#define	FCP_PORTBLOOP	flag(3)		/*  */
#define	FCP_MACACC      flag(4)         /*  */ 
#define	FCP_MAC_BUFF    flag(5)		/*  */
#define	FCP_MACINLOOP   flag(6)		/*  */
#define	FCP_MACEXLOOP   flag(7)		/*  */
/* FCPErrorStat3 */
#define	FCP_CAM0	flag(0)		/*  */
#define	FCP_CAM1	flag(1)		/*  */
#define	FCP_CAM2	flag(2)		/*  */
#define	FCP_CAM3	flag(3)		/*  */
#define	FCP_CAM4	flag(4)		/*  */
#define	FCP_CAM5	flag(5)		/*  */
#define	FCP_CAM6	flag(6)		/*  */
#define	FCP_CAM7	flag(7)		/*  */
/* FCPErrorStat4 */
#define	FCP_CAM8	flag(0)		/*  */
#define	FCP_CAM9	flag(1)		/*  */
#define	FCP_CAM10	flag(2)		/*  */
#define	FCP_CAM11	flag(3)		/*  */
#define	FCP_CAM12	flag(4)		/*  */
#define	FCP_CAM13	flag(5)		/*  */
#define	FCP_CAM14	flag(6)		/*  */
#define	FCP_CAM15	flag(7)		/*  */
/* DMPErrorStat1 */
#define	DMP_FLASH	flag(0)		/*  */
#define	DMP_RAM		flag(1)		/*  */
#define	DMP_SRAM	flag(2)		/*  */
#define	DMP_FCP_COMM	flag(3)		/*  */
#define	DMP_SWRAM	flag(4)		/*  */
#define	DMP_CACHE	flag(5)		/*  */
#define	DMP_OVERLAY	flag(6)		/*  */
#define	DMP_SPARE	flag(7)		/*  */
/* DMPErrorStat2 */
#define	DMP_MACACC	flag(0)		/*  */
#define	DMP_MACBUFF	flag(1)		/*  */
#define	DMP_MACINLOOP	flag(2)		/*  */
#define	DMP_MACEXLOOP	flag(3)		/*  */
#define	DMP_BIGAACC	flag(4)		/*  */
#define	DMP_BIGAMACBUFF	flag(5)		/*  */
#define	DMP_BIGAMACTX	flag(6)		/*  */
#define	DMP_BIGAMACRX	flag(7)		/*  */
/* DMPErrorStat3 */
#define	DMP_BIGAMACILP	flag(0)		/*  */
#define	DMP_BIGAMACELP	flag(1)		/*  */
#define	DMP_BIGASAGEACC	flag(2)		/*  */
#define	DMP_BIGASAGELP1	flag(3)		/*  */
#define	DMP_BIGASAGELP2	flag(4)		/*  */
#define	DMP_LOADGEN	flag(5)		/*  */
#define	DMP_BIGALOADGEN	flag(6)		/*  */
#define	DMP_XTRA	flag(7)		/*  */



/* SET_FDDI reqType */
#define	TLMINA		0x0000		/* Set tlmin for port A */
#define	TLMINB		0x0001		/* Set tlmin for port B */
#define	TNOTIFY		0x0002		/* Set tnotify  */
#define	TREQ		0x0003		/* Set treq */
#define	USERDATA	0x0004		/* Set userdata */

#define MAX_USERDATA_SIZE   32          /* Userdata is less than 32 characters */

typedef struct _SET_FDDI {
	SCPHDR	hdr;
	uChar	cmd_status;		/* status */
	uChar	SubOpcode;	/* Set_FDDI_Req = xx */
	uChar   reqType; 	/* tlmin, tnotify, treq or userdata */
	uInt32	setValue;	/* tlmin hexvalue, tnotify secs, or treq usecs */
	uChar	userData[MAX_USERDATA_SIZE];	/* valid only if userdata type is requested */
} SET_FDDI;


/* SET_FDDI_CAM reqType */
#define	AGINGTIME	0x0000		/* Set cam AgingTime, should be half of EARL aging time */
#define	ENTRY_DYN	0x0001		/* Set cam dynamic entry, for fddi filtering only */
#define	ENTRY_STATIC	0x0002		/* Set cam static entry, for fddi filtering only */

typedef struct _SET_FDDI_CAM {
	SCPHDR	hdr;
	uChar	cmd_status;		/* status */
	uChar	SubOpcode;	/* Set_FDDI_CAM_Req = xx */
	uChar   reqType; 	/* agingtime, dynamic_entry, or static entry */
	uInt32	agingTime;	/* agingtime in secs, valid only if agingTime is requested */
	uChar	macAddr[6];	/* valid only if dyn/static entry is requested */
	uChar	portNum;        /* port to assigne MAC addr to */
} SET_FDDI_CAM;


/* CLEAR_FDDI_CAM reqType */
#define	ALL_ENTRIES	0x0000		/* Clear all fddicam entries */
#define	SINGLE_ENTRY	0x0001		/* Clear single fddicam entry */
#define STATIC_ENTRIES  0x0002          /* Clear all static fddicam entry */
#define DYNAMIC_ENTRIES 0x0003          /* Clear all dynamic fddicam entry */
#define PERM_ENTRIES    0x0004          /* Clear all permenant fddicam entry */

typedef struct _CLEAR_FDDI_CAM {
	SCPHDR	hdr;
	uChar	cmd_status;		/* status */
	uChar	SubOpcode;	/* Clear_FDDI_CAM_Req = xx */
	uChar   reqType; 	/* all or single entry */
	uChar	macAddr[6];	/* valid only if single entry is requested */
} CLEAR_FDDI_CAM;


/* SET_UNREACHABLE and SET_BRIDGE setValue */
#define	DISABLE		0x0000		/*  */
#define	ENABLE		0x0001		/*  */

typedef struct _SET_UNREACHABLE {   /* A.K.A. set MTU discovery */
	SCPHDR	hdr;
	uChar	cmd_status;		/* status */
	uChar	SubOpcode;	/* Set_UNREACHABLE_Req = xx */
	uChar   setValue; 	/* enable or disable MTU discovery */
} SET_UNREACHABLE;


/* SET_BRIDGE reqType */
#define	IPFRAG		0x0000		/* Enable or disable ip fragmentation */
#define	IPX		0x0001		/* Set ipx default translation */

/* SET_BRIDGE transType */
#define SNAPTOETHER	0x0000
#define _8022TOETHER	0x0001
#define _8023RAWTOFDDI	0x0002

/* SET_BRIDGE, GET_BRIDGE_RESP and GET_FDDI_CAM_RESP protocolType */
#define PRTCL_8023	0x0000
#define PRTCL_SNAP	0x0001
#define PRTCL_EII	0x0002
#define PRTCL_8023RAW	0x0003
#define PRTCL_8022	0x0004
#define PRTCL_FDDIRAW	0x0005

typedef struct _SET_BRIDGE {
	SCPHDR	hdr;
	uChar	cmd_status;		/* status */
	uChar	SubOpcode;	/* Set_BRIDGE_Req = xx */
	uChar   reqType; 	/* ip fragmentation or ipx translation */
	uChar	setValue;	/* enable or disable for ipfrag */
	uChar	transType;	/* translation type for ipx translation */
	uChar	prtclType;	/* protocol type for ipx translation */
} SET_BRIDGE;

typedef struct _GET_BRIDGE_REQ {
	SCPHDR	hdr;
	uChar	cmd_status;		/* status */
	uChar	SubOpcode;	/* Get_BRIDGE_Req = xx */
} GET_BRIDGE_REQ;

typedef struct _GET_BRIDGE_RESP {
	SCPHDR	hdr;
	uChar	cmd_status;		/* status */
	uChar	SubOpcode;	/* Get_BRIDGE_Resp = xx */
	uChar   Unreachable;    /* enabled/disabled */
	uChar	EnIpFrag;	/* ip fragmentation enabled or disabled */
	uChar	IpxSnapToEther;	/* default translation prtclType */
	uChar 	Ipx8022ToEther;	/* default translation prtclType */
	uChar   Ipx8023RToFddi;	/* default translation prtclType */ 
} GET_BRIDGE_RESP;

/* GET_FDDI_CAM_REQ reqType */
#define	SINGLE_MAC_ADDR	0x0000		/* Show single mac address in cam table */
#define	DUMP_CAM	0x0001		/* Dump 20 entries of cam table */

typedef struct _GET_FDDI_CAM_REQ {
	SCPHDR	hdr;
	uChar	cmd_status;		/* status */
	uChar	SubOpcode;	/* Get_FDDI_CAM_Req = xx */
	uChar   reqType; 	/* single mac_addr or cam dump */
	uInt32	cam_index;	/* index into cam table, valid only for cam dump request */
	uChar	macAddr[6];	/* valid only if single mac_addr type is requested */
} GET_FDDI_CAM_REQ;

typedef struct _fddicam_entry {
		uChar 	index;		/* may start from non-zero value */
 		uChar	macAddr[6];	/* Mac address */
		uChar   prtclType;   	/* Protocol type */
} fddicam_entry;   

typedef struct _GET_FDDI_CAM_RESP {
	SCPHDR	hdr;
	uChar	cmd_status;		/* status */
	uChar	SubOpcode;	/* Get_FDDI_CAM_Resp = xx */
	uChar   reqType; 	/* individial mac_addr or cam dump */
	uInt32	total_entries;	/* total number of cam entries at this time */
	uChar	num_of_entries;	/* number of cam entries dumped at once can be 0 to 20 */
	fddicam_entry fcam_entry[20];  /* up to 20 entries at a time */
} GET_FDDI_CAM_RESP;


typedef struct _GET_MAC_REQ {
	SCPHDR	hdr;
	uChar	cmd_status;		/* status */
	uChar	SubOpcode;	/* Get_MAC_Req = xx */
} GET_MAC_REQ;

typedef struct _GET_MAC_RESP {
	SCPHDR	hdr;
	uChar	cmd_status;		/* status */
	uChar	SubOpcode;	/* Get_MAC_Resp = xx */
	uChar	SMTAddress[6];	/* MIB Attr */
	uChar 	CurrentPath;	/* MIB Attr */
	uInt32	T_Req;		/* MIB Attr */
	uInt32	T_Neg;		/* MIB Attr */
	uInt32	T_Max;		/* MIB Attr */	
	uInt32	TvxValue;	/* MIB Attr */
	uChar	UpstrmNbr[6];	/* MIB Attr */
	uChar	DownstrmNbr[6];	/* MIB Attr */
	uChar	OldUpNbr[6];	/* MIB Attr */
	uChar	OldDnNbr[6];	/* MIB Attr */
	uInt32 	rxSmtFrames;
	uInt32	txSmtFrames;
	uInt32	rxLLCFrames;
	uInt32	txLLCFrames;
	uInt32	TvxExp_Ct;	/* MIB Attr */
	uInt32	RingOp_Ct;	/* MIB Attr */
	uInt32	Inframes;
	uInt32	Outframes;
	uInt32	multicast_tx;
	uInt32	multicast_rx;
	uInt32	broadcast_rx;
	uInt32	broadcast_tx;
	uInt32	delayExceededDisc;
	uInt32	mtuExceededDisc;
	uInt32	Discards;
	uInt32	learnedEntryDisc;	
        uInt32  lastchange;
        uInt32  inoctets;
        uInt32  outoctets;
        uInt32  unknownprotol;
        uInt32  inerrors;
        uInt32  outerrors;
        uInt32  InLost;
        uInt32  OutLost;

} GET_MAC_RESP;


typedef struct _GET_MAC_DBG_REQ {
	SCPHDR	hdr;
	uChar	cmd_status;		/* status */
	uChar	SubOpcode;	/* Get_MAC_DBG_Req = xx */
} GET_MAC_DBG_REQ;

typedef struct _GET_MAC_DBG_RESP {
	SCPHDR	hdr;
	uChar	cmd_status;		/* status */
	uChar	SubOpcode;	/* Get_MAC_DBG_Resp = xx */
	uInt32	Copied_Ct;	/* MIB Attr */
	uInt32	Transmit_Ct;	/* MIB Attr */
	uInt32	Error_Ct; 	/* MIB Attr */	
	uInt32	Lost_Ct; 	/* MIB Attr */	
	uInt32	Frame_Ct; 	/* MIB Attr */	
	uChar	BufferFull;	
	uInt32	TMax_Cap;	/* MIB Attr */	
	uInt32	TVX_Cap;	/* MIB Attr */
	uInt16	FrameStatus;	/* MIB Attr */	
	uChar	AvailablePaths;	/* MIB Attr */	
	uChar	ReqPath;        /* MIB ATTR */
	uInt16	DupAddrTest;	/* MIB Attr */	
	uChar	DA_Flag;	/* MIB Attr */	
	uChar	FrmError_Flag;	/* MIB Attr */	
} GET_MAC_DBG_RESP;


typedef struct _GET_PORT_REQ {
	SCPHDR	hdr;
	uChar	cmd_status;		/* status */
	uChar	SubOpcode;	/* Get_PORT_Req = xx */
} GET_PORT_REQ;

typedef struct _GET_PORT_RESP {
	SCPHDR	hdr;
	uChar	cmd_status;		/* status */
	uChar	SubOpcode;	/* Get_PORT_Resp = xx */
	uChar	PortStatus[2];	
	uChar 	CurrentPath[2];	/* MIB Attr */
	uInt16	ConnectSt[2];	/* MIB Attr */		
	uChar	MyType[2];	/* MIB Attr */
	uChar	NbrType[2];	/* MIB Attr */
	uChar	MediaType[2];
	uChar	LerFlag[2];	/* MIB Attr */
	uChar 	LerEst[2];	/* MIB Attr */
	uChar 	LerAlarm[2];	/* MIB Attr */
	uChar 	LerCutoff[2];	/* MIB Attr */
	uInt32	LemCount[2];	/* MIB Attr */
	uInt32	LemRejCount[2];	/* MIB Attr */
	uInt32	tlmin[2];
} GET_PORT_RESP;	


typedef struct _GET_PORT_DBG_REQ {
	SCPHDR	hdr;
	uChar	cmd_status;		/* status */
	uChar	SubOpcode;	/* Get_PORT_DBG_Req = xx */
} GET_PORT_DBG_REQ;

typedef struct _GET_PORT_DBG_RESP {
	SCPHDR	hdr;
	uChar	cmd_status;		/* status */
	uChar	SubOpcode;	/* Get_PORT_DBG_Resp = xx */
	uChar	ConnPolicies[2];/* MIB Attr */
	uInt16	ReqPaths[2];	/* MIB Attr */
	uChar	AvailPaths[2];	/* MIB Attr */
	uInt16	MACPlacement[2];/* MIB Attr */
	uInt32	LCTFail_Ct[2];	/* MIB Attr */
	uChar	MACIndicated[2];/* MIB Attr */
	uChar 	PC_Withhold[2]; /* MIB Attr */
	uChar	BS_Flag[2];	/* MIB Attr */
	uInt32	EBError_Ct[2];	/* MIB Attr */
	uInt32	MACLoop_Time[2];/* MIB Attr */
	uChar	PCMState[2];	/* MIB Attr */
	uInt32	attempt_Ct[2];	/* ALWAYS 0 */
	uInt32	success_Ct[2];  /* 0 */
	uInt32	timeOut_Ct[2];  /* 0 */
	uInt32	cancel_Ct[2];   /* 0 */
	uInt32	PHYBreakCt[2];  /* 0 */
} GET_PORT_DBG_RESP;


typedef struct _GET_SYSTEM_REQ {
	SCPHDR	hdr;
	uChar	cmd_status;		/* status */
	uChar	SubOpcode;	/* Get_SYSTEM_Req = xx */
} GET_SYSTEM_REQ;

typedef struct _GET_SYSTEM_RESP {
	SCPHDR	hdr;
	uChar	cmd_status;		/* status */
	uChar	SubOpcode;	/* Get_SYSTEM_Resp = xx */
	uInt32	ModuleStatus;	
	uChar	SMTUsrData[32];	/* MIB Attr */
	uInt16	TNotify;	/* MIB Attr */
	uInt16	CF_State;	/* MIB Attr */
	uChar	ECM_State;	/* MIB Attr */
	uChar 	BypassPresent;  /* MIB Attr */
} GET_SYSTEM_RESP;	


#define FDDI_NVRAM_DATA_SIZE    240     /* max fddi nvram message data in bytes */

/* msg from Super to Monterey to set fddi parameters, sent during initialization */
typedef struct _SET_FDDI_NVRAM {
        SCPHDR  hdr PAK;
        uChar   cmd_status PAK;             /* status */
        uChar   SubOpcode PAK;      /* Set_FDDI_NVRAM = xx */
        uChar   FddiDataLength PAK;
				/* length of data in this message in bytes */
        uChar   NVRAMStatus PAK;
			/* set to zero if good, set to FF if NVRAM is invalid */
        uChar   FDDI_NVRAM_parms[FDDI_NVRAM_DATA_SIZE] PAK;
						/* NVRAM parameters */
} SET_FDDI_NVRAM;

/* msg from Monterey to Super to update NVRAM with new fddi parameters */
typedef struct _UPDATE_FDDI_NVRAM {
        SCPHDR  hdr PAK;
        uChar   cmd_status PAK;             /* status */
        uChar   SubOpcode PAK;      /* Update_FDDI_NVRAM = xx */
        uChar   FddiDataLength PAK;
				/* length of data in this message in bytes */
        uChar   NVRAMStatus PAK;    /* set to zero for now */
        uChar   FDDI_NVRAM_parms[FDDI_NVRAM_DATA_SIZE] PAK;
					/* NVRAM parameters */
} UPDATE_FDDI_NVRAM;


/* msg from Super to Monterey to send system diagnostics results */
typedef struct _SYSTEM_DIAG_RESULTS	{
	SCPHDR  header PAK;
	uInt16  diag_global  PAK;
	uInt32  saint_detail PAK;
	uInt32	pkt_buf_detail PAK;
        uChar   mpu_xtra_diag_result[MPU_XTRA_DIAG_BYTES] PAK;	/* FDDIDIAG,ALCDIAG */
} SYSTEM_DIAG;


/* msg from Super to Monterey to set VLAN */
typedef struct _SET_VLAN  {
        FDDI_HDR header PAK;
	uChar	 port  PAK;		/* port to set - not used */
	uInt16 	 VLAN_num PAK;	/* VLAN to manipulate */
	uChar	 state PAK;		/* new VLAN state */
} SET_VLAN;

 
 /*---   GETSTAT msg values (reflect status of LEDs)             */
  
  /*---   Module Status                   */
   
   #define   SCP_MSTAT_RINGOP        0x08  /* GREEN: 1=ringop, 0=ringdown  */
   #define   SCP_MSTAT_THRU          0x10  /* GREEN: 1=thru, 0=other       */
   #define   SCP_MSTAT_WRAPA         0x20  /* GREEN: 1=wrapA, 0=other      */
   #define   SCP_MSTAT_WRAPB         0x40  /* GREEN: 1=wrapB, 0=other      */
   #define   SCP_MSTAT_BYPASS        0x80  /* GREEN: 1=inserted, 0=bypass  */
    
    /*---   Port Status                     */
     
   #define   SCP_PSTAT_OFF           0x01  /* 1 = OFF, 0 = enable     */
   #define   SCP_PSTAT_GREEN         0x02  /* 1 = GREEN, 0 = other    */
   #define   SCP_PSTAT_ORANGE        0x04  /* 1 = ORANGE, 0 = other   */
   #define   SCP_PSTAT_RED           0x08  /* 1 = RED, 0 = OK         */
   #define   SCP_PSTAT_LER_ALARM     0x80  /* 1 = Ler_alarm, 0 = OK   */
      

typedef struct module_status  {
	SCPHDR	hdr;
	uChar	mod_status;
	uChar	portA_stat;
	uChar	portB_stat;
} MODULE_STATUS;


typedef struct get_FDDI_resp {
	FDDI_HDR  header;
	uInt16    TNotify;
	uInt16    TLminA;
	uInt16    TLminB;
	uInt32    TReq;
	uChar     LerCutoff_A;
	uChar     LerCutoff_B;
	uChar     LerAlarm_A;
	uChar     LerAlarm_B;
	uChar     UserData[MAX_USERDATA_SIZE]; 
} GET_FDDI_RESP;

typedef struct SET_CUTOFF {
	FDDI_HDR  header;
        uChar     portNum;
	uChar     cutoff_value;
} SET_LER_CUTOFF;

typedef struct SET_ALARM {
	FDDI_HDR  header;
        uChar     portNum;
	uChar     alarm_value;
} SET_LER_ALARM;


#endif 
