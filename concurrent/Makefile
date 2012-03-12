CC=g++
CFLAGS= -Wall -o
SOURCES= test.cpp
EXECUTABLE= atomic_test
INCFLAGS	=	-I /usr/local/boost_1_49_0
LINKFLAGS	=	-lpthread -ggdb -lrt

.PHONY: all $(EXECUTABLE)

all: $(EXECUTABLE)

$(EXECUTABLE): 
	$(CC) $(SOURCES) $(INCFLAGS) $(LINKFLAGS) $(CFLAGS) $@


clean:
	rm -rf $(EXECUTABLE)