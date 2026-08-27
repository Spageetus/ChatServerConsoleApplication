#pragma once

#include <string>
#include <winsock2.h>


class Client
{
private:
	static inline int nextClientId = 0;
	std::string username;
	SOCKET userSocket;
	bool registered = false;

	

public:
	Client(SOCKET clientSocket);
	Client(SOCKET clientSocket, std::string username) : userSocket(clientSocket), username(username) {};
	
	static const SOCKET invalidSocket = INVALID_SOCKET;
	static const Client* InvalidClient;
	static const Client* AllClients; //used as a way to tell the server to send a message to all clients
	
	bool isActive() { return this->userSocket != SOCKET_ERROR; } //I think this should work?
	bool isRegistered() { return this->registered; };

	void registerClient(std::string username);
	void logout();

	SOCKET getSocket();
	std::string getUsername();

	void shutdownClient();

	bool operator==(const Client other) const
	{
		return (this->username == other.username && this->userSocket == other.userSocket);
	}

	void operator=(const Client& other)
	{
		this->username = other.username;
		this->userSocket = other.userSocket;
	}
};