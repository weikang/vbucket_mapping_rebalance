bin_PROGRAMS = rebalancer testGen testGenRandom

rebalancer_SOURCES = \
	src/Header.h \
	src/HelperFunctions.cpp \
	src/MappingBalanceR.cpp \
	src/MappingConform.cpp \
	src/Mapping.cpp \
	src/MappingRebalance.cpp \
	src/MappingTopology.cpp \
	src/rebalance.cpp

testGen_SOURCES = \
	src/Header.h \
	src/HelperFunctions.cpp \
	src/MappingBalanceR.cpp \
	src/MappingConform.cpp \
	src/Mapping.cpp \
	src/MappingRebalance.cpp \
	src/MappingTopology.cpp \
	src/testGen.cpp

testGenRandom_SOURCES = \
	src/Header.h \
	src/HelperFunctions.cpp \
	src/MappingBalanceR.cpp \
	src/MappingConform.cpp \
	src/Mapping.cpp \
	src/MappingRebalance.cpp \
	src/MappingTopology.cpp \
	src/testGenRandom.cpp


EXTRA_DIST = slides

run_some_tests: rebalancer
	find testCases/ -type f -print0 | xargs -P3 -0 -n1 -- sh -c './rebalancer <$$1 >> test.log' -
