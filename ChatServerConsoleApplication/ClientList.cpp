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

void ClientList::remove(Client clientToRemove)
{
	SOCKET s = clientToRemove.getSocket();
	for (size_t i = 0; i < this->clientVector.size(); ++i)
	{
		if (this->clientVector[i] == clientToRemove)
		{
			// remove socket from the fd_set
			FD_CLR(s, &this->masterList);
			this->clientVector.erase(this->clientVector.begin() + i);
			break;
		}
	}
}

bool ClientList::inList(Client clientToFind) 
{
	return FD_ISSET(clientToFind.getSocket(), &this->masterList);
};

void ClientList::clear()
{
	while (!this->clientVector.empty())
	{
		Client c = this->clientVector.front();
		FD_CLR(c.getSocket(), &this->masterList);
		this->clientVector.erase(this->clientVector.begin());
	}
}

void ClientList::shutdownAll()
{
	for (int i = 0; i < this->size(); i++)
	{
		Client c = this->clientVector[i];
		c.shutdownClient();
	}
	this->clear();
}

Client ClientList::getClient(SOCKET clientSocket)
{
	for (int i = 0; i < this->clientVector.size(); i++)
	{
		if (this->clientVector[i].getSocket() == clientSocket) return this->clientVector[i];
	}
	return Client::InvalidClient;
}

Client ClientList::getClient(std::string clientUsername)
{
	for (int i = 0; i < this->clientVector.size(); i++)
	{
		if (this->clientVector[i].getUsername() == clientUsername) return this->clientVector[i];
	}
	return Client::InvalidClient;
}

fd_set ClientList::getReadyReadSockets()
{
	timeval selectPauseTime;
	selectPauseTime.tv_sec = 1;

	fd_set temp = this->masterList;
	select(NULL, &temp, NULL, NULL, &selectPauseTime);
	return temp;
}

fd_set ClientList::getReadyWriteSockets()
{
	timeval selectPauseTime;
	selectPauseTime.tv_sec = 1;
	fd_set temp = this->masterList;
	select(NULL, NULL, &temp, NULL, &selectPauseTime);
	return temp;
}