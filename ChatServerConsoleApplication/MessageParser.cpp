#include "MessageParser.h"
#include <sstream>
#include <vector>
#include "Command.h"
#include "Server.h"

std::vector<std::string> split(const std::string& s, char delim)
{
	std::vector<std::string> result;
	std::stringstream ss(s);
	std::string item;
	while (std::getline(ss, item, delim))
	{
		result.push_back(item);
	}
	return result;
}

//server_response MessageParser::parseMessage(Client* srcClient, std::string msg)
//{
//	server_response resp;
//	resp.srcClient = srcClient;
//	resp.dstClient = nullptr;
//	if (msg[0] == MessageParser::commandCharacter) //handle commands
//	{
//		//remove the command character
//		msg.erase(msg.begin());
//		//split the message by ' '
//		std::vector<std::string> splitMessage = split(msg, ' ');
//		//use the first word to identify the command used
//		Command c = Commands::getCommand(splitMessage[0]);
//
//		//store the rest of the input in a string vector and pass it into the command's run function
//		splitMessage.erase(splitMessage.begin());
//		resp = c.run(srcClient, splitMessage);
//	}
//	else
//	{
//		resp.status = StatusCode::SUCCESS;
//	}
//	MessageParser::formatMessage(resp);
//	return resp;
//}



void MessageParser::parseMessage(message_info& msg)
{
	//if message starts with command character, run the command
	if (msg.ogMsg[0] == MessageParser::commandCharacter)
	{
		return;
	}

	MessageParser::formatMessage(msg);
	return;


}

void MessageParser::formatMessage(message_info& msg)
{
	if (msg.srcClient == Server::ServerClient) msg.header = msg.srcClient->getUsername(); //should be [SERVER]
	//if the message is private, add more to the header
	if (msg.dstClient != ALL_CLIENTS) msg.header += " whispered to you";
	msg.header += ":";
}








