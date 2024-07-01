#include <stdint.h>
#include <stdio.h>
#include <string>

#include "can.hpp"

using namespace std;

#include "supervisory_controller.hpp"

void SupervisoryController::run() {
    uint8_t last_floor = 0;
    string request_method = "";

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
                m_database.insert_floor_history(msg.DATA[0]);
                printf("[DB] FloorHistory: floor number = %d\n", msg.DATA[0]);

                last_floor = msg.DATA[0];
            }
        } else {
            switch ((int)msg.ID) {
                case FLOOR_ONE_CONTROLLER:
                    request_method = "FloorOneController";
                    break;
                case FLOOR_TWO_CONTROLLER:
                    request_method = "FloorTwoController";
                    break;
                case FLOOR_THREE_CONTROLLER:
                    request_method = "FloorThreeController";
                    break;
            }
            m_database.insert_request_history(request_method, msg.DATA[0]);
            printf("[DB] RequestHistory: method = %s floor number = %d\n", request_method.c_str(), msg.DATA[0]);
            m_can.tx(msg.DATA[0]);
        }
    }

    m_can.close();
}
