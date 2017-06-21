#!/bin/sh -- # This line is needed by perl to prevent looping  -*-Perl-*-
eval 'exec perl5 -S $0 ${1+"$@"}'
	if 0;
'di';
'ig00';
# $Id: flatten_dependancies.perl,v 3.1.2.1 1996/07/14 07:13:38 hampton Exp $
# $Source: /release/112/cvs/Xsys/scripts/flatten_dependancies.perl,v $
#------------------------------------------------------------------
# flatten_dependancies.perl -- Flatten a dependancies file
#
# June 1996, David Hampton
#
# Copyright (c) 1996, 1997 by cisco Systems, Inc.
# All rights reserved.
#
# This perl script reads an intermediate dependancies file (.D.xxx.c),
# strips out any execss backtracking in dependancy paths, and then
# insures that any given dependancy appears only once.
#------------------------------------------------------------------
# $Log: flatten_dependancies.perl,v $
# Revision 3.1.2.1  1996/07/14  07:13:38  hampton
# Eliminate all redundant dependencies for any given file.  Split out
# platform specific dependencies and put them in the appropriate obj-xxx
# directory.  [CSCdi62810]
# Branch: California_branch
#
# Revision 3.1  1996/07/12  20:14:05  hampton
# New file to flatten paths in dependency lists, and to strip duplicate
# dependencies.
#
#
#------------------------------------------------------------------
# $Endlog$
#

$has_platform_dep = 0;

# What mode were we invoked in?
foreach (@ARGV) {		# 
    if ($_ eq '-p') { $mode_platform = 1; next; }
    if ($_ eq '-l') { $mode_lis = 1; next; }
    if ($_ eq '-h') { $help = 1; next; }
    print STDERR "Usage: $0 [-p] [-l] [-h]\n";
    print STDERR " -p output platform specific includes\n";
    print STDERR " -l output .lis rule\n";
    print STDERR " -h help\n";
    exit 1;
}

if ($help == 1) {
    system("nroff -man $0");
    exit 1;
}

# Default to not stripping any file names with -p switch
if ($mode_platform == 0) {
    &read_strip_list;
}

# Read and clean up the input lines
$lines = &read_file_without_backslash('-'); # read from stdin

# Create the list of dependancy names
($object_file, $source_file, @names) = split(' ', $lines);
$object_file =~ s/://;

# Clean up the dependancies list
foreach $name (@names) {
    $name = &simplify_path($name);
}
if ($mode_platform == 0) {
    @names = &strip_paths(@names);
}
if (scalar(@names) > 1) {
    @names = &unique(sort(@names));
}

# Reformat and print the compacted dependancies list
&dump_file_list;

if ($mode_lis == 1) {
    local($lis_file);
    ($lis_file = $object_file) =~ s/\.o$/.lis/;
    printf("%s: %s\n", $lis_file, $source_file);
}
exit 0;


sub read_file_without_backslash {
    local($file) = @_;
    local($lines) = '';

    if (open(FILE, $file)) {
	local($slash) = $/;
	undef $/;
	$lines = <FILE>;	# slurp the entire file
	close(FILE);
	$/ = $slash;
	$lines =~ s/\\\n/\n/g;	# nuke the backslash characters
    }
    $lines;
}

sub read_strip_list {
    local($lines);
    $lines = &read_file_without_backslash('../dependancies.global');
    $lines =~ s/^\s*PLATFORM_DEPENDANCIES\s*=//; # toss it
    @strip_list = split(' ', $lines);
}

sub simplify_path {
    local($i);
    local($pathname) = @_;
    local(@path) = split(/\//, $pathname);

    if (scalar(@path) > 2) {	# Need enough parts for backtracking
	for ($i = 2; $i < $#path; $i++) {
	    if (($path[$i] eq '..') && ($path[$i - 1] ne '..')) {
		splice(@path, $i - 1, 2);
		$i -= 2;
	    }
	}
	($pathname = join('/', @path)) =~ s#../sys/##;
    }
    $pathname;
}

sub strip_paths {
    local(@filenames) = @_;
    local(@new);

    foreach $file (@filenames) {
	if (!grep($_ eq $file, @strip_list)) {
	    push(@new, $file);
	} else {
	    $has_platform_dep = 1;
	}
    }
    @new;
}

sub unique {
    local(@filenames) = @_;
    local(@new) = ();

    @new = shift(@filenames);
    foreach (@filenames) {
	push(@new, $_) if ($new[$#new] ne $_);
    }
    @new;
}

sub dump_file_list {
    local($first_line, $second_line, $len);

    # The first line is either a variable declaration or a source/object
    # file name pair.  If the latter, check to see if the source file is
    # the only dependancy.
    if ($mode_platform) {
	$first_line = 'PLATFORM_DEPENDANCIES = ';
	$len = length($first_line);
	print($first_line);
    } else {
	printf("%s: %s", $object_file, $source_file);
	if (scalar(@names) == 0) {
	    print("\n");
	    return;
	}
	print(" \\\n");
    }

    # The second line will include the platform dependancies if necessary.
    if ($has_platform_dep == 1) {
	$second_line = ' $(PLATFORM_DEPENDANCIES)';
        $len = length($second_line);
        print($second_line);
    }

    # Now pretty print all the other dependancies.
    foreach $name (@names) {
        if ($len + length($name) > 75) {
	    print(" \\\n");
	    $len = 0;
	}
	print(' ', $name);
	$len += length($name) + 1;
    }
    print("\n");
}

##############################################################################

	# These next few lines are legal in both Perl and nroff.

.00;			# finish .ig
 
'di			\" finish diversion--previous line must be blank
.nr nl 0-1		\" fake up transition to first page again
.nr % 0			\" start at page 1
'; __END__ ############# From here on it's a standard manual page ############
.TH flatten_dependancies 1 "June 28, 1996"
.AT 3
.SH NAME
flatten_dependancies.perl \- Perl script to flattend a dependancies file
.SH SYNOPSIS
flatten_dependancies.perl [-p] [-l] [-h]
.SH DESCRIPTION
.I 
flatten_dependancies.perl
when invoked with the -p switch works in platform mode which will
cause a "PLATFORM_DEPENDANCIES = " line to be generated with a list of
common dependancies.  When invoked with the -l switch also outputs a
lister rule into the dependancies file.  When invoked with the -h
switch it prints this text.
.br
.SH ENVIRONMENT
No environment variables are used.
.SH FILES
Used by sys/depend.make in the generation of .D. dependancies files as
well as sys/dependancies.global and sys/obj-*/dependancies.specific
.SH AUTHOR
David Hampton
.SH "SEE ALSO"
perl(1)
.ex
