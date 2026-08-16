#pragma once

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
	MESSAGE_ERROR = 9
};

/*
class StatusCode
{
private:
	static inline int nextId = 0;
	
	
public:
	//StatusCode() : what("unknown error"), id(-1) {};
	StatusCode(std::string errorText) : id(StatusCode::nextId++), what(errorText){};
	//StatusCode(const StatusCode& baseCode, std::string message) : id(baseCode.id), what(message) {};
	const int id;
	const std::string what;

	friend bool operator==(const StatusCode& code1, const StatusCode& code2)
	{
		return code1.id == code2.id;
	}

	StatusCode operator=(const StatusCode& other)
	{
		if (*this != other)
		{
			return other;
		}
		return *this;
	}
};


namespace StatusCodes
{
	const StatusCode& SUCCESS = StatusCode("Success");
	const StatusCode& SHUTDOWN = StatusCode("Shutdown");
	const StatusCode& DISCONNECT = StatusCode("Disconnect");
	const StatusCode& BIND_ERROR = StatusCode("Bind Error");
	const StatusCode& CONNECT_ERROR = StatusCode("Connection Error");
	const StatusCode& SETUP_ERROR = StatusCode("Setup Error");
	const StatusCode& STARTUP_ERROR = StatusCode("Startup Error");
	const StatusCode& ADDRESS_ERROR = StatusCode("Address Error");
	const StatusCode& PARAMETER_ERROR = StatusCode("Parameter Error");
	const StatusCode& MESSAGE_ERROR = StatusCode("Message Error");
	const StatusCode& MAX_CLIENTS_ERROR = StatusCode("Message Error");
}

*/