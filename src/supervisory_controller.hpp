#ifndef SUPERVISORY_CONTROLLER_HPP
#define SUPERVISORY_CONTROLLER_HPP

#include "can.hpp"
#include "database.hpp"

class SupervisoryController {
public:
    SupervisoryController() : m_can(0x100) {};

    void run();

private:
    Can m_can;
    Database m_database;
};

#endif
