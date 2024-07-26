#ifndef SUPERVISORY_CONTROLLER_HPP
#define SUPERVISORY_CONTROLLER_HPP

#include <cstdlib>

#include "can.hpp"
#include "database.hpp"
#include "serial.hpp"

class SupervisoryController {
public:
    SupervisoryController() : m_can(0x100), m_database(std::getenv("DATABASE_IP"),
                                                       std::getenv("DATABASE_USERNAME"),
                                                       std::getenv("DATABASE_PASSWORD"),
                                                       std::getenv("DATABASE_SCHEMA")) {};

    int run();

private:
    Can m_can;
    Database m_database;
    Serial m_serial;
};

#endif
