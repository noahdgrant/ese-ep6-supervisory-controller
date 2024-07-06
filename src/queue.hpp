#ifndef QUEUE_HPP
#define QUEUE_HPP

#include <deque>
#include <stdint.h>

using namespace std;

void QueueAdd(deque<uint8_t>& queue, int current_floor, int new_floor);
void QueuePrint(deque<uint8_t>& queue);

#endif
