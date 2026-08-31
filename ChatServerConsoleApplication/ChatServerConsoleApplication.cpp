//#include "definitions.h"
#include <iostream>
#include "Server.h"
#include "ClientHandler.h"
#include "Credentials.h"
#include "Command.h"

//memory leak detection
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

//TODO: deal with memory leaks from creating new Clients (probably use shared pointers instead of normal pointers)

int main()
{
    //memory leak detection
    //_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

    int SERVER_LISTEN_PORT = 31337;
    int MAX_CONNECTIONS = 2;
    char COMMAND_CHARACTER = '/';

    //server console prompt for the listening port
    
    std::cout << "Enter the port number you would like the server to listen to: ";
    while (!(std::cin >> SERVER_LISTEN_PORT) || (SERVER_LISTEN_PORT < 1 || SERVER_LISTEN_PORT > 65535))
    {
        std::cout << "Invalid port value. Values cannot be negative or exceed 65535" << std::endl;
        std::cout << "Enter the port number you would like the server to listen to: ";

        std::cin.clear();
        std::cin.ignore(INT_MAX, '\n');
    }

    //prompt for max server capacity
    std::cout << "Enter the max server capacity: ";
    while (!(std::cin >> MAX_CONNECTIONS) || (MAX_CONNECTIONS < 1 || MAX_CONNECTIONS > 128)) //setting the max value to 128 for now
    {
        std::cout << "Invalid input. Value must be a positive integer." << std::endl;
        std::cout << "Enter the max server capacity: ";

        std::cin.clear();
        std::cin.ignore(INT_MAX, '\n');
    }
    
    //prompt for command character
    std::cout << "Enter the server's command character: ";
    while (std::cin >> COMMAND_CHARACTER || true)
    {
        //verify command character based on ASCII values
        if (COMMAND_CHARACTER >= '!' && COMMAND_CHARACTER < '0') break;
        if (COMMAND_CHARACTER >= ':' && COMMAND_CHARACTER < 'A') break;
        std::cout << "Invalid command character. Character cannot be alphanumeric or invisible" << std::endl;
        std::cout << "Enter the server's command character: ";

        std::cin.clear();
        std::cin.ignore(INT_MAX, '\n');
    }

    std::cout << "Server Listening Port: " << SERVER_LISTEN_PORT << std::endl;
    std::cout << "Max Connections: " << MAX_CONNECTIONS << std::endl;
    std::cout << "Command Character: " << COMMAND_CHARACTER << std::endl;

    Credentials::init();
    MessageParser::commandCharacter = COMMAND_CHARACTER;


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

    while (true)
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
    Figure out how to allow the server to be shutdown gracefully instead of just closing the program 
           - Use SIGINT from <csignal> to catch CTRL+C

    Make sure accounts that are signed in cannot register new accounts
    
    Add the ability to send messages that are LONGER than the 256 limit
*/

/*
*   ##### ASSIGNMENT TODOS #####
        1.1 
            - display the server host ip and port using gethostname() and getaddrinfo() to the server console when started
        1.2
            - display command character

*/

/*
    When sending messages: format as followed

    send as two separate messages, one detailing who sent the message, the second containing the untouched original message

    [Received]: <SOURCE CLIENT>
    [Received]: message

    

    
    //Reciever end
    [Received]: <SOURCE CLIENT> whispered:
    [Recieved]: message


*/