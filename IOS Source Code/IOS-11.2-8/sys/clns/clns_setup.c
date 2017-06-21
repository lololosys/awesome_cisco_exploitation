/* $Id: clns_setup.c,v 3.3.54.3 1996/08/08 14:50:34 dkatz Exp $
 * $Source: /release/112/cvs/Xsys/clns/clns_setup.c,v $
 *------------------------------------------------------------------
 * CLNS setup commands
 *
 * May 1993, Robert Widmer
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: clns_setup.c,v $
 * Revision 3.3.54.3  1996/08/08  14:50:34  dkatz
 * CSCdi65076:  ISIS/NLSP uses old scheduler primitives
 * Branch: California_branch
 * Use new scheduler primitives.  Get rid of gratuitous type coercion.
 *
 * Revision 3.3.54.2  1996/08/07  08:57:14  snyder
 * CSCdi65343:  more things that should be declared const
 *              char * [] -> char *const []
 *              char *    -> const char []
 *              savings 17,496 data space, 1,852 image space
 * Branch: California_branch
 *
 * Revision 3.3.54.1  1996/03/18  19:03:12  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.22.2  1996/03/07  08:40:21  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.3.22.1  1996/02/20  13:40:24  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.3  1995/11/28  03:58:24  widmer
 * CSCdi44175:  Add setup_printf to replace sprintf in setup code
 *
 * Revision 3.2  1995/11/17  08:52:43  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:08:09  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:17:50  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <ctype.h>
#undef   toupper			/* use library function call */
#include <string.h>
#include <ciscolib.h>
#include "interface_private.h"
#include "sys_registry.h"
#include "ttysrv.h"
#include "../ui/setup.h"
#include "../if/network.h"
#include "../clns/clns.h"
#include "../clns/clns_route.h"
#include "../clns/clns_externs.h"

typedef struct clns_setup_struct_ {
    boolean run_clns;
    clns_pdbtype *clns_pdb;
    char clns_area_string[MAXBUF];
} clns_setup_struct;

static void get_clns_domain(char *, NSAP_T, NSAP_T);
static void get_clns_area (char *, uchar[2], uchar *);
static void get_clns_station(char *, uchar[6], uchar *);

static const char clns_proto_name[] = "CLNS";

void clns_setup_global (setup_struct *sp)
{
    uchar CLNS_AREA[2] = {0, 1};
    uchar def_clns_area[2];
    char *def_clns_area_string;
    uchar *temp_clns_area, *def_clns_station;
    int length, i;
    uchar station[6];
    uchar area[2];
    NSAP_T nsap, def_clns_domain;
    char buffer[MAXBUF], *cp;
    clns_pdbtype *clns_pdb;
    clns_setup_struct *css;

    sp->args[LINK_CLNS] = FALSE;
    if (system_loading || !router_enable) {
	return;
    }

    css = malloc(sizeof(clns_setup_struct));
    if (!css) {
	return;
    }

    sp->args[LINK_CLNS] = css;
    css->run_clns = system_protocol(sp, clns_proto_name,
				    RUNNING(LINK_CLNS));
    if (css->run_clns) {
	/*
	 * clns router igrp <tag> net <net>
	 */
	setup_printf(sp, "\nrouter iso-igrp");
	clns_pdb = clns_protoQ.qhead;

	/*
	 * Find an ISO IGRP pdb if it exists
	 */
	while (clns_pdb && (!(clns_pdb->typeflag & ISO_IGRPMASK))) {
	    clns_pdb = clns_pdb->next;
	}

	if (clns_pdb) {
	    def_clns_area_string = clns_pdb->pname;
	    def_clns_domain[0] = clns_pdb->domain_length;
	    if (clns_pdb->net[0]) {
	        bcopy(clns_pdb->domain, &def_clns_domain[1], 
		      clns_pdb->domain_length);
                temp_clns_area = get_area(clns_pdb->net);
                def_clns_area[0] = *temp_clns_area++;
                def_clns_area[1] = *temp_clns_area++;
	    } else {
	        /* Use default if no net specified */
                def_clns_domain[0] = 1;
                def_clns_domain[1] = 0x49;
                def_clns_area[0] = CLNS_AREA[0];
                def_clns_area[1] = CLNS_AREA[1];	       
	    }
	    css->clns_pdb = clns_pdb;
        } else {
            def_clns_area_string = "area_1";
            def_clns_domain[0] = 1;
            def_clns_domain[1] = 0x49;
            def_clns_area[0] = CLNS_AREA[0];
            def_clns_area[1] = CLNS_AREA[1];
	    css->clns_pdb = NULL;
        }
        cp = setup_string(buffer,"    CLNS router tag", def_clns_area_string);
        strcpy(css->clns_area_string, cp);
	setup_printf(sp, " %s %s ", css->clns_area_string, "\nnet");
        get_clns_domain("    CLNS domain", def_clns_domain, nsap);
        get_clns_area("    CLNS area", def_clns_area, area);
        length = nsap[0];
        nsap[++length] = area[0];
        nsap[++length] = area[1];

        if (clns_router_running && clns_pdb && clns_myNET[0]) {
            def_clns_station = get_station(clns_myNET);
        } else {
            def_clns_station = default_mac_addr;
	}

        get_clns_station("    CLNS station id", def_clns_station, station);
        for (i = 0; i < 6; i++) {
            nsap[++length] = (uchar)station[i];
	}
        nsap[++length] = 0;
        nsap[0] += 9;
	setup_printf(sp, " %s", nsap_string(nsap));
    } else {
	css->clns_pdb = NULL;
	css->clns_area_string[0] = '\0';
    }
}

void clns_setup_end (setup_struct *sp)
{
    if (sp->args[LINK_CLNS]) {
	free(sp->args[LINK_CLNS]);
	sp->args[LINK_CLNS] = NULL;
    }
}


void clns_setup_int (setup_struct *sp)
{
    clns_setup_struct *css = sp->args[LINK_CLNS];
    boolean active = FALSE;

    if (system_loading || !router_enable || !sp->args[LINK_CLNS])
	return;
    if (css->clns_pdb && itf_in_pdb(sp->idb, css->clns_pdb))
	active = TRUE;

    if (css->run_clns) {
	if (interface_protocol(clns_proto_name, sp->idb->clns_enabled)) {
	    if (!sp->idb->clns_enabled || active)
		setup_printf(sp, "\nclns router iso-igrp %s",
			     css->clns_area_string);
	} else {
	    if (active) {
		setup_printf(sp, "\nno clns router iso-igrp %s",
			     css->clns_area_string);
	    } else if (sp->idb->clns_enabled) {
		setup_printf(sp, "\nno clns enable");
		if (sp->idb->clns_net)
		    setup_printf(sp, "\nno clns net %n",
				 sp->idb->clns_net);
	    }
	}
    }
}


/*
 * get_clns_domain
 * Get the clns net specifier.
 * In the goal of simplicity, the user will be prompted for a
 * clns domain and area. He will be allowed to default the domain
 * to '49' which means 'local'. He MUST specify an area.  We will
 * take the station id from the first ethernet address we can find.
 */

static void get_clns_domain (char *prompt, NSAP_T def, NSAP_T nsap)
{
    boolean status;
    char *c,buffer[MAXBUF];

    while (!(stdio->statbits & CONTROLC)) {
        printf("%s [%s]: ", prompt, nsap_string(def));
        status = rdtty(buffer, MAXBUF);
        if (!status) {
            nsap_copy(def, nsap);
            break;
        }
        c = deblank(buffer);
        if (null(c)) {
            nsap_copy(def, nsap);
            break;
        }
        if (!parse_nsap(c, nsap)) {
            printf("\n%% Enter a hexadecimal number (1 to 22 digits) in "
		   "the form xx.xxxx.xxxx...\n");
        } else {
            break;
	}
    }
}


/*
 * get_clns_area
 * Get the CLNS area
 */

static void get_clns_area (char *prompt, uchar def[2], uchar *area)
{
    boolean status;
    char buffer[MAXBUF];
    NSAP_T nsap;

    while (!(stdio->statbits & CONTROLC)) {
        printf("%s [%-02x%-02x]: ", prompt, def[0], def[1]);
        status = rdtty(buffer, MAXBUF);
        if (!status || null(buffer)) {
            *area++ = def[0];
            *area = def[1];
            break;
        }
        if (strlen(buffer) == 4) {
            if (parse_nsap(buffer, nsap)) {
                if ((nsap[0] > 0) && (nsap[0] < 3)) {
                    *area++ = nsap[1];
                    *area =  nsap[2];
                    break;
                }
            } else {
                *area++ = def[0];
                *area = def[1];
                break;
            }
        }
        printf("\n%% Four digit hexadecimal number\n");
    }
}


/*
 * get_clns_station
 *
 */
#define ISHEXDIGIT(c) (((c>='0') && (c<='9')) || ((c >= 'A') && (c <= 'F')))
#define CVHTOB(c) ((int)c - (((c >= '0') && (c <= '9')) ? '0' : '7'))

static void get_clns_station (char *prompt, uchar def[6], uchar *station)
{
    boolean status;
    char *ptr,buffer[MAXBUF];
    char c;
    int i;

    while (!(stdio->statbits & CONTROLC)) {
        printf("%s [%-02x%-02x.%-02x%-02x.%-02x%-02x]: ", prompt,
               def[0], def[1], def[2], def[3], def[4], def[5]);
        status = rdtty(buffer, MAXBUF);
        if (!status || null(buffer)) {
            for (i = 0; i < STATION_LENGTH; i++) {
                *station++ = def[i];
            }
            return;
        }
        ptr = deblank(buffer);
        if (null(ptr)) {
            for (i = 0; i < STATION_LENGTH; i++) {
                *station++ = def[i];
            }
            return;
        }
        if (strlen(buffer) == 14) {
            while (*ptr) {
                c = toupper(*ptr++);
                if (c == '.') {
                    continue;
		}
                if (ISHEXDIGIT(c)) {
                    i = CVHTOB(c);
                    if (*ptr) {
                        c = toupper(*ptr++);
                        if (ISHEXDIGIT(c)) {
                            i = (i << 4) + CVHTOB(c);
                            *station++ = (uchar) i;
                        }
                    }
                }
            }
            return;
        }
        printf("\n%% Twelve digit station id in the form: xxxx.xxxx.xxxx\n");
    }
}
