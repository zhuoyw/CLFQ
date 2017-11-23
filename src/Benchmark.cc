#include <pthread.h>
#include <semaphore.h>
#include <cassert>
#include <chrono>
#include <iostream>
#include "Queue.h"
#include "LamportQueueX86.h"
#include "LamportQueueAtomic.h"
#include "FastForwardQueue.h"
// #include "DPDKQueue.h"

#define LOOPSIZE 100000000
#define SKIPSIZE 10000

pthread_barrier_t bar;

class Context {
public:
	Queue& q;
	Context(Queue& queue) : q(queue) {} 
};

using Clock = std::chrono::high_resolution_clock;
using TimePoint = std::chrono::time_point<Clock>;

uint64_t tsc()
{
    uint64_t    time;
    uint32_t    msw   , lsw;
    __asm__     __volatile__("rdtsc\n\t"
                "movl %%edx, %0\n\t"
                "movl %%eax, %1\n\t"
                :         "=r"         (msw), "=r"(lsw)
                :   
                :         "%edx"      , "%eax");
    time = ((uint64_t) msw << 32) | lsw;
    return time;
}

void spin_tsc(uint64_t count)
{
	uint64_t time = tsc() + count;
	while (tsc() < time) {}
}

void printTimeDuration(TimePoint tstart, TimePoint tend) {
	auto nano = std::chrono::duration_cast<std::chrono::nanoseconds>(tend - tstart).count();
	std::cout << "Latency(?):" << nano / LOOPSIZE << "ns" << std::endl;
	std::cout << "Throughput:" << LOOPSIZE / (nano * 1e-9) << "op/s" << std::endl;
}

void printCycleDuration(uint64_t cstart, uint64_t cend) {
	uint64_t c = cend - cstart;
	std::cout << "Latency(?):" << c / LOOPSIZE << "cycles" << std::endl;
}

void *func1(void* arg) {
	Context* pctx = (Context*)arg;
	Queue& q = pctx->q;

	// pthread_barrier_wait(&bar);
	TimePoint tstart;
	uint64_t cstart;
	queue_msg_t msg;
	for (int i = 0; i < LOOPSIZE + SKIPSIZE; ++i) {
		if (i == SKIPSIZE) { cstart = tsc(); tstart = Clock::now(); }
		msg.data[0] = i + 1;
		while (!q.enqueue(msg));
	}
	uint64_t cend = tsc();
	TimePoint tend = Clock::now();
	printTimeDuration(tstart, tend);
	printCycleDuration(cstart, cend);
}

void *func2(void* arg) {
	Context* pctx = (Context*)arg;
	Queue& q = pctx->q;

	// pthread_barrier_wait(&bar);
	// TimePoint tstart;
	queue_msg_t msg;
	for (int i = 0; i < LOOPSIZE + SKIPSIZE; ++i) {
		// if (i == SKIPSIZE) tstart = Clock::now();
		while (!q.dequeue(msg));
		// if (msg.data[0] != i+1) {
		// 	fprintf(stderr, "Error: data%llu expect%llu\n", msg.data[0], i+1);
		// 	abort();
		// }
	}
	// TimePoint tend = Clock::now();
	// printTimeDuration(tstart, tend);
}


int main(int argc, char const *argv[])
{
	if (argc != 3) {
		std::cerr << "Usage: app cpu1 cpu2" << std::endl;
		return 0;
	}

	int cpu1 = strtol(argv[1], NULL, 10);
	int cpu2 = strtol(argv[2], NULL, 10);
	pthread_attr_t attr1, attr2;
	pthread_attr_init(&attr1);
	pthread_attr_init(&attr2);
    cpu_set_t cpus1, cpus2;
    CPU_ZERO(&cpus1);
    CPU_ZERO(&cpus2);
    CPU_SET(cpu1, &cpus1);
    CPU_SET(cpu2, &cpus2);
    pthread_attr_setaffinity_np(&attr1, sizeof(cpu_set_t), &cpus1);
    pthread_attr_setaffinity_np(&attr2, sizeof(cpu_set_t), &cpus2);
    pthread_barrier_init(&bar, NULL, 2);

	#if defined(WithLamportQueueX86) 
	LamportQueueX86 q(1<<12);
	#elif defined(WithLamportQueueAtomic)
    LamportQueueAtomic q(1<<12);
    #elif defined(WithFastForwardQueue)
	FastForwardQueue q(1<<12);
    #elif defined(WithDPDKQueue)
	DPDKQueue q(1<<12);
    #endif

	Context ctx(q);
	pthread_t thread1, thread2;
	pthread_create(&thread1, &attr1, func1, (void*)(&ctx));
	pthread_create(&thread2, &attr2, func2, (void*)(&ctx));

	pthread_join(thread1, NULL);
	pthread_join(thread2, NULL);
	pthread_barrier_destroy(&bar);
	return 0;
}

