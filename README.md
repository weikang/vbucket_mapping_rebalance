vbucket_mapping_rebalance

version 2.0
Implementation of topology-replication-mapping scheme


INPUT: (through standard input std::cin)

First 4 lines of parameters of the input mapping
N // the number of vbuckets
L // the number of copies
M // the number of nodes
S // the slave number

Then N lines of node names.
Node name is a string not having spaces or tabs.
Each line should have at least one, at most L, strings,
separated by spaces/tabs.
If a line has less than L strings,
the node undefined is automatically filled in its place
until every vbucket has exactly L nodes.

Then, until eof, are node names.
Each line should be a string of node name to be kept in the output mapping.


OUTPUT: (through standard output std::cout)

Some lines of log messages are printed.

To indicate the end of log messages, a line of 20 minus signs is printed.

After that line, output mapping is printed in the same manner
as the input mapping: a total of (4 + N) lines.

=========================
