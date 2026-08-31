#include "ClientHandler.h"

ClientList& ClientHandler::getAllClients()
{
	return ClientHandler::allClients;
}

ClientList& ClientHandler::getRegisteredClients()
{
	return ClientHandler::registeredClients;
}

ClientList& ClientHandler::getUnRegisteredClients()
{
	return ClientHandler::unregisteredClients;
}

Client* ClientHandler::addClient(Client* clientToAdd)
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
	return clientToAdd;
}

void ClientHandler::removeClient(Client* clientToRemove)
{
	ClientHandler::allClients.remove(clientToRemove);
	ClientHandler::registeredClients.remove(clientToRemove);
	ClientHandler::unregisteredClients.remove(clientToRemove);
	clientToRemove->shutdownClient();
}

Client* ClientHandler::addClient(SOCKET clientSocket)
{
	Client* newClient = new Client(clientSocket);
	ClientHandler::addClient(newClient);
	return newClient;
}

///Theres a chance this gives me an error BUT IT HASNT SO FAR
void ClientHandler::shutdownAllClients()
{
	ClientHandler::allClients.shutdownAll();
	ClientHandler::registeredClients.shutdownAll();
	ClientHandler::unregisteredClients.shutdownAll();

	//delete all Clients
	
}

void ClientHandler::registerClient(Client* client, std::string username)
{
	ClientHandler::getUnRegisteredClients().remove(client);
	ClientHandler::getAllClients().remove(client);
	client->registerClient(username);
	ClientHandler::getAllClients().add(client);
	ClientHandler::getRegisteredClients().add(client);
	
}

void ClientHandler::unregisterClient(Client* client)
{
	if (!client->isRegistered()) return;
	ClientHandler::getRegisteredClients().remove(client);
	ClientHandler::getAllClients().remove(client);
	client->logout();
	ClientHandler::getAllClients().add(client);
	ClientHandler::getUnRegisteredClients().add(client);
}