#include "Logger.h"
#include "Server.h"
void Logger::init(char commandCharacter, int maxLogs)
{
	if (Logger::initialized == true) return;
	if (maxLogs <= 0) maxLogs = 64;
	Logger::MAX_MESSAGES = maxLogs;
	Logger::commandCharacter = commandCharacter;
	Logger::initialized = true;
}


void Logger::logCommand(std::string command)
{
	if (Logger::initialized == false) return;
	Logger::commandLog.push_back(command);
	if (Logger::commandLog.size() > Logger::MAX_MESSAGES)
	{
		Logger::commandLog.erase(Logger::commandLog.begin());
	}
}

void Logger::logMessage(std::string message)
{
	if (Logger::initialized == false) return;
	Logger::messageLog.push_back(message);
	if (Logger::messageLog.size() > Logger::MAX_MESSAGES)
	{
		Logger::messageLog.erase(Logger::messageLog.begin());
	}
}

void Logger::log(const message_info& msg)
{
	if (Logger::initialized == false) return;
	std::string logString;
	//if the message is a command, save it to the command log
	if (msg.ogMsg[0] == Logger::commandCharacter)
	{
		if (msg.srcClient != Server::ServerClient) return; //this should prevent any /msg commands from being logged
		if (msg.status != StatusCode::SUCCESS) return; //only log successful commands
		logString = "<" + msg.dstClient->getUsername() + ">: " + msg.ogMsg;
		Logger::logCommand(logString);
	}
	else
	{
		logString = msg.header + " " + msg.ogMsg;
		if (msg.srcClient->isRegistered() == true) Logger::logMessage(logString);
	}
}

std::vector<std::string> Logger::getCommandLog()
{
	return Logger::commandLog;
}

std::vector<std::string> Logger::getMessageLog()
{
	return Logger::messageLog;
}