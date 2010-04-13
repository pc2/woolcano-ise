#!/usr/bin/perl -w

# this script generates excel spreadsheet with all ident method 
#  ./excel-spreadsheet.pl && cd $RES_DIR && rm -f tmp.txt &&  paste ./maxmiso-i4-o1-excel.txt   ./multicut-i4-o1-excel.txt  ./singlecut-i4-o1-excel.txt ./union-i4-o1-excel.txt > tmp.txt && cd ..


if ($#ARGV == -1) {
  printf STDERR "usage: $0 dir_with_res (dst_dir)\n";
  exit 1;
}
my $RES_DIR = $ARGV[0];
my $DST_DIR = $ARGV[1] || $RES_DIR;
mkdir $DST_DIR unless -d $DST_DIR;

# this is used to adjust no of rows when there are no $RES_DIR
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

my @oi = `ls $RES_DIR`;
foreach $oi (@oi) {
  chomp $oi;
  #  skip if thats not a dir
  next unless -d "$RES_DIR/$oi";
  #  skip if thats the dir with results
  next if $oi =~  /$DST_DIR/;

  foreach $algo (@algos) {
    open(DST,"> $DST_DIR/$algo-$oi-excel.txt");
    print DST "$algo\t, size\t, time\t, it\t, cand\t,\n";

    foreach $app (@apps) {
      print DST "\n\n $app";
      my $src_file = "$RES_DIR/${oi}/ident-$algo-$app.txt";
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

foreach $oi (@oi) {
  chomp $oi;
  #  skip if thats not a dir
  next unless -d "$RES_DIR/$oi";
  #  skip if thats the dir with results
  next if $oi =~  /$DST_DIR/;


  my $files = "";
  foreach $algo (@algos) {
    $files .= "$DST_DIR/$algo-$oi-excel.txt ";
  }
  system("paste $files > $DST_DIR/$oi.txt");
}
