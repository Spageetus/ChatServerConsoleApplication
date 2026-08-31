#include "ClientList.h"

int ClientList::add(Client* newClient) 
{
	if (this->socketMap.contains(newClient->getSocket())) return -1; //map already contains the value
	if (this->usernameMap.contains(newClient->getUsername())) return -1;

	this->socketMap.insert({ newClient->getSocket(), newClient });
	this->usernameMap.insert({ newClient->getUsername(), newClient });
	return 0; //success
}

void ClientList::remove(Client* clientToRemove)
{
	this->socketMap.erase(clientToRemove->getSocket());
	this->usernameMap.erase(clientToRemove->getUsername());
}

bool ClientList::inList(Client* clientToFind) 
{
	return this->socketMap.contains(clientToFind->getSocket()) && this->usernameMap.contains(clientToFind->getUsername());
};

void ClientList::shutdownAll()
{
	for (auto it = this->socketMap.begin(); it != this->socketMap.end(); it++)
	{
		it->second->shutdownClient();
	}
}

Client* ClientList::getClient(SOCKET clientSocket)
{
	auto it = this->socketMap.find(clientSocket);
	if (it == this->socketMap.end()) return nullptr;
	return it->second;
}

Client* ClientList::getClient(std::string clientUsername)
{
	auto it = this->usernameMap.find(clientUsername);
	if (it == this->usernameMap.end()) return nullptr;
	return it->second;
}

fd_set ClientList::getReadyReadSockets()
{
	timeval selectPauseTime;
	selectPauseTime.tv_sec = 1;

	fd_set readSocks;
	FD_ZERO(&readSocks);
	
	for (auto it = this->socketMap.begin(); it != this->socketMap.end(); ++it)
	{
		FD_SET(it->second->getSocket(), &readSocks);
	}

	select(0, &readSocks, NULL, NULL, &selectPauseTime);
	
	return readSocks;
}

fd_set ClientList::getReadyWriteSockets()
{
	timeval selectPauseTime;
	selectPauseTime.tv_sec = 1;

	fd_set writeSocks;
	FD_ZERO(&writeSocks);

	for (auto it = this->socketMap.begin(); it != this->socketMap.end(); ++it)
	{
		FD_SET(it->second->getSocket(), &writeSocks);
	}

	select(0, NULL, &writeSocks, NULL, &selectPauseTime);

	return writeSocks;
}

size_t ClientList::size()
{
	return this->socketMap.size();
}