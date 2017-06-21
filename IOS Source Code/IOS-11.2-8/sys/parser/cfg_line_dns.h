/*
 *------------------------------------------------------------
 * C F G _ L I N E _ D N S . H
 *
 * October 1996, Chris White
 *
 * Copyright (c) 1996-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------
 */

/******************************************************
 *       [no] domain-lookup
 *     
 *
 */
EOLS(cl_dns_eol, line_command, TERM_DNS);
NVGENS  (cl_dns_nvgen, cl_dns_eol, line_command, TERM_DNS);
KEYWORD (cl_dns, cl_dns_nvgen, ALTERNATE, "domain-lookup", 
	 "Enable domain lookups in show commands", PRIV_USER);

#undef ALTERNATE
#define ALTERNATE	cl_dns
