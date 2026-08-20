//#include "definitions.h"
#include "Server.h"
#include "ClientHandler.h"

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

    server.getHostName();

    while (ClientHandler::numClients() < 5) //temporary code to auto shutdown server 
    {
        result = server.runOnce();
        if (result != StatusCode::SUCCESS)
        {
            std::cout << "server run returned code: " << (int)result << std::endl;
            break;
        }
    }

    server.stop();
}


/*
    ###### TODOS #####

    Make max connections actually function
    format relayed messages like so:
        <username>: message
                or
        [SERVER]: message

    Figure out how to allow the server to be shutdown gracefully instead of just closing the program 
           - Use SIGINT from <csignal> to catch CTRL+C

*/

/*
*   ##### ASSIGNMENT TODOS #####
        1.1 
            - on server startup, prompt user for TCP port number, chat capacity, and the command character
            - display the server host ip and port using gethostname() and getaddrinfo() to the server console when started
        1.2
            - display command character
        1.3
            - help command
        1.4
            - add user registration via the /register <username> <password> commmand (store details in a hashtable for uniqueness

*/