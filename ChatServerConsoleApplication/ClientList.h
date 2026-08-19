#pragma once
#include <vector>
#include <string>
#include <winsock2.h>
#include "Client.h"
class ClientList
{
private:
	fd_set masterList;
	std::vector<Client> clientVector;

public:
	ClientList();

	/// <summary>
	/// Attempts to add a new client to the list
	/// </summary>
	/// <param name="newClient"></param>
	/// <returns> 
	///		success: 0 | fail: -1
	/// </returns>
	int add(Client newClient);
	void remove(Client clientToRemove);
	bool inList(Client clientToFind);
	int length() { return clientVector.size(); }
	void clear();

	Client getClient(SOCKET clientSocket);
	Client getClient(std::string clientUsername);


	//TODO: find a way to be able to retrieve an fd_set containing ready sockets


};

