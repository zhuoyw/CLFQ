#include "FastForwardQueue.h"
// #include <atomic>
#include <cstdlib>
#include <cstdio>
#include <cstring>

FastForwardQueue::FastForwardQueue(queue_idx_t count)
{
	capacity_ = next_pow(count);
	buffer_ = (queue_msg_t*)aligned_alloc(2*CACHELINE, sizeof(queue_msg_t)*capacity_);
	memset(buffer_, 0, sizeof(queue_msg_t)*capacity_);
	mask_ = capacity_ - 1;
	head_ = 0;
	tail_ = 0;
	#ifdef PRODBATCH
	tail_batch_ = 0;
	#endif
	#ifdef CONSBATCH
	head_batch_ = 0;
	#endif
}
FastForwardQueue::~FastForwardQueue()
{
	free(buffer_);
}

bool FastForwardQueue::enqueue(queue_msg_t const& msg)
{
	#ifdef PRODBATCH	
	if (tail_ == tail_batch_) {
		queue_idx_t nxt_tail_batch = tail_batch_ + prod_batch_size;
		if (buffer_[nxt_tail_batch & mask_].data[0] != empty_data)
			return false;
		tail_batch_ = nxt_tail_batch + 1;
	}
	#else
	if (buffer_[tail_ & mask_].data[0] != empty_data)
		return false;
	#endif
	for (int i = 1; i < QUEUE_MSG_DATASIZE; ++i)
		buffer_[tail_ & mask_].data[i] = msg.data[i];
	asm volatile("");
	buffer_[tail_ & mask_].data[0] = msg.data[0];
	tail_ = tail_ + 1;
	return true;
}

#ifdef PRODBATCH
bool FastForwardQueue::tail_batch_probe()
{
	// queue_idx_t batch = cons_batch_size;
	// while (true) {
	// 	queue_idx_t nxt_head_batch = head_batch_ + batch;
	// 	if (buffer_[nxt_head_batch & mask_].data[0] != empty_data) {
	// 		// fprintf(stderr, "%lu %lu %lu\n", head_batch_, batch, nxt_head_batch);
	// 		head_batch_ = nxt_head_batch + 1;
	// 		return true;
	// 	}
	// 	batch >>= 1;
	// }
	return false;
}
#endif

bool FastForwardQueue::dequeue(queue_msg_t& msg) 
{
	#ifdef CONSBATCH
	if (head_ == head_batch_) {
		if (!head_batch_probe())
			return false;
	}
	#else
	if (buffer_[head_ & mask_].data[0] == empty_data)
		return false;
	#endif
	for (int i = 0; i < QUEUE_MSG_DATASIZE; ++i)
		msg.data[i] = buffer_[head_ & mask_].data[i];
	asm volatile("");
	buffer_[head_ & mask_].data[0] = empty_data;
	head_ = head_ + 1;
	return true;
}

#ifdef CONSBATCH
bool FastForwardQueue::head_batch_probe()
{
	queue_idx_t batch = cons_batch_size;
	while (true) {
		queue_idx_t nxt_head_batch = head_batch_ + batch;
		if (buffer_[nxt_head_batch & mask_].data[0] != empty_data) {
			// fprintf(stderr, "%lu %lu %lu\n", head_batch_, batch, nxt_head_batch);
			head_batch_ = nxt_head_batch + 1;
			return true;
		}
		batch >>= 1;
	}
	return false;
}
#endif