// 
// camera_mediator.h
// 
// Version: 1.0
// Date:	April 6, 2020
// 
// Copyright (c) 2020, Gregory Gibson - All rights reserved.
// 
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; version 3 of the License
// 
// 

#pragma once

#include "bbb_uart.h"
#include <unistd.h>
#include <fstream>
#include <chrono>

#define CAMERABUFFSIZ 100

enum class CameraResolution : uint8_t {
  k640x480 = 0,
  k320x240 = 0x11,
  k160x120 = 0x22
};

class CameraMediator
{
public:

    CameraMediator(int port, int rate)
        : uart_port_(port),
          baud_rate_(rate),
          serial_port_(port, rate, kUartDirection, false)
    {

      input_buffer_length_ = 0;
      frameptr = 0;
    }

    // Initial operation process
	//
    //  （1） open port if not already opened before
    //  （1） reset command
    //  （2） set image resolution command
    //  （3） set image compressibility command
	//
	// @return 0 if successful otherwise an error code usually -1 
    int InitializationProcess();

    // Image operation process 
	//
    //    （1）capture a image command
    //    （2）read image data length command
    //    （3）read image data command
    //    （4）stop capture command (resume video)
	//
	// @param output_file an open fstream to write to. It is expected that it is already open
	// @return 0 if successful otherwise returns an error code usually -1
    int TakePicture(std::fstream& output_file);

    ////Basic Commands///////////////////////////////////

    // @return 0 if successful otherwise returns an error code usually -1
    int Initialize();

    // @return true if successful otherwise false
    bool Reset();

    // @return true if successful otherwise false
    bool SetImageSize(CameraResolution resolution);

    // @return true if successful otherwise false
    bool SetImageCompression(uint8_t compression);

    // @return true if successful otherwise false
    bool CaptureImage();

    // @return true if successful otherwise false
    bool ResumeVideo();

	// @return size in bytes of the image taken if sucessful otherwise zero
    uint32_t GetImageDataLength();
	
	// @param read_only when set to true only response is read (no command is sent)
    // @return null pointer if failed to read data or if data is invalid
    // @return otherwise returns pointer to image data
    const uint8_t * ReadImageData(const uint8_t bytes_to_read, bool read_only);


    //// Low level functions////////////////////////////////
    
	UART& GetSerialPort()
    {
      return serial_port_;
    }

    // @return 0 if successful otherwise returns an error code usually -1
    int WriteByte(const char & input)
    {
      return serial_port_.uart_write(static_cast<const void*>(&input), 1);
    }

    // @return 0 if successful otherwise returns an error code usually -1
    int ReadByte(char& byteRead)
    {
      return serial_port_.uart_read(static_cast<void*>(&byteRead), 1);
    }

private:

    ////Command Constants/////////////////////////////////
	
    static const uint8_t kReset;
    static const uint8_t kGenVersion;
    static const uint8_t kSetPort;
    static const uint8_t kReadFrameBuffer;
    static const uint8_t kGetFrameBufferLength;
    static const uint8_t kFrameBufferControl;
    static const uint8_t kDownsizeCtrl;
    static const uint8_t kDownsizeStatus;
    static const uint8_t kReadData;
    static const uint8_t kWriteData;
    static const uint8_t kCommMotionCtrl;
    static const uint8_t kCommMotionStatus;
    static const uint8_t kCommMotionDetected;
    static const uint8_t kMotionCtrl;
    static const uint8_t kMotionStatus;
    static const uint8_t kTvoutCtrl;
    static const uint8_t kOsdAddChar;

    static const uint8_t kStopCurrentFrame;
    static const uint8_t kStopNextFrame;
    static const uint8_t kResumeFrame;
    static const uint8_t kStepFrame;

    static const uint8_t kMotioncontrol;
    static const uint8_t kUartmotion;
    static const uint8_t kActivatemotion;

    static const uint8_t kSetZoom;
    static const uint8_t kGetZoom;
	
	////Serial port constants///////////////////////////////
	
    static const UART_TYPE kUartDirection;

    //// Low level functions////////////////////////////////

	// @param flush_flag when set to true will erase data in serial ports input buffer before sending command
	// @param wait_for_write when set to true will wait for command to be sent out before checking response from camera
    bool RunCommand(const uint8_t cmd, const uint8_t *args, const uint8_t args_count, const uint8_t response_length, bool flush_flag = true, bool wait_for_write = false);

    // @return if command sent successfully returns the number of bytes written otherwise zero
    int SendCommand(const uint8_t cmd, const uint8_t args[], const uint8_t args_count);


    // @input bytes_to_read: number of bytes to read
    // @input timeout: timeout for each byte to read in miliseconds
    // @return number of bytes read
    int ReadResponse(const uint8_t bytes_to_read, const uint8_t timeout);

    
    // @returns true if command is valid in input_buffer_[] otherwise returns false
    bool VerifyResponse(const uint8_t command);

    ///////////////////////////////////////////////

    uint8_t  input_buffer_[CAMERABUFFSIZ]; //general buffer used for reading responses from camera
    uint8_t  input_buffer_length_; //keeps track of size of input_buffer_[]
    uint16_t frameptr; //used to keep track of what is the next index of the image that is being read from the camera

    UART serial_port_;   //interface for serial port
    int uart_port_;      //parameter to configure serial_port_: port number (setup specific)
    int baud_rate_;      //parameter to configure serial_port_: port baudrate (typically 38400)

};

class Chronometer
{

public:
  void StartTimer()
  {
    start_time_ = std::chrono::steady_clock::now();
  }

  void StopTimer()
  {
    stop_time_= std::chrono::steady_clock::now();
  }

//  std::chrono::duration<double, std::milli> GetDuration()
//  {
//    return stop_time_ - start_time_;
//  }
  double GetDurationMillisec()
  {
     std::chrono::duration<double, std::milli> duration_ms = stop_time_ - start_time_;
     return duration_ms.count();
  }
private:
  std::chrono::time_point<std::chrono::steady_clock> start_time_;
  std::chrono::time_point<std::chrono::steady_clock> stop_time_;
};

//class CameraResponseSimulator
//{
//public:
//  CameraResponseSimulator(int uart_num, int baud_rate, bool two_stop_bits):
//        serial_port_(uart_num, baud_rate, BOTH, two_stop_bits)
//    {}
//
//  bool Init()
//  {
//      return serial_port_.init() == 0;
//  }
//
////  void Run()
////  {
////    while(true)
////    {
////
////      if(!ReadCharacter(Camera::kResetCmd[0])) //all commands start by 0x56
////      {
////        continue;
////      }
////      if(!ReadCharacter(Camera::kResetCmd[1])) //second bit is always 0
////      {
////        continue;
////      }
////
////      //third bit is unique so now we should be able to start to deduce which command it is
////      char read_data = 0;
////      bool data_available = serial_port_.uart_read(static_cast<void*>(&read_data) , 1) > 0;
////      if(!data_available)
////      {
////        continue;
////      }
////      //string remaining_command_data = "";
////      if(read_data ==  Camera::kResetCmd[2])
////      {
////        if(ReadCharacter(Camera::kResetCmd[3]))
////        {
////          //write acknowledge
////          cout << "Writing reset ack" << endl;
////          if(serial_port_.uart_write(static_cast<const void*>(Camera::kResetAck), 4) != 0)
////          {
////            cerr << "Error writing reset acknowledge" << endl;
////          }
////          continue;
////        }
////      }
////
////      else if(read_data == Camera::kCaptureImgCmd[2] || read_data == Camera::kStopCaptureCmd[2])
////      {
////        if(ReadCharacter(Camera::kCaptureImgCmd[3]) && serial_port_.uart_read(static_cast<void*>(&read_data) , 1) > 0)
////        {
////          if(Camera::kCaptureImgCmd[4] == read_data)
////          {
////            cout << "Writing Capture Image Acknowledge" << endl;
////            if(serial_port_.uart_write(static_cast<const void*>(Camera::kCaptureImgAck), 5) != 0)
////            {
////              cerr << "Error writing Capture Image Acknowledge" << endl;
////            }
////          }
////          else if(Camera::kStopCaptureCmd[4] == read_data)
////          {
////            cout << "Writing Stop Capture Image Acknowledge" << endl;
////            if(serial_port_.uart_write(static_cast<const void*>(Camera::kStopCaptureAck), 5) != 0)
////            {
////              cerr << "Error writing Stop Capture Image Acknowledge" << endl;
////            }
////          }
////          else
////          {
////            cerr << "Invalid command CaptureImgCmd/StopCaptureCmd" << endl;
////          }
////        }
////        else
////        {
////          cerr << "Invalid command CaptureImgCmd/StopCaptureCmd" << endl;
////        }
////        continue;
////      }
////
////      /*else if(read_data ==  Camera::kReadImgDataLengthCmd[2]){
////      }
////
////      else if(read_data ==  Camera::kReadImgDataPartOneCmd[2]){
////      }
////
////
////
////      else if(read_data ==  Camera::kSetImgCompressibilityCmd[2] ||    //all these have the same 3rd byte
////        read_data ==  Camera::kSetImgResolution320x240Cmd[2] ||
////        read_data ==  Camera::kSetImgResolution640x480Cmd[2])
////      {
////
////      }*/
////      else
////      {
////        cerr << "Unexpected command received " << read_data << endl;
////        continue;
////      }
////    }
////  }
//private:
//  bool ReadCharacter(const char expected)
//  {
//    char read_data = 0;
//    bool data_available = false;
//    for (int i = 0; i < 3; i++)
//    {
//      data_available = serial_port_.uart_read(static_cast<void*>(&read_data) , 1) == 1;
//      if(data_available && read_data == expected) //all commands follow by 0
//      {
//        return true;
//        break;
//      }
//      else if(data_available)
//      {
//        cerr << "Expected " << hex << expected << "; Received " << read_data << ";" << endl;
//        break;
//      }
//      usleep(100);
//    }
//    return false;
//  }
//
//  string ReadSeveralCharacters(unsigned length)
//  {
//    const unsigned kMaxBufferSize = 20;
//    char input_buffer[kMaxBufferSize] = {0};
//    string return_string = "";
//    if(length > kMaxBufferSize)
//    {
//      cerr << "Requested too many bytes; Max " << kMaxBufferSize << " Requested " << length << endl;
//      return return_string;
//    }
//    int data_read_length = serial_port_.uart_read(static_cast<void*>(input_buffer), length);
//    if(data_read_length > 0 && data_read_length == static_cast<int>(length))
//    {
//      return_string.append(input_buffer, data_read_length);
//    }
//    else if(data_read_length < static_cast<int>(length))
//    {
//      cerr << "Data read less than length; Read " << data_read_length
//          << " Length expected " << length << endl;
//    }
//    return return_string;
//  }
//
//  UART serial_port_; //interface for serial port
//};

