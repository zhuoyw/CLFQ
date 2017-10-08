#ifndef _DPDKQUEUE_H_
#define _DPDKQUEUE_H_

#include "Queue.h"

class DPDKQueue final : public Queue {
	queue_data_t* buffer_;
	queue_idx_t capacity_;
	queue_idx_t mask_;
	alignas(2*CACHELINE) queue_idx_t prod_head_;
	alignas(2*CACHELINE) queue_idx_t prod_tail_;
	alignas(2*CACHELINE) queue_idx_t cons_head_;
	alignas(2*CACHELINE) queue_idx_t cons_tail_;
public:
	DPDKQueue(queue_idx_t count);
	bool enqueue(queue_data_t const& data) override;
	bool dequeue(queue_data_t& data) override;
	~DPDKQueue();
};

#endif // _DPDKQUEUE_H_
