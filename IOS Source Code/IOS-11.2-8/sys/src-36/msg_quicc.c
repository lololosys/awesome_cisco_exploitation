/* $Id: msg_quicc.c,v 3.3.62.3 1996/09/14 00:01:45 hampton Exp $
 * $Source: /release/112/cvs/Xsys/src-36/msg_quicc.c,v $
 *------------------------------------------------------------------
 * msg_quicc.c - Message file for quicc facility
 *
 * August 1995, Satish Kumar Gannu
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: msg_quicc.c,v $
 * Revision 3.3.62.3  1996/09/14  00:01:45  hampton
 * Incorporate the Error Messages Manual text into the source code.
 * [CSCdi69164]
 * Branch: California_branch
 *
 * Revision 3.3.62.2  1996/08/28  13:10:27  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.3.62.1  1996/03/18  21:56:35  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.26.1  1996/01/24  22:49:11  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.3  1995/11/17  18:39:14  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:29:18  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:19:08  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:26:21  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/08/09  19:32:46  sgannu
 * CSCdi38309:  catwoman msg causes modular build failure
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#define DEFINE_MESSAGES	TRUE
#include "logger.h"
#include "../os/msgtxt_sys.h"


facdef(QUICC);
msgdef_section("MC68360 Quad Integrated Communications Controller error
	messages");


msgdef(NOMEMORY, QUICC, LOG_ALERT, 0, msgtxt_nomemory);
msgdef_explanation(
	"The MC68360/QUICC CPU was unable to access the memory it needs to
	carry out its functions. Here are some possible causes.

	The network is large, requiring a lot of memory for routing tables, and
	so on.

	The router configuration has many features enabled, each of which
	require a certain amount of memory.

	A software error (memory leak) exists.");
msgdef_recommended_action(
	"Reduce system activity to ease the memory demand, or upgrade to a
	larger memory configuration.");
msgdef_ddts_component("");


msgdef(OWNERR, QUICC, LOG_ERR, MSG_TRACEBACK | MSG_PROCESS, 
       msgtxt_ownerror);
msgdef_explanation(
	"An internal software error occurred.");
msgdef_recommended_action(
	"Call your technical support representative to obtain a software
	upgrade.");
msgdef_ddts_component("");
