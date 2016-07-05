/*
 * read_write.cpp
 *
 *  Created on: 2016. 2. 21.
 *      Author: leon
 */

//
// *********     Read and Write Example      *********
//
//
// Available DXL model on this example : All models using Protocol 1.0
// This example is tested with a DXL MX-28, and an USB2DYNAMIXEL
// Be sure that DXL MX properties are already set as %% ID : 1 / Baudnum : 1 (Baudrate : 1000000)
//

#ifdef __linux__
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#elif defined(_WIN32) || defined(_WIN64)
#include <conio.h>
#endif

#include <stdlib.h>
#include <stdio.h>
#include <iostream>

#include "dynamixel_sdk.h"                                  // Uses Dynamixel SDK library

// Control table address
#define ADDR_MX_MOTOR_ID			     3
#define ADDR_MX_TORQUE_ENABLE           24                  // Control table address is different in Dynamixel model
#define ADDR_MX_GOAL_POSITION           30
#define ADDR_MX_PRESENT_POSITION        36

// Protocol version
#define PROTOCOL_VERSION                1.0                 // See which protocol version is used in the Dynamixel

// Default setting
#define DXL_ID                          1                   // Dynamixel ID: 1
#define BAUDRATE                        1000000
#define DEVICENAME                      "/dev/ttyUSB0"      // Check which port is being used on your controller
                                                            // ex) Windows: "COM1"   Linux: "/dev/ttyUSB0"

#define TORQUE_ENABLE                   1                   // Value for enabling the torque
#define TORQUE_DISABLE                  0                   // Value for disabling the torque
#define DXL_MINIMUM_POSITION_VALUE      100                 // Dynamixel will rotate between this value
#define DXL_MAXIMUM_POSITION_VALUE      4000                // and this value (note that the Dynamixel would not move when the position value is out of movable range. Check e-manual about the range of the Dynamixel you use.)
#define DXL_MOVING_STATUS_THRESHOLD     10                  // Dynamixel moving status threshold

#define ESC_ASCII_VALUE                 0x1b

using namespace std;

int getch()
{
#ifdef __linux__
  struct termios oldt, newt;
  int ch;
  tcgetattr(STDIN_FILENO, &oldt);
  newt = oldt;
  newt.c_lflag &= ~(ICANON | ECHO);
  tcsetattr(STDIN_FILENO, TCSANOW, &newt);
  ch = getchar();
  tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
  return ch;
#elif defined(_WIN32) || defined(_WIN64)
  return _getch();
#endif
}

int kbhit(void)
{
#ifdef __linux__
  struct termios oldt, newt;
  int ch;
  int oldf;

  tcgetattr(STDIN_FILENO, &oldt);
  newt = oldt;
  newt.c_lflag &= ~(ICANON | ECHO);
  tcsetattr(STDIN_FILENO, TCSANOW, &newt);
  oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
  fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

  ch = getchar();

  tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
  fcntl(STDIN_FILENO, F_SETFL, oldf);

  if (ch != EOF)
  {
    ungetc(ch, stdin);
    return 1;
  }

  return 0;
#elif defined(_WIN32) || defined(_WIN64)
  return _kbhit();
#endif
}

int main()
{
  // Initialize PortHandler instance
  // Set the port path
  // Get methods and members of PortHandlerLinux or PortHandlerWindows
  dynamixel::PortHandler *portHandler = dynamixel::PortHandler::getPortHandler(DEVICENAME);

  // Initialize PacketHandler instance
  // Set the protocol version
  // Get methods and members of Protocol1PacketHandler or Protocol2PacketHandler
  dynamixel::PacketHandler *packetHandler = dynamixel::PacketHandler::getPacketHandler(PROTOCOL_VERSION);

  // Open port
  if (portHandler->openPort())
  {
    printf("Succeeded to open the port!\n");
  }
  else
  {
    printf("Failed to open the port!\n");
    printf("Press any key to terminate...\n");
    getch();
    return 0;
  }

  // Set port baudrate
  if (portHandler->setBaudRate(BAUDRATE))
  {
    printf("Succeeded to change the baudrate!\n");
  }
  else
  {
    printf("Failed to change the baudrate!\n");
    printf("Press any key to terminate...\n");
    getch();
    return 0;
  }

  uint16_t   model_num;
  uint8_t    err;
  vector<int> valid_ids;
  for (int id = 1; id < 25; id++)
  {
    int rtn = packetHandler->ping(portHandler, id, &model_num, &err);
    if (rtn != COMM_SUCCESS )
      {
        // packetHandler->printTxRxResult(rtn);
          cout << "ID " << id << " is not found." << endl;
      }
      else if (err != 0)
      {
        packetHandler->printRxPacketError(err);
      }
    else
    {
    	valid_ids.push_back(id);
      printf("[ID:%03d] ping Succeeded. Dynamixel model number : %d\n", id, model_num);
    }
  }

  if (valid_ids.size() > 0)
  {
	  cout << "There are " << valid_ids.size() << " motors are connected. You can set ID for each of them." << endl;
  }

  vector<int>::iterator it;

  for (it = valid_ids.begin(); it != valid_ids.end(); it++)
  {
	  int newid = 0;
	  cout << "Please input new ID for motor #" << *it << endl;
	  cin >> newid;
	  if (newid < 254 && newid > 0)
	  {
		  cout << "Changing motor ID" << endl;
		  int rtn = packetHandler->write1ByteTxRx(portHandler, *it, ADDR_MX_MOTOR_ID, newid);
		  if (rtn != COMM_SUCCESS)
		  {
			  cout << "Change motor ID failed." << endl;
		  }
		  uint8_t data;
		  uint8_t err;
		  rtn = packetHandler->read1ByteTxRx(portHandler, newid, ADDR_MX_MOTOR_ID, &data, &err);
		  if (rtn != COMM_SUCCESS)
		  {
			  packetHandler->printTxRxResult(rtn);
		  }
		  else if (err != 0)
		  {
			  packetHandler->printRxPacketError(err);
		  }
		  else
		  {
              cout << "New motor ID is " << newid << endl;
		  }
	  }
  }

  // Close port
  portHandler->closePort();

  return 0;
}

