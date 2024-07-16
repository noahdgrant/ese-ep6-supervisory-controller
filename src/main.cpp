#include <iostream>
#include <stdint.h>
#include <termios.h>

#include "can.hpp"
#include "database.hpp"
#include "environment.hpp"
#include "serial.hpp"
#include "supervisory_controller.hpp"

using namespace std;

static void Help();
static void TestAudio();
static void TestCan();
static void TestDatabase();
static int TestSerial();

int main(int argc, char* argv[]) {
    LoadEnvironmentVariables(".env");

    if (argc > 2) {
        cerr << "Error: too many inputs" << endl;
        Help();
    } else if (argc == 1) {
        SupervisoryController controller = SupervisoryController();
        controller.run();
    } else {
        string cmd = string(argv[1]);

        if (cmd == "--test-audio") {
            TestAudio();
        } else if (cmd == "--test-can") {
            TestCan();
        } else if (cmd == "--test-database") {
            TestDatabase();
        } else if (cmd == "--test-serial") {
            TestSerial();
        } else if (cmd == "--help") {
            Help();
        } else {
            cerr << "Error: unknown command" << endl;
            Help();
        }
    }

    return 0;
}

static void Help() {
    cout << "Usage: ./main <options>" << endl;
    cout << "Options:" << endl;
    cout << "--help" << endl;
    cout << "--test-audio" << endl;
    cout << "--test-can" << endl;
    cout << "--test-database" << endl;
    cout << "--test-nfc" << endl;
    cout << "--test-serial" << endl;
}

static void TestAudio() {
    system("aplay ~/Projects/ese-ep6-supervisory-controller/audio/floor1.wav");
    system("aplay ~/Projects/ese-ep6-supervisory-controller/audio/floor2.wav");
    system("aplay ~/Projects/ese-ep6-supervisory-controller/audio/floor3.wav");
}

static void TestCan() {
    int cmd = 0;
    Can can = Can(0x100);
    can.open();

    cout << "1. Transmit command" << endl;
    cout << "2. Echo received commands" << endl;

    while (cmd < 1 || cmd > 2) {
        scanf("%d", &cmd);
        if (cmd < 1 || cmd > 2) {
            cerr << "Error: invalid option" << endl;
        }
    }

    switch (cmd) {
        case 1: {
            // Test CAN TX
            int id = 0;
            int data = 0; 

            cout << "Enter ID for CAN message" << endl;
            scanf("%d", &id);
            can.set_id(id);

            cout << "Enter data for CAN message" << endl;
            scanf("%d", &data);

            printf("[CAN] TX: ID = 0x%X LEN = 0x%X DATA = 0x%X \n", id, 1, data);
            can.tx(data);
            break;

        }
        case 2: {
            // Test CAN RX
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
            break;
        }
        default:
            cerr << "should be unreachable" << endl;
            break;
    }
}

static void TestDatabase() {
    Database database = Database(std::getenv("DATABASE_IP"), std::getenv("DATABASE_USERNAME"),
                                 std::getenv("DATABASE_PASSWORD"), std::getenv("DATABASE_SCHEMA"));
    uint8_t floor_number = 1;
    char request_method[] = "FloorOneController";
    database.update_request_history(request_method, floor_number);
    printf("Inserted into request history: method = %s floor number = %d\n", request_method, floor_number);
}

static int TestSerial() {
    Serial serial;
    int floor_number = 0;

    if (!serial.open("/dev/ttyACM0")) {
        return -1;
    }
    if (!serial.configure(B9600)) {
        serial.close();
        return -1;
    }
    floor_number = serial.check_for_request();
    if (floor_number != 0) {
        printf("Floor number: %d", floor_number);
    }
    serial.close();

    return 0;
}
