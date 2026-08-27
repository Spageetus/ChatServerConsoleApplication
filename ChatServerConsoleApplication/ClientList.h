#pragma once
#include <string>
#include <winsock2.h>
#include "Client.h"
#include <unordered_map>
class ClientList
{
private:
	std::unordered_map<std::string, Client*> usernameMap;
	std::unordered_map<SOCKET, Client*> socketMap;

public:
	//ClientList();

	int add(Client* newClient);
	void remove(Client* clientToRemove);
	bool inList(Client* clientToFind);
	size_t size();

	Client* getClient(SOCKET clientSocket);
	Client* getClient(std::string clientUsername);

	std::unordered_map<std::string, Client*>& getUsernameMap(){ return this->usernameMap; }
	std::unordered_map<SOCKET, Client*>& getSocketMap() { return this->socketMap; }

	fd_set getReadyReadSockets();
	fd_set getReadyWriteSockets();

	void shutdownAll();

	void freeMemory();


};

