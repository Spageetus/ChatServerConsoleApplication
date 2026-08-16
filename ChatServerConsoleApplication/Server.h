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
	SOCKET clientSocket;
	uint16_t listenPort;
	bool active = false;

public:
	static const int32_t MAX_MESSAGE_LENGTH = 256;

	bool isActive() { return this->active; }

	StatusCode init(uint16_t port, uint16_t maxConnections);
	StatusCode acceptIncomingConnections();
	StatusCode readMessage(char* inputBuffer);
	StatusCode sendMessage(char* msg, int32_t length);
	void stop();

};