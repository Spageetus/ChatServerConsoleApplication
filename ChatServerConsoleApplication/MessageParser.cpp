#include "MessageParser.h"
#include <sstream>
#include <vector>
#include "Command.h"

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

server_response MessageParser::parseMessage(Client* srcClient, std::string msg)
{
	server_response resp;
	resp.srcClient = srcClient;
	resp.dstClient = nullptr;
	if (msg[0] == MessageParser::commandCharacter) //handle commands
	{
		//remove the command character
		msg.erase(msg.begin());
		//split the message by ' '
		std::vector<std::string> splitMessage = split(msg, ' ');
		//use the first word to identify the command used
		Command c = Commands::getCommand(splitMessage[0]);

		//store the rest of the input in a string vector and pass it into the command's run function
		splitMessage.erase(splitMessage.begin());
		server_response command_result = c.run(srcClient, splitMessage);
		command_result.srcClient = resp.srcClient;
		command_result.dstClient = resp.srcClient;
		return command_result;
		
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








