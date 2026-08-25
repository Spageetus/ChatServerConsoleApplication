#include "MessageParser.h"

server_response MessageParser::parseMessage(Client* srcClient, std::string msg)
{
	server_response resp;
	resp.srcClient = srcClient;
	resp.dstClient = nullptr;
	if (msg[0] == MessageParser::commandCharacter) //handle commands
	{
		//split the message by ' '
		//use the first word to identify the command used
		//store the rest of the input in a string vector and pass it into the command's run function
	}
	else
	{
		//format the message to contain the client's username
		msg = "<" + srcClient->getUsername() + ">: " + msg;
	}
	resp.message = msg;
	resp.status = StatusCode::SUCCESS;

	return resp;
}