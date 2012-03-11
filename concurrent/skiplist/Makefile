CC=g++
CFLAGS= -Wall -Werror -o
SOURCES= test.cpp
EXECUTABLE= skiplist
INCFLAGS	=	-I /usr/local/boost_1_49_0
LINKFLAGS	=	-lpthread -ggdb -lrt

.PHONY: all $(EXECUTABLE)

all: $(EXECUTABLE)

$(EXECUTABLE): 
	$(CC) $(SOURCES) $(INCFLAGS) $(LINKFLAGS) $(CFLAGS) $@


clean:
	rm -rf $(EXECUTABLE)