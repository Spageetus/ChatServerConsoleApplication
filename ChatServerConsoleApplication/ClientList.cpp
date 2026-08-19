#include "ClientList.h"

ClientList::ClientList()
{
	FD_ZERO(&this->masterList); //initializes empty socket list
	this->clientVector.clear();
}


int ClientList::add(Client newClient) 
{
	if (this->inList(newClient))
	{
		return -1;
	}
	this->clientVector.push_back(newClient);
	FD_SET(newClient.getSocket(), &this->masterList);
	return 0;
}

//wait shit I shouldnt shut down the client 
void ClientList::remove(Client clientToRemove) 
{
	if (!this->inList(clientToRemove)) return;
	for (int i = 0; i < this->clientVector.size(); i++)
	{
		if (this->clientVector[i] == clientToRemove)
		{
			this->clientVector.erase(this->clientVector.begin() + i);
			break;
		}
	}
	//clientToRemove.shutdownClient();
}

bool ClientList::inList(Client clientToFind) 
{
	return FD_ISSET(clientToFind.getSocket(), &this->masterList);
};

void ClientList::clear()
{
	for (int i = 0; i < this->clientVector.size(); i++)
	{
		this->remove(this->clientVector[0]);
	}
}

Client ClientList::getClient(SOCKET clientSocket)
{
	for (int i = 0; i < this->clientVector.size(); i++)
	{
		if (this->clientVector[i].getSocket() == clientSocket) return this->clientVector[i];
	}
}

Client ClientList::getClient(std::string clientUsername)
{
	for (int i = 0; i < this->clientVector.size(); i++)
	{
		if (this->clientVector[i].getUsername() == clientUsername) return this->clientVector[i];
	}
}

