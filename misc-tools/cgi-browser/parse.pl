#!/usr/bin/perl -w
use CGI::Carp qw(fatalsToBrowser warningsToBrowser);
use CGI qw/:standard/;


$current_time = localtime;

print header,
      start_html("parser"),
     "The current time is $current_time.", "\n",
     br, hr, "\n";

if (param('srcfile')) {
  $srcfile = param('srcfile');
  $res=`/Users/zuza/Work/src-repos/woolcano-ise/b_test/parse.pl $srcfile`;
  print pre($res), hr , "\n";

  $res=`/Users/zuza/Work/src-repos/woolcano-ise/b_test/htmlize.pl $srcfile`;
  print pre($res), "\n";
}

print
     hr,
     end_html;
