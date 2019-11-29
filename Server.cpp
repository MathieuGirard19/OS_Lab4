#include "thread.h"
#include "socketserver.h"
#include <stdlib.h>
#include <time.h>
#include <list>
#include <vector>
#include <algorithm>
using namespace std;

using namespace Sync;
// This thread handles each client connection
class SocketThread : public Thread
{
private:
    // Reference to our connected socket
    Socket& socket;
    // The data we are receiving
    ByteArray data; //data transfered between client and server
    bool& exit; //used to stop while loop
public:
    SocketThread(Socket& socket, bool& exit)
    : socket(socket), exit(exit)
    {}
    //destructor
    ~SocketThread()
    {}

    Socket& GetSocket()
    {
        return socket;
    }

    virtual long ThreadMain()
    {
        while(!exit)
        {
            try
            {
                // Wait for data
                //between server and client, read what the client sent through the socket
                //reassign data, alter to prove that server accepted
                //Enter critical section, only a single client can read or write to this socket
                socket.Read(data);
                string data_received = data.ToString();
                data_received = data_received + " Server successfully received string";
                data = ByteArray(data_received);

                // Send it back
                socket.Write(data);
            }
            catch (string exception)
            {
                // ???
                cout<<exception<<endl;
            }
        }
		
	// ???

        return 0;
    }
};

// This thread handles the server operations
class ServerThread : public Thread
{
private:
    SocketServer& server;
    bool exit = false;
    vector<SocketThread*> socketThreads;
public:
    ServerThread(SocketServer& server)
    : server(server)
    {}

    ~ServerThread()
    {
        //get sockets for each sockethread and close them
        for(SocketThread* thread: socketThreads){
            try{
                thread->GetSocket().Close();
            }
            catch(string exception){
                cout<<exception<<endl;
            }
        }
        exit = true;
    }

    virtual long ThreadMain()
    {
        while(!exit){
            // Wait for a client socket connection, used to accept new connections
            Socket* newConnection = new Socket(server.Accept());

            // Pass a reference to this pointer into a new socket thread
            Socket& socketReference = *newConnection;
            SocketThread* socketThread = new SocketThread(socketReference, exit);
        }
    }
};


int main(void)
{
    std::cout << "I am a server." << std::endl;
	std::cout << "Press enter to terminate the server...";
    std::cout.flush();
	
    // Create our server
    SocketServer server(3000);    

    // Need a thread to perform server operations
    ServerThread serverThread(server);
	
    // This will wait for input to shutdown the server
    FlexWait cinWaiter(1, stdin);
    cinWaiter.Wait();
    std::cin.get();

    // Shut down and clean up the server
    server.Shutdown();

}
