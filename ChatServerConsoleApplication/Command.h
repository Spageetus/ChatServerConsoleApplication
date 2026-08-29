#pragma once
#include <string>
#include <vector>
#include <functional>

#include "definitions.h"
#include "Credentials.h"


class Command
{
private:
	int expectedArguments;
	std::string commandString;
	std::string helpString;
	std::string detailedHelpString;
	std::function<server_response(Client*, std::vector<std::string> args)> runFunc; //lambda function 
	

	static inline std::function<server_response(Client* client, std::vector<std::string> args)> tempFunction = [](Client* client, std::vector<std::string> args)
		{
			server_response defaultResponse;
			defaultResponse.status = StatusCode::FAILURE;
			defaultResponse.message = "INVALID COMMAND";
			return defaultResponse;
		};

	

public:
	Command(std::string commandName, int numParameters);
	Command(std::string commandName, int numParameters, std::function<server_response(Client* client, std::vector<std::string>)> func);
	Command(std::string commandName, int numParameters, std::function<server_response(Client* client, std::vector<std::string>)> func, std::string helpString);
	Command(std::string commandName, int numParameters, std::function<server_response(Client* client, std::vector<std::string>)> func, std::string helpString, std::string detailedHelpString);
	server_response run(Client* client, std::vector<std::string> args);
	
	std::string getCommandString() { return this->commandString; };
	std::string getHelpString() { return this->helpString; };
	std::string getDetailedHelpString() { return this->detailedHelpString; };
	static inline std::unordered_map<std::string, Command*> commandsMap;
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
}