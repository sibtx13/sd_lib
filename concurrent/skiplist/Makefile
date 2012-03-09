CC=g++
CFLAGS= -Wall -o
SOURCES= test.cpp
EXECUTABLE= skiplist
INCFLAGS	=	-I /usr/local/boost_1_47_0 #-I/usr/local/include/thrift #-I/usr/local/zi_lib
LINKFLAGS	=	-lpthread -ggdb -lrt

.PHONY: all $(EXECUTABLE)

all: $(EXECUTABLE)

$(EXECUTABLE): 
	$(CC) $(SOURCES) $(INCFLAGS) $(LINKFLAGS) $(CFLAGS) $@


clean:
	rm -rf $(EXECUTABLE)