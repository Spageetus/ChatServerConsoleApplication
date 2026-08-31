#pragma once
#include <string>
#include <vector>
#include <functional>

#include "definitions.h"
#include "Credentials.h"
#include "Server.h"


class Command
{
private:
	int expectedArguments;
	std::string commandString;
	std::string helpString;
	std::string detailedHelpString;
	std::function<void(message_info& msg, std::vector<std::string> args)> runFunc; //lambda function 
	

	static inline std::function<void(message_info& msg, std::vector<std::string> args)> tempFunction = [](message_info& msg, std::vector<std::string> args)
		{
			msg.status = StatusCode::PARAMETER_ERROR;
			msg.dstClient = msg.srcClient;
			msg.srcClient = Server::ServerClient;
			msg.dstClientMsg = "INVALID COMMAND";
		};

	

public:
	Command(std::string commandName, int numParameters);
	Command(std::string commandName, int numParameters, std::function<void(message_info& msg, std::vector<std::string>)> func);
	Command(std::string commandName, int numParameters, std::function<void(message_info& msg, std::vector<std::string>)> func, std::string helpString);
	Command(std::string commandName, int numParameters, std::function<void(message_info& msg, std::vector<std::string>)> func, std::string helpString, std::string detailedHelpString);
	void run(message_info& msg, std::vector<std::string> args);
	
	std::string getCommandString() { return this->commandString; };
	std::string getHelpString() { return this->helpString; };
	std::string getDetailedHelpString() { return this->detailedHelpString; };
	static inline std::unordered_map<std::string, Command*> commandsMap;


	bool operator==(const Command& other) const
	{
		return this->commandString == other.commandString;
	}
};

namespace Commands
{
	extern Command getCommand(std::string commandName);

	extern Command invalidCommand;
	extern Command shutdown;
	extern Command registerUser;
	extern Command login;
	extern Command logout;
	extern Command status; 
	extern Command help;
	extern Command users;
	extern Command msg;
	extern Command getlog;
	extern Command getcommandlog;

}