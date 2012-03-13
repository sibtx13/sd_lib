CC=g++
CFLAGS= -Wall -o
SOURCES= test.cpp
EXECUTABLE= test p_test
INCFLAGS	=	-I /usr/local/boost_1_49_0
LINKFLAGS	=	-lpthread -ggdb -lrt /usr/local/lib/libboost_thread.a

.PHONY: all $(EXECUTABLE)

all: $(EXECUTABLE)

$(EXECUTABLE): 
	#$(CC) $(SOURCES) $(INCFLAGS) $(LINKFLAGS) $(CFLAGS) $@
	$(CC) $@.cpp $(INCFLAGS) $(LINKFLAGS) $(CFLAGS) $@


clean:
	rm -rf $(EXECUTABLE)