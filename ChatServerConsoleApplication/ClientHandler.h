#pragma once
#include <vector>
#include "definitions.h"
#include "ClientList.h"

class ClientHandler
{
private:
	inline static ClientList allClients;
	inline static ClientList unregisteredClients;
	inline static ClientList registeredClients;
	
public:
	



	static ClientList& getAllClients();
	static ClientList& getRegisteredClients();
	static ClientList& getUnRegisteredClients();

	static void removeClient(Client* clientToRemove);

	static Client* addClient(Client* clientToAdd);
	static Client* addClient(SOCKET clientSocket);

	static void registerClient(Client* client, std::string username);
	static void unregisterClient(Client* client);

	static void shutdownAllClients();

	static int numClients() { return ClientHandler::getAllClients().size(); }
};