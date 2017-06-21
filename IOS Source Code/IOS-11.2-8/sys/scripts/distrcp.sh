#!/bin/sh

# $Id: distrcp.sh,v 3.2 1995/11/17 18:49:40 hampton Exp $
# $Source: /swtal/cherf.111/ios/sys/scripts/distrcp.sh,v $
#------------------------------------------------------------------
# More paranoid rcp for build folks use, typically called during
# a "make dist". Kinda a slow behemoth
#
# March 19955, Dave Uebele
#
# Copyright (c) 1995 by cisco Systems, Inc.
# All rights reserved.
#------------------------------------------------------------------
# $Log: distrcp.sh,v $
# Revision 3.2  1995/11/17  18:49:40  hampton
# Remove old entries from the RCS header logs.
#
# Revision 3.1  1995/11/09  13:10:51  shaker
# Bump version numbers from 2.x to 3.x.
#
# Revision 2.2  1995/06/19  01:39:44  gwynne
# CSCdi35976:  make dist assumes default tftp host for image copy to obj-*
#
# Dealing with a tftp host that was not "dirt:" caused
# ambiguity in host name assignment in distrcp.sh.
# To fix this,  shell routines extracttfile and extracttmachine
# were added to parse host:filename combinations.
# Changes are required to sys/makefile also, although this change is
# atomic and does not require the sys/makefile change.
#
# Revision 2.1  1995/06/07  22:43:17  hampton
# Bump version numbers from 1.x to 2.x.
#
#------------------------------------------------------------------
# $Endlog$
#

# Assumptions:
# supports a subset of rcp syntax.
# Source file will always assume to be local machine (and local directory)
# destination file may have machine name included, and full pathname
# if destination file does not have a machine name, first assume local,
# then determine via "df" command if that directory is actually on 
# another machine, set target machine acordingly for rcp command.
#
# test if target dir exists, create if needed
# md5 fingerprint source file
# perform rcp
# md5 fingerprint destination file
# compare source and dest md5. if different, remove dest and exit with 
# return value other than 0

# extracttmachine
#
# Extract the host name $tmachine from $destination.
# Split the destination name into "host" "filename" from "host:filename".
#
extracttmachine() {

   tcnt=`echo $destination | awk '{ printf("%d", split($0, dest, ":")) }'` 

   if [ $tcnt -gt 1 ]; then
      tmachine=`echo $destination | \
             awk '{split($0, dest, ":"); print dest[1]}'`
   else
      tmachine=""
   fi

}
# extracttfile
#
# Extract the file name from $destination.
# Split the destination name into "host" "filename" from "host:filename".
#
extracttfile() {

   tcnt=`echo $destination | awk '{ printf("%d", split($0, dest, ":")) }'` 

   if [ $tcnt -gt 1 ]; then
      tfile=`echo $destination | \
             awk '{split($0, dest, ":"); print dest[2]}'`
   else
      tfile=`echo $destination | \
             awk '{split($0, dest, ":"); print dest[1]}'`
   fi

   if [ "x$tfile" = "x" ]; then
      echo "$0: incomplete target filename for $destination"
      exit 1
   fi

}

# define location of md5 program to use.
md5=/router/bin/md5
retval=0
destdir=false

if [ "$#" -lt "2" ]; then
    echo "Usage: $0 <source.file> <machine:dest.file>"
    exit 1
fi

sfile=$1
while [ "$#" -gt "2" ]; do
    shift
    sfile="$sfile $1"
    destdir=true
done
destination=$2

# Extract machine and file name from destination arguement

extracttmachine
extracttfile

if [ "x$tmachine" = "x" ]; then
   # no machine name in argument, set target file as second arg
   tdir=`dirname $tfile`
   # test for existence of target directory, create if it does not
   # exist and test again, still needs to handle when the directory is
   # a symbolic link
   if [ "$tdir" != "." ] ; then
      if [ ! -d "$tdir" ] ; then
         mkdir $tdir
      fi
      if [ ! -d "$tdir" ] ; then
        echo "$0: Can not create $tdir"
        exit 1
      fi
   else
      echo "$0: incomplete target pathname for file $tfile"
      echo "Please give complete pathname for target file"
      exit 1
   fi
   # set target machine as either local or where directory is mounted
   destination=`df $tdir | grep -v Filesystem | awk '{ print $1 }'`
   extracttmachine
   if [ "x$tmachine" = "x" ]; then
      # directory is on local machine
      tmachine=`hostname`
   fi
fi
[ -d "$tfile" ] && destdir=true

# Test for absolute path on target file, put current directory in front
# of any target filename that does not begin with "/"
firstchar=`echo $tfile | awk ' { printf("%s\n", substr($1,1,1) ) }'`
if [ "$firstchar" != "/" ] ; then
   tfile="`pwd`/$tfile"
fi

# make sure source file exists

for file in $sfile
do
   if [ ! -f "$file" ] ; then
       echo "$0: Source file $file does not exist"
       # need to allow a normal return value, so dist can continue with
       # missing files. Besides, for a rigorous dependency, it will get 
       # get tested before it gets to this point.
       # exit 1
   fi
done

# make sure have some kind of machine name for target file.
if [ "x$tmachine" = "x" ] ; then
    echo "$0: No target machine for rcp"
    exit 1
fi


# determine if target directory exists, create if it does not.
# this time using rsh commands, to make sure the directory exists on the 
# remote machine as well as appearing locally.
# destdir=true
tdir=`dirname $tfile`
if [ "$tdir" != "." ] ; then
   testdir=`rsh $tmachine ls -d $tdir`
   if [ "$tdir/" != "$testdir" ]; then
      rsh $tmachine mkdir $tdir
      testdir=`rsh $tmachine ls -d $tdir`
      if [ "$tdir/" != "$testdir" ]; then
         echo "$0: Can not create $tdir on $tmachine"
         exit 1
      fi
   fi
   if [ "$destdir" = "true" ] ; then
      testdir=`rsh $tmachine ls -d $tfile`
      if [ "$tfile/" != "$testdir" ]; then
         rsh $tmachine mkdir $tfile
         testdir=`rsh $tmachine ls -d $tfile`
         if [ "$tfile/" != "$testdir" ]; then
            echo "$0: Can not create $tfile on $tmachine"
            exit 1
         fi
      fi
   fi
else
   echo "$0: incomplete target pathname for file $tfile"
   echo "Please give complete pathname for target file"
   exit 1
fi
# check md5 signatures before and after, do the rcp and compare signatures.

for file in $sfile
do
   md5pre=0
   md5post=1
   tplus=""
   if [ ! -f "$file" ] ; then
      echo "$0: Source file $file does not exist"
      # need to allow a normal return value, so dist can continue with
      # missing files.
      # exit 1
   else
      if [ "$destdir" = "true" ] ; then
         tplus="/`basename $file`"
      fi
      md5pre=`$md5 $file | awk '{ print $1 }'`
      rcp $file $tmachine:$tfile
      retval=$?
      md5post=`rsh $tmachine $md5 $tfile$tplus | awk '{ print $1 }'`
      if [ "$md5pre" != "$md5post" ] ; then
	 rsh $tmachine rm $tfile$tplus
         echo "$0: md5 sums do not match on files $file and $tfile$tplus"
	 echo "file $tfile$tplus removed"
         exit 1
      fi
   fi
done
exit $retval
