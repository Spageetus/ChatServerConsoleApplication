#pragma once
#include "definitions.h"
#include "Client.h"

static class MessageParser
{
public:
	const static char COMMAND_CHARACTER = '/';

	static fuckifiknow parseMessage(Client sourceClient, std::string message);
};



//struct containing information telling the server what to do with the message
struct fuckifiknow
{
	StatusCode result = StatusCode::FAILURE;
	Client sourceClient = Client::InvalidClient;
	Client destinationClient = Client::InvalidClient;
	std::string message = "";
};

