/* $Id: igrp_common.h,v 3.1.32.2 1996/07/31 17:28:15 gcox Exp $
 * $Source: /release/112/cvs/Xsys/iprouting/igrp_common.h,v $
 *------------------------------------------------------------------
 * Definition of IGRP packet common symbols
 *
 * February 1996, Dave Katz
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: igrp_common.h,v $
 * Revision 3.1.32.2  1996/07/31  17:28:15  gcox
 * CSCdi64717:  Remove doubly-defined macros - SCALED_BANDWIDTH etc
 * Branch: California_branch
 *
 * Revision 3.1.32.1  1996/04/02  05:37:09  dkatz
 * CSCdi36031:  Running out of memory running 2 or more EIGRP ASs
 * Branch: California_branch
 * EIGRP Jumbo Patch.
 *
 * Revision 3.2  1996/03/19  03:04:08  dkatz
 * CSCdi36031:  Running out of memory running 2 or more EIGRP ASs
 * EIGRP Jumbo Patch.
 *
 * Revision 3.1  1996/02/12  03:49:03  dkatz
 * Add placeholders.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef _IGRP_COMMON_H_
#define _IGRP_COMMON_H_

/*
 * IGRP packet opcode types.
 */
#define IGRP_OPC_UPDATE  1
#define IGRP_OPC_REQUEST 2

/*
 * Default variance
 */
#define IGRP_DEFVARIANCE 1			/* default variance */


/*
 * Scaling/weighting parameters for scalar metric computation
 */
#define IGRP_K1_DEFAULT 1		/* unweighed inverse bandwidth */
#define IGRP_K2_DEFAULT 0		/* no loading term */
#define IGRP_K3_DEFAULT 1		/* unweighted delay */
#define IGRP_K4_DEFAULT 0		/* no reliability term */
#define IGRP_K5_DEFAULT	0		/* no reliability term */

/*
 * Default maximum number of hops
 */
#define IGRP_MAXHOPS 100

#define IGRP2_BANDWIDTH_SCALE ((uint)BANDWIDTH_SCALE * (1 << 8)) /* igrp2 */

#endif					/* _IGRP_COMMON_H_ */
