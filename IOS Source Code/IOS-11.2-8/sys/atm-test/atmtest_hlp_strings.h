/* $Id: atmtest_hlp_strings.h,v 3.2 1995/11/17 08:47:43 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/atm-test/atmtest_hlp_strings.h,v $
 *------------------------------------------------------------------
 * atmtest_hlp_strings.h -- Help strings to support the TCL 'help' 
 * command.
 * [Was ../tcl/tcl_hlp_strings.h]
 *
 * April 1995,  Chris Parrott
 *
 * Copyright (c) 1994-1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: atmtest_hlp_strings.h,v $
 * Revision 3.2  1995/11/17  08:47:43  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:02:59  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef _ATMTEST_HLP_STRINGS_H_
#define _ATMTEST_HLP_STRINGS_H_

/*
 * General help string printed for each help message.
 */
static char *general_atmtest_help_msg =
"\nHelp is available for some TCL commands by typing 'tclhelp {item}',\n\
where {item} can be one of the following:\n\n\
lane                          General help with lane specific commands.\n\
atmsig                        General help with atmsig specific commands.\n\n";

/*
 * Help strings for specific items.
 */
static char *lane_help_str = 
"\nThe following TCL commands have been implemented to help \n\
with LANE debugging and testing: \n\n\
tclhelp [item] <CR>           General Help\n\
lsend  [msgtype] [vcd] <CR>   Send a packet\n\
set_lane_def <CR>             Set TCL variable defaults\n\
sh_vars [default] <CR>        Show TCL variables\n\
\n\
The 'lsend' command is used to send LANE control packets of type <msgtype>. \n\
When this command is issued, a 'msgtype' type LANE control packet will \n\
be sent out the ATM interface specified by the 'slot', 'port', and \n\
'subinterface' TCL variables, which must be set prior to running this \n\
command.  If any of these variables is not set prior to running lsend, \n\
default values will be assigned.  In most instances this will result in \n\
an error.\n\
\n\
For example, if you wanted to send a join_request control packet out \n\
atm interface atm2/0.1 on pvc 3, you would type the following sequence \n\
of TCL commands:\n\n\
    router(tcl)# set_lane_def\n\
    router(tcl)# set slot 2\n\
    router(tcl)# set port 0\n\
    router(tcl)# set subinterface 1\n\
    router(tcl)# lsend join_req 3\n\
\n\
If the lsend command is typed with no arguments, a list of supported\n\
LANE control packet types is listed.\n\
\n\
To look at all the TCL variables that are used to construct the various\n\
LANE control packets, use the sh_vars command.  These variables\n\
can be tweeked at any time using the TCL set command.\n\
\n\
If any of the LANE variables aren't set prior to running the lsend command,\n\
default values will be assigned before the message is sent.  Default\n\

values can be assigned at any time using the set_lane_def command.\n\
A list of the default settings can be seen by typing 'sh_vars default'.\n";

static char *atmsig_help_str =
"\nATM Signalling related TCL commands have been implemented to help \n\
test ATM signalling.  In general, the commands allow a user to send \n\
messages to the ATM signalling driver software on the router so that \n\
SVCs can be established.  Both 'called' and 'calling' users can be\n\
simulated.  Note that the primary motivation for creating this test \n\
tool was to test the initial implementation of the LAN Emulation software\n\
on the router. \n\
The following ATM Sig commands are available: \n\n\
\n\
si                            Initialize Signalling\n\
sln                           Send LISTEN_NSAP message\n\
scn                           Send CANCEL_NSAP message\n\
ss                            Send SETUP message\n\
sc                            Send CONNECT message\n\
sr                            Send RELEASE message\n\
src                           Send RELEASE_COMPLETE message\n\
sap                           Send ADD_PARTY message\n\
sdp                           Send DROP_PARTY message\n\
\n";

/*
 * static char *vcd_arg_help_str =
 * "\n\
 *     lec_direct\n\
 *     lec_configure\n\
 *     lec_forward\n\
 *     les_direct\n\
 *     les_configure\n\
 *     bus_forward\n\
 *     configure\n\n";
 */

/*
 * static char *sln_help_str =
 * "\nSyntax: sln <CR>\n\
 * \n\
 * The Send LISTEN_NSAP command (sln) sends a LISTEN_NSAP message \n\
 * to the sigAPI driver code running on the router.\n";
 */


#endif _ATMTEST_HLP_STRINGS_H_
