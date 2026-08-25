#include "ClientHandler.h"

ClientList ClientHandler::getAllClients()
{
	return ClientHandler::allClients;
}

ClientList ClientHandler::getRegisteredClients()
{
	return ClientHandler::registeredClients;
}

ClientList ClientHandler::getUnRegisteredClients()
{
	return ClientHandler::unregisteredClients;
}

void ClientHandler::addClient(Client* clientToAdd)
{
	ClientHandler::allClients.add(clientToAdd);
	if (clientToAdd->isRegistered())
	{
		ClientHandler::registeredClients.add(clientToAdd);
	}
	else
	{
		ClientHandler::unregisteredClients.add(clientToAdd);
	}
}

void ClientHandler::removeClient(Client* clientToRemove)
{
	ClientHandler::allClients.remove(clientToRemove);
	ClientHandler::registeredClients.remove(clientToRemove);
	ClientHandler::unregisteredClients.remove(clientToRemove);
	clientToRemove->shutdownClient();
}

void ClientHandler::addClient(SOCKET clientSocket)
{
	ClientHandler::addClient(new Client(clientSocket));
}

///Theres a chance this gives me an error BUT IT HASNT SO FAR
void ClientHandler::shutdownAllClients()
{
	ClientHandler::allClients.shutdownAll();
	ClientHandler::registeredClients.shutdownAll();
	ClientHandler::unregisteredClients.shutdownAll();
}