#pragma once

#include <winsock2.h>
#include <WS2tcpip.h>
#pragma comment(lib, "Ws2_32.lib")
#include <iostream>

#include "definitions.h"
#include "platform.h"
#include "ClientHandler.h"
#include "MessageParser.h"

class Server
{
private:
	WSADATA wsadata;
	SOCKET listenSocket;
	//SOCKET clientSocket;
	uint16_t listenPort;

	fd_set clientSockets;

	fd_set serverSocketContainer;

	bool active = false;

	std::string welcomeMessage;

	std::string hostname;

	char* readBuffer;
	char* writeBuffer;

	timeval serverWaitTime;

	int maxClients; 

	


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
	StatusCode relayMessage(Client* sender, ClientList toReceive, char* msg);

	static Client* ServerClient;

	StatusCode runOnce();
	
	/// <summary>
	/// Checks if the server's listening socket has been written to (client connecting) then adds them to the list of unregistered clients
	/// </summary>
	StatusCode getNewConnections();
	
	StatusCode sendWelcomeMessage(Client c);

	StatusCode listenTo(ClientList clients);
	message_info listenTo(Client* client);


	//StatusCode readFromClient(Client* client);

	StatusCode listenToRegisteredClients();

	StatusCode listenToUnregisteredClients();

	void print(const std::string msg, bool dontLog);
	void print(const char* msg, bool dontLog);

};

/*
* Server Run Sequence:
* 1. Check for new connections (if the Listening Socket has been written to)
*	1a. create a new unregistered Client instance using the accepted connection
*	1b. privately send the client a welcome message
*
* 2. Check for registered users who have sent a message
*	2a. Parse message
*		- If message is command: (TEMP) do nothing
*		- Else: relay the message to all other registered users
*
* 3. Check for UNREGISTERED users who have sent a message
*	3a. Parse Message
*		- Check for command usage
*		- If /login used: verify login
*			- If login success: bind username to the client and mark them as registered
*			- else: display appropriate error message
*		- If /register used: attempt to create a new login
*			- if success: tell user account has been created and to use /login to sign in
*			- else: display appropriate error message
*/