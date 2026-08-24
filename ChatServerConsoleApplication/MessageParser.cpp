#include "MessageParser.h"

server_response MessageParser::parseMessage(Client* srcClient, std::string msg)
{
	server_response resp;
	resp.srcClient = srcClient;
	resp.dstClient = nullptr;
	if (msg[0] != '/') //command character
	{
		//format the message to contain the client's username
		msg = "<" + srcClient->getUsername() + ">: " + msg;
	}
	resp.message = msg;
	resp.status = StatusCode::SUCCESS;

	return resp;
}