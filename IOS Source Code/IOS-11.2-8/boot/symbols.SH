#!/bin/sh
# $Id: symbols.SH,v 3.2 1995/11/17 07:41:58 hampton Exp $
# $Source: /swtal/cherf.111/ios/boot/symbols.SH,v $
#------------------------------------------------------------------
# symbols.SH is run by prep_boot_tree to generate symbols.sh
#
# June 1995, Mark D. Baushke
#
# Copyright (c) 1995 by cisco Systems, Inc.
# All rights reserved.
#------------------------------------------------------------------
# $Log: symbols.SH,v $
# Revision 3.2  1995/11/17  07:41:58  hampton
# Remove old entries from the RCS header logs.
#
# Revision 3.1  1995/11/09  09:05:01  shaker
# Bump version numbers from 2.x to 3.x.
#
# Revision 2.1  1995/06/07  19:12:53  hampton
# Bump version numbers from 1.x to 2.x.
#
#------------------------------------------------------------------
# $Endlog$
#

v="$1"
trunk_rev="$2"
copy_name="$3"

rm -f symbols.sh
cat > symbols.sh <<@@E_O_F
#!/bin/sh
v=\`cat ../version\`
v=\`expr \$v\`
nm.95q1.68k -n \$1 | pr -2 -f -l60 -w79 -h "--  \$3 Monitor Symbol Table, Version $trunk_rev [$copy_name \$v]" > \$2
@@E_O_F
chmod +x symbols.sh
