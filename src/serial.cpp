#include <fcntl.h>
#include <iostream>
#include <string.h>
#include <termios.h>
#include <unistd.h>

int main() {
  int serial_port = open("/dev/ttyACM0", O_RDWR);

  // Check for errors
  if (serial_port < 0) {
    std::cout << "Error " << errno << " opening " << "/dev/ttyACM0" << ": "
              << strerror(errno) << std::endl;
  }

  // Create new termios struc, we call it 'tty' for convention
  struct termios tty;
  memset(&tty, 0, sizeof tty);

  // Read in existing settings, and handle any error
  if (tcgetattr(serial_port, &tty) != 0) {
    std::cout << "Error " << errno << " from tcgetattr: " << strerror(errno)
              << std::endl;
  }

  tty.c_cflag &= ~PARENB; // Clear parity bit, disabling parity (most common)
  tty.c_cflag &= ~CSTOPB; // Clear stop field, only one stop bit used in
                          // communication (most common)
  tty.c_cflag |= CS8;     // 8 bits per byte (most common)
  tty.c_cflag &= ~CRTSCTS; // Disable RTS/CTS hardware flow control (most common)
  tty.c_cflag |= CREAD | CLOCAL; // Turn on READ & ignore ctrl lines (CLOCAL = 1)

  tty.c_lflag &= ~ICANON;
  tty.c_lflag &= ~ECHO;   // Disable echo
  tty.c_lflag &= ~ECHOE;  // Disable erasure
  tty.c_lflag &= ~ECHONL; // Disable new-line echo
  tty.c_lflag &= ~ISIG;   // Disable interpretation of INTR, QUIT and SUSP
  tty.c_iflag &= ~(IXON | IXOFF | IXANY); // Turn off s/w flow ctrl
  tty.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR |
                   ICRNL); // Disable any special handling of received bytes

  tty.c_oflag &= ~OPOST; // Prevent special interpretation of output bytes (e.g.
                         // newline chars)
  tty.c_oflag &= ~ONLCR; // Prevent conversion of newline to carriage return/line feed

  tty.c_cc[VTIME] = 10; // Wait for up to 1s (10 deciseconds), returning as soon
                        // as any data is received.
  tty.c_cc[VMIN] = 0;

  // Set in/out baud rate to be 9600
  cfsetispeed(&tty, B9600);
  cfsetospeed(&tty, B9600);

  // Save tty settings, also checking for error
  if (tcsetattr(serial_port, TCSANOW, &tty) != 0) {
    std::cout << "Error " << errno << " from tcsetattr: " << strerror(errno)
              << std::endl;
  }

  // Allocate memory for read buffer, set size according to your needs
  char read_buf[256];
  memset(&read_buf, '\0', sizeof(read_buf));

  // Non-blocking read of 1 byte from the serial port,
  // read will return as soon as some data is available.
  int num_bytes = read(serial_port, &read_buf, sizeof(read_buf));

  // Output the data
  if (num_bytes < 0) {
    std::cout << "Error reading: " << strerror(errno) << std::endl;
  } else {
    std::cout << "Read " << num_bytes
              << " bytes. Received message: " << read_buf << std::endl;
  }

  // Close the serial port
  close(serial_port);
  return 0;
}
