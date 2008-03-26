#!/usr/bin/perl 
#===============================================================================
#
#         FILE:  log-parser.pl
#
#        USAGE:  ./log-parser.pl 
#
#  DESCRIPTION:  
#
#      OPTIONS:  ---
# REQUIREMENTS:  ---
#         BUGS:  ---
#        NOTES:  ---
#       AUTHOR:   (), <>
#      COMPANY:  
#      VERSION:  1.0
#      CREATED:  13.03.2008 14:51:03 CET
#     REVISION:  ---
#===============================================================================

use strict;
use warnings;
use Getopt::Long;
use Data::Dumper;

my $log_file = "/home/hickert/log.gosa-si-server"; 
#my $log_file = "/var/log/gosa-si-server.log"; 
my $within_session = 0;
my $within_incoming = 0;
my $within_header = 0;
my $session;
my $incoming;
my $header;
my $syslog_state;

sub check_syslog_state {
	my ($line) = @_ ;
	if($line =~ /$syslog_state:/ ) {
		return $line;
	}
	return;
}

sub check_header {
	my ($line) = @_ ;

	# new header, set all values back to default
	if ($line =~ /INFO: Incoming msg with header/ ) {
		$within_header = 0; 
	}

	if ($line =~ /INFO: Incoming msg with header '$header'/) {
		$within_header = 1;
		return $line;
	} else {
		if ($within_header) { return $line; }
	}
	return;
}

sub check_incoming {
	my ($line) = @_ ;

	# new incoming msg, set all values back to default
	if ($line =~ /INFO: Incoming msg with session ID \d+ from/ ) {
		$within_incoming = 0;
	} 

	if ($line =~ /INFO: Incoming msg with session ID \d+ from '$incoming'/) {
		$within_incoming = 1;
		return $line;
	} else {
		if ($within_incoming) { return $line; } 
	}
	return;
}

sub check_session {
	my ($line) = @_ ;
	
	if ($line =~ /gosa-si-server (\d+) / ) {
		if ((defined $1) && ($1 eq $session)) {
			$within_session = 1;	
			return $line;
		} else { $within_session = 0; }
	} else {
		if ($within_session == 1) { return $line; } 
	}
	return;
}



### MAIN ######################################################################

GetOptions(
		"s|session=s"  => \$session,
		"i|incoming=s" => \$incoming,
		"h|header=s"   => \$header,
		"l|log=s" => \$syslog_state,
		);

# check script pram
my $script_pram = {};
if (defined $session) { 
	print "session: $session\n";
	$script_pram->{'session'} = $session;
	}
if (defined $incoming) { 
	print "incoming: $incoming\n";
	$script_pram->{'incoming'} = $incoming;
} 
if (defined $header) {
	print "header: $header\n";
	$script_pram->{'header'} = $header;
}
if (defined $syslog_state) {
	print "syslog_state: $syslog_state\n"; 
	$script_pram->{'syslog_state'} = $syslog_state;
}

if (keys(%$script_pram) == 0) {
	# print usage and die
	print "exiting script\n"; 
	exit(0);
}

open(FILE, "<$log_file") or die "\t can not open log-file"; 
my @lines;
my $positive_msg = 0;
# Read lines
while ( my $line = <FILE>){
    chomp($line);

	# start of a new message, plot saved log lines
	if ($line =~ /INFO: Incoming msg with session ID \d+ from / ) {
		if ($positive_msg) {
			print "#####################################################\n";
			print join("\n", @lines)."\n"; 
		}

		$positive_msg = 0;
		$within_session = 0;
		$within_header = 0;
		$within_incoming = 0;
		@lines = ();
	}

	push (@lines, $line); 

	my $positiv_counter = 0;
	while (my ($pram, $val) = each %$script_pram) {
		if ($pram eq 'session') {
			my $l = &check_session($line);
			if (defined $l) { $positiv_counter++; } 
		}

		elsif ($pram eq 'incoming') {
			my $l = &check_incoming($line);
			if (defined $l) { $positiv_counter++; } 
		}
		
		elsif ($pram eq 'header') {
			my $l = &check_header($line);
			if (defined $l) { $positiv_counter++; } 
		}
		
		elsif ($pram eq 'syslog_state') {
			my $l = &check_syslog_state($line);
			if (defined $l) { $positiv_counter++; }
		}
	}

	if (keys(%$script_pram) == $positiv_counter) {
		$positive_msg = 1;
	}

}



