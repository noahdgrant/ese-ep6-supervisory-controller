#ifndef DATABASE_HPP
#define DATABASE_HPP

#include <stdint.h>
#include <string>

using namespace std;

class Database {
public:
    Database(string port, string username, string password, string schema);

    void insert_request_history(string request_method, uint8_t floor_number);
    void insert_floor_history(uint8_t floor_number);

private:
    string m_port;
    string m_username;
    string m_password;
    string m_schema;

};

#endif
