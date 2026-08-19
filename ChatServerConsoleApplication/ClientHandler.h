#pragma once
#include <vector>
#include "definitions.h"
#include "ClientList.h"

class ClientHandler
{
private:
	static std::vector<Client> allClients;
	static std::vector<Client> unregisteredClients;
	static std::vector<Client> registeredClients;

public:
	
};

// need to make sure I can retrieve an fd_set from each list 