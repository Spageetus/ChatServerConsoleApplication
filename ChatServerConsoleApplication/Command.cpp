#include "Command.h"
#include "ClientHandler.h"

Command::Command(std::string commandName, int numParameters) : commandString(commandName), expectedArguments(numParameters), runFunc(Command::tempFunction)
{
	//TODO: give each command a better help string
	this->helpString = "help: " + this->commandString;
	this->detailedHelpString = "detailed help: " + this->commandString;
	Command::commandsMap.insert({ commandName, this });
}

Command::Command(std::string commandName, int numParameters, std::function<server_response(Client* client, std::vector<std::string>)> func) : Command(commandName, numParameters)
{
	this->runFunc = func;
}

Command::Command(std::string commandName, int numParameters, std::function<server_response(Client* client, std::vector<std::string>)> func, std::string helpString) : Command(commandName, numParameters, func)
{
	this->helpString = helpString;
	this->detailedHelpString = helpString;
}

Command::Command(std::string commandName, int numParameters, std::function<server_response(Client* client, std::vector<std::string>)> func, std::string helpString, std::string detailedHelpString) : Command(commandName, numParameters, func, helpString)
{
	this->detailedHelpString = detailedHelpString;
}



server_response Command::run(Client* client, std::vector<std::string> args)
{
	//make sure the number of arguments matches the expected number
	server_response resp;
	//allow the help function to take either 0 or 1 argument
	if (this->commandString == "help")
	{
		if (args.size() > 1)
		{
			resp.status = StatusCode::PARAMETER_ERROR;
			resp.message = "expected 0-1 arguments, but recieved " + std::to_string(args.size());
			return resp;
		}
	}
	else if (args.size() != this->expectedArguments)
	{
		resp.status = StatusCode::PARAMETER_ERROR;
		resp.message = "expected " + std::to_string(this->expectedArguments) + " arguments, but recieved " + std::to_string(args.size());
		return resp;
	}

	//run the attached function
	resp = this->runFunc(client, args);
	return resp;

}

namespace Commands
{

	extern Command invalidCommand = Command("INVALID_COMAND", 0);

	Command getCommand(std::string commandName)
	{
		auto iter = Command::commandsMap.find(commandName);
		if (iter != Command::commandsMap.end()) return *iter->second;
		return Commands::invalidCommand;
	}

	//creating register command

	//TODO: allow for different error messages based on what exactly went wrong while registering a user
	extern Command registerUser = Command("register", 2, [](Client* client, std::vector<std::string> args)
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
		}, 
		R"(register <username> <password>: attempts to create a new account)",
		R"(register <username> <password>: attempts to create a new account
-username:
	- only start with letters
	- may contain '_'
	- max len: 24
-password:
	- no spaces
	- length: 4-64 chars)");
	//create login command
	extern  Command login = Command("login", 2, [](Client* client, std::vector<std::string> args)
		{
			server_response resp;
			resp.srcClient = client;
			resp.dstClient = client;

			std::string username = args[0];
			std::string password = args[1];
			if (client->isRegistered())
			{
				resp.status == StatusCode::FAILURE;
				resp.message = "You are already logged in, use the logout command first";
				return resp;
			}
			//check if the user is currently logged in
			if (ClientHandler::getRegisteredClients().getClient(username) != nullptr)
			{
				resp.status == StatusCode::FAILURE;
				resp.message = "A user with that name is already logged in";
				return resp;
			}
			if (Credentials::verifyLogin(username, password))
			{
				resp.status = StatusCode::SUCCESS;
				resp.message = "Successfully logged in!";
				ClientHandler::registerClient(client, username);
				return resp;
			}
			else
			{
				resp.status = StatusCode::FAILURE;
				resp.message = "Invalid username or password";
				
				return resp;
			}
			return resp;
		},
		R"(login <username> <password>)");
	//creating logout command
	extern Command logout = Command("logout", 0, [](Client* client, std::vector<std::string> args)
		{
			server_response resp;
			resp.dstClient = client;
			if (!client->isRegistered())
			{
				resp.message = "User is already logged out";
				resp.status = StatusCode::SUCCESS;
			}
			else
			{
				ClientHandler::unregisterClient(client);
				resp.message = "Successfully logged out!";
				resp.status = StatusCode::SUCCESS;
			}
			return resp;
		},
		"logout: self explanatory");

	extern Command status = Command("status", 0, [](Client* client, std::vector<std::string> args)
		{
			server_response resp;
			resp.dstClient = client;

			resp.message = "\nUsername: " + client->getUsername()
				+ "\nSocket: " + std::to_string((int)client->getSocket())
				+ "\nRegistered?: " + (client->isRegistered() ? "true" : "false");
			resp.status == StatusCode::SUCCESS;
			return resp;
		},
		R"(status: displays information about the current user)",
		R"(status: displays information about the current user
- Username: the user's username (uses a generic "Client" name when not logged in)
- Socket: displays the socket the user is connected to
- Registered: displays if the user is currently logged in or not)");

	
	extern Command shutdown = Command("shutdown", 0, [](Client* client, std::vector<std::string> args)
		{
			server_response resp;
			resp.dstClient = client;
			resp.message = "INVALID COMMAND";
			if (client->getUsername() == "admin")
			{
				resp.status = StatusCode::SHUTDOWN;
				resp.message = "SHUTDOWN COMMAND RUN BY ADMIN ACCOUNT";
			}
			return resp;
		});

	extern Command help = Command("help", 0, [](Client* client, std::vector<std::string> args)
		{
			server_response resp;
			std::string commandHelpString = "\n";

			//no argument given
			if (args.size() == 0)
			{
				commandHelpString = "";
				for (auto& [commandName, command] : Command::commandsMap)
				{
					if (commandName == Commands::invalidCommand.getCommandString()) continue;
					if (commandName == Commands::shutdown.getCommandString()) continue;
					commandHelpString += command->getHelpString() + "\n";
				}
			}
			else
			{
				Command c = Commands::getCommand(args[0]);
				if (c.getCommandString() == Commands::invalidCommand.getCommandString())
				{
					commandHelpString = "INVALID COMMAND";
					resp.status = StatusCode::PARAMETER_ERROR;
				}
				else commandHelpString += c.getDetailedHelpString();
			}
			resp.message = commandHelpString;
			return resp;
		},
		R"(help <command?>: displays available commands)",
		R"(help <command?>:
- command (optional):
	- displays more detailed information about a specific command)");

	extern Command users = Command("users", 0, [](Client* client, std::vector<std::string> args)
		{
			server_response resp;
			resp.message = "Online users:\n";
			for (const auto& [username, client] : ClientHandler::getRegisteredClients().getUsernameMap())
			{
				resp.message += username + "\n";
			}
			return resp;
		},
		R"(users: displays all active users)");
}