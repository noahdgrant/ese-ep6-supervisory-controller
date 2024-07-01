#include <stdint.h>
#include <stdio.h>
#include <string>

#include "can.hpp"

using namespace std;

#include "supervisory_controller.hpp"

void SupervisoryController::run() {
    uint8_t last_floor = 0;
    string request_method = "";
    uint8_t floor_number = 0;

    m_can.open();

    while (true) {
        TPCANMsg msg = m_can.rx(); // TODO: this can't be blocking since floors can be requested from the website

        // Ignore bus status messages
        if(msg.ID == 0x01 && msg.LEN == 0x04) {
            continue;
        }

        printf("[CAN] RX: ID = 0x%X LEN = 0x%X DATA = 0x%X \n",
               (int)msg.ID,
               (int)msg.LEN,
               (int)msg.DATA[0]);

        if (msg.ID == ELEVATOR_CONTROLLER) {
            if (last_floor != msg.DATA[0]) {
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
            m_can.tx(msg.DATA[0]);
        }
    }

    m_can.close();
}
