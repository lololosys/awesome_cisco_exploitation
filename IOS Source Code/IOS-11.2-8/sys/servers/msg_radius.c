/* $Id$
 * $Source$
 *------------------------------------------------------------------
 * msg_radius.c - Radius SYSLOG message definitions
 *
 * June 1997, Che-lin Ho
 *
 * Copyright (c) 1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log$
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#define DEFINE_MESSAGES TRUE
#include "logger.h"


facdef(RADIUS);
msgdef_section("RADIUS error messages");

msgdef(ALLDEADSERVER, RADIUS, LOG_ERR, 0, "No active radius servers found.");
msgdef_explanation("All radius servers were found to be unresponsive.");
msgdef_recommended_action("Check the network connectivity to the radius servers, and check that the servers are running");
msgdef_ddts_component("");

msgdef(SERVERALIVE, RADIUS, LOG_INFO, 0, "Radius server %i is responding again (previously dead).");
msgdef_explanation("A radius server, which previously was dead, has started responding again.");
msgdef_recommended_action("No action required.");
msgdef_ddts_component("");
