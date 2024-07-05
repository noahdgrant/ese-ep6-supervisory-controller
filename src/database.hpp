#ifndef DATABASE_HPP
#define DATABASE_HPP

#include <stdint.h>
#include <string>
#include <vector>

using namespace std;

class Database {
public:
    Database(string port, string username, string password, string schema);

    vector<uint8_t> get_new_website_requests();
    void read_last_website_request();
    void update_request_history(string request_method, uint8_t floor_number);
    void update_floor_history(uint8_t floor_number);

private:
    string m_port;
    string m_username;
    string m_password;
    string m_schema;
    uint64_t m_last_website_request_id;

};

#endif
