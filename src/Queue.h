#ifndef _QUEUE_H_
#define _QUEUE_H_

#include <cstdint>

using queue_idx_t = uint32_t;

using queue_data_t = uint64_t;

#define CACHELINE 64

#define QUEUE_MSG_BYTESIZE 8 

#define QUEUE_MSG_DATASIZE (QUEUE_MSG_BYTESIZE/sizeof(queue_data_t))

struct queue_msg_t
{
	queue_data_t data[QUEUE_MSG_DATASIZE];
};

static_assert(QUEUE_MSG_BYTESIZE == sizeof(queue_msg_t), "Wrong queue_msg_t byte size!");

static queue_idx_t next_pow(queue_idx_t i)
{
	int leading_zeros = 0;
	constexpr int width = 8 * sizeof(queue_idx_t);
	for (queue_idx_t pos = 1 << (width - 1); (pos & i) == 0 && leading_zeros < 32; pos >>= 1, ++leading_zeros);
	return 1 << (width - leading_zeros);
}

class Queue {
public:
	virtual bool enqueue(queue_msg_t const& msg) = 0;
	virtual bool dequeue(queue_msg_t& msg) = 0;
};

#endif // _QUEUE_H_
