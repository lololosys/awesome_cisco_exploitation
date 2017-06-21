/* $Id: iprouting_aspath.c,v 3.1 1996/02/24 02:01:52 pst Exp $
 * $Source: /release/111/cvs/Xsys/iprouting/iprouting_aspath.c,v $
 *------------------------------------------------------------------
 * IP routing AS path control
 *
 * Feburary 1996, Paul Traina
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: iprouting_aspath.c,v $
 * Revision 3.1  1996/02/24  02:01:52  pst
 * CSCdi49774:  modularize regexp's for bgp (part 3)
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "config.h"
#include "access.h"
#include "parser.h"
#include "registry.h"

#include "../ip/ip.h"
#include "interface_private.h"
#include "../ip/ip_registry.h"

regexpacctype *as_path_accesslists[(MAXREGEXPACC-MINREGEXPACC)+1];

void
ip_aspath_command (parseinfo *csb)
{
    int list;

    list = regexpaccess_command(csb, as_path_accesslists);
    if (list)
	reg_invoke_ip_aspath_access_list(list);
}

void
ip_aspath_show (parseinfo *csb)
{
    int list = GETOBJ(int, 1);

    automore_enable(NULL);
    show_regexpacl(as_path_accesslists, list, "AS path");
    automore_disable();
}
