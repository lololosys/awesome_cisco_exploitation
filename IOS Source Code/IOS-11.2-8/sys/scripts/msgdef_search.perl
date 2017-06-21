#!/bin/sh -- # This line is needed by perl to prevent looping  -*-Perl-*-
eval 'exec perl5 -S $0 ${1+"$@"}'
	if 0;
'di';
'ig00';

# $Id: msgdef_search.perl,v 3.1.2.1 1996/09/14 00:53:38 hampton Exp $
# $Source: /release/112/cvs/Xsys/scripts/msgdef_search.perl,v $
#------------------------------------------------------------------
# Locate and extract error messages from cisco source code.
#
# September 1996, David Hampton
# From a script by Mark Baushke.
#
# Copyright (c) 1996, 1997 by cisco Systems, Inc.
# All rights reserved.
#------------------------------------------------------------------
# $Log: msgdef_search.perl,v $
# Revision 3.1.2.1  1996/09/14  00:53:38  hampton
# Incorporate the Error Messages Manual text into the source code.
# [CSCdi69164]
# Branch: California_branch
#
# Revision 3.1  1996/09/13  23:29:12  hampton
# Placeholder file.
#
#------------------------------------------------------------------
# $Endlog$
#

require "find.pl";
require "getopts.pl";

$STATE_LOOKING = 1;
$STATE_FACILITY = 2;
$STATE_ERRMSG = 3;
$STATE_AFU = 4;			# Oops
$STATE_DONE = 5;

#Read the options early.
$opt_e = 0;
$opt_h = 0;
$opt_i = 0;
&Getopts('ehi');			# Sets opt_* as a side effect
if ($opt_h == 1) {
    system("nroff -man $0");
    exit 1;
}


# find all files named msg_*.c
sub wanted { /^.*\.c$/ && (!/^\.[TD]/) && push(@files,$dir.'/'.$_); }

if (!scalar(@ARGV)) {
    opendir(DIR, '.') || die "Unable to read directory \".\": $!";
    @symlist = ( '.' );
    while($entry = readdir(DIR)) {
	next if ($entry =~ /^\.{1,2}$/);
	push(@symlist, $entry) if (-l $entry && -d $entry);
    }
    closedir(DIR);
    for $entry (@symlist) {
	&find($entry);
    }
}
else {
    &find(shift @ARGV);
}

undef $/;				# suck in entire files
$* = 1;					# multi-line buffer
foreach $file (@files) {
    open(FILE, $file) || die "unable to open $file: $!";
    $_ = <FILE>;			# the entire file
    s/msgdef_limit/msgdef/g;		# Alias msgdef_limit
    next unless (/msgdef\(/ || /facdef\(/);	# Bail if no messages

    # Start output.
    $filename_printed = 0;
    &print_filename if (($opt_e == 0) && ($opt_i == 0));

    # break on C statements
    @text_in = grep(/msgdef|facdef/, split(/\)\s*\;/)); 
    @text_out = ();
    $error_cnt = 0;
    $incomplete = 0;
    $state = $STATE_LOOKING;

    # A startment is everything before a pair of ");" characters.
    foreach(@text_in) {
	s,/\*.*\*/, ,g;			# Remove comments
	s/\s+/ /g;			# Compact whitespace (incl newlines)
	s/^.*facdef/facdef/;		# Remove everything before 'facdef' or
	s/^.*msgdef/msgdef/;		#   'msgdef' if they are present
	$_ .= ");\n";			# Replace the 'break' characters
#	print $_;
	&state_machine($_);
	push(@text_out, "\n") if (m/msgdef\(/ || m/facdef/);
	push(@text_out, $_);
    }
    &new_state($STATE_DONE);
    print @text_out;			# Dump all accumulated text.
    close(FILE);
}


sub print_filename
{
    print "\n\n/* file $file */\n" if ($filename_printed == 0);
    $filename_printed = 1;
}

sub record_error
{
    local($string) = @_;
    push(@text_out, $string);
    $error_cnt++;
}


sub print_errors
{
    &print_filename if ($error_cnt > 0);
    print @text_out if ($error_cnt > 0);
    @text_out = ();
    $error_cnt = 0;
}


sub print_incomplete
{
    &print_filename if ($incomplete > 0);
    print @text_out if ($incomplete > 0);
    @text_out = ();
    $incomplete = 0;
}


sub new_state
{
    local($new_state) = @_;
    if ($state == $STATE_FACILITY) {
	if ($new_state != $STATE_LOOKING) {
	    &record_error("#### Missing msgdef_section macro.\n");
	}
    }
    elsif ($state == $STATE_ERRMSG) {
	if ($dont_document == 0) {
	    &record_error("#### Missing msgdef_explanation macro.\n")
		if $has_expl == 0;
	    &record_error("#### Missing msgdef_recommended_action macro.\n")
		if $has_action == 0;
	    &record_error("#### Missing msgdef_ddts_component macro.\n")
		if $has_ddts == 0;
	}
	if ($new_state == $STATE_LOOKING) {
	    push(@text_out, "#### Missing facdef macro.\n");
	}
    }

    if (($new_state == $STATE_ERRMSG) ||
	($new_state == $STATE_DONE)) {
      &print_errors() if ($opt_e == 1);
      &print_incomplete() if ($opt_i == 1);
    }

    if ($new_state == $STATE_AFU) {
	&record_error("#### Missing msgdef or facdef macro.\n");
    }
    elsif ($new_state == $STATE_ERRMSG) {
	$has_expl = 0;
	$has_action = 0;
	$has_ddts = 0;
	$dont_document = 0;
    }
    $state = $new_state;
}

sub state_machine
{
    local($_) = @_;

    if (($state == $STATE_LOOKING) || ($state == $STATE_AFU)) {
	if (m/^facdef/) {
	    &new_state($STATE_FACILITY);
	} elsif (m/^msgdef\(/) {
	    &new_state($STATE_ERRMSG);
	} else {
	    &new_state($STATE_AFU);
	}	  
    } elsif ($state == $STATE_FACILITY) {
	if (m/^msgdef_section/) {
	    &new_state($STATE_LOOKING);
	} elsif (m/^msgdef\(/) {
	    &new_state($STATE_ERRMSG);
	} else {
	    &new_state($STATE_AFU);
	}
    } elsif ($state == $STATE_ERRMSG) {
	if (m/^facdef/) {
	    &new_state($STATE_FACILITY);
	    $exit = 1;
	} elsif (m/^msgdef\(/) {
	    &new_state($STATE_ERRMSG);
	} elsif (m/^msgdef_section/) {
	    &new_state($STATE_LOOKING);
	} else {
	    if (m/^msgdef_explanation/) {
		&record_error("#### Duplicate msgdef_explanation macro.\n")
		    if $has_expl == 1;
		$incomplete = 1 if /""/;
		$has_expl = 1;
	    }
	    elsif (m/^msgdef_recommended_action/) {
		&record_error("#### Duplicate msgdef_recommended_action macro.\n")
		    if $has_action == 1;
		$incomplete = 1 if /""/;
		$has_action = 1;
	    }
	    elsif (m/^msgdef_ddts_component/) {
		&record_error("#### Duplicate msgdef_ddts_component macro.\n")
		    if $has_ddts == 1;
		$has_ddts = 1;
	    }
	    elsif (m/^msgdef_do_not_document/) {
		&record_error("#### Duplicate msgdef_do_not_document macro.\n")
		    if $dont_document == 1;
		$dont_document = 1;
	    }
	}
    }
}


##############################################################################

	# These next few lines are legal in both Perl and nroff.

.00;			# finish .ig
 
'di			\" finish diversion--previous line must be blank
.nr nl 0-1		\" fake up transition to first page again
.nr % 0			\" start at page 1
'; __END__ ############# From here on it's a standard manual page ############
.TH msgdef_search 1 "August 28, 1996"
.AT 3
.SH NAME
msgdef_search.perl \- Perl script to extract error message information
.SH SYNOPSIS
msgdef_search.perl [-e | -h | -i ] [ <dirs> ]
.SH DESCRIPTION
.I 
msgdef_search.perl
when invoked with no options extracts all error messages (and their
associated text) from the directory tree starting with the specified
directory.  If no directory is specified the search starts with the
current directory.  When invoked with the -e switch prints out all
errors in declaration of error messages.  When invoked with the -i
switch prints out all errors message declarations that are incomplete.
When invoked with the -h switch it prints this text.
.br
.SH ENVIRONMENT
No environment variables are used.
.SH FILES
None
.SH AUTHOR
David Hampton
.SH "SEE ALSO"
perl(1)
.ex
