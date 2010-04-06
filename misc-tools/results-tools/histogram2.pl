#!/usr/bin/perl -w

my @apps = qw/adpcm aes cjpeg blowfish fft md5 sha sor whetstone /;
my %res;

foreach $app (@apps) {
#  printf $app . "\n";
  foreach $line (`llvm-bcanalyzer $app/$app.bc 2>&1 | grep INST_`) {
    my ($cnt, $inst) = ($line =~ /^\s+(\d+).*INST_(.*)/);
    $res{$inst}{$app} = $cnt;
#    printf $cnt . " " . "$inst\n";
  }
}

printf "%-20s", "";
foreach $app (@apps) { printf ",%-20s", "$app" }
print "\n";

foreach $inst (sort keys %res) {
  printf "%-20s", $inst;
  foreach $app (@apps) {
    my $val = ${$res{$inst}}{$app} || 0;
    printf ",%-20s", $val;
  }
  print "\n";
}
