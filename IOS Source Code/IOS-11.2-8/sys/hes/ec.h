/* $Id: ec.h,v 3.3 1995/11/17 09:15:38 hampton Exp $
 * $Source: /vws/aam/cherf.111/ios/sys/hes/ec.h,v $
 *------------------------------------------------------------------
 * ec.h - definitions specific to the 68K side.
 *
 * Copyright (c) 1990-1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ec.h,v $
 * Revision 3.3  1995/11/17  09:15:38  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:26:06  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:38:12  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/08/21  07:39:55  hampton
 * Transition the hes directory code to use the passive timer macros for
 * all its timers.  It no longer references the system clock directly.
 * [CSCdi39033]
 *
 * Revision 2.1  1995/06/07 20:39:05  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Hardware regs for the EC
 */
#define EC_IO	  (MULTIIOBASE+0x00)

#define ECM_NOTPRESENT		0
#define ECM_PRES_NODATA		1
#define ECM_PRES_AND_DATA	2
#define ECM_7000		3

#define ECM_OK    0
#define ECM_QUIT  1
#define ECM_ERROR -1

/* ENVM REVISION Macros and flags                                 */
#define EC_SWREV(mod)    ((mod) & 0xF)
#define EC_HWREV(mod)    (((mod)>>4) & 0xF)
#define EC_SWREV_1_1     1 /* Obsolete (unsupported) I/F to CSC   */
#define EC_SWREV_1_2     2 /* First working CSC/EC interface      */
#define EC_HWREV_2       2 /* Can't use MB SRAM (couldn't probe flash eprom)*/
#define EC_HWREV_4       4 /* Uses MB SRAM (prefered for flash transfer     */


/* Move this to cpu_68k.h ? */
#define LEVEL_EC 6 /* This is a pretty hi pri event */

#define EC_MBSIZE      256 /* Size of Mailbox buffer               */

/* ECMON Test Points (referenced by SHOW struct)                   */
#define ECTEST_T_AMB     0 /* Temperature Ambient (inlet)          */
#define ECTEST_T_AIR     1 /* Air flow sensor temperature          */
#define ECTEST_RES_0     2 /* Reserved                             */
#define ECTEST_RES_1     3 /* Reserved                             */
#define ECTEST_V_P5      4 /* +5 Volts                             */
#define ECTEST_V_P12     5 /* +12 Volts                            */
#define ECTEST_V_N12     6 /* -12 Volts                            */
#define ECTEST_V_N5      7 /* -5 Volts                             */
#define ECTESTPTS        8 /* Current Max testpoints               */
#define ECVALIDTESTS     6 /* only 6 testpoints are measured       */
#define ECSHOWBUFLEN   512 /* String buffer for what ENV sends     */
#define ECSTATBUFLEN    64 /* Keep some of stats for debug         */

#define TESTPOINTLEN	12
#define WARNLEN		2
#define SHLEN 
typedef struct {
	char     t_description[TESTPOINTLEN];/* What's measured, "+5 Volts" */
	char     t_measured[TESTPOINTLEN];   /* Measured value, "5.013(V)"  */
	char     t_shut_marg[TESTPOINTLEN];  /* Shutdown margin, e.g. "10%" */
	char     t_shut_last[TESTPOINTLEN];  /* Last shutdown measurement   */
	char     t_warning[2];      /* flag it's a warning condition
				       */
} ectest_t;

/* ECSHOW Structure used for "show ecmon" and SNMP                          */
struct ecshow_t_ {
	char     s_revhw[4];       /* "rev2" vs. "rev4"                     */
	char     s_revsw[4];       /* "9.0"                                 */
	char     s_revfw[4];       /* "1.7"                                 */
	char     s_tech_id[4];     /* "jj" technician id                    */
	char     s_devnam[12];     /* "ecmon" vs. "clean_envm"              */
	char     s_burn_date[12];  /* "8-22-91" calibration/burn-in date    */
	char     s_serial_num[12]; /* "1234123412" serial number            */
	ectest_t s_test[ECTESTPTS];       /* There's 6 testpoints for ENVM             */
};

/* Header for events over the EC/CSC interface                    */
typedef struct {
	uchar   h_ctl;         /* Type of control info                */
	uchar   h_mod;         /* length or modifier                  */
} echdr_t;

typedef struct {
	ulong   intr;
	ulong   input_intr;
	ulong   output_intr;
	ushort  unknown_intr;
 	uchar   unknown_ctl;
 	uchar   unknown_mod;
 	ushort  bcmd_intr;
 	ushort  not_responding;
 	ulong   reads;
 	ulong   read_errors;
 	ulong   read_bytes;
 	ulong   read_ms;
 	ulong   stat_calls;
 	ulong   stat_ok;
 	ulong   stat_bytes;
 	ushort  parse_errors;
} eccnt_t;

/* EC struct */
typedef struct {
    ulong    ec_state;        /* Current state of Environmental Ctrlr */
    char     ec_hwrev;        /* Hardware Revision of ENVM Controller */
    uchar    ec_swrev;        /* Software (firmware) rev on ENVM prom */
    volatile echdr_t *ec_iop; /* I/O Port pointer                     */
    volatile uchar   *ec_mbp; /* mailbox addr in Multibus sram        */
    echdr_t  ec_ihdr;         /* Last input ctl header from EC        */
    echdr_t  ec_indhdr;       /* Indicator (output) ctl/dat           */
    echdr_t  ec_reqhdr;       /* Requested (input) ctl/dat            */
    ulong    ec_len;          /* Write/Read max length                */
    ulong    ec_cur;          /* Current offset in read/write buf     */
    sys_timestamp ec_delay;   /* Used for sleeping timeout            */
    uchar    *ec_buf;         /* Read/write buffer                    */
    boolean  ec_rev4;         /* May we use MBus SRAM at all?         */
    boolean  ec_mbsram;       /* Use MBus SRAM for *next* read/write ?*/
    boolean  ec_inited;       /* Finished initializing                */
    ecshow_t *ec_show;        /* Info Strings for SHOW and SNMP       */
    eccnt_t  ec_cnt;          /* Various counters                     */
    sys_timestamp ec_warndelay[ECTESTPTS];
    char     ec_statbuf[ECSTATBUFLEN]; /* For debug                   */
} ec_t;

/* Valid states for EC */
#define EC_STATE_NODEV   0 /* EC device not found                  */
#define EC_STATE_GETREV  0 /* Waiting for revision info            */
#define EC_STATE_MBINIT  1 /* Waiting Multibus addr initialization */
#define EC_STATE_IDLE    2 /* EC running, nothing pending          */
#define EC_STATE_OUTPUT  3 /* CSC is currently outputting to EC    */
#define EC_STATE_INPUT   4 /* CSC is currently outputting to EC    */
#define EC_STATE_CMD     5 /* Simple command requiring only an ack */
#define EC_STATE_FLASH   6 /* Issuing flash command                */
#define EC_STATE_ERROR   7 /* Something went wacky                 */
#define EC_STATE_MAX     EC_STATE_ERROR


/* IOCTL Commands to EC from system code and processes                      */

/*
 * ENV Control Types - the ENV recognizes these values when written
 * to the control byte of the ECIOP (byte-0).
 *
 * IND suffixes indicate we are sending a asynchronous data in the
 * modifier byte of the ECIOP (byte-1), or that the ENV is sending us
 * asynchronous data.  The ENV should always respond with the
 * corresponding ACK and the data echoed in the modifier byte, while
 * we need only respond with a clear interrupt request.
 *
 * REQ suffixes are for requesting that data be sent from the ENV
 * to us, or that some command be invoked.  The ENV should respond
 * with the corresponding ACK and data in the ECIOP modifier byte,
 * or with a FINISH acknowledge if no data is available.
 *
 * ACK suffixes acknowledge that command or data was received
 * by the ENV, and the modifier byte verifies the data or results
 * of the command.
 */
#if 0
#define ECCTL_CLRINT      0 /* Sent by CSC to clear current interrupt     */
#define ECCTL_REV         1 /* Request for EC Revision number             */
#define ECCTL_MBADDR      2 /* Send Multibus shared mem addr to ENVM      */
#define ECCTL_WARN        3 /* ENV sends temp/voltage warning             */
#define ECCTL_SHUT        4 /* ENV sends temp/voltage shutdown warning    */
#define ECCTL_RESET       5 /* Reset ENVM                                 */
#define ECCTL_LOOP_IND    6 /* Read back loopback buffer                  */
#define ECCTL_LOOP_REQ    7 /* Output loopback buffer                     */
#define ECCTL_MBLOOP_IND  8 /* loopback write through multibus shared sram*/
#define ECCTL_MBLOOP_REQ  9 /* loopback read through multibus shared sram */
#define ECCTL_MON_IND    10 /* CSC/ENV send ascii byte to monitor shell   */
#define ECCTL_MON_REQ    11 /* CSC requests ascii data from ECMON shell   */
#define ECCTL_BCMD       12 /* Bad command sent to ENVM                   */
#define ECCTL_UNDEF1     13 /* Bad command sent to ENVM                   */
#define ECCTL_UNDEF2     14 /* Bad command sent to ENVM                   */
#define ECCTL_FINISH     15 /* Read/Write ended unexpectedly              */
#define ECCTL_FLASH_VPP  16 /* Turn ON (dat=1), OFF (dat=0) VPP for flash */
#define ECCTL_FLASH_RESET 17 /* Issue 0xFF command to flash device         */
#define ECCTL_FLASH_INFO  18 /* Issue INFO command to flash device         */
#define ECCTL_FLASH_READ  19 /* Issue READ command to flash device         */
#define ECCTL_FLASH_SEEK  20 /* CSC sends 1 of 3 seek offset bytes to ENV  */
#define ECCTL_FLASH_ERASE 21 /* Erase flash rom, addr in modifier byte     */
#define ECCTL_FLASH_ZERO  22 /* Zero out flash                             */
#define ECCTL_FLASH_IND   23 /* CSC wants to write to flash, data in modifier*/
#define ECCTL_FLASH_REQ   24 /* CSC wants to read next byte from flash     */
#define ECCTL_MAX         25
#else
#define ECCTL_CLRINT      0 /* Sent by CSC to clear current interrupt     */
#define ECCTL_REV         1 /* Request for EC Revision number             */
#define ECCTL_MBADDR      2 /* Send Multibus shared mem addr to ENVM      */
#define ECCTL_CHKSUM      3 /* CSC wants ENV to return, then clear cksum  */
#define ECCTL_FLINFO      4 /* CSC req flash info, address in modifier    */
#define ECCTL_FLASH_SEEK  5 /* CSC sends 1 of 3 seek offset bytes to ENV  */
#define ECCTL_UNDEF1      6 /* CSC wants to know current seek offset      */
#define ECCTL_FLASH_ERASE 7 /* Erase flash rom, addr in modifier byte     */
#define ECCTL_FLASH_IND   8 /* CSC wants to write to flash, data in modifier*/
#define ECCTL_FLASH_REQ   9 /* CSC wants to read next byte from flash     */
#define ECCTL_MON_IND    10 /* CSC/ENV send ascii byte to monitor shell   */
#define ECCTL_MON_REQ    11 /* CSC requests ascii data from ECMON shell   */
#define ECCTL_WARN       12 /* ENV sends temp/voltage warning             */
#define ECCTL_SHUT       13 /* ENV sends temp/voltage shutdown warning    */
#define ECCTL_BCMD       14 /* Bad command sent to ENVM                   */
#define ECCTL_FINISH     15 /* Read/Write ended unexpectedly              */
#define ECCTL_LOOP_IND   16 /* Read back loopback buffer                  */
#define ECCTL_LOOP_REQ   17 /* Output loopback buffer                     */
#define ECCTL_MBLOOP_IND 18 /* loopback write through multibus shared sram*/
#define ECCTL_MBLOOP_REQ 19 /* loopback read through multibus shared sram */
#define ECCTL_FLASH_VPP  20 /* Turn ON (dat=1), OFF (dat=0) VPP for flash */
#define ECCTL_FLASH_ZERO 21 /* Zero out flash                             */
#define ECCTL_FLASH_RESET 22 /* Issue 0xFF command to flash device         */
#define ECCTL_FLASH_INFO  23 /* Issue INFO command to flash device         */
#define ECCTL_FLASH_READ  24 /* Issue READ command to flash device         */
#define ECCTL_MAX        25

#endif
/*
 * These error codes are sent by ENVM to CSC along with a code
 * indicating whether it is a WARNING message, or a SHUTDOWN message.
 *
 * (also indices into ec_errmsg[] table)
 */
#define EC_OK             0
#define ENVMERR_V_5       1  /*  +5 Volts: warn >5%, shut >10%           */
#define ENVMERR_V_M5      2  /*  -5 Volts: warn >5%, shut >10%           */
#define ENVMERR_V_12      3  /* +12 Volts: warn >10%, shut >15%          */
#define ENVMERR_V_M12     4  /* -12 Volts: warn >10%, shut >15%          */
#define ENVMERR_TEMP      5  /* inlet ambiant temperature too high       */
#define ENVMERR_FAN       6  /* temp at outlet too high (not enough air) */
#define ENVMERR_BAD_CTL   7  /* EC sent bad REQ/IND to ENVM              */
#define ENVMERR_BATTERY   8  /* NVRAM battery failure                    */
#define ENVMERR_CALB      9  /* Calibration failed- shutdown             */
#define ENVMERR_SHUT     10  /* Couldn't shutdown system...              */
#define ENVMERR_HEATER   11  /* heater malfunction (won't send this...)  */
#define ENVMERR_UNKNOWN  12
#define ENVMERRMAX       ENVMERR_UNKNOWN

/* Errors known only to CSC's EC driver */
#define ECERRMIN         ENVMERRMAX+ 1 /* Lowest valid error from driver    */
#define ECERR_ENVM       ENVMERRMAX+ 1 /* ENVM returned error status        */
#define ECERR_BUSY       ENVMERRMAX+ 2 /* EC currently executing other cmd  */
#define ECERR_SWREV      ENVMERRMAX+ 3 /* Unknown rev of ENVM firmware      */
#define ECERR_HWREV      ENVMERRMAX+ 4 /* Unknown hardware rev of ENVM ctlr */
#define ECERR_UNEXP_INTR ENVMERRMAX+ 5 /* Intr with unexpected ctl/mod      */
#define ECERR_BCMD       ENVMERRMAX+ 6 /* EC sent bad command to ENV        */
#define ECERR_WRLEN_MIN  ENVMERRMAX+ 7 /* write len too small               */
#define ECERR_WRACK_CTL  ENVMERRMAX+ 8 /* bad ctl from envm on wr ack       */
#define ECERR_WRACK_MOD  ENVMERRMAX+ 9 /* bad mod from envm on wr ack       */
#define ECERR_RDLEN_MIN  ENVMERRMAX+10 /* read length too small             */
#define ECERR_RDACK      ENVMERRMAX+11 /* bad ctl from envm on rd ack wait  */
#define ECERR_RDACK_TIME ENVMERRMAX+12 /* timeout on read ack wait          */
#define ECERR_CHKSUM     ENVMERRMAX+13 /* bad checksum                      */
#define ECERR_CHKSUM_TIME ENVMERRMAX+14/* timeout on bad checksum           */
#define ECERR_CLRINT     ENVMERRMAX+15 /* couldn't clear int                */
#define ECERR_FLSEEK_TIME ENVMERRMAX+16/* flash seek timeout                */
#define ECERR_FLER_TIME  ENVMERRMAX+17 /* flash erase timeout               */
#define ECERR_UNKNOWN    ENVMERRMAX+18 /* EC driver gave bad err to logger  */
#define ECERRMAX         ECERR_UNKNOWN /* Largest valid driver error        */

/*
 * exported data
 */
extern boolean ec_present;
extern uint ec_flash_cmdfails;
extern ec_t *ecp;

/*
 * exported code
 */
int getenvtemp(char *s);
int getenvvoltage(char *s);
int getenvnumber(char *s, int i);
