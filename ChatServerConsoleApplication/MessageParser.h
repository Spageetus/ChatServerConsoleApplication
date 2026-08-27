#pragma once
#include "Client.h"
#include "definitions.h"
class MessageParser
{
public:
	inline static char commandCharacter = '/';
	static server_response parseMessage(Client* srcClient, std::string rawMessage);

	
};


