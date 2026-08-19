#include "Client.h"
//Client::Client(SOCKET& clientSocket)
//{
//	this->userSocket = clientSocket;
//}

void Client::shutdownClient()
{
	shutdown(this->userSocket, SD_BOTH);
	closesocket(this->userSocket);
	if (this->userSocket == SOCKET_ERROR)
	{
		this->userSocket = SOCKET_ERROR;
	}
}

std::string Client::getUsername()
{
	return this->username;
}

SOCKET Client::getSocket()
{
	return this->userSocket;
}