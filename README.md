vbucket_mapping_rebalance
=========================

(Version 3.0) Added node tagging feature.

(Version 2.1) Added non-indexed optimization step

(Version 2.0) Implementation of the topology-replication-mapping scheme

### INPUT

(through standard input stdin)

Input is (4+N) lines of the input mapping, then 3 lines of integers
indicating the desired parameter of the mapping, and at last additional lines 
indicating desired node names and their tags that make up the new mapping. Please see `samples/` for 2 examples.

* First 4 lines of integers, which are the four parameters of the input mapping.
    1. `N // the number of vbuckets`
    2. `L // the number of copies`
    3. `M // the number of nodes`
    4. `S // the slave number`

* Then N lines of node names.
    1. A node name is a string not having any whitespaces. Each line should have exactly L node names, separated by spaces or tabs.
    2. Each line represents the nodes for a vBucket. First node is active, and the rest are replicas.
    3. The node name `undefined` is the (only) reserved node name, indicating an undefined node.

* Then 3 lines of parameters for the new mapping.
    1. First line: the slave number `S` of the new mapping.
    2. Second line: the number of copies `L` for the new mapping.
    3. Third line: number of tags of the node names for the new mapping.

* The last part of input is the nodes to make up the new vBucket mapping. Until the end of stdin, are node names that are to be kept in the new mapping. Each line has one node name, followed by a number of node tags separated by space or tab. The number of node tags must match the number provided above as the third parameter of the new mapping. Node tag is a string without any white space in it. Nodes will prefer to replicate to other nodes with different tags.

### OUTPUT

(through standard output stdout)

Please see `samples/` for 2 examples.

* Some lines of log messages are printed first.

* To indicate the end of log messages, a line of 20 minus signs is printed.

* After that separation line, output mapping is printed in the same format
as the input mapping: a total of (4 + N) lines.

### rebalancer

`./rebalance [price]...` where `[price]...` are positive integers indicating the _prices_ of the corresponding node tag. The higher the price, the greater deferrence is given to that tag to make sure that nodes replicate to different tags. If the number of prices entered is less than the number of tags included in the input, the default tag price is set to 10. Excessive prices are ignored.

### test cases generation

This is used to generate test cases for rebalancer.

To generate new test cases, call `./testGenRandom int1 int2 >/dev/null` with two positive integers as parameters. We have `int1` to be the number of test cases to generate in the directory `./testCases/`, and `int2` is the seed for the random number generator. They have default value 9 and 1, respectively.

(To generate 100 by 100 matrix test cases, call `./testGen L seed >/dev/null` with two positive integers as parameters. We have `L` to be the number of copies for the test cases, and `seed` is the seed for the random number generator, which has default value 1.)

To test these generated cases, use `make run_some_tests`. The results will be partially appended to `test.log`. (Average running time is 1.7 seconds for v2.0)


