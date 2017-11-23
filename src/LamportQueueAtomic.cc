#include "LamportQueueAtomic.h"
#include <cstdlib>
#include <cstdio>
#include <cstring>

LamportQueueAtomic::LamportQueueAtomic(queue_idx_t count)
{
	capacity_ = next_pow(count);
	buffer_ = (queue_msg_t*)aligned_alloc(2*CACHELINE, sizeof(queue_msg_t)*capacity_);
	memset(buffer_, 0, sizeof(queue_msg_t)*capacity_);
	mask_ = capacity_ - 1;
	head_ = 0;
	tail_ = 0; 
}

LamportQueueAtomic::~LamportQueueAtomic()
{
	free(buffer_);
}

bool LamportQueueAtomic::enqueue(queue_msg_t const& msg)
{
	queue_idx_t cur_head = head_.load(std::memory_order_relaxed);
	queue_idx_t cur_tail = tail_.load(std::memory_order_relaxed);
	if (cur_tail - cur_head >= capacity_)
		return false;
	for (int i = 0; i < QUEUE_MSG_DATASIZE; ++i)
		buffer_[cur_tail & mask_].data[i] = msg.data[i];
	tail_.store(cur_tail + 1, std::memory_order_release);
	return true;
}

bool LamportQueueAtomic::dequeue(queue_msg_t& msg)
{
	queue_idx_t cur_head = head_.load(std::memory_order_relaxed);
	queue_idx_t cur_tail = tail_.load(std::memory_order_acquire);
	if (cur_head == cur_tail)
		return false;
	for (int i = 0; i < QUEUE_MSG_DATASIZE; ++i)
		msg.data[i] = buffer_[cur_head & mask_].data[i];
	head_.store(cur_head + 1, std::memory_order_relaxed);
	return true;
}
