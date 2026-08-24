#pragma once
#include "Client.h"
#include "definitions.h"
class MessageParser
{

public:
	static server_response parseMessage(Client* srcClient, std::string rawMessage);

};


