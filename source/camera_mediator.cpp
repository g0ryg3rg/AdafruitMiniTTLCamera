// 
// camera_mediator.cpp
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

#include <stdlib.h> 
#include <stdio.h> 
#include <iostream>
//#include <chrono>

#include "camera_mediator.h"

using namespace std;

//const char CameraMediator::kResetCmd[] = {0x56, 0, 0x26, 0};
//const char CameraMediator::kResetAck[] = {0x76, 0, 0x26, 0};
//const char CameraMediator::kCaptureImgCmd[] = {0x56, 0, 0x36, 1, 0};
//const char CameraMediator::kCaptureImgAck[] = {0x76, 0, 0x36, 0, 0};
//const char CameraMediator::kReadImgDataLengthCmd[] = { 0x56, 0, 0x34, 1, 0};
//const char CameraMediator::kReadImgDataLengthAck[] = { 0x76, 0, 0x34, 0, 4, 0, 0}; // last two chars are the image length; that is 76 00 34 00 04 00 00 XX YY; XX is the high bit and YY is the low byte
//const char CameraMediator::kReadImgDataPartOneCmd[] = {0x56, 0, 0x32, 0x0C, 0, 0x0D, 0, 0}; //56 00 32 0C 00 0D 00 00 XX XX 00 00 YY YY 00 0A
//const char CameraMediator::kReadImgDataPartTwoCmd[] = {0, 0};
//const char CameraMediator::kReadImgDataPartThreeCmd[] = {0, 0x0A};
//const char CameraMediator::kReadImgDataStartAck[] = {0x76, 0, 0x32, 0, 0, 0xFF, 0xD8};
//const char CameraMediator::kReadImgDataEndAck[] = {0xFF, 0xD9, 0x76, 0, 0x32, 0, 0};
//const char CameraMediator::kStopCaptureCmd[] = {0x56, 0, 0x36, 1, 3};
//const char CameraMediator::kStopCaptureAck[] = {0x76, 0, 0x36, 0, 0};
//const char CameraMediator::kSetImgCompressibilityCmd[] = {0x56, 0, 0x31, 5, 1, 1, 0x12, 4}; //56 00 31 05 01 01 12 04 XX; XX ----default value: 36 （range：00 ----FF）
//const char CameraMediator::kSetImgCompressibilityAck[] = {0x76, 0, 0x31, 0, 0};
//const char CameraMediator::kSetImgResolution320x240Cmd[] = {0x56, 0, 0x31, 5, 4, 1, 0, 0x19, 0x11};
//const char CameraMediator::kSetImgResolution640x480Cmd[] = {0x56, 0, 0x31, 5, 4, 1, 0, 0x19, 0};
//const char CameraMediator::kSetImgResolutionAck[] = {0x76, 0, 0x31, 0, 0};

const uint8_t CameraMediator::kReset = 0x26;
const uint8_t CameraMediator::kGenVersion = 0x11;
const uint8_t CameraMediator::kSetPort = 0x24;
const uint8_t CameraMediator::kReadFrameBuffer = 0x32;
const uint8_t CameraMediator::kGetFrameBufferLength = 0x34;
const uint8_t CameraMediator::kFrameBufferControl = 0x36;
const uint8_t CameraMediator::kDownsizeCtrl = 0x54;
const uint8_t CameraMediator::kDownsizeStatus = 0x55;
const uint8_t CameraMediator::kReadData = 0x30;
const uint8_t CameraMediator::kWriteData = 0x31;
const uint8_t CameraMediator::kCommMotionCtrl = 0x37;
const uint8_t CameraMediator::kCommMotionStatus = 0x38;
const uint8_t CameraMediator::kCommMotionDetected = 0x39;
const uint8_t CameraMediator::kMotionCtrl = 0x42;
const uint8_t CameraMediator::kMotionStatus = 0x43;
const uint8_t CameraMediator::kTvoutCtrl = 0x44;
const uint8_t CameraMediator::kOsdAddChar = 0x45;

const uint8_t CameraMediator::kStopCurrentFrame = 0x0;
const uint8_t CameraMediator::kStopNextFrame = 0x1;
const uint8_t CameraMediator::kResumeFrame = 0x3;
const uint8_t CameraMediator::kStepFrame = 0x2;



const uint8_t CameraMediator::kMotioncontrol = 0x0;
const uint8_t CameraMediator::kUartmotion = 0x01;
const uint8_t CameraMediator::kActivatemotion = 0x01;
const uint8_t CameraMediator::kSetZoom = 0x52;
const uint8_t CameraMediator::kGetZoom = 0x53;

const UART_TYPE CameraMediator::kUartDirection = BOTH;

int CameraMediator::InitializationProcess()
{
  CameraResolution resolution = CameraResolution::k640x480;
  uint8_t compression =  0x36;

  int returned_value = Initialize();
  if(returned_value != 0)
  {
    cerr << "Failed to initialize port" << endl;
    return returned_value;
  }

  //  （2） delay 2.5s
  //usleep(2.5*1000*1000);

  //  （3） reset command
  if(!Reset())
  {
    std::cerr << "Failed to reset camera" << std::endl;
    return -1;
  }
  usleep(0.5*1000*1000); //wait half a second before the next command
  //  （4） set image resolution command
  if(!SetImageSize(resolution))
  {
    std::cerr << "Failed to set image size" << std::endl;
    //return -1;
  }
  usleep(0.25*1000*1000); //wait 0.25 seconds
  //  （5） set image compressibility command
  if(!SetImageCompression(compression))
  {
    std::cerr << "Failed to set image compression" << std::endl;
    return -1;
  }
  return 0;
}

int CameraMediator::TakePicture(fstream& output_file)
{

  //output_file.open("test.jpg", std::ofstream::out | std::ofstream::app);

  if(!output_file.is_open())
  {
    cerr << "Output is not open" << endl;
    return -1;
  }

  if(!CaptureImage())
  {
    cerr << "Failed to send capture image command" << endl;
    return -1;
  }
  usleep(2*1000*1000); //wait before the next command
  //the rest of this code is based off Adafruit arduino example
  //https://github.com/adafruit/Adafruit-VC0706-Serial-Camera-Library/blob/master/examples/Snapshot/Snapshot.ino
  const uint32_t image_length = GetImageDataLength();
  cout << "Image is " << dec << image_length << " bytes" << endl;

  bool sent_read_command = false;
  unsigned sent_read_counter = 0;
  uint8_t bytes_to_read = 64;
  for(unsigned remaining_bytes_to_read = image_length; remaining_bytes_to_read > 0; sent_read_counter ++)
  {
    //read 64 bytes or the remaining amount left whichever is smaller
    bytes_to_read = 64;
    if (remaining_bytes_to_read < bytes_to_read)
    {
      bytes_to_read = remaining_bytes_to_read;
    }
    if(sent_read_counter == 3) //every x loops resend command data request
    {
      sent_read_counter = 0;
      sent_read_command = false;
    }
    const uint8_t *buffer = ReadImageData(bytes_to_read, sent_read_command);
    sent_read_command = true;
    if (buffer != nullptr)
    {
      //sometimes data still contains header.... //why doesnt this work
      //if(buffer[0] == 0x76 && buffer[1] == 0x0 && buffer[2] == 0x32 && buffer[3] == 0x0 && buffer[4] == 0x0)
      //{
      //  buffer = &buffer[5]; //trim off header
      //  bytes_to_read -= 5;
      //  cerr << "Timmed header patch" << endl;
      //}
      if(remaining_bytes_to_read == image_length) //if first line check if valid start of jpg image
      {
        if(input_buffer_[5] != 0xff && input_buffer_[6] != 0xd8)
        {
          cerr << "Invalid jpeg header" << endl;
          cout << "Last line read in input is the following:" << endl;
          for(unsigned i = 0; i < input_buffer_length_; i++)
          {
            cout << hex << static_cast<unsigned int>(input_buffer_[i]) << " ";
          }
          cout << endl;
          return -20;
        }
      }
      else if ((remaining_bytes_to_read - bytes_to_read) == 0) //if last line check if valid end of jpg image
      {
        bool footer_found = false;
        for(unsigned i = 5; (i + 1) < input_buffer_length_; i++)
        {
          if(input_buffer_[i] == 0xff && input_buffer_[i+1] == 0xd9)
          {
            footer_found = true;
            break;
          }
        }
        if(!footer_found)
        {
          cerr << "Invalid jpeg footer" << endl;
          cout << "Last line read in input is the following:" << endl;
          for(unsigned i = 0; i < input_buffer_length_; i++)
          {
            cout << hex << static_cast<unsigned int>(input_buffer_[i]) << " ";
          }
          cout << endl;
          return -21;
        }
      }
      output_file.write(reinterpret_cast<const char*>(buffer), bytes_to_read);
    }
    else //try reading again if
    {
      //cerr << "Failed to read image data; remaining_bytes_to_read= " << remaining_bytes_to_read << endl;
      continue;
    }

    remaining_bytes_to_read -= bytes_to_read; //sucessfully read data decrement counter
    sent_read_command = false;
    //if(0 == (bytes_written % (640))) //
    //{
    //  cout << remaining_bytes_to_read << " / " << image_length << " remaining to read from camera" << endl;
    //}
  }

//  cout << "Last line read in input is the following:" << endl;
//  for(unsigned i = 0; i < input_buffer_length_; i++)
//  {
//    cout << hex << static_cast<unsigned int>(input_buffer_[i]) << " ";
//  }
//  cout << endl;
//  while(true)
//  {
//    cout << "Read another 64 bytes?";
//    char input;
//    cin >> input;
//    if('y' == input)
//    {
//      for(unsigned i = 0; i < 100; i++) //read until data
//      {
//        const uint8_t *buffer = ReadImageData(64, true);
//        if (buffer != nullptr)
//        {
//          break;
//        }
//        usleep(100000);
//      }
//      cout << "Data read:" << endl;
//      for(unsigned i = 0; i < input_buffer_length_; i++)
//      {
//        cout << hex << static_cast<unsigned int>(input_buffer_[i]) << " ";
//      }
//
//    }
//    else
//      break;
//  }
  usleep(0.25*1000*1000); //wait before the next command
  if(!ResumeVideo())
  {
    cerr << "Failed to send stop capture command" << endl;
    return -1;
  }
  return 0;
}

int CameraMediator::Initialize()
{
  int return_value = -1;
  if(!serial_port_.is_initialized())
  {
    return_value = serial_port_.init();
  }
  else
  {
    return_value = 0;
  }

  //if(return_value != 0)
  //{
  //    std::cerr << "Failed to initialize port" << std::endl;
  //}

  return return_value;
}

bool CameraMediator::Reset()
{
  uint8_t args[] = {0x0};

  return RunCommand(kReset, args, sizeof(args), 4);
}

bool CameraMediator::SetImageSize(CameraResolution resolution)
{
  uint8_t args[] = {0x05, 0x04, 0x01, 0x00, 0x19, static_cast<uint8_t>(resolution)};

  return RunCommand(kWriteData, args, sizeof(args), 5);
}

bool CameraMediator::SetImageCompression(uint8_t compression)
{
  uint8_t args[] = {0x5, 0x1, 0x1, 0x12, 0x04, compression};
  return RunCommand(kWriteData, args, sizeof(args), 5);
}

bool CameraMediator::CaptureImage()
{
  frameptr = 0;
  uint8_t args[] = {0x1, kStopCurrentFrame};
  return RunCommand(kFrameBufferControl, args, sizeof(args), 5);
}

bool CameraMediator::ResumeVideo() {
  uint8_t args[] = {0x1, kResumeFrame};
  return RunCommand(kFrameBufferControl, args, sizeof(args), 5);
}

uint32_t CameraMediator::GetImageDataLength()
{
  uint8_t args[] = {0x01, 0x00};
  if (!RunCommand(kGetFrameBufferLength, args, sizeof(args), 8))
    return 0;

  uint32_t len = 0;
  //len = input_buffer_[5];
  //len <<= 8;
  //len |= input_buffer_[6];
  //len <<= 8;
  len |= input_buffer_[7];
  len <<= 8;
  len |= input_buffer_[8];

  return len;
}

const uint8_t * CameraMediator::ReadImageData(const uint8_t bytes_to_read, bool read_only)
{
  // doesnt seem to match ptc06 doc. below is what is in the ptc06 doc
  //
  //0C 00 '0D' 00 00 XX XX 00 00 YY YY 00 0A
  //       /\
  //        \__  this is different

  if(!read_only)
  {
    uint8_t args[] = {0x0C, 0x0, 0x0A,
                      0, 0, frameptr >> 8, frameptr & 0xFF,
                      0, 0, 0, bytes_to_read,
                      0, 0x0A};
    serial_port_.flush_input();
    int bytes_sent = SendCommand(kReadFrameBuffer, args, sizeof(args));
    if(bytes_sent != (3 + sizeof(args)))
    {
      return nullptr;
    }
    serial_port_.wait_for_write();
  }
  //overwrite first 5 bytes of buffer before reading as the previous data will look like valid data otherwise
  //read will return with garbage so this is the work around
  for(int i = 0; i < 5; i++)
  {
    input_buffer_[i] = 0;
    input_buffer_length_ = 0;

  }
  //usleep(500000); //doesn't seem to make it any better
  if((ReadResponse(5 + bytes_to_read, 200) != (5 + bytes_to_read)) || !VerifyResponse(kReadFrameBuffer))
  {
    //cerr << "Failed to read or invalid image data header" << endl;
    return nullptr;
  }
  // Original code replaced by the above
  //if (! RunCommand(kReadFrameBuffer, args, sizeof(args), 5, false, true))
  //  return nullptr;
  //
  // read into the buffer
  //if (ReadResponse(bytes_to_read, CAMERADELAY) == bytes_to_read) //if 0 bytes have been read return nullptr
  //{
  //  return nullptr;
  //}

  frameptr += bytes_to_read;

  return &input_buffer_[5]; //return the pointer to the first bit of image data
}

bool CameraMediator::RunCommand(const uint8_t cmd, const uint8_t *args, const uint8_t args_count, const uint8_t response_length, bool flush_flag, bool wait_for_write)
{
  // flush out anything in the buffer?
  if (flush_flag)
  {
    serial_port_.flush_input();
  }
  int bytes_sent = SendCommand(cmd, args, args_count);
  if (wait_for_write)
  {
    serial_port_.wait_for_write();
  }
  if(bytes_sent == (3 + args_count)) //if all the bytes are sent check reponse and validate response
  {
    if (ReadResponse(response_length, 200) != response_length)
      return false;

    return VerifyResponse(cmd);
  }
  else
  {
    cerr << "Failed to run command" << endl;
    return false;
  }
}

int CameraMediator::SendCommand(const uint8_t cmd, const uint8_t args[], const uint8_t args_count)
{
  //cout << "Sending command: " << hex;
  int return_value = 0;
  //all commands start with 0x56 0
  return_value = serial_port_.write_byte(0x56);
  if(return_value < 0)
  {
    //cout << "Failed to send 0x56 " << endl;
    return 0;
  }
  //cout << 0x56 << " ";

  return_value = serial_port_.write_byte(0);
  if(return_value < 0)
  {
    //cout << "Failed to send 0x0 #1" << endl;
    return 1;
  }
  //cout << 0 << " ";

  return_value = serial_port_.write_byte(cmd);
  if(return_value < 0)
  {
    //cout << "Failed to send " << cmd << endl;
    return 2;
  }
  //cout << static_cast<unsigned>(cmd) << " ";

  return_value = serial_port_.uart_write(args, args_count);
  if(return_value < 0)
  {
    //cout << "Failed to send args"<< endl;
    return 3;
  }
  //for(int i = 0; i < args_count; i++)
  //{
  //  cout << static_cast<unsigned>(args[i]) << " ";
  //}
  //cout << endl;

  serial_port_.wait_for_write();
  return args_count + 3; //first 3 bytes + arguments
}

int CameraMediator::ReadResponse(const uint8_t bytes_to_read, const uint8_t timeout)
{
  uint8_t msec_counter = 0;
  uint8_t byte_read = 0;
  for(input_buffer_length_ = 0; (input_buffer_length_ < bytes_to_read) && (msec_counter < timeout);)
  {
    int avail = serial_port_.uart_read(&byte_read , 1);

    if (avail <= 0) //if nothing read then sleep 1 msec
    {
      usleep(1000);
      msec_counter++;
      continue;
    }
    //read byte and reset timeout counter
    msec_counter = 0; //reset counter for next byte
    input_buffer_[input_buffer_length_++] = byte_read;
  }

  //for (int i = 0; i < input_buffer_length_; i++)
  //{
  //  cout << "Buffer[" << i << "] = " << hex << static_cast<unsigned int>(input_buffer_[i]) << endl;
  //}

  return input_buffer_length_;
}

bool CameraMediator::VerifyResponse(const uint8_t command)
{
  return ((0x76 == input_buffer_[0]) &&
      (0 == input_buffer_[1]) &&
      (command == input_buffer_[2]) &&
      (0 == input_buffer_[3]));
}

