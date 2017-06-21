/* $Id: scp_defs.h,v 1.1.4.1 1996/05/09 15:01:49 rbadri Exp $
 * $Source: /release/112/cvs/Xwbu/cat5k/sysif/scp_defs.h,v $
 *-----------------------------------------------------------------
 * scp_defs.h -- serial protocol data structures.
 *
 * November 1994, rbatra
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: scp_defs.h,v $
 * Revision 1.1.4.1  1996/05/09  15:01:49  rbadri
 * Branch: California_branch
 * Synergy features to the mainline
 *
 * Revision 1.1.40.1  1996/04/27  06:17:45  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.28.2  1996/04/11  01:50:24  sakumar
 * Header file changes to support LEC creation via VTP messages.
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 1.1.28.1  1996/04/08  01:43:14  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.16.2  1996/04/02  05:08:16  rlowe
 * Fix up include file paths.  Cleanup main ALC makefile regarding
 * sar images and tools.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.16.1  1996/03/22  09:13:56  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.1  1996/03/13  07:42:39  rlowe
 * Add file content from Synergy repository.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1  1996/03/11  07:44:53  rlowe
 * Placeholders for California.
 *
 *------------------------------------------------------------------
 * Legacy comments from Synergy repository:
 *
* Revision 1.70.2.1  1995/11/17  01:40:07  jkuo
* Added SPAN monitor and MII register set/dump messages.
*
* Revision 1.70  1995/10/05  21:23:06  sgarcia
* Mainline post-sync fixup:
* - remove from comments test strings (< or >) which confuse sync scripts.
*
* Revision 1.69  1995/10/04  01:07:27  kchristi
* Fix CVS Endlog directives so that cvs-fix-merge script will work.
*
* Revision 1.68  1995/09/25  17:32:38  lla
* Moved EXTRA_CAM BIT definition from diag.h to scp_defs.h for CAM status display
*
* Revision 1.67  1995/09/14  00:01:25  ralkass
* define opcode SCP_DNLD_STATUS for flash-download status
*
* Revision 1.66  1995/09/07  00:22:01  jkuo
* CHanged SCPSTAT_NOTSUPPORTED from 0xF0 to 0xAA.
*
* Revision 1.65  1995/08/31  22:19:52  jkuo
* Changed new msg defs using SCP_ prefix.
*
* Revision 1.64  1995/08/31  21:26:50  jkuo
* Added SET_MCP_MAC_ADDR_CMD & SET_SPAN_MONITOR_CMD op codes.
* Removed SCP_GET_SAINT_MODE_BY_MASK & SCP_SET_SAINT_MODE_BY_MASK.
* Added error return code SCPSTAT_NOTSUPPORTED (0xF0).
*
* Revision 1.63  1995/08/01  23:37:37  ralkass
* define SCP_FCP_ERASE_FLASH cmd for monterey
*
* Revision 1.62  1995/07/31  23:44:47  ralkass
* Added SET_LER_CUTOFF_CMD & SET_LER_ALARM_CMD for monterey
*
* Revision 1.61  1995/07/25  16:50:48  ralkass
* Define FDDI protocol translation types
*
* Revision 1.60  1995/07/21  00:31:50  ralkass
* Define opcode GET_FDDI_REQ_CMD for monterey
*
* Revision 1.59  1995/07/20  23:16:56  ralkass
* Define command GET_FDDI_RESP_CMD for monterey
*
* Revision 1.58  1995/07/18  19:35:48  kiett
* Added SCP_FCP_CLEAR_CONFIG
*
* Revision 1.57  1995/06/22  17:18:51  kiett
* define NVRAM_REQUEST
*
* Revision 1.56  1995/06/13  23:37:46  vinod
* Changed SET_CARD_INSTANCE to SCP_SET_ATM_INSTANCE
*
* Revision 1.55  1995/06/09  23:36:29  vinod
* Added NMP_FDDI_SCP_SAP for the NMP FDDI TASK
*
* Revision 1.54  1995/06/09  08:42:07  rlowe
* Define ALC_SAP_BASE to indicate base of ATM line card specific SCP SAPs.
* Have ALC_SCP_SAP (0x60) and ALC_RESET_ACK_SAP (0x61) be relative to this
* base (0x60).  Eliminate L68K_RESET_SAP (once again) since ALC no longer
* uses it.  All for ALC integration to latest LCP driver requirements
* (message model, startup & HW handshakes).
*
* Revision 1.53  1995/05/26  18:33:28  sgarcia
* Noted that SCP_PING defined elsewhere as well.
*
* Revision 1.52  1995/05/25  23:19:08  rfa
* Added SCP_ILC_DNLD message type for intelligent line card downloads
*
* Revision 1.51  1995/05/24  16:46:39  ralkass
* Redfine FDDI_CONNECT_CMD and FDDI_DISCONNECT_CMD for monterey
*
* Revision 1.50  1995/05/24  00:35:23  aannaama
* Added CDP Multicast Adrs set up cmd
*
* Revision 1.49  1995/05/22  21:36:13  ralkass
* Added monterey FDDI_SCP_SAP_BASE
*
* Revision 1.48  1995/05/19  05:12:48  vinod
* Added SCP_SUPERVISOR_MAC_IP_ADDR opcode
*
* Revision 1.47  1995/05/19  00:08:06  sgarcia
* scp_defs.h: Added comments to those opcodes that are also defined elsewhere.
*             Removed unused CP_SAP for 8051s.
* scp_msgs.h: added length define for COLL_COUNTS.
*
* Revision 1.46  1995/05/16  03:39:40  rlowe
* Restored symbol L68K_RESET_SAP for common init-time use (although at $40
* instead of $02).  Added ALC_SCP_SAP for ATM line card specific SCP messages.
*
* Revision 1.45  1995/05/10  06:02:03  jkuo
* Changed LINECARD_READY (0x06) to MODULE_READY, which
* is the same as DNLD_RESTART_ACK_TO_NMP (0x05) from now on.
*
* Revision 1.44  1995/05/10  01:20:28  ralkass
* Defined CONFIG_DMP for monterey DMP
*
* Revision 1.43  1995/05/09  20:16:15  ralkass
* Added FDDI SAP and FDDI SCP msgs opcdoes
*
* Revision 1.42  1995/05/03  04:28:58  jkuo
* Swapped scp_defs.h with scp_opcode.h, i.e., new op codes start
* taking effect from now on.
*
* Revision 1.41  1995/04/28  18:24:26  jkuo
* Added PSTAT bits 100MBPS, MII, & FULL_DPLX.
* Also get ready for cutover to the new scp_opcode.h
*
* Revision 1.40  1995/04/25  00:19:26  sgarcia
* Mother of all Merges.
* Within syn_common, merely added several defines so message handlers no
* longer need to use "magic numbers" as offsets into the message buffers.
*
* Revision 1.39  1995/04/13  02:15:18  jkuo
* Added SCP_MSTAT_ON_LINE (0x08)
*
* Revision 1.38  1995/04/04  19:48:28  jlockhar
* Added opcode for SET_PORT_FEATURES msg.
*
* Revision 1.37  1995/04/03  21:14:18  blouie
* fixed by terminating a couple of FDDI comment lines.
*
* Revision 1.36  1995/04/01  00:42:52  ralkass
* Added and reserved opcodes for FDDI
*
* Revision 1.35  1995/03/17  23:47:06  jkuo
* Added DNLD_RESTART_ACK_TO_NMP(0xB), and flagged _SAINT_MODE_ msgs as NOT implemented (too busy).
*
* Revision 1.34  1995/03/14  18:58:54  ralkass
* Added 'Added L68K_RESET_SAP' for Multi-processor 8051->68K reset_ACK SAP
*
* Revision 1.33  1995/03/09  07:50:57  jkuo
* Removed duplicate def for SCP_BOARD_PRESENT
*
* Revision 1.32  1995/03/02  01:39:43  sgarcia
* Added SCP_GET_COLL_COUNTS and its associated struct.
*
* Revision 1.31  1995/03/01  00:20:52  rbatra
* Added a new msg type for the scp statistics
*
* Revision 1.30  1995/02/28  22:48:04  jkuo
* Shorten port/module stats to 8 bits,
* Made DIABLED and ENABLED for MSTAT & PSTAT mutually exclusive,
* Moved the last two stats up by one bit.
*
* Revision 1.29  1995/02/27  01:00:41  vinod
* After some discussion redefined Enable and Disable module/port
* message.
*
* Revision 1.28  1995/02/26  04:22:51  jkuo
* Added error stats.
*
* Revision 1.27  1995/02/25  23:10:30  rbatra
* Added SCP_SET_ENDIS enabe/disable opcode.
*
* Revision 1.26  1995/02/25  21:32:05  ralkass
* add set_card_instance opcode
*
* Revision 1.25  1995/02/25  18:09:34  jlockhar
* Updated LED status definitions for GETSTAT.
*
* Revision 1.24  1995/02/25  06:07:40  vinod
* Removed NMP & SLAVE test messages and instead defined a
* single SCP_PING message for echo back through the SCP
* channel
*
* Revision 1.23  1995/02/24  04:47:18  jkuo
* Added mem peek/poke msgs
*
* Revision 1.22  1995/02/23  18:53:05  umahajan
* Added SCP_SET_PORT_STATE define.
*
* Revision 1.21  1995/02/22  22:50:38  jkuo
* Write 2 bytes in DPRAM for Traffic Meter value for redundency
*
* Revision 1.20  1995/02/15  03:01:31  jkuo
* Added NMP_CONFIG_SAP=1
*
* Revision 1.19  1995/02/14  21:24:47  jkuo
* Changed traffic meter msg in DPRAM from 2 bytes to 1
*
* Revision 1.18  1995/02/13  21:53:49  jkuo
* Added traffic meter msg using DP RAM
*
* Revision 1.17  1995/02/11  23:20:33  husain
* Changes for send_pkt & rcv_pkt routines; changed SCP_HEADER_LEN to
* sizeof scp header struct
*
* Revision 1.16  1995/02/09  00:18:29  jkuo
* Removed SCP_ACK bit (0x80)
*
* Revision 1.15  1995/02/07  03:37:52  jkuo
* Added SCP_DUMP_SAINT_REGS & SCP_SET_SAINT_REGS_BY_ID msgs
*
* Revision 1.13  1995/02/03  18:31:32  sgarcia
* Minor changes for get/clear saint counters, and read/write eeprom.
*
* Revision 1.12  1995/02/02  19:54:07  jlockhar
* Changed SCP_SET_COUNTERS to SCP_CLR_COUNTERS
*
* Revision 1.11  1995/02/01  21:22:04  jkuo
* Incorperated with ErrorStat bits defs
*
* Revision 1.10  1995/02/01  01:03:56  vinod
* Added SCP_READ_SEEPROM & SCP_WRITE_SEEPROM opcodes.
*
* Revision 1.9  1995/01/26  02:08:55  jlockhar
* Added GETSTAT status bit descriptions.
*
* Revision 1.8  1994/12/29  05:04:19  blouie
* Rearranged opcode definition for Download.
*
* Revision 1.7  1994/12/29  04:06:27  jlockhar
* Added additinal opcodes for Download
*
* Revision 1.6  1994/12/24  02:47:14  husain
* merging
*
* Revision 1.5  1994/12/22  05:02:17  vinod
* Added 4 new opcodes for getting and setting SAINT registers
*
* Revision 1.4  1994/12/14  19:33:08  blouie
* Deleted inclusion of utypes.h. It breaks the 68k NMP build.
* Do NOT put 8051 specific include files within these two header files.
*
* Revision 1.3  1994/12/13  23:33:55  husain
* Added include for utypes.h and scp_msgs.h
*
* Revision 1.1  1994/12/07  22:46:03  husain
* Moved to syn_common
*
* Revision 1.2  1994/12/08  00:43:22  vinod
* removed 8051 specific scp defines.
*
* Revision 1.1  1994/12/07  22:46:03  husain
* Moved to syn_common
*
* Revision 1.4  1994/12/03  00:33:51  husain
* update
*
* Revision 1.3  1994/12/01  04:10:07  jlockhar
* Incoported OPCODES.h and removed old SCPHDR structure.
*
* Revision 1.2  1994/11/18  00:49:25  rbatra
* Changed the rcv timer to 100msec.
*
* Revision 1.1  1994/11/17  19:30:51  rbatra
* This file contains the serial protocol data structures.
*
*
 *------------------------------------------------------------------
 * $Endlog$
 */
#ifndef _SCP_DEFS_H
#define _SCP_DEFS_H

#include "../sysif/scp_msgs.h"

/*
-------------------------------------------------
    UNSOLICITED MESSAGES OP CODE 01 - 0F

    Unsolicited msgs: MCP/LCP ---> NMP 68040
    Unsolicited msgs: ATM/FDDI ---> MP 68030
-------------------------------------------------
*/
 
#define SCP_INVALID_OPCODE      0x00  /* needed, as long as we have gaps
                                       * in the opcode table */
#define SCP_BOARD_PRESENT	0x01  /* MCP->NMP Board present msg */
#define RESET_ACK_TO_MP  	0x02  /* L8051->L68K 8051 performed reset
                                	- params include diagnostic etc */
#define RESET_ACK_TO_NMP        0x03  /* L8051->NMP via SCP */
 
#define DNLD_RESTART_ACK_TO_MP  0x04  /* L8051->L68K 8051 download restart
                                	- same params as RESET_ACK_TO_NMP */
#define LCP_READY		DNLD_RESTART_ACK_TO_MP

#define DNLD_RESTART_ACK_TO_NMP 0x05  /* L8051->NMP via SCP */

#define MODULE_READY		DNLD_RESTART_ACK_TO_NMP
				      /* L68K->NMP ATM/FDDI ready */
#define EXTRA_CAM      0x0002

#define SCP_DBG_PRINT           0x07  /* request to NMP for printing msg */




/*
xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

	ENTER  MORE  UNSOLICITED  OP  CODES  HERE  (07-0F)

xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
*/




/*
-------------------------------------------
    SOLICITED MESSAGES OP CODE 10 - 4F

    Normal msgs: NMP ---> MCP/LCP/68030
    Normal msgs: MP 68030 ---> LCP
-------------------------------------------
*/

 
#define SCP_PING                0x11  /* SCP Ping to a Line card */
                                      /* Also in 8051/common/msg_proc.s */
#define SCP_GET_LTL             0x12
#define SCP_SET_LTL             0x13
#define SCP_GET_CBL             0x14
#define SCP_SET_CBL             0x15
#define	SCP_PEEK_MEM		0x16
#define	SCP_POKE_MEM		0x17
#define SCP_GET_COLL_COUNTS     0x18  /* get saint "collision counters" */

#define SCP_GET_COUNTERS        0x19
#define SCP_CLR_COUNTERS        0x1A
 
#define SCP_GET_DIAG_STAT       0x1B  /* to get diag results */
                                      /* Also in 8051/common/msg_proc.s */
#define SCP_GET_STATS		0x1C  /* Msg to get statistics of serial Link*/

#define SCP_ENADIS_MODPORT	0x1D  /* Enable/Disable port/module, and card
				       * go on/off line.  Also in
				       * 8051/common/msg_proc.s */
#define SCP_GET_STATUS          0x1E

/*---	GETSTAT msg values (reflect status of LEDs)		*/ 

/*---	Module Status			*/

#define   SCP_MSTAT_DISABLED	  0x00	/* Orange- Offline, 		*/ 
#define   SCP_MSTAT_ENABLED	  0x01	/* Green - Online, DiagOK	*/ 
#define   SCP_MSTAT_CARDBAD	  0x02	/* RED   - Diag Failed		*/ 
#define   SCP_MSTAT_CARDDEAD	  0x04	/* OFF   - Card does not respond*/
#define	  SCP_MSTAT_ON_LINE	  0x08  /* Card is on line, 0 = offline */
				  
/*---	Port Status		  	*/

#define   SCP_PSTAT_DISABLED	  0x00	/* Orange- Disabled	     	*/ 
#define   SCP_PSTAT_ENABLED	  0x01	/* Green - Enab, linkgood	*/
#define   SCP_PSTAT_LINKBAD	  0x02	/* OFF   - linkbad		*/ 
#define   SCP_PSTAT_PORTBAD	  0x04	/* Orange/Blink-Port Fail,Disab	*/ 
#define   SCP_PSTAT_FULL_DPLX	  0x08	/* Duplex: 1=full, 0=half	*/
#define	  SCP_PSTAT_100MBPS	  0x10	/* EtherNet Speed: 1=100, 0=10  */
#define   SCP_PSTAT_MII		  0x20	/* MII mode: 1=ON, 0=off	*/


/*---   For Intelligent Line cards NOTE: Message with this opcode 
        is not sent to the 8051, but instead directly to the L68K       */

#define   SCP_SUPERVISOR_MAC_IP_ADDR 0x1F  /* Supervisor MAC/IP Address */


/*---	System Diagnostic Send/Receive Packet */

#define SEND_PACKET    		0x20  /* send msg to network */
#define   PACKET_SENT    	  (SEND_PACKET+0X80)    /* reply: pkt sent */ 
#define RCV_PACKET     		0x21  /* receive packet from network */
#define   RCV_PKT_RESP    	  (RCV_PACKET+0X80)     /* reply: pkt rcvd */
 
 
#define SCP_START_DIAG          	0x22
#define SCP_SET_SAINT_REGS_BY_MASK  	0x23	/* NOT implemented */
#define SCP_SET_SAINT_REGS_BY_ID    	0x24
#define SCP_GET_SAINT_REGS_BY_ID    	0x25
#define	SCP_DUMP_SAINT_REGS		0x26
#define SCP_SET_MCP_MAC_ADDR_CMD	0x27	/* Set MAC addrs on all MCP ports */
#define SCP_SET_SPAN_MONITOR_CMD	0x28	/* Set Switched Port ANalyzer Mon */
#define SCP_GET_SAINT_MODE_BY_ID    	0x29	/* NOT implemented */
#define SCP_SET_SAINT_MODE_BY_ID    	0x2A	/* NOT implemented */
#define SCP_SET_PORT_STATE       	0x2B
#define SCP_SET_PORT_FEATURES   	0x2C
#define SCP_READ_SEEPROM        	0x2D   /* See 8051/common/msg_proc.s */
#define SCP_WRITE_SEEPROM       	0x2E   /* See 8051/common/msg_proc.s */
#define SCP_SET_ATM_INSTANCE		0x2F	/* set ATM card instance */


/*---   MCP/LCP download msgs   */

#define SCP_DNLD_STATE0         0x30    /* Set Page 0 active, Init Checksum*/
#define SCP_DNLD_STATE1         0x31    /* Set Page 1 active            */
#define SCP_DNLD_STATE2         0x32    /* Set Page 2 active            */
#define SCP_DNLD_STATE3         0x33    /* Check Checksum, send Resp    */
#define SCP_DNLD_STATE4         0x34    /* Switch to target environment */

#define SCP_DNLD_STATE5         0x35    /* Switch  back to PROM env	*/
#define SCP_DNLD_DATA           0x36    /* Checksummed Data             */
#define SCP_DNLD_OK               0x36  /* Reply: Download Checksum OK  */
#define SCP_DNLD_NOK              0xB6  /* ReplY: Download Checksum Bad */
#define SCP_DNLD_STATUS		0xB7	/* Flash download status from   */
					/* intelligent linecards to NNP */



/*
xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

	ENTER  MORE  NORMAL  OP  CODES  HERE  (37-3F)

xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
*/

#define SET_CDP_MATCH_ADDR_CMD 	0x37	/* CDP Multicast Addr set up cmd */
#define SCP_SET_CDP_MATCH_ADDR 	SET_CDP_MATCH_ADDR_CMD

#define SCP_SET_VLAN_PARAMETERS 0x3C  /* VTP message for intelligent line cards */


/*
-------------------------------------------
   FDDI specific opcodes 0x40 - 0x4F
-------------------------------------------
*/

#define SCP_FCP_SYS_CMD		0x40	/* system level command */
#define	SCP_FCP_SET_CMD		0x41	/* set command  */
#define	SCP_FCP_SHOW_CMD	0x42	/* show command */
#define SCP_FCP_SNMP_CMD        0x43   /* Get or set MIB attributes */
#define SCP_FCP_SNMP_TRAP_CMD   0x44   /* Send SNMP Trap message to Supervisor */
#define SCP_FCP_NVRAM_CMD       0x45   /* Set or Update nvram parameters */
#define CONFIG_DMP		0x46   /* Config DMP from FCP */
#define SCP_FCP_CLEAR_CONFIG	0x47   /* Tell the FCP to reload mfg. config */ 
#define SCP_FCP_ERASE_FLASH     0x48   /* erase flash */

/*---   FDDI sub-commands */

#define FDDI_DISCONNECT_CMD     0x01
#define FDDI_CONNECT_CMD        0x02
#define CLEAR_FDDI_CNTRS_CMD    0x03
#define CLEAR_FDDI_CAM_CMD      0x04
#define XCH_MAC_ADDR_CMD        0x05
#define SOFT_MODULE_RESET       0x06
 
         /* set commands */
#define SET_FDDI_CMD            0x10
#define SET_FDDI_CAM_CMD        0x12
#define SET_UNREACHABLE_CMD     0x14
#define SET_BRIDGE_CMD          0x16
#define SET_VLAN_CMD		0x17
#define SET_LER_CUTOFF_CMD      0x18
#define SET_LER_ALARM_CMD       0x19
 
        /* show commands */
#define GET_FDDI_CAM_REQ_CMD    0x20
#define GET_FDDI_CAM_RESP_CMD   0x21
#define GET_MAC_REQ_CMD         0x22
#define GET_MAC_RESP_CMD        0x23
#define GET_MAC_DBG_REQ_CMD     0x24
#define GET_MAC_DBG_RESP_CMD    0x25
#define GET_PORT_REQ_CMD        0x26
#define GET_PORT_RESP_CMD       0x27
#define GET_PORT_DBG_REQ_CMD    0x28
#define GET_PORT_DBG_RESP_CMD   0x29
#define GET_SYSTEM_REQ_CMD      0x2a
#define GET_SYS_RESP_CMD  	0x2b
#define GET_BRIDGE_REQ_CMD      0x2c
#define GET_BRIDGE_RESP_CMD     0x2d
#define GET_MAC_REG_REQ_CMD     0x2e
#define GET_MAC_REG_RESP_CMD    0x2f
#define	GET_SYSTEM_DIAG_CMD	0x30
#define	GET_SYSTEM_RESP_CMD	0x31
#define GET_FDDI_REQ_CMD        0x32
#define GET_FDDI_RESP_CMD       0x33

         /* nvram commands */
#define SET_FDDI_NVRAM_CMD      0x50
#define NVRAM_REQUEST		SET_FDDI_NVRAM_CMD

#define UPDATE_FDDI_NVRAM_CMD   0x51

#define FDDI_PROTOCOL_EII	0x01
#define FDDI_PROTOCOL_SNAP	0x02
#define FDDI_PROTOCOL_FDDI	0x04
#define FDDI_PROTOCOL_8023	0x08
#define FDDI_PROTOCOL_8023RAW	0x10

/*---   For Intelligent Line cards NOTE: Message with this opcode 
        is not sent to the 8051, but instead directly to the L68K       */
#define SCP_ILC_DNLD		0x52

/* ************************************************************************ */
/* ***************************  END OF OP CODES *************************** */

#define SCP_MAX_OPCODE          0x4F    /* max value for use as opcode */

/* ***************************  END OF OP CODES *************************** */
/* ************************************************************************ */


/*
 * --------------- SCP Status -----------------
 *
 *	Return code:	>= 0	good staus
 *			<  0	bad status
 *
 * --------------------------------------------
 */
#define	SCPSTAT_OK		0x00	/* SCP Status OK */
#define	SCPSTAT_FAILED		0xFF
#define	SCPSTAT_NOTSUPPORTED	0xAA	/* not supported op/feature */
#define	SCPSTAT_ERROR		0x80
#define	SCPSTAT_BAD_SAINT_NUM	0x81	/* Invalid SAINT/SAGE number */
#define	SCPSTAT_BAD_PARAM	0x82	/* Invalid parameter(s) */
#define SCPSTAT_BAD_CRD_INSTNC	0x83	/* Invalid ATM card instance */


/* SAP definitions */


#define NMP_CONFIG_SAP		0x01
#define NMP_FDDI_SCP_SAP        0x02


/* Well known Global SCP SAP for the FDDI/MONTEREY card */
#define FDDI_SCP_SAP_BASE       0x4F
#define FDDI_SCP_SAP    	0X50    /* FDDI SCP message handler task */

/* Various SAPs for the ATM line card, ALC. */
#define	ALC_SAP_BASE		(0x60)		/* Start ALC specific SAPs. */
#define	ALC_SCP_SAP		(ALC_SAP_BASE)			/* 0x60 */
#define	ALC_RESET_ACK_SAP     	(ALC_SAP_BASE+1)		/* 0x61 */


/*
 * --------------- Back door messages using Dual Port RAM ---------------
 *
 * All numbers are offsets relative to Dual Port RAM (size 1K).
 */

#define DP_MSG_TRAFFIC_METER1	0x3FE	/* Traffic meter 1-byte data */
#define DP_MSG_TRAFFIC_METER2	0x3FF	/* Duplicate for redundency */

#endif
