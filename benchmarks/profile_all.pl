#!/usr/bin/perl -w

my $srcdir = `llvm-config --src-root`;
chomp $srcdir;
my $profile_script = $srcdir . "/utils/profile.pl";

chdir "adpcm";
system "make clean";
system "make";
system "rm -f llvmprof.out";
system "perl $profile_script -block timing_.bc";
system "cp timing_.bc ..";
system "cp llvmprof.out ../timing_.prof";

chdir "../aes";
system "make clean";
system "make";
system "rm -f aes.prof";
system "perl $profile_script -block -o aes.prof aes.bc";
system "cp aes.prof aes.bc ..";

chdir "../blowfish";
system "make clean";
system "make";
system "rm -f blowfish.prof";
system "perl $profile_script -block -o blowfish.prof blowfish.bc";
system "cp blowfish.prof blowfish.bc ..";

chdir "../cjpeg";
system "make clean";
system "make";
system "rm -f cjpeg_.prof";
system "perl $profile_script -block -o cjpeg_.prof cjpeg_.bc -dct float -progressive -opt -outfile output1/output_small_encode_.jpeg input1/input_small.ppm";
system "cp cjpeg_.prof cjpeg_.bc ..";

chdir "../fft";
system "make clean";
system "make";
system "rm -f fft_test.prof";
system "perl $profile_script -block -o fft_test.prof fft_test.bc";
system "cp fft_test.prof fft_test.bc ..";

chdir "../md5";
system "make clean";
system "make";
system "rm -f md5_test_.prof";
system "perl $profile_script -block -o md5_test_.prof md5_test_.bc";
system "cp md5_test_.prof md5_test_.bc ..";

chdir "../sha";
system "make clean";
system "make";
system "rm -f sha_test_.prof";
system "perl $profile_script -block -o sha_test_.prof sha_test_.bc";
system "cp sha_test_.prof sha_test_.bc ..";

chdir "../sor";
system "make clean";
system "make";
system "rm -f sor_test.prof";
system "perl $profile_script -block -o sor_test.prof sor_test.bc";
system "cp sor_test.prof sor_test.bc ..";

chdir "../whetstone";
system "make clean";
system "make";
system "rm -f whetstone.prof";
system "perl $profile_script -block -o whetstone.prof whetstone.bc 10000";
system "cp whetstone.prof whetstone.bc ..";
