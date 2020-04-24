
#pragma once
#include <cstddef> //for size_t definition
#include <cstdint>
enum UART_TYPE {TX = 0, RX = 1, BOTH = 2};
//enum UART_RETURN_VALUE {SUCCESS = 0, FAILURE = -1};

class UART {
    private:
        int uartNum;
        int baudRate;
        int uartID;
        bool initFlag;
        bool twoStopBits;
        UART_TYPE uartType;
        
    public:
		/**
		 * Constructor
		 * uartNum - the number that identifies the UART. e.g. 1 - 5 is the common range. 
		 *			 Be sure to make sure that the UART is freed by your system and can be used.
		 * baudRate - The rate at which this baud rate will send information. 
		 * uartType - Identifies in which mode the UART will operate in.
		 *			  TX - Sending
		 *			  RX - Receiving
		 *			  BOTH - Sending and receiving, keep in mind that this means the UART will be connected at
		 *				     two different ports.
		 * twoStopBits - Identifies whether there will be two stop bits or just a single stop bit at the end
		 *				 of each message sent by UART.
		 */
        UART(int uartNum, int baudRate, UART_TYPE uartType, bool twoStopBits);
        
		/**
		 * Initializes UART. Must be called before operating on UART.
		 * Sets UART using Termios2 interface to set UART configurations specified in constructor.
		 */
        int init();

        bool is_initialized()
        {
          return initFlag;
        }
		/**
		 * Writes data from UART. 
		 * data - Buffer to read from.
		 * len - number of bytes to read from data buffer.
		 */
        int uart_write(const void* data, size_t len);
        
        int write_byte(const uint8_t data);

        int wait_for_write();

		/**
		 * Reads data from UART.
		 * buffer - the buffer to read the data into.
		 * len - the number of bytes to read into the buffer of the data in the UART.
		 */
        int uart_read(void* buffer, size_t len);


        int flush_input();
    /*    Doesn't work seems to always return zero
     * int data_to_read();*/

		/**
		 * Writes zeroes at Baud rate for specified amount of time.
		 * msTime - amount of time in milliseconds to write zeros.
		 *
		 * This function is useful to fufill minimum required hold times that
		 *	    some protocols require.
		 * This function is gaurenteed to hold for atleast the amount of time specified,
		 *	    but might hold for longer due to hardware/software context switchs.
		 */
        int write_zeros(long msTime);
		
		/**
		 * Writes ones at Baud rate for specified amount of time.
		 * msTime - amount of time in milliseconds to write ones. 
		 * 
		 * This function is useful to fufill minimum required hold times that
		 *	    some protocols require.
		 * This function is gaurenteed to hold for atleast the amount of time specified,
		 *	    but might hold for longer due to hardware/software context switchs.
		 */
        int write_ones(long msTime);
        
		/**
		 * Destructor
		 *
		 * Closes UART file descriptor.
		 */	
        ~UART();
};
