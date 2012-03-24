CC=g++
CFLAGS= -Wall -o
SOURCES= ../../lib/gc.cc
EXECUTABLE= test p_test
INCFLAGS	=	-I /usr/local/boost_1_49_0 -I ../../include -std=c++0x -include ../../include/param.h -include ../../include/compiler.h 
LINKFLAGS	=	-lpthread -ggdb -lrt /usr/local/lib/libboost_thread.a

.PHONY: all $(EXECUTABLE)

all: $(EXECUTABLE)

$(EXECUTABLE): 
	$(CC) $(SOURCES) $@.cpp $(INCFLAGS) $(LINKFLAGS) $(CFLAGS) $@


clean:
	rm -rf $(EXECUTABLE)