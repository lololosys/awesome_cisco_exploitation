# $Id: makefile,v 3.1.10.1 1996/03/18 20:49:57 gstovall Exp $
# $Source: /release/111/cvs/Xsys/lib/cisco/makefile,v $
#------------------------------------------------------------------
# makefile -- Definitions of the cisco library objects
#
# March 1996, Mark D. Baushke
#
# Copyright (c) 1996-1997 by cisco Systems, Inc.
# All rights reserved.
#------------------------------------------------------------------
# $Log: makefile,v $
# Revision 3.1.10.1  1996/03/18  20:49:57  gstovall
# Branch: California_branch
# Elvis has left the building.  He headed out to California, and took the
# port ready changes with him.
#
# Revision 3.1.2.2  1996/03/09  05:28:46  hampton
# Slim down the kernel of the router.
# Branch: DeadKingOnAThrone_branch
#
# Revision 3.1.2.1  1996/03/07  08:20:03  mdb
# Branch: DeadKingOnAThrone_branch
# cisco and ANSI/POSIX libraries.
#
# Revision 3.1  1996/03/05  00:17:25  mdb
# placeholder for the cisco library makefile
#
#------------------------------------------------------------------
# $Endlog$
#

libcisco_dirinc = $(SYSROOT)/lib/cisco/include
libcisco_inc    = -I$(libcisco_dirinc)

libcisco        = bcmp_$(LIBRARY_PREFIX).o bcopy_$(LIBRARY_PREFIX).o \
                  bscopy_$(LIBRARY_PREFIX).o \
                  atoo.o atohex.o badbcd.o bcdtochar.o bit_swaps.o \
                  bzero.o chartobcd.o chartohex.o cmpid.o concat.o \
		  deblank.o firstbit.o firstrbit.o tohexchar.o \
		  is_string.o lcmp.o lowercase.o num_bits.o null.o \
		  remove_ws.o sstrncat.o sstrncpy.o strcasecmp.o \
                  strncasecmp.o string_getnext.o sys_ebcdic_to_ascii.o \
                  term_char.o uppercase.o wcopy.o

libcisco.a      : $(libcisco)
libcisco_shl    = shl_cisco.a
