#pragma once
#include <vector>
#include "definitions.h"
#include "ClientList.h"

static class ClientHandler
{
private:
	inline static ClientList allClients;
	inline static ClientList unregisteredClients;
	inline static ClientList registeredClients;
	
public:
	


	static ClientList getAllClients();
	static ClientList getRegisteredClients();
	static ClientList getUnRegisteredClients();

	static void removeClient(Client clientToRemove);

	static void addClient(Client clientToAdd);
	static void addClient(SOCKET clientSocket);

	static void shutdownAllClients();

	static int numClients() { return ClientHandler::getAllClients().size(); }
};