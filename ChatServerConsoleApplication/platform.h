#pragma once
#include <winsock2.h>

#pragma comment(lib, "Ws2_32.lib")
#include "stdint.h"


//Helper function for sending tcp data
int sendTcpData(SOCKET skSocket, const char* data, uint16_t msgLength);

//Helper function for receiving tcp data
int receiveTcpData(SOCKET sockfd, char* readBuffer, uint16_t length);