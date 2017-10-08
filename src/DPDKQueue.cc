#include "DPDKQueue.h"
// #include <atomic>
#include <cstdlib>
#include <cstdio>

DPDKQueue::DPDKQueue(queue_idx_t count)
{
	capacity_ = next_pow(count);
	fprintf(stderr, "%lu\n", capacity_);
	buffer_ = (queue_data_t*)malloc(sizeof(queue_data_t)*capacity_);
	mask_ = capacity_ - 1;
	prod_head_ = 0;
	prod_tail_ = 0; 
	cons_head_ = 0;
	cons_tail_ = 0;
}

DPDKQueue::~DPDKQueue()
{
	free(buffer_);
}

bool DPDKQueue::enqueue(queue_data_t const& data)
{
	
	queue_idx_t cur_head = prod_head_;
	queue_idx_t cur_tail = cons_tail_;
	queue_idx_t cur_next = cur_tail + 1;
	if (cur_tail - cur_head >= capacity_)
		return false;
	buffer_[cur_tail & mask_] = data;
	prod_head_ = cur_next;
	asm volatile("" ::: "memory");
	prod_tail_ = cur_next;
	return true;
}

bool DPDKQueue::dequeue(queue_data_t& data) 
{
	queue_idx_t cur_head = cons_head_;
	queue_idx_t cur_tail = prod_tail_;
	queue_idx_t cur_next = cur_head + 1;
	if (cur_head == cur_tail)
		return false;
	queue_idx_t cur_indx = cur_head & mask_;
	data = buffer_[cur_indx];
	cons_head_ = cur_next;
	cons_tail_ = cur_next;
	return true;
}
