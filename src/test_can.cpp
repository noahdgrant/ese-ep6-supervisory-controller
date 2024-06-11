#include <iostream>

#include "test_can.hpp"

using namespace std;

void TestCan() {
    int choice = 0;

    cout << "1. Transmit command" << endl;
    cout << "2. Echo received commands" << endl;

    while (choice < 1 || choice > 2) {
        scanf("%d", &choice);
        if (choice < 1 || choice > 2) {
            cerr << "Error: invalid option" << endl;
        }
    }

    switch (choice) {
        case 1:
            cerr << "not implemented" << endl;
            break;
        case 2:
            cerr << "not implemented" << endl;
            break;
        default:
            cerr << "should be unreachable" << endl;
            break;
    }
}
