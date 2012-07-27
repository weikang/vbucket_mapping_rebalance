NAME = rebalance

SRC_DIR  = src
ODIR = obj

_OBJS = HelperFunctions.o Mapping.o MappingConform.o MappingTopology.o MappingBalanceR.o MappingRebalance.o $(NAME).o
OBJS = $(patsubst %,$(ODIR)/%,$(_OBJS))
HH = $(SRC_DIR)/Header.h

CC = g++
DEBUG = -Wall -g -ggdb -O3
BOOST = -I ~/boost_1_50_0
CFLAGS = -c $(DEBUG) $(BOOST)
LFLAGS = $(DEBUG) $(BOOST)

$(NAME) : $(OBJS) $(HH)
	$(CC) $(LFLAGS) $(OBJS) -o $(NAME)

$(ODIR)/%.o: $(SRC_DIR)/%.cpp $(HH)
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -rf obj/*.o

