#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> 
#include <fcntl.h>    					// O_RDWR
#include <unistd.h>
#include <libpcan.h>   					// PCAN library

#include "can.hpp"

Can::Can(int id) {
        m_tx.ID = id;
}

void Can::close() {
    CAN_Close(m_handle);

    printf("[CAN] close\n");
    return;
}

void Can::open() {
    DWORD status;

	m_handle = LINUX_CAN_Open("/dev/pcanusb32", O_RDWR);
    // TODO: error when 'status' isn't right
	status = CAN_Init(m_handle, CAN_BAUD_125K, CAN_INIT_TYPE_ST);
	status = CAN_Status(m_handle);

    printf("[CAN] init\n");
    printf("[CAN] open\n");

    return;
}

TPCANMsg Can::rx() {
    TPCANMsg msg;
    DWORD status;

    while ((status = CAN_Read(m_handle, &msg)) == PCAN_RECEIVE_QUEUE_EMPTY) {
        sleep(1);
    }

    if (status != PCAN_NO_ERROR) {
        printf("[CAN] Error 0x%x\n", (int)status);
        msg.ID = 0; // NOTE: Set ID to 0 so we can check for error in calling function.
    } else if(msg.ID == 0x01 && msg.LEN == 0x04) {
        printf("[CAN] RX: ID = 0x%X LEN = 0x%X DATA = 0x%X \n",
               (int)msg.ID,
               (int)msg.LEN,
               (int)msg.DATA[0]);
    } else {
        // Ignore bus status messages
    }
    
    return msg;
}

void Can::set_id(int id) {
    m_tx.ID = id;
}

void Can::tx(int data, int data_length, int msg_type) {
    DWORD status;
    m_tx.DATA[0] = data;
    m_tx.LEN = data_length;
    m_tx.MSGTYPE = msg_type;

    // TODO: error check status
    // TODO: look at TDD book and see how I can test this function by subbing
    // out CAN_Write() with a different function for testing.
    // (compile time substitution)
	status = CAN_Write(m_handle, &m_tx);

    printf("[CAN] TX: ID = 0x%X LEN = 0x%X DATA = 0x%X \n",
           (int)m_tx.ID,
           (int)m_tx.LEN,
           (int)m_tx.DATA[0]);

    return;
}
