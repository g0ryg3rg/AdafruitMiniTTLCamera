#include "bbb_uart.h"

#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
//#include <asm/termbits.h>
#include <string.h>
#include <errno.h>
#include <termios.h>


using namespace std;
//#define DEBUG         //--> flag to debug termios settings and flags for open()

UART::UART(int num, int baud, UART_TYPE type, bool twoStopBitsBool) {
    uartNum = num;    
    baudRate = baud;
    uartType = type;
    twoStopBits = twoStopBitsBool;
    
    initFlag = false;
    uartID = -1;     //-1 indicates not intialized
}



int UART::init() {
    
    string namepath = "/dev/ttyO" + to_string(uartNum);
    
    /* Open device in appropriate mode */
    int open_flags = O_NOCTTY | O_NDELAY;
    if (uartType == TX) {
      open_flags |= O_WRONLY;
    }
    else if (uartType == RX) {
      open_flags |= O_RDONLY;
    }
    else {
      open_flags |= O_RDWR;
    }
    /* Checking if opened correctly */
    uartID = open(namepath.c_str(), open_flags);
    if (uartID < 0)
    {
      cerr << __FILE__ << ":" << __LINE__ << ": " << "INIT: UART" << uartNum << " could not be opened" << endl;
      perror("Init port open error");
      return -1;
    }

    //cout << hex << "Open flags: " << open_flags << endl;

    
    
    struct termios serial_settings; // Struct to be populated with uart data

    /* get existing info on uart */
     if (tcgetattr(uartID,&serial_settings) < 0)
     {
       cerr << __FILE__ << ":" << __LINE__ << ": " << "INIT: UART" << uartNum << " Termios getting failure" << endl;
       //bzero(&serial_settings,sizeof(serial_settings));
       return -1;
     }


    //Convert baud for termios structure
    int baud = 0;
    switch(baudRate){
    case 1200:
        baud = B1200;
        break;
    case 1800:
        baud = B1800;
        break;
    case 2400:
        baud = B2400;
        break;
    case 4800:
        baud = B4800;
        break;
    case 9600:
        baud = B9600;
        break;
    case 19200:
        baud = B19200;
        break;
    case 38400:
        baud = B38400;
        break;
    case 57600:
        baud = B57600;
        break;
    case 115200:
        baud = B115200;
        break;
    case 230400:
        baud = B230400;
        break;
    case 460800:
        baud = B460800;
        break;
    case 500000:
        baud = B500000;
        break;
    case 576000:
        baud = B576000;
        break;
    case 921600:
        baud = B921600;
        break;
    case 1000000:
        baud = B1000000;
        break;
    case 1152000:
        baud = B1152000;
        break;
    case 1500000:
        baud = B1500000;
        break;
    case 2000000:
        baud = B2000000;
        break;
    case 2500000:
        baud = B2500000;
        break;
    case 3000000:
        baud = B3000000;
        break;
    case 3500000:
        baud = B3500000;
        break;
    case 4000000:
        baud = B4000000;
        break;
    default:
      cerr << __FILE__ << ":" << __LINE__ << ": " << "INIT: UART" << "Incorrect baud rate";
        return -1;
    }

    cfmakeraw(&serial_settings);
    if(cfsetispeed(&serial_settings, baud) < 0 || cfsetospeed(&serial_settings, baud) < 0)
    {
      cerr << __FILE__ << ":" << __LINE__ << ": " << "INIT: UART" << "Incorrect baud rate";
      return -1;
    }

    //Control Modes
    // serial_settings.c_cflag &= ~CSIZE; // clear current char size mask ; already done by cfmakeraw()
    serial_settings.c_cflag |= CS8 | CLOCAL; //size 8 bits; ignore control lines (modem)

    //serial_settings.c_cflag |= PARENB;// Parity set to dont check by cfmakeraw() might want to set to add a flag to check it
    //serial_settings.c_cflag |= PARODD;
    if(RX == uartType || BOTH == uartType)
    {
      serial_settings.c_cflag |= CREAD; //enable receiver
    }
    else
    {
      serial_settings.c_cflag &= ~CREAD; //disable receiver
    }
    serial_settings.c_cflag &= ~CRTSCTS; // Disable RTS/CTS hardware flow control (most common)
    if (twoStopBits)
    {
      serial_settings.c_cflag |= CSTOPB; //two stop bits set
    }
    else
    {
      serial_settings.c_cflag &= ~CSTOPB; //one stop bits set
    }


    //Input Modes
    serial_settings.c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL); // Disable any special handling of received bytes
    serial_settings.c_iflag &= ~(IXON | IXOFF | IXANY); // Turn off s/w flow ctrl

    //Output Mode
    serial_settings.c_oflag &= ~ONLCR; // Prevent conversion of newline to carriage return/line feed
    //serial_settings.c_oflag &= ~OPOST; // Prevent special interpretation of output bytes (e.g. newline chars); already done by cfmakeraw()

    //Local Mode
    serial_settings.c_lflag &= ~ECHOE;  // Disable erasure
    //serial_settings.c_lflag &= ~ICANON; //Disable Cannonical Mode(process input when new line is received); already done by cfmakeraw()
    //serial_settings.c_lflag &= ~ECHO;   // Disable echo; already done by cfmakeraw()
    //serial_settings.c_lflag &= ~ECHONL; // Disable new-line echo; already done by cfmakeraw()
    //serial_settings.c_lflag &= ~ISIG; // Disable interpretation of INTR, QUIT and SUSP; already done by cfmakeraw()

    serial_settings.c_cc[VTIME] = 0;
    serial_settings.c_cc[VMIN]  = 0;

//#ifdef DEBUG
//    cout << "Termios settings \n" << hex
//      << serial_settings.c_iflag << endl
//      << serial_settings.c_oflag << endl
//      << serial_settings.c_cflag << endl
//      << serial_settings.c_lflag << endl;
//#endif

    
    /* Writing termios options to uart */
    tcflush(uartID,TCIOFLUSH);
    if(tcsetattr(uartID,TCSANOW,&serial_settings) < 0)
    {
      cerr << __FILE__ << ":" << __LINE__ << ": " << "INIT: UART" << uartNum << " Termios setting failure" << endl;
      return -1;
    }
    
    if (ioctl(uartID, TIOCEXCL, NULL) < 0) {
      cerr << __FILE__ << ":" << __LINE__ << ": " << "INIT: UART" << uartNum << " Failed getting exclusive access to port" << endl;
      return -1;
    }

#ifdef DEBUG
    struct termios current_serial_settings;
    tcgetattr(uartID, &current_serial_settings);
    bool settings_equal = current_serial_settings.c_iflag == serial_settings.c_iflag;
    settings_equal &= current_serial_settings.c_oflag == serial_settings.c_oflag;
    settings_equal &= current_serial_settings.c_cflag == serial_settings.c_cflag;
    settings_equal &= current_serial_settings.c_lflag == serial_settings.c_lflag;
    if(!settings_equal)
    {
      cerr << __FILE__ << ":" << __LINE__ << ": " << "Settings not applied correctly" << endl;
    }
#endif



    initFlag = true; //init completed
    return 0;
}

//Init version with Termios2; Bug right now uart_read() returns bad file director
// --> uncomment #include <asm/termbits.h>
//int UART::init()
//{
//    
//  string name = "/dev/ttyO";
//  string namepath = name + to_string(uartNum);
//
//  /* Open device in appropriate mode */
//  /* Open device in appropriate mode */
//  int open_flags = O_NOCTTY | O_NDELAY;
//  if (uartType == TX) {
//    //uartID = open(namepath.c_str(), O_WRONLY | O_NOCTTY | O_NDELAY); //Removed O_NDELAY based on https://stackoverflow.com/questions/41841772/linux-serial-read-blocks-minicom
//    open_flags |= O_WRONLY;
//  }
//  else if (uartType == RX) {
//    //uartID = open(namepath.c_str(), O_RDONLY | O_NOCTTY | O_NDELAY);
//    open_flags |= O_RDONLY;
//  }
//  else {
//      //uartID = open(namepath.c_str(), O_RDWR | O_NOCTTY | O_NDELAY);
//    open_flags |= O_RDWR;
//  }
//  uartID = open(namepath.c_str(), open_flags);
//  if (uartID < 0) {
//      cerr << "__FILE__ __LINE__INIT: UART" << uartNum << " could not be opened" << endl;
//      perror("Init port open error");
//      return -1;
//  }
//  /* Checking if opened correctly */
//  if (uartID < 0) {
//    cerr << __FILE__ << ":" << __LINE__ << ": " << uartNum << " could not be opened" << endl;
//    return -1;
//  }
//
//  /* Struct to be populated with uart data */
//  struct termios2 uartTerm;
//
//  /* getting info on uart */
//  if (ioctl(uartID, TCGETS2, &uartTerm) < 0) {
//      cerr << "INIT: UART" << uartNum << " Termios2 getting failure" << endl;
//      return -1;
//  }
//
//  uartTerm.c_cflag &= ~CBAUD; //Ignoring baudrate
//  uartTerm.c_cflag |= BOTHER; //termios2, other baud rate
//  uartTerm.c_cflag |= CLOCAL; //Ignore control lines
//
//  if (uartType == TX) {
//      uartTerm.c_ospeed = baudRate; //Setting output rate
//  }
//  else if (uartType == RX) {
//      uartTerm.c_cflag |= CREAD; //Reading
//      uartTerm.c_ispeed = baudRate; //setting input rate
//  }
//  else {
//      uartTerm.c_cflag |= CREAD; //Reading and writing
//      uartTerm.c_ospeed = baudRate;
//      uartTerm.c_ispeed = baudRate; //Setting output and input speeds
//  }
//  if (twoStopBits == true) {
//      uartTerm.c_cflag |= CSTOPB; //two stop bits set
//  }
//
//  /* Writing termios options to uart */
//  if (ioctl(uartID, TCSETS2, &uartTerm) < 0) {
//      cerr << "INIT: UART" << uartNum << " Termios2 setting failure" << endl;
//      return -1;
//  }
//
//  if(close(uartID))
//  {
//    cerr << __FILE__ << ":" << __LINE__ << ": " << "INIT: UART" << uartNum << " could not be opened" << endl;
//    perror("Init port close error");
//    return -1;
//  }
//
//  uartID = open(namepath.c_str(), open_flags);
//  if (uartID < 0) {
//      cerr << "__FILE__ __LINE__INIT: UART" << uartNum << " could not be opened" << endl;
//      perror("Init port open error");
//      return -1;
//  }
//
//  initFlag = 1; //init complete, recoginizing
//
//  return 0;
//}

int UART::uart_write(const void* data, size_t len)
{
  if (!initFlag) {
      cerr << "uart_write: UART" << uartNum << " has not been initiated." << endl;
      return -1;
  }

  const int return_value = write(uartID, data, len);
  if (return_value != len || return_value < 0) // write() is expected to return the same number of bytes written (len)
  {
    cerr << "Write failed return value: " << return_value << endl;
    if(return_value < 0)
    {
      perror("Write error");
    }
    return -1;
  }

  return 0;
}

int UART::write_byte(const uint8_t data)
{
  return uart_write(&data, 1);
}

int UART::wait_for_write()
{
  if (!initFlag) {
      std::cerr << "uart_read: UART" << uartNum << " has not been initiated." << std::endl;
      return -1;
  }
  return tcdrain(uartID);
}

int UART::uart_read(void* buffer, size_t len) {
  int count;

  if (!initFlag) {
      cerr << "uart_read: UART" << uartNum << " has not been initiated." << endl;
      return -1;
  }

  count = read(uartID, buffer, len);
  //if ((count < 0)) {
    //cerr << "UART" << uartNum << " Couldnt read data" << endl;
    //cerr << __FILE__ << ":" << __LINE__ << ": " << "FD: " << uartID << endl;
    //return -1;
  //}

  return count;
}

int UART::flush_input()
{
  if (!initFlag) {
      cerr << "uart_read: UART" << uartNum << " has not been initiated." << endl;
      return -1;
  }

  return tcflush(uartID, TCIFLUSH);
}

/*Doesn't work seems to always return zero
 * int UART::data_to_read()
{
  if (!initFlag) {
      cerr << "uart_read: UART" << uartNum << " has not been initiated." << endl;
      return -1;
  }
  int bytes= 0;
  if(ioctl(uartID, FIONREAD, &bytes) < 0)
  {
    cerr << __FILE__ << ":" << __LINE__ << ": " << " error checking input buffer" << endl;
    return -1;
  }
}*/

int UART::write_zeros(long interval) {
  if (!initFlag) {
      cerr << "Write_zeros: UART" << uartNum << " has not been initiated." << endl;
      return -1;
  }

  if(ioctl(uartID, TIOCSBRK) < 0) {
      cerr << "write_zeros: UART" << uartNum << " failed" << endl;
      return -1;
  }

  usleep(interval); //Sleep while in default state of writing zeroes
  return 0;
}

int UART::write_ones(long interval) {
  if (!initFlag) {
      cerr << "Write_ones: UART" << uartNum << " has not been initiated." << endl;
      return -1;
  }
  if (ioctl(uartID, TIOCCBRK) < 0) {
      cerr << "write_zeroes:" << uartNum << " failed" << endl;
      return -1;
  }

  usleep(interval); //Sleep while in default state of writing 1's
  return 0;
}

UART::~UART() {     
    if (uartID != -1) {     
        close(uartID);
    }
}

