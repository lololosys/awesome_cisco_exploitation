#!/bin/sh -- # This line is needed by perl to prevent looping  -*-Perl-*-
eval 'exec perl5 -S $0 ${1+"$@"}'
        if 0;
'di';
'ig00';
###############################################################################
# mibcomp.perl
# A front-end for the SNMP Research MIB Compiler which also does a little
# back-end processing
#
# Copyright (c) 1997 by cisco Systems, Inc.
###############################################################################
#
# establish some environment variables

$postmosy	= 'postmosy';	# name of the postmosy program

$configfile	= '';		# name of last configuration file
$configbase	= '';		# base name of last configuration file
$configdir	= '';		# directory containing last configuration file
$stampfile	= '';		# name of the stamp file
$stampbase	= '';		# base name of stamp file
$stampdir	= '';		# directory of stamp file
$directory	= '';		# where should the output go?
$basename	= '';		# how should the files be named?
$codegen	= 0;		# should user-modifiable code be generated?
$xlatetable	= 0;		# should oid translation file be generated?
$debugging	= 0;		# should we say what we're doing?
$rowstatus	= '';		# should we enable postmosy rowstatus code?
$userpart	= '';		# should we enable postmosy userpart code?
$caching	= '-nocache';	# should we enable postmosy caching code?
@groups		= ();		# what mib groups should be processed?

@parameters = @ARGV;
while ($parameters[0] =~ /^-/) {
    $arg = shift(@parameters);

    if ("$arg" eq "-f") {
	$configfile = shift(@parameters);
	open(CONFIG, "<$configfile") ||
	    die("Unable to open $configfile\n");
	($configbase = $configfile) =~ s|.*/||;
	($configdir = $configfile) =~ s|$configbase$||;
	$configbase =~ s/\..*?$//;
	@additions = ();
	while (<CONFIG>) {
	    chop;
	    s/#.*//;
	    s/^\s+//g;
	    s/\s+$//g;
	    next if ($_ eq '');
	    push(@additions, split);
	}
	@parameters = (@additions, @parameters);

    } elsif ("$arg" eq "-s") {
	$stampfile = shift(@parameters);
	($stampbase = $stampfile) =~ s|.*/||;
	($stampdir = $stampfile) =~ s|$stampbase$||;
	$stampbase =~ s/\..*?$//;
	if ($codegen) {
	    print STDERR "Use of -s is deprecated, please use -f\n";
	}

    } elsif ("$arg" eq "-postmosy") {
	$postmosy = shift(@parameters);

    } elsif ("$arg" eq "-group" || "$arg" eq "-g") {
	$newgroup = shift(@parameters);
	push(@groups, $newgroup);
	if ("$arg" eq "-g" && $codegen) {
	    print STDERR "Use of -g is deprecated, please use -group\n";
	}

    } elsif ("$arg" eq "-codegen") {
	$codegen = 1;

    } elsif ("$arg" eq "-debug") {
	$debugging = 1;

    # remaining switches behave exactly like their postmosy counterpart

    } elsif ("$arg" eq "-cache") {
	$caching = "";

    } elsif ("$arg" eq "-row_status") {
	$rowstatus = "-row_status";

    } elsif ("$arg" eq "-userpart") {
	$userpart = "-userpart";
	    

    } elsif ("$arg" eq "-snmpmibh") {
	$xlatetable = 1;

    } else {
	print STDERR "Unknown switch:  $arg\n";
    }
}

# sanity check:  are there any parameters (mibs) left to process?
die("Error: no mib files specified\n") unless scalar(@parameters);

# sanity check:  do we have any groups selected?
die("Error: no mib groups selected\n") unless scalar(@groups);

# do we know where to put the output?
$directory = $configdir unless $directory;
$directory = $stampdir unless $directory;
$directory = "./" unless $directory;

# do we know the base file name?
$basename = $configbase unless $basename;
$basename = $stampbase unless $basename;
$basename = "sr_default" unless $basename;

$stampfile = "$directory$basename.stamp" unless $stampfile;
$stamptext = '';

# for historical reasons, strip any "sr_" prefix when naming variables
($basevar = $basename) =~ s/^sr_//;

# determine the complete set of mibs required by postmosy
@mibs = &mib_requirements(@parameters);

# create the postmosy configuration file
$pmconfig = "$basename.cfg$$";
open(PMCONFIG, ">$pmconfig") ||
    die("Error:  Unable to create temporary file $pmconfig\n");
print PMCONFIG "[ EMANATE ]\n";
print PMCONFIG "$basename ", join(" ", @groups), "\n";
close(PMCONFIG);

# create and execute the postmosy command line
$pmcommand = "$postmosy -agent -ansi -noinit -no_cvs -snmpmibh " .
    "-f $pmconfig $caching $rowstatus $userpart " .
    join(" ", @mibs);

if ($debugging) {print STDERR $pmcommand, "\n"}
system("$pmcommand");
if ($?) {
    die("Error: compilation failed with status $?, command used\n",
	"$pmcommand\n");
    # the config file and generated files are left for debugging
}

unlink $pmconfig;

# postmosy has created the following files
# {basename}info.dat
# k_{basename}.stb
# v_{basename}.stb
# {basename}-mib.h
# {basename}defs.h
# {basename}part.h
# {basename}supp.h
# {basename}type.h
# {basename}oid.c

# don't need the .dat postmosy output file
unlink("${basename}info.dat");

# only process the oid translation file if code generation was requested
# and the old translation table was requested

$infile = "$basename-mib.h";
if ($codegen && $xlatetable) {
    # suck in the contents of the oid translation file
    $mibfile = &read_file($infile)  ||
	die("Unable to read $infile\n");
    $mibfile = &fixup_mibfile($mibfile);

    # need to write the puppy out
    $outfile = "$directory$basename-mib.h";
    if (-e $outfile) {
	print STDERR "Warning:  $outfile already exists, not overwritten\n";
    } else {
	open(OUTFILE, ">$outfile") ||
	    die("Unable to create $outfile\n");
	print STDERR "writing $outfile\n" if $debugging;
	print OUTFILE $mibfile;
	close(OUTFILE);
    }
    $stamptext .= "\n\n$outfile\n\n$mibfile";
}
unlink $infile;

$kstbname = "k_$basename.stb";
$vstbname = "v_$basename.stb";

# create a userpart file if code generation was requested
# and a userpart file was requested

if ($codegen && $userpart) {
    # create user file based on the stub file
    ($userfile) = create_user($vstbfile);

    # need to write the puppy out
    $outfile = "$directory${basename}user.h";
    if (-e $outfile) {
	print STDERR "Warning:  $outfile already exists, not overwritten\n";
    } else {
	open(OUTFILE, ">$outfile") ||
	    die("Unable to create $outfile\n");
	print STDERR "writing $outfile\n" if $debugging;
	print OUTFILE $userfile;
	close(OUTFILE);
    }
    $stamptext .= "\n\n$outfile\n\n$userfile";
}
unlink $infile;

# only process the stub files if code generation was requested

if ($codegen) {
    # suck in the contents of the stub files
    $vstbfile = &read_file($vstbname) ||
	die("Unable to read $vstbname\n");
    $kstbfile = &read_file($kstbname) ||
	die("Unable to read $kstbname\n");

    # create .c and .h files from the stubs
    ($cfile, $hfile) = create_c_and_h($vstbfile, $kstbfile);

    # need to write the puppies out
    $outfile = "$directory$basename.c";
    if (-e $outfile) {
	print STDERR "Warning:  $outfile already exists, not overwritten\n";
    } else {
	open(OUTFILE, ">$outfile") ||
	    die("Unable to create $outfile\n");
	print STDERR "writing $outfile\n" if $debugging;
	print OUTFILE $cfile;
	close(OUTFILE);
    }

    $stamptext .= "\n\n$outfile\n\n$cfile";

    $outfile = "$directory$basename.h";
    if (-e $outfile) {
	print STDERR "Warning:  $outfile already exists, not overwritten\n";
    } else {
	open(OUTFILE, ">$outfile") ||
	    die("Unable to create $outfile\n");
	print STDERR "writing $outfile\n" if $debugging;
	print OUTFILE $hfile;
	close(OUTFILE);
    }

    $stamptext .= "\n\n$outfile\n\n$hfile";

}

unlink $kstbname, $vstbname;

# suck in the contents of the definitions file
$infile = "${basename}defs.h";
$defsfile = &read_file($infile) ||
    die("Unable to open $infile\n");
$defsfile = &fixup_trivial($defsfile);

$outfile = "$directory${basename}defs.h";
unlink $outfile if -e $outfile;
open(OUTFILE, ">$outfile") ||
    die("Unable to create $outfile\n");
print STDERR "writing $outfile\n" if $debugging;
print OUTFILE $defsfile;
close(OUTFILE);
chmod 0444, $outfile;
unlink $infile;

# suck in the contents of the particle file
$infile = "${basename}part.h";
$partfile = &read_file($infile) ||
    die("Unable to open $infile\n");
$partfile = &fixup_trivial($partfile);

$outfile = "$directory${basename}part.h";
unlink $outfile if -e $outfile;
open(OUTFILE, ">$outfile") ||
    die("Unable to create $outfile\n");
print STDERR "writing $outfile\n" if $debugging;
print OUTFILE $partfile;
close(OUTFILE);
chmod 0444, $outfile;
unlink $infile;

# suck in the contents of the support file
$infile = "${basename}supp.h";
$suppfile = &read_file($infile) ||
    die("Unable to open $infile\n");
$suppfile = &fixup_suppfile($suppfile);
$suppfile = &fixup_trivial($suppfile);

$outfile = "$directory${basename}supp.h";
unlink $outfile if -e $outfile;
open(OUTFILE, ">$outfile") ||
    die("Unable to create $outfile\n");
print STDERR "writing $outfile\n" if $debugging;
print OUTFILE $suppfile;
close(OUTFILE);
chmod 0444, $outfile;
unlink $infile;

# suck in the contents of the types file
$infile = "${basename}type.h";
$typefile = &read_file($infile) ||
    die("Unable to open $infile\n");
$typefile = &fixup_typefile($typefile);
$typefile = &fixup_trivial($typefile);

$outfile = "$directory${basename}type.h";
unlink $outfile if -e $outfile;
open(OUTFILE, ">$outfile") ||
    die("Unable to create $outfile\n");
print STDERR "writing $outfile\n" if $debugging;
print OUTFILE $typefile;
close(OUTFILE);
chmod 0444, $outfile;
unlink $infile;

# suck in the contents of the oid file
$infile = "${basename}oid.c";
$oidfile = &read_file($infile) ||
    die("Unable to open $infile\n");

$oidfile = &fixup_oidfile($oidfile);
$oidfile = &fixup_trivial($oidfile);

$outfile = "$directory${basename}oid.c";
unlink $outfile if -e $outfile;
open(OUTFILE, ">$outfile") ||
    die("Unable to create $outfile\n");
print STDERR "writing $outfile\n" if $debugging;
print OUTFILE $oidfile;
close(OUTFILE);
chmod 0444, $outfile;
unlink $infile;

unlink $stampfile if -e $stampfile;
open(STAMPFILE, ">$stampfile") ||
    die("Unable to create $stampfile\n");
print STDERR "writing $stampfile\n" if $debugging;
print STAMPFILE $stamptext;
close(STAMPFILE);
chmod 0444, $stampfile;


#
# fixup_mibfile
# perform post-processing of the {base}-mib.h file
#
sub fixup_mibfile {
    local($text) = @_;

    # the file begins with 4 separate comment blocks:
    # 1) copyright
    # 2) proprietary notice
    # 3) arguments used
    # 4) do not edit
    #
    # we'll keep the first two and remove the second two

    $text =~ s#(/\*.*?\*/.*?/\*.*?\*/.*?)/\*.*?\*/.*?/\*.*?\*/#$1#s;

    # there is code that is conditional upon the LIGHT define.
    # we usually compile with LIGHT enabled, but in this case
    # we want it disabled, so the easiest thing to do is just
    # remove the conditional code

    $text =~ s|#ifndef LIGHT(.*?)#else.*LIGHT \*/|$1|s;

    # there is code that is conditional upon c++.  nuke it.
    $text = &nuke_cplusplus($text);

    # while we're at it, get rid of a comment that was
    # bound to the c++ conditional code
    $text =~ s|/\* Don\'t put anything after this #endif \*/||sg;

    # make sure the array is declared const with a unique name
    $text =~ s/\nstatic/\nstatic const/;
    $text =~ s/orig_mib/$basevar/;

    # add the cisco header
    $text = &add_header($text);

    # add notice to remove non-leaf entries
    $text = "NOTICE: these entries waste space with little benefit.\n" .
	    "        remove all unnecessary entries, including:\n" .
	    "        o all Table entries\n" .
	    "        o all leaf entries\n" .
	    "        o all entries without leaf descendants\n\n" .
		$text;

    return $text;
}


#
# create_c_and_h
# create the {base}.c and {base}.h files based upon the 
# k_{base}.stb and v_{base}.stb stub files
#
sub create_c_and_h {
    local($vstbfile, $kstbfile) = @_;
    local($ctext);
    local($htext);
    local($utext);
    local($srheader);

    # both stub files begin with three separate comment blocks
    # remember them for later
    ($srheader = $vstbfile) =~ s|(/\*.*?\*/.*?/\*.*?\*/.*?/\*.*?\*/).*|$1\n|s;

    # following the three comment blocks is a series of include directives,
    # ending with the include of "mibout.h", which are inappropriate in our
    # environment, so blow away the entire top of the file up through the
    # list of includes
    $vstbfile =~ s|^.*?"mibout\.h"\n||s;
    $kstbfile =~ s|^.*?"mibout\.h"\n||s;

    # if the mib has writable objects, the k_ stub file has a comment
    # which describes how to enable the undo routines.  the method does not
    # work in our environment, so update the comment to show the proper
    # method of enabling undo routines
    $kstbfile =~ s|sitedefs\.h|$basename.h|g;

    # create the guts of the header file
    $htext = $srheader . "\n" .
	"#ifndef _${basename}_h_\n" .
	"#define _${basename}_h_\n\n" .
	"#include \"../snmp/snmp_api.h\"\n" .
	"#include \"${basename}defs.h\"\n" .
	"#include \"${basename}supp.h\"\n";
    if ($userpart) {
	$htext .= "#include \"${basename}user.h\"\n";
    }
    $htext .=
	"#include \"${basename}part.h\"\n" .
	"#include \"${basename}type.h\"\n\n" .
	"extern const ObjectInfo ${basevar}_OidList\[\];\n" .
	"extern const uint       ${basevar}_OidListNum;\n\n" .
	"#endif  /* _${basename}_h_ */\n";

    # create the guts of the code file
    $ctext = $srheader . "\n" .
	"#include \"master.h\"\n" .
	"#include \"subsys.h\"\n" .
	"#include \"$basename.h\"\n";
    if ($xlatetable) {
	$ctext .= "#include \"$basename-mib.h\"\n";
    }
    $ctext .= "\n\n" .
	"/*\n" .
	" * Start of system-independent routines\n" .
	" */\n\n" .
	$vstbfile .
	"/*\n" .
	" * Start of system-dependent routines\n" .
	" */\n\n" .
	$kstbfile .
	"\n\nstatic void\n" .
	"init_$basevar (subsystype *subsys)\n" .
	"{\n" .
	"    load_mib(${basevar}_OidList, ${basevar}_OidListNum);\n";
    if ($xlatetable) {
	$ctext .= "    load_oid(${basevar}_oid_table);\n";
    }
    $ctext .= "}\n\n/*\n * $basevar subsystem header\n */\n\n" .
	"#define MAJVERSION_$basevar 1\n" .
	"#define MINVERSION_$basevar 0\n" .
	"#define EDITVERSION_$basevar 0\n\n" .
	"SUBSYS_HEADER($basevar,\n" .
	"              MAJVERSION_$basevar,\n" .
	"              MINVERSION_$basevar,\n" .
	"              EDITVERSION_$basevar,\n" .
	"              init_$basevar,\n" .
	"              SUBSYS_CLASS_MANAGEMENT,\n" .
	"              NULL,\n" .
	"              NULL);\n" .
	"WARNING: replace last two NULL parameters with \"req:\" and \"seq:\"\n" .
	"         directives if required\n";

    $ctext = &add_header($ctext);
    $htext = &add_header($htext);
    return($ctext, $htext);
}

#
# create_user
# create the {base}user.h file based upon a stub file
#
sub create_user {
    local($stbfile) = @_;
    local($usertext);
    local($htext);
    local($utext);
    local($srheader);

    # a stub files begins with three separate comment blocks
    # start the user file with the same comments
    ($utext = $stbfile) =~ s|(/\*.*?\*/.*?/\*.*?\*/.*?/\*.*?\*/).*|$1\n|s;

    # create the guts of the user file
    $utext .= "\n" .
	"#ifndef _${basename}user_h_\n" .
	"#define _${basename}user_h_\n\n" .
	"Add your U_* macro definitions here\n\n" .
	"#endif  /* _${basename}user_h_ */\n";

    return($utext);
}


#
# fixup_suppfile
# perform post-processing of the {base}supp.h file
# 
sub fixup_suppfile {
    local($text) = @_;

    # the file begins with 5 separate comment blocks:
    # 1) copyright
    # 2) proprietary notice
    # 3) arguments used
    # 4) undef instructions
    # 5) do not edit
    #
    # the undef instructions are incorrect for our environemnt, so nuke them

    $text =~ s#(/\*.*?\*/.*?/\*.*?\*/.*?/\*.*?\*/.*?)/\*.*?\*/#$1#s;

    # there is also a #include that is inappropriate
    $text =~ s|#include "undefs.h"\n||;

    return $text;
}

#
# fixup_typefile
# perform post-processing of the {base}type.h file
# 
sub fixup_typefile {
    local($text) = @_;

    # there is a #include that is inappropriate
    $text =~ s|#include "sr_proto.h"\n||;
    $text =~ s|#include "userpart.h"\n||;

    return $text;
}

#
# fixup_oidfile
# perform post-processing of the {base}oid.c file
# 
sub fixup_oidfile {
    local($text) = @_;

    # need to add some text following the "do not edit" comment
    local($header) = "
#include \"master.h\"
#define SNMPPART_DECL
#include \"$basename.h\"
#undef SNMPPART_DECL

const ObjectInfo ${basevar}_OidList\[\] = {
";

    $text =~ s|(/\*.*? do not edit.*?\*/\n)|$1$header|s;

    # need to add some text to the end of the file
    $text .= "
};
const uint ${basevar}_OidListNum = sizeof(${basevar}_OidList)/sizeof(${basevar}_OidList[0]);";

    return $text;
}

#
# fixup_trivial
# perform post-processing applicable to all non-modifiable files
# 
sub fixup_trivial {
    local($text) = @_;

    # there is code that is conditional upon c++.  nuke it.
    $text = &nuke_cplusplus($text);

    # add the "do not modify" header
    $text = &add_dnm($text);

    return $text;
}


#
# nuke_cplusplus
# remove cplusplus conditional code from postmosy output files
# 
sub nuke_cplusplus {
    local($text) = @_;
    $text =~ s/\n#ifdef __cplusplus.*?#endif\n//gs;
    return $text;
}


#
# add_header
# add the standard cisco header to a user-modifiable file
# 
sub add_header {
    local($text) = @_;
    local($sec,$min,$hour,$mday,$mon,$year,$wday,$yday,$isdst) = localtime;
    local($name);
    local($copyrite) = 'Copyright';	#to fake out copyright checking

    $year += 1900;
    $mon = (January, February, March, April,
	    May, June, July, August,
	    September, October, Novemeber, December)[$mon];
    $name = (getpwuid($<))[6];

    $text =
"/* \$Id\$
 * \$Source\$
 *------------------------------------------------------------------
 * SNMP/SNMPv2 bi-lingual agent code.
 *
 * $mon $year, $name (via the SNMP Research MIB Compiler)
 *
 * $copyrite (c) $year by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * \$Log\$
 *------------------------------------------------------------------
 * \$Endlog\$
 */

" . $text;

    return($text);
}

#
# add_dnmp
# add the "do not modify" header to non-modifiable files
#
sub add_dnm {
    return
"/*
 * This file is machine generated
 * Do not attempt to edit it or place it under source control
 */

" . shift;
}

#
# mib_requirements
# given an input list of mibs to be compiled, recursively determine
# what mibs are imported, and hence must be supplied to postmosy
#
sub mib_requirements {
    local(@process) = @_;
    local(@defs) = ();
    local($def);
    local($mib);
    local(%processed);

    while (scalar(@process)) {
	$current = shift @process;

	$mib = &mib_locate($current);
	die "Unable to find mib files for $current\n" unless $mib;

	unless ($processed{$mib}) {
	    $processed{$mib} = 1;
	    ($def = $mib) =~ s/\.my/\.def/;
	    push(@defs, $def);
	    @process = (@process, &mib_imports($mib));
	}
    }
    return @defs;
}

#
# given a mib name, locate the mib
#
sub mib_locate {
    local($fullname) = @_;
    local($mibname);
    local($name);
    local($path);
    local(@spath);

    # separate the name into its file and path components
    ($name = $fullname) =~ s|.*/||;
    ($path = $fullname) =~ s|$name$||;

    # remove any extension from the filename
    $name =~ s|\..*||;

    # our mib search path consists of the path for this mib plus
    # the paths specified on the command line

    @spath = ($path, @path);

    foreach (@spath) {
	$mibname = $_ . $name . ".my";
	return $mibname if (-e $mibname);
    }
    return;
}

#
# mib_imports
# given a mib filename, determine the mibs imported by that mib
#
sub mib_imports {
    local($mib) = @_;
    local($text);
    local(@imports);
    local($savenext) = 0;
    local($name);
    local($path);

    # separate the name into its file and path components
    ($name = $mib) =~ s|.*/||;
    ($path = $mib) =~ s|$name$||;

    $text = &read_file($mib) || die("Unable to read $mib\n");
    $text = &mib_decomment($text);

    # short circuit if no IMPORTS
    return if $text !~ m/IMPORTS/;

    $text =~ s/.*IMPORTS(.*?);.*/$1/s;
    @tokens = split(" ", $text);
    foreach (@tokens) {
	if ($savenext) {
	    $savenext = 0;
	    push(@imports, $path.$_);
	} elsif ($_ eq "FROM") {
	    $savenext = 1;
	}
    }
    return @imports;
}

#
# mib_decomment
# strip comments from and ASN.1 file
#
sub mib_decomment {
    local($mibtext) = @_;
    local($comment) = '--';

    # an ASN.1 comment starts with two hyphens and ends with
    # either a second set of two hyphens or end of line

    $mibtext =~ s/--.*?--//gm;
    $mibtext =~ s/--.*$//gm;
    return $mibtext;
}

#
# read_file
# suck the contents of a file into a string variable
#
sub read_file {
    local($filename) = @_;
    local($text) = '';
    local($oldslash);
 
    if(open(FILE, "<$filename")) {
        $oldslash = $/;
        undef $/;
        $text = <FILE>;
        close(FILE);
        $/ = $oldslash;
    }
    $text;
}

##############################################################################
 
        # These next few lines are legal in both Perl and nroff.
 
.00;                    # finish .ig
 
'di                     \" finish diversion--previous line must be blank
.nr nl 0-1              \" fake up transition to first page again
.nr % 0                 \" start at page 1
'; __END__ ############# From here on it's a standard manual page ############
.TH MIBCOMP 1 "January 3, 1997"
.AT 3
.SH NAME
mibcomp.perl \- Perl script to 
.SH SYNOPSIS
.BR mibcomp.perl
[
.IR options
] {
.IR name
} ...
.SH DESCRIPTION
Given a list of mib definition files as input, this script will invoke the
.I postmosy(l)
mib compiler to generate C source code for the mibs.
.br
.SH OPTIONS
.IP \-f\ filename
Specifies a configuration file that contains additional
.I mibcomp.perl
parameters.  Note that since options are processed left to right, options
that are specified in a configuration file may be overridden by switches
that occur later on the command line.
.IP \-postmosy\ filename
Specifies the name of the 
.I postmosy(l)
program.  by default
this program runs the command "postmosy" but this switch
may be used to specify an alternate file, typicially a
postmosy executable with a version identifier added to the
name, such as "postmosy-14.2.0.0"
.IP \-group\ name
Specifies a mib group for which code should be generated.  If multiple
.I -group
switches are specified, the results are cumulative.
.IP \-debug
Tell a little about what the program is doing.
.IP \-codegen
Instructs
.I mibcomp.perl
to generate user-modifiable files as well as non-modifiable files.
.PP
The following switches behave exactly like the associated
.I postmosy(l)
switch
.IP \-cache
.IP \-row_status
.IP \-userpart
.IP \-snmpmibh

.SH ENVIRONMENT
No environment variables are used.
.SH FILES
.IP *.def
MIB definitions files.  These files are generated by
.I mosy(l)
and are valid input files for
.I postmosy(l),
and hence for
.I mibcomp.perl.
.IP <base>defs.h
Contains macros for enumerated MIB object values in the form of
`#define D_<object>_<enumeration> value'.
.IP <base>part.h
Contains data structure definitions or external declarations (depending
on the #ifdef settings) for components of the OID for each object in
the MIB, with names in the form of `ID<object>'; also contains macros
for each object in the form of `#define LN_<object> <length>'.
.IP <base>user.h
Contains a placeholder for user-supplied macros.
This file is generated by the
.I \-userpart
option.
.IP <base>supp.h
Contains macros for leaf objects in the MIB in the form of
`#define I_<object> <family offset>'.
.IP <base>type.h
Contains a typedef definition of a structure (named <family>_t) for
each family of objects in the MIB; also contains prototypes for
system dependent and system independent method routines.
.IP <base>-mib.h
Contains name\-to\-OID translation information for the MIB.
This file is generated by the
.I \-snmpmibh
option.
.IP <base>oid.c
Contains information about each object in the MIB, including
OID structure, MAX\-ACCESS (ACCESS for SNMPv1), family offset,
and associated method routine
pointers.  The format of the file is that of an initializer for
an array of C data structures.
.IP <base>.c
Contains the skeleton for the MIB implementation code.
.IP <base>.h
Contains a definition of the MIB dispatch table.  Also includes the
other header files in the appropriate order.
.IP <base>.stamp
A timestamp file used to determine the last time
.I postmosy.perl
was run on the given file.  If used in conjunction with the
.I \-codegen
switch, it will contain the combined text of the user-modifiable files.
.SH AUTHOR
Jeff Johnson
.SH "SEE ALSO"
.BR perl (1),
.BR mosy (l),
.BR postmosy (l)
.ex
