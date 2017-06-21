/* $Id: mci_err.h,v 3.1 1995/11/09 09:06:18 shaker Exp $
 * $Source: /swtal/cherf.111/ios/boot/diag/h/mci_err.h,v $
 *------------------------------------------------------------------
 *  mci_err.h -- MCI Diagnostic Error stuff
 *
 * 8-December-1989, Steve Grant (Access Associates)
 *
 * Copyright (c) 1989-1992 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: mci_err.h,v $
 * Revision 3.1  1995/11/09  09:06:18  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  19:13:24  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 * Revision 1.1  1992/03/07 23:56:47  hampton
 * Initial conversion from RCS to CVS.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#define MAX_TABLES		6
#define MAX_ERRORS		32
#define MAXCTRLLERS		16
#define RESULT_HDR		"\nTest Results (P = pass, F = fail, X = test not done, - = no interface) \
						 \nMCI BUS MEMA MEMD XMEM 0 1 2 3"
#define DCI_RESULT_HDR	"\nTest Results (P = pass, F = fail, X = test not done, - = no interface) \
						 \nDCI MEMA MEMD 0 1 2 3 4 5"

/*
 *  these are used in the MCI_ERROR structure status field
 */

#define STAT_NOT_DONE	'X'
#define STAT_PASS		'P'
#define STAT_FAIL		'F'
#define STAT_NO_PORT	'-'

/*
 *  these are used in the MCI_ERROR structure flag field
 */

#define AF_FLAG			'A'
#define PING_FLAG		'P'
#define NO_FLAG			'N'

/*
 *  Structure for storing errors encountered during diagnostic
 */

typedef struct mcierror_ {
	char   stat;					/*  test status  */
	ushort bad_bits;				/* 	which bits are failing  */
	ushort data_is[MAX_ERRORS];		/* 	actual data read  */
	ushort data_sb[MAX_ERRORS];		/* 	expected data  */
	ushort data_addr[MAX_ERRORS];	/* 	address of the error  */
	int    count; 					/* 	# of errors(array subscript)  */
	char	flag;					/*  flag defines how the following are used  */
	int    data1[MAX_ERRORS];
	int    data2[MAX_ERRORS];
	ushort af_sent;
	ushort af_enabled;
} MCI_ERROR;

/*
 *  Structure for results of each diagnostic test 
 */

typedef struct mci_test_ {
	MCI_ERROR bus_test;
	MCI_ERROR mema_test;
	MCI_ERROR memd_test;
	MCI_ERROR xmem_test;
	MCI_ERROR ether_ping_test[MAX_TABLES];
	MCI_ERROR serial_ping_test[MAX_TABLES];
	MCI_ERROR add_flt_mem[MAX_TABLES];
	MCI_ERROR add_flt[MAX_TABLES];
} MCI_TEST;
									   
/* end of mci_err.h */


