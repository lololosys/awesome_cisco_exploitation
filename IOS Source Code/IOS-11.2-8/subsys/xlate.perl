: # use perl -*-Perl-*-
eval 'exec perl5 -S $0 ${1+"$@"}'
    if 0;
#
# $Id: xlate.perl,v 3.18.4.15 1996/08/22 18:58:12 ajchopra Exp $
# $Source: /release/112/cvs/Xsubsys/xlate.perl,v $
#------------------------------------------------------------------
# Create appropriate symlinks as given after __END__ statement
#
# September 1995, Ron Chiao
#
# Copyright (c) 1996, 1997 by cisco Systems, Inc.
# All rights reserved.
#------------------------------------------------------------------
# $Log: xlate.perl,v $
# Revision 3.18.4.15  1996/08/22  18:58:12  ajchopra
# CSCdi66823:  To Commit APPN subsystem files for 112(0.22)
# Branch: California_branch
#
# Revision 3.18.4.14  1996/08/19  23:37:27  dharkins
# CSCdi66502:  remove crypto toolkit from repository.
# Branch: California_branch
# crypto toolkit version bumped
#
# Revision 3.18.4.13  1996/08/14  00:54:58  susingh
# CSCdi65951:  Add appn subsystem object files for 112(0.21)
# Branch: California_branch
#
# Revision 3.18.4.12  1996/08/09  20:13:09  susingh
# CSCdi65640:  Add appn subsystem object files for 112(0.20)
# Branch: California_branch
#
# Revision 3.18.4.11  1996/08/03  01:34:14  susingh
# CSCdi65037:  Add appn subsystem object files 112(0.19)
# Branch: California_branch
#
# Revision 3.18.4.10  1996/07/31  01:27:17  alanyu
# CSCdi64672:  new appn object files for 11.2 (0.18) build/release
# Branch: California_branch
#
# Revision 3.18.4.9  1996/07/20  05:16:06  susingh
# CSCdi63570:  Add appn subsystem objects files for 11.2(0.17)
# Branch: California_branch
#
# Revision 3.18.4.8  1996/07/14  15:17:23  susingh
# CSCdi62884:  Add appn subsystem objects for 112(0.16).
# Branch: California_branch
#
# Revision 3.18.4.7  1996/07/09  13:55:14  susingh
# CSCdi62254:  Add appn subsystem object files for 112(0.15)
# Branch: California_branch
#
# Revision 3.18.4.6  1996/06/30  01:41:12  susingh
# CSCdi61695:  Add appn subsystem object files for 112(0.14)
# Branch: California_branch
#
# Revision 3.18.4.5  1996/06/19  21:57:24  hochan
# CSCdi60819:  Add appn subsystem object files for 112(0.13)
# Branch: California_branch
#
# Revision 3.18.4.4  1996/06/12  23:47:50  hochan
# CSCdi60304:  Add appn subsystem object files for 112(0.12)
# Branch: California_branch
#
# Revision 3.18.4.3  1996/06/04  23:49:07  alanyu
# CSCdi59505:  add appn sub system object files for 11.2 (0.10)
# Branch: California_branch
#
# Revision 3.18.4.2  1996/05/17  10:24:21  ppearce
# Merge IbuMod into Calif
#
# Revision 3.15.2.5  1996/05/05  22:38:49  ppearce
# Sync to IbuMod_Calif_baseline_960504
#
# Revision 3.15.2.4  1996/04/22  21:52:10  pleung
# CSCdi55319:  add appn subsystem object files to repository
# Branch: IbuMod_Calif_branch
#
# Revision 3.15.2.3  1996/04/10  00:13:54  pleung
# CSCdi53908:  add appn subsystem object files to repository
# Branch: IbuMod_Calif_branch
#
# Revision 3.15.2.2  1996/04/09  18:30:57  pleung
# CSCdi53908:  add appn subsystem object files to repository
# Branch: IbuMod_Calif_branch
#
# Revision 3.15.2.1  1996/03/17  17:23:19  ppearce
# Sync IbuMod_Calif_branch to V111_1_3
#
# Revision 3.18.4.1  1996/04/19  14:23:46  che
# CSCdi55138:  IosSec_branch feature commit
# Branch: California_branch
# IosSec_branch feature commits
#
# Revision 3.18  1996/03/13  03:06:18  mkamson
# CSCdi51385:  New Appn subsytem (obj file) for 11.1(1.3) release
#
# Revision 3.17  1996/03/05  23:52:08  mkamson
# CSCdi50770:  New appn subsystem obj for 11.1(1.3)
#
# Revision 3.16  1996/02/28  18:19:43  mkamson
# CSCdi50190:  New appn subsystem obj for 11.1(1.1) release
#
# Revision 3.15  1996/02/23  02:47:14  mkamson
# CSCdi49659:  New appn subsystem obj for 11.1(1.0.4) release
#
# Revision 3.14  1996/02/15  04:14:10  mkamson
# CSCdi48941:  Appn subsystem obj file for 11.1(1.0.3) release
#
# Revision 3.13  1996/02/07  21:04:26  mkamson
# CSCdi48342:  Appn subsystem obj file for 11.1(1.0.2) release
#
# Revision 3.12  1996/02/01  21:11:57  mkamson
# CSCdi47995:  New appn subsystem obj file for 11.1(1.0.1) release
#
# Revision 3.11  1996/01/23  23:54:33  nitin
# CSCdi47371:  New appn subsystem object files for 11.1(0.18) release
#
# Revision 3.10  1996/01/18  00:38:19  nitin
# CSCdi46996:  New appn subsystem object files for 11.1(0.17) release
#
# Revision 3.9  1996/01/09  21:09:18  nitin
# CSCdi46622:  New appn subsystem object files for 11.1 (0.16) release
#
# Revision 3.8  1996/01/04  19:28:14  nitin
# CSCdi46433:  New appn subsystem object files for 11.1(0.15) release
#
# Revision 3.7  1995/12/27  20:40:59  nitin
# CSCdi46150:  New appn subsystem object files for 11.1(0.13) release
#
# New appn subsystem object files for 11.1(0.13) release.
#
# Revision 3.6  1995/12/19  18:56:02  nitin
# CSCdi45857:  New appn subsystem object files for 11.1(0.12) release
# New appn object files for 11.1(0.12) build.
#
# Revision 3.5  1995/12/12  22:27:16  nitin
# CSCdi45470:  New appn subsystem object files for 11.1(0.11) release
# New .gz files for 11.1(0.11) build.
#
# Revision 3.4  1995/12/06  19:57:47  nitin
# CSCdi45033:  New appn subsystem object files for 11.1 (0.9) release
# New appn subsystem for 11.1(0.10) release.
#
# Revision 3.3  1995/11/28  00:13:02  nitin
# CSCdi44501:  New appn subsystem object files for 11.1(0.8) release
# Checked in new appn .gz files for 11.1(0.8) build.
#
# Revision 3.2  1995/11/17  07:45:40  hampton
# Remove old entries from the RCS header logs.
#
# Revision 3.1  1995/11/09  09:32:23  shaker
# Bump version numbers from 2.x to 3.x.
#
#------------------------------------------------------------------
# $Endlog$
#

$debug = 0;
$verbose = 0;

while(<DATA>) {
    next if /^\#/;      # ignore comment lines
    next if /^$/;       # ignore empty lines
    next if /^\s+$/;    # ignore blank lines
    s/\s+/ /g;  # multiple whitespace becomes one space
    ($dir, $alias, $link) = split;
    $new = $dir.'/'.$alias;
    print "new is $new\n" if $debug;
    $pathlink = $dir.'/'.$link;
    if ( -l $pathlink ) { 	# there is an old link
        $old = readlink($pathlink);
        print "old is $old\n" if $debug;
        if ($old ne $new) { 	# and they are not the same with cvs update!
            print "I am Nuking $link!!!\n" if ($verbose);
            $chk=unlink($pathlink);
	    if ($chk) { 
		print "unlink successful\n" if ($verbose);
	    }
	    else {
		print "Warning: unable to unlink $link\n";
	    }
	    &do_symlnk;		# after unlink, link the new one
        }
	else			# link and cvs are the same, do nothing
	{} 
    }
    elsif ( -e $pathlink ) { # no existing link, but name is in use!
        unlink($pathlink) if ( -f _ ); # try to nuke it if it is a file
        rmdir($pathlink) if ( -d _ );  # try to nuke it if it is a directory
        if ( -e $pathlink ) {
            print "Possible trouble, $pathlink is NOT a symlink\n";
        }
        &do_symlnk;
    }
    else { # no existing link! link them
	&do_symlnk;
    }
    
}

exit 0;

sub do_symlnk
{
    local($chks);

    $chks=symlink($new, $pathlink);
    if ($chks) { 
	print "link successful\n" if ($verbose);
    }
    else {
        print "Warning, unable to \"ln -s $new $pathlink\"\n";
    }
}
# if you have more things to be linked, add them after the __END__
# line. Format is:
#	- name of directory to contain the link
#	- name of version controlled file
#	- name of generic file used in sys/makesubsys makefile
# directory name should be relative to sys/obj-*-* and start in the
# first column.
__END__

../../subsys/obj-4k appn112.1-1.gz sub_appn.o.gz
../../subsys/obj-68 appn112.1-1.gz sub_appn.o.gz
../../subsys/obj-36 crypto_toolkit003.gz sub_crypto_toolkit.o.gz
../../subsys/obj-4k crypto_toolkit003.gz sub_crypto_toolkit.o.gz
../../subsys/obj-68 crypto_toolkit003.gz sub_crypto_toolkit.o.gz
../../subsys/obj-4k dlur112.1-1.gz sub_dlur.o.gz
../../subsys/obj-68 dlur112.1-1.gz sub_dlur.o.gz
