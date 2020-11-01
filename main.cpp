#include <iostream> // print text, etc.

// some historical (BSD) implementations required <sys/types.h> header file,  and  portable  applications
// are probably wise to include it.
// #include <sys/types.h>

#include <unistd.h> // unix standard

//this is socket library which is the most important one in this code
#include <sys/socket.h>

// netdb.h will be used by getnameinfo which gives us ip address of the connecting client
#include <netdb.h>

//definitions for internet operations: htons, ntohs, inet_ntop, inet_pton
#include <arpa/inet.h>

// defines one variable type, one macro, and various functions for manipulating arrays of characters.
// sting.h will be used for memset
#include <string.h>


#include <string> // Create a string variable

using namespace std;//using standard name space,e.g. cout, err, etc.

int main()
{
    // Create a socket to listen to incomming connection
    // [Note]
    // s = socket(family, type, protocol);
    // - AF_INET is IPv4 Internet protocols,
    // - SOCK_STREAM for TCP socket
    // - Set the protocol argument to 0, the default, in most situations.
    //   You can specify a protocol other than the default
    int listening = socket(AF_INET, SOCK_STREAM, 0);
    if (listening == -1) // -1 something bad happened
    {
        cerr << "Can't create a socket! Quitting" << endl;
        return -1;
    }

    // Bind the ip address and port to a socket
    // [Note] It is a convention to use 'sockaddr_in hint'
    sockaddr_in hint; //socket address version 4, using sockaddr_in6 for version 6

    hint.sin_family = AF_INET;//internet socket address family: IPv4
    // [Note] We can't assign port number as an int, since everything is store as Big-Endian
    // [Note] Big endian vs. little endian. Big-endian is an order in which the "big end" (most significant
    // value in the sequence) is stored first (at the lowest storage address). Little-endian is an order in
    // which the "little end" (least significant value in the sequence) is stored first.
    hint.sin_port = htons(54000);//socket input port = Host_To_Network_Short(5400), this function will flip bit order to Big-Endian
    // inet_pton: InterNET command - Pointer string TO Number: convert a string to an array of number
    inet_pton(AF_INET, "0.0.0.0", &hint.sin_addr);//0.0.0.0 means any address

    bind(listening, (sockaddr*)&hint, sizeof(hint));

    // Tell Winsock the socket is for listening
    // SOMAXCONN: Maxium number of socket connection - 128
    listen(listening, SOMAXCONN);

    // Wait for a connection
    sockaddr_in client;
    socklen_t clientSize = sizeof(client); //socklen_t: an unsigned opaque integral type of length of at least 32-bits.

    int clientSocket = accept(listening, (sockaddr*)&client, &clientSize); // open a new socket to send message to client

    // buffer for putting host and sever names
    char host[NI_MAXHOST];      // Client's remote name, maximum value for hostlen is NI_MAXHOST
    char service[NI_MAXSERV];   // Service (i.e. port) the client is connect on, maximum value for servlen is NI_MAXSERV

    memset(host, 0, NI_MAXHOST); // clear memory
    memset(service, 0, NI_MAXSERV); // clear memory

    if (getnameinfo((sockaddr*)&client, sizeof(client), host, NI_MAXHOST, service, NI_MAXSERV, 0) == 0)
    {
        cout << host << " connected on port " << service << endl;
    }
    else
    {
        //inet_ntop: Internet command - number to (string) pointer
        // convert an array of number to string
        inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
        // ntohs: Network to Host Short: convert to Little-Endian format
        cout << host << " connected on port " << ntohs(client.sin_port) << endl;
    }

    // Close listening socket after a host is connected
    close(listening);

    // While loop: accept and echo message back to client
    char buf[4096];

    while (true)
    {
        memset(buf, 0, 4096);

        // Wait for client to send data
        int bytesReceived = recv(clientSocket, buf, 4096, 0);
        if (bytesReceived == -1)
        {
            cerr << "Error in recv(). Quitting" << endl;
            break;
        }

        if (bytesReceived == 0)
        {
            cout << "Client disconnected " << endl;
            break;
        }

        cout << string(buf, 0, bytesReceived) << endl;

        // Echo message back to client
        send(clientSocket, buf, bytesReceived + 1, 0);
    }

    // Close the socket
    close(clientSocket);

    return 0;
}
