#include "MessageParser.h"
#include <sstream>
#include <vector>
#include "Command.h"

//if limit is 0, split at every delim instance
std::vector<std::string> split(const std::string& s, char delim, int limit = 0)
{
	std::vector<std::string> result;
	std::stringstream ss(s);
	std::string item;
	while (std::getline(ss, item, delim))
	{
		result.push_back(item);

		if (result.size() == limit)
		{
			std::getline(ss, item);
			result.push_back(item);
		}
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

//splits a string into multiple parts by a delimiter. only splits when the message length would go over the max size
std::vector<std::string> MessageParser::splitBySize(std::string msg, size_t maxSize, char delim)
{
	std::vector<std::string> splitMessage;
	std::string temp = msg;

	while (temp.size() >= maxSize)
	{
		int offset = maxSize; //calculate how much to offset the text by
		size_t index = temp.find_last_of(delim, offset);
		std::string substring = temp.substr(0, index);
		splitMessage.push_back(substring);
		temp.erase(0, index);
	}
	splitMessage.push_back(temp);
	return splitMessage;
}



void MessageParser::parseMessage(message_info& msg)
{
	//if message starts with command character, run the command
	if (msg.ogMsg[0] == MessageParser::commandCharacter)
	{
		//extract the name of the command from the text
		size_t firstSpace = msg.ogMsg.find_first_of(" ");
		std::string commandString = msg.ogMsg.substr(1, firstSpace - 1);
		std::vector<std::string> args;
		//get the necessary command
		Command c = Commands::getCommand(commandString);
		//if the command is invalid, alter msg so it will send an error message to the original client
		if (c == Commands::invalidCommand)
		{
			msg.dstClient = msg.srcClient;
			msg.srcClient = Server::ServerClient;
			msg.dstClientMsg = "INVALID COMMAND";
		}
		if (c == Commands::msg) args = split(msg.ogMsg, ' ', 2); //the message command is split only twice
		else args = split(msg.ogMsg, ' '); //split the message on every space
		args.erase(args.begin()); //remove the command name from the arguments call

		c.run(msg, args);


	}
	else if(msg.srcClient->isRegistered() == false) //if the user is not registered, tell them to login first
	{
		msg.dstClient = msg.srcClient;
		msg.srcClient = Server::ServerClient;
		msg.dstClientMsg = "Other users cannot see your messages until you are logged in";
	}
	msg.dstClientMsg = msg.ogMsg;
	MessageParser::formatMessage(msg);
	return;
}

void MessageParser::formatMessage(message_info& msg)
{
	if (msg.srcClient == Server::ServerClient) msg.header = msg.srcClient->getUsername(); //should be [SERVER]
	else msg.header = "<" + msg.srcClient->getUsername() + ">";
	//if the message is private, add more to the header
	if (msg.dstClient != ALL_CLIENTS) msg.header += " whispered to you";
	msg.header += ":";

	if (msg.srcClientMsg == "")
	{
		msg.srcClientMsg = msg.ogMsg;
	}
}








