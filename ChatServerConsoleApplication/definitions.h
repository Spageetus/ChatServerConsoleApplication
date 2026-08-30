#pragma once
#include "Client.h"
#include <string>

#define ALL_CLIENTS nullptr

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
	USER_NOT_FOUND = 10,
	NOT_IMPLEMENTED
};

struct server_response //used by message parser and command handler to tell the server what to do with a message
{
	StatusCode status;
	Client* srcClient;
	Client* dstClient;
	std::string message;
};

struct message_info
{
	StatusCode status; //lowkey not sure if I even need this
	Client* srcClient; //pointer to the client who originally sent the message
	Client* dstClient; //pointer to the client who will receive the message

	std::string header; // will contain a formatted header that will be sent before the rest of the message
	std::string ogMsg; //the original message
	std::string srcClientMsg; //message that will be sent back to the original client
	std::string dstClientMsg; //message that will be sent to the recipient of the message

};