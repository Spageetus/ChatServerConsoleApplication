#pragma once
#include "Client.h"
#include <string>

enum class StatusCode
{
	FAILURE = -1, //only use for generic failures that dont yet have a unique error code
	SUCCESS = 0,
	SHUTDOWN = 1,
	DISCONNECT = 2,
	BIND_ERROR = 3,
	CONNECT_ERROR = 4,
	SETUP_ERROR = 5,
	STARTUP_ERROR = 6,
	ADDRESS_ERROR = 7,
	PARAMETER_ERROR = 8,
	MESSAGE_ERROR = 9,
	NOT_IMPLEMENTED = 10
};

struct server_response //used by message parser and command handler to tell the server what to do with a message
{
	StatusCode status;
	Client* srcClient;
	Client* dstClient;
	std::string message;
};