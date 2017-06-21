/* $Id: tftp.h,v 3.3.54.2 1996/09/06 21:00:59 snyder Exp $
 * $Source: /release/112/cvs/Xsys/ip/tftp.h,v $
 *------------------------------------------------------------------
 * tftp.h - TFTP server definitions
 *
 * 27-April-1900, Kevin Paul Herbert
 *
 * Copyright (c) 1990-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: tftp.h,v $
 * Revision 3.3.54.2  1996/09/06  21:00:59  snyder
 * CSCdi68334:  dump unused spinner support, save memory
 *              412 image, 4 data
 * Branch: California_branch
 *
 * Revision 3.3.54.1  1996/04/15  21:16:56  bcole
 * CSCdi54649:  IP Host Modularity, IP Automatic Stub Routing feature
 * commit
 * Branch: California_branch
 *
 * Revision 3.3  1995/11/21  07:41:24  jenny
 * CSCdi43195:  TFTP: cannot specify source address
 *
 * Revision 3.2  1995/11/17  09:36:46  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:58:41  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/07/16  18:04:14  thille
 * CSCdi37049:  Console output from SNMP WriteNet SET confuses console user
 * Bad programmer, no doggie biscuit.
 *
 * Revision 2.2  1995/07/15  02:36:58  thille
 * CSCdi37049:  Console output from SNMP WriteNet SET confuses console user
 * Add a silent_mode parameter to some routines and registries and set it
 * to TRUE when making the calls from SNMP WriteNet, netConfigSet, or
 * hostConfigSet.  Also, add an errmsg for each so the event can still get
 * logged, and make it clear what caused the event.  Fix other callers to
 * pass FALSE so current behavior will be maintained.
 *
 * Revision 2.1  1995/06/07  21:01:23  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __TFTP_H__
#define __TFTP_H__

#define TFTP_MIN_TIMEOUT 4*ONESEC
#define TFTP_MAX_TIMEOUT 8*ONESEC
#define TFTP_BACKOFF_DELAY ONESEC
#define TFTP_WRITE_RETRIES  5

#define TFTP_OPEN_SLAVECORE_SOCKET 0x01
#define TFTP_CLOSE_SLAVECORE_SOCKET 0x02
#define TFTP_WRITE_SLAVECORE_SOCKET 0x04

/*
 * TFTP opcodes
 */
#define RRQ	1			/* read request */
#define WRQ	2			/* write request */
#define DATA	3			/* data */
#define ACK	4			/* acknowledgement of data */
#define ERROR	5			/* error message */

typedef struct tftpdatatype_ {
    struct tftpdatatype_ *next;		/* pointer to next element */
    char *filename;			/* filename server recognizes */
    int filelocation;			/* file location */
    char *alias;			/* alias if a flash file */
    int acl;				/* server access control list */
} tftpdatatype;


/***********************************************************************
 *
 *                      Externs and Prototypes
 *
 ***********************************************************************/

/*
 * locals and passed vars for new tftp read.  (some of these can be removed.)
 */
extern tftpdatatype *tftpdata;

/*
 * tftp.c
 */ 
extern void tftp_RRQ(ipsocktype *soc, const char *filename, const char *mode);
extern void tftp_ACK(ipsocktype *soc, short blocknumber);
extern void tftp_ERROR(ipsocktype *soc, short code, char *errmsg);
extern void tftp_sendERROR(paktype *pak, uint lport, uint fport, int code,
			   char *errmsg); 
extern int tftp_getack(ipsocktype *soc, int expect, boolean server,
		       int retry_num);
extern boolean tftp_WRQ(ipsocktype *, const char *, const char *, boolean);
extern boolean tftp_sendblock(ipsocktype *soc, int blknum, uchar *buffer,
			      int len, boolean server);
extern boolean tftp_ip_router_active(void);
extern boolean tftp_write_socket(ipsocktype *, const char *, uchar *,
				 const char *, uint);
extern char *tftp_opcode_string (int opcode);
extern addrtype *tftp_get_laddr(addrtype *);

/*
 * tftp_util.c
 */
extern void ip_put_config_file(parseinfo *);
extern void ip_get_config_file(parseinfo *, boolean);
extern boolean tftp_noninteractively_get_config_file(char *, addrtype *,boolean);

/*
 * tftp_server.c
 */
ipsocktype *ip_tftp_server_init(addrtype *, addrtype *, ushort, ushort);

/*
 * tftp_chain.c
 */
extern void tftp_parser_init(void);
#endif __TFTP_H__
