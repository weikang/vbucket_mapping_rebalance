bin_PROGRAMS = rebalancer testGenerate

rebalancer_SOURCES = \
	src/Header.h \
	src/HelperFunctions.cpp \
	src/MappingBalanceR.cpp \
	src/MappingConform.cpp \
	src/Mapping.cpp \
	src/MappingRebalance.cpp \
	src/MappingTopology.cpp \
	src/rebalance.cpp

testGenerate_SOURCES = \
	src/Header.h \
	src/HelperFunctions.cpp \
	src/MappingBalanceR.cpp \
	src/MappingConform.cpp \
	src/Mapping.cpp \
	src/MappingRebalance.cpp \
	src/MappingTopology.cpp \
	src/testGen.cpp

EXTRA_DIST = slides

run_some_tests: rebalancer
	find testCases/ -type f -print0 | xargs -P3 -0 -n1 -- sh -c 'echo "****"$$1"****" >> test.log; ./rebalancer <$$1 | head -n10 >> test.log' -
