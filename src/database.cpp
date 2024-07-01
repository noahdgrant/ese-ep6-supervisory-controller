#include <stdlib.h>
#include <string>
#include <stdint.h>

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
}

void Database::insert_floor_history(uint8_t floor_number) {
	sql::Driver *driver;
	sql::Connection *con;
	sql::PreparedStatement *pstmt;

    driver = get_driver_instance();
    con = driver->connect(m_port, m_username, m_password);
    con->setSchema(m_schema);

    pstmt = con->prepareStatement("INSERT INTO FloorHistory(floor) VALUES (?)");
    pstmt->setInt(1, floor_number);
    pstmt->executeUpdate();

	delete pstmt;
	delete con;

    return;
}

void Database::insert_request_history(string request_method, uint8_t floor_number) {
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

	delete pstmt;
	delete con;

    return;
}
