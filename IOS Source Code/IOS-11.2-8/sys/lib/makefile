# $Id: makefile,v 3.1.10.1 1996/03/18 20:47:53 gstovall Exp $
# $Source: /release/111/cvs/Xsys/lib/makefile,v $
#------------------------------------------------------------------
# makefile -- lib makefile definitions
#
# March 1996, Mark D. Baushke
#
# Copyright (c) 1996 by cisco Systems, Inc.
# All rights reserved.
#------------------------------------------------------------------
# $Log: makefile,v $
# Revision 3.1.10.1  1996/03/18  20:47:53  gstovall
# Branch: California_branch
# Elvis has left the building.  He headed out to California, and took the
# port ready changes with him.
#
# Revision 3.1.2.1  1996/03/07  08:18:36  mdb
# Branch: DeadKingOnAThrone_branch
# cisco and ANSI/POSIX libraries.
#
# Revision 3.1  1996/03/05  00:13:31  mdb
# placeholder for toplevel library makefile
#
#------------------------------------------------------------------
# $Endlog$
#

include $(SYSROOT)/lib/ansi/makefile
include $(SYSROOT)/lib/cisco/makefile

lib_shl = $(libcisco_shl) $(libansi_shl) 
lib_all = $(libcisco) $(libansi) 
lib_inc = $(libcisco_inc) $(libansi_inc)

lib_realclean:
	@echo "nuking .T.* and .D.* files from the ansi library sources"
	@find $(SYSROOT)/lib -name \.\[DT\]\.\* -print | xargs /bin/rm
