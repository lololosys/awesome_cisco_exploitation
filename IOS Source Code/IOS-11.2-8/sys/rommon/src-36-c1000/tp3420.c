/* $Id: tp3420.c,v 3.2.58.1 1996/03/21 23:29:30 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/rommon/src-36-c1000/tp3420.c,v $
 *------------------------------------------------------------------
 * tp3420.c
 *
 * January 1995, Rob Clevenger
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Test for the ISDN iface chip tp3420 for Sapphire
 *------------------------------------------------------------------
 * $Log: tp3420.c,v $
 * Revision 3.2.58.1  1996/03/21  23:29:30  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.2.2.1  1995/12/01  04:22:31  mbeesley
 * Branch: ELC_branch
 * Reorganize Rom monitor header files to make it easier to port
 * to new platforms.
 *
 * Revision 3.2  1995/11/17  18:44:40  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:07:44  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:39:43  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "monitor.h"
#include "quicc.h"
#include "tp3420.h"

int
tp3420_test(void)
{
	int rc;

	spi_open();
	rc = spi_cmd(TP3420_CRF_ENST);	/* Enable device state on the NOCST */

	rc = spi_cmd(TP3420_CRF_PDN);	/* Power-Down */
	rc = spi_cmd(TP3420_CRF_NOP);	/* get internal state */
	if (rc != 0x80) {
		spi_close();
		return(-1);
	}

	rc = spi_cmd(TP3420_CRF_PUP);	/* Power-Up */
	rc = spi_cmd(TP3420_CRF_NOP);	/* get internal state */
	if (rc != 0x90) {
		spi_close();
		return(-1);
	}

	spi_close();
	return(0);
}

/* end of module */
