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

	//create a Client object so messages can be marked as being sent/received to/from the server
	Server::ServerClient = new Client(this->listenSocket, "[SERVER]");

	result = listen(this->listenSocket, maxConnections);
	if (result == SOCKET_ERROR) return StatusCode::SETUP_ERROR;

	this->welcomeMessage = "[Server]: Welcome to the server! Use " + std::string(1, MessageParser::commandCharacter) + "help for a list of commands!";
	this->readBuffer = new char[256];
	this->writeBuffer = new char[256];

	//set the server waiting time to 1 second
	this->serverWaitTime.tv_sec = 1;

	//marking server as active
	this->active = true;
	this->maxClients = maxConnections;
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
	data[length] = '\0';
	length++;

	if (length < 0 || length > 255)
	{
		std::cout << "ATTEMPTING TO SEND A MESSAGE WITH LENGTH " << length << std::endl;
	}

	//fixes the issue of characters from previous messages still being visible.
	//TODO: try to fix the root cause of the issue
	
	

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

StatusCode Server::relayMessage(Client* sender, ClientList toReceive, char* msg)
{
	//get a list of all the clients who are ready to recieve the message (theoretically should be all of them)
	fd_set recipientSockets = toReceive.getReadyWriteSockets();
	FD_CLR(sender->getSocket(), &recipientSockets); //remove the initial sender
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
	result = this->listenTo(ClientHandler::getRegisteredClients());
	if (result != StatusCode::SUCCESS) return result;

	//Check for messages from UNregistered users
	result = this->listenTo(ClientHandler::getUnRegisteredClients());
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
	Client* c = ClientHandler::addClient(newClient);

	//send the client a welcome message!
	//StatusCode result = this->sendMessage(newClient, this->welcomeMessage.data(), this->welcomeMessage.size());
	StatusCode result = this->sendTo(c, this->welcomeMessage);
	if (result != StatusCode::SUCCESS) return result;

	return StatusCode::SUCCESS;
}

StatusCode Server::sendTo(Client* client, std::string message)
{
	std::vector<std::string> splitMessage = MessageParser::splitBySize(message, this->MAX_MESSAGE_LENGTH-4, '\n');
	
	for (std::string s : splitMessage)
	{
		StatusCode status = this->sendMessage(client->getSocket(), s.data(), s.size());
		if (status != StatusCode::SUCCESS) return status;
	}
	return StatusCode::SUCCESS;
}

//
StatusCode Server::listenTo(ClientList clients)
{
	fd_set readySockets = clients.getReadyReadSockets();
	for (int i = 0; i < readySockets.fd_count; i++)
	{
		Client* c = clients.getClient(readySockets.fd_array[i]);
		if (c == nullptr) continue; //if for whatever reason, the client does not exist
		message_info mi = this->listenTo(c);
		if (mi.status == StatusCode::DISCONNECT)
		{
			ClientHandler::removeClient(c);
			continue;
		}
		if (mi.status != StatusCode::SUCCESS) return mi.status;
		//print the original message to the server console
		this->print("<" + mi.srcClient->getUsername() + ">: " + mi.ogMsg);
		//now I need to parse the message
		MessageParser::parseMessage(mi);
		

		this->print(mi.header);
		this->print(mi.dstClientMsg);

		//finally, send the message back
		StatusCode status = this->sendResponse(mi);
		if (status != StatusCode::SUCCESS) return status;
	}
	return StatusCode::SUCCESS;
}


StatusCode Server::sendResponse(message_info& msg)
{
	if (msg.dstClient == ALL_CLIENTS)
	{
		this->relayMessage(msg.srcClient, ClientHandler::getRegisteredClients(), msg.header.data());
		this->relayMessage(msg.srcClient, ClientHandler::getRegisteredClients(), msg.dstClientMsg.data());
	}
	else
	{
		this->sendTo(msg.dstClient, msg.header.data());
		this->sendTo(msg.dstClient, msg.dstClientMsg.data());
	}

	return StatusCode::SUCCESS;
}

message_info Server::listenTo(Client* client)
{
	message_info mi;
	mi.srcClient = client;
	mi.status = this->readMessage(client->getSocket(), this->readBuffer);
	mi.ogMsg = std::string(this->readBuffer);
	if (mi.status == StatusCode::DISCONNECT || (mi.status != StatusCode::SUCCESS && WSAGetLastError() == WSAECONNRESET)) //
	{
		mi.dstClient = Server::ServerClient;
		this->print(client->getUsername() + " has disconnected");
		mi.status = StatusCode::DISCONNECT;
		return mi;
	}
	if (mi.status == StatusCode::PARAMETER_ERROR)
	{
		std::cout << "RECEIVED TOO MANY BYTES IN A SINGLE MESSAGE" << std::endl;
		return mi;
	}
	else if (mi.status != StatusCode::SUCCESS) return mi;
	return mi;
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
	delete Server::ServerClient;
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


//used so I can print using strings or char*
void Server::print(const std::string msg, bool dontLog)
{
	this->print(msg.data(), dontLog);
}

//TODO: also log the message
void Server::print(const char* msg, bool dontLog)
{
	std::cout << msg << std::endl;
}

