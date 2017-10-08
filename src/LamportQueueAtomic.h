#ifndef _LAMPORTQUEUEATOMIC_H_
#define _LAMPORTQUEUEATOMIC_H_

#include "Queue.h"
#include <atomic>

class LamportQueueAtomic final : public Queue {
	queue_data_t* buffer_;
	queue_idx_t capacity_;
	queue_idx_t mask_;
	alignas(2*CACHELINE) std::atomic<queue_idx_t> head_;
	alignas(2*CACHELINE) std::atomic<queue_idx_t> tail_;
public:
	LamportQueueAtomic(queue_idx_t count);
	bool enqueue(queue_data_t const& data) override;
    bool dequeue(queue_data_t& data) override;
	~LamportQueueAtomic();
};

#endif // _LAMPORTQUEUEATOMIC_H_
