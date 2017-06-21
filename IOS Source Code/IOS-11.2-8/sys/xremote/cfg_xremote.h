/* $Id: cfg_xremote.h,v 3.2 1995/11/17 19:26:26 gstovall Exp $
 * $Source: /swtal/cherf.111/ios/sys/xremote/cfg_xremote.h,v $
 *------------------------------------------------------------------
 * C F G _ X R E M O T E . H
 *
 * Copyright (c) 1992-1995 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_xremote.h,v $
 * Revision 3.2  1995/11/17  19:26:26  gstovall
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  14:01:26  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  01:44:42  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize xns, atalk, vines, decnet, xremote, x25, unix, wan, tn3270,
 * clns, cdp
 *
 * Revision 2.1  1995/06/07  22:14:58  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/***************************************************************
 * xremote tftp retries <num>
 * no xremote tftp retries
 */
PARAMS_KEYONLY(xremote_tftp_retries, no_alt, "retries",
	       OBJ(int,1), 1, 15,
	       xremote_command, XREMOTE_TFTP_RETRIES,
	       "Set number of retries to attempt loading fonts",
	       "Number of retries to attempt loading fonts", PRIV_CONF);

/***************************************************************
 * [no] xremote tftp host <ipaddr>
 *
 */
EOLS    (xremote_tftp_host_eol, xremote_command, XREMOTE_TFTP_HOST);
IPADDR_NAME(xremote_tftp_host_name, xremote_tftp_host_eol, no_alt,
	    OBJ(paddr,1), "Hostname or IP address of font server");
NVGENS	(xremote_tftp_host_nvgen, xremote_tftp_host_name,
	 xremote_command, XREMOTE_TFTP_HOST);
KEYWORD	(xremote_tftp_host, xremote_tftp_host_nvgen, xremote_tftp_retries,
	 "host", "Add a TFTP font server", PRIV_CONF);

/***************************************************************
 * xremote tftp buffersize <num>
 * no xremote  tftp buffersize
 */
PARAMS_KEYONLY(xremote_tftp_buf, xremote_tftp_host, "buffersize",
	       OBJ(int,1), 4096, -1,
	       xremote_command, XREMOTE_TFTP_BUFFERSIZE,
	       "Set the buffer size used for loading font files",
	       "Buffer size used for loading font files in bytes",
	       PRIV_CONF);

/***************************************************************/
KEYWORD (xremote_tftp, xremote_tftp_buf, no_alt,
         "tftp", "Set font loader host identification", PRIV_CONF);

/***************************************************************
 * xremote buffersize <num>
 * no xremote buffersize
 */
PARAMS_KEYONLY(xremote_buf, xremote_tftp, "buffersize",
	       OBJ(int,1), 4096, -1,
	       xremote_command, XREMOTE_BUFFERSIZE_CMD,
	       "Set the buffer size used for loading font files",
	       "Buffer size used for loading font files in bytes", PRIV_CONF);

/***************************************************************/
KEYWORD	(xremote, xremote_buf, NONE, "xremote",
	 "Configure XRemote", PRIV_CONF);

TEST_BOOLEAN(xremote_test, xremote, NONE, ALTERNATE, xremote_enable);

#undef	ALTERNATE
#define	ALTERNATE	xremote_test
