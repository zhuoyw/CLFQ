#include "LamportQueueX86.h"
// #include <atomic>
#include <cstdlib>
#include <cstdio>
#include <cstring>

LamportQueueX86::LamportQueueX86(queue_idx_t count)
{
	capacity_ = next_pow(count);
	buffer_ = (queue_msg_t*)aligned_alloc(2*CACHELINE, sizeof(queue_msg_t)*capacity_);
	memset(buffer_, 0, sizeof(queue_msg_t)*capacity_);
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

bool LamportQueueX86::enqueue(queue_msg_t const& msg)
{
	queue_idx_t cur_tail = prod_tail_;

	if (cur_tail - prod_head_ >= capacity_) {
		prod_head_ = cons_head_;
		if (cur_tail - prod_head_ >= capacity_) {
			return false;
		}
	}
	for (int i = 0; i < QUEUE_MSG_DATASIZE; ++i)
		buffer_[cur_tail & mask_].data[i] = msg.data[i];
	asm volatile("" ::: "memory");
	prod_tail_ = cur_tail + 1;
	return true;
}

bool LamportQueueX86::dequeue(queue_msg_t& msg) 
{
	queue_idx_t cur_head = cons_head_;
	if (cur_head == cons_tail_) {
		cons_tail_ = prod_tail_;
		if (cur_head == cons_tail_) {
			return false;
		}
	}
	for (int i = 0; i < QUEUE_MSG_DATASIZE; ++i)
		msg.data[i] = buffer_[cur_head & mask_].data[i];
	asm volatile("" ::: "memory");
	cons_head_ = cur_head + 1;
	return true;
}
