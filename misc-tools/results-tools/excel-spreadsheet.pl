#!/usr/bin/perl -w

# this script generates excel spreadsheet with all ident method results
#  ./results-col.pl && cd results && rm -f tmp.txt &&  paste ./maxmiso-i4-o1-excel.txt   ./multicut-i4-o1-excel.txt  ./singlecut-i4-o1-excel.txt ./union-i4-o1-excel.txt > tmp.txt && cd ..

# this is used to adjust no of rows when there are no results
%bb_no= (
  adpcm    => 35,
  aes      => 180,
  blowfish => 52,
  cjpeg    => 133,
  fft => 42,
  md5 => 23,
  sha => 42,
  sor => 17,
  whetstone => 38
);

my @algos = qw/maxmiso singlecut union multicut/;
my @apps = sort keys %bb_no;

my @oi = `ls results`;
foreach $oi (@oi) {
  chomp $oi;
  next unless -d "results/$oi";

  foreach $algo (@algos) {
    open(DST,"> results/$algo-$oi-excel.txt");
    print DST "$algo\t, size\t, time\t, it\t, cand\t,\n";

    foreach $app (@apps) {
      print DST "\n\n $app";
      my $src_file = "results/${oi}/ident-$algo-$app.txt";
      my @data = split (/\n/, "\t,\t,\t,\n" x$bb_no{$app});
      if (-e $src_file) {
        @data=`grep [0-9] $src_file | grep ","`
      } else {
        print DST "\t\t,\t,\t,\t,\t,\n"; 
      }
      foreach $line (@data) {
        chomp $line;
        print DST "\t\t, $line\t,\n";
      }
    }

  }
}

