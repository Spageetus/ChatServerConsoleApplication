//#include "definitions.h"
#include "Server.h"
#include "ClientHandler.h"

#include <iostream>

int main()
{
    int SERVER_LISTEN_PORT = 31337;
    int MAX_CONNECTIONS = 4;
    char COMMAND_CHARACTER = '/';

    //server console prompt for the listening port, max connections, and command character
    
    //std::cout << "Enter the port number you would like the server to listen to: ";
    //while (!(std::cin >> SERVER_LISTEN_PORT) || (SERVER_LISTEN_PORT < 0 || SERVER_LISTEN_PORT > 65535))
    //{
    //    std::cout << "Invalid port value. Values cannot be negative or exceed 65535" << std::endl;
    //    std::cout << "Enter the port number you would like the server to listen to: ";

    //    std::cin.clear();
    //    std::cin.ignore(INT_MAX, '\n');
    //}


    //std::cout << "Enter the max server capacity: ";
    //while (!(std::cin >> MAX_CONNECTIONS) || (MAX_CONNECTIONS < 0 || MAX_CONNECTIONS > 128)) //setting the max value to 128 for now
    //{
    //    std::cout << "Invalid input. Value must be a positive integer." << std::endl;
    //    std::cout << "Enter the max server capacity: ";

    //    std::cin.clear();
    //    std::cin.ignore(INT_MAX, '\n');
    //}
    //
    //do
    //{
    //    std::cout << "Enter the server's command character: ";
    //    std::cin >> COMMAND_CHARACTER;
    //} while (false); //TODO: change the condition to actually verify the value is valid

    std::cout << "Server Listening Port: " << SERVER_LISTEN_PORT << std::endl;
    std::cout << "Max Connections: " << MAX_CONNECTIONS << std::endl;
    std::cout << "Command Character: " << COMMAND_CHARACTER << std::endl;

    //if (true) return 0;

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