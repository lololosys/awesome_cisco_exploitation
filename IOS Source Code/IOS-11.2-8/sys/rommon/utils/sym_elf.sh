# $Id: sym_elf.sh,v 3.2 1995/11/17 18:48:47 hampton Exp $
# $Source: /swtal/cherf.111/ios/sys/rommon/utils/sym_elf.sh,v $
#------------------------------------------------------------------
# sym_elf.sh
#
# January 1994, Michael Beesley
#
# Copyright (c) 1994 by cisco Systems, Inc.
# All rights reserved.
# 
# Symbol table utiltiy script for ELF files.
#------------------------------------------------------------------
# $Log: sym_elf.sh,v $
# Revision 3.2  1995/11/17  18:48:47  hampton
# Remove old entries from the RCS header logs.
#
# Revision 3.1  1995/11/09  13:10:12  shaker
# Bump version numbers from 2.x to 3.x.
#
# Revision 2.1  1995/06/07  22:42:38  hampton
# Bump version numbers from 1.x to 2.x.
#
#------------------------------------------------------------------
# $Endlog$
#

echo "\$\$ DATA" 
nm.94q2.mips64 -B -p $1 | awk -e '{ print "  " $3 " " $1 }' | egrep -v "   U"
