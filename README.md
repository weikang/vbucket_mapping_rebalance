vbucket_mapping_rebalance
=========================
(Version 2.0) Implementation of the topology-replication-mapping scheme
-------------------------

### INPUT

(through standard input std::cin)

Input is (4+N) lines of the input mapping with additional lines 
indicating desired node names that makes up the new mapping.

* First 4 lines of integers, which are the four parameters of the input mapping.
    1. `N // the number of vbuckets`
    2. `L // the number of copies`
    3. `M // the number of nodes`
    4. `S // the slave number`

* Then N lines of node names.
    1. A node name is a string not having any spaces or tabs. Each line should have at least one, at most L, strings, separated by spaces/tabs.
    2. Each line represents the nodes for a vBucket. First node is active, and the rest are replicas.
    3. If a line has less than L strings, the undefined node is automatically filled in
at the end of the line, until every vBucket has exactly L nodes.
    4. The node name `undefined` is the only reserved node name, indicating an undefined node.

* At last, until the end of std::cin, are node names that are to be kept in the output mapping, separated by newlines.


### OUTPUT

(through standard output std::cout)

* Some lines of log messages are printed first.

* To indicate the end of log messages, a line of 20 minus signs is printed.

* After that separation line, output mapping is printed in the same format
as the input mapping: a total of (4 + N) lines.

