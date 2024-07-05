#include <stdlib.h>
#include <string>
#include <stdint.h>
#include <vector>

#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <mysql_connection.h>

#include "database.hpp"
 
using namespace std; 

Database::Database(string port, string username, string password, string schema) {
    m_port = port;
    m_username = username;
    m_password = password;
    m_schema = schema;
    m_last_website_request_id = 0;
}

vector<uint8_t> Database::get_new_website_requests() {
    sql::Connection *con;
	sql::Driver *driver;
    sql::PreparedStatement *pstmt;
    sql::ResultSet *res;
    vector<uint8_t> floors;

    driver = get_driver_instance();
    con = driver->connect(m_port, m_username, m_password);
    con->setSchema(m_schema);

    pstmt = con->prepareStatement("SELECT * FROM RequestHistory WHERE Method = ? AND Id > ? ORDER BY Id");
    pstmt->setString(1, "Website");
    pstmt->setInt(2, m_last_website_request_id);

    while (res->next()) {
        m_last_website_request_id = res->getInt("Id");
        floors.push_back(res->getInt("Floor"));
        printf("[DB] New website request ID = %d Floor = %d\n", res->getInt("Id"), res->getInt("Floor"));
    }

    delete con;
    delete pstmt;
    delete res;

    return floors;
}

void Database::read_last_website_request() {
    sql::Connection *con;
	sql::Driver *driver;
    sql::PreparedStatement *pstmt;
    sql::ResultSet *res;

    driver = get_driver_instance();
    con = driver->connect(m_port, m_username, m_password);
    con->setSchema(m_schema);

    pstmt = con->prepareStatement("SELECT * FROM RequestHistory WHERE Method = ? ORDER BY Id DESC LIMIT 1");
    pstmt->setString(1, "Website");
    res = pstmt->executeQuery();

    while (res->next()) {
        m_last_website_request_id = res->getInt("Id");
    }

    printf("[DB] Last website request ID = %lld\n", m_last_website_request_id);

    delete con;
    delete pstmt;
    delete res;

    return;
}

void Database::update_floor_history(uint8_t floor_number) {
	sql::Driver *driver;
	sql::Connection *con;
	sql::PreparedStatement *pstmt;

    driver = get_driver_instance();
    con = driver->connect(m_port, m_username, m_password);
    con->setSchema(m_schema);

    pstmt = con->prepareStatement("INSERT INTO FloorHistory(floor) VALUES (?)");
    pstmt->setInt(1, floor_number);
    pstmt->executeUpdate();

    printf("[DB] FloorHistory: floor number = %d\n", floor_number);

	delete pstmt;
	delete con;

    return;
}

void Database::update_request_history(string request_method, uint8_t floor_number) {
	sql::Driver *driver;
	sql::Connection *con;
	sql::PreparedStatement *pstmt;

    driver = get_driver_instance();
    con = driver->connect(m_port, m_username, m_password);
    con->setSchema(m_schema);

    pstmt = con->prepareStatement("INSERT INTO RequestHistory(method, floor) VALUES (?,?)");
    pstmt->setString(1, request_method);
    pstmt->setInt(2, floor_number);
    pstmt->executeUpdate();

    printf("[DB] RequestHistory: method = %s floor number = %d\n", request_method.c_str(), floor_number);

	delete pstmt;
	delete con;

    return;
}
