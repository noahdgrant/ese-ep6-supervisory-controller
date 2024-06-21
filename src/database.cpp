#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <stdlib.h>
#include <stdio.h>

#include <mysql_connection.h>

#include "database.hpp"
 
using namespace std; 
 
int Database::get_floor_number() {
    int floor_number = -1; 
	sql::Driver *driver; 			// Create a pointer to a MySQL driver object
	sql::Connection *con; 			// Create a pointer to a database connection object
	sql::Statement *stmt;			// Crealte a pointer to a Statement object to hold statements 
	sql::ResultSet *res;			// Create a pointer to a ResultSet object to hold results 
	
	driver = get_driver_instance();
	con = driver->connect("tcp://127.0.0.1:3306", "ese", "ese");	
	con->setSchema("elevator");		
	
	stmt = con->createStatement();
	res = stmt->executeQuery("SELECT currentFloor FROM elevatorNetwork WHERE nodeID = 1");	// message query
	while(res->next()){
		floor_number = res->getInt("currentFloor");
	}
	
	delete res;
	delete stmt;
	delete con;
	
	return floor_number;
}
 
 
int Database::set_floor_number(int floor_number) {
    int error_code = -1;
	sql::Driver *driver; 				// Create a pointer to a MySQL driver object
	sql::Connection *con; 				// Create a pointer to a database connection object
	sql::Statement *stmt;				// Crealte a pointer to a Statement object to hold statements 
	sql::ResultSet *res;				// Create a pointer to a ResultSet object to hold results 
	sql::PreparedStatement *pstmt; 		// Create a pointer to a prepared statement	

    if (floor_number >= 1 && floor_number <= 3) {
        driver = get_driver_instance();
        con = driver->connect("tcp://127.0.0.1:3306", "ese", "ese");	
        con->setSchema("elevator");										

        // TODO: possibly not necessary
        stmt = con->createStatement();
        res = stmt->executeQuery("SELECT currentFloor FROM elevatorNetwork WHERE nodeID = 1");	// message query
        while(res->next()){
            res->getInt("currentFloor");
        }

        pstmt = con->prepareStatement("UPDATE elevatorNetwork SET currentFloor = ? WHERE nodeID = 1");
        pstmt->setInt(1, floor_number);
        pstmt->executeUpdate();

        error_code = 0;
    } else {
        printf("ERROR: Given floor number (%d) is not valid", floor_number);
    }
		
	delete res;
	delete pstmt;
	delete stmt;
	delete con;

    return error_code;
} 
