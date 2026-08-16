//#include "definitions.h"
#include "Server.h"

#include <iostream>

int main()
{
    const int SERVER_LISTEN_PORT = 31337;
    const int MAX_CONNECTIONS = 1;

    char* readBuffer = new char[256];

    //setup the server
    Server server;
    StatusCode result = server.init(SERVER_LISTEN_PORT, MAX_CONNECTIONS);
    if (result != StatusCode::SUCCESS)
    {
        std::cout << "Server init failed with code: " << (int)result << std::endl;
        server.stop();
        return (int)result;
    }

    std::cout << "Server Initialized on port " << SERVER_LISTEN_PORT << " with up to " << MAX_CONNECTIONS << " connections!" << std::endl;
    
    //accept an incoming connection
    result = server.acceptIncomingConnections();

    if (result != StatusCode::SUCCESS)
    {
        std::cout << "Server accept failed with code: " << (int)result << std::endl;
        
        return (int)result;
    }

    std::cout << "Server accepted incoming connections!" << std::endl;

    while (server.isActive())
    {
        //read a message
        std::cout << "Waiting for a message from the client..." << std::endl;
        result = server.readMessage(readBuffer);
        if (result != StatusCode::SUCCESS)
        {
            std::cout << "Server read failed with code: " << (int)result << std::endl;
            return (int)result;
        }
        std::cout << "[Recieved Message] " << readBuffer << std::endl;

        std::cout << "Send a message to the client" << std::endl;
        //send a message
        char* message = new char[256];
        std::cin.getline(message, 256);
        result = server.sendMessage(message, std::strlen(message));
        if (result != StatusCode::SUCCESS) return (int)result;
        std::cout << "[Sent] " << message << std::endl;

    }
    server.stop();
}

