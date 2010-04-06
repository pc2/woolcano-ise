loop/     - used to find out when oop-unrolling occours (with what kind
          of passes for llvm-gcc and opt trigger unrolling the loop)

ir/       - used to manipulate IR by hand, visualize DAGs etc

bb_info/  - visualize Basic Blocks with GraphViz (it is not possible to do
          that on BB level with 'opt' tool)

ise_dots/ - examples with graphviz (dot) graphs which visualize how the
          identification and selection algorithms are working
