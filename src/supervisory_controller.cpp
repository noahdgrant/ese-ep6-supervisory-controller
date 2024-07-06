#include <cstdlib>
#include <deque>
#include <stdint.h>
#include <string>
#include <vector>

#include "supervisory_controller.hpp"
#include "can.hpp"
#include "queue.hpp"

using namespace std;

void SupervisoryController::run() {
    deque<uint8_t> request_queue;
    uint8_t next_floor = 0;
    uint8_t current_floor = 0;
    uint8_t last_floor = 0;
    string request_method = "";
    uint8_t floor_number = 0;
    bool waiting = false;
    vector<uint8_t> website_requests;

    m_database.read_last_website_request();

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
                switch(current_floor) {
                    case 0x5:
                        floor_number = 1;
                        system("aplay ../audio/floor1.wav");
                        break;
                    case 0x6:
                        floor_number = 2;
                        system("aplay ../audio/floor2.wav");
                        break;
                    case 0x7:
                        floor_number = 3;
                        system("aplay ../audio/floor3.wav");
                        break;
                    default:
                        break;
                }

                m_database.update_floor_history(floor_number);
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

            m_database.update_request_history(request_method, floor_number);
            QueueAdd(request_queue, current_floor - 4, floor_number); // NOTE: -4 converts to true floor number
            QueuePrint(request_queue);
        }

        website_requests = m_database.get_new_website_requests();
        for (auto floor : website_requests) {
            QueueAdd(request_queue, current_floor - 4, floor); // NOTE: -4 converts to true floor number
            QueuePrint(request_queue);
        }

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
