# $Id: makefile,v 3.1.10.1 1996/03/18 20:48:03 gstovall Exp $
# $Source: /release/111/cvs/Xsys/lib/ansi/makefile,v $
#------------------------------------------------------------------
# makefile -- Definitions of the ansi library objects
#
# March 1996, Mark D. Baushke
#
# Copyright (c) 1996 by cisco Systems, Inc.
# All rights reserved.
#------------------------------------------------------------------
# $Log: makefile,v $
# Revision 3.1.10.1  1996/03/18  20:48:03  gstovall
# Branch: California_branch
# Elvis has left the building.  He headed out to California, and took the
# port ready changes with him.
#
# Revision 3.1.2.1  1996/03/07  08:18:46  mdb
# Branch: DeadKingOnAThrone_branch
# cisco and ANSI/POSIX libraries.
#
# Revision 3.1  1996/03/05  00:15:58  mdb
# placeholder for the ansi library makefile
#
#------------------------------------------------------------------
# $Endlog$
#

libansi_incdir = $(SYSROOT)/lib/ansi/include
libansi_inc    = -I$(libansi_incdir)

libansi_ctype  = isalnum.o isalpha.o isascii.o iscntrl.o isdigit.o \
                 isgraph.o islower.o isprint.o ispunct.o isspace.o \
                 isupper.o isxdigit.o toascii.o tolower.o _tolower.o \
                 toupper.o _toupper.o ctype.o

libansi_reent  = errno.o reent_init.o

libansi_stdlib = abs.o atoi.o atol.o calloc.o div.o labs.o ldiv.o \
                 strtol.o strtoul.o wctomb.o

libansi_string = memchr.o memcmp.o memcpy.o memset.o memmove.o \
                 strcat.o strchr.o strcmp.o strcoll.o strcpy.o \
                 strcspn.o strerror.o strlen.o strncat.o strncmp.o \
                 strncpy.o strpbrk.o strrchr.o strspn.o strstr.o \
                 strtok.o

libansi        = $(libansi_ctype) $(libansi_reent) $(libansi_stdlib) \
		 $(libansi_string)

libansi.a      : $(libansi)
libansi_shl    = shl_ansi.a
