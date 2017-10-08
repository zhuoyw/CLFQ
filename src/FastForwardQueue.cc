#include "FastForwardQueue.h"
// #include <atomic>
#include <cstdlib>
#include <cstdio>
#include <cstring>

FastForwardQueue::FastForwardQueue(queue_idx_t count)
{
	capacity_ = next_pow(count);
	buffer_ = (queue_data_t*)aligned_alloc(2*CACHELINE, sizeof(queue_data_t)*capacity_);
	memset(buffer_, 0, sizeof(queue_data_t)*capacity_);
	mask_ = capacity_ - 1;
	head_ = 0;
	#ifdef PRODBATCH
	head_batch_ = 0;
	#endif
	tail_ = 0;
	#ifdef CONSBATCH
	tail_batch_ = 0;
	#endif
}
FastForwardQueue::~FastForwardQueue()
{
	free(buffer_);
}

bool FastForwardQueue::enqueue(queue_data_t const& data)
{
	#ifdef PRODBATCH	
	if (tail_ == tail_batch_) {
		queue_idx_t nxt_tail_batch = tail_batch_ + prod_batch_size;
		// if ((*(volatile queue_data_t *)(&buffer_[nxt_tail_batch & mask_])) != queue_empty_data)
		if (buffer_[nxt_tail_batch & mask_] != empty_data)
			return false;
		tail_batch_ = nxt_tail_batch;
	}
	#else
	if (buffer_[tail_ & mask_] != empty_data)
		return false;
	#endif
	buffer_[tail_ & mask_] = data;
	tail_ = tail_ + 1;
	return true;
}

bool FastForwardQueue::dequeue(queue_data_t& data) 
{
	#ifdef CONSBATCH
	if (head_ == head_batch_) {
		if (!backtrack())
			return false;
	}
	#else
	data = buffer_[head_ & mask_];
	if (data == empty_data)
		return false;
	#endif
	buffer_[head_ & mask_] = empty_data;
	head_ = head_ + 1;
	return true;
}

#ifdef CONSBATCH
bool FastForwardQueue::backtrack()
{
	queue_idx_t batch = cons_batch_size;
	while (batch) {
		queue_idx_t nxt_head_batch = head_batch_ + batch;
		if (buffer_[nxt_head_batch & mask_] != empty_data) {
			head_batch_ = nxt_head_batch;
			return true;
		}
		batch >>= 1;
	}
	return false;
}
#endif