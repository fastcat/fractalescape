#!/usr/bin/perl -w

use strict;
use warnings;

binmode STDOUT;
$| = 1;

my %done;

while (@ARGV) {
	my $fractname = shift @ARGV;
	if (length($fractname) > 12) {
		if ($fractname =~ /^([a-z]{12})[^a-z]/i) {
			$fractname = $1;
		}
	}
	die unless length($fractname) == 12;
	next if $done{$fractname};
	$done{$fractname} = 1;
	my @coeffs = map {c2v($_) / 10.0} split //, $fractname;
	die unless $#coeffs == 11;
	my $ufm = <<UFM;
FractalEscape $fractname {
	global:
	init:
		z = #pixel
	loop:
		float x = real(z)
		float y = imag(z)
		float xnew = $coeffs[0] + x * ($coeffs[1] + x * $coeffs[2] + y * $coeffs[3]) + y * ($coeffs[4] + y * $coeffs[5])
		float ynew = $coeffs[6] + x * ($coeffs[7] + x * $coeffs[8] + y * $coeffs[9]) + y * ($coeffs[10] + y * $coeffs[11])
		z = xnew + flip(ynew)
	bailout:
		|z| < \@bailout
	default:
		title = "FractalEscape $fractname"
		float param bailout
			caption = "Bail-out value"
			hint = "Maximum x^2 + y^2 before bailing out"
			default = 1000000
			min = 1
			max = 1000000000
		endparam
		maxiter = 100
		periodicity = 0
		method = guessing
}
UFM
	# get us some dos newlines biotch
	$ufm =~ s/\n/\r\n/g;
	print $ufm;
}


sub c2v($) {
	my $char = shift;
	die if length($char) != 1;
	if ($char ge 'a' and $char le 'z') {
		return (ord($char) - ord('a')) - 25;
	} elsif ($char ge 'A' and $char le 'Z') {
		return (ord($char) - ord('A')) + 1;
	} else {
		die;
	}
}
