# --------------------------
make.sh     - compiles, ident, select, estimation
run-all.sh  - runs make.sh for all apps

# --------------------------
make_bench.sh    - compile, prepare, ident benchmarking mode, ident cand
run-all-bench.sh - runs make_bench.sh for all apps.

# --------------------------
histogram1.sh   - info about apps
histogram2.pl
histogram.sh

# --------------------------
compile-all.sh  - compile all apps with gnu autotools (not with Cmake)
                - used for obtaining front-end & unrolling pass perf.

# --------------------------
excel-spreadsheet.pl  - reads results and produces excel spreadsheet for it

# --------------------------
res-rm-not-finished.sh - remove files which do not have results
gen-cmd-lst.sh         - generate pool of jobs for bash_threads tool
threads-resume.sh      - resume threads
