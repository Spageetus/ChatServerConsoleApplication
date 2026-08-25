#pragma once
#include <string>
#include <vector>
#include <functional>

#include "definitions.h"
#include "Credentials.h"


class Command
{
private:
	const int expectedArguments;
	const std::string commandString;
	std::string helpString;
	std::string detailedHelpString;
	std::function<server_response(std::vector<std::string> args)> runFunc; //lambda function 
	

	static inline std::function<server_response(std::vector<std::string> args)> tempFunction = [](std::vector<std::string> args)
		{
			server_response defaultResponse;
			defaultResponse.status = StatusCode::FAILURE;
			return defaultResponse;
		};

public:
	Command(std::string commandName, int numParameters);
	Command(std::string commandName, int numParameters, std::function<server_response(std::vector<std::string>)> func);
	server_response run(std::vector<std::string> args);
	
	std::string getHelpString() { return this->helpString; };
	std::string getDetailedHelpString() { return this->detailedHelpString; };
};

namespace Commands
{
	//std::unordered_map<std::string, Command> allCommands;
	extern Command registerUser;
	extern Command login;
	extern Command help;
}