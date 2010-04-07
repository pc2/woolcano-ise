#!/usr/bin/perl -w

use CGI::Carp qw(fatalsToBrowser warningsToBrowser);
use CGI qw/:standard/;

my $bb;

# full path name
my $srcfile = $ARGV[0];

# separate dir and fname
my ($dir, $fname) = ($srcfile=~ /^(.*)\/(.*)$/);

# get filenename without extension
my ($name) = ($fname =~ /^(.*)\./);

my $fs_dotsdir  = "$dir/$name/dots";
my $web_dotsdir = "dots/b_test_link/$name/dots";

@res=`egrep "profile|selected" $srcfile | grep -v "not selected"`;

# print "srcfile = $srcfile \n dir = $dir \n fname = $fname\n dotsdir = $dotsdir\n\n\n";

my $bb = "";
foreach $line (@res)  {
  if ( $line =~ /^(profile.*) (\S+)$/ )  {
    # profile: [49.73%]
    $data = $1;
    # main.bb
    $bb = $2 ; 
    # main-bb
    $bb =~ s/\./-/;
    # find files matching c
    my $dotfile = (grep(!/cand/, glob("$fs_dotsdir/$bb\_*.gv")))[0];
    my ($dotfilename) =  ($dotfile =~ /^.*\/(.*)$/);
    # print "dotfile = $dotfile\ndotfilename = $dotfilename\n";

    print "\n$data ", a( {-href=>"/cgi-bin/visualize/$web_dotsdir/$dotfilename"}, "$dotfilename") , "\n";
    next;
  } elsif ( $line =~ /^\- (\S+)(:.*)$/) {
    $cand = $1;
    $d = $2;

    # figure out full name of candidate
    my $pat = "$fs_dotsdir/$bb\_$cand";
    # print "\npattern = $pat\n";
    my $dotfile = (glob("$pat"))[0];
    my ($dotfilename) =  ($dotfile =~ /^.*\/(.*)$/);

    print "- " , 
      a( {-href=>"/cgi-bin/visualize/$web_dotsdir/$dotfilename"}, "$cand") , $d, "\n";
#    print $d , "\n";
    next;
  }
  print $line;
}
