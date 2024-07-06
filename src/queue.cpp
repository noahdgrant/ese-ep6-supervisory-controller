#include <algorithm>
#include <deque>
#include <stdio.h>
#include <stdint.h>

#include "queue.hpp"

using namespace std;

void QueueAdd(deque<uint8_t>& queue, int current_floor, int new_floor) {
    // NOTE: Only add floors to the queue that are not already in the queue.
    // If a floor is already in the queue, then it doesn't need to be added
    // again since everyone waiting for the floor can get in the elevator
    // at the same time.
    if (find(queue.begin(), queue.end(), new_floor) == queue.end()) {
        if (queue.empty()) {
            queue.push_back(new_floor);
        } else {
            if (current_floor < new_floor) {
                // Elevator is moving up
                auto it = find_if(queue.begin(), queue.end(), [&](int floor) { return floor > new_floor; });
                queue.insert(it, new_floor);
            } else {
                // Elevator is moving down
                auto it = find_if(queue.rbegin(), queue.rend(), [&](int floor) { return floor < new_floor; });
                queue.insert(it.base(), new_floor);
            }
        }
    }
}

void QueuePrint(deque<uint8_t>& queue) {
    printf("[QUEUE]");
    if (!queue.empty()) {
        for (auto item : queue) {
            printf(" %d", item);
        }
        printf("\n");
    } else {
        printf(" is empty\n");
    }
}
