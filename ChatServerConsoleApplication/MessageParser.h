#pragma once
#include "Client.h"
#include "definitions.h"
class MessageParser
{
private:
	static void formatMessage(message_info& resp);

public:
	inline static char commandCharacter = '/';
	//static server_response parseMessage(Client* srcClient, std::string rawMessage);

	/// <summary>
	/// Modifies the data stored in info
	/// </summary>
	/// <param name="info"></param>
	/// <returns></returns>
	static void parseMessage(message_info& info);

	static void formatMessage(message_info& info);
};


