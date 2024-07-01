#ifndef DATABASE_HPP
#define DATABASE_HPP

#include <stdint.h>
#include <string>

using namespace std;

class Database {
public:
    Database(string port, string username, string password, string schema);

    int get_floor_number();
    int set_floor_number(int floor_number);
    void insert_request_history(string request_method, uint8_t floor_number);

private:
    string m_port;
    string m_username;
    string m_password;
    string m_schema;

};

#endif
