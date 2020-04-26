#include "camera_mediator.h"
//#include "uart.h"
#include "chronometer.h"
#include <unistd.h>
#include <string>
#include <iostream>

#define UART_TEST 2

using namespace std;

int main(int argc, char **argv)
{
  int baudrate = 38400;

  const int kCameraPort = 2;
  const bool kTwoStopBits = false;

  cout << "Test Ports App BBB UART lib\n\n";

  //Process command line params
  //std::string directory = "";
  for(int i = 0; i < argc; i++)
  {
    //if(0 == i)
    //{ //This doesn't work
    //  directory = argv[0];//program full path + name of binary file
    //  directory.erase(directory.find_last_of('\\')+1);//remove name of binary file
    //  continue;
    //}
    if(1 == i) //first param is baudrate
    {
      baudrate = atoi(argv[1]);
      break; //exit loop as nothing else to process
    }
  }
  //cout << "Baudrate set to " << baudrate << endl;
  //cout << "Directory set to " << directory << endl;
  CameraMediator camera_mediator = CameraMediator(kCameraPort, baudrate);

  //if(!(camera_mediator.Initialize() == 0))
  //{
  //  cout << "Failed to init the port" << endl;
  //  return -1;
  //}
  UART& serial_port_2 = camera_mediator.GetSerialPort();

  uint8_t data = 0;
  uint8_t returned_data[10] = {0};
  Chronometer timer;
  while(true)
  {
    cout << "What would you like to write (enter x to exit)?" << endl;
    cin >> data;
    const char* command;
    int command_length = 0;
    char variable_command[10] = {0};
    bool write_read_procedure = false;

    if('x' == data)
    {
      //Exit program
      break;
    }
    else if('i' == data)
    {
      cout << "Initilization Process Command" << endl;
      timer.StartTimer();
      if(camera_mediator.InitializationProcess() != 0)
      {
      }
      timer.StopTimer();
      cout << "Took " << timer.GetDurationMillisec() << " milliseconds" << endl;
    }
    else if('p' == data)
    {
      cout << "Take Picture Command" << endl;
      string output_file_name = "/home/test.jpg";
      fstream output_file;
      output_file.clear(); //clear error flags
      output_file.open(output_file_name, ios::out | ios::trunc);
      timer.StartTimer();
      //for(unsigned i = 0; i < 100; i++)
      //{
        int return_value = camera_mediator.TakePicture(output_file);
        //if(return_value == -21 || return_value == -20)
        //{
          //file is closed and reopened with the truncate flag to discard bad data
          //output_file.close();
          //output_file.clear(); //clear error flags
          //output_file.open(output_file_name, ios::out | ios::trunc);
          //sleep(i);
          //continue;
        //}
        //else if(return_value < 0)
        //{
        //  cout << "Another error; stopping attempts to take picture" << endl;
          //break;
        //}
        //else if(return_value == 0)
        //{
          //cout << "Took " << i << " attempts" << endl;
          //break;
        //}
      //}
      timer.StopTimer();
      cout << "Took " << timer.GetDurationMillisec() << " milliseconds" << endl;
      output_file.close();
      if(output_file.bad())    //bad() function will check for badbit
      {
        cerr <<"Writing image to file failed"<<endl;
      }

      output_file.seekp(ios_base::end);

      long file_size = output_file.tellp(); //bug doesnt work atm
      cout << "File size " << file_size << endl;
    }
    else if('1' == data)
    {
      cout << "Init port" << endl;
      if(camera_mediator.Initialize() != 0)
        cerr << "Failed to read ack" << endl;
    }
    else if('2' == data)
    {
      cout << "Reset" << endl;
      if(!camera_mediator.Reset())
        cerr << "Failed to read ack" << endl;
    }
    else if('3' == data)
    {
      cout << "What resolution would you like? 1 = 640x480 2 = 320x240 3 = 160x120" << endl;
      unsigned input = 0;
      cin >> input;
      CameraResolution resolution = CameraResolution::k640x480;
      switch(input)
      {
      case 3:
        resolution = CameraResolution::k160x120;
        break;
      case 2:
        resolution = CameraResolution::k320x240;
        break;
      case 1:
      default:
        //640x480
        //already set above
        break;
      }
      cout << "Image resoultion" << endl;
      if(!camera_mediator.SetImageSize(resolution))
        cerr << "Failed to read ack" << endl;
    }
    else if('4' == data)
    {
      cout << "Image compress 36" << endl;
      if(!camera_mediator.SetImageCompression(36))
        cerr << "Failed to read ack" << endl;
    }
    else if('5' == data)
    {
      cout << "Capture image" << endl;
      if(!camera_mediator.CaptureImage())
        cerr << "Failed to read ack" << endl;
    }
    else if('6' == data)
    {
      cout << "Read captured image length" << endl;
      cout << "Image length: " << dec << camera_mediator.GetImageDataLength() << endl;
      cout << hex;
    }
    else if('8' == data)
    {
      cout << "Resume video" << endl;
      if(!camera_mediator.ResumeVideo())
        cerr << "Failed to read ack" << endl;
    }
    else
    {
      cout << "Invalid command" << endl;
      continue;
    }


//    if(write_read_procedure)
//    {
//      cout << "Writing : ";
//      for(int i = 0; i < command_length; i++)
//      {
//        cout << hex << static_cast<int> (command[i]) << " ";
//      }
//      cout << endl;
//
//      if(serial_port_2.uart_write(static_cast<const void*>(command), command_length) != 0)
//      {
//        cerr << "Failed to write command" << endl;
//      }
//
//      bool data_received = false;
//      int data_received_count = 0;
//      unsigned kReadTimeout = 100; // unit milliseconds
//      for(unsigned i = 0; i < (kReadTimeout * 10); i++)
//      {
//        //cout << "Read iteration " << i << endl;
//
//        //data_received_count = serial_port_2.data_to_read();
//        //cout << "Data available to read: " << data_received_count << endl;
//        //if(data_received_count >= command_length)
//        //{
//          data_received_count = serial_port_2.uart_read(returned_data, command_length);
//          cout << "Data read: " << data_received_count << endl;
//
//          if(data_received_count >= command_length)
//          {
//            data_received = true;
//            cout << " data_received true" << endl;
//            break;
//          }
//        //}
//        usleep(100); //max time before "timeout"; 100 usec x kReadTimeout * 10 = kReadTimeout
//      }
//      cout << " data_received " << data_received << endl;
//      if(data_received)
//      {
//        cout << "What has been read on 2 : ";
//        for(int i = 0; i < data_received_count; i++)
//        {
//          cout << hex << static_cast<int>(returned_data[i]) << " ";
//        }
//        cout << endl;
//      }
//      else
//      {
//        cerr << "Failed to read data on port 2" << endl;
//        perror("Read Error: ");
//      }
//    }
  } //end of while(true)

  return 0;
}


#if UART_TEST == 1
  if(argc == 1) //any command line argument will activate the simulator i.e argc > 1
  {
    cout << "Camera Test App\n";
    Camera camera = Camera(kCameraPort, kRate, kTwoStopBits);

    /*while(true)
    {
      if(camera.InitializationProcess() != 0)
      {
        break;
      }
      usleep(5000);
    }*/
    bool exit_program = false;
    while(!exit_program)
    {
      char action;
      cout << "What would you like to do: i(nitialize), w(rite), r(ead), q(uit)" << endl;
      //cin.ignore(std::numeric_limits<std::streamsize>::max()); // flush cin buffer
      //cin.ignore(20);
      cin >> action;
      switch(action)
      {
      case 'i':
        camera.Initialize();
        //cam2.Initialize();
        break;

      case 'w':
      {
        string user_input = "";
        cin >> user_input;
        int byte_to_write = stoi(user_input,0,16);
        //cout << hex;
        //cout << "Writing " << static_cast<int>(byte_to_write) << endl;
        if(camera.WriteByte(static_cast<char>(byte_to_write)) != 0)
        {
          cerr << hex;
          cerr << "Failed to write " << static_cast<int>(byte_to_write) << endl;
        }
        break;
      }

      case 'r':
      {
        char byte_read = 0;
        if(camera.ReadByte(byte_read) != 0)
        {
          cerr << "Failed to read byte" << endl;
        }
        else
        {
          cout << hex;
          cout << "Read " << static_cast<int>(byte_read) << endl;
        }
        break;
      }

      case '1':
        cout << "Initialization Process" << endl;
        camera.InitializationProcess();
      break;

      case '2':
      {
        cout << "Reset Command" << endl;
        int return_value = camera.Reset();
        if(return_value != 0)
        {
          cout << "Failed to send Reset command. Value returned: " << return_value << endl;
        }
        else
        {
          cout << "Successfully sent Reset command" << endl;
        }
        break;
      }
      case 'q':
        exit_program = true;
        break;

      default:
        cout << "Invalid command" << endl;
        break;
      }
    }
  }
  else //any command line argument will activate the simulator
  {
    cout << "Camera simulator app\n";
    CameraResponseSimulator camera_simulator = CameraResponseSimulator(kCameraSimulatorPort, kRate, kTwoStopBits);
    if(camera_simulator.Init())
    {
      camera_simulator.Run();
    }
    else
    {
      cout << "Failed to init port \n";
    }
  }
  return 0;
}
#endif

#if  UART_TEST == 0
  cout << "Test Ports App UART lib\n\n";

  if(!(uartInitialize(kCameraPort, kRate) == 0 && uartInitialize(kCameraSimulatorPort, kRate) == 0))
  {
    cout << "Failed to init the ports" << endl;
    uartDeinit(kCameraPort);
    uartDeinit(kCameraSimulatorPort);
    return -1;
  }

  unsigned char data = 0;
  unsigned char returned_data[10] = {0};
  bool firstIteration = false;
  while(true)
  {

    cout << "What would you like to write (enter x to exit)?" << endl;
    cin >> data;
    if('x' == data)
    {
      break;
    }

    uartWriteByte(kCameraPort, data);
    uartWriteByte(kCameraSimulatorPort, data);
    if(firstIteration)
    {
      //read buffer somehow it has empty data... which seems like a bug
      uartRead(kCameraPort);
      uartRead(kCameraSimulatorPort);
      firstIteration = false
    }
    uartRead(kCameraPort);
    uartRead(kCameraSimulatorPort);
    cout << "What has been read on port" << hex << kCameraPort << ": " <<  uartReadBuffer2[0] << endl;
    cout << "What has been read on port" << hex << kCameraSimulatorPort << ": " <<  uartReadBuffer4[0]<< endl;
  }

  uartDeinit(kCameraPort);
  uartDeinit(kCameraSimulatorPort);
  return 0;
}
#endif



