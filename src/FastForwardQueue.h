#ifndef _FASTFORWARDQUEUE_H_
#define _FASTFORWARDQUEUE_H_

#include "Queue.h"

class FastForwardQueue final : public Queue {
	enum : queue_data_t { empty_data = 0 };
	queue_data_t* buffer_;
	queue_idx_t capacity_;
	queue_idx_t mask_;
	alignas(2*CACHELINE) queue_idx_t head_;
	alignas(2*CACHELINE) queue_idx_t head_batch_;
	alignas(2*CACHELINE) queue_idx_t tail_;
	alignas(2*CACHELINE) queue_idx_t tail_batch_;
	// #ifdef PRODBATCH
	// alignas(2*CACHELINE) queue_idx_t head_batch_;
	// enum : queue_idx_t { prod_batch_size = 4*CACHELINE};
	// #endif
	// #ifdef CONSBATCH
	// alignas(2*CACHELINE) queue_idx_t tail_batch_;
	// enum : queue_idx_t { cons_batch_size = 2*CACHELINE };
	// bool backtrack();
	// #endif
public:
	FastForwardQueue(queue_idx_t count);
	bool enqueue(queue_data_t const& data) override;
	bool dequeue(queue_data_t& data) override;
	~FastForwardQueue();
};

#endif // _FASTFORWARDQUEUE_H_
