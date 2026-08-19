#pragma once

#include <string>
#include <winsock2.h>

class Client
{
private:
	static inline int nextClientId = 0;
	std::string username;
	SOCKET& userSocket;
	bool registered = false;

public:
	Client(SOCKET& clientSocket) : userSocket(clientSocket), username("Client" + nextClientId++) {};
	Client(SOCKET& clientSocket, std::string username) : userSocket(clientSocket), username(username) {};

	bool isActive() { return this->userSocket != SOCKET_ERROR; } //I think this should work?

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