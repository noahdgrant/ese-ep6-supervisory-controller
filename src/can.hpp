#ifndef CAN_HPP
#define CAN_HPP

#include <libpcan.h>

#define SUPERVISORY_CONTROLLER      0x100
#define ELEVATOR_CONTROLLER         0x101
#define CAR_CONTROLLER              0x200
#define FLOOR_ONE_CONTROLLER        0x201
#define FLOOR_TWO_CONTROLLER        0x202
#define FLOOR_THREE_CONTROLLER      0x203

#define PCAN_RECEIVE_QUEUE_EMPTY    0x00020U
#define PCAN_NO_ERROR               0x00000U

class Can {
public:
    Can(int id);

    void close();
    void open();
    TPCANMsg rx();
    void set_id(int id);
    void tx(int data, int data_length = 1, int msg_type = MSGTYPE_STANDARD);

private:
    HANDLE m_handle;
    TPCANMsg m_tx;
};

#endif
