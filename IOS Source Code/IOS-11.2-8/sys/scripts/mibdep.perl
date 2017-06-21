#!/bin/sh -- # This line is needed by perl to prevent looping  -*-Perl-*-
eval 'exec perl5 -S $0 ${1+"$@"}'
        if 0;
'di';
'ig00';
###############################################################################
# mibdep.perl:  MIB dependancies lister
# Copyright (c) 1997 by cisco Systems, Inc.
###############################################################################

# establish some global variables

@path = ('');				# where to look for mib files

# process all the switches
while ($ARGV[0] =~ /^-/) {
    $arg = shift;

    if ($arg eq '-I') {
	$path = shift;
	if ($path !~ m|/$|) {
	    $path .= '/';
	}
	push(@path, $path);
    } else {
	die "Unknown switch:  $arg\n";
    }
}

# remaining arguments are files to be processed
die "Required parameter missing\n" unless scalar(@ARGV);
while (scalar(@ARGV)) {
    $current = shift;

    $mib = &mib_locate($current);
    die "Unable to find mib files for $current\n" unless $mib;

    unless ($processed{$mib}) {
	$processed{$mib} = 1;

	# a *.def file depends on the associated .my file
	($def = $mib) =~ s/\.my/\.def/;
	print STDOUT "$def : $mib\n";

	# a .def file depends on the .def file of each imported mib
	@imports = &mib_imports($mib);
	foreach (@imports) {
	    print STDOUT "$def : $_.def\n";
	}
    }
}

#
# mib_locate
# locate the source file associated with a given mib filename
#
sub mib_locate {
    local($fullname) = @_;
    local($mibname);
    local($name);
    local($path);
    local(@spath) = @path;

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
# given the text of a mib file as input, this function returns a list
# of all of the mibs that the given mib imports
#
# note that an ASN.1 module may have at most one IMPORTS clause with
# the syntax IMPORTS {{symbol} [, {symbol}]+ FROM {module}}+;
#
sub mib_imports {
    local($mib) = @_;
    local($text);
    local(@imports);
    local(@tokens);
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

    # zero in on the contents of the imports clause
    $text =~ s/.*IMPORTS(.*?);.*/$1/s;
    @tokens = split(" ", $text);
    foreach (@tokens) {
	# only keep the tokens which appear after a "FROM" clause
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
# strip comments from an ASN.1 file
#
sub mib_decomment {
    local($mibtext) = @_;

    # an ASN.1 comment starts with two hyphens and ends with
    # either a second set of two hyphens or end of line

    $mibtext =~ s/--.*?--//gm;
    $mibtext =~ s/--.*$//gm;
    return $mibtext;
}

#
# read_file
# suck the contents of the named file into a string variable
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
.TH MIBDEP 1 "January 2, 1997"
.AT 3
.SH NAME
mibdep.perl \- Perl script to determine mib compilation dependancies
.SH SYNOPSIS
.BR mibdep.perl " [ -I " \c
.IR path " ].\|.\|. "
.IR filename ".\|.\|.
.SH DESCRIPTION
Given a list of mib files as input, this script will output the makefile
dependancies required to compile the mibs.
.br
.SH OPTIONS
.TP
.BI \-I " " path
Add the given path to the list of directories that
.B mibdep.perl
searches to locate imported MIBs.  By default
.B mibdep.perl
only searchs for imported MIBs in the same directory as the input
MIBs.
.SH ENVIRONMENT
No environment variables are used.
.SH NOTES
Although
.BR mosy (l)
does not actually require that all of the MIBs that a given MIB imports be compiled prior to compiling the given MIB,
.BR postmosy (l)
does, so the dependancies output by this program force the imported MIBs to be compiled prior to the given MIB.
.SH AUTHOR
Jeff Johnson
.SH "SEE ALSO"
.BR perl (1)
.BR mosy (l)
.BR postmosy (l)
.ex
