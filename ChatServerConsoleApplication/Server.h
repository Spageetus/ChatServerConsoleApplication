#pragma once

#include <winsock2.h>
#pragma comment(lib, "Ws2_32.lib")
#include <iostream>

#include "definitions.h"
#include "platform.h"

class Server
{
private:
	WSADATA wsadata;
	SOCKET listenSocket;
	//SOCKET clientSocket;
	uint16_t listenPort;

	fd_set clientSockets;
	bool active = false;

	std::string hostname;


public:
	static const int32_t MAX_MESSAGE_LENGTH = 256;

	bool isActive() { return this->active; }
	std::string getHostName();

	StatusCode init(uint16_t port, uint16_t maxConnections);
	StatusCode run();
	void stop();

	StatusCode addClient(SOCKET clientSocket);
	void removeClient(SOCKET clientSocket);
	
	StatusCode readMessage(SOCKET clientSock, char* inputBuffer);
	StatusCode sendMessage(SOCKET clientSock, char* msg, int32_t length);
	StatusCode relayMessage(SOCKET sourceSocket, char* msg, int32_t length);
	
	

};