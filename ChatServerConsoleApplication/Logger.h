#pragma once
#include <string>
#include <vector>
#include "definitions.h"
class Logger
{
private:
	inline static bool initialized = false;
	inline static int MAX_MESSAGES = 0;
	inline static int commandCharacter;
	inline static std::vector<std::string> messageLog;
	inline static std::vector<std::string> commandLog;
public:
	static void init(char commandCharacter, int maxLogs);

	static void logCommand(std::string command);
	static void logMessage(std::string message);

	static void log(const message_info& msg);
	static std::vector<std::string> getMessageLog();
	static std::vector<std::string> getCommandLog();
};

