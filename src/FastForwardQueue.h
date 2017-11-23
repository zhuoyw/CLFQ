#ifndef _FASTFORWARDQUEUE_H_
#define _FASTFORWARDQUEUE_H_

#include "Queue.h"

class FastForwardQueue final : public Queue {
	enum : queue_data_t { empty_data = 0 };
	queue_msg_t* buffer_;
	queue_idx_t capacity_;
	queue_idx_t mask_;
	// consumer 
	alignas(CACHELINE) queue_idx_t head_;
	#ifdef CONSBATCH // batch
	alignas(CACHELINE) queue_idx_t head_batch_;
	enum : queue_idx_t { cons_batch_size = 2*CACHELINE };
	bool head_batch_probe();
	#endif
	// producer
	alignas(CACHELINE) queue_idx_t tail_;
	#ifdef PRODBATCH // batch
	alignas(CACHELINE) queue_idx_t tail_batch_;
	enum : queue_idx_t { prod_batch_size = 8*CACHELINE };
	bool tail_batch_probe();
	#endif
public:
	FastForwardQueue(queue_idx_t count);
	bool enqueue(queue_msg_t const& msg) override;
	bool dequeue(queue_msg_t& msg) override;
	~FastForwardQueue();
};

#endif // _FASTFORWARDQUEUE_H_
