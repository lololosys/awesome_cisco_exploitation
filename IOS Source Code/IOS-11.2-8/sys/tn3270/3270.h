/* $Id: 3270.h,v 3.2 1995/11/17 18:50:41 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/tn3270/3270.h,v $
 *------------------------------------------------------------------
 * 3 2 7 0 . H
 *
 * July, 1994, Bill Palter
 * 
 * Copyright (c) 1994 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: 3270.h,v $
 * Revision 3.2  1995/11/17  18:50:41  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:37:07  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:07:32  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __3270_LOADED
#define __3270_LOADED	1

/*
 * SNA Mode Data Stream Command Bytes 
 */


#define CMD_SNA_EAU		(0x6f)
#define CMD_SNA_EW		(0xf5)
#define CMD_SNA_EWA		(0x7e)
#define CMD_SNA_RB		(0xf2)
#define CMD_SNA_RM		(0xf6)
#define CMD_SNA_RMA		(0x6e)
#define CMD_SNA_W		(0xf1)
#define CMD_SNA_WSF		(0xf3)

/* 
 * Non SNA mode Data Stream Command Bytes 
 */


#define CMD_EAU		(0x0f)
#define CMD_EW		(0x05)
#define CMD_EWA		(0x0d)
#define CMD_RB		(0x02)
#define CMD_RM		(0x06)
#define CMD_W		(0x01)
#define CMD_WSF		(0x11)

/*
 * WCC bit defeinitions
 */

#define WCC_HIGH_BIT		(0x80)
#define WCC_RESET		(0x40)
#define WCC_START_PRINTER	(0x08)
#define WCC_SOUND_ALARM		(0x04)
#define WCC_KEYBOARD_RESTORE	(0x02)

#define WCC_RESET_MDT		(0x01)


/* 
 *  Define the ORDERs
 */

#define ORDER_SBA		(0x11)
#define ORDER_SF		(0x1D)
#define ORDER_SFE		(0x29)
#define ORDER_SA		(0x28)
#define ORDER_MF		(0x2c)
#define ORDER_IC		(0x13)
#define ORDER_PT		(0x05)
#define ORDER_RA		(0x3c)
#define ORDER_EUA		(0x12)
#define ORDER_GE		(0x08)
#define ORDER_YALE		(0x2b)

#define YALE_DOLLAR		(0x5b)
#define YALE_QUERY		(0xbc)
#define YALE_QUERY_ATTN		(0xbe)

#define YALE_CLEAR_MARK		(0xb4)
#define YALE_IMPROVED_NULL	(0xd5)
#define YALE_ZONES_MODE_OFF	(0xe9)
#define YALE_RESTORE_ENTER_NL	(0xc5)
#define YALE_RESTORE_FLD_CLM_TAB (0xc3)
#define YALE_DISABLE_ASYNC_STATUS (0xe2)
#define YALE_3270_NUM_ONLY	(0xe5)
#define YALE_RESTORE_PACING	(0x97)
#define YALE_APL_OFF		(0xc1)
#define YALE_3278_INSERT	(0x89)
#define YALE_PRIMARY_ATT_DIS	(0xc4)
#define YALE_UNKNOWN1		(0x88)
#define YALE_SET_HOME_LEFT_COL  (0xe3)



#define EBCDIC_SPACE		(0x40)
#define EBCDIC_NULL		(0x00)

/*
 * Define Field Attribute Bits 
 */

#define FA_PROT_MASK		(0x20)
#define FA_NUMERIC_MASK 	(0x10)
#define FA_DISPLAY_MASK 	(0x0C)
#define FA_DISPLAY_NOSELECT	(0x00)
#define FA_DISPLAY_SELECT	(0x04)
#define FA_DISPLAY_INTENSIFIED	(0x08)
#define FA_DISPLAY_NODISPLAY	(0x0C)
#define FA_DISPLAY_MDT_MASK	(0x01)
#define FA_SKIP_MASK		(0x30)


/*
 * Define extended attribute types
 */

#define EXT_ATT_ALL			(0x00)
#define EXT_ATT_3270_FIELD		(0xc0)
#define EXT_ATT_FIELD_VALIDATION	(0xc1)
#define EXT_ATT_FIELD_OUTLINING		(0xc2)
#define EXT_ATT_HIGHLIGHTING     	(0x41)
#define EXT_ATT_FOREGROUND_COLOR 	(0x42)
#define EXT_ATT_CHARACTER_SET	 	(0x43)
#define EXT_ATT_BACKGROUND_COLOR 	(0x45)
#define EXT_ATT_TRANSPARENCY     	(0x46)


#define EA_CHAR_SET_GE			(0x01)
#define EA_CHAR_SET_DEFAULT		(0x00)
#define EA_CHAR_SET_APL			(0xf1)


#define EA_COLOR_DEFAULT		(0x00)
#define EA_COLOR_NEUTRAL_BLACK		(0xF0)
#define EA_COLOR_BLUE			(0xF1)
#define EA_COLOR_RED			(0xF2)
#define EA_COLOR_PINK			(0xf3)
#define EA_COLOR_GREEN			(0xf4)
#define EA_COLOR_TURQUOISE		(0xf5)
#define EA_COLOR_YELLOW			(0xf6)
#define EA_COLOR_NEUTRAL_WHITE		(0xf7)
#define EA_COLOR_BLACK			(0xf8)
#define EA_COLOR_DEEP_BLUE		(0xf9)
#define EA_COLOR_ORANGE			(0xfa)
#define EA_COLOR_PURPLE			(0xfb)
#define EA_COLOR_PALE_GREEN		(0xfc)
#define EA_COLOR_PALE_TURQUOISE		(0xfd)
#define EA_COLOR_GREY			(0xfe)
#define EA_COLOR_WHITE			(0xff)


#define EA_HIGHLIGHT_DEFAULT		(0x00)
#define EA_HIGHLIGHT_NORMAL		(0xf0)
#define EA_HIGHLIGHT_BLINK		(0xf1)
#define EA_HIGHLIGHT_REVERSE		(0xf2)
#define EA_HIGHLIGHT_INTENSIFY		(0xf3)
#define EA_HIGHLIGHT_UNDERSCORE		(0xf4)
 
#define EA_OUTLINE_MASK_NONE		(0x00)
#define EA_OUTLINE_MASK_UNDERLINE	(0x01)
#define EA_OUTLINE_MASK_RIGHT_VERTICAL	(0x02)
#define EA_OUTLINE_MASK_OVERLINE	(0x04)
#define EA_OUTLINE_MASK_LEFT_VERTICAL	(0x08)



/*
 * Define the Structured Field Codes, expand the one-byte code to two bytes 
 * to make it easy to parse
 */

#define	SF_RESET_PARTITION			(0x0000)
#define	SF_READ_PARTITION			(0x0001)
#define		SF_RP_QUERY			(0x02)
#define		SF_RP_QUERY_LIST		(0x03)
#define	SF_ERASE_RESET				(0x0003)
#define SF_ER_ALT_SIZE_MASK			(0x01)
#define	SF_LOAD_PS				(0x0006)
#define SF_SET_REPLY_MODE			(0x0009)
#define SF_SET_WINDOW_ORIGIN			(0x000B)
#define SF_CREATE_PARTITION			(0x000c)
#define SF_DESTROY_PARTITION			(0x000d)
#define SF_ACTIVATE_PARTITION			(0x000E)
#define SF_OUTBOUND_3270DS			(0x0040)
#define SF_SCS_DATA				(0x0041)
#define SF_SELECT_FORMAT_GROUP			(0x004A)
#define SF_PRESENT_ABSOLUTE_FORMAT		(0x004B)
#define SF_PRESENT_RELITIVE_FORMAT		(0x004C)
#define SF_INBOUND_3270DS			(0x0080)
#define SF_SET_MSR_CONTRL			(0x0F01)
#define SF_DESTINATION_ORIGIN			(0x0F02)
#define SF_SELECT_COLOR_TABLE			(0x0F04)
#define SF_LOAD_COLOR_TABLE  			(0x0F05)
#define SF_LOAD_LINE_TYPE    			(0x0F07)
#define SF_SET_PARTITION_CHARACTERISTICS	(0x0F08)
#define SF_MODIFY_PARTITION			(0x0F0A)
#define SF_OBJECT_DATA      			(0x0F0F)
#define SF_OBJECT_PICTURE   			(0x0F10)
#define SF_OBJECT_CONTROL    			(0x0F11)
#define SF_OEM_DATA          			(0x0F1F)
#define SF_DATA_CHAIN       			(0x0F21)
#define SF_EXCEPTION_STATUS  			(0x0F22)
#define SF_LOAD_FORMAT_STORAGE			(0x0F24)
#define SF_OUTBOUND_TEXT_HEADER			(0x0F71)
#define SF_SELECT_IPDS_MODE 			(0x0F83)
#define SF_SET_PRINTER_CHARACTERISTICS		(0x0F84)
#define SF_BEGIN_END_OF_FILE			(0x0F85)
#define SF_INBOUND_TEXT_HEADER			(0x0FB1)
#define SF_TYPE_1_TEXT				(0x0FC1)
#define SF_QUERY_REPLY				(0x0081)
#define SF_REQUEST_RECOVERY_DATA		(0x1030)
#define SF_RECOVERY_DATA			(0x1031)
#define SF_SET_CHECKPOINT_INTERVAL		(0x1032)
#define SF_RESTART              		(0x1033)
#define SF_SAVE_RESTORE_FORMAT  		(0x1034)
                                        	

#define SF_FLAGS_SPANF				(0xc0)
#define SF_SPANF_FIRST				(0x80)
#define SF_SPANF_LAST				(0x40)


#define QR_SUMMARY				(0X80)
#define QR_USABLE_AREA				(0X81)
#define 	QR_UA_SDP_ALTERNATE_USABLE_AREA	(0X02)
#define QR_IMAGE				(0X82)
#define QR_TEXT_PARTITIONS			(0X83)
#define QR_ALPHANUMERIC_PARTITIONS		(0x84)
#define QR_CHARACTER_SETS			(0X85)
#define QR_COLOR				(0X86)
#define QR_HIGHLIGHTING				(0X87)
#define QR_REPLY_MODES				(0X88)
#define QR_FIELD_VALIDATION			(0X8A)
#define QR_MSR_CONTROL				(0X8B)
#define QR_FIELD_OUTLINING			(0X8C)
#define QR_PARTITION_CHARACTERISTICS		(0X8E)
#define QR_OEM_AUXILIARY_DEVICE			(0X8F)
#define QR_FORMAT_PRESENTATION			(0X90)
#define QR_DBCS_ASIA				(0X91)
#define QR_SAVE_RESTORE_FORMAT			(0X92)
#define QR_FORMAT_STORAGE_AUXILIARY_DEVICE	(0X94)
#define QR_DISTRIBUTED_DATA_MANAGEMENT		(0X95)
#define QR_STORAGE_POOLS			(0X96)
#define		QR_SP_SDP_STORAGE_POOLS		(0x01)
#define QR_DOCUMENT_INTERCHANGE_ARCHITECTURE	(0X97)
#define QR_DATA_CHAINING			(0X98)
#define QR_AUXILIARY_DEVICE			(0x99)
#define QR_3270_IPDS				(0X9A)
#define QR_PRODUCT_DEFINED_DATA_STREAM		(0X9C)
#define QR_IBM_AUXILIARY_DEVICE			(0X9E)
#define QR_BEGIN_END_OF_FILE			(0X9F)
#define QR_DEVICE_CHARACTERISTICS		(0XA0)
#define QR_RPQ_NAMES				(0XA1)
#define QR_DATA_STREAMS				(0XA2)
#define QR_IMPLICIT_PARTITION			(0XA6)
#define 	QR_IP_SDP_PARTITION_SIZES	(0X01)
#define 	QR_IP_SDP_CELL_SIZES		(0x02)

#define QR_PAPER_FEED_TECHNIQUES		(0XA7)
#define QR_TRANSPARENCY				(0XA8)
#define QR_SETTABLE_PRINTER_CHARACTERISTICS	(0XA9)
#define QR_IOCA_AUXILIARY_DEVICE		(0XAA)
#define QR_COOPERATIVE_PROC_REQUESTOR		(0XAB)
#define QR_PROCEDURE				(0XB1)
#define 	QR_PROCEDURE_SDP_BUILT_IN	(0x01)
#define QR_LINE_TYPE				(0XB2)
#define QR_PORT					(0XB3)
#define QR_GRAPHICS_COLOR			(0XB4)
#define QR_EXTENDED_DRAWING_ROUTINE		(0XB5)
#define QR_GRAPHICS_SYMBOL_SETS			(0XB6)
#define		QR_GSS_SDP_SYMBOL_STORE		(0x01)
#define		QR_GSS_MASK_MULTIID		(0x40)
#define		QR_GSS_MASK_LOADPS		(0x20)
#define		QR_GSS_MASK_LOADPS_EXT		(0x10)
#define		QR_GSS_MASK_MULTI_PLANE		(0x80)
#define		QR_GSS_SDP_SYMBOL_STORE		(0x01)
#define		QR_GSS_MASK_LOADABLE		(0x80)
#define		QR_GSS_MASK_MULTI_COLOR		(0x40)
#define		QR_GSS_MASK_DOUBLE_BYTE		(0x20)
#define		QR_GSS_MASK_LCID_COMPARE	(0x10)
#define		QR_GSS_MASK_DYNAMIC		(0x08)
#define		QR_GSS_MASK_ECARREQ		(0x04)
#define		QR_GSS_MASK_USE_NOPATTERNS	(0x80)
#define		QR_GSS_MASK_USE_NOMARKERS	(0x40)
#define		QR_GSS_MASK_USE_PRECISION_1	(0x01)
#define		QR_GSS_MASK_USE_PRECISION_2	(0x02)
#define		QR_GSS_MASK_USE_PRECISION_3	(0x03)


#define QR_SEGMENT				(0XB0)
#define 	QR_SEGMENT_SDP_PROPERTY_VECTOR	(0x02)
#define		QR_SEGMENT_PROPERTY_DRAWING_ATT	(0x00)
#define		QR_SEGMENT_PROPERTY_PATTERN_ATT	(0x04)
#define QR_NULL					(0XFF)


#define RESERVED_BYTE			(0x00)
#define TRUE				1
#define FALSE				0



#define AID_NOAID		(0x60)
#define AID_PRINTER		(0xe8)
#define AID_SF			(0x88)
#define AID_READ_PARTITION	(0x61)
#define AID_TRIGGER		(0x7f)
#define AID_SYSREQ		(0xf0)
#define AID_PF1			(0xf1)
#define AID_PF2			(0xf2)
#define AID_PF3			(0xf3)
#define AID_PF4			(0xf4)
#define AID_PF5			(0xf5)
#define AID_PF6			(0xf6)
#define AID_PF7			(0xf7)
#define AID_PF8			(0xf8)
#define AID_PF9			(0xf9)
#define AID_PF10		(0x7a)
#define AID_PF11		(0x7b)
#define AID_PF12		(0x7c)
#define AID_PF13		(0xc1)
#define AID_PF14		(0xc2)
#define AID_PF15		(0xc3)
#define AID_PF16		(0xc4)
#define AID_PF17		(0xc5)
#define AID_PF18		(0xc6)
#define AID_PF19		(0xc7)
#define AID_PF20		(0xc8)
#define AID_PF21		(0xc9)
#define AID_PF22		(0x4a)
#define AID_PF23		(0x4b)
#define AID_PF24		(0x4c)
#define AID_PF25		(0xd1)
#define AID_PF26		(0xd2)
#define AID_PF27		(0xd3)
#define AID_PF28		(0xd4)
#define AID_PF29		(0xd5)
#define AID_PF30		(0xd6)
#define AID_PF31		(0xd7)
#define AID_PF32		(0xd8)
#define AID_PF33		(0xd9)
#define AID_PF34		(0x5a)
#define AID_PF35		(0x5b)
#define AID_PF36		(0x5c)
#define AID_PA1 		(0x6c)
#define AID_PA2 		(0x6e)
#define AID_PA3 		(0x6b)
#define AID_CLEAR		(0x6d)
#define AID_PARTITION		(0x6a)
#define AID_ENTER		(0x7d)
#define AID_SELPEN		(0xe6)
#define AID_TRANS1		(0xe4)
#define AID_TRANS2		(0xe8)

/* 
 * Graphics Order Definitions
 */

#define GSO_GNOP1		(0x00)		

#define GSO_GCOMT		(0x01)		
#define GSO_GPSCC		(0x03)
#define GSO_GSGCH		(0X04)
#define GSO_GCALLS		(0X07)

#define GSO_GSPS		(0X08)
#define GSO_GPSPT		(0x09)
#define GSO_GCOL		(0X0A)
#define GSO_GMX			(0X0C)
#define GSO_GBMX		(0X0D)

#define GSO_GFLW		(0X11)
#define GSO_GIA			(0X14)
#define GSO_GSSLW		(0X15)

#define GSO_GSLT		(0X18)
#define GSO_GSLW                (0X19)
#define GSO_GSLE                (0X1A)
#define GSO_GSLJ		(0X1B)

#define GSO_GSCP		(0X21)
#define GSO_GSAP		(0X22)
#define GSO_GPSPIK		(0X23)
#define GSO_GSTM		(0X24)
#define GSO_GSBCOL		(0X25)
#define GSO_GSECOL		(0X26)
#define GSO_GSVW		(0X27)

#define GSO_GSPT                (0X28)
#define GSO_GSMT		(0X29)
                       
#define GSO_GSTV   		(0X31)
#define GSO_GSSB		(0X32)
#define GSO_GSCC		(0X33)
#define GSO_GSCA		(0X34)	
#define GSO_GSCH		(0X35)
#define GSO_GSMC		(0X37)

#define GSO_GSCS		(0X38)
#define GSO_GSCR		(0X39)
#define GSO_GSCD		(0X3A)
#define GSO_GSMP		(0X3B)
#define GSO_GSMS		(0X3C)
#define GSO_GEPROL		(0X3E)
#define GSO_GPOP		(0X3F)

#define GSO_GSPIK		(0X41)

#define GSO_GPSPS		(0X48)
#define GSO_GEEL		(0X49)
#define GSO_GPSCOL		(0X4A)
#define GSO_GPSMX		(0X4C)
#define GSO_GPSBMX		(0X4D)

#define GSO_GPSFLW		(0X51)
#define GSO_GPSIA		(0X54)
#define GSO_GPSSLW		(0X55)
#define GSO_GPSLT		(0X58)
#define GSO_GPSLW		(0X59)
#define GSO_GPSLE		(0X5A)
#define GSO_GPSLJ		(0X5B)

#define GSO_GEAR		(0x60)
#define GSO_GPSCP		(0x61)
#define GSO_GPSAP		(0x62)
#define GSO_GPSTM		(0x64)
#define GSO_GPSBCOL		(0x65)
#define GSO_GPSECOL		(0x66)
#define GSO_GPSVW		(0x67)

#define GSO_GBAR		(0x68)
#define GSO_GPSMT		(0x69)

#define GSO_GESEG		(0x71)

#define GSO_GPSCA		(0x74)
#define GSO_GPSCH		(0x75)
#define GSO_GPSMC		(0x77)

#define GSO_GPSCS		(0x78)
#define GSO_GPSCR		(0x79)
#define GSO_GPSCD		(0x7A)
#define GSO_GPSMP		(0x7B)
#define GSO_GPSMS		(0x7C)
#define GSO_GCLFIG		(0x7D)
#define GSO_GEPTH		(0x7F)

#define GSO_GCBOX		 (0x80)
#define GSO_GCLINE               (0x81)
#define GSO_GCMRK                (0x82)
#define GSO_GCCHST               (0x83)
#define GSO_GCFLT                (0x85)
#define GSO_GCARC                (0x86)
#define GSO_GCFARC               (0x87)
#define GSO_GCBIMG               (0x91)
#define GSO_GIMD                 (0x92)
#define GSO_GEIMG                (0x93)
#define GSO_GSPRP                (0xa0)
#define GSO_GCRLINE              (0xa1)
#define GSO_GCPARC               (0xa3)
#define GSO_GCSFLT               (0xa4)
#define GSO_GCBEZ                (0xa5)
#define GSO_GSICOL               (0xa6)
#define GSO_GBICOL               (0xa7)
#define GSO_GCCHSTM              (0xb1)
#define GSO_GSCPTH               (0xb4)
#define GSO_GBOX                 (0xc0)
#define GSO_GLINE                (0xc1)
#define GSO_GMRK                 (0xc2)
#define GSO_GCHST                (0xc3)
#define GSO_GFLT                 (0xc5)
#define GSO_GARC                 (0xc6)
#define GSO_GFARC                (0xc7)
#define GSO_GBPTH                (0xd0)
#define GSO_GBIMG                (0xd1)
#define GSO_GBEL                 (0xd2)
#define GSO_GLBL                 (0xd3)
#define GSO_GBBLT                (0xd6)
#define GSO_GFPTH                (0xd7)
#define GSO_GMPTH                (0xd8)
#define GSO_GPSPRP               (0xe0)
#define GSO_GRLINE               (0xe1)
#define GSO_GPARC                (0xe3)
#define GSO_GSFLT                (0xe4)
#define GSO_GBEZ                 (0xe5)
#define GSO_GPSICOL              (0xe6)
#define GSO_GPSBICOL              (0xe7)
#define GSO_GCHSTM               (0xf1)
#define GSO_EXT                  (0xfe)
#define GSO_GESD                 (0xff)

#define GSO_TWO_BYTE_MASK	(0x08)

#define EBCDIC_NULL		(0x00)

#endif
