#include "LamportQueueX86.h"
// #include <atomic>
#include <cstdlib>
#include <cstdio>

LamportQueueX86::LamportQueueX86(queue_idx_t count)
{
	capacity_ = next_pow(count);
	buffer_ = (queue_data_t*)malloc(sizeof(queue_data_t)*capacity_);
	mask_ = capacity_ - 1;
	prod_head_ = 0;
	prod_tail_ = 0; 
	cons_head_ = 0;
	cons_tail_ = 0;
}
LamportQueueX86::~LamportQueueX86()
{
	free(buffer_);
}

bool LamportQueueX86::enqueue(queue_data_t const& data)
{
	queue_idx_t cur_tail = prod_tail_;
	if (cur_tail - prod_head_ >= capacity_) {
		prod_head_ = cons_head_;
		if (cur_tail - prod_head_ >= capacity_) {
			return false;
		}
	}
	buffer_[cur_tail & mask_] = data;
	asm volatile("" ::: "memory");
	prod_tail_ = cur_tail + 1;
	return true;
}

bool LamportQueueX86::dequeue(queue_data_t& data) 
{
	queue_idx_t cur_head = cons_head_;
	if (cur_head == cons_tail_) {
		cons_tail_ = prod_tail_;
		if (cur_head == cons_tail_) {
			return false;
		}
	}
	queue_idx_t cur_indx = cur_head & mask_;
	data = buffer_[cur_indx];
	cons_head_ = cur_head + 1;
	return true;
}
