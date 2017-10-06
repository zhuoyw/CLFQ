#include "FastForwardQueue.h"
// #include <atomic>
#include <cstdlib>
#include <cstdio>

FastForwardQueue::FastForwardQueue(queue_idx_t count)
{
	capacity_ = next_pow(count);
	buffer_ = (queue_data_t*)malloc(sizeof(queue_data_t)*capacity_);
	// memset()
	mask_ = capacity_ - 1;
	head_ = 0;
	tail_ = 0; 
}
FastForwardQueue::~FastForwardQueue()
{
	free(buffer_);
}

bool FastForwardQueue::enqueue(queue_data_t const& data)
{
	queue_idx_t cur_head = head_;
	queue_idx_t cur_tail = tail_;
	if (cur_tail - cur_head >= capacity_)
		return false; 
	buffer_[cur_tail & mask_] = data;
	asm volatile("" ::: "memory");
	tail_ = cur_tail + 1;
	return true;
}

bool FastForwardQueue::dequeue(queue_data_t& data) 
{
	queue_idx_t cur_head = head_;
	queue_idx_t cur_tail = tail_;
	if (cur_head == cur_tail)
		return false;
	queue_idx_t cur_indx = cur_head & mask_;
	data = buffer_[cur_indx];
	head_ = cur_head + 1;
	return true;
}
