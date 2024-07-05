#include <stdint.h>
#include <stdio.h>
#include <string>
#include <deque>
#include <algorithm>

#include "supervisory_controller.hpp"
#include "can.hpp"

using namespace std;

static void QueuePrint(deque<uint8_t>& queue) {
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

static void QueueAdd(deque<uint8_t>& queue, int current_floor, int new_floor) {
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

using namespace std;

static void print_queue(deque<uint8_t> queue) {
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

void SupervisoryController::run() {
    deque<uint8_t> request_queue;
    uint8_t next_floor = 0;
    uint8_t current_floor = 0;
    uint8_t last_floor = 0;
    string request_method = "";
    uint8_t floor_number = 0;
    bool waiting = false;

    m_can.open();

    while (true) {
        // TODO: Ideally, non-blocking since floors can be requested from the website
        // but might not be the end of the world since the loop runs at least as
        // often as the elevator controller puts the current floor on the CAN bus.
        TPCANMsg msg = m_can.rx();

        // Ignore bus status messages
        if(msg.ID == 0x01 && msg.LEN == 0x04) {
            continue;
        }

        if (msg.ID == ELEVATOR_CONTROLLER) {
            current_floor = msg.DATA[0];
            if (next_floor == current_floor && current_floor != last_floor) {
                switch(current_floor){
                    case 0x5:
                        floor_number = 1;
                        break;
                    case 0x6:
                        floor_number = 2;
                        break;
                    case 0x7:
                        floor_number = 3;
                        break;
                    default:
                        break;
                }

                m_database.insert_floor_history(floor_number);
                last_floor = current_floor;
                waiting = false;
            }
        } else {
            switch (msg.ID) {
                case CAR_CONTROLLER:
                    request_method = "CarController";
                    break;
                case FLOOR_ONE_CONTROLLER:
                    request_method = "FloorOneController";
                    break;
                case FLOOR_TWO_CONTROLLER:
                    request_method = "FloorTwoController";
                    break;
                case FLOOR_THREE_CONTROLLER:
                    request_method = "FloorThreeController";
                    break;
                default:
                    break;
            }

            floor_number = msg.DATA[0] - 4; // NOTE: -4 converts to true floor number

            m_database.insert_request_history(request_method, floor_number);
            QueueAdd(request_queue, current_floor - 4, floor_number); // NOTE: -4 converts to true floor number
            QueuePrint(request_queue);
        }

        // TODO: Check database to see if a floor has been requested from the website

        if (!request_queue.empty() && !waiting) {
            switch (request_queue.front()) {
                case 1:
                    next_floor = 0x5;
                    break;
                case 2:
                    next_floor = 0x6;
                    break;
                case 3:
                    next_floor = 0x7;
                    break;
                default:
                    break;
            }

            m_can.tx(next_floor);
            request_queue.pop_front();
            QueuePrint(request_queue);
            waiting = true;
        }
    }

    m_can.close();
}
