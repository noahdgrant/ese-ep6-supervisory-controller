#include <iostream>

#include "supervisory_controller.hpp"
#include "test_can.hpp"

using namespace std;

void Help();

int main(int argc, char* argv[]){
    if (argc > 2){
        cerr << "Error: too many inputs" << endl;
        Help();
    } else if (argc == 1) {
        SupervisoryController controller = SupervisoryController();
        controller.run();
    } else {
        string cmd = string(argv[1]);

        if (cmd == "--test-can") {
            TestCan();
        } else if (cmd == "--test-nfc") {
            cerr << "not implemented" << endl;
        } else if (cmd == "--test-database") {
            cerr << "not implemented" << endl;
        } else if (cmd == "--help") {
            Help();
        } else {
            cerr << "Error: unknown command" << endl;
            Help();
        }
    }

    return 0;
}

void Help() {
    cout << "Usage: ./main <options>" << endl;
    cout << "Options:" << endl;
    cout << "--help" << endl;
    cout << "--test-can" << endl;
    cout << "--test-nfc" << endl;
    cout << "--test-database" << endl;
}
