
#include "thread.h"
#include "socket.h"
#include <iostream>
#include <stdlib.h>
#include <time.h>
using namespace std;

using namespace Sync;

// This thread handles the connection to the server
class ClientThread : public Thread
{
private:
	// Reference to our connected socket
	Socket& socket;

	// Data to send to server
	ByteArray data;
	std::string data_str;
public:
	bool& exit;
	//tells us a connection has established, server has accepted
	//have own socket and socket thread for client and server pair
	bool connected=false;
	ClientThread(Socket& socket, bool& exit)
	: socket(socket), exit(exit)
	{}

	void AttemptConnect(){
		try{
			cout<<"Attempting to connect"<<endl;
			cout.flush(); //clean up data
			socket.Open();//try to connect to server via socket
			connected = true;
			cout<<"Connection Established"<<endl;
		}
		catch(string exception){
			cout<<exception<<endl;
		}
	}

	~ClientThread()
	{}

	virtual long ThreadMain()
	{
		try{
			int result = socket.Open();
		}
		catch(...){
			cout<<"Server is not running"<<endl;
			exit = true;
			return 0;
		}
		while(!exit){
			cout << "Please input your data (done to exit): ";
			cout.flush();

			// Get the data
			getline(std::cin, data_str);
			data = ByteArray(data_str);
			

			if(data_str.size() == 0){
				cout<<"No value entered"<<endl;
				continue;
			}
			else if(data_str == "exit"){
				exit = true;
				break;
			}

			// Write to the server
			socket.Write(data);

			// Get the response
			socket.Read(data);
			if(!data.ToString().size()){
				cout<<"Server is down"<<endl;
				exit = true;
				break;
			}
			data_str = data.ToString();
			cout<<"Server: "<<data_str<<endl;
		}	
	}
};

int main(void)
{
	// Welcome the user 
	cout << "SE3313 Lab 4 Client" << endl;
	bool exit = false;
	// Create our socket
	Socket socket("127.0.0.1", 3000);
	ClientThread clientThread(socket, exit);
	while(!exit)
	{
		sleep(1);
	}
	socket.Close();

	return 0;
}
