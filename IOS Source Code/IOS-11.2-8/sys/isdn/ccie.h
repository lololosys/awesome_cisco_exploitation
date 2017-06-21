/* $Id: ccie.h,v 3.3.52.1 1996/04/30 23:07:04 dclare Exp $
 * $Source: /release/112/cvs/Xsys/isdn/ccie.h,v $
 *------------------------------------------------------------------
 * ccie.h  Q.931 message information element literal definition 
 *
 * March 1992, Ronnie Kon
 *
 * Copyright (c) 1992-1997 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: ccie.h,v $
 * Revision 3.3.52.1  1996/04/30  23:07:04  dclare
 * CSCdi56247:  fix ISDN code to use proper prototyping and not use DIRTY
 * or DUSTY.
 * Branch: California_branch
 *
 * Revision 3.3  1995/11/28  22:51:51  dclare
 * CSCdi44526:  ISDN PRI NET5 changes for Italian homologation. Changes to
 * handle corner test cases. Restart handling changed to handle interfaces,
 * a channel, and multiple channels.
 *
 * Revision 3.2  1995/11/17  17:39:47  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:12:41  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/10/19  15:10:55  dclare
 * CSCdi42248:  PRI doesnt support international called party number with
 * Accunet. For a 4ESS switchtype only, if a number starts with 011 or
 * x011, then mark it as international.
 *
 * Revision 2.1  1995/06/07  21:14:07  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
/*************************************************************************
**************************************************************************

MODULE:       ccie.h
DESCRIPTION:  Q.931 message information element literal definitions
VERSION:      3.21

*************************************************************************
****copyright_c**********************************************************

                         COPYRIGHT STATEMENT

                        Copyright (c) 1991,92
                          by Telenetworks
                        Petaluma, CA   94952
                           (707) 778-8737

All rights reserved. Copying, compilation, modification, distribution
or any other use whatsoever of this material is strictly prohibited
except in accordance with a Software License Agreement with
Telenetworks.

****copyright_c**********************************************************
****history**************************************************************
****history**************************************************************
*************************************************************************
*
* GENERAL NOTES
*
* This include file contains definitions that are shared between tasks
* interested in Q.931 Information Elements.
*
*/

#ifndef     CCIE_H                        /* prevent nested inclusions */
#define     CCIE_H                                      /* module name */

/************************************************************************
*                           C O N S T A N T S                           *
*************************************************************************
*/
/* Codeset specifiers
*/
#define CODESET_0                 0             /* The default codeset */
#define CODESET_5                 5       /* Reserved for national use */
#define CODESET_6                 6  /* Reserved for local network use */
#define CODESET_7                 7 /* Reserved for user-specific info */

/* Length of standard Information Element Header.
*  IE_ID = 1 byte, IE_LEN = 1 byte.
*/
#define IE_HEAD_LENGTH            2
#define MAX_NUMB_INFO_FIELDS     260                 /* IE info fields */

/* ********** The Codeset 0 Information Element Identifiers ************ */

/* Single-byte Codeset 0 information elements
*/
#define CODESET_SHIFT_IE       0x90          /* Codeset shift - Type 1 */
#define MORE_DATA_IE           0xA0              /* More data - Type 2 */
#define SENDING_COMPLETE_IE    0xA1          /* Send complete - Type 2 */
#define CONGEST_LEVEL_IE       0xB0       /* Congestion level - Type 1 */
#define REPEAT_IND_IE          0xD0       /* Repeat indicator - Type 1 */

/* Multi-byte Codeset 0 information elements
*/
#define SEGMENTED_MSG_IE       0x00               /* Segmented message */
#define CHANGE_STATUS_IE       0x01         /* Change status maint. ie */
#define BEARER_CAPABILITY_IE   0x04               /* Bearer capability */
#define CAUSE_IE               0x08                      /* Cause elem */
#define CONNECTED_NUM_IE       0x0C                /* Connected number */

#define CALL_IDENT_IE          0x10                   /* Call identity */
#define CALL_STATE_IE          0x14                      /* Call state */
#define CHANNEL_ID_IE          0x18          /* Channel identification */
#define FACILITY_IE            0x1C                /* Facility element */
#define PROGRESS_IND_IE        0x1E              /* Progress indicator */

#define NETWORK_FACILITY_IE    0x20       /* Network-specific facility */
#ifdef VN2
#define NATIONAL_FACILITY_IE   NETWORK_FACILITY_IE      /* VN3 element */
#endif
#define ENDPOINT_ID_IE         0x26         /* Endpoint Identification */
#define NOTIFY_IND_IE          0x27          /* Notification indicator */
#define DISPLAY_IE             0x28                 /* Display element */
#define TIME_DATE_IE           0x29               /* Time/Date element */
#define KEYPAD_FACILITY_IE     0x2C                 /* Keypad facility */

#define INFO_REQ_IE            0x32             /* Information request */
#define SIGNAL_IE              0x34                  /* Signal element */
#define SWITCHHOOK_IE          0x36              /* Switchhook element */
#define FEATURE_ACTIV_IE       0x38              /* Feature activation */
#define FEATURE_IND_IE         0x39              /* Feature indication */

#define INFO_RATE_IE           0x40                /* Information rate */
#define TRANSIT_DELAY_EE_IE    0x42        /* End to end transit delay */
#define TRANSIT_DELAY_SEL_IE   0x43    /* Transit delay select & indic */
#define PACKET_BIN_PARMS_IE    0x44  /* Packet layer binary parameters */
#define PACKET_WINDOW_IE       0x45        /* Packet layer window size */
#define PACKET_SIZE_IE         0x46                     /* Packet size */

#define CALLER_NUMBER_IE       0x6C            /* Calling party number */
#define CALLER_SUB_ADDR_IE     0x6D       /* Calling party sub-address */

#define CALLED_NUMBER_IE       0x70             /* Called party number */
#define CALLED_SUB_ADDR_IE     0x71        /* Called party sub-address */
#define ORIGINAL_CALLED_NUM_IE 0x73          /* Original Called Number */
#define REDIRECTING_NUM_IE     0x74              /* Redirecting number */
#define REDIRECTION_NUM_IE     0x76              /* Redirection number */
#define TRANSIT_NETWORK_SEL_IE 0x78       /* Transit network selection */
#define RESTART_IND_IE         0x79             /* Re-start indication */
#define USER_USER_FAC_IE       0x7A           /* User-to-user facility */
#define LOW_LAYER_COMPAT_IE    0x7C         /* Low layer compatibility */
#define HIGH_LAYER_COMPAT_IE   0x7D        /* High layer compatibility */
#define USER_USER_IE           0x7E                    /* User-to-User */
#define ESCAPE_FOR_EXT_IE      0x7F            /* Escape for extension */

#ifdef BRI_1TR6
#define CONNECTED_ADDRESS_IE   0x0C      					 /* phone number */
#define FACILITY_SELECT_IE     0x05     /* Network specific facilities */
#define DESTINATION_ADDRESS_IE 0x70                 /* Called Party IE */
#define ORIGINATION_ADDRESS_IE 0x6c              /* Calling Party IE   */
/*
#define LEER_ELEMENT           0x78       For special cause process */
#define LEER_ELEMENT           0x70       /* For special cause process ===*/
#define TIMEOUT_1TR6           0x71      /* For Local Host/CC Timeouts */
#define CAUSE_1TR6_INVALID     0x01                          /* ditto  */
#define DATE_IE_LEN            0x13    /* for syntax checks of date IE */
#endif
#ifdef PRI_1TR6
#define CONNECTED_ADDRESS_IE   0x0C      					 /* phone number */
#define FACILITY_SELECT_IE     0x05     /* Network specific facilities */
#define DESTINATION_ADDRESS_IE 0x70                 /* Called Party IE */
#define ORIGINATION_ADDRESS_IE 0x6c              /* Calling Party IE   */
/*
#define LEER_ELEMENT           0x78       For special cause process */
#define LEER_ELEMENT           0x70       /* For special cause process ===*/
#define TIMEOUT_1TR6           0x71      /* For Local Host/CC Timeouts */
#define CAUSE_1TR6_INVALID     0x01                          /* ditto  */
#define DATE_IE_LEN            0x13    /* for syntax checks of date IE */
#define KEINE_ANGABE           0x01 
#define TLN_WIRD_GERUFEN       0x02 
#endif 


/************ The Codeset 6 Information Element Identifiers ************/

#define ADVICE_OF_CHARGE_IE    0x01        /* NTT-KDD advice of charge */
#define USER_CODE_IE           0x02             /* User Code, PRI_5ESS */
#define TRAVEL_CLASSMARK_IE    0x08  /* Traveling Classmark PRI_4/5ESS */
#define ORIG_CALL_APPEAR_IE    0x23                  /* Origination CA */
#define TERMINAL_CAP_IE        0x24        /* Terminal capabilities IE */
#define DISPLAY_IE             0x28                         /* Display */
#define OTHER_CALL_REF_IE      0x37                  /* Other call ref */
#ifdef VN2
#define USER_USER_FAC_IE       0x7A              /* User-User Facility */
#define	MODE_DE_FONCTIONNEMENT_D_USER	0xA4		/* === */
#endif
#ifdef BRI_1TR6
#define SERVICE_INDICATOR_IE    0x01  		/* 1TR6 Bearer Capability IE */
#define CHARGING_INFORMATION_IE 0x02  		 /* 1TR6 Advice of Charges   */
#define DATE_IE                 0x03   		 /* 1TR6 Date and Time    */
#define STATUS_FACILITIES_IE    0x06        			 /* Facility Status */
#define STATUS_GERUF_TLN_IE     0x07      	  /* Calling party status */
#define ADD_TRANSMIT_ATTR_IE    0x08   /* 1TR6 Add transmit attributes */
#endif
#ifdef PRI_1TR6 
#define SERVICE_INDICATOR_IE    0x01  		/* 1TR6 Bearer Capability IE */
#define CHARGING_INFORMATION_IE 0x02  		 /* 1TR6 Advice of Charges   */
#define DATE_IE                 0x03   		 /* 1TR6 Date and Time    */
#define STATUS_FACILITIES_IE    0x06        			 /* Facility Status */
#define STATUS_GERUF_TLN_IE     0x07      	  /* Calling party status */
#define ADD_TRANSMIT_ATTR_IE    0x08   /* 1TR6 Add transmit attributes */
#endif 

#ifdef BRI
#ifdef COMMAND_INTERFACE
/* special custom IE used by CCBRI only
*/
#define CHANNEL_STATUS_IE      0xF0
#endif
#endif

/************************** Coding Standards ***************************/

#define CCITT_STANDARD            0                 /* Coding standard */
#define CEPT_NATIONAL_STANDARD    2          /* CEPT/National standard */


/********************** Standard ISDN Locations ************************/

#define LOC_USER               0X00                            /* User */
#define LOC_PRIV_NETWORK       0X01  /* Private network <-> local user */
#define LOC_PUB_NETWORK        0X02   /* Public network <-> local user */
#define LOC_TRANSIT_NETWRK     0X03                 /* Transit network */
#define LOC_PUB_NETWORK_REMOTE 0X04  /* Public network <-> remote user */
#define LOC_PRIV_NETWRK_REMOTE 0X05 /* Private network <-> remote user */
#define LOC_PRIV_NETWRK_BEYOND 0X0A/* Priv network beyond interworking */


/************************************************************************
* Miscellaneous constants organized alphabetically by the info element  *
* with which they are associated.  New constants are added as needed.   *
************************************************************************/

/********************* For BEARER_CAPABILITY_IE *************************
*/
/* Information Transfer Capability
*/
#define BEAR_CAP_SPEECH        0x00        /* Speech Bearer Capability */
#define BEAR_CAP_UNREST_DIG    0x08 /* Unrestricted Digital Capability */
#define BEAR_CAP_REST_DIG      0x09   /* Restricted Digital Capability */
#define BEAR_CAP_3DOT1K_AUDIO  0x10         /* 3.1KHz Audio Capability */
#define BEAR_CAP_7K_AUDIO      0x11           /* 7KHz Audio Capability */
#define BEAR_CAP_VIDEO         0x18                /* Video Capability */

/*
 * Information Transfer Mode, circuit or packet
 */
#define BEAR_RATE_XFER_CIRCUIT 0x00                /* circuit mode */

/* Information Transfer Rate
*/
#define BEAR_RATE_64KBPS       0x10                 /* B_CHANNEL_UNITS */
#define BEAR_RATE_384KBPS      0x13                /* H0_CHANNEL_UNITS */
#define BEAR_RATE_1536KBPS     0x15               /* H11_CHANNEL_UNITS */
#define BEAR_RATE_1920KBPS     0x17               /* H12_CHANNEL_UNITS */

/************************ For CALL_STATE_IE *****************************
*/
#define CALL_STATE_MASK        0x3F                 /* call state mask */
#define CALL_STATE_LENGTH      0x01              /* Internal IE length */

/********************* For CALLER/ED_NUMBER_IE **************************
*/
/* Number types
*/
#define EN_BLOC_NUMBER         0x00          /* Number is sent en-bloc */
#define UNKNOWN_NUMBER         0x00                  /* Unknown number */
#define INTL_NUMBER            0x01            /* International number */
#define NAT_NUMBER             0x02                 /* National number */
#define LOC_NUMBER             0x04        /* Loacl (directory) number */
#define OVERLAP_NUMBER         0x05          /* Number is sent overlap */

/* Numbering plans
*/
#define UNKNOWN_NUMB_PLAN      0x00                    /* Unknown plan */
#define ISDN_NUMB_PLAN         0x01           /* ISDN numb. plan E.164 */
#define TELEPHONY_NUMB_PLAN    0x02      /* Telephony numb. plan E.163 */
#define PRIVATE_NUMB_PLAN      0x09          /* Private numbering plan */

/* Presentation indicators
*/
#define PRESENTATION_ALLOWED   0x00                /* Default AT&T BRI */

/* Screening indicators
*/
#define USER_PROVIDED          0x00                /* Default AT&T BRI */

/*********** For CALLER_SUB_ADDR_IE and CALLED_SUB_ADDR_IE **************
*/
#define OSI_SUB_ADDR           0x00                 /* OSI Sub-address */
#define USER_SPECIFIC_SUB_ADDR 0x02       /* User-specific Sub-address */
#define IA_5_FORMAT            0x50   /* IA 5 sub-address digit format */
#ifdef KDD_NTT
#define MAX_SUB_ADDR_LEN         32                   /* Japanese Len */
#else
#define MAX_SUB_ADDR_LEN         23                   /* CCITT default */
#endif

/************************** For CAUSE_IE ********************************
*/
#define CAUSE_LENGTH           0x02             /* Internal length = 2 */
#define DIAG_LEN               0x03       /* Maximum diagnostic length */
#define NO_DIAG                0x00   /* No CAUSE diagnostic specified */

/* Cause values
*
* NOTE: Not all cause values are universally supported across switch types.
*       Before a particular cause value is used, its validity should be
*       compared with the appropriate switch vendor specs.
*/
#define UNASSIGNED_NUMBER      0x01                        /* Cause 01 */
#define NO_ROUTE               0x02                        /* Cause 02 */
#define CHANNEL_UNACCEPTABLE   0x06                        /* Cause 06 */
#define NORMAL_CLEARING        0x10                        /* Cause 16 */
#define USER_BUSY              0x11                        /* Cause 17 */
#define NO_USER_RESPONDING     0x12                        /* Cause 18 */
#define CALL_REJECTED          0x15                        /* Cause 21 */
#define NUMBER_CHANGED         0x16                        /* Cause 22 */
#define DEST_OUT_OF_ORDER      0x1B                        /* Cause 27 */
#define INVALID_NUMBER_FORMAT  0x1C                        /* Cause 28 */
#define FACILITY_REJECTED      0x1D                        /* Cause 29 */
#define RESP_TO_STAT_ENQ       0x1E                        /* Cause 30 */
#define UNSPECIFIED_CAUSE      0x1F                        /* Cause 31 */
#define NO_CIRCUIT_AVAILABLE   0x22                        /* Cause 34 */
#define NETWORK_OUT_OF_ORDER   0x26                        /* Cause 38 */
#define TEMPORARY_FAILURE      0x29                        /* Cause 41 */
#define NETWORK_CONGESTION     0x2A                        /* Cause 42 */
#define ACCESS_INFO_DISCARDED  0x2B                        /* Cause 43 */
#define REQ_CHANNEL_NOT_AVAIL  0x2C                        /* Cause 44 */
#define PRE_EMPTED             0x2D                        /* Cause 45 */
#define FACILITY_NOT_SUBSCRIBED0x32                        /* Cause 50 */
#define OUTGOING_CALL_BARRED   0x34                        /* Cause 52 */
#define INCOMING_CALL_BARRED   0x36                        /* Cause 54 */
#define BEAR_CAP_NOT_AVAIL     0x3A                        /* Cause 58 */
#define SERVICE_NOT_AVAIL      0x3F                        /* Cause 63 */
#define CAP_NOT_IMPLEMENTED    0x41                        /* Cause 65 */
#define CHAN_NOT_IMPLEMENTED   0x42                        /* Cause 66 */
#define FACILITY_NOT_IMPLEMENT 0x45                        /* Cause 69 */
#define INVALID_CALL_REF       0x51                        /* Cause 81 */
#define CHAN_DOES_NOT_EXIST    0x52                        /* Cause 82 */
#define INCOMPATIBLE_DEST      0x58                        /* Cause 88 */
#define INVALID_MSG_UNSPEC     0x5F                        /* Cause 95 */
#define MANDATORY_IE_MISSING   0x60                        /* Cause 96 */
#define NONEXISTENT_MSG        0x61                        /* Cause 97 */
#define WRONG_MESSAGE          0x62                        /* Cause 98 */
#define BAD_INFO_ELEM          0x63                        /* Cause 99 */
#define INVALID_ELEM_CONTENTS  0x64                       /* Cause 100 */
#define WRONG_MSG_FOR_STATE    0x65                       /* Cause 101 */
#define TIMER_EXPIRY           0x66                       /* Cause 102 */
#define MANDATORY_IE_LEN_ERR   0x67                       /* Cause 103 */
#define PROTOCOL_ERROR         0x6F                       /* Cause 111 */
#define INTERWORKING_UNSPEC    0x7F                       /* Cause 127 */

#ifdef BRI_1TR6
#define USER_BUSY_1TR6         0x3B                        /* Cause 59 */
#define OUT_OF_ORDER_1TR6      0x39                        /* Cause 57 */
#define CALL_REJECTED_1TR6     0x3E                        /* Cause 62 */
#endif
#ifdef PRI_1TR6
#define USER_BUSY_1TR6         0x3B                        /* Cause 59 */
#define OUT_OF_ORDER_1TR6      0x39                        /* Cause 57 */
#define CALL_REJECTED_1TR6     0x3E                        /* Cause 62 */
#endif 

/*
 * Modem Error cause codes
 *
 * Note: these are not standard cause codes
 * nor are they part of the cause ie.
 * They are used between the async modem
 * code and the ISDN code to indicate
 * errors 
 */
#define INVALID_MODEM_STATE    0xFA
#define INVALID_MODEM_INFO     0xFB
#define NULL_MODEM_INFO        0xFC
#define UNDEFINED_B_IDB        0xFD
#define UNKNOWN_MODEM_ERROR    0xFE
#define UNFOUND_B_CHANNEL      0xFF

/*********************** For CHANGE_STATUS_IE ***************************
*/
#define INTERFACE_PREF         0x00            /* Interface preference */
#define CHANNEL_PREF           0x01              /* Channel preference */
#define CHANGE_STATUS_LENGTH   0x01                   /* len of IE = 1 */


/************************* For CHANNEL_ID_IE ****************************
*/
/* B-Channel choices for the Channel Identification IE.
*/
#define NO_CHANNEL                0                    /* No B-Channel */
#define B1_CHANNEL                1                    /* B1 B-Channel */
#define B2_CHANNEL                2                    /* B2 B-Channel */
#define ANY_CHANNEL               3                   /* Any B-Channel */
#define CHANNELS_LISTED           1         /* Channel selection value */
#define CHANNEL_NUMBERS           0   /* Chan numbers in CHANNEL_ID IE */
#define CHANNEL_SLOT_MAP          1       /* Slot map in CHANNEL_ID IE */
#define NO_CHANNEL_UNITS          0                    /* No Chan type */
#define B_CHANNEL_UNITS           3  /* Chan type -   single B channel */
#define H0_CHANNEL_UNITS          6  /* Chan type -    H0 chan (6 B's) */
#define H11_CHANNEL_UNITS         8  /* Chan type -  H11 chan (24 B's) */
#define H12_CHANNEL_UNITS         9  /* Chan type -  H12 chan (30 B's) */
#define CHANNEL_ID_LEN_BRI     0x01        /* Internal IE length - BRI */
#define CHANNEL_ID_LEN_PRI     0x02        /* Internal IE length - PRI */
#define SLOT_MAP               0x10        /* Slot map in CHANNEL_ID IE */
#define PRI_INTERFACE          0x20        /* Pri int in CHANNEL_ID IE  */


/*********************** For CONGEST_LEVEL_IE ***************************
*/
#define NOT_CONGESTED             0           /* Level = NOT_CONGESTED */
#define CONGESTED              0x0F               /* Level = CONGESTED */

/************************** For DISPLAY_IE *****************************/
#define DISPLAY_IE_LEN           34

/************************ For ENDPOINT_ID_IE ****************************
*/
#define ENDPOINT_ID_LENGTH        2   /* length of endpoint id element */


/********************** For HIGH_LAYER_COMPAT_IE ************************
*/

#define NATIONAL_INTERPRETATION   0            /* Interpretation value */
#define CCITT_CEPT_INTERPRETATION 4            /* Interpretation value */
#define PROFILE_PRESENTATION      1              /* Presentation value */

/* CCITT Teleservices
*/
#define TELEPHONY_SERVICE      0x01                     /* Teleservice */
#define GR3_FAX_SERVICE        0x04                     /* Teleservice */
#define TELETEX_MIXED_SERVICE  0x24                     /* Teleservice */
#define GR4_FAX_SERVICE        0x21                     /* Teleservice */
#define TELETEX_CHAR_SERVICE   0x31                     /* Teleservice */
#define ALPHA_MOS_VID_SERVICE  0x32                     /* Teleservice */
#define TELEX_SERVICE          0x35                     /* Teleservice */
#define MHS_SERVICE            0x38                     /* Teleservice */
#define OSI_APPL_SERVICE       0x41                     /* Teleservice */

/* CEPT Teleservices
*/
#define UNKNOWN_SERVICE        0x00      /* Teleservice (interworking) */

/* National Standard Teleservices
*/
#define AUDIOSCRIPT_SERVICE    0x40                     /* Teleservice */
#define AUDIOVIDEOTEX_SERVICE  0x48                     /* Teleservice */
#define NON_STD_APPL_SERVICE   0x7F                     /* Teleservice */

#define HIGH_LAYER_LENGTH         2 /* High Layer Compatability length */


/********************** For KEYPAD_FACILITY_IE **************************
*/
#define KEYPAD_FACILITY_LENGTH    1                /* Keypad IE length */


/************************ For PROGRESS_IND_IE ***************************
*/
#define PROG_IND_LENGTH           2       /* Progress Indicator length */

/* Progress values
*/
#define CALL_NOT_END_TO_END_ISDN  1        /* Progress indicator value */
#define IN_BAND_INFO              8        /* Progress indicator value */


/************************ For RESTART_IND_IE ****************************
*/
#define BASIC_RATE_INTERFACE      0        /* Restart applies to a BRI */
#define CHANNEL_NUMBER_PTR        4       /* Loc of chan numb(s) in IE */
#define RESTART_SINGLE            0       /* Restart selected channels */
#define ALL_CHANNELS           0xFF         /* All channels identifier */
#define RESTART_SINGLE_INTERFACE 0x06      /* Restart all on interface */
#define RESTART_ALL            0x07            /* Restart all channels */
#define RESTART_IND_LENGTH        1              /* Internal IE length */


/*************************** For SIGNAL_IE ******************************
*/
#define SIGNAL_IE_LENGTH       0x01                          /* Length */

/* Signal values
*/
#define DIAL_TONE_ON           0x00
#define RINGBACK_ON            0x01
#define INTCPT_TONE_ON         0x02
#define REORDER_TONE_ON        0x03
#define BUSY_TONE_ON           0x04
#define CONFIRM_TONE_ON        0x05
#define ANSWER_TONE_ON         0x06
#define CW_TONE_ON             0x07
#define OFFHOOK_WARN_TONE_ON   0x08
#define CUSTOM_TONE_ON         0x09
#define RECALL_DIAL_TONE_ON    0x0A
#define BUSY_VERIFY_TONE_ON    0x0B
#define ERROR_TONE_ON          0x0C
#define STUTTER_DT_TONE_ON     0x0D
#define EXPEN_ROUTE_TONE_ON    0x0E
#define TONES_OFF              0x3F
#define ALERT_ON_PATT_0        0x40
#define ALERT_ON_PATT_1        0x41
#define ALERT_ON_PATT_2        0x42
#define ALERT_ON_PATT_3        0x43
#define ALERT_ON_PATT_4        0x44
#define ALERT_ON_PATT_5        0x45
#define ALERT_ON_PATT_6        0x46
#define RESERVED_SIGNAL        0x47
#define ALERTING_OFF           0x4F
#define SIGNAL_RESERVED        0x60
#define UNSPECIFIED_TONE       0x7F


/************************ For SWITCHHOOK_ID_IE **************************
*/
/* VHCB States in Call Control
*/
#define ONHOOK_IDLE            0x00                /* On hook and idle */
#define OFFHOOK                0x01       /* Receiver/speaker off hook */
#undef BELL	/* === */
#define BELL                   0x02          /* Audibly (BELL) ringing */
#define DIAL_ONHOOK            0x03            /* Dialing before SETUP */
#define SWITCHHOOK_LENGTH      0x01              /* Internal IE length */


/************************* For TERMINAL_CAP_IE **************************
*/
#define NATIONAL_FUNCTION         0          /* Capability description */
#define TYPE_2_STIMULUS           2          /* Capability description */
#define TERMINAL_CAP_LENGTH       1                 /* Coding standard */


/************************** For USER_USER_IE ****************************
*/
#define USER_SPEC_PROTO        0x00          /* User-specific protocol */
#define OSI_HIGH_LAYER_PROTO   0x01         /* OSI high layer protocol */
#define X244_PROTO             0x02                  /* X.244 protocol */
#define MAINT_MGMT_PROTO       0x03 /* Maintenance/management protocol */
#define IA5_CHAR_PROTO         0x04          /* IA5 character protocol */
#define V120_RATE_ADPT_PROTO   0x07    /* V.120 rate adaption protocol */
#define Q931_USER_NET_PROTO    0x08     /* Q.931 user-network protocol */
#define NT2_NT2_FAC_PROTO      0x40 /* VN2 NT2-NT2 facilities protocol */
#define MINI_MSG_SERV_PROTO    0x41  /* VN2 mini-msg handling protocol */
#define MAX_UUI_LEN             131       /* A network-dependent value */
/************************* For INFORMATION_REQ_IE **********************
*/
#define AUTHORIZATION_CODE     0x01     /* Authorization Code === */
#define ADDRESS_DIGITS         0x02                  /* Address === */
#define TERMINAL_ID            0x03  /* Terminal Identification === */

/************************* For NETWORK_FACILITY_IE ********************/
#define SEMI_PERMANENT_LINK    0x1f    /* German semi perm connection */
#define ACTIVATE_USE_OF_LINK   0x1d    /* Activate german semi perm link */
#define ALL_SERVICES           0x00    /* Applicable to all 1tr6 services */
#define NO_ADDITIONAL_INFO     0x00    /* Octet 6 of 1tr6 semi perm conn */

/*************************** OTHER CONSTANTS ****************************
*/
/* Constants used by CCIE.C in its core search routines.
*/
#define CALL_REF_LEN_OFFSET       1  /* Msg offset for call ref length */
#define CODESET_LEN               3  /* Bit size of codeset data field */
#define CODESET_SHIFT             1    /* Codeset shift information ID */
#define EXTENSION_MASK          128 /* Indicates last byte in sequence */
#define FIXED_L3_MSG_HEADER_LEN   3            /* At least three bytes */
#define NON_LOCKING_SHIFT_MASK    8 /* Codeset shift IE data field bit */
#define SINGLE_BYTE_DATA_LEN      4  /* Single-byte IE data field size */
#define SINGLE_BYTE_DATA_LOC      0 /* Single-byte IE data field start */
#define SINGLE_BYTE_ID_LEN        3  /* Single-byte IE ID field length */
#define SINGLE_BYTE_ID_LOC        4   /* Single-byte IE ID field start */
#define SINGLE_BYTE_TYPE_2        2 /* For Type 2 one byte ie decoding */

/************************************************************************
*               D A T A   T Y P E   D E F I N I T I O N S               *
*************************************************************************
*/
/* Call Control Information Element Structures
*/

/* Common element header for multi-byte information elements
*/
typedef struct
    {
    BYTE Ident;                                  /* Element identifier */
    BYTE Len;          /* Element size (in bytes) excluding the header */
    } IE_HEAD;

/* CALL_STATE_IE (0x14) - Call State Information Element
*/
typedef struct
    {
    IE_HEAD     IE_Header;         /* Common multi-byte element header */
    BYTE        Call_State;      /* Sender's L3 protocol control state */
    } *CALL_STATE_ELEM;

/* Additional IE structure definitions should be added here as needed.
*/
/************************************************************************
*                              M A C R O S                              *
*************************************************************************
*/
/* Account for call reference (cr) flag bit and cr length (l) to detect
*  the Global Call Reference.  The length is assumed to be 1 or 2 octets.
*/
#define GLOBAL_CALL_REF(cr,l) (l==2) ? (!( (cr) & 0x7FFF)) : (!((cr) & 0x7F))

/************************************************************************
*                     D A T A   A L L O C A T I O N                     *
*************************************************************************
*/
/************************************************************************
*                F U N C T I O N   D E F I N I T I O N S                *
*************************************************************************
*/
PUBLIC int     CCIE_BuildBearerCap_IE     (BYTE *, BYTE *, BYTE);
PUBLIC int     CCIE_BuildCallAppear_IE    (BYTE *, BYTE);
#ifdef VN2
PUBLIC int     CCIE_BuildCallIdentity_IE  (BYTE *, BYTE *, int);
#endif
PUBLIC int     CCIE_BuildCallState_IE     (BYTE *, BYTE);
PUBLIC int     CCIE_BuildCallingNumb_IE   (BYTE *, int, int, \
                                                int, int, BYTE *, int, \
                                                BOOLEAN);
PUBLIC int     CCIE_BuildCause_IE         (BYTE *, BYTE, BOOLEAN, \
                                                BYTE *);
PUBLIC int     CCIE_BuildChangeStatus_IE  (BYTE *, BYTE, BYTE);
PUBLIC int     CCIE_BuildChanId_IE        (BYTE *, int, int, int, \
                                                BOOLEAN, BOOLEAN, int, \
                                                BYTE *);
PUBLIC int     CCIE_BuildCodesetShift_IE  (BYTE *, BYTE, BOOLEAN);
PUBLIC int     CCIE_BuildCongLevel_IE     (BYTE *, int);
PUBLIC int     CCIE_BuildEndpointId_IE    (BYTE *, int, int, int);
PUBLIC int     CCIE_BuildHighLayer_IE     (BYTE *, int, int, int, int);
PUBLIC int     CCIE_BuildHookState_IE     (BYTE *, BYTE);
PUBLIC int     CCIE_BuildKeypad_IE        (BYTE *, BYTE *, int, BOOLEAN);
#ifdef VN2
PUBLIC int     CCIE_BuildNatFac_IE        (BYTE *, BYTE *, int);
#endif
#ifdef PRI
PUBLIC int     CCIE_BuildNetFac_IE        (BYTE *, int, int, int,    \
                                                BYTE *, BOOLEAN, BOOLEAN, \
                                                BOOLEAN, int, BYTE *, int);
#endif                                                      /* BRI/PRI */
#ifdef PRI_1TR6
PUBLIC int     CCIE_BuildNetFac_IE_1TR6   (BYTE *, BYTE , BOOLEAN, BYTE *);
#endif 
PUBLIC int     CCIE_BuildNotifyInd_IE     (BYTE *, BYTE *, int);
PUBLIC int     CCIE_BuildPartyNumb_IE     (BYTE *, int, int, int, \
                                                BYTE *, int, BOOLEAN);
PUBLIC int     CCIE_BuildProgInd_IE       (BYTE *, int, int, int);
PUBLIC int     CCIE_BuildRestartInd_IE    (BYTE *, int);
PUBLIC int     CCIE_BuildSignal_IE        (BYTE *, int);
PUBLIC int     CCIE_BuildTermCap_IE       (BYTE *, int, int);
PUBLIC int     CCIE_BuildUser_IE          (BYTE *, int, int, BYTE *, int);
PUBLIC BOOLEAN CCIE_CauseValid            (int);
PUBLIC void    CCIE_Change_Msg_Type       (BYTE *, int);
#ifdef KDD_NTT
PUBLIC BYTE   *CCIE_ExpandAdviceOfCharge_IE (BYTE *, BOOLEAN *);
#endif
PUBLIC BYTE   *CCIE_ExpandBearCap_IE      (BYTE *, BOOLEAN *);
PUBLIC BYTE   *CCIE_ExpandCause_IE        (BYTE *, BOOLEAN *);
PUBLIC BYTE   *CCIE_ExpandChanId_IE       (BYTE *, BOOLEAN *);
PUBLIC BYTE   *CCIE_ExpandChangeStatus_IE (BYTE *, BOOLEAN *);
PUBLIC BYTE   *CCIE_ExpandNetFac_IE       (BYTE *, BOOLEAN *);
PUBLIC BYTE   *CCIE_ExpandNotifyInd_IE    (BYTE *, BOOLEAN *);
PUBLIC BYTE   *CCIE_ExpandPartyNumb_IE    (BYTE *, BOOLEAN *);
PUBLIC BYTE   *CCIE_ExpandProgressInd_IE  (BYTE *, BOOLEAN *);
PUBLIC BYTE   *CCIE_ExpandSignal_IE       (BYTE *, BOOLEAN *);
PUBLIC BYTE   *CCIE_ExpandUser_IE         (BYTE *, BOOLEAN *);
PUBLIC BYTE   *CCIE_Get_IE                (BYTE *, int, BYTE, int, int);
PUBLIC BYTE   *CCIE_GetAnyIE              (BYTE *, int, BYTE, int, int);
#ifdef BRI_5ESS
PUBLIC int     CCIE_Get_OCRV              (BYTE *);
#endif
PUBLIC BYTE    CCIE_LenOf_L3_Msg_Header   (BYTE *);
PUBLIC BYTE    CCIE_Pack_IE               (BYTE, BYTE, BYTE, BYTE);
PUBLIC int     CCIE_Remove_IE             (BYTE *, int, BYTE, int, int); 
PUBLIC BOOLEAN CCIE_Single_Byte_IE        (BYTE, BYTE *, BYTE *);
PUBLIC BYTE    CCIE_Unpack_IE             (BYTE, BYTE, BYTE);

#endif
