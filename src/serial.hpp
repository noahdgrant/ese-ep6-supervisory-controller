#ifndef SERIAL_HPP
#define SERIAL_HPP

#include <stdint.h>
#include <unistd.h>

class Serial {
public:
    uint8_t check_for_request();
    void close();
    bool configure(int speed);
    bool open(const char* portname);
    int read(char* buffer, size_t size);
private:
    int m_fd;
};

#endif
