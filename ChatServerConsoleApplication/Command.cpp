#include "Command.h"
#include "ClientHandler.h"
#include "Server.h"

Command::Command(std::string commandName, int numParameters) : commandString(commandName), expectedArguments(numParameters), runFunc(Command::tempFunction)
{
	//TODO: give each command a better help string
	this->helpString = "help: " + this->commandString;
	this->detailedHelpString = "detailed help: " + this->commandString;
	Command::commandsMap.insert({ commandName, this });
}

Command::Command(std::string commandName, int numParameters, std::function<void(message_info& msg, std::vector<std::string>)> func) : Command(commandName, numParameters)
{
	this->runFunc = func;
}

Command::Command(std::string commandName, int numParameters, std::function<void(message_info& msg, std::vector<std::string>)> func, std::string helpString) : Command(commandName, numParameters, func)
{
	this->helpString = helpString;
	this->detailedHelpString = helpString;
}

Command::Command(std::string commandName, int numParameters, std::function<void(message_info& msg, std::vector<std::string>)> func, std::string helpString, std::string detailedHelpString) : Command(commandName, numParameters, func, helpString)
{
	this->detailedHelpString = detailedHelpString;
}


//the code that runs BEFORE the command specific code
void Command::run(message_info& msg, std::vector<std::string> args)
{
	//if the current function is the help function and has no parameters, add "ALL" as the parameter
	if (*this == Commands::help && args.size() == 0)
	{
		args.push_back("ALL");
	}

	//if an invalid number of arguments are given, send an error message back
	if (args.size() != this->expectedArguments)
	{
		msg.status = StatusCode::PARAMETER_ERROR;
		msg.dstClient = msg.srcClient;
		msg.srcClient = Server::ServerClient;
		msg.dstClientMsg = "PARAMETER ERROR: expected " + std::to_string(this->expectedArguments) + " arguments, but " + std::to_string(args.size()) + " were received";
		return;
	}

	this->runFunc(msg, args);
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
	extern Command registerUser = Command("register", 2, [](message_info& msg, std::vector<std::string> args)
		{
			Client* currentClient = msg.srcClient;
			//this command just needs to send a response back to the current user
			msg.dstClient = msg.srcClient;
			msg.srcClient = Server::ServerClient;

			std::string username = args[0];
			std::string password = args[1];

			if (currentClient->isRegistered())
			{
				msg.status = StatusCode::FAILURE;
				msg.dstClientMsg = "Please logout before creating a new account";
				return;
			}

			//check for an invalid username
			if (!Credentials::validateUsername(username))
			{
				msg.status = StatusCode::PARAMETER_ERROR;
				if (Credentials::usernameTaken(username)) msg.dstClientMsg = "That username is already taken. Please choose a new username.";
				else msg.dstClientMsg = "INVALID USERNAME. Please ensure usernames do not start with numbers or special characters";
				return;
			}
			//check for an invalid password
			if (!Credentials::validatePassword(password))
			{
				msg.status = StatusCode::PARAMETER_ERROR;
				msg.dstClientMsg = "INVALID PASSWORD";
				return;
			}
			//attempt to actually create the new login
			bool created = Credentials::addNew(username, password);
			if (created == true)
			{
				msg.status == StatusCode::SUCCESS;
				msg.dstClientMsg = "Successfully created your new account!";
			}
			else
			{
				msg.status == StatusCode::FAILURE;
				msg.dstClientMsg = "Unable to create your account";
			}

		}, 
		R"(register <username> <password>: attempts to create a new account)",
		R"(register <username> <password>: attempts to create a new account
-username:
	- start with letters
	- may contain '_'
	- max len: 24
-password:
	- no spaces
	- length: 4-64 chars)");
	//create login command
	extern Command login = Command("login", 2, [](message_info& msg, std::vector<std::string> args)
		{
			Client* currentClient = msg.srcClient;

			msg.dstClient = msg.srcClient;
			msg.srcClient = Server::ServerClient;

			std::string username = args[0];
			std::string password = args[1];

			if (currentClient->isRegistered())
			{
				msg.status = StatusCode::FAILURE;
				msg.dstClientMsg = "Please logout before attempting to log in again";
				return;
			}
			
			if (ClientHandler::getAllClients().getClient(username) != nullptr)
			{
				msg.status = StatusCode::FAILURE;
				msg.dstClientMsg = "A user with that name is already logged in.";
				return;
			}

			if (Credentials::verifyLogin(username, password))
			{
				//register the new client
				msg.status = StatusCode::SUCCESS;
				msg.dstClientMsg = "Welcome " + username + "!";
				ClientHandler::registerClient(currentClient, username);
			}
			else
			{
				msg.status = StatusCode::PARAMETER_ERROR;
				msg.dstClientMsg = "Invalid username or password";
				return;
			}
			return;
		},
		R"(login <username> <password>)");
	//creating logout command
	extern Command logout = Command("logout", 0, [](message_info& msg, std::vector<std::string> args)
		{
			Client* currentClient = msg.srcClient;
			msg.dstClient = currentClient;
			msg.srcClient = Server::ServerClient;

			if (!currentClient->isRegistered())
			{
				msg.dstClientMsg = "User is already logged out";
				msg.status = StatusCode::SUCCESS;
			}
			else
			{
				ClientHandler::unregisterClient(currentClient);
				msg.dstClientMsg = "Successfully logged out!";
				msg.status = StatusCode::SUCCESS;
			}
		},
		"logout: self explanatory");

	extern Command status = Command("status", 0, [](message_info& msg, std::vector<std::string> args)
		{
			Client* currentClient = msg.srcClient;
			msg.dstClient = currentClient;
			msg.srcClient = Server::ServerClient;


			msg.dstClientMsg = "\nUsername: " + currentClient->getUsername()
				+ "\nSocket: " + std::to_string((int)currentClient->getSocket())
				+ "\nRegistered?: " + (currentClient->isRegistered() ? "true" : "false");
			msg.status == StatusCode::SUCCESS;
		},
		R"(status: displays information about the current user)",
		R"(status: displays information about the current user
- Username: the user's username (uses a generic "Client" name when not logged in)
- Socket: displays the socket the user is connected to
- Registered: displays if the user is currently logged in or not)");

	
	extern Command shutdown = Command("shutdown", 0, [](message_info& msg, std::vector<std::string> args)
		{
			Client* currentClient = msg.srcClient;

			if (currentClient->getUsername() == "admin")
			{
				msg.dstClient = ALL_CLIENTS;
				msg.srcClient = Server::ServerClient;
				msg.status = StatusCode::SHUTDOWN;
				msg.dstClientMsg = "SHUTDOWN COMMAND RUN BY ADMIN ACCOUNT";
				return;
			}
			else
			{
				Commands::invalidCommand.run(msg, {});
			}
		});

	extern Command help = Command("help", 1, [](message_info& msg, std::vector<std::string> args)
		{
			Client* currentClient = msg.srcClient;
			msg.dstClient = currentClient;
			msg.srcClient = Server::ServerClient;

			std::string commandHelpString = "\n";

			//no argument given
			if (args[0] == "ALL")
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
					msg.status = StatusCode::PARAMETER_ERROR;
				}
				else commandHelpString += c.getDetailedHelpString();
			}
			msg.dstClientMsg = commandHelpString;
		},
		R"(help <command?>: displays available commands)",
		R"(help <command?>:
- command (optional):
	- displays more detailed information about a specific command)");

	extern Command users = Command("users", 0, [](message_info& msg, std::vector<std::string> args)
		{
			Client* currentClient = msg.srcClient;
			msg.dstClient = currentClient;
			msg.srcClient = Server::ServerClient;

			msg.dstClientMsg = "Online users:\n";
			for (const auto& [username, client] : ClientHandler::getRegisteredClients().getUsernameMap())
			{
				msg.dstClientMsg += username + "\n";
			}
		},
		R"(users: displays all active users)");

	//currently might work if unregistered users use it. also need to do some shit with the args so it counts the entire message as ONE parameter
	extern Command msg = Command("msg", 2, [](message_info& msg, std::vector<std::string> args)
		{
			Client* currentClient = msg.srcClient;

			if (!currentClient->isRegistered())
			{
				msg.dstClient = currentClient;
				msg.srcClient = Server::ServerClient;
				msg.dstClientMsg = "You need to be logged in to use this command";
				return;
			}

			std::string username = args[0];
			Client* dstClient = ClientHandler::getRegisteredClients().getClient(username);
			if (dstClient == nullptr)
			{
				msg.dstClient = currentClient;
				msg.srcClient = Server::ServerClient;
				msg.dstClientMsg = "User not found";
				return;
			}

			msg.dstClient = dstClient;
			msg.dstClientMsg = msg.ogMsg;
		}, "msg: <username> <message>", "msg: <username> <message>\n\t-username: the name of the user you want to send a private message to\n\t-message: the message you want to send");
}

