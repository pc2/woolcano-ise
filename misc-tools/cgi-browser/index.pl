#!/usr/bin/perl -w
use CGI::Carp qw(fatalsToBrowser warningsToBrowser);
use CGI qw/:standard/;

symlink "/Users/zuza/Work/src-repos/woolcano-ise/b_test", "../Documents/dots/b_test_link";
my $dirpath = "/Users/zuza/Work/src-repos/woolcano-ise/b_test";
$dirpath=param('path') if param('path');

use vars qw/@FolderContent @Folders @Files/;

## Change current path/folder to $dirpath.
chdir $dirpath;
$dirpath=`pwd`; chomp $dirpath;
unless (opendir DIR,'./') { print "Cannot open directory $dirpath: $!\n"; exit; }
@FolderContent = sort readdir DIR;
closedir DIR;

## Go through each objects collected.
foreach $FolderContent(@FolderContent) {
  if (-d $FolderContent) {
    push(@Folders, $FolderContent);
  } else {
    push(@Files, $FolderContent);
  }	
}

## Print header and HTML title (to be viewable in browser).
print header,start_html($dirpath);
foreach $Folders(@Folders) {
  print a( {-href=>"/cgi-bin/index.pl?path=$dirpath/$Folders"}, "$Folders/"), br, "\n";
}

print hr, "\n";

my $local_dir = $dirpath;
$local_dir=~ s/^.*b_test\/?//g;

foreach $Files(@Files) {
  if ( $Files =~ /\.gv$/) {
    print a( {-href=>"/cgi-bin/visualize/dots/b_test_link/$local_dir/$Files"}, $Files), br, "\n";
  } elsif ( $Files =~ /txt$/) {
    print a( {-href=>"http://localhost/dots/b_test_link/$local_dir/$Files"}, $Files), br, "\n";
  } elsif ( $Files =~ /log$/) {
    print a( {-href=>"http://localhost/dots/b_test_link/$local_dir/$Files"}, "$Files"), br, "\n";
    print a( {-href=>"/cgi-bin/parse.pl?srcfile=$dirpath/$Files"}, $Files ."_run_parser"), br, "\n";
  } else {
    print "$Files<br>\n";
  }
}

