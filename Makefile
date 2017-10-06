CC=icc
CLFAGS=-std=c++11 -pthread -O3 -xHost
LDLIBS=-ltcmalloc
BINS=LamportQueueX86 LamportQueueAtomic FastForwardQueue
TARGETS=$(addprefix bin/,$(BINS))

all: $(TARGETS)

bin/%: src/%.cc src/LatencyBenchmark.cc
	$(CC) $(CLFAGS) -DWith$* -o $@ $^ $(LDLIBS)

clean:
	rm $(TARGETS)