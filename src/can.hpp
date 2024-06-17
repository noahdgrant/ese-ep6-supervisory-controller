#ifndef CAN_HPP
#define CAN_HPP

class Can {
public:
    Can(int id);

    void close();
    void open();
    void rx();
    void set_id(int id);
    void tx(int data, int data_length = 1, int msg_type = MSGTYPE_STANDARD);

private:
    HANDLE m_handle;
    TPCANMsg m_tx;
};

#endif
