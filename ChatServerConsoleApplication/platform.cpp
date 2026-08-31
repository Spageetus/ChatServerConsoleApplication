#include "platform.h"


int sendTcpData(SOCKET skSocket, const char* data, uint16_t length)
{
	int result;
	int bytesSent = 0;

	while (bytesSent < length)
	{
		result = send(skSocket, (const char*)data + bytesSent, length - bytesSent, 0);

		if (result <= 0)
			return result;

		bytesSent += result;
	}

	return bytesSent;
}


int receiveTcpData(SOCKET sockfd, char* readBuffer, uint16_t length)
{
	int receivedBytes = 0;
	while (receivedBytes < length)
	{
		int result = recv(sockfd, readBuffer + receivedBytes, 1, 0);
		if (result == SOCKET_ERROR)
		{
			int errorCode = WSAGetLastError();
			//printf("Error occured while receiving data. Error code: %u", errorCode);
			return -1; //TODO: confirm if this is correct
		}
		else if (result == 0) return receivedBytes;
		else receivedBytes += result;
	}
	return receivedBytes;
}