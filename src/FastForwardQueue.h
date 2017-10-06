#ifndef _FASTFORWARDQUEUE_H_
#define _FASTFORWARDQUEUE_H_

#include "Queue.h"

class FastForwardQueue final : public Queue {
	queue_data_t* buffer_;
	queue_idx_t capacity_;
	queue_idx_t mask_;
	alignas(2*CACHELINE) queue_idx_t head_;
	alignas(2*CACHELINE) queue_idx_t tail_;
public:
	FastForwardQueue(queue_idx_t count);
	bool enqueue(queue_data_t const& data) override;
	bool dequeue(queue_data_t& data) override;
	~FastForwardQueue();
};

#endif // _FASTFORWARDQUEUE_H_
