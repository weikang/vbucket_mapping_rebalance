ACLOCAL_AMFLAGS = -I m4

AM_CPPFLAGS = @BOOST_CPPFLAGS@
AM_LDFLAGS = @BOOST_LDFLAGS@

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
	find testCases/ -type f -print0 | xargs -P2 -0 -n1 -- sh -c './rebalancer <$$1 | head -n2' --
