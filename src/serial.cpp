#include <algorithm>
#include <errno.h>
#include <fcntl.h>
#include <iostream>
#include <stdint.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

#include "serial.hpp"

using namespace std;

static string StripNewlines(const string& str);

uint8_t Serial::check_for_request() {
    uint8_t floor_number = 0;
    int bytes_read = 0;
    char buffer[256];

    memset(&buffer, '\0', sizeof(buffer));

    bytes_read = ::read(m_fd, buffer, sizeof(buffer));
    if (bytes_read < 0) {
        cerr << "[SERIAL] Error reading from serial port: " << strerror(errno) << endl;
    } else if (bytes_read == 0) {
        // Do nothing
    } else {
        string stripped_buffer(buffer);
        stripped_buffer = StripNewlines(stripped_buffer);

        if(!stripped_buffer.empty()) {
            if (stripped_buffer == "one") {
                floor_number = 1;
            } else if (stripped_buffer == "two") {
                floor_number = 2;
            } else if (stripped_buffer == "three") {
                floor_number = 3;
            }

            cout << "[SERIAL] Read from serial port: " << stripped_buffer << endl;
        }
    }

    return floor_number;
}

void Serial::close() {
    if (m_fd != -1) {
        ::close(m_fd);
        m_fd = -1;
    }
}

bool Serial::configure(int speed) {
    struct termios tty;
    if (tcgetattr(m_fd, &tty) != 0) {
        cerr << "[SERIAL] Error from tcgetattr: " << strerror(errno) << endl;
        return false;
    }
    cfsetospeed(&tty, speed);
    cfsetispeed(&tty, speed);
    tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8; // 8-bit characters
    tty.c_iflag &= ~IGNBRK; // disable break processing
    tty.c_lflag = 0; // no signaling chars, no echo, no canonical processing
    tty.c_oflag = 0; // no remapping, no delays
    tty.c_cc[VMIN] = 0; // read doesn't block
    tty.c_cc[VTIME] = 5; // 0.5 seconds read timeout
    tty.c_iflag &= ~(IXON | IXOFF | IXANY); // shut off xon/xoff ctrl
    tty.c_cflag |= (CLOCAL | CREAD); // ignore modem controls, enable reading
    tty.c_cflag &= ~(PARENB | PARODD); // shut off parity
    tty.c_cflag &= ~CSTOPB;
    tty.c_cflag &= ~CRTSCTS;
    if (tcsetattr(m_fd, TCSANOW, &tty) != 0) {
        cerr << "[SERIAL] Error from tcsetattr: " << strerror(errno) << endl;
        return false;
    }
    return true;
}

bool Serial::open(const char* portname) {
    m_fd = ::open(portname, O_RDWR | O_NOCTTY | O_SYNC);
    if (m_fd < 0) {
        cerr << "[SERIAL] Error opening " << portname << ": " << strerror(errno) << endl;
        return false;
    }
    return true;
}

int Serial::read(char* buffer, size_t size) {
    return ::read(m_fd, buffer, size);
}

 static string StripNewlines(const string& str) {
    // Remove '\r'
    string result = str;
    result.erase(remove(result.begin(), result.end(), '\r'), result.end());

    // Remove '\n'
    size_t first = result.find_first_not_of('\n');
    if (string::npos == first) {
        return "";
    }

    size_t last = result.find_last_not_of('\n');
    return result.substr(first, (last - first + 1));
}
