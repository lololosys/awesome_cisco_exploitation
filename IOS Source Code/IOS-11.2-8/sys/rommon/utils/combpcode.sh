#!/bin/sh
# $Id: combpcode.sh,v 3.2.62.2 1996/06/16 21:19:09 vnguyen Exp $
# $Source: /release/112/cvs/Xsys/rommon/utils/combpcode.sh,v $
#------------------------------------------------------------------
# combpcode.sh
#
# March 1994, Rob Clevenger
#
# Copyright (c) 1994-1996 by cisco Systems, Inc.
# All rights reserved.
# 
# Script to glue an image onto an srecord file with spacer
# between.
#------------------------------------------------------------------
# $Log: combpcode.sh,v $
# Revision 3.2.62.2  1996/06/16  21:19:09  vnguyen
# Finally committing Brasil to California_branch
#
# Revision 3.2.62.1  1996/05/08  11:58:01  mdb
# Implement a method of naming locally built tools based on the type of
# the Machine, the name and the revision of the Operating System.
# Also make provide for all locally built tools be to removed when a
# 'make superclean' is performed in the sys directory. [CSCdi57043]
# Branch: California_branch
#
# Revision 3.2  1995/11/17  18:48:33  hampton
# Remove old entries from the RCS header logs.
#
# Revision 3.1  1995/11/09  13:10:03  shaker
# Bump version numbers from 2.x to 3.x.
#
# Revision 2.1  1995/06/07  22:42:28  hampton
# Bump version numbers from 1.x to 2.x.
#
#------------------------------------------------------------------
# $Endlog$
#

# combpcode.sh <srecfile> <binfile> <outfile>
#
# Script to glue an image onto an srecord file with spacer
# between
# The first file must be in srecord format as made by
# hxld (or other program garanteed to generate S3 records)
# The second file must be a binary image
# The second file will be converted to srecords
# and catted onto the first with appropriate addresses
# The resultant file will be named the same as arg 3
#
# Update 1/17/95 - allow command line option (-a) to specify
# the starting address of the second file

#set -x
USAGE="usage: $0 [-b] [-a <hexaddr>] <infile1> <infile2> <outfile>"
ADDR_OPT=0
B_OPT=

set -- `getopt ba: $*`
if [ $? != 0 ]
then
	echo $USAGE
	exit 2
fi

for i in $*
do
	case $i in
	-a) ADDR_OPT=1; ADDR=$2; shift 2;;
	-b) B_OPT=b; shift;;
	--) shift; break;;
	esac
done

if test $# != 3
then
	echo $USAGE
	exit 1
fi

# Remove terminator record from first file
grep -v '^S9' $1 >temppcode

if test $ADDR_OPT = 0
then
	# Get ending address from first file
	# Warning: must be S3 srecords for this to work
	ADDR=`tail -1 temppcode | cut -c5-12`

	# add 30 hex to the address
	ADDR=`dc << !!!END!!!
16
16
i
o
$ADDR
30
+
p
q
!!!END!!!`

fi


# convert second file to srecords with proper address
NATIVE_ARCH=`uname -m | sed -e 's,/,-,g'`
NATIVE_OS=`uname -s`
NATIVE_REV=`uname -r`
NATIVE_OSREV=${NATIVE_ARCH}-${NATIVE_OS}${NATIVE_REV}
../utils/hxldsrc/hxld-${NATIVE_OSREV} -${B_OPT}Sr32 -i "$ADDR" -f temp $2
# trim off the S0 record at the beginning
sed -e 's/^S0.*$//' temp >pfsicode
# combine the two images
cat temppcode pfsicode >$3
# clean up
rm -f temppcode pfsicode temp

# end of module
