#include "Server.h"

StatusCode Server::init(uint16_t port, uint16_t maxConnections)
{
	this->listenPort = port;
	int result = WSAStartup(WINSOCK_VERSION, &wsadata);
	if (result != 0) return StatusCode::STARTUP_ERROR;

	this->listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (this->listenSocket == INVALID_SOCKET) return StatusCode::SETUP_ERROR;

	sockaddr_in serverAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = INADDR_ANY;
	serverAddr.sin_port = htons(this->listenPort);
	result = bind(this->listenSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr));
	if (result == SOCKET_ERROR) return StatusCode::BIND_ERROR;

	result = listen(this->listenSocket, maxConnections);
	if (result == SOCKET_ERROR) return StatusCode::SETUP_ERROR;
	
	//marking server as active
	this->active = true;
	return StatusCode::SUCCESS;
}

StatusCode Server::acceptIncomingConnections()
{
	this->clientSocket = accept(this->listenSocket, NULL, NULL);
	if (clientSocket == INVALID_SOCKET)
	{
		int errorCode = WSAGetLastError();
		if (errorCode == WSAESHUTDOWN) return StatusCode::SHUTDOWN;
		return StatusCode::CONNECT_ERROR;
	}
	return StatusCode::SUCCESS;
}

StatusCode Server::readMessage(char* inputBuffer)
{
	//first byte is the size of the incoming message
	uint8_t msgSize = 0;
	int result = receiveTcpData(this->clientSocket, (char*)&msgSize, 1);
	if (result == 0) return StatusCode::DISCONNECT;
	if (result == SOCKET_ERROR) return StatusCode::SHUTDOWN;

	//using the expected message length, take in the rest of the message.
	result = receiveTcpData(this->clientSocket, inputBuffer, msgSize);

	//Error handling
	if (result == 0) return StatusCode::DISCONNECT;
	if (result == SOCKET_ERROR) return StatusCode::SHUTDOWN;
	if (result > msgSize) return StatusCode::PARAMETER_ERROR; //TODO: determine if comparing to size or msgSize is correct
	return StatusCode::SUCCESS;
}

StatusCode Server::sendMessage(char* data, int32_t length)
{
	if (length < 0 || length > 255) return StatusCode::PARAMETER_ERROR;

	//first send the length of the message
	int result = sendTcpData(clientSocket, (char*)&length, 1);
	//check for errors
	if (result == 0) return StatusCode::DISCONNECT;
	if (result == SOCKET_ERROR) return StatusCode::SHUTDOWN;

	//then send the rest of the message
	result = sendTcpData(clientSocket, data, length);
	if (result == 0) return StatusCode::DISCONNECT;
	if (result == SOCKET_ERROR) return StatusCode::SHUTDOWN;
	return StatusCode::SUCCESS;
}



void Server::stop()
{
	this->active = false;

	shutdown(this->listenSocket, SD_BOTH);
	closesocket(this->listenSocket);

	//TODO: make sure to close ALL client sockets (once implemented)
	shutdown(clientSocket, SD_BOTH);
	closesocket(clientSocket);
	WSACleanup();
}

