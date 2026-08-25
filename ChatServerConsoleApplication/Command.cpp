#include "Command.h"

Command::Command(std::string commandName, int numParameters) : commandString(commandName), expectedArguments(numParameters), runFunc(Command::tempFunction)
{
	//TODO: give each command a better help string
	this->helpString = "help: " + this->commandString;
	this->detailedHelpString = this->helpString;

}

Command::Command(std::string commandName, int numParameters, std::function<server_response(std::vector<std::string>)> func) : Command(commandName, numParameters)
{
	this->runFunc = func;
}

server_response Command::run(std::vector<std::string> args)
{
	//make sure the number of arguments matches the expected number
	server_response resp;
	if (args.size() != this->expectedArguments)
	{
		resp.status = StatusCode::PARAMETER_ERROR;
		resp.message = "expected " + std::to_string(this->expectedArguments) + " arguments, but recieved " + std::to_string(args.size());
		return resp;
	}

	//run the attached function
	resp = this->runFunc(args);
	return resp;

}

namespace Commands
{
	namespace
	{
		std::unordered_map<std::string, Command> commandsMap;
	}
	//creating register command
	extern Command registerUser = Command("register", 2, [](std::vector<std::string> args)
		{
			server_response resp;

			std::string username = args[0];
			std::string password = args[1];
			if (Credentials::addNew(username, password))
			{
				resp.status = StatusCode::SUCCESS;
				resp.message = "Successfully created a new account!";
			}
			else
			{
				resp.status = StatusCode::FAILURE;
				resp.message = "Unable to register your account";
			}
			return resp;
		});
	//create login command
	extern Command login = Command("login", 2, [](std::vector<std::string> args)
		{
			server_response resp;
			std::string username = args[0];
			std::string password = args[1];
			if (Credentials::verifyLogin(username, password))
			{
				resp.status = StatusCode::SUCCESS;
				resp.message = "Successfully logged in!";
			}
			else
			{
				resp.status = StatusCode::FAILURE;
				resp.message = "Failed to login";
			}
			return resp;
		});
}