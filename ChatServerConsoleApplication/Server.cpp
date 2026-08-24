#include "Server.h"

StatusCode Server::init(uint16_t port, uint16_t maxConnections)
{
	if (this->isActive()) return StatusCode::SUCCESS; //makes sure server cannot be initialized more than once
	
	//Startup WSA
	int result = WSAStartup(WINSOCK_VERSION, &wsadata);
	if (result != 0) return StatusCode::STARTUP_ERROR;
	
	//setup the server's listening port
	this->listenPort = port;
	this->listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (this->listenSocket == INVALID_SOCKET) return StatusCode::SETUP_ERROR;

	//bind the listening socket 
	sockaddr_in serverAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = INADDR_ANY;
	serverAddr.sin_port = htons(this->listenPort);
	result = bind(this->listenSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr));
	if (result == SOCKET_ERROR) return StatusCode::BIND_ERROR;

	//store the server's listening socket separate from the rest of the connected sockets
	FD_ZERO(&this->serverSocketContainer); //clear it just in case
	FD_SET(this->listenSocket, &this->serverSocketContainer);

	result = listen(this->listenSocket, maxConnections);
	if (result == SOCKET_ERROR) return StatusCode::SETUP_ERROR;

	this->welcomeMessage = "[Server]: Welcome to the server!";
	this->readBuffer = new char[256];
	this->writeBuffer = new char[256];

	//set the server waiting time to 1 second
	this->serverWaitTime.tv_sec = 1;

	//marking server as active
	this->active = true;
	return StatusCode::SUCCESS;
}

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
	//inputBuffer[msgSize] = '\0'; //adding an escape character so I dont keep data from the previous read
	return StatusCode::SUCCESS;
}

//TODO: figure out why clients are recieving more data than they should
StatusCode Server::sendMessage(SOCKET clientSock, char* data, int32_t length)
{
	if (length < 0 || length > 255) return StatusCode::PARAMETER_ERROR;

	//fixes the issue of characters from previous messages still being visible.
	//TODO: try to fix the root cause of the issue
	data[length] = '\0';
	length++;

	//first send the length of the message
	int result = sendTcpData(clientSock, (char*)&length, 1);
	//check for errors
	if (result == 0) return StatusCode::DISCONNECT;
	if (result == SOCKET_ERROR) return StatusCode::SHUTDOWN;
	if (result != 1)
	{
		std::cout << "Sent " << result << " bytes" << std::endl;
	}

	//then send the rest of the message
	result = sendTcpData(clientSock, data, length);
	if (result == 0) return StatusCode::DISCONNECT;
	if (result == SOCKET_ERROR) return StatusCode::SHUTDOWN;
	if (result > length)
	{
		std::cout << "Sent " << result << " bytes" << std::endl;
	}
	return StatusCode::SUCCESS;
}

StatusCode Server::relayMessage(Client sender, ClientList toReceive, char* msg)
{
	//get a list of all the clients who are ready to recieve the message (theoretically should be all of them)
	fd_set recipientSockets = toReceive.getReadyWriteSockets();
	FD_CLR(sender.getSocket(), &recipientSockets); //remove the initial sender
	for (int i = 0; i < recipientSockets.fd_count; i++)
	{
		SOCKET s = recipientSockets.fd_array[i];
		StatusCode result = this->sendMessage(s, msg, strlen(msg));
		if (result != StatusCode::SUCCESS) return result;
	}
	return StatusCode::SUCCESS;
}

StatusCode Server::run()
{
	StatusCode status = StatusCode::FAILURE; //just setting it to whatever for now
	do
	{
		status = this->runOnce();
	} while (status != StatusCode::SUCCESS);

	return status;
}

StatusCode Server::runOnce()
{
	StatusCode result;
	if (ClientHandler::numClients() < this->maxClients)
	{
		//Get new connections
		result = this->getNewConnections();
		if (result != StatusCode::SUCCESS) return result;
	}
	
	//Check for messages from registered users
	//result = this->listenToRegisteredClients();
	//if (result != StatusCode::SUCCESS) return result;

	//Check for messages from UNregistered users
	result = this->listenToUnregisteredClients();
	if (result != StatusCode::SUCCESS) return result;

	return StatusCode::SUCCESS;
}

//TODO: limit the number of connections
StatusCode Server::getNewConnections()
{
	timeval selectPauseTime;
	selectPauseTime.tv_sec = 1;


	fd_set tempListenSet = this->serverSocketContainer;
	int numReadySockets = select(NULL, &tempListenSet, NULL, NULL, &selectPauseTime);
	if (numReadySockets <= 0) return StatusCode::SUCCESS; //there are no sockets awaiting connection
	if (numReadySockets > 1)
	{
		std::cout << "too many ready sockets: " << numReadySockets << std::endl; //theoretically, this code should never run
		return StatusCode::FAILURE;
	}
	SOCKET newClient = accept(this->listenSocket, NULL, NULL); //accept the new connection!
	if (newClient == INVALID_SOCKET)
	{
		int error = WSAGetLastError();
		std::cout << "Accept failed with WSA Error Code: " << error << std::endl;
		return StatusCode::FAILURE;
	}

	//add the new client to our client list
	std::cout << "New Client Connected!" << std::endl;
	ClientHandler::addClient(newClient);

	//send the client a welcome message!
	StatusCode result = this->sendMessage(newClient, this->welcomeMessage.data(), this->welcomeMessage.size());
	if (result != StatusCode::SUCCESS) return result;

	return StatusCode::SUCCESS;
}

StatusCode Server::readFrom(ClientList clients)
{
	//get a list of all the unregistered clients who have sent something to the server
	fd_set sockets = clients.getReadyReadSockets();
	if (sockets.fd_count == 0) return StatusCode::SUCCESS;

	for (int i = 0; i < sockets.fd_count; i++)
	{
		//for each ready socket, attempt to read from it
		SOCKET s = sockets.fd_array[i];
		Client currentClient = clients.getClient(s);
		if (currentClient == Client::InvalidClient)
		{
			std::cout << "Client does not exist" << std::endl;
			continue;
		}
		StatusCode result = this->readMessage(s, this->readBuffer);
		if (result == StatusCode::DISCONNECT) //check if socket disconnected
		{
			std::cout << currentClient.getUsername() << " has disconnected" << std::endl;
			ClientHandler::removeClient(currentClient);
			continue;
		}
		else if (result != StatusCode::SUCCESS) //check for any other errors
		{
			//client was forcibly disconnected (I dont think pressing stop on the client is supposed to work like this?)
			int error = WSAGetLastError();
			if (error == WSAECONNRESET)
			{
				std::cout << "A user has been forcefully disconnected (WSAECONNRESET)" << std::endl;
				ClientHandler::removeClient(currentClient);
				continue;
			}

			std::cout << "Last WSA error code: " << error << std::endl;
			std::cout << "Status Code: " << (int)result << std::endl;

			return StatusCode::FAILURE;
		}
		
		if (!currentClient.isRegistered() && false) //TODO: remove the && false 
		{
			std::cout << "[UNREGISTERED USER] ";
		}

		//temp code to close the server
		if (std::string(this->readBuffer) == "shutdown")
		{
			return StatusCode::SHUTDOWN;
		}

		//parse the message
		fuckifiknow todo = MessageParser::parseMessage(currentClient, std::string(this->readBuffer));
		
		std::cout << todo.message;

		if (todo.destinationClient == Client::AllClients)
		{
			this->relayMessage(todo.sourceClient, ClientHandler::getAllClients(), todo.message.data());
		}
	}
	return StatusCode::SUCCESS;
}

StatusCode Server::listenToUnregisteredClients()
{
	return this->readFrom(ClientHandler::getUnRegisteredClients());
}

void Server::stop()
{
	shutdown(this->listenSocket, SD_BOTH);
	closesocket(this->listenSocket);

	ClientHandler::shutdownAllClients();

	WSACleanup();
	std::cout << "Stopped server" << std::endl;
	this->active = false;
	delete[] this->readBuffer;
	delete[] this->writeBuffer;
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