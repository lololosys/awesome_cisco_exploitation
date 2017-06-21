/* $Id$
 * $Source$
 *------------------------------------------------------------------
 * msg_csm.c -- Message file for Call Switching Module (CSM).
 *
 * September, 1996 - Jonathan Chiung-Hsi Lin
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log$
 *------------------------------------------------------------------
 * $Endlog$
 */
#include "master.h"
#define DEFINE_MESSAGES TRUE
#include "logger.h"

facdef(CSM);

msgdef(INITSYS,  CSM, LOG_ALERT,  0, "%s");
msgdef(INITPORT, CSM, LOG_ALERT,  0, "%s at slot %d, port %d");
msgdef(PRI, CSM, LOG_NOTICE, 0, "%s at slot %d, unit %d, channel %d "
                                "with index %d");
msgdef(RBS, CSM, LOG_NOTICE, 0, "%s at slot %d, controller %d, timeslot %d");
msgdef(ERROR, CSM, LOG_NOTICE, 0, "%s");
msgdef(MODEM_INFO, CSM, LOG_ALERT, 0, "%s: modem_info is NULL");
msgdef(HWIDB, CSM, LOG_ALERT, 0, "%s: hwidb is NULL");
