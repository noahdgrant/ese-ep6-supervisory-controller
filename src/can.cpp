#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h> 
#include <signal.h>
#include <string.h>
#include <fcntl.h>    					// O_RDWR
#include <unistd.h>
#include <ctype.h>
#include <libpcan.h>   					// PCAN library

#include "can.hpp"

Can::Can(int id) {
        m_tx.ID = id;
}

void Can::close() {
    CAN_Close(m_handle);

    return;
}

void Can::open() {
    DWORD status;

	m_handle = LINUX_CAN_Open("/dev/pcanusb32", O_RDWR);
    // TODO: error when 'status' isn't right
	status = CAN_Init(m_handle, CAN_BAUD_125K, CAN_INIT_TYPE_ST);
	status = CAN_Status(m_handle);

    return;
}

TPCANMsg Can::rx() {
    TPCANMsg msg;
    DWORD status;

    while ((status = CAN_Read(m_handle, &msg)) == PCAN_RECEIVE_QUEUE_EMPTY) {
        usleep(100000);
    }

    if (status != PCAN_NO_ERROR) {
        msg.ID = 0; // NOTE: Set ID to 0 so we can check for error in calling function.
    }
    
    return msg;
}

void Can::set_id(int id) {
    m_tx.ID = id;
}

void Can::tx(int data, int data_length = 1, int msg_type = MSGTYPE_STANDARD) {
    DWORD status;
    m_tx.DATA[0] = data;
    m_tx.LEN = data_length;
    m_tx.MSGTYPE = msg_type;

	sleep(1);  
    // TODO: error check status
    // TODO: look at TDD book and see how I can test this function by subbing
    // out CAN_Write() with a different function for testing.
    // (compile time substitution)
	status = CAN_Write(m_handle, &m_tx);

    return;
}
