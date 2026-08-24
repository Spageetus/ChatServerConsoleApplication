#include "MessageParser.h"

fuckifiknow MessageParser::parseMessage(Client source, std::string message)
{
	fuckifiknow result;
	result.sourceClient = source;

	if (message[0] != MessageParser::COMMAND_CHARACTER)
	{
		result.sourceClient = source;
		result.destinationClient = Client::AllClients;
		result.message = message;
		result.result = StatusCode::SUCCESS;
		return result;
	}

	return result;
}