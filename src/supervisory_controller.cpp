#include <stdint.h>
#include <stdio.h>
#include <string>
#include <deque>
#include <algorithm>

#include "supervisory_controller.hpp"

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
            if (last_floor != msg.DATA[0] && next_floor == msg.DATA[0]) {
                switch(msg.DATA[0]){
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
                printf("[DB] FloorHistory: floor number = %d\n", floor_number);

                last_floor = msg.DATA[0];
                waiting = false;
            }
        } else {
            switch (msg.ID) {
                case FLOOR_ONE_CONTROLLER:
                    request_method = "FloorOneController";
                    floor_number = 1;
                    break;
                case FLOOR_TWO_CONTROLLER:
                    request_method = "FloorTwoController";
                    floor_number = 2;
                    break;
                case FLOOR_THREE_CONTROLLER:
                    request_method = "FloorThreeController";
                    floor_number = 3;
                    break;
                default:
                    break;
            }

            m_database.insert_request_history(request_method, floor_number);
            printf("[DB] RequestHistory: method = %s floor number = %d\n", request_method.c_str(), floor_number);

            // Only add floors to the queue that are not already in the queue.
            // If a floor is already in the queue, then it doesn't need to be added
            // again since everyone waiting for the floor can get in the elevator
            // at the same time.
            if (find(request_queue.begin(), request_queue.end(), floor_number) == request_queue.end()) {
                // TODO: organize queue to increase efficiency
                request_queue.push_back(floor_number);
                print_queue(request_queue);
            }
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

            request_queue.pop_front();
            print_queue(request_queue);
            m_can.tx(next_floor);
            waiting = true;
        }
    }

    m_can.close();
}
