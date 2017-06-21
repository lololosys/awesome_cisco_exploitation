# $Id: makefile,v 3.39.4.68 1996/09/14 05:23:06 mvenugop Exp $
# $Source: /release/112/cvs/Xsys/makefile,v $
#------------------------------------------------------------------
# Base Makefile that contains all file and module definitions
#
# Copyright (c) 1992-1997 by cisco Systems, Inc.
# All rights reserved.
#------------------------------------------------------------------
# $Log: makefile,v $
# Revision 3.39.4.68  1996/09/14  05:23:06  mvenugop
# CSCdi69188:  Updating Gaia changes
# Branch: California_branch
#
# Revision 3.39.4.67  1996/09/14  01:01:02  ppearce
# CSCdi68885:  SRB subblock not setup properly for VIP interfaces
# Branch: California_branch
#   Previous commit breaks build for c7200- images
#     Backout previous changes
#     Create SRB_SB subsystem to init SRB vector table
#       rather than init in  VIP reset routines
#
# Revision 3.39.4.66  1996/09/14  00:12:38  hampton
# Incorporate the Error Messages Manual text into the source code.
# [CSCdi69164]
# Branch: California_branch
#
# Revision 3.39.4.65  1996/09/03  18:47:04  richl
# CSCdi67863:  commit gaia stuff, and crypto top-level
#
# Revision 3.39.4.64  1996/09/03  15:51:07  richl
# CSCdi67850:  rename images
#
# Revision 3.39.4.63  1996/08/30  05:16:26  rchiao
# CSCdi67676:  install locking control for reg src file generation
# Branch: California_branch
#
# Revision 3.39.4.62  1996/08/28  12:34:57  thille
# CSCdi67083: Message externs need a cleanup.
# Branch: California_branch
# Get rid of all extern msgsym()s in .c files, rework logger.h and
# msg_foo.c files so they are included and provide the externs.  Clean up
# other random externs in .c files.
#
# Revision 3.39.4.61  1996/08/20  03:13:51  dkatz
# CSCdi65483:  NTP needs modularity cleanup
# Branch: California_branch
# Code review comments.
#
# Revision 3.39.4.60  1996/08/19  19:04:38  dkatz
# CSCdi65483:  NTP needs modularity cleanup
# Branch: California_branch
# Break up NTP into separate subsystems.  Significantly reduce image size
# and stack usage.  Clean up refclock interface.  Make all modules
# compile cleanly with all error checking enabled (DUSTY is clean!)
#
# Revision 3.39.4.59  1996/08/16  18:31:23  lmetzger
# CSCdi66255:  Add Modularity of RTR for DSPU/SNANM
# Branch: California_branch
#
# Revision 3.39.4.58  1996/08/10  23:44:05  irfan
# CSCdi64746:  DHCP proxy client included in BOOT
# Branch: California_branch
# rename DHCP to DHCP_CLIENT. boot it out of BOOT.
#
# Revision 3.39.4.57  1996/08/06  18:09:50  skalahas
# CSCdi65242:  GAIA makefile enhancements
# Branch: California_branch
#
# Revision 3.39.4.56  1996/08/02  20:09:09  snijsure
# CSCdi64981:  make crypto snmp modular so images can be built without
# snmp.
# Branch: California_branch
# Separate out the snmp objects from main crypto objects.
#
# Revision 3.39.4.55  1996/07/31  21:40:55  lmetzger
# CSCdi64744:  Move Registry Initialization to the RTT subsystem
# Branch: California_branch
#
# Revision 3.39.4.54  1996/07/15  07:25:59  bchan
# CSCdi62509:  Change tn3270 server mib
# Branch: California_branch
# rename sr_tn3270smib* to sr_tn3270servermib*
#
# Revision 3.39.4.53  1996/07/15  02:10:41  bchan
# CSCdi50608:  makefile for mib incorrect
# Branch: California_branch
#
# Revision 3.39.4.52  1996/07/14  07:10:46  hampton
# Eliminate all redundant dependencies for any given file.  Split out
# platform specific dependencies and put them in the appropriate obj-xxx
# directory.  [CSCdi62810]
# Branch: California_branch
#
# Revision 3.39.4.51  1996/07/13  23:03:37  gstovall
# CSCdi62880:  make superclean does not nuke everything
# Branch: California_branch
# Make find follow symlinks.
#
# Revision 3.39.4.50  1996/07/12  23:28:35  jjohnson
# CSCdi62131:  Need a mechanism to support logical snmp entities
# Branch: California_branch
#
# Revision 3.39.4.49  1996/07/09  05:36:12  ppearce
# CSCdi59527:  inlines wastes code space
# Branch: California_branch
#   Just say "no" to inline abuse - SRB subblock inlines removed
#
# Revision 3.39.4.48  1996/07/02  23:26:36  bhavesh
# CSCdi61949:  Explicit enforcement of GAIA target dependencies.
# Branch: California_branch
#
# Revision 3.39.4.47  1996/07/02  18:15:32  ralls
# CSCdi61900:  Update make.features
# Branch: California_branch
#
# Revision 3.39.4.46  1996/07/02  00:46:24  jpfeifer
# CSCdi61850:  adding support for gaia based production images
# Branch: California_branch
#
# Revision 3.39.4.45  1996/06/24  21:24:58  smackie
# Fix the assumption that the R4600 and 68K platforms always have a
# "classic" Rom Monitor. (CSCdi61149)
# Branch: California_branch
#
# Revision 3.39.4.44  1996/06/23  18:19:02  dkatz
# CSCdi61119:  Commit SNTP Code
# Branch: California_branch
# Code review comments.
#
# Revision 3.39.4.43  1996/06/19  23:16:30  smackie
# The chunk manager displays it's new look for 1996. (CSCdi60779)
#
#   o Soothe the raging masses and implement lockable chunks
#   o Start hiding chunk structures and defines in a private header
#   o Add a chain file for "show chunk" and remove entry from chain.c
#   o Tidy up some redundant sections of code
#
# Branch: California_branch
#
# Revision 3.39.4.42  1996/06/19  21:45:10  bhavesh
# CSCdi60827:  Makefile enhancements to perfrom parallel builds.
# Branch: California_branch
#
# Revision 3.39.4.41  1996/06/19  18:28:41  widmer
# CSCdi56931:  Length of the text fields are high
# Branch: California_branch
#
# Revision 3.39.4.40  1996/06/16  21:24:37  vnguyen
# Finally committing Brasil to California_branch
#
# Revision 3.39.4.39  1996/06/12  22:49:14  pbopardi
# CSCdi60241:  Async HTTP 11.2 commit
# CSCdi60245: Frame relay EzSetup HTTP 11.2 commit
# Branch: California_branch
#
# Revision 3.39.4.38  1996/06/07  00:33:20  bew
# CSCdi59169:  Crypto cleanup work
# Branch: California_branch
# Removed special Cylink compilation rules and added those
# objects to the crypto subsystem.
#
# Revision 3.39.4.37  1996/06/04  23:26:55  ralls
# CSCdi57580:  Update gaia features, onec more
# Branch: California_branch
#
# Revision 3.39.4.36  1996/06/04  05:14:59  mshannon
# CSCdi45507:  sanity checking code required for memory corruptions
# Branch: California_branch
# Added initial debug sanity code for memory corruptions
#
# Revision 3.39.4.35  1996/06/04  00:40:25  ddecapit
# CSCdi59346:  Makefile changes reqd. to move TRLANE to non-production
# images
# Branch: California_branch
#
# Revision 3.39.4.34  1996/05/30  20:26:24  bew
# CSCdi58950:  Need to make exportable crypto images
# Branch: California_branch
# Add exportable crypto subsystem.
#
# Revision 3.39.4.33  1996/05/30  16:46:59  thille
# CSCdi58948:  Oddballs targets in obj-68-ags not being built
# Branch: California_branch
# Stick obj-68-ags back into PROD_68_OBJDIRS.
#
# Revision 3.39.4.32  1996/05/22  20:55:02  alanyu
# The current 'nodepend' rule in sys/makefile will not find all the
# elements
# that intend to find under clearcase environment.  The rule
#         find . -name \.D\.\* -print
#                 should change to
#         find . -name \.D\.\* -follow -print
# The new change has no effect under cvs environment.
#
# CSCdi58450:  target nodepend will no work properly under clearcase
# environment
# Branch: California_branch
#
# Revision 3.39.4.31  1996/05/21  06:14:38  mordock
# Branch: California_branch
# Improve snmp modularity via creating services to register interfaces
# and chassis cards.
# Add syslog mib.
#
# Revision 3.39.4.30  1996/05/19  00:24:18  rpratt
# CSCdi57306:  HTML config has problems in new platform
# Branch: California_branch
#
# Revision 3.39.4.29  1996/05/17  10:26:30  ppearce
# Merge IbuMod into Calif
#
# Revision 3.34.2.10  1996/05/12  07:51:12  bchan
# CSCdi57469:  change tn3270smib and show tn3270
# Branch: IbuMod_Calif_branch
#
# Revision 3.34.2.9  1996/05/05  22:39:33  ppearce
# Sync to IbuMod_Calif_baseline_960504
#
# Revision 3.34.2.8  1996/05/02  09:16:37  dstine
# Branch: IbuMod_Calif_branch
# - tweek modularity linking for IBU cleanups
#
# Revision 3.34.2.7  1996/04/26  08:03:42  dstine
# Branch: IbuMod_Calif_branch
# - commit of IBU modularity work, phase 1.
#
# Revision 3.34.2.6  1996/04/05  06:04:31  schiang
# Branch: IbuMod_Calif_branch
# merge ncia server code
#
# Revision 3.34.2.5  1996/04/03  13:15:40  ppearce
# Sync to IbuMod_Calif_baseline_960402
#
# Revision 3.34.2.4  1996/03/28  12:06:02  bchan
# CSCdi52724:  merge tn3280 server code
# Branch: IbuMod_Calif_branch
#
# Revision 3.34.2.3  1996/03/27  01:33:07  bchan
# CSCdi50608:  makefile for mib incorrect
# Branch: IbuMod_Calif_branch
#
# Revision 3.34.2.2  1996/03/22  18:38:14  lmetzger
# Initial Version of Response Time Reporter
#
# Revision 3.34.2.1  1996/03/17  17:27:58  ppearce
# Sync IbuMod_Calif_branch to V111_1_3
#
# Revision 3.39.4.28  1996/05/15  04:29:08  fowler
# Frame relay VC queuing & rate enforcement
# Branch: California_branch
#
# Revision 3.39.4.27  1996/05/13  22:00:26  rbadri
# CSCdi57412:  Token Ring lane subsystem should be only int 4500/RSP J
# images
# Branch: California_branch
# Include TRLANE only on select RSP/4500 images.
#
# Revision 3.39.4.26  1996/05/11  06:09:05  fox
# CSCdi55358:  PPP subsystem needs to be more modular
# Branch: California_branch
# Attach lex_ncp subsystem to lex subsystem as lex requires PPP.
# Add registry calls to eliminate PPP's dependancy on CCP/Compress.
#
# Revision 3.39.4.25  1996/05/10  23:24:44  gstovall
# CSCdi56792:  Need to remove unused files
# Branch: California_branch
# Nuke support for the Irix image, also stamp out the last of the CS500.
#
# Revision 3.39.4.24  1996/05/10  01:29:59  wmay
# CPP and ppp half-bridge commit
# Branch: California_branch
#
# Revision 3.39.4.23  1996/05/09  14:09:43  rbadri
# Branch: California_branch
# LANE, UNI3.1 features
#
# Revision 3.39.4.22  1996/05/08  12:01:25  mdb
# Implement a method of naming locally built tools based on the type of
# the Machine, the name and the revision of the Operating System.
# Also make provide for all locally built tools be to removed when a
# 'make superclean' is performed in the sys directory. [CSCdi57043]
# Branch: California_branch
#
# Revision 3.39.4.21  1996/05/08  03:57:43  ralls
# CSCdi57036:  Ole man river he just keeps rollin along
# Branch: California_branch
#
# Revision 3.39.4.20  1996/05/08  01:07:19  gchristy
# CSCdi56944:  Compilation errors from IP named access list commit
# Branch: California_branch
# - Modularize new ACL code for inclusion in VIP images.
#
# Revision 3.39.4.19  1996/05/04  00:27:27  wilber
# Branch: California_branch
#
# Integrated Routing and Bridging
# - Add tbridge_bvi.o to the tbridge subsystem.
#
# Constrained Multicast Flooding
# - Define the CMF subsystem for Transparent Bridging.
# - Add the CMF subsystem to -j images.
# - Add the tbridge monitor to the tbridge subsystem,
#   and add the rules for building the CMF subsystem.
#
# Name Access List
#
# Revision 3.39.4.18  1996/05/03  01:56:44  sluong
# Branch: California_branch
# IPX ACL Violation Logging, Plaining English Filters, Display SAP by
# name,
# NLSP MIBS, Scaleable RIP/SAP, Modularity, and SAP query by name.
#
# Revision 3.39.4.17  1996/05/02  07:21:43  dwong
# Branch: California_branch
# Commit SMRP-7kFastSwitch, Load Balancing, and AURP Modularity
#
# Revision 3.39.4.16  1996/05/01  14:20:43  fox
# CSCdi55358:  PPP subsystem needs to be more modular
# Branch: California_branch
# Make PPP NCPs into real subsystems.  Add them to all images where
# PPP and the related network protocol are located.  NCPs are dependant
# on PPP and the related network protocol subsystem but not vice versa.
# Removed $(KERNEL) from FR_SVC_ONLY in order to avoid massive errors
# in modularity builds.
#
# Revision 3.39.4.15  1996/04/30  23:06:14  dclare
# CSCdi56247:  fix ISDN code to use proper prototyping and not use DIRTY
# or DUSTY.
# Branch: California_branch
#
# Revision 3.39.4.14  1996/04/30  20:07:51  ralls
# CSCdi56220:  Update Gaia features
# Branch: California_branch
#
# Revision 3.39.4.13  1996/04/29  08:36:55  fox
# CSCdi55358:  PPP subsystem needs to be more modular
# Branch: California_branch
# Move Network Control Protocol (NCP) support from PPP subsystem to
# individual subsystems.  Attach NCP subsystems to respective protocol
# subsystems.  Add registry calls to network protocol init code to
# register default NCP support for NCPs not requiring option negotiation.
#
# Revision 3.39.4.12  1996/04/27  15:11:44  tkolar
# Branch: California_branch
# Complete Dial1_branch merge.
#
# Revision 3.39.4.11.2.2  1996/05/04  02:42:47  rlowe
# Add obj-68-alc to list of production image directories.
# Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
#
# Revision 3.39.4.11.2.1  1996/04/27  06:32:07  cakyol
# non sync sync from V112_0_2 to ....
# Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
#
# Revision 3.39.4.11  1996/04/25  22:55:50  ronnie
# CSCdi55766:  Hey!  This ATM and Frame Relay is no different than in 11.1
# Feature commit for ATM and Frame Relay into 11.2
# Branch: California_branch
#
# Revision 3.39.4.10  1996/04/23  19:25:22  ralls
# CSCdi55428:  Update the gaia packaging data
# Branch: California_branch
#
# Revision 3.20.2.6  1996/04/25  23:16:30  syiu
# Branch: Dial1_branch
# Sync Dial1_branch to IosSec_postmerge_960419
#
# Revision 3.20.2.5  1996/04/16  20:14:04  syiu
# Branch: Dial1_branch
# Sync Dial1_branch to V111_1_3
#
# Revision 3.20.2.4  1996/01/31  18:30:16  tkolar
# Branch: Dial1_branch
# Get all of the VPN code into the repository, with the right names
# and in the right places.
#
# Revision 3.20.2.3  1996/01/26  20:30:35  agh
# Branch: Dial1_branch
# Initial Code Commit for MLPVT.
#
# Revision 3.20.2.2  1996/01/22  18:51:34  syiu
# Branch: Dial1_branch
# Makefile support for L2F
#
# Revision 3.20.2.1  1996/01/18  05:24:36  perryl
# Branch: Dial1_branch
# Virtual Access interface services
#
# Revision 3.39.4.9  1996/04/19  14:29:50  che
# CSCdi55138:  IosSec_branch feature commit
# Branch: California_branch
# IosSecurity feature commits
#
# Revision 3.39.4.8  1996/04/17  13:34:13  fred
# CSCdi54910:  commit rsvp, traffic shaping, and RED
# Branch: California_branch
#
# Revision 3.39.4.7  1996/04/16  18:48:04  rchandra
# CSCdi54830:  IP feature commit
# Branch: California_branch
#
# Revision 3.39.4.6  1996/04/15  21:06:03  bcole
# CSCdi54649:  IP Host Modularity, IP Automatic Stub Routing feature
# commit
# Branch: California_branch
#
# Revision 3.39.4.5  1996/04/15  14:57:12  widmer
# CSCdi47180:  No Single Command to Collect General Router Data
# Branch: California_branch
# Add "show tech-support" command
# Change references to encryption types to uint
# Change tt_soc to use password_struct
#
# Revision 3.39.4.4  1996/04/12  16:59:41  mdb
# CSCdi54154:  Need to remove old files
# Branch: California_branch
# Remove references to gutted files in the makefiles and nuke the gutted
# files.
#
# Revision 3.39.4.3  1996/04/09  01:03:33  mdb
# CSCdi54061:  speedup checks for GDB_REQUIRED
# Branch: California_branch
#
# Move from using a .gdb_required file in each directory to having a
# single sys/gdb_required file which lists all of the directories to be
# built with debugging enabled.
#
# Revision 3.39.4.2.6.2  1996/04/21  01:11:04  rbadri
# Branch: LE_Cal_V112_0_2_branch
# Make TRLANE a new sub-system
#
# Revision 3.39.4.2.6.1  1996/04/08  02:10:23  bbenson
# Branch: LE_Cal_V112_0_2_branch
# Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
#
# Revision 3.39.4.2  1996/03/21  22:09:40  gstovall
# Branch: California_branch
# The ELC_branch now knows the joy of California dreaming.
#
# Revision 3.39.4.1  1996/03/18  18:45:04  gstovall
# Branch: California_branch
# Elvis has left the building.  He headed out to California, and took the
# port ready changes with him.
#
# Revision 3.39.8.1  1996/03/22  09:32:04  rlowe
# Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
# yielding LE_Syn_Cal_V111_1_3_branch.
# Branch: LE_Syn_Cal_V111_1_3_branch
#
# Revision 3.39  1996/03/10  07:45:32  ahh
# CSCdi50817:  TCP: add ttcp to some images
# (Patch 2 of 2)
#
# Revision 3.38  1996/03/09  22:46:54  widmer
# CSCdi47890:  HTTP server doesnt work
# Add HTTP server
#
# Revision 3.37  1996/03/01  16:41:57  vandys
# CSCdi50393:  No way to monitor TTY lines remotely
#
# Revision 3.36  1996/03/01  11:30:43  mdb
# Add a new shl_%.a construct for libraries in platform directories as a
# symlink to the lib%.a file in the generic processor directories. This
# uses the same general mechanism as the shr_%.o construct for sub_%.o
# files. This fix (CSCdi45329) will ensure that libraries such as
# libutil.a need only be built once for each processor type.
#
# Revision 3.35  1996/03/01  05:42:54  shj
# CSCdi50334:  <make nodepend> does not delete all previous dependancies
# Use find to locate .D.* files, so we get 'em no matter how deep
# directories are nested.
#
# Revision 3.34  1996/02/27  07:17:48  rchiao
# CSCdi49917:  makefile changes for ClearCase, part II
# This is 2nd portions of the commit to fix problems identified with
# earlier commit today.
# makefile: two perl scripts moved from sys/scripts to /router/bin are
# corrected.
# ../dependancies changed to $(SYSROOT)/dependancies.
# cisco.SH: the definition of SYSROOT should be relative to where cisco.sh
# is called.
#
# Revision 3.33  1996/02/21  20:21:58  rchiao
# CSCdi49477:  backing out SYSROOT from GDB_REQUIRED
# changed $(SYSROOT) back to ..
#
# Revision 3.32  1996/02/21  07:39:38  pst
# Add cisco_inet_aton() to library
#
# Revision 3.31  1996/02/20  06:14:38  jjohnson
# CSCdi49356:  Need MIB variable for total DRAM
# Fully instrument the memory pools
#
# Revision 3.30  1996/02/17  09:06:58  rchiao
# CSCdi49271:  makefile changes for ClearCase
# adding sysroot definition for atria. For obj-* changes, please see
# obj-4k/makefile or obj-68/makefile for example.
#
# Revision 3.29  1996/02/17  00:45:58  pst
# CSCdi49257:  need user-friendly support for RFC-1828 keyed MD5
# authentication
#
# Revision 3.28  1996/02/15  04:15:53  smackie
# Kerberos include path should only be used for source files in the
# actual kerberos subsystem. (CSCdi49003)
#
# Revision 3.27.6.1  1996/03/05  05:41:53  rlowe
# Apply LANE content to merge branch.
# Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
#
# Revision 3.27.2.1  1996/02/27  20:39:33  cakyol
# non sync sync of LE_Cal_V111_0_16_branch to V111_1_0_3 yielding
# LE_Cal_V111_1_0_3_branch
# Branch: LE_Cal_V111_1_0_3_branch
#
# Revision 3.27  1996/02/08  20:05:29  thille
# CSCdi48347: format string in inlines, code size impact
# Pass 1 - Clean up some literal strings in inlines, duplicate literals,
# etc.  11,356 bytes saved on gs7-j-m, 9336 bytes saved on igs-j-l
#
# Revision 3.26  1996/02/05  22:45:21  hampton
# CSCdi48192:  Scheduler test subsystem should not be in production images
#
# Revision 3.25  1996/02/02  00:53:09  nitin
# CSCdi46878:  build performance
# Changed spaces to tab in build rule. Bug with my previous commit.
#
# Revision 3.24  1996/01/29  05:48:01  nitin
# CSCdi46878:  build performance
# Changes and comments were added to makefile so that make dist4K, make
# dist36,
# and make dist68 can be invoked at the same time. Merged changes from
# 11.0 rev 2.39.
#
# Revision 3.23  1996/01/25  11:15:48  smackie
# Registry code gets some much-needed hot-oil massaging, free liposuction
# and a makeover.  (CSCdi47101)
#
#    o Compress registry structures. Saves over 120K for all platforms
#    o Add registry subsystems and remove feature registry initializion
#      from registry.c to fix woeful initialization scaling problems
#    o Remove unused and seemingly unloved registry debugging code
#    o Add registry memory statistics to shame people into action
#
# Revision 3.22  1996/01/18  16:37:21  jjohnson
# CSCdi45754:  snmp subsystem should not contain non-snmp mibs
# move the rs232 mib, image mib, and terminal services mib into separate
# subsystems.  move the cisco interfaces mib into the interfaces mib
# subsystem.  consolidate the snmp timer subsystem into the snmp
# subsystem since it really is core snmp technology.
#
# Revision 3.21  1996/01/16  06:21:41  billw
# CSCdi46868:  latgroup.o should not be in core subsystem
# move it into a library, and add function registration for the bits
# called by the parser.
#
# Revision 3.20  1996/01/11  11:27:47  billw
# CSCdi43115:  PPP CHAP should use larger challenge for radius
# compatibility
# oops.  forgot the makefile change.
#
# Revision 3.19.2.2  1996/01/24  01:33:21  cyoung
# VTP MIB support and dependant auto-config base. Part I
# Branch: LE_Cal_V111_0_16_branch
#
# Revision 3.19.2.1  1996/01/11  01:43:59  cakyol
# non sync sync of LE_Calif_branch to V111_0_16 yielding
#     LE_Cal_V111_0_16_branch
#
# Branch: LE_Cal_V111_0_16_branch
#
# Revision 3.19  1996/01/08  00:41:57  sdurham
# CSCdi46545:  CONFMANMIB should be compiled in with MIBS not SNMP
#
# Revision 3.18  1996/01/06  02:49:31  gwynne
# CSCdi46429:  Add *.binsize files to makefile cleanobj rule
#
# Revision 3.17  1996/01/05  10:17:06  hampton
# Move/rename the files containing the deprecated timer callback
# routines.  [CSCdi46482]
#
# Revision 3.16  1996/01/03  03:24:29  ahh
# CSCdi45528:  Move key manager out of core
#
# Revision 3.15  1996/01/01  00:02:21  hampton
# Compilation of full debugging information should be off by default.
# [CSCdi46269]
#
# Revision 3.14  1995/12/21  16:28:25  hampton
# Switch the 68K version of DELAY() to call the existing usecdelay
# routine which uses the timer chip.  For all other platforms, move a
# routine or two between files.  [CSCdi45965]
#
# Revision 3.13  1995/12/17  18:23:40  thille
# CSCdi45760: Parser needs to go on a diet.
# Move object files that are not part of parser to os_exec or libutil.a
# as appropriate.  Rename parser files to make it clear what is part of
# parser and avoid other stuff accumulating.  Modify files that include
# parser .h files to deal with new names.
#
# Revision 3.12  1995/12/14  15:33:28  ravip
# CSCdi41809:  DLSW MIB ciruit counters and OIDs always set to 0
#              add CDLSWMIB back in the makefile
#
# Revision 3.11  1995/12/14  08:21:49  jjohnson
# CSCdi44148:  rationalize snmp library services
#
# Revision 3.10  1995/12/14  00:13:17  ralls
# CSCdi45537:  Add null and nonbooting images to makefile
#
# Revision 3.9  1995/12/11  23:15:23  billw
# CSCdi42614:  RADIUS is in all images
# Include in -j-, -p-, and all 25xx commserver images.
#
# Revision 3.8  1995/12/06  21:34:13  foster
# CSCdi44432:  rsp slave image dependenices are broken
#
# Revision 3.7.4.1  1995/12/23  03:57:22  dcheng
# Branch: LE_Calif_branch
#
# Revision 3.7  1995/12/03  17:55:42  thille
# CSCdi44905:  ciscopro targets not in 11.1
# Put them in.
#
# Revision 3.6  1995/11/22  22:13:14  ahh
# CSCdi44357:  Some files are not dusty anymore
# Remove them from DUSTY.
#
# Revision 3.5  1995/11/22  05:40:56  eschaffe
# CSCdi42241:  PROTOCONVERT is TRUE for images that don't include the PT
# option.
# Obsolete PROTOCONVERT flag in makefiles.  The control for defining
# protocolconversion for an image is now platform_specific.
#
# Revision 3.4  1995/11/18  07:56:39  tkolar
# CSCdi42613:  Ident should not be in all images
# Shuffle poor Ident off to small feature Siberia (-j image)
#
# Revision 3.3  1995/11/17  08:32:16  hampton
# Remove old entries from the RCS header logs.
#
# Revision 3.2  1995/11/16  22:56:37  gstovall
# Ladies and gentlemen, I introduce to you, the port ready commit.
#
# Revision 3.1  1995/11/09  10:48:12  shaker
# Bump version numbers from 2.x to 3.x.
#
# Revision 2.35  1995/11/08  20:47:13  shaker
# Merge Arkansas_branch into 11.1 mainline.
#
# Revision 2.34  1995/10/21  07:34:41  bdas
# CSCdi38453:  ISDN traps need corresponding syslog messages
#
# Revision 2.33  1995/10/20  05:13:27  pst
# CSCdi42495:  replace current memmove() routine with one that is more
# efficient in the reverse case and more portable in general.
#
# Revision 2.32  1995/10/18  21:24:40  hampton
# Need a space conserving way to test build all images.  Add the top
# level targets production.testall, oddballs.testall, modular.testall,
# and everything.testall.  These delete each target after making it to
# limit disk space usage.  Also add a way to make a vi ctags file.
# [CSCdi42405]
#
# Revision 2.31  1995/10/13  21:14:22  rchiao
# CSCdi42155:  makesubsys fails when build fisrt appn related images
# Added xlate to cleanobj for makefile. This allows appn subsystem object
# file creation a little more complete.
# If xlate file is removed, then we make sure xlate.perl is executed and
# both sub_appn.o.gz and sub_dlur.o.gz are linked again.
#
# Revision 2.30  1995/09/25  08:34:58  tli
# CSCdi40960:  ISP jumbo patch 2
#
# Revision 2.29  1995/09/24  03:25:43  dkatz
# CSCdi40898:  Synchronized reload needed
#
# Revision 2.28  1995/09/23  21:24:21  dkatz
# CSCdi38306:  ISIS shouldnt run SPF on leaf route changes
# Rework subsystems for ISIS.
#
# Revision 2.27  1995/09/18  07:26:38  smackie
# Prune buffer code into discrete and manageable sections to restore some
# sanity and focus to buffers.c (CSCdi40529).
#
#   o buffers.c - buffer management code
#   o element.c - queue element code
#   o queue.c   - singly-linked queue support
#
# Revision 2.26  1995/09/17  15:38:36  thille
# CSCdi40516:  Mib compilation not always done when it should be
# Nice spot Jeff!
#
# Revision 2.25  1995/09/15  02:12:23  dcoli
# CSCdi39433:  Bad Encapulation Error
# Break frame_relay and atm_dxi into separate object modules.
#
# Revision 2.24  1995/09/14  01:42:03  biren
# CSCdi40312:  Move pakgen and pgen (Pagent 2.2) to seperate subsystems
#
# Revision 2.23  1995/09/04  01:00:45  richl
# CSCdi39767:  Add BiSync to -r- images on low end
#
# Revision 2.22  1995/09/03  23:53:49  richl
# CSCdi39765:  Modularity: modular-ip broken in nightly build
# Change makefile variables so routing mibs aren't always included
# with routing protocols.
#
# Revision 2.21  1995/09/02  18:20:30  richl
# CSCdi39762:  pakgen image broken by CSCdi39304
#
# Revision 2.20  1995/08/25  00:46:49  biren
# Move the TCL-based LANE/ATM signalling test tool to a seperate subsytem
# (atmtest).
#
# CSCdi39304:  Move TCL-based LANE/ATM test tool to separate subsystem
#
# Revision 2.19  1995/08/09  16:17:58  jjohnson
# CSCdi38326:  quicksort should be a utility function
# delete arap_qsort from the arap subsystem
# add new utility function qsort to libutil.a
#
# Revision 2.18  1995/08/08  22:38:57  richl
# CSCdi38432:  dspu subsystem causing confusion, rename it
#
# Revision 2.17  1995/08/07  05:26:57  richl
# CSCdi38315:  makefile rework
#
# Revision 2.16  1995/08/01  18:53:31  smackie
# Remove reference to obj-68-rsp from $(OBJDIRS) (CSCdi37193)
#
# Revision 2.15  1995/08/01  03:58:07  snyder
# CSCdi37989:  RFC 1285 obsolete for current version of SMT
#
# Revision 2.14  1995/07/17  23:26:42  smackie
# Tidy up various makefile semantics (CSCdi37193)
#
#   o Split out native tool support from cross-compilation tools
#   o Rename errant makefiles
#   o Make native object directories more explicitly named
#   o Clean up bogus tool includes
#
# Revision 2.13  1995/07/01  06:00:21  gwynne
# CSCdi36669: Use DISTMAXJOBS to control the -j flag at the top-level
#             make dist step. This allow safe parallelization of
#             release builds.
#
# Revision 2.12  1995/06/28  09:18:11  smackie
# Finally make subsystem sequencing robust with multiple hierarchy
# sequences. Tidy up subsystem code into the bargain and split the
# parser code into a new file. Make the subsystem code far noisier
# about bogusness. (CSCdi23568)
#
# Revision 2.11  1995/06/27  20:44:45  motto
# Code Review updates:
#   - Make use of snmp_if_index instead of hw_if_index
#     using snmp functions in sr_core.c.
#   - Source line length adjustments.
#   - Change VLAN to LAN for CIP internal LANs.
#     This was basically renaming the MIBS,
#     mib source files, and makefiles.
#     This also involved changing the
#     CIPCSNA mib references to virtual
#     LANs on the CIP.
#   - Fix query bug in sr_ciplanmib.c for
#     CIP internal LAN adapters.
# CSCdi33408:  CIP SNA SNMP MIB support
#
# Revision 2.10  1995/06/27  17:06:49  tkolar
# CSCdi35462:  DES code needs to be its own subsystem
# Make it easier to export the source base.
#
# Revision 2.9  1995/06/23  21:54:35  gwynne
# CSCdi36129:  make dist requires loggin of image compile results in each
# obj-xx-xx
# Adding error msgs for release logging to production.dist in makedist
# and dist in makefile. Cleaned up rcp to local host via redefinition of
# "$(TFTPHOST):" to "$(TFTPHOST)$(COLON)".
#
# Revision 2.8  1995/06/23  16:38:41  fred
# CSCdi36295:  Commit Queue MIB to 11.0
#
# Revision 2.7  1995/06/22  07:46:07  bchan
# CSCdi36280:  makealert done twice at make depend
#
# Revision 2.6  1995/06/22  05:20:33  ahh
# CSCdi35994:  LLC2 breaks fair queuing
# Remove mystery patch.
#
# Revision 2.5  1995/06/21  21:08:57  fred
# CSCdi35994:  LLC2 breaks fair queuing
#              rewrite fair_enqueue/fair_dequeue to use
#              pak_enqueue/pak_dequeu
#
# Revision 2.4  1995/06/19  01:53:49  gwynne
# CSCdi35980:  make dist moves subset images prior to release auditing
# process
#
# The dist rule was divided into the dist and the dist epilog rules
# to allow auditing of the images built in the engineering tree prior
# to those same images being moved to the $DISTBASE/bin directory.
#
# Removal of the dist images from the
# engineering tree was moved into a distepilog rule in makedist,
# from the dist rule in makedist.
#
# Additionally, a top-level make production rule was added in the
# makefile.
# (This is in response to bug id CSCdi35978.)
#
# Changes required in the makefile for CSCdi35976 are also included.
#
# Revision 2.3  1995/06/13  23:15:42  cakyol
# CSCdi35795:  make clean should not be allowed to run in non obj
# directories
# - attempting to do otherwise spews out an error message
#
# Revision 2.2  1995/06/10  01:55:28  smackie
# Remove IPC as a default for all platforms.
#
# Revision 2.1  1995/06/07  19:58:37  hampton
# Bump version numbers from 1.x to 2.x.
#
#------------------------------------------------------------------
# $Endlog$
#

ifeq ($(origin SYSROOT), undefined)
SYSROOT :=       $(shell scripts/sysroot)
endif
#
# System label (Experimental Version 9.xx(vvvv))
# (don't forget the trailing space...)
#
SYSTEM_LABEL="Experimental "


#
# For production builds:
#
ifeq ($(origin GLOBAL_DEBUG), undefined)
GLOBAL_DEBUG =
else
GLOBAL_DEBUG = -DGLOBAL_DEBUG 
endif

ifeq ($(origin QUIET), undefined)
QUIET_COM=@\#
QUIET_CMD=@
QUIET_CC=
else
QUIET_COM=@
QUIET_CMD=@
QUIET_CC=@
endif


#
# filename of "viewable" dependancies:
# created by running "make view_depend" in an obj directory
#
VIEWFILE = view_depend

#
# The "bash" shell can't cope with a "-" in an environment variable name
# so provide an alternative for MAKEFLAG-J that lets folks set the value
# of MAKEFLAG-J via an environment variable
#
ifneq ($(origin MAKEFLAG_J), undefined)
MAKEFLAG-J=$(MAKEFLAG_J)
endif

#
# For development:
#
#GLOBAL_DEBUG = -DGLOBAL_DEBUG 
#
# Cut down on search of suffixes
#
.SUFFIXES:

#
# Intermediate files to keep
#
.PRECIOUS: %.symbols %.bin %.sun dependancies %.relocbin
#
# How to compile various files
#

#
# Some files don't need any work done. Tell make that so that we spend
# our time compiling and not dependancy parsing
#

%.h		: ;
../makefile     : ;
$(SYSROOT)/dependancies : ;
../makefile.%	: ;
../makecflow	: ;
../makesubsys	: ;
../makeimages   : ;
../make.features : ;

.PHONY:	default lock all depend tags mibs clean dependancies \
	makefile cisco.o cflowclean force depend_start \
	objdir_includes onedir_includes dependancies.global \
	dependancies.specific all_derived_source

#
# The following rules cancel out the internal suffix rules for
# objects, SCCS and RCS files. This speeds up dependancy parsing.
#

%: %.o
%: %,v
%: RCS/%,v
%: s.%
%: SCCS/s.%

#
# Rules for shared (common object) subsystems.
#

ifeq ($(origin LIBRARY_PREFIX), undefined)
LIBRARY_PREFIX = generic
endif

ifeq ($(OBJECTS),)

shr_%.o: sub_%.o
	$(QUIET_CMD)rm -f $@
	$(QUIET_CMD)ln -s $(patsubst shr_%.o,sub_%.o,$(@F)) $@
	@if [ -f ../export-syms/sub_$(*F).visible ]; then \
	    echo unexport -l ../export-syms/sub_$(*F).visible $@; \
	    unexport --quiet -u -l ../export-syms/sub_$(*F).visible $@ > sub_$(*F).static; \
	    unexport -l ../export-syms/sub_$(*F).visible $@; \
	fi

shl_%.a: lib%.a
	$(QUIET_CMD)rm -f $@
	$(QUIET_CMD)ln -s $(patsubst shl_%.a,lib%.a,$(@F)) $@

else

shr_%.o: FORCE
	$(QUIET_CMD)$(MAKE) --no-print -C $(SYSROOT)/$(OBJECTS) $(patsubst shr_%.o,sub_%.o,$(@F))
	$(QUIET_CMD)rm -f $@
	$(QUIET_CMD)ln -s $(SYSROOT)/$(OBJECTS)/$(patsubst shr_%.o,sub_%.o,$(@F)) $@

shl_%.a: FORCE
	$(QUIET_CMD)$(MAKE) --no-print -C ../$(OBJECTS) $(patsubst shl_%.a,lib%.a,$(@F))
	$(QUIET_CMD)rm -f $@
	$(QUIET_CMD)ln -s $(SYSROOT)/$(OBJECTS)/$(patsubst shl_%.a,lib%.a,$(@F)) $@

FORCE:

endif

sub_%.o:
	$(QUIET_COM)echo "Updating $@"
	$(QUIET_CC) rm -f $(@F)
	$(QUIET_CC)$(LD) $(LOPTIONS) -r -o $(@F) $(sub_$*)
	@if [ -f ../export-syms/sub_$(*F).visible ]; then \
	    echo unexport -l ../export-syms/sub_$(*F).visible $@; \
	    unexport --quiet -u -l ../export-syms/sub_$(*F).visible $@ > sub_$(*F).static; \
	    unexport -l ../export-syms/sub_$(*F).visible $@; \
	fi

%.o:
ifeq ($(FROMLEGO),)
	@$(MAKE) -s -C $(SYSROOT) CC=$(CC) all_derived_source --no-print-directory
endif
	$(QUIET_COM) echo "Compiling $< to $@" \
	    "$(patsubst %,(with debugging),$(filter $(<D), $(GDB_REQUIRED)))"
	$(QUIET_CC)$(CC) -c -MD $(CFLAGS) $(WARN_ALL) \
	    $(patsubst %,-g,$(filter $(<D), $(GDB_REQUIRED))) $< \
	|| (rm -f $@ ; exit 1)
	@sed 's|$(SYSROOT)|\.\.|g' $*.d > .DD.$*
	@rm -f $*.d

%.lis: 
	lister3 CC="$(CC) $(CFLAGS)" $(LIS_FLAG) $< >$*.lis

lib%.a:
	$(QUIET_COM)echo "Updating $@"
	$(QUIET_CC)$(AR) rs $(@F) $(lib$*)

%.cmd: %.m4
	$(QUIET_COM) echo "Compiling $< to $(dir $<)$(@F)"
	$(QUIET_CC)$(M4) $(GLOBAL_DEBUG) $< | grep '[^	 ]' > $(dir $<)$(@F)

REG=$(SYSROOT)/scripts/registry_define
%.regh: %.reg $(REG)
	$(QUIET_COM) echo "Compiling $< to $@"
	$(QUIET_CC)touch $<.lock
	$(QUIET_CC)flock $<.lock $(REG) $< || (rm -f $@ ; exit 1)

%.regc: %.reg $(REG)
	$(QUIET_COM) echo "Compiling $< to $@"
	$(QUIET_CC)touch $<.lock
	$(QUIET_CC)flock $<.lock $(REG) -c $< || (rm -f $@ ; exit 1)

FILES_REG  := $(wildcard $(SYSROOT)/*/*.reg \
                         $(SYSROOT)/c3800/*/*.reg)
FILES_RPC  := $(wildcard $(SYSROOT)/*/*.rpc)

all_derived_source: all_registry_files all_rpc_files $(SYSROOT)/h/interface_types.h

all_registry_files: $(FILES_REG:%.reg=%.regh) $(FILES_REG:%.reg=%.regc)

all_registry_note: 
	@echo "Building all registry files"

all_rpc_files: $(FILES_RPC:%.rpc=%.rpch) $(FILES_RPC:%.rpc=%.rpcc)

all_rpc_note:
	@echo "Building all rpc files"

RPC=$(SYSROOT)/scripts/rpc_define
%.rpch: %.rpc $(RPC)
	$(QUIET_COM) echo "Compiling $< to $@"
	$(QUIET_CC)$(RPC) $< || (rm -f $@ ; exit 1)

%.rpcc: %.rpc $(RPC)
	$(QUIET_COM) echo "Compiling $< to $@"
	$(QUIET_CC)$(RPC) -c $< || (rm -f $@ ; exit 1)

#
# Rules for the process of compiling the various image types
#
# .bin images are stripped images suitable for loading into ROM
# .sys images are suitable for loading by a MOP server.
# .symbols is used for creating symbol table list files
# .tftp creates an image in the tftp directory for netbooting
# .rom images are used to announce the location of something to be loaded
#      into ROMS
# .Z images are compressed using the compress program
# .H images are compressed using the lzhuf program
# .CZ images are self decompressing zipped images
#

#
# Self decompressing zipped image, run from RAM
#
%-mz: %-m.czbin
	$(COPY) $< $(TFTPHOST)$(COLON)$(TFTPDIR)/$(*F)-mz$(BIN_SUFFIX)
	mv $(*F)-m.czsun $(*F)-mz.sun
	rm $<
	@-echo ""
	@-echo "TFTP image: $(TFTPHOST)$(COLON)$(TFTPDIR)/$(*F)-mz$(BIN_SUFFIX)"
	cat $(SYSROOT)/version
	sh $(SYSROOT)/symbols.sh "$(NM)" $(*F)-mz.sun $(*F)-mz.symbols $(PLATFORM) $(*F)-mz

%.tftp: %.bin
	$(COPY) $< $(TFTPHOST)$(COLON)$(TFTPDIR)/$(*F)$(BIN_SUFFIX)
	rm -f $<
	@-echo ""
	@-echo "TFTP image: $(TFTPHOST)$(COLON)$(TFTPDIR)/$(*F)$(BIN_SUFFIX)"
	cat $(SYSROOT)/version
	sh $(SYSROOT)/symbols.sh "$(NM)" $*.sun $*.symbols $(PLATFORM) $*

# reloc denotes a relocatable image.
%.reloc: %.relocbin
	$(COPY) $< $(TFTPHOST)$(COLON)$(TFTPDIR)/$(*F)$(BIN_SUFFIX)
	rm -f $<
	@-echo ""
	@-echo "TFTP image: $(TFTPHOST)$(COLON)$(TFTPDIR)/$(*F)$(BIN_SUFFIX)"
	cat $(SYSROOT)/version
	sh $(SYSROOT)/symbols.sh "$(NM)" $*.sun $*.symbols $(PLATFORM) $*

%.z: %.bin
	compress $<
	$(COPY) $<.Z $(TFTPHOST)$(COLON)$(TFTPDIR)/$(*F)$(BIN_SUFFIX).Z
	rm -f $<.Z
	@-echo ""
	@-echo "TFTP image: $(TFTPHOST)$(COLON)$(TFTPDIR)/$(*F)$(BIN_SUFFIX).Z"
	sh $(SYSROOT)/symbols.sh "$(NM)" $*.sun $*.symbols $(PLATFORM) $*

%.cz: %.czbin
	$(COPY) $*.czbin $(TFTPHOST)$(COLON)$(TFTPDIR)/$(*F)
	rm $*.czbin
	mv $*.czsun $*.sun
	@-echo ""
	@-echo "TFTP image: $(TFTPHOST)$(COLON)$(TFTPDIR)/$(*F)"
	sh $(SYSROOT)/symbols.sh "$(NM)" $*.sun $*.symbols $(PLATFORM) $*

%.rom: %.bin
	@-echo ""
	@-echo "Image not tftpable. Left as `hostname`:$(*F).bin"
	sh $(SYSROOT)/symbols.sh "$(NM)" $*.sun $*.symbols $(PLATFORM) $*

%.test: %.sun
	rm -f $< $*.relocbin

%-mz.test: %-m.czsun
	rm -f $< $*.relocbin

# Rules for MOP versions of images. A .sys file is just a .bin
# file with a 512 byte VMS header at the beginning.
#
%.sys: %.bin
	vmsimage $< $@
	rm -f $<
	@-echo ""
	@-echo "MOP image: $@"

%.relocsys: %.relocbin
	vmsimage $< $*.sys
	rm $<
	@-echo ""
	@-echo "MOP image: $*.sys"

%.mop: %.sys
	$(COPY) $< $(MOPDIR)$(*F)-$(TFTP_POST).sys
	rm -f $<
	@-echo ""
	@-echo "MOP image: $(*F)-$(TFTP_POST)"
	sh $(SYSROOT)/symbols.sh "$(NM)" $*.sun $*.symbols $(PLATFORM) $*

%.Z: %.bin
	compress $<
	mv -f $<.Z $(*F).Z
	@-echo ""
	@-echo "Image left as $(*F).Z"
	sh $(SYSROOT)/symbols.sh "$(NM)" $*.sun $*.symbols $(PLATFORM) $*

%.CZ: %.czbin
	mv -f $*.czbin $(*F).CZ
	mv $*.czsun $*.sun
	@-echo ""
	@-echo "Image left as $(*F).CZ"
	sh $(SYSROOT)/symbols.sh "$(NM)" $*.sun $*.symbols $(PLATFORM) $*

%.H: %.bin
	lzhuf e $< $(*F).H
	rm -f $<
	@-echo ""
	@-echo "Image left at $(*F).H"
	sh $(SYSROOT)/symbols.sh "$(NM)" $*.sun $*.symbols $(PLATFORM) $*

#
# TFTP_POST is used to automatically generate a unique name for
# the image copy into the tftp directory.  The user can
# explicitly override it by specifing a macro definition for
# "tftpsuffix" on the command line.  TFTP_POST is actually used in
# the machine dependent makefiles but is the same for all types so
# is defined here (once).
#
# BIN_SUFFIX is provided to generate an extension to the
# filename if the environment variable bin_suffix is defined.
#

ifeq ($(origin USER),undefined)
USER =		Bozo
endif

ifeq ($(origin tftpsuffix),undefined)
TFTP_POST = 	$(USER)
else
TFTP_POST = 	$(tftpsuffix)
endif

ifeq ($(origin MD5),undefined)
MD5 =		md5
endif


ifeq ($(origin bin_suffix),undefined)
BIN_SUFFIX := 
else
ifeq ($(bin_suffix), )
BIN_SUFFIX := .112
else
BIN_SUFFIX := $(bin_suffix)
endif
endif

#
# Set up definitions for our netbooting hosts
#

# Punctuation for TFTPHOST to enable a null string to be
# substituted for "$(TFTPHOST):" in the dist: rule.
# This prevents copying images to the netbooting hosts when
# doing releases.
COLON=:

ifeq ($(origin TFTPHOST),undefined)
TFTPHOST= dirt
endif

ifeq ($(origin TFTPDIR),undefined)
TFTPDIR= /tftpboot/$(TFTP_POST)
endif

COPY= rcp
TMPLOC= /csctmp
MOPHOST= wheel
MOPDIR = $(MOPHOST):sys\$$common:\[cisco\]

#
# Controlflow Coverage variables
#

# flags for controlflow coverage
CFLOW_SWITCHES=-fprofile-arcs -ftest-coverage

# If user has chosen to do controlflow coverage of the entire system then
# add compiler options for controlflow coverage to CSWITCHES
ifeq ($(CFLOW),ALL)
CFLOW_FLAGS=$(CFLOW_SWITCHES)
else
CFLOW_FLAGS=
endif

ifneq ($(CFLOW),)
CFLOW_SYSTEM = sub_cflow.o
else
CFLOW_SYSTEM =
endif

#
# Specify GDB symbols on a per source directory basis.
#
# To mark a directory so that files in it will be compiled with
# debugging, add a line to sys/gdb_required which has the name
# of the directory prefixed by ../ (e.g., to compile all files in
# the sys/os directory with debugging, add a line with "../os" to
# the sys/gdb_required file. Lines in the sys/gdb_required file which
# begin with a '#' character are treated as comments and are ignored.

# If for some reason you need complete debugging information (e.g.
# you work in the build group) then you should specify "GDB_FLAG=-g" on
# the command line of your make.

# The previous method of marking a directory for debugging by creating
# an empty file called ".gdb_required" in that directory may still be
# used, but will require that you use 'make gdb_required' in the sys
# directory to generate the new-style sys/gdb_required file.
GDB_REQUIRED := $(shell touch $(SYSROOT)/gdb_required) \
		$(shell grep -v '^\#' $(SYSROOT)/gdb_required 2>/dev/null)

#
# System independent variables
#
SHELL=	/bin/sh
ifeq ($(origin GDB_FLAG), undefined)
GDB_FLAG=
endif
HFILES= ../h
MACHINE= ../machine


# the -Wno-format switch stops gcc 2.x from checking printf format
# strings. this avoids problems with our printf extensions. gcc 1.x
# compilers will barf on this flag
#
WARN_ERROR=	-Werror
WARN_NOFORMAT=	-Wno-format 
WARN_ALL=	-Wall -Wpointer-arith -Wstrict-prototypes $(WARN_NOFORMAT)

#
# ASWITCHES and CSWITCHES are the flags that all files share. It is augmented
# by the machine specific makefiles. Error checking is added in the rules.
# We also include $(WARN_ERROR)
#
ASWITCHES=	-I$(MACHINE) -I$(HFILES) $(lib_inc) \
		$(GDB_FLAG) -nostdinc \
		$(PLATFORM_FLAGS) $(OBJECT_FLAGS) $(CFLOW_FLAGS) \
		$(GLOBAL_DEBUG) $(OFLAG) $(OTHER_CFLAGS) \
		$(WARN_ERROR) $(AOPTIONS)
CSWITCHES=	-I$(MACHINE) -I$(HFILES) $(lib_inc) \
		$(GDB_FLAG) -nostdinc \
		$(PLATFORM_FLAGS) $(OBJECT_FLAGS) \
		$(GLOBAL_DEBUG) $(OFLAG) $(OTHER_CFLAGS) \
		$(WARN_ERROR) $(COPTIONS)
DSWITCHES=	-I../machine -I../h $(lib_inc) \
		$(GDB_FLAG) -nostdinc \
		$(PLATFORM_FLAGS) $(OBJECT_FLAGS) \
		$(GLOBAL_DEBUG) $(OFLAG) $(OTHER_CFLAGS) \
		$(WARN_ERROR) $(COPTIONS)

CFLAGS=			$(CSWITCHES) $(CFLOW_FLAGS) $(COVERAGE_FLAGS)
CFLAGS_NOFLOW=		$(CSWITCHES) $(COVERAGE_FLAGS)
CFLAGS_NOCOVERAGE =	$(CSWITCHES) $(CFLOW_FLAGS)

#
# Object directories containing makefiles with production image targets.
# The production image targets must always be named "production:"
#
#
# The PRODUCTION_OBJDIRS is now divided by processor types; this when
# applied to dist rule allows us to run multiple makes at the same
# time, thus speeds things up if the build engine is powerful.
# when introducing a new processortype, please do the following:
# 1.  Create a new PROD_???_OBJDIR
# 2.  Add it to PRODUCTION_OBJDIRS
# 3.  Create  DISTLIST??? and DISTDIRS??? for PROD_???_OBJDIR
# 4.  Add dist???

PROD_4K_OBJDIRS= obj-4k-rsp obj-4k-c4000 obj-4k-rvip obj-4k-c7100 obj-4k-svip
PROD_36_OBJDIRS= obj-36-c1000 obj-36-c1005
PROD_68_OBJDIRS= obj-68-c3000 obj-68-c4000 obj-68-c7000 obj-68-alc obj-68-ags obj-68-as5200
PRODUCTION_OBJDIRS= $(PROD_4K_OBJDIRS) $(PROD_36_OBJDIRS) $(PROD_68_OBJDIRS)
#
# All the object directories in the system.
# To add an object directory that contains a production image
# target, add the directory name to PRODUCTION_OBJDIRS.
#
OBJDIRS= $(PRODUCTION_OBJDIRS) \
	obj-4k obj-68 obj-36 obj-sp-sunos obj-sp-solaris

#
# The root directory for the distribution
#
 
DISTSYMDIR = $(DISTBASE)/sym
DISTSUNDIR = $(DISTBASE)/sun
DISTBINDIR = $(DISTBASE)/bin

#
# All the object directories needed for the install
#
DISTLIST4K=      $(PROD_4K_OBJDIRS)
DISTLIST68=      $(PROD_68_OBJDIRS)
DISTLIST36=      $(PROD_36_OBJDIRS)
DISTLIST=        $(PRODUCTION_OBJDIRS)
DISTDIRS4K=      $(addsuffix .dist,$(DISTLIST4K))
DISTDIRS68=      $(addsuffix .dist,$(DISTLIST68))
DISTDIRS36=      $(addsuffix .dist,$(DISTLIST36))
DISTDIRS=        $(addsuffix .dist,$(DISTLIST))
DISTEPILOGDIRS=  $(addsuffix .distepilog,$(DISTLIST))
DISTEPILOGDIRS4K=     $(addsuffix .distepilog,$(DISTLIST4K))
DISTEPILOGDIRS68=     $(addsuffix .distepilog,$(DISTLIST68))
DISTEPILOGDIRS36=     $(addsuffix .distepilog,$(DISTLIST36))

#
# Argument for make -j flag on make production.makedist in each obj-* directory.
# Maximum number of jobs make can start at once.
#
ifeq ($(origin DISTMAXJOBS), undefined)
DISTMAXJOBS=1
endif

distclean:
ifeq ($(origin DISTBASE), undefined)
	@echo "$@: Error - need DISTBASE variable set"
	exit 1
else
	@[ -d $(DISTBASE) ] || mkdir $(DISTBASE)
	@[ -d $(DISTSYMDIR) ] || mkdir $(DISTSYMDIR)
	@[ -d $(DISTSUNDIR) ] || mkdir $(DISTSUNDIR)
	@[ -d $(DISTBINDIR) ] || mkdir $(DISTBINDIR)
	rm -f $(DISTBINDIR)/*
	rm -f $(DISTSUNDIR)/*
	rm -f $(DISTSYMDIR)/*
endif

%.dist:
ifeq ($(origin DISTBASE), undefined)
	@exit 0
else
	@echo "$@: make -j$(DISTMAXJOBS) -C $* production.makedist "
	@echo "$@:     TFTPDIR=`pwd`/$* TFTPHOST= COLON= " 
	@$(MAKE) MAKEFLAG-J=-j$(DISTMAXJOBS) -C $* production.makedist \
        TFTPDIR=`pwd`/$* TFTPHOST= COLON= ; \
	make_status=$$? ; \
	if [ $$make_status -gt 0 ]; \
	then \
	   echo "$@: Error - make failed for production.makedist"; \
	fi
endif
 
distinit: 
ifeq ($(origin DISTBASE), undefined)
	@echo "$@: Error - need DISTBASE variable set"
	exit 1
else
	@[ -d $(DISTBASE) ] || mkdir $(DISTBASE)
	@[ -d $(DISTSYMDIR) ] || mkdir $(DISTSYMDIR)
	@[ -d $(DISTSUNDIR) ] || mkdir $(DISTSUNDIR)
	@[ -d $(DISTBINDIR) ] || mkdir $(DISTBINDIR)
endif

dist:	distinit $(DISTDIRS)
dist4K:	distinit $(DISTDIRS4K)
dist68:	distinit $(DISTDIRS68)
dist36:	distinit $(DISTDIRS36)

# 
# The distepilog rule does clean up of production images after the dist rule  
# has completed. distepilog must be explicitly specified; it is not 
# called by dist. The production images are retained from the dist 
# step so that released image contents and sizes can be audited 
# programmatically in each production object directory.
#
%.distepilog: 
	@$(MAKE) -C $* production.distepilog \
	TFTPHOST= COLON= TFTPDIR=`pwd`/$*

distepilog: $(DISTEPILOGDIRS)

#######################################################################
#
# Common subsystem labels
#
# Note the alphabetical order! Keep it that way.
#
#######################################################################
APPN = 		shr_appn.o
APPNMIB =       shr_appnmib.o
APPNMIB_SHARE = shr_appnmib_share.o
APPNMIB_TN3270S = shr_appnmib_share.o
APPNUTIL =      shr_appnutil.o
APOLLO = 	shr_apollo.o
ARAP =          shr_arap.o
ARP =		shr_arp.o
ASYNC_SERIAL_HTTP = shr_async_serial_http.o
ATALK_AURP = 	shr_ataurp.o
ATALK_CORE =	shr_atalk.o shr_atdomain.o
ATALK_EIGRP =	shr_ateigrp.o
ATALK_IP =	shr_atip.o
ATALK_MIB =	shr_atalkmib.o
ATALK_TEST =	shr_atalktest.o
ATCP =          shr_atcp.o
ATM =           shr_atm.o 
ATM_ARP =       shr_atm_arp.o
ATM_DXI =       shr_atm_dxi.o
ATOMMIB_ES =	shr_atommib_es.o
ATMMIB =        shr_atmmib.o 
ATM_TEST =      shr_atmtest.o shr_tcl.o
BRIDGE_T =	shr_bridge_t.o shr_bridge_t_ui.o
BRIDGE_T_CMF =	shr_bridge_t_cmf.o
BRIDGE_T_MIB =	shr_tbmib.o
BRIDGE_SR =	shr_bridge_sr.o	shr_bridge_srb_ui.o shr_srb_sb.o
BRIDGE_SR_MIB = shr_srbmib.o
CCP =           shr_ccp.o
CDP =		shr_cdp.o
CDP_NCP =       shr_cdp_ncp.o
CDPMIB =	shr_cdpmib.o
VTP =           shr_vtp.o
CHAT =		shr_chat.o
CHASSISMIB =	shr_chassismib.o
CISCOPRO =	shr_ciscopro.o
CLNS_CORE =	shr_clns.o
CLNS_ADJ =	shr_clns_adj.o
CLS =		shr_cls.o
CLS_TEST =	shr_cls_test.o
COMMSERVER =	shr_comm.o
COMPRESS =	shr_compress.o
CONFIG_HISTORY = shr_config_history.o
CONFMANMIB =	shr_confmanmib.o
CPP = 		shr_cpp.o
CRYPTO =	shr_crypto.o
CRYPTO_CORE =	shr_crypto_core.o 
CRYPTO_MIB =	shr_crypto_mib.o
DECNET_CNV =	shr_dncnv.o
DECNET_IV =	shr_decnet.o
DECNET_MIB =	shr_decnetmib.o
DES =		shr_des.o
DHCP_CLIENT =	shr_dhcp_client.o
DIALER =	shr_dialer.o 
DLC_BASE =	shr_dlc_base.o 
DLC_SW =	shr_dlcsw.o
DLSW =		shr_dlsw.o
DLUR =          shr_dlur.o
DSPUMIB =	shr_dspumib.o
EIGRP =		shr_eigrp.o
EON =		shr_eon.o
ETHER =		shr_ether.o 
ETHERMIB =	shr_ethermib.o
EXPORT_CRYPTO =	shr_exportable_crypto.o
FLASH =		shr_flash.o
FLASHMIB =	shr_flashmib.o
FLASH_LES_MIB = shr_flash_les_mib.o
FASTSWITCH_CORE = shr_fastswitch.o
FRAME_RELAY =	shr_frame.o
FRAME_RELAY_SVC = shr_frame_svc.o
FRAME_RELAY_ARP = shr_frame_arp.o
FRAME_RELAY_TRAFFIC = shr_frame_traffic.o
FRAME_RELAY_TUNNEL = shr_frame_tunnel.o
FRAME_RELAY_HTTP = shr_http_ezsetup.o shr_fr_http.o
FRAME_RELAY_MIB = shr_frmib.o
FTP =		shr_ftp.o
GRIPROUTE =	shr_griproute.o
IBM_NETMAN =	shr_ibmnm.o
IBM_INTERFACE = shr_ibuint.o
IPC =           shr_ipc.o
IPC_UDP =       shr_ipc_udp.o
IGRP =	        shr_igrp.o
MODEMCAP = 	shr_modem_discovery.o shr_modemcap.o
RIP =		shr_rip.o
RSVP =		shr_rsvp.o
RSVP_MIB =	shr_rsvpmib.o
OSPF =		shr_ospf.o 
OSPF_MIB =	shr_ospfmib.o
BGP =		shr_bgp.o 
BGP_MIB =	shr_bgpmib.o
BSC =		shr_bsc.o 
BSC_TST =	shr_bsc_tst.o
BSTUN =		shr_bstun.o 
BSTUNMIB =	shr_bstunmib.o
BSTUN_TST =	shr_bstun_tst.o 
EGP =		shr_egp.o 
EGP_MIB =	shr_egpmib2.o 
IDENT = 	shr_ident.o
IFMIB = 	shr_ifmib.o 
IMAGEMIB =	shr_imagemib.o
IPAUTHSVCS =	shr_kerberos.o
RADIUS =	shr_radius.o
IPCORE =	shr_ipcore.o
IPCP =		shr_ipcp.o
IPUDPTCP =	shr_ipudptcp.o
IPBOOT =	shr_ipboot.o
TACACS =	shr_tacacs.o
IPAPPS =	shr_ipapps.o
IPDIAG =	shr_ipdiag.o
IPGDP =		shr_ipgdp.o
HPPROBE =	shr_hpprobe.o
IPRARP =	shr_iprarp.o
IPBOOTP =	shr_ipbootp.o
IPDNS =		shr_ipdns.o
IPACCOUNT =	shr_ipaccount.o
IPALIAS =	shr_ipalias.o
IPCOMPRS =	shr_ipcomprs.o
IPEIGRP =	shr_ipeigrp.o
IPFAST =	shr_ipfast.o
IPSERVICES1 =	shr_ipservices.o
IPHTTP =	shr_http.o
IPCDP =		shr_ipcdp.o
IPROUTE_STATIC = shr_iprouting.o
IPROUTE_STATIC_MIBS = shr_atmib2.o shr_icmpmib2.o shr_ipmib2.o shr_tcpmib2.o \
	        shr_udpmib2.o
IPMULTICAST =	shr_ipmulticast.o 
IPMULTICAST_MIBS = shr_pimmib.o shr_igmpmib.o shr_ipmroutemib.o
IPNACL =	shr_ipnacl.o
IPNAT =		shr_ipnat.o
IPNHRP =	shr_ipnhrp.o
IPSERVICES =    shr_tunnel.o shr_gre.o shr_ipip.o shr_tacacs_plus.o
IPPOLICY =	shr_ip_policy.o
IPTTCP =	shr_ipttcp.o
IPX_COMPRESSION = shr_ipxcompression.o
IPX_EIGRP =     shr_ipxeigrp.o
IPX_LITE =	shr_ipx.o
IPX_MIBS =	shr_ipxmib.o shr_ripsapmib.o shr_novellmib.o
IPX_NASI =      shr_ipxnasi.o
IPX_NLSP =      shr_ipxnlsp.o
IPX_NLSP_MIBS = shr_nlspmib.o
IPXCP =		shr_ipxcp.o
IPXNHRP =	shr_ipxnhrp.o
IPXWAN  =       shr_ipxwan.o
ISIS =		shr_isis.o
ISIS_CLNS =	shr_isis_clns.o
ISIS_IP =	shr_isis_ip.o
ISIS_NLSP_DEBUG = shr_isis_nlsp_debug.o
ISPY =		shr_ispy.o
KEYMAN =	shr_keyman.o
LACK =		shr_lack.o
LANE =          shr_lane.o
LANEMIB = 	shr_lanemib.o
LANMGR = 	shr_lanmgr.o  shr_lanmgr_ui.o
LANMGRREGSITRY = shr_lanmgrregistry.o 
LAT =		shr_lat.o
LEX =		shr_lex.o sub_lex_platform.o shr_lex_ncp.o
LLC2 =		shr_llc2.o
LPD =		shr_lpd.o
MEMPOOLMIB =	shr_mempoolmib.o
MENUS =		shr_menus.o	
MLPVT =		shr_mlpvt.o
MOP =	        shr_mop.o
NBFCP =		shr_nbfcp.o
NCIA =          shr_ncia.o shr_ncia_ui.o
NETBIOS =	shr_netbios.o shr_netbios_ui.o
NETBIOS_ACL =	shr_netbios_acl.o shr_netbios_acl_ui.o
NETBIOS_AS =    shr_netbios_as.o
NHRP =		shr_nhrp.o
NMTEST =        shr_nmtest.o
NTP_CORE =	shr_ntp.o
NTP_REFCLOCK =	shr_ntp_refclock.o
NTP_REFCLOCK_MASTER = shr_ntp_refclock_master.o
NTP_REFCLOCK_PPS = shr_ntp_refclock_pps.o
NTP_REFCLOCK_TELSOL = shr_ntp_refclock_telsol.o
NTP_REFCLOCK_TRIMBLE = shr_ntp_refclock_trimble.o
OPT_DSPU =	shr_dspu_ui.o
OPT_RSRB =	shr_bridge_rsrb_ui.o
OPT_TRLANE =	shr_trlane.o
OPT_SERIAL =	shr_stun_ui.o shr_sdlc_ui.o
PAD =		shr_pad.o
PAKGEN =	shr_pakgen.o
PGEN =		shr_pgen.o
PPP =		shr_ppp.o
PT =		shr_pt.o
PT_AUTO =	shr_pt_auto.o
PT_LAT =	shr_pt_lat.o
PT_PAD =	shr_pt_pad.o
PT_SLIP_PPP =	shr_pt_slip_ppp.o
PT_TCP =	shr_pt_tcp.o
SMRP_CORE =	shr_smrp.o shr_atsmrp.o
SMRP_TEST =	shr_smrptest.o
SNA_PU =	shr_sna_pu.o
PT_LATPAD =	shr_pt_latpad.o
PT_PADTCP =	shr_pt_padtcp.o
PT_TCPLAT =	shr_pt_tcplat.o
PT_PADSLIP =	shr_pt_padslip.o
PT_LATSLIP =	shr_pt_latslip.o
PT_TCPSLIP =	shr_pt_tcpslip.o
PT_LATAUTO =	shr_pt_latauto.o
PT_PADAUTO =	shr_pt_padauto.o
PT_TCPAUTO =	shr_pt_tcpauto.o
QLLC =		shr_qllc.o
QUEUEMIB =	shr_queuemib.o
REM =		shr_rem.o
RMON =		shr_rmon.o
RMONLITE =	shr_rmonlite.o
RSRBMIB =       shr_rsrbmib.o
RTTMON =	shr_rtt_mon.o
RTTMONMIB =	shr_rtt_monmib.o
RTT_DSPU =	shr_rtt_dspu.o
RTT_SNANM =	shr_rtt_snanm.o
CDLSWMIB =      shr_cdlswmib.o
ROUTING =	shr_routing.o
RS232MIB =	shr_rs232mib.o
SDLC = 		shr_sdlc.o shr_snasdlcmib.o
SDLLC = 	shr_sdllc.o
SDLLCMIB = 	shr_sdllcmib.o
SERIAL =	shr_serial.o
SMDS =		shr_smds.o 
SNA_PRIORITY =	shr_sna.o 
SNANM = 	shr_snanm.o 
SNAPSHOT =	shr_snapshot.o 
SNAPSHOTMIB =	shr_snapshotmib.o 
SNMP =		shr_snmp.o shr_entity.o $(CONFIG_HISTORY)
SNTP =		shr_sntp.o
SRBCORE =	shr_srbcore.o
SRB_SB =	shr_srb_sb.o
SRSWITCH =      shr_srswitch.o 
STUN = 		shr_stun.o
STUNMIB =	shr_stunmib.o
SYSLOG_HISTORY = shr_syslog_history.o
SYSLOGMIB =	shr_syslogmib.o
TARP =          shr_tarp.o
TCL =		shr_tcl.o
TN3270 =	shr_tn3270.o
TRAFFIC_SHAPE = shr_traffic_shape.o
TRING =		shr_tring.o 
TRINGMIB =	shr_trmib.o
TSMIB =		shr_tsmib.o
V120 =		shr_v120.o
VDLC =		shr_vdlc.o
VINES_CORE =	shr_vines.o
VINES_MIB =	shr_vinesmib.o
VINES_TEST =	shr_vinestest.o
VLANS =		shr_vlan.o $(FAST_VLANS)
VPN =           shr_vpn.o
VTEMPLATE = 	shr_vtemplate.o
X25 =		shr_x25.o
X25MIBS =	shr_x25mib.o shr_lapbmib.o
XNS_CORE =	shr_xns.o
XNS_MIB =	shr_xnsmib.o
XREMOTE = 	shr_xremote.o
VTPMIB =        shr_vtpmib.o
VTPPRUNE =      shr_vtp_prune.o

#######################################################################
#
# These variables document the base level of MIB support in all
# images which have SNMP support.  
#
# These definitions are modified by some platforms with hw-specific mibs.
# 
#######################################################################
MIBS =     $(FLASHMIB) $(IPROUTE_STATIC_MIBS) $(IMAGEMIB) \
           $(IFMIB) $(CONFMANMIB) $(RS232MIB) $(TSMIB) \
           $(MEMPOOLMIB) $(CHASSISMIB) $(CDPMIB) $(RMONLITE)

#######################################################################
#
# Local subsystem and library labels
#
#######################################################################

CALLMIB =	sub_callmib.o
CHANNELMIB =	sub_channelmib.o
DS1MIB =	sub_ds1mib.o
FDDI_SMT =      sub_fddi_smt.o
FDDIMIB =	sub_fddismt73mib.o
FSIPMIB =	sub_fsipmib.o
ISDN =		sub_isdn.o
ISDN_HTTP =	shr_http_ezsetup.o sub_isdn_http.o
ISDNMIB =	sub_isdnmib.o

#
# Platforms get sched-interval support by default.
# However on certain boxes like Predator, MARs, svip and rsp
# an alternative technique called schedular allocation
# is used for throttling. On these boxes, this variable is
# overriden in the platform specific makefiles
#
SCHED_INTERVAL =  sched_intval.o
UTIL =		$(CFLOW_SYSTEM) shl_util.a shl_ibmutil.a $(lib_shl)
NUTIL =         shl_util.a $(lib_shl)

OS_CORE =	shr_core.o sub_core_platform.o $(SCHED_INTERVAL)
OS_UKERNEL =	shr_ukernel.o sub_ukernel_platform.o

KERNEL =	$(OS_CORE) $(OS_UKERNEL)


#######################################################################
#
# The operating system.
#
# The following glop constitutes a minimal system, which is too large.
# We will be fixing this.
#
########################################################################

GDB_CORE =	gdbcore.o gdbproto.o remcompress.o $(GDBCPU)
GDB_UI =	gdb_chain.o gdb_process.o exception.o 

sub_ukernel =	$(CPUASM) stacks_$(FAMILY_PREFIX).o $(UKERNEL_CPU) \
		chunk.o clock.o clock_guts.o clock_util_core.o free.o \
		libgcc_math.o list.o mgd_timers.o monitor1.o msg_subsys.o \
		msg_system.o msg_sched.o msgtxt_sys.o passive_timers.o \
		pool.o process.o profile_kernel.o queue.o random.o \
		region.o registry.o sched_compatibility.o stacks.o \
		subsys.o location.o

sub_ukernel_platform = $(GDB_CORE) \
		$(PLATFORM_ASM) $(PLATFORM_TIMER) $(PLATFORM_OTHER) \
		$(PLATFORM_MONITOR) \
		platform.o sched.o signal.o sum.o 

os_core =	aaa.o aaa_acct.o msg_aaa.o access_common.o access.o \
		address.o async.o async_debug.o \
		boot.o clock_util.o compress_lzw.o config_compress.o \
		connect.o coverage_analyze.o hostname.o if_groups.o \
		if_vidb.o init.o interface_api.o linkdown_event.o \
		logger.o login.o loopback.o modemsupport.o msg_clear.o \
		msg_lineproto.o name.o network_debug.o nv_common.o \
		old_timer_callbacks.o os_debug.o pak_api.o printf.o \
		priority.o profile.o reload.o service.o sr_core.o \
		time_utils.o tty_null.o ttycon.o ttysrv.o ttystatem.o \
		util.o techsupport.o bitlogic.o

os_core_platform = $(GDB_UI) buffers.o buffers_debug.o element.o \
		interface.o network.o particle.o

os_parser_lite = msg_parser.o parser.o parser_debug.o \
		parser_modes.o parser_print.o parser_util.o

os_parser =	$(os_parser_lite) parser_alias.o parser_privilege.o

os_parser_lite_platform = parser_actions.o

os_exec =	chain.o command1.o command2.o command_chain.o config.o \
		debug.o exec.o exec_chain.o setup.o trace.o \
		common_strings.o

os_chain =	free_chain.o registry_chain.o chunk_chain.o \
		region_chain.o sched_chain.o list_chain.o subsys_chain.o \
		async_chain.o access_chain.o connect_chain.o logger_chain.o \
		login_chain.o old_timer_callbacks_chain.o clock_chain.o \
		priority_chain.o aaa_chain.o stacks_chain.o $(UKERNEL_UI_CPU)

os_chain_platform = buffers_chain.o $(UKERNEL_UI_PLATFORM) \
		stacks_$(FAMILY_PREFIX)_chain.o

os_media =	ieee.o msg_datalink.o static_map.o static_map_chain.o

os_registry =	partner_registry.o snanm_registry.o flash_registry.o \
		media_registry.o auth_registry.o cs_registry.o

sub_core =	$(os_core) $(os_parser) $(os_exec) $(os_media) \
		$(os_chain) $(os_registry)

sub_core_platform = $(os_core_platform) $(os_chain_platform) \
		$(os_parser_lite_platform)

sub_schedtest = sched_test.o msg_schedtest.o

####################

sub_ds1mib =	sr_ds1mib.o sr_ds1miboid.o

sub_channelmib = sr_channelmib.o sr_channelmiboid.o \
                 sr_tcpoffloadmib.o sr_tcpoffloadmiboid.o \
                 sr_ciptcpipmib.o sr_ciptcpipmiboid.o \
                 sr_ciplanmib.o sr_ciplanmiboid.o \
                 sr_snallcmib.o sr_snallcmiboid.o \
                 sr_cipcsnamib.o sr_cipcsnamiboid.o \
                 channelmib_utils.o

sub_tn3270smib = sr_tn3270servermib.o sr_tn3270servermiboid.o \
                 sr_snanaumib.o sr_snanaumiboid.o \
                 sr_cipappnmib.o sr_cipdlurmib.o

sub_fddi_smt =  csp.o fbmglbl.o fbmq.o mibget.o cspcfm.o cspproc.o \
                fbminit.o fbmraf.o mibglbl.o cspecm.o csprmt.o fbmmsg.o \
                fbmrcv.o mibinit.o cspglbl.o cspsig.o fbmnn.o fbmsrp.o \
                mibmsg.o cspinit.o csptimer.o fbmparam.o fbmsup.o mibproc.o \
                csplem.o fbm.o fbmpmf.o fbmtimer.o mibsup.o cspmsg.o fbmesf.o \
                fbmpmfp.o mib.o mibtable.o fbmframe.o fbmproc.o \
                mibchng.o mibxdi.o if_xdi.o

sub_fddismt73mib = sr_newfddimib.o sr_newfddimiboid.o

sub_fsipmib =	sr_fsipmib.o sr_fsipmiboid.o

sub_isdn =	cc_chan.o ccb5ess.o ccbdms.o ccbntkd.o ccbri.o ccie.o ccif.o \
		isdnexec.o execuser.o gp.o inchrw.o l2.o l2_sdl.o l2if.o l3.o \
		l3_sdl_u.o l3if.o lif.o me.o meif.o memim.o \
		ccb1tr6.o ccbvn.o isdnintrfc.o isdn.o $(CPUISDN) \
		ccbfit.o ccbni1.o isdn_chain.o isdn_setup.o isdn_debug.o \
		ccbnet3.o ccbts013.o msg_isdn.o

sub_isdnmib =   sr_isdnmib.o sr_isdnmiboid.o 

sub_callmib =   sr_callmib.o sr_callmiboid.o 

sub_sdu_cs =	csdata.o csdata2.o csutil2.o csutil1.o connstup.o authcy.o \
		cy_api.o ep_frm.o

libutil =	range.o tree.o fsm.o regexp_access.o regexp.o regsub.o \
		iso_chksum.o access_expr.o msg_util.o avl.o wavl.o \
		md5.o md5_crypt.o qsort.o sorted_array.o rif_util.o \
		itemlist.o radix.o msg_radix.o parse_util.o \
		random_fill.o latgroup.o md5_rfc1828.o inet_aton.o

sub_isdn_http =	isdn_http.o

include $(SYSROOT)/lib/makefile

libibmutil =	peer_util.o sna_util.o

sub_core.o		:	$(sub_core)
sub_core_platform.o	:	$(sub_core_platform)
sub_ukernel.o		:	$(sub_ukernel)
sub_ukernel_platform.o	:	$(sub_ukernel_platform)
sub_schedtest.o		:	$(sub_schedtest)
sub_channelmib.o	:	$(sub_channelmib)
sub_tn3270smib.o	:	$(sub_tn3270smib)
sub_ds1mib.o		:	$(sub_ds1mib)
sub_fddi_smt.o		:	$(sub_fddi_smt)
sub_fddismt73mib.o	:	$(sub_fddismt73mib)
sub_fsipmib.o		:	$(sub_fsipmib)
sub_isdn.o		:	$(sub_isdn)
sub_isdnmib.o		:	$(sub_isdnmib)
sub_callmib.o		:	$(sub_callmib)
libutil.a		:	$(libutil)
libibmutil.a		:	$(libibmutil)
libsmutil.a		:	$(libsmutil)
sub_isdn_http.o		:	$(sub_isdn_http)

#
# Compute tags at the top level, compute dependencies down one level.
#
# a hacked make will set MAKEFLAG-J, but it can also be done manually
#
# now that some snmp mib processing files are automaticially generated,
# the process of calculating dependancies is a bit trickier.  We must
# first generate the mib generator dependancies, then we must generate
# the mib processing source files, and then finally we can compute the
# source code dependancies (including mib source file dependancies)
#
# This is even more complex now.  The first thing generated must be a
# list of all dependances that will be included because of the defines
# passed to gcc by each makefile.  These files are then included by
# each makefile, and thus have to be present before any other step.
#
mibs:
	@$(MAKE) $(MAKEFLAG-J) -C obj-68-c7000 -f ../makemibs \
						--no-print-directory depend
	@$(MAKE) $(MAKEFLAG-J) -C obj-68-c7000 -f ../makemibs \
						--no-print-directory mibfiles
monlib:
	@$(MAKE) $(MAKEFLAG-J) -C obj-4k-rsp -f makefile \
			--no-print-directory monlib_depend
	@$(MAKE) $(MAKEFLAG-J) -C obj-4k-c7100 -f makefile \
			--no-print-directory monlib_depend

alert:
	@$(MAKE) $(MAKEFLAG-J) -C snanm -f makealert all

as_fm_dependancies:
	$(MAKE) -C ../sys/firmware/as/t1 dependancies;

dependancies.global:
	@echo Computing platform include summary
	@$(MAKE) -C obj-68-c7000 -f ../depend.make \
		--no-print-directory dependancies.global

# Executed with the environment of an obj-* makefile.
onedir_includes:
	@$(MAKE) -f ../depend.make --no-print-directory CC="$(CC)" \
		CFLAGS="$(DSWITCHES)" INCLUDE_FILE="../$(MAKEFILE_NAME)" \
		dependancies.specific

# Keep make happy when running 'make nodepend' when this file doesn't exist
dependancies.specific:
	
# Invoke each obj-* makefile and have it compute its normal includes.
objdir_includes: dependancies.global
	@echo Computing per directory platform includes
	@for dir in $(OBJDIRS); do \
	    echo "include .DD.*" > $$dir/dependancies.specific; \
	    touch $$dir/.DD.depend_seed; \
	done

depend_start:
	@touch $(SYSROOT)/dependancies	# prevents complaints
	@touch $(SYSROOT)/dependancies.seed # prevents complaints

dependancies: depend_start objdir_includes mibs monlib alert as_fm_dependancies all_derived_source
	@rm -f obj*/vip-version.*
	@rm -f obj*/cisco-zip.*
	@$(MAKE) $(MAKEFLAG-J) -C obj-68-c7000 -f ../depend.make \
						--no-print-directory depend
	@$(MAKE) $(MAKEFLAG-J) -C obj-68-c7000 ../hes/sseasm.h ../hes/ssesym.h \
						--no-print-directory

ifeq ($(origin FLATTEN), undefined)
FLATTEN :=      ../scripts/flatten_dependancies.perl
endif

view_depend: FORCE
	@echo creating viewable dependancies file: $(VIEWFILE)
	@rm -f $(VIEWFILE)
	@(for i in .DD.* ; do \
	    cat $$i | sed 's|$(SYSROOT)|\.\.|g' | $(FLATTEN) >> $(VIEWFILE); \
	done)

as_fm_tagsid:
	$(MAKE) -C ../sys/firmware/as/t1 tags_id;

depend: dependancies as_fm_tagsid
	@$(MAKE) $(MAKEFLAG-J) -f tags-id.make

depend_seed:
	@$(MAKE) $(MAKEFLAG-J) -C obj-68-c7000 -f ../depend.make depend_seed

gdb_required: force
	find $(SYSROOT) -name .gdb_required -print -follow | \
	perl -pane 's,/[^/]+\n,\n,;s,^$(SYSROOT)/,../,;' > $(SYSROOT)/$@.new
	mv $(SYSROOT)/$@ $(SYSROOT)/$@.old; mv $(SYSROOT)/$@.new $(SYSROOT)/$@

force:

dependencies: dependancies

tags:
	@$(MAKE) $(MAKEFLAG-J) -f tags-id.make TAGS

id:
	@$(MAKE) $(MAKEFLAG-J) -f tags-id.make id

#
# Rule to tell the mib compiler to generate user-modifiable code 
#

%.code:
	@$(MAKE) $(MAKEFLAG-J) -C obj-68-c7000 -f ../makemibs \
						--no-print-directory $@

#
# Rules to rebuild the files in a subdirectory
#

%.modular:
	@cd $(basename $@) ; \
	echo "Executing $(MAKE) modular in $(basename $@)" ;\
	$(MAKE) -k modular ;

%.modularity_check:
	@cd $(basename $@) ; \
	echo "Executing $(MAKE) modularity_check in $(basename $@)" ;\
	$(MAKE) -k modularity_check ;

%.production: as_firmware
	@cd $(basename $@) ; \
	echo "Executing $(MAKE) production images in $(basename $@)" ;\
	$(MAKE) production ;

%.crypto:
	@cd $(basename $@) ; \
	echo "Executing $(MAKE) crypto images in $(basename $@)" ;\
	$(MAKE) crypto ;

%.oddballs:
	@cd $(basename $@) ; \
	echo "Executing $(MAKE) oddballs suite in $(basename $@)" ;\
	$(MAKE) oddballs

ODDBALL_TARGETS = $(addsuffix .oddballs,$(PRODUCTION_OBJDIRS))
MODULAR_TARGETS = $(addsuffix .modular,$(PRODUCTION_OBJDIRS))
# Modularity check targets do not include objects built with the 4k tool
# chain because weak symbols in the 4k symbols region appear unresolved
# to the modularity check tools, when they are in fact resolved.
MODULARITY_CHECK_TARGETS = $(addsuffix .modularity_check,\
                           $(filter-out obj-4k-%,$(PRODUCTION_OBJDIRS)))
PRODUCTION_TARGETS = $(addsuffix .production,$(PRODUCTION_OBJDIRS))
CRYPTO_TARGETS = $(addsuffix .crypto,$(PRODUCTION_OBJDIRS))

modular.all:	$(MODULAR_TARGETS)
modularity_check.all:   $(MODULARITY_CHECK_TARGETS)
production.all:	$(PRODUCTION_TARGETS)
crypto.all:	$(CRYPTO_TARGETS)
oddballs.all:	$(ODDBALL_TARGETS)
everything.all:	production.all oddballs.all modular.all

%.production.test: as_firmware
	@cd $(basename $*) ; \
	echo "Executing $(MAKE) production images in $(basename $*)" ;\
	$(MAKE) production.test

%.crypto.test:
	@cd $(basename $*) ; \
	echo "Executing $(MAKE) crypto images in $(basename $*)" ;\
	$(MAKE) crypto.test

%.oddballs.test:
	@cd $(basename $*) ; \
	echo "Executing $(MAKE) oddballs suite in $(basename $*)" ;\
	$(MAKE) oddballs.test

as_firmware:
	$(MAKE) -C ../sys/firmware/as/t1;

ODDBALL_TEST_TARGETS = $(addsuffix .oddballs.test,$(PRODUCTION_OBJDIRS))
PRODUCTION_TEST_TARGETS = $(addsuffix .production.test,$(PRODUCTION_OBJDIRS))

modular.testall:	modular.all
modularity_check.testall:       modularity_check.all
production.testall:	$(PRODUCTION_TEST_TARGETS)
crypto.testall:		$(CRYTPO_TEST_TARGETS)
oddballs.testall:	$(ODDBALL_TEST_TARGETS)
everything.testall:	production.testall crypto.testall oddballs.testall \
			modular.testall

include $(SYSROOT)/make.gaia

help:		
	@echo "	" ;\
	echo "From the sys directory you may now make:" ;\
	echo "    production.all       - builds production images" ;\
	echo "    crypto.all	       - builds production images" ;\
	echo "    modular.all          - compiles the modular suite" ;\
	echo "    modularity_check.all - check the modular suite" ;\
	echo "    oddballs.all         - builds engineering specials" ;\
	echo "    everything.all       - builds all of the above" ;\
	echo "	" ; \
	echo "    production.testall   - test builds production images" ;\
	echo "    modular.testall      - test compiles the modular suite" ;\
	echo "    features.all      - compiles the gaia suite" ;\
	echo "    oddballs.testall     - test builds engineering specials" ;\
	echo " or everything.testall   - test builds all of the above" ;\
	echo "	" ; \
	echo "The test builds will remove the targets in order to conserve " ;\
	echo "space. " ;\
	echo "	" ; \
	echo "In a similar manner, from the platform directories, you may ";\
	echo "make: production, production.test, oddballs, oddballs.test, ";\
	echo "modular, modular.test, everything, or everything.test.";\
	echo "	" ; \
	echo "Also, you might find sys/Doc/Makefile_Primer interesting.";\
	echo "	" ; \

everything.trunkd: help
trunkd.test: help


lock: 
	@$(SYSROOT)/lock

nodepend:
	@echo 'removing all *.o shl_*.a lib*.a and .DD.* files'
	@for dir in $(OBJDIRS); do \
	    rm -f $$dir/*.o $$dir/shl_*.a $$dir/lib*.a $$dir/.DD.*; \
	    rm -f $$dir/dependancies.specific; \
	done
	@find . -name \.\[DV\]\.\* -follow -print | xargs -l /bin/rm
	@$(MAKE) -f ../makemibs -C obj-68-c7000 --no-print-directory $@
	@rm -f dependancies dependancies.global dependancies.seed

cleanfw:
	$(MAKE) -C ../sys/firmware/as/t1 clean;

cleanfwr:
	$(MAKE) -C ../firmware/as/t1 clean;

cleanobj:
	@currentDirectory=`pwd | grep '\<obj'`; \
	files1="*.o *.a *.lis *.imp *.exp *.bin *.sun *.symbols *.temp xlate"; \
	files2="*.bb *.bbg *.da *.gcov *.lock cisco-*.c Main_*c *-*"; \
	files3="*.binsize dependancies.specific .DD.* $(VIEWFILE)" ; \
	if [ "$$currentDirectory" = "" ]; \
	then \
	    echo this make option should be run ONLY in the object directories; \
	    exit 1; \
	else \
	    echo "rm -f $$files1"; \
	    rm -f $$files1; \
	    echo "rm -f $$files2"; \
	    rm -f $$files2; \
	    echo "rm -f $$files3"; \
	    rm -f $$files3; \
	fi;

cleansrc:
	rm -f *.import *.extern *.reg.lock *.regc *.regh *.rpcc *.rpch *.alth .reloc .relocbin

clean:	cleanobj cleanfwr

realclean: cleansrc
	@find $(SYSROOT) -name \.\[DT\]\.\* -print -follow | xargs -l rm
	rm -f */*.import
	rm -f */*.extern
	rm -f */*.reg.lock
	rm -f */*.regc
	rm -f */*.regh
	rm -f */*.rpcc
	rm -f */*.rpch
	rm -f */*.stamp */*.def
	rm -f */*.alth
	rm -f */*.reloc
	rm -f */*.relocbin
	rm -f $(SYSROOT)/h/interface_types.h

superclean: realclean cleanfw
	@for dir in $(OBJDIRS); \
	    do echo "Cleaning $$dir"; \
	    $(MAKE) -w -C $$dir --no-print-directory cleanobj; \
	done
	$(MAKE) -C $(SYSROOT) -f makefile.tools cleanprogs
	rm -f TAGS tags ID dependancies dependancies.global

msg_manual:
	@scripts/msgdef_search.perl

msg_errors:
	@scripts/msgdef_search.perl -e

msg_text:
	@scripts/msgdef_search.perl -i

lemonade:
	@echo -n yummy 

love:
	@echo "Not war?"

#
# Compile some routines with more warnings and with prototypes.
#

REMAKE=$(MAKE)

SFLAGS= $(subst NONE,,\
	$(subst w,-DNO_WAN ,\
	$(subst l,-DLAT_ENABLE ,\
	$(subst x,-DX25_ENABLE -DSMDS_ENABLE ,\
	$(subst b,-DBRIDGE_ENABLE ,\
	$(subst s,-DSRSWITCH_ENABLE , \
	$(subst r,-DROM_ENABLE ,\
	$(subst c1, -DCISCOPRO_IMAGE=1 ,\
	$(subst c2, -DCISCOPRO_IMAGE=2 ,\
	$(subst c3, -DCISCOPRO_IMAGE=3 ,\
	$(subst SMALLTS,-DSMALLTS ,\
	$(subst PGEN,-DPGEN ,\
	$(subst BOOT,-DBOOT ,$(SWFLAGS)\
	)))))))))))))

main_$(HWFLAGS).o:
	$(QUIET_CC)touch main_$(HWFLAGS).o

Main_$(OBJNAME).o: main_$(HWFLAGS).o
	$(QUIET_COM)echo "Compiling $@ with flags:"
	$(QUIET_COM)@echo "$(SFLAGS)"
	$(QUIET_CC)$(CC) $(CFLAGS) -DPRODUCTION $(SFLAGS) \
		-c $(SYSROOT)/os/main_$(HWFLAGS).c -o $@

$(SYSROOT)/version:
	@echo 99 > $(SYSROOT)/version
 
VIEW = $(shell viewname)
STAMP = $(shell datestamp)
 
SYSREVA= $(shell $(SYSROOT)/scripts/getversion SYS_MAJORVERSION < $(SYSROOT)/h/master.h)
SYSREVB= $(shell $(SYSROOT)/scripts/getversion SYS_MINORVERSION < $(SYSROOT)/h/master.h)
SYSREV= $(SYSREVA).$(SYSREVB)\($(STAMP)\)
 
ROMREVA= $(shell $(SYSROOT)/scripts/getversion ROM_MAJORVERSION < $(SYSROOT)/rommon/src/mon_defs.h)
ROMREVB= $(shell $(SYSROOT)/scripts/getversion ROM_MINORVERSION < $(SYSROOT)/rommon/src/mon_defs.h)
ROMREV= $(ROMREVA.$(ROMREVB)\($(STAMP)\)"
 
$(SYSROOT)/cisco.sh: $(SYSROOT)/cisco.SH $(SYSROOT)/h/master.h
	(cd $(SYSROOT); sh ./cisco.SH $(STAMP) $(SYSREV) $(SYSREV) $(VIEW))
 
$(SYSROOT)/symbols.sh: $(SYSROOT)/symbols.SH $(SYSROOT)/h/master.h
	(cd $(SYSROOT); sh ./symbols.SH $(STAMP) $(SYSREV) $(SYSREV) $(VIEW))
 
$(SYSROOT)/rommon/utils/banner.sh: $(SYSROOT)/rommon/utils/banner.SH $(SYSROOT)/rommon/src/mon_defs.h
	(cd $(SYSROOT)/rommon/utils; sh banner.SH $(STAMP) $(ROMREV) $(SYSREV) $(VIEW))
$(SYSROOT)/rommon/utils/symbols_elf.sh: $(SYSROOT)/rommon/utils/symbols_elf.SH $(SYSROOT)/rommon/src/mon_defs.h
	(cd $(SYSROOT)/rommon/utils; sh symbols_elf.SH $(ROMREV) $(VIEW))

main: Main_$(OBJNAME).o $(SYSROOT)/version $(SYSROOT)/cisco.sh $(SYSROOT)/symbols.sh $(SYSROOT)/rommon/utils/symbols_elf.sh $(SYSROOT)/rommon/utils/banner.sh

# was will build both ../hes/ssesym.h and output ../hes/sseasm.h
# gnumake has problems with a single command generating two targets
# hence the following two rules are a bit odd looking
../hes/ssesym.h: ../hes/sseasm.w
	$(CC) -traditional-cpp -DUNIX -E -x assembler-with-cpp $< |\
	was > ../hes/sseasm.h

../hes/sseasm.h: ../hes/sseasm.w
	$(CC) -traditional-cpp -DUNIX -E -x assembler-with-cpp $< |\
	was > ../hes/sseasm.h

# temporary thing while interface_types is being constructed:
$(SYSROOT)/h/interface_types.h: $(SYSROOT)/h/interface_private.h
	$(SYSROOT)/scripts/gen_if_types

# DO NOT DELETE THIS LINE -- make depend depends on it.
