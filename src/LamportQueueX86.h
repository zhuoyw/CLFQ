#ifndef _LAMPORTQUEUEX86_H_
#define _LAMPORTQUEUEX86_H_

#include "Queue.h"

class LamportQueueX86 final : public Queue {
	queue_data_t* buffer_;
	queue_idx_t capacity_;
	queue_idx_t mask_;
	alignas(2*CACHELINE) queue_idx_t prod_head_;
	alignas(2*CACHELINE) queue_idx_t prod_tail_;
	alignas(2*CACHELINE) queue_idx_t cons_head_;
	alignas(2*CACHELINE) queue_idx_t cons_tail_;
public:
	LamportQueueX86(queue_idx_t count);
	bool enqueue(queue_data_t const& data) override;
	bool dequeue(queue_data_t& data) override;
	~LamportQueueX86();
};

#endif // _LAMPORTQUEUEX86_H_
