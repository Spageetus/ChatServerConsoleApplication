#include "Client.h"
//Client::Client(SOCKET& clientSocket)
//{
//	this->userSocket = clientSocket;
//}

// sentinel definition (one translation unit)
const Client* Client::InvalidClient = new Client(Client::invalidSocket, "<invalid>");
const Client* Client::AllClients = new Client(Client::invalidSocket, "<allClients>");

Client::Client(SOCKET sock)
{
	this->userSocket = sock;
	this->username = "Client" + std::to_string((int)sock);
	Client::nextClientId++;
}


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

void Client::registerClient(std::string username)
{
	this->username = username;
	this->registered = true;
}

void Client::logout()
{
	this->username = "Client" + std::to_string((int)this->userSocket);
	this->registered = true;
}