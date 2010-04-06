#!/usr/bin/perl -w

my $bb_cnt = 0;
my $bb_prof = 0;

my @nodes = ();
my @cand  = ();
my $cand_sel_cnt = 0;
my @cand_sel = ();

my $est_sw = 0;
my $est_hw = 0;
my $est_ratio = 0;

my $ISE_maxudi   = "";
my $ISE_maxinput = "";

sub reset_stats() {
    $bb_cnt = 0;
    $bb_prof = 0;
    @nodes = ();
    @cand  = ();
    $cand_sel_cnt = 0;
    @cand_sel = ();

    $est_sw = 0;
    $est_hw = 0;
    $est_ratio = 0;

    $ISE_maxudi   = "";
    $ISE_maxinput = "";
}
sub show() {

    
    print "  arch:               $arch\n";
    print "  algo_ident:         $algo_ident\n";
    print "  algo_sel:           $algo_sel\n";

    my @nodes_sort = sort { $b <=> $a }  @nodes;
    my $nodes_cnt = 0; foreach $n (@nodes) { $nodes_cnt += $n }

    my @cand_sort = sort { $b <=> $a }  @cand;
    my $cand_cnt = 0; foreach $c (@cand) { $cand_cnt += $c }

    print "  lim. by MaxUDI:     $ISE_maxudi\n";
    print "  lim. by MaxInput:   $ISE_maxinput\n";
    print "  bb_cnt:             $bb_cnt\n";
    print "  nodes:              @nodes_sort \n";
    print "  nodes_cnt:          $nodes_cnt \n";
    print "  cand in bb:         @cand_sort \n";
    print "  cand_cnt:           $cand_cnt \n";

    print "  cand_sel_cnt:       $cand_sel_cnt \n" if ($cand_sel_cnt > 0);
    print " !cand_sel_cnt:       $cand_sel_cnt \n" if ($cand_sel_cnt == 0);

    # foreach $cand_sel (@cand_sel) {
    #  print "    $cand_sel\n";
    # }
    print "  est_sw:             $est_sw \n";
    print "  est_hw:             $est_hw \n";
    print "  est_ratio:          $est_ratio \n";

    reset_stats()
}


while( <> ) {
  my ($line) = $_;
  if ( $line =~ /^app/ )      { print "\n$line"; }
  if ( $line =~ /^gcc|^opt/ ) { print "$line"; }

  if ( $line =~ /^Selected architecture: (\S+)/ ) { $arch = $1 }
  if ( $line =~ /^Identification \S+: (\S+)/ )    { $algo_ident = $1 }
  if ( $line =~ /^Selection algorithm: (\S+)/ )   { $algo_sel  = $1 }
  if ( $line =~ /^Selection algorithm: (\S+)/ )   { $algo_sel  = $1 }
  if ( $line =~ /^ISEPass: limitedMaxUDI (\S+)/ )       { $ISE_maxudi   = $1 }
  if ( $line =~ /^ISEPass: limitedMaxInput (\S+)/ )     { $ISE_maxinput = $1 }

  if ( $line =~ /Processing DFG.* with (\d+) .* (\d+)/) {
    my ($n, $c) = ($1, $2);
    $bb_cnt++;
    push (@nodes, $n);
    push (@cand, $c);
  }

  if ( $line =~ /^profile: (\S+)/ ) { $bb_prof = $1 }
  if ( $line =~ /(.*\d\s+ selected)/ ) { push (@cand_sel, "$bb_prof $1") }

  if ( $line =~ /^Selected (\d+) templates/ ) { $cand_sel_cnt = $1 }
  if ( $line =~ /estimated 'software' runtime: (\d+)/ ) { $est_sw = $1 }
  if ( $line =~ /estimated 'hardware' runtime: (\d+)/ ) { $est_hw = $1 }
  if ( $line =~ /^\s+ratio: ([\d\.]+)/ )                { 
    $est_ratio = $1;
    show();
  }

}
