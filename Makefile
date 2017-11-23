CC=icc
CFLAGS=-std=c++11 -O3
LDLIBS=-ltcmalloc
ifeq ($(CC), icc)
CFLAGS+=-pthread -xHost
LDLIBS+=
else
ifeq ($(CC), g++)
CFLAGS+=
LDLIBS+=-lpthread
endif
endif

CPPFLAGS=
#PRODBATCH

BINS=LamportQueueX86 LamportQueueAtomic FastForwardQueue 
#DPDKQueue
TARGETS=$(addprefix bin/,$(BINS))

all: $(TARGETS)

bin/%: src/%.cc src/%.h src/Queue.h src/Benchmark.cc
	$(CC) $(CFLAGS) $(addprefix -D,With$*) $(addprefix -D,$(CPPFLAGS)) -o $@  src/$*.cc src/Benchmark.cc $(LDLIBS)

clean:
	rm -f $(TARGETS)