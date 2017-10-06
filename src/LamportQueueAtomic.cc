#include "LamportQueueAtomic.h"
#include <cstdlib>
#include <cstdio>

LamportQueueAtomic::LamportQueueAtomic(queue_idx_t count)
{
	capacity_ = next_pow(count);
	buffer_ = (queue_data_t*)malloc(sizeof(queue_data_t)*capacity_);
	mask_ = capacity_ - 1;
	head_ = 0;
	tail_ = 0; 
}
LamportQueueAtomic::~LamportQueueAtomic()
{
	free(buffer_);
}

bool LamportQueueAtomic::enqueue(queue_data_t const& data)
{
	queue_idx_t cur_head = head_.load(std::memory_order_relaxed);
	queue_idx_t cur_tail = tail_.load(std::memory_order_relaxed);
	if (cur_tail - cur_head >= capacity_)
		return false; 
	buffer_[cur_tail & mask_] = data;
	tail_.store(cur_tail + 1, std::memory_order_release);
	return true;
}

bool LamportQueueAtomic::dequeue(queue_data_t& data) 
{
	queue_idx_t cur_head = head_.load(std::memory_order_relaxed);
	queue_idx_t cur_tail = tail_.load(std::memory_order_acquire);
	if (cur_head == cur_tail)
		return false;
	queue_idx_t cur_indx = cur_head & mask_;
	data = buffer_[cur_indx];
	head_.store(cur_head + 1, std::memory_order_relaxed);
	return true;
}
