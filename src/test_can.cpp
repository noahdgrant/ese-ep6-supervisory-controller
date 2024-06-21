#include <iostream>

#include "can.hpp"
#include "test_can.hpp"

using namespace std;

static void echo_rx();
static int get_cmd();
static void tx_cmd();

void TestCan() {
    switch (get_cmd()) {
        case 1:
            tx_cmd();
            break;
        case 2:
            echo_rx();
            break;
        default:
            cerr << "should be unreachable" << endl;
            break;
    }
}

static void echo_rx() {
    Can can = Can(0x100);
    can.open();

    while (true) {
        TPCANMsg msg = can.rx();

        if(msg.ID != 0x01 && msg.LEN != 0x04) { // Ignore bus status messages
            printf("[CAN] RX: ID = 0x%X LEN = 0x%X DATA = 0x%X \n",
                   (int)msg.ID,
                   (int)msg.LEN,
                   (int)msg.DATA[0]);

            if (msg.ID != 0x101) { // Don't repeat elevator controller messages
                can.tx(msg.DATA[0]);
            }
        }
    }

    return;
}

static int get_cmd() {
    int cmd = 0;

    cout << "1. Transmit command" << endl;
    cout << "2. Echo received commands" << endl;

    while (cmd < 1 || cmd > 2) {
        scanf("%d", &cmd);
        if (cmd < 1 || cmd > 2) {
            cerr << "Error: invalid option" << endl;
        }
    }

    return cmd;
}

static void tx_cmd() {
    Can can = Can(0x100);
    int id = 0;
    int data = 0; 

    cout << "Enter ID for CAN message" << endl;
    scanf("%d", &id);
    can.set_id(id);

    cout << "Enter data for CAN message" << endl;
    scanf("%d", &data);

    printf("[CAN] TX: ID = 0x%X LEN = 0x%X DATA = 0x%X \n", id, 1, data);
    can.tx(data);

    return;
}
