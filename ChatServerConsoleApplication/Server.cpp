#include "Server.h"

StatusCode Server::init(uint16_t port, uint16_t maxConnections)
{
	if (this->isActive()) return StatusCode::SUCCESS; //makes sure server cannot be initialized more than once

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

	FD_ZERO(&clientSockets);
	FD_SET(this->listenSocket, &clientSockets);

	result = listen(this->listenSocket, maxConnections);
	if (result == SOCKET_ERROR) return StatusCode::SETUP_ERROR;
	
	//marking server as active
	this->active = true;
	return StatusCode::SUCCESS;
}

//StatusCode Server::acceptIncomingConnections()
//{
//	return StatusCode::SHUTDOWN;
//	//this->clientSocket = accept(this->listenSocket, NULL, NULL);
//	//if (clientSocket == INVALID_SOCKET)
//	//{
//	//	int errorCode = WSAGetLastError();
//	//	if (errorCode == WSAESHUTDOWN) return StatusCode::SHUTDOWN;
//	//	return StatusCode::CONNECT_ERROR;
//	//}
//	//return StatusCode::SUCCESS;
//}

StatusCode Server::readMessage(SOCKET clientSock, char* inputBuffer)
{
	//first byte is the size of the incoming message
	uint8_t msgSize = 0;
	int result = receiveTcpData(clientSock, (char*)&msgSize, 1);
	if (result == 0) return StatusCode::DISCONNECT;
	if (result == SOCKET_ERROR) return StatusCode::SHUTDOWN;

	//using the expected message length, take in the rest of the message.
	result = receiveTcpData(clientSock, inputBuffer, msgSize);

	//Error handling
	if (result == 0) return StatusCode::DISCONNECT;
	if (result == SOCKET_ERROR) return StatusCode::SHUTDOWN;
	if (result > msgSize) return StatusCode::PARAMETER_ERROR; //TODO: determine if comparing to size or msgSize is correct
	return StatusCode::SUCCESS;
}

StatusCode Server::sendMessage(SOCKET clientSock, char* data, int32_t length)
{
	if (length < 0 || length > 255) return StatusCode::PARAMETER_ERROR;

	//first send the length of the message
	int result = sendTcpData(clientSock, (char*)&length, 1);
	//check for errors
	if (result == 0) return StatusCode::DISCONNECT;
	if (result == SOCKET_ERROR) return StatusCode::SHUTDOWN;

	//then send the rest of the message
	result = sendTcpData(clientSock, data, length);
	if (result == 0) return StatusCode::DISCONNECT;
	if (result == SOCKET_ERROR) return StatusCode::SHUTDOWN;
	return StatusCode::SUCCESS;
}

StatusCode Server::relayMessage(SOCKET srcSocket, char* msg, int32_t length)
{
	if (length < 0 || length > 255) return StatusCode::PARAMETER_ERROR;
	//get all sockets who are ready to be written to
	fd_set tempSockets = this->clientSockets;
	select(NULL, NULL, &tempSockets, NULL, NULL); //make a list of all sockets who are ready to be written to
	FD_CLR(srcSocket, &tempSockets); //remove the initial sender from the list of ready sockets
	for (int i = 0; i < tempSockets.fd_count; i++)
	{
		SOCKET s = tempSockets.fd_array[i];
		StatusCode result = this->sendMessage(s, msg, length);
		if (result != StatusCode::SUCCESS)
		{
			std::cout << "Message Relay failed with code: " << (int)result << std::endl;
			return result;
		}
	}
	return StatusCode::SUCCESS;
}



//TODO: add error handling, extract parts of this function to other functions
StatusCode Server::run()
{
	timeval selectPauseTime;
	selectPauseTime.tv_sec = 1;


	while (this->isActive())
	{
		//first check for sockets who are ready to read
		fd_set temp_sockets = this->clientSockets; //copying list of sockets so select doesnt modify the master list
		int numReadySockets = select(NULL, &temp_sockets, NULL, NULL, &selectPauseTime);
		if (numReadySockets <= 0) continue;
		//loop through ready sockets
		for (int i = 0; i < temp_sockets.fd_count; i++)
		{
			SOCKET sock = temp_sockets.fd_array[i];
			if (sock == this->listenSocket)
			{
				SOCKET newClient = accept(this->listenSocket, NULL, NULL);
				FD_SET(newClient, &this->clientSockets);
				std::cout << "New Client Connected!" << std::endl;
			}
			else
			{
				char* receivedData = new char[256];
				StatusCode result = this->readMessage(sock, receivedData);
				if (result == StatusCode::DISCONNECT)
				{
					std::cout << "A user has disconnected" << std::endl;
					this->removeClient(sock);
					continue;
				}
				else if (result != StatusCode::SUCCESS)
				{
					//client was forcibly disconnected (I dont think pressing stop on the client is supposed to work like this?)
					int error = WSAGetLastError();
					if (error == WSAECONNRESET)
					{
						std::cout << "A user has been forcefully disconnected (WSAECONNRESET)" << std::endl;
						this->removeClient(sock);
						continue;
					}
					
					std::cout << "Last WSA error code: " << error << std::endl;
					std::cout << "Status Code: " << (int)result << std::endl;
					
					return StatusCode::FAILURE;
				}

				//send a generic message to the client 
				std::cout << "[Recieved]: " << receivedData << std::endl;

				//TODO: parse the message

				//relay the message to all other active clients
				this->relayMessage(sock, receivedData, strlen(receivedData));
			}
		}
	}
	return StatusCode::SUCCESS;
}



void Server::stop()
{
	this->active = false;

	shutdown(this->listenSocket, SD_BOTH);
	closesocket(this->listenSocket);
	FD_CLR(this->listenSocket, &this->clientSockets);

	//TODO: make sure to close ALL client sockets (once implemented)
	for (int i = 0; i < this->clientSockets.fd_count; i++)
	{
		SOCKET sock = this->clientSockets.fd_array[i];
		shutdown(sock, SD_BOTH);
		closesocket(sock);
	}
	WSACleanup();
	std::cout << "Stopped server" << std::endl;
}


std::string Server::getHostName()
{
	char* tempBuffer = new char[256];
	gethostname(tempBuffer, 256);

	std::cout << "result of gethostname: " << tempBuffer << std::endl;
	return "";
}

void Server::removeClient(SOCKET clientSocket)
{
	//remove the client socket from the list of connected sockets
	FD_CLR(clientSocket, &this->clientSockets);
	shutdown(clientSocket, SD_BOTH);
	closesocket(clientSocket);
}